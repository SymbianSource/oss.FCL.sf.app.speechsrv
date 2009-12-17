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
#include <avkon.hrh>
#include <StringLoader.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuicstate.h"
#include "vuicverificationstate.h"
#include "vuicverificationinitstate.h"
#include "vuicplaystate.h"
#include "vuicabortstate.h"
#include "vuicerrorstate.h"
#include "vuicexitstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuicverificationdialog.h"
#include "vuicttsplayer.h"
#include "vuictoneplayer.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CVerificationState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVerificationState* CVerificationState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CVerificationState* self = new (ELeave) CVerificationState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CVerificationState::~CVerificationState()
    {
    RUBY_DEBUG0( "CVerificationState::~CVerificationState START" );
    
    if ( DataStorage().TtsPlayer() )
        {
        DataStorage().TtsPlayer()->Stop();
        }
    
    // If recording has not ended we should cancel recording
    if ( iInternalState != ECompleted &&
         DataStorage().VerificationRecognitionHandler() )
        {
        DataStorage().VerificationRecognitionHandler()->Cancel();
        }
        
    delete iVerificationDialog;
    
    RUBY_DEBUG0( "CVerificationState::~CVerificationState EXIT" );
    }
    
// ---------------------------------------------------------
// CVerificationState::HandleEventL
// ---------------------------------------------------------
//       
void CVerificationState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CVerificationState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == EStarted )
                {
                iInternalState = ETtsInitialized;
                }
            else if ( iInternalState == ETtsInitialized )
                {
                iInternalState = ETtsCompleted;
                nextState = this;
                }
            else if ( iInternalState == ETtsCompleted )
                {
                iInternalState = EStartCompleted;
                nextState = this;
                }
            else if ( iInternalState == EStartCompleted )
                {
                iInternalState = ERecordStarted;
                }
            else if ( iInternalState == ERecordStarted )
                {
                iInternalState = ERecordEnded;
                }
            else if ( iInternalState == ERecordEnded )
                {
                iInternalState = ECompleted;
                nextState = this;
                }
            break;
        
        case EVoiceTagSoftKeySelect:
            
            nextState = CPlayState::NewL( DataStorage(), UiModel() );
            break;
            
        case KErrNoMatch:
        case KErrRecogFailed:
        case KErrNoSpeech:
        case KErrTooEarly:
        case KErrTooLong:
        case KErrTooShort:
        
            DataStorage().SetVerificationTryCount( DataStorage().VerificationTryCount() - 1 );
            nextState = CVerificationInitState::NewL( DataStorage(), UiModel() );
            break;
        
        case EVoiceTagSoftKeyOther:
            
            nextState = CVerificationInitState::NewL( DataStorage(), UiModel() );
            break;
        
        case EVoiceTagSoftKeyCancel:
        case EEndCallKeypress:
        case ELongKeypress:
        case KErrNoResults:

            nextState = CAbortState::NewL( DataStorage(), UiModel() );
            break;
        
        case EShortKeypress:
        case ESelectKeypress:
        
            iKeypress = aEvent;
            nextState = this;
            break;
            
        case EUpKeypress:
        case EDownKeypress:
        
            // Do nothing
            break;
            
        default:
            
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CVerificationState::ExecuteL
// ---------------------------------------------------------
//       
void CVerificationState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationState::ExecuteL" );

    if ( HandleKeypressL() )
        {
        iKeypress = ENoKeypress;
        }
    else if ( iInternalState == ETtsCompleted )
        {
        DataStorage().TtsPlayer()->Stop();
        
        // Play the starting tone
        DataStorage().TonePlayer()->PlayTone( EAvkonSIDNameDiallerStartTone ); 
        }
    else if ( iInternalState == EStartCompleted )
        {
        MNssRecognitionHandler* handler = DataStorage().VerificationRecognitionHandler();
        User::LeaveIfError( handler->Recognize( DataStorage().VoiceRecognitionImpl() ) );
        }
    else if ( iInternalState == ECompleted )
        {
        SelectTag();
        
        DataStorage().SetVerificationTryCount( KVerificationTries );
        
        HandleEventL( SelectCommandById() );
        }
    else
        {
        TInt returnValue = KErrNoResults;
        
        if ( DataStorage().CompleteTagList()->Count() != KErrNone &&
             DataStorage().VerificationTryCount() > KErrNone )
            {
            iInternalState = EStarted;

            MNssTag* selectedTag = NULL;
            for ( int i = 0; i < DataStorage().CompleteTagList()->Count(); ++i )
                {
                MNssTag* tag = DataStorage().CompleteTagList()->At( i );
                if ( !DataStorage().DeviceLockMode() ||
                     ( tag->Context()->ContextName() == KVoiceDialContext &&
                     tag->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial ) )
                    {
                    iTagIndex = i;
                    selectedTag = tag;
                    break;
                    }
                }
                     
            if ( selectedTag )
                {
                HBufC* header = selectedTag->SpeechItem()->Text().AllocLC();
                                
                iVerificationDialog = CVerificationDialog::NewL();
                iVerificationDialog->CreateVerificationPopupLC( *header );
                
                PlayPromptL();
                BringToForeground();
    
                iVerificationDialog->RegisterForKeyCallback( DataStorage().VoiceRecognitionImpl() );            
                returnValue = iVerificationDialog->ShowVerificationPopupL(); // Pops things pushed in CreateVerificationPopupLC
                RUBY_DEBUG1( "CVerificationState::ExecuteL - Verification dialog returns [%d]", returnValue );
                
                CleanupStack::PopAndDestroy( header );
                }
            }
        
        if ( returnValue != KErrNone )
            {           
            SelectTag();
            
            DataStorage().SetVerificationTryCount( KVerificationTries );
            
            if ( returnValue == EVoiceTagSoftKeySelect )
                {
                HandleEventL( SelectCommandById() );
                }
            else
                {
                HandleEventL( returnValue );
                }            
            }
        }
    }

