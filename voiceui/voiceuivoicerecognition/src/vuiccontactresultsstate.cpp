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
#include <CPbkViewState.h>
#include <PbkUID.h>
#include <StringLoader.h>

#include <vasmbasepbkhandler.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuicstate.h"
#include "vuiccontactresultsstate.h"
#include "vuicplaystate.h"
#include "vuicabortstate.h"
#include "vuicerrorstate.h"
#include "vuicexitstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuicnbestlistdialog.h"
#include "vuicttsplayer.h"
#include "vuivoiceicondefs.h"

#include "rubydebug.h"

// Constants
_LIT( KTab, "\t" );
_LIT( KStringPosition, "%U" );
    
// -----------------------------------------------------------------------------
// CContactResultsState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactResultsState* CContactResultsState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CContactResultsState* self = new (ELeave) CContactResultsState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CContactResultsState::~CContactResultsState()
    {
    RUBY_DEBUG0( "CContactResultsState::~CContactResultsState START" );
    
    DataStorage().TtsPlayer()->Stop();
    
    delete iNBestListDialog;
    
    delete iTagList; // Only copies of pointers, do not reset and destroy
    
    DataStorage().SetAdditionalTagList( NULL ); // Reset for next time
    
    delete iNssVasDbManager;
    iNssTagManager = NULL;
    
    RUBY_DEBUG0( "CContactResultsState::~CContactResultsState EXIT" );
    }
    
// ---------------------------------------------------------
// CContactResultsState::HandleEventL
// ---------------------------------------------------------
//       
void CContactResultsState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == ENotStarted )
                {
                nextState = this;
                }
            else if ( iInternalState == EStarted )
                {
                iInternalState = EInitialized;
                nextState = this;
                }
            else if ( iInternalState == EInitialized )
                {
                iInternalState = ECompleted;
                nextState = this;
                }
            break;
            
        case EVoiceTagSoftKeySelect:
        
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
        case EDragKeypress:
        case EScrollKeypress:
        
            iKeypress = aEvent;
            nextState = this;
            break;
            
        default:
            
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CContactResultsState::ExecuteL
// ---------------------------------------------------------
//       
void CContactResultsState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::ExecuteL" );

    if ( HandleKeypressL() )
        {
        iKeypress = ENoKeypress;
        }
    else if ( iInternalState == EInitialized )
        {
        // If automatic confirmation or device lock is on then start the timer
        if ( DataStorage().VerificationMode() == EAutomatic ||
             DataStorage().DeviceLockMode() )
            {
            TInt timeoutValue = DataStorage().PlayDuration().Int64();
            if ( timeoutValue < KTimeoutMicroseconds )
                {
                timeoutValue = KTimeoutMicroseconds;
                }
        
            StartTimerL( *this, timeoutValue, timeoutValue );
            }
        }
    else if ( iInternalState == ECompleted )
        {
        DataStorage().TtsPlayer()->Stop();
        }
    else
        {
        if ( !DataStorage().AdditionalTagList() )
            {
            if ( iNssTagManager->GetTagList( DataStorage().VoiceRecognitionImpl(),
                                             DataStorage().Tag()->Context(),
                                             DataStorage().Tag()->RRD()->IntArray()->At( KVasContactIdRrdLocation ),
                                             KVasContactIdRrdLocation ) != KErrNone )
                {
                HandleEventL( KErrGeneral );
                }
            }
        else
            {
            iInternalState = EStarted;
            
            TInt returnValue = KErrNoResults;

            // Get the name for the n-best list title
            HBufC* name = DataStorage().Tag()->SpeechItem()->PartialTextL( KNameTrainingIndex );
            CleanupStack::PushL( name );
            
            CreateContactInfoArrayL();
            
            if ( iTagList->Count() != KErrNone )
                {
                iNBestListDialog = CNBestListDialog::NewL();
                iNBestListDialog->CreateNBestListPopupLC( CreateItemsArrayL(), *name );
    
                PlaySelectedNameL();
                BringToForeground();
    
                iNBestListDialog->RegisterForKeyCallback( DataStorage().VoiceRecognitionImpl() );            
                returnValue = iNBestListDialog->ShowNBestListPopupL(); // Pops things pushed in CreateNBestListPopupLC
                }
            CleanupStack::PopAndDestroy( name );
            
            if ( returnValue != KErrNone )
                {
                iInternalState = ECompleted;
                
                SelectTag();
                
                HandleEventL( returnValue );
                }
            }
        }
    }

// ---------------------------------------------------------
// CContactResultsState::CContactResultsState
// ---------------------------------------------------------
//              
CContactResultsState::CContactResultsState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iKeypress( ENoKeypress ),
   iInternalState( ENotStarted )
    {
    }

// ---------------------------------------------------------
// CContactResultsState::ConstructL
// ---------------------------------------------------------
//           
void CContactResultsState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::ConstructL" );
    
    CState::ConstructL();
    
    iNssVasDbManager = CNssVASDBMgr::NewL();
    iNssVasDbManager->InitializeL();
    iNssTagManager = iNssVasDbManager->GetTagMgr();
   
    DataStorage().SetVerificationMode();
    }

