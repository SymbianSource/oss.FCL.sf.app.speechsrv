/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


// INCLUDE FILES
#include <ccafactory.h>
#include <mccaparameter.h>
#include <mccaconnection.h>

#include <vasmbasepbkhandler.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuicstate.h"
#include "vuicresultsstate.h"
#include "vuicplaystate.h"
#include "vuiccontactresultsstate.h"
#include "vuicabortstate.h"
#include "vuicerrorstate.h"
#include "vuicexitstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuicnbestlistdialog.h"
#include "vuicttsplayer.h"
#include "vuivoiceicondefs.h"

#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactIdConverter.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>

#include "rubydebug.h"

_LIT( KTab, "\t" );
    
// -----------------------------------------------------------------------------
// CResultsState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CResultsState* CResultsState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CResultsState* self = new (ELeave) CResultsState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CResultsState::~CResultsState()
    {
    RUBY_DEBUG0( "CResultsState::~CResultsState START" );
    
    DataStorage().TtsPlayer()->Stop();
    
    delete iNBestListDialog;
    
    delete iTagList; // Only copies of pointers, do not reset and destroy
    
    if ( iConnection )
        {
        iConnection->Close();
        }
    
    RUBY_DEBUG0( "CResultsState::~CResultsState EXIT" );
    }
    
// ---------------------------------------------------------
// CResultsState::HandleEventL
// ---------------------------------------------------------
//       
void CResultsState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CResultsState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == EStarted )
                {
                iInternalState = EInitialized;
                }
            else if ( iInternalState == EInitialized )
                {
                iInternalState = ECompleted;
                nextState = this;
                }                
            break;
                    
        case EVoiceTagSoftKeySelect:
            
            if ( DataStorage().Tag()->Context()->ContextName() == KVoiceDialContext )
                {
                nextState = CContactResultsState::NewL( DataStorage(), UiModel() );
                }
            else
                {
                nextState = CPlayState::NewL( DataStorage(), UiModel() );
                }
            break;
            
        case EVoiceTagSoftKeyDirectSelect:

            nextState = CPlayState::NewL( DataStorage(), UiModel() );
            break;
        
        case EAknSoftkeyQuit:
        case EAknSoftkeyNo:
        case EVoiceTagSoftKeyQuit:
        case EEndCallKeypress:
        case ELongKeypress:

            nextState = CAbortState::NewL( DataStorage(), UiModel() );
            break;
        
        case EShortKeypress:
        case EUpKeypress:
        case EDownKeypress:
        case ESelectKeypress:
        case EDirectSelectKeypress:
        case EOptionsKeypress:
        case EOpenKeypress:
        case EDragKeypress:
        case EScrollKeypress:
        
            iKeypress = aEvent;
            nextState = this;
            break;
        
        case EAknSoftkeyOpen:
        
            nextState = CExitState::NewL( DataStorage(), UiModel() );
            break;
            
        default:
            
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CResultsState::ExecuteL
// ---------------------------------------------------------
//       
void CResultsState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CResultsState::ExecuteL" );

    if ( HandleKeypressL() )
        {
        iKeypress = ENoKeypress;
        }
    else if ( iInternalState == ECompleted )
        {
        DataStorage().TtsPlayer()->Stop();
        }
    else
        {
        TInt returnValue = KErrNoResults;
        
        if ( DataStorage().CompleteTagList()->Count() != KErrNone )
            {
            iInternalState = EStarted;
            
            CreateContactArrayL();
            
            if ( iTagList->Count() != KErrNone )
                {
                iNBestListDialog = CNBestListDialog::NewL();
                
                CDesC16ArrayFlat* items = NULL;
                TRAPD( error, items = CreateItemsArrayL() );
                
                if ( !error )
                    {
                    iNBestListDialog->CreateNBestListPopupLC( items );
                    iNBestListDialog->SetSoftkeysL( R_SOFTKEYS_OPTIONS_QUIT__SELECT );
                
                    CheckOptionsCommands();
                    PlaySelectedNameL();
                    BringToForeground();
                    
                    // If automatic confirmation or device lock is on then start the timer
                    if ( DataStorage().VerificationMode() == EAutomatic ||
                         DataStorage().DeviceLockMode() )
                        {
                        StartTimerL( *this, KTimeoutMicroseconds, KTimeoutMicroseconds );
                        }
    
                    iNBestListDialog->RegisterForKeyCallback( DataStorage().VoiceRecognitionImpl() );        
                    returnValue = iNBestListDialog->ShowNBestListPopupL(); // Pops things pushed in CreateNBestListPopupLC
                    RUBY_DEBUG1( "CResultsState::ExecuteL - N-Best list returns [%d]", returnValue );
                    }
                else if ( error != KErrNotFound )
                    {
                    User::Leave( error );
                    }
                }
            }
        
        if ( returnValue != KErrNone )
            {
            iInternalState = ECompleted;
            
            SelectTag();
            
            HandleEventL( returnValue );
            }
        }
    }

