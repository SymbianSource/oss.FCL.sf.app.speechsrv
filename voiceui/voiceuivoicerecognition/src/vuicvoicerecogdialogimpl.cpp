/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The Voice Recognition dialog implementation
*
*/


// INCLUDE FILES
#include <e32base.h>

#include <voiceuidomainpskeys.h>

#include "vuivoicerecognition.hrh"

#include "vuicvoicerecogdialogimpl.h"

#include "vuicpropertyhandler.h"

#include "vuicdatastorage.h"
#include "vuicuimodel.h"

#include "vuicprecheckstate.h"

#include "vuimvoicerecogdialogcallback.h"

#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVoiceRecognitionDialogImpl* CVoiceRecognitionDialogImpl::NewL( MVoiceRecognitionDialogCallback* aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVoiceRecognitionDialogImpl::NewL" );
    
    CVoiceRecognitionDialogImpl* self = new (ELeave) CVoiceRecognitionDialogImpl( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CVoiceRecognitionDialogImpl::~CVoiceRecognitionDialogImpl()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::~CVoiceRecognitionDialogImpl START" );

    iObserver = NULL;
        
    iMutex.Close();
    
    Cancel();

    // If states are equal only one can be deleted       
    if ( iState != iNextState )
        {
        delete iState;
        delete iNextState;
        }
    else
        {
        delete iState;
        }

    delete iDataStorage;
    delete iUiModel;    
        
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::~CVoiceRecognitionDialogImpl EXIT" );
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::ExecuteL
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CVoiceRecognitionDialogImpl::ExecuteL" );

    ChangeState( CPrecheckState::NewL( *iDataStorage, *iUiModel ) );
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::SetDeviceLockMode
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::SetOnlyCallsAllowed()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::SetOnlyCallsAllowed START" );
    
    iDataStorage->SetDeviceLockMode( ETrue );
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::SetOnlyCallsAllowed EXIT" );
    }

// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::CVoiceRecognitionDialogImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVoiceRecognitionDialogImpl::CVoiceRecognitionDialogImpl( MVoiceRecognitionDialogCallback* aObserver )
 : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::CVoiceRecognitionDialogImpl START" );
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::CVoiceRecognitionDialogImpl EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVoiceRecognitionDialogImpl::ConstructL" );
    
    CActiveScheduler::Add( this );
    
    iDataStorage = CDataStorage::NewL( this );
    
    iUiModel = CUiModel::NewL();
    
    iMutex.CreateLocal();
    }
            
// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::RunL
// Handle a change event.
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::RunL()
    {
    RUBY_DEBUG_BLOCK( "CVoiceRecognitionDialogImpl::RunL" );
    
    if ( iState != iNextState )
        {
        delete iState;
        iState = iNextState;
        }
    iNextState = NULL;
    
    if ( iMutex.IsHeld() )
        {
        iMutex.Signal();
        }
    
    TRAPD( error, iState->ExecuteL() );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::RunL - State execution failed with error [%d]", error );
        Exit();
        }
    }
        
// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::DoCancel
// Cancel the request to receive events.
// ---------------------------------------------------------
//        
void CVoiceRecognitionDialogImpl::DoCancel()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::DoCancel START" );

    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::DoCancel EXIT" );
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandleKeypressL
// Called when a key is pressed
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandleKeypressL( TInt aSoftkey )
    {
    RUBY_DEBUG_BLOCK( "CVoiceRecognitionDialogImpl::HandleKeypressL" );
    
    CheckState();
    
    TRAPD( error, iState->HandleEventL( aSoftkey ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandleKeypressL - Event handling failed with error [%d]", error );
        Exit();
        }
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandleRecognizeInitComplete
// Called when HandleRecognizeInit() completes
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandleRecognizeInitComplete( TNssRecognizeInitError aErrorCode )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleRecognizeInitComplete START" );
    
    CheckState();
       
    TInt errorCode = KErrNone;
    
    if ( aErrorCode == MNssRecognizeInitCompleteHandler::EVasRecognizeInitFailed )
        {
        errorCode = KErrInit;
        }
    else if ( aErrorCode == MNssRecognizeInitCompleteHandler::EVasNoTagInContexts )
        {
        errorCode = KErrNoContacts;
        }
    else if ( aErrorCode != MNssRecognizeInitCompleteHandler::EVasErrorNone )
        {
        errorCode = KErrGeneral;
        }
    
    TRAPD( error, iState->HandleEventL( errorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandleRecognizeInitComplete - Event handling failed with error [%d]", error );
        Exit();
        }
   
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleRecognizeInitComplete EXIT" );
    }

#ifndef __WINS__  
#ifdef __FULLDUPLEX_CHANGE
// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandlePreSamplingStarted
// Called when HandlePreSamplingStarted() completes
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandlePreSamplingStarted( TInt aErrorCode )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandlePreSamplingStarted START" );
    
    CheckState();
          
    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandlePreSamplingStarted - Event handling failed with error [%d]", error );
        Exit();
        }
   
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandlePreSamplingStarted EXIT" );
    }