// ---------------------------------------------------------
// CContactResultsState::DoTimedEventL
// ---------------------------------------------------------
//
void CContactResultsState::DoTimedEventL()
   {   
   SelectTag();
   
   HandleEventL( EVoiceTagSoftKeySelect );
   }

// ---------------------------------------------------------
// CContactResultsState::HandleKeypressL
// ---------------------------------------------------------
//           
TBool CContactResultsState::HandleKeypressL()
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::HandleKeypressL" );
    
    TBool handled = ETrue;
    
    if ( iKeypress == EShortKeypress )
        {       
        if ( iNBestListDialog->HandleShortKeypressL( NULL ) )
            {
            iInternalState = EStarted;
            
            PlaySelectedNameL();
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
        
        PlaySelectedNameL();
        }
    else if ( iKeypress == EScrollKeypress )
        {
        StopTimer();
        }
    else if ( iKeypress == ESelectKeypress || iKeypress == EDirectSelectKeypress )
        {        
        if ( !DataStorage().Tag() )
            {
            SelectTag();
    
            HandleEventL( EVoiceTagSoftKeySelect );
            }
        }
    else
        {
        handled = EFalse;
        }
    
    return handled;
    }
    
// ---------------------------------------------------------------------------
// CContactResultsState::PlaySelectedNameL
// ---------------------------------------------------------------------------
//
void CContactResultsState::PlaySelectedNameL()
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::PlaySelectedNameL" );

    // if TTS is used
    if ( ( DataStorage().SynthesizerMode() != KErrNone ||
         DataStorage().DeviceLockMode() ) &&
         iTagList->Count() > 0 )
        {
        TInt index = iNBestListDialog->SelectedIndex();        
        MNssTag* tag = iTagList->At( index );
            
        if ( tag->Context()->ContextName() == KVoiceDialContext &&
             tag->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial )
            {
            HBufC* ttsText = StringLoader::LoadLC( R_QAN_VC_TTS_DIALLING );
            
            TInt position = ttsText->FindC( KStringPosition );
            User::LeaveIfError( position );            
            
            CleanupStack::PopAndDestroy( ttsText );
            
            ttsText = StringLoader::LoadLC( R_QAN_VC_TTS_DIALLING,
                                            tag->SpeechItem()->Text() );
                                                   
            HBufC* text = tag->SpeechItem()->PartialTextL( KNameTrainingIndex );        
            CleanupStack::PushL( text ); 
            
            DataStorage().TtsPlayer()->PlayL( *ttsText, position,
                                              tag->SpeechItem()->Text().Length() , *text );
            
            CleanupStack::PopAndDestroy( text );
            CleanupStack::PopAndDestroy( ttsText );
            }
        else if ( tag->Context()->ContextName() == KVoiceDialContext )
            {           
            HBufC* text = tag->SpeechItem()->PartialTextL( KNameTrainingIndex );        
            CleanupStack::PushL( text ); 
        
            DataStorage().TtsPlayer()->PlayL( tag->SpeechItem()->Text(), 0,
                                              tag->SpeechItem()->Text().Length(), *text );

            CleanupStack::PopAndDestroy( text );
            }
        }
    else
        {
        DataStorage().SetPlayDuration( KTimeoutMicroseconds );
        HandleEventL( KErrNone );
        }
    }
    
// ---------------------------------------------------------------------------
// CContactResultsState::CreateContactInfoArrayL
// ---------------------------------------------------------------------------
//
void CContactResultsState::CreateContactInfoArrayL()
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::CreateContactInfoArrayL" );
    
    CArrayPtrFlat<MNssTag>* completeTagList = DataStorage().CompleteTagList();
    CArrayPtrFlat<MNssTag>* additionalTags = DataStorage().AdditionalTagList();
    
    iTagList = new ( ELeave ) CArrayPtrFlat<MNssTag>( KSindMaxResults );
    
    iTagList->AppendL( DataStorage().Tag() );
    
    // Add all commands from selected contact to taglist
    for ( TInt i = 0; i < completeTagList->Count(); ++i )
        {
        MNssTag* tag = completeTagList->At( i );
        
        if ( tag->Context()->ContextName() == KVoiceDialContext &&
             ( !DataStorage().DeviceLockMode() || tag->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial ) &&
             tag->RRD()->IntArray()->At( KVasContactIdRrdLocation ) ==
             DataStorage().Tag()->RRD()->IntArray()->At( KVasContactIdRrdLocation ) &&
             CompareContactNamesL( tag ) )
            {
            // Add tag to list
            iTagList->AppendL( tag );
            }
        }
    
    // Add all additional commands from selected contact to taglist if not added already    
    for ( TInt i = 0; i < additionalTags->Count(); ++i )
        {
        MNssTag* tag = additionalTags->At( i );
        
        if ( !CheckIfAlreadyAdded( tag ) && CompareContactNamesL( tag ) &&
             ( !DataStorage().DeviceLockMode() || tag->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial ) )
            {
            // Add tag to list
            iTagList->AppendL( tag );
            }
        }
        
    // Add selected tag back to tag list
    completeTagList->AppendL( DataStorage().Tag() );
    
    // Clear selected tag    
    DataStorage().SetTag( NULL, EFalse );
    }