// ---------------------------------------------------------
// CResultsState::CResultsState
// ---------------------------------------------------------
//     
void CResultsState::CCASimpleNotifyL( TNotifyType /*aType*/, TInt /*aReason*/ )
    {
    RUBY_DEBUG_BLOCK( "CResultsState::CCASimpleNotifyL" );
        
    HandleEventL( EAknSoftkeyOpen );
    }

// ---------------------------------------------------------
// CResultsState::CResultsState
// ---------------------------------------------------------
//              
CResultsState::CResultsState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iKeypress( ENoKeypress ),
   iInternalState( ENotStarted )
    {
    }

// ---------------------------------------------------------
// CResultsState::ConstructL
// ---------------------------------------------------------
//           
void CResultsState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CResultsState::ConstructL" );
    
    CState::ConstructL();
    
    DataStorage().SetAdaptationEnabled( ETrue );
    }
    
// ---------------------------------------------------------
// CResultsState::DoTimedEventL
// ---------------------------------------------------------
//
void CResultsState::DoTimedEventL()
   {
   RUBY_DEBUG_BLOCK( "CResultsState::DoTimedEventL" );
   
   SelectTag();
   
   HandleEventL( EVoiceTagSoftKeySelect );
   }
    
// ---------------------------------------------------------
// CResultsState::HandleKeypressL
// ---------------------------------------------------------
//           
TBool CResultsState::HandleKeypressL()
    {
    RUBY_DEBUG_BLOCK( "CResultsState::HandleKeypressL" );
    
    TBool handled = ETrue;
    
    if ( iKeypress == EShortKeypress )
        {
        CDesC16ArrayFlat* items = NULL;
        
        if ( UpdateContactArray() )
            {
            items = CreateItemsArrayL();
            }
        
        if ( iNBestListDialog->HandleShortKeypressL( items ) )
            {
            iInternalState = EStarted;
        
            CheckOptionsCommands();
            PlaySelectedNameL();
            
            // If automatic confirmation or device lock is on then start the timer
            if ( DataStorage().VerificationMode() == EAutomatic ||
                 DataStorage().DeviceLockMode() )
                {
                StartTimerL( *this, KTimeoutMicroseconds, KTimeoutMicroseconds );
                }
            }
        else
            {
            HandleEventL( KErrNoResults );
            }
        }
    else if ( iKeypress == EUpKeypress || iKeypress == EDownKeypress ||
              iKeypress == EDragKeypress )
        {
        iInternalState = EStarted;
        
        CheckOptionsCommands();
        PlaySelectedNameL();
        
        // If automatic confirmation or device lock is on then start the timer
        if ( DataStorage().VerificationMode() == EAutomatic ||
                DataStorage().DeviceLockMode() )
            {
            StartTimerL( *this, KTimeoutMicroseconds, KTimeoutMicroseconds );
            }
        }
    else if ( iKeypress == EScrollKeypress )
        {
        StopTimer();
        }
    else if ( iKeypress == ESelectKeypress )
        {        
        if ( !DataStorage().Tag() )
            {
            SelectTag();
    
            HandleEventL( EVoiceTagSoftKeySelect );
            }
        }
    else if ( iKeypress == EDirectSelectKeypress )
        {        
        if ( !DataStorage().Tag() )
            {
            SelectTag();
    
            HandleEventL( EVoiceTagSoftKeyDirectSelect );
            }
        }
    else if ( iKeypress == EOptionsKeypress )
        {        
        StopTimer();
        DataStorage().SetVerificationMode( EManual );
        }
    else if ( iKeypress == EOpenKeypress )
        {       
        OpenPhonebookContactL();
        }
    else
        {
        handled = EFalse;
        }
    
    return handled;
    }
    