#endif // __FULLDUPLEX_CHANGE
#endif // __WINS__  
   
// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandleRecordStarted
// Called when recording has started
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandleRecordStarted()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleRecordStarted START" );
    
    CheckState();

    TRAPD( error, iState->HandleEventL( KErrNone ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandleRecordStarted - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleRecordStarted EXIT" );
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandleEouDetected
// Called when end-of-utterance is detected
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandleEouDetected()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleEouDetected START" );
    
    CheckState();
    
    TRAPD( error, iState->HandleEventL( KErrNone ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandleEouDetected - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleEouDetected EXIT" );
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandleRecognizeComplete
// Called when recognition ends
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandleRecognizeComplete( CArrayPtrFlat<MNssTag>* aTagList,
                                                           TNssRecognizeError aErrorCode  )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleRecognizeComplete START" );
    
    CheckState();

    TInt errorCode = KErrNone;
    
    if ( aErrorCode == MNssRecognizeEventHandler::EVasRecognitionFailedNoMatch )
        {
        errorCode = KErrNoMatch;
        }
    else if ( aErrorCode == MNssRecognizeEventHandler::EVasRecognizeAccessDeny )
        {
        errorCode = KErrAbort;
        }
    else if ( aErrorCode == MNssRecognizeEventHandler::EVasRecognizeFailed )
        {
        errorCode = KErrRecogFailed;
        }
    else if ( aErrorCode == MNssRecognizeEventHandler::EVasRecognitionFailedNoSpeech )
        {
        errorCode = KErrNoSpeech;
        }
    else if ( aErrorCode == MNssRecognizeEventHandler::EVasRecognitionFailedTooEarly )
        {
        errorCode = KErrTooEarly;
        }
    else if ( aErrorCode == MNssRecognizeEventHandler::EVasRecognitionFailedTooLong )
        {
        errorCode = KErrTooLong;
        }
    else if ( aErrorCode == MNssRecognizeEventHandler::EVasRecognitionFailedTooShort )
        {
        errorCode = KErrTooShort;
        }
    else if ( aErrorCode != MNssRecognizeEventHandler::EVasErrorNone )
        {
        errorCode = KErrGeneral;
        }

    // If complete tag list has been set then results are voice verification results
    // and should be put to additional tag list
    if ( !iDataStorage->CompleteTagList() )
        {
        iDataStorage->SetCompleteTagList( aTagList );
        }
    else
        {
        iDataStorage->SetAdditionalTagList( aTagList );
        }

    TRAPD( error, iState->HandleEventL( errorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandleRecognizeComplete - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleRecognizeComplete EXIT" );
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandleAdaptComplete
// Called when the adaptation is complete
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandleAdaptComplete( TInt aErrorCode )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleAdaptComplete START" );
    
    CheckState();
      
    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandleAdaptComplete - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleAdaptComplete EXIT" );
    }
    
// ---------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::MapcInitComplete
// CMdaAudioPlayerUtility initialization complete
// ---------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::MapcInitComplete( TInt aError,
                                                    const TTimeIntervalMicroSeconds& aDuration )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::MapcInitComplete START" );
    
    CheckState();

    iDataStorage->SetPlayDuration( aDuration );
    
    TRAPD( error, iState->HandleEventL( aError ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::MapcInitComplete - Event handling failed with error [%d]", error );
        Exit();
        }

    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::MapcInitComplete EXIT" );
    }

// ---------------------------------------------------------------------------
// CVoiceRecogPlaybackDialog::MapcPlayComplete
// Playback complete, notify observer
// ---------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::MapcPlayComplete( TInt aError )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::MapcPlayComplete START" );
    
    CheckState();
 
    TRAPD( error, iState->HandleEventL( aError ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::MapcPlayComplete - Event handling failed with error [%d]", error );
        Exit();
        }
        
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::MapcPlayComplete EXIT" );
    }
    
// ---------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::MapcCustomCommandEvent
// Called when asynchronous custom command finishes.
// ---------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::MapcCustomCommandEvent( TInt /*aEvent*/, TInt aError )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::MapcCustomCommandEvent START" );
    
    CheckState();
      
    TRAPD( error, iState->HandleEventL( aError ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::MapcCustomCommandEvent - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::MapcCustomCommandEvent EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::GetContextCompleted
// -----------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::GetContextCompleted( MNssContext* aContext, TInt aErrorCode )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::GetContextCompleted START" );
    
    CheckState();
    
    iDataStorage->SetContext( aContext );
      
    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::GetContextCompleted - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::GetContextCompleted EXIT" );
    }

// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::GetContextListCompleted
// -----------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::GetContextListCompleted( MNssContextListArray* /*aContextList*/,
                                                           TInt aErrorCode )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::GetContextListCompleted START" );
    
    CheckState();
      
    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::GetContextListCompleted - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::GetContextListCompleted EXIT" );
    }

// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::GetTagListCompleted
// -----------------------------------------------------------------------------
//    
void CVoiceRecognitionDialogImpl::GetTagListCompleted( MNssTagListArray* aTagList, TInt aErrorCode )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::GetTagListCompleted START" );
    
    CheckState();

    iDataStorage->SetAdditionalTagList( aTagList );

    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::GetTagListCompleted - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::GetTagListCompleted EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::SaveContextCompleted
// -----------------------------------------------------------------------------
//  
void CVoiceRecognitionDialogImpl::SaveContextCompleted( TInt aErrorCode )
	{
	RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::SaveContextCompleted START" );
	
    CheckState();

    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::SaveContextCompleted - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::SaveContextCompleted EXIT" );
	}
    
// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::HandleTrainComplete
// -----------------------------------------------------------------------------
//       
void CVoiceRecognitionDialogImpl::HandleTrainComplete( TInt aErrorCode )
	{
	RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleTrainComplete START" );
    
    CheckState();

    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::HandleTrainComplete - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandleTrainComplete EXIT" );
	}
    
// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::SaveTagCompleted
// -----------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::SaveTagCompleted( TInt aErrorCode )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::SaveTagCompleted START" );
    
    CheckState();

    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::SaveTagCompleted - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::SaveTagCompleted EXIT" );
    }

// -----------------------------------------------------------------------------
// CVoiceRecognitionDialogImpl::DeleteContextCompleted
// -----------------------------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::DeleteContextCompleted( TInt aErrorCode )
	{
	RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::DeleteContextCompleted START" );
    
    CheckState();

    TRAPD( error, iState->HandleEventL( aErrorCode ) );
    
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CVoiceRecognitionDialogImpl::DeleteContextCompleted - Event handling failed with error [%d]", error );
        Exit();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::DeleteContextCompleted EXIT" );
	}
    
// ----------------------------------------------------
// CVoiceRecognitionDialog::HandlePropertyValueChange
// Handle property value change event.
// ----------------------------------------------------
//
void CVoiceRecognitionDialogImpl::HandlePropertyValueChange( TInt aValue )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandlePropertyValueChange START" );
    
    CheckState();
   
    switch( aValue )
        {
        // Short press of a headset key
        case KVoiceUiShortPressEvent:
            {
            TRAP_IGNORE( iState->HandleEventL( EShortKeypress ) );
            break;
            }

        // Long press of a headset key
        case KVoiceUiLongPressEvent:
            {
            TRAP_IGNORE( iState->HandleEventL( ELongKeypress ) );
            break;
            }

        default:
            {
            // pass
            break;
            }
        }

    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::HandlePropertyValueChange EXIT" );         
    }
       
// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::ChangeState
// Change current state and set object active
// ---------------------------------------------------------
//  
void CVoiceRecognitionDialogImpl::ChangeState( CState* aNextState )
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::ChangeState START" ); 
    
    if ( aNextState )
        {
        CheckState();
        
        iNextState = aNextState;
        
        if ( !IsActive() )
            {
            TRequestStatus* pRS = &iStatus;
            User::RequestComplete( pRS, KErrNone );
            SetActive();
            }        
        }       
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::ChangeState EXIT" ); 
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::CheckState
// Check if state can be changed and if it can't starts waiting
// ---------------------------------------------------------
//      
void CVoiceRecognitionDialogImpl::CheckState()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::CheckState START" ); 
    
    if ( iNextState )
        {
        RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::CheckState - Start waiting" );
        iMutex.Wait();
        }
    
    RUBY_DEBUG0( "CVoiceRecognitionDialogImpl::CheckState EXIT" ); 
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialogImpl::Exit
// 
// ---------------------------------------------------------
//
void CVoiceRecognitionDialogImpl::Exit()
    {
    if ( iObserver )
        {
        iObserver->DialogDismissed();
        
        iObserver = NULL;
        }
    }

// End of File