// ---------------------------------------------------------
// CContactResultsState::CompareContactNamesL
// ---------------------------------------------------------
//     
TBool CContactResultsState::CompareContactNamesL( MNssTag* aTag )
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::CompareContactNamesL" );
    
    TBool returnValue = EFalse;
    
    HBufC* firstName = DataStorage().Tag()->SpeechItem()->PartialTextL( KNameTrainingIndex );
    CleanupStack::PushL( firstName );
    
    HBufC* secondName = aTag->SpeechItem()->PartialTextL( KNameTrainingIndex );
    CleanupStack::PushL( secondName );
    
    returnValue = ( *firstName == *secondName );
        
    CleanupStack::PopAndDestroy( secondName );
    CleanupStack::PopAndDestroy( firstName );
    
    return returnValue;
    }

// ---------------------------------------------------------
// CContactResultsState::CheckIfAlreadyAdded
// ---------------------------------------------------------
//    
TBool CContactResultsState::CheckIfAlreadyAdded( MNssTag* aTag )
    {
    RUBY_DEBUG0( "CContactResultsState::CheckIfAlreadyAdded START" );
    
    TBool returnValue = EFalse;

    for ( TInt i = 0; i < iTagList->Count(); ++i )
        {
        if ( aTag->SpeechItem()->RawText() ==
             iTagList->At( i )->SpeechItem()->RawText() ||
             aTag->SpeechItem()->RawText() ==
             DataStorage().Tag()->SpeechItem()->RawText() )
            {
            returnValue = ETrue;
            
            break;
            }
        }
    
    RUBY_DEBUG0( "CContactResultsState::CheckIfAlreadyAdded EXIT" );
    
    return returnValue;
    }
    
// ---------------------------------------------------------------------------
// CContactResultsState::CreateItemsArrayLC
// ---------------------------------------------------------------------------
//    
CDesC16ArrayFlat* CContactResultsState::CreateItemsArrayL()
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::CreateItemsArrayL" );
        
    TInt count = iTagList->Count();
           
    CDesC16ArrayFlat* items = new ( ELeave ) CDesC16ArrayFlat( KSindMaxResults );
    CleanupStack::PushL( items );

    TName buffer;
    
    for ( TInt i = 0; i < count; ++i )
        {
        MNssTag* tag = iTagList->At( i );        
        
        FillDialContextBufferL( tag, buffer );
            
        items->AppendL( buffer );
        }
        
    CleanupStack::Pop( items );
    
    return items;
    }

// ---------------------------------------------------------
// CContactResultsState::FillDialContextBufferL
// ---------------------------------------------------------
//
void CContactResultsState::FillDialContextBufferL( MNssTag* aTag, TDes& aBuffer )
    {
    RUBY_DEBUG_BLOCK( "CContactResultsState::FillDialContextBufferL" );
    
    TPtrC type = KNullDesC();
    
    DataStorage().PbkHandler()->FindContactFieldL( aTag );
                           
    TBufC<KMaxFieldLength> phoneNumber = DataStorage().PbkHandler()->TextL();
    type.Set( GetContactIconType( DataStorage().PbkHandler()->FieldTypeL() ) );

    aBuffer.Zero();
    aBuffer.Append( type );
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
        
    aBuffer.Append( KTab );
    aBuffer.Append( phoneNumber );
    }
    
// ---------------------------------------------------------
// CContactResultsState::GetContactIconType
// ---------------------------------------------------------
//
TPtrC CContactResultsState::GetContactIconType( TFieldType aFieldType )
    {
    RUBY_DEBUG0( "CContactResultsState::GetContactIconType START" );
    
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

    RUBY_DEBUG0( "CContactResultsState::GetContactIconType EXIT" );
    
    return type;
    }
    
// ---------------------------------------------------------
// CContactResultsState::SelectTag
// ---------------------------------------------------------
//   
void CContactResultsState::SelectTag()
    {
    RUBY_DEBUG0( "CContactResultsState::SelectTag START" );
    
    if ( iTagList->Count() > 0 )
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
            
        // Remove selected item from the additional taglist
        for ( TInt i = 0; i < DataStorage().AdditionalTagList()->Count(); ++i )
            {
            if ( DataStorage().AdditionalTagList()->At( i ) ==
                 DataStorage().Tag() )
                {
                // Disable adaptation since tag is not from recognition results
                DataStorage().SetAdaptationEnabled( EFalse );
                
                DataStorage().AdditionalTagList()->Delete( i );
                break;
                }
            }
        }
        
    RUBY_DEBUG0( "CContactResultsState::SelectTag EXIT" );
    }

// End of File