// ---------------------------------------------------------------------------
// CResultsState::PlaySelectedNameL
// ---------------------------------------------------------------------------
//
void CResultsState::PlaySelectedNameL()
    {
    RUBY_DEBUG_BLOCK( "CResultsState::PlaySelectedNameL" );

    // if TTS is used
    if ( ( DataStorage().SynthesizerMode() != KErrNone ||
         DataStorage().DeviceLockMode() ) &&
         iTagList->Count() > 0 )
        {
        TInt index = iNBestListDialog->SelectedIndex();
        HBufC* text = NULL;
                    
        if ( iTagList->At( index )->Context()->ContextName() == KVoiceDialContext )
            {
            text = iTagList->At( index )->SpeechItem()->PartialTextL( KNameTrainingIndex );
            }
        else
            {
            text = HBufC::NewL( 0 );
            }

        CleanupStack::PushL( text );        
                
        DataStorage().TtsPlayer()->PlayL( iTagList->At( index )->SpeechItem()->Text(), 0,
                                          iTagList->At( index )->SpeechItem()->Text().Length(),
                                          *text );

        CleanupStack::PopAndDestroy( text );
        }
    }
    
// ---------------------------------------------------------------------------
// CResultsState::CheckOptionsCommands
// ---------------------------------------------------------------------------
//
void CResultsState::CheckOptionsCommands()
    {
    RUBY_DEBUG0( "CResultsState::CheckOptionsCommands START" );

    if ( iTagList->Count() > 0 )
        {
        TBool hidden = ETrue;
        
        TInt index = iNBestListDialog->SelectedIndex();
        if ( iTagList->At( index )->Context()->ContextName() == KVoiceDialContext )
            {
            hidden = EFalse;
            }
        iNBestListDialog->SetCommandsHidden( hidden );
        }
    RUBY_DEBUG0( "CResultsState::CheckOptionsCommands EXIT" );
    }
    