// ---------------------------------------------------------
// CVerificationState::CVerificationState
// ---------------------------------------------------------
//              
CVerificationState::CVerificationState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iKeypress( ENoKeypress ),
   iInternalState( ENotStarted )
    {
    }

// ---------------------------------------------------------
// CVerificationState::ConstructL
// ---------------------------------------------------------
//           
void CVerificationState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationState::ConstructL" );
    
    CState::ConstructL();
    
    DataStorage().SetAdaptationEnabled( ETrue );
    }
    
// ---------------------------------------------------------
// CVerificationState::HandleKeypressL
// ---------------------------------------------------------
//           
TBool CVerificationState::HandleKeypressL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationState::HandleKeypressL" );
    
    TBool handled = ETrue;
    
    if ( iKeypress == EShortKeypress )
        {
        SelectTag();

        HandleEventL( EVoiceTagSoftKeySelect );
        }
    else if ( iKeypress == ESelectKeypress )
        {        
        SelectTag();
        
        DataStorage().SetVerificationTryCount( KVerificationTries );
        
        HandleEventL( SelectCommandById() );
        }
    else
        {
        handled = EFalse;
        }
    
    return handled;
    }
    
// ---------------------------------------------------------------------------
// CVerificationState::PlayPromptL
// ---------------------------------------------------------------------------
//
void CVerificationState::PlayPromptL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationState::PlayPromptL" );

    // if TTS is used
    if ( DataStorage().SynthesizerMode() != KErrNone ||
         DataStorage().DeviceLockMode() )
        {
        HBufC* text = NULL;
                                  
        if ( DataStorage().CompleteTagList()->At( iTagIndex )->Context()->ContextName() == KVoiceDialContext )
            {
            text = DataStorage().CompleteTagList()->At( iTagIndex )->SpeechItem()->PartialTextL( KNameTrainingIndex );
            }
        else
            {
            text = HBufC::NewL( 0 );
            }

        CleanupStack::PushL( text );
        
        HBufC* select = StringLoader::LoadLC( R_QAN_VC_VERIFICATION_SELECT );
        HBufC* other = StringLoader::LoadLC( R_QAN_VC_VERIFICATION_OTHER );
        HBufC* cancel = StringLoader::LoadLC( R_QAN_VC_VERIFICATION_CANCEL );
                
        DataStorage().TtsPlayer()->PlayL(
            DataStorage().CompleteTagList()->At( iTagIndex )->SpeechItem()->Text(), 0,
            DataStorage().CompleteTagList()->At( iTagIndex )->SpeechItem()->Text().Length(),
            *text, *select, *other, *cancel );

        CleanupStack::PopAndDestroy( cancel );
        CleanupStack::PopAndDestroy( other );
        CleanupStack::PopAndDestroy( select );
        CleanupStack::PopAndDestroy( text );
        }
    }
    
// ---------------------------------------------------------
// CVerificationState::SelectTag
// ---------------------------------------------------------
//   
void CVerificationState::SelectTag()
    {
    RUBY_DEBUG0( "CVerificationState::SelectTag START" );
    
    if ( DataStorage().CompleteTagList()->Count() > KErrNone )
        {
        DataStorage().SetTag( DataStorage().CompleteTagList()->At( iTagIndex ), ETrue );
    
        DataStorage().CompleteTagList()->Delete( iTagIndex );
        }
        
    RUBY_DEBUG0( "CVerificationState::SelectTag EXIT" );
    }

// ---------------------------------------------------------
// CVerificationState::SelectTag
// ---------------------------------------------------------
//       
TInt CVerificationState::SelectCommandById()
    {
    RUBY_DEBUG0( "CVerificationState::SelectCommandById START" );
    
    TInt index = KErrGeneral;
    
    if ( DataStorage().AdditionalTagList() &&
         DataStorage().AdditionalTagList()->Count() > KErrNone )
        {
        index = DataStorage().AdditionalTagList()->At( 0 )->RRD()->IntArray()->At( 0 );
        DataStorage().SetAdditionalTagList( NULL );
        }
        
    RUBY_DEBUG0( "CVerificationState::SelectCommandById EXIT" );
    
    return iVerificationDialog->SelectedCommand( index );
    }
    
// End of File