// ---------------------------------------------------------------------------
// CResultsState::CreateContactArrayL
// ---------------------------------------------------------------------------
//
void CResultsState::CreateContactArrayL()
    {
    RUBY_DEBUG_BLOCK( "CResultsState::CreateContactArrayL" );
    
    CArrayPtrFlat<MNssTag>* completeTagList = DataStorage().CompleteTagList();
    
    iTagList = new ( ELeave ) CArrayPtrFlat<MNssTag>( KSindMaxResults );
    
    // Add commands to taglist
    for ( TInt i = 0; i < completeTagList->Count() && iTagList->Count() < KSindMaxResults; ++i )
        {
        MNssTag* tag = completeTagList->At( i );
        
        if ( tag->Context()->ContextName() == KVoiceDialContext &&
             ( !DataStorage().DeviceLockMode() || tag->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial ) )
            {   
            if ( !CheckIfAlreadyAdded( tag ) )
                {
                iTagList->AppendL( tag );            
                }
            }
        else if ( !DataStorage().DeviceLockMode() )
            {
            iTagList->AppendL( tag );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CResultsState::UpdateContactArray
// ---------------------------------------------------------------------------
//
TBool CResultsState::UpdateContactArray()
    {
    RUBY_DEBUG0( "CResultsState::UpdateContactArray START" );
    
    MNssTag* tag = iTagList->At( iNBestListDialog->SelectedIndex() );
        
    TBool currentTagFound = EFalse;
    TBool updated = EFalse;
                        
    if ( tag->Context()->ContextName() == KVoiceDialContext )
        {  
        for ( TInt i = 0; i < DataStorage().CompleteTagList()->Count(); ++i )
            {
            MNssTag* newTag = DataStorage().CompleteTagList()->At( i );
            
            if ( newTag->Context()->ContextName() == KVoiceDialContext &&
                 ( !DataStorage().DeviceLockMode() || newTag->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial ) &&
                 tag->RRD()->IntArray()->At( KVasContactIdRrdLocation ) ==
                 newTag->RRD()->IntArray()->At( KVasContactIdRrdLocation ) )
                {
                if ( currentTagFound )
                    {
                    iTagList->At( iNBestListDialog->SelectedIndex() ) = newTag;
                    updated = ETrue;
                    break;
                    }   
                else if ( tag->SpeechItem()->RawText() == newTag->SpeechItem()->RawText() )
                    {
                    currentTagFound = ETrue;
                    }
                }
            }
        }
        
    RUBY_DEBUG0( "CResultsState::UpdateContactArray EXIT" );
        
    return updated;
    }
    
// ---------------------------------------------------------
// CResultsState::CheckIfAlreadyAdded
// ---------------------------------------------------------
//    
TBool CResultsState::CheckIfAlreadyAdded( MNssTag* aTag )
    {
    RUBY_DEBUG0( "CResultsState::CheckIfAlreadyAdded START" );
    
    TBool returnValue = EFalse;

    for ( TInt i = 0; i < iTagList->Count(); ++i )
        {
        if ( aTag->RRD()->IntArray()->At( KVasContactIdRrdLocation ) ==
             iTagList->At( i )->RRD()->IntArray()->At( KVasContactIdRrdLocation ) )
            {
            returnValue = ETrue;
            
            break;
            }
        }
    
    RUBY_DEBUG0( "CResultsState::CheckIfAlreadyAdded EXIT" );
    
    return returnValue;
    }
    
// ---------------------------------------------------------------------------
// CResultsState::CreateItemsArrayLC
// ---------------------------------------------------------------------------
//    
CDesC16ArrayFlat* CResultsState::CreateItemsArrayL()
    {
    RUBY_DEBUG_BLOCK( "CResultsState::CreateItemsArrayL" );
    
    TInt count = iTagList->Count();
            
    CDesC16ArrayFlat* items = new ( ELeave ) CDesC16ArrayFlat( KSindMaxResults );
    CleanupStack::PushL( items );

    TName buffer;
    
    for ( TInt i = 0; i < count; ++i )
        {
        MNssTag* tag = iTagList->At( i );
        
        if ( tag->Context()->ContextName() == KVoiceDialContext )
            {            
            FillDialContextBufferL( tag, buffer );
            
            items->AppendL( buffer );
            }
        else
            {                      
            FillCommandContextBuffer( tag, buffer );
            
            items->AppendL( buffer );
            }
        }
        
    CleanupStack::Pop( items );
    
    return items;
    }

// ---------------------------------------------------------
// CResultsState::FillDialContextBufferL
// ---------------------------------------------------------
//
void CResultsState::FillDialContextBufferL( MNssTag* aTag, TDes& aBuffer )
    {
    RUBY_DEBUG_BLOCK( "CResultsState::FillDialContextBufferL" );
    
    TPtrC type = KNullDesC();
    
    DataStorage().PbkHandler()->FindContactFieldL( aTag );
        
    type.Set( GetContactIconType( DataStorage().PbkHandler()->FieldTypeL() ) );

    // Get the actual name only        
    HBufC* name = aTag->SpeechItem()->PartialTextL( KNameTrainingIndex );
    CleanupStack::PushL( name );

    aBuffer.Zero();
    aBuffer.Append( type );
    aBuffer.Append( KTab );
    aBuffer.Append( *name );
    aBuffer.Append( KTab );
    
    HBufC* label = NULL;
        
    TRAPD( error, label = aTag->SpeechItem()->PartialTextL( KExtensionTrainingIndex ) );
    if ( error == KErrNone || error == KErrNotFound )
        {
        // If there is no extension then the phonebook label is shown
        if ( label )
            {
            CleanupStack::PushL( label );
    
            aBuffer.Append( *label );
    
            CleanupStack::PopAndDestroy( label );
            }
        else
            {
            aBuffer.Append( DataStorage().PbkHandler()->LabelL() );
            }
        }
    else
        {
        User::Leave( error );
        }

    CleanupStack::PopAndDestroy( name );
    }

// ---------------------------------------------------------
// CResultsState::FillCommandContextBuffer
// ---------------------------------------------------------
//    
void CResultsState::FillCommandContextBuffer( MNssTag* aTag, TDes& aBuffer )
    {
    RUBY_DEBUG0( "CResultsState::FillCommandContextBuffer START" );
    
    aBuffer.Zero();
    
    if ( aTag->RRD()->IntArray()->At( KVasContactIdRrdLocation ) >= KProfileValue )
        {
        aBuffer.Append( KIconProfile );
        }
    else
        {
        aBuffer.Append( KIconApplication );
        }
            
    aBuffer.Append( KTab );
    aBuffer.Append( aTag->SpeechItem()->Text() );
    
    RUBY_DEBUG0( "CResultsState::FillCommandContextBuffer EXIT" );
    }
        
// ---------------------------------------------------------
// CResultsState::GetContactIconType
// ---------------------------------------------------------
//
TPtrC CResultsState::GetContactIconType( TFieldType aFieldType )
    {
    RUBY_DEBUG0( "CResultsState::GetContactIconType START" );
    
    TPtrC type;
    // select a phone type
    if ( aFieldType == KUidContactFieldVCardMapVOICE )
        {
        type.Set( KIconPhone );
        }
    else if ( aFieldType == KUidContactFieldVCardMapCELL )
        {
        type.Set( KIconMobile );
        }
    else if ( aFieldType == KUidContactFieldVCardMapFAX )
        {
        type.Set( KIconFax );
        }
    else if ( aFieldType == KUidContactFieldVCardMapEMAILINTERNET )
        {
        type.Set( KIconEmail );
        }
    else if ( aFieldType == KUidContactFieldVCardMapVIDEO )
        {
        type.Set( KIconVideo );
        }
    else if ( aFieldType == KUidContactFieldVCardMapVOIP )
        {
        type.Set( KIconVoip );
        }
    else
        {
        type.Set( KIconBlank );
        }

    RUBY_DEBUG0( "CResultsState::GetContactIconType EXIT" );
    
    return type;
    }
    
// ---------------------------------------------------------
// CResultsState::SelectTag
// ---------------------------------------------------------
//   
void CResultsState::SelectTag()
    {
    RUBY_DEBUG0( "CResultsState::SelectTag START" );
    
    if ( iTagList && iTagList->Count() > 0 )
        {
        DataStorage().SetTag( iTagList->At( iNBestListDialog->SelectedIndex() ), ETrue );
       
        // Remove selected item from the complete taglist
        for ( TInt i = 0; i < DataStorage().CompleteTagList()->Count(); ++i )
            {
            if ( DataStorage().CompleteTagList()->At( i ) ==
                 DataStorage().Tag() )
                {
                DataStorage().CompleteTagList()->Delete( i );
                break;
                }
            }
        }
        
    RUBY_DEBUG0( "CResultsState::SelectTag EXIT" );
    }

// ---------------------------------------------------------
// CResultsState::OpenPhonebookContactL
// ---------------------------------------------------------
//      
void CResultsState::OpenPhonebookContactL()
    {
    RUBY_DEBUG_BLOCK( "CResultsState::OpenPhonebookContactL" );
    
    SelectTag();
    
    if ( iConnection )
        {
        iConnection->Close();
        }
    iConnection = TCCAFactory::NewConnectionL();

    MCCAParameter* parameter = TCCAFactory::NewParameterL();
    CleanupClosePushL( *parameter );

    parameter->SetConnectionFlag( MCCAParameter::ENormal );
    parameter->SetContactDataFlag( MCCAParameter::EContactLink );
        
    // Create a Contact Manager and bind it with the default contact database 
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewL();
    CleanupStack::PushL( uriArray );
    
    TVPbkContactStoreUriPtr uri( VPbkContactStoreUris::DefaultCntDbUri() );
    uriArray->AppendL( uri );  
    
    CVPbkContactManager* contactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PushL( contactManager );
    
    // Find Contacts Model store URI from the contact manager (copied from Phonebook app)
    MVPbkContactStore* defaultStore = contactManager->ContactStoresL().Find( uri );
    User::LeaveIfNull( defaultStore );
    
    // Create a ContactIdConverter object for the ContactId-to-ContactLink conversion 
    CVPbkContactIdConverter* idConverter = CVPbkContactIdConverter::NewL( *defaultStore );
    CleanupStack::PushL( idConverter );
    
    MVPbkContactLink* link = idConverter->IdentifierToLinkLC(
            DataStorage().Tag()->RRD()->IntArray()->At( KVasContactIdRrdLocation ));
    
    HBufC8*  link8  = link->PackLC(); 
    HBufC16* link16 = HBufC16::NewLC( link8->Length() );
    link16->Des().Copy( *link8 );

    parameter->SetContactDataL( link16->Des() );

    iConnection->LaunchAppL( *parameter, this );
    
    CleanupStack::PopAndDestroy(6,uriArray);    
    CleanupStack::Pop();//parameter
    }
    
// End of File

