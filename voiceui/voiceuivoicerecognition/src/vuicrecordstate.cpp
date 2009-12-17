/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <featmgr.h>

#include <avkon.rsg>
#include <AknIconUtils.h>
#include <StringLoader.h>

#include <vuivoicerecognition.rsg>

#include "vuicstate.h"
#include "vuicrecordstate.h"
#include "vuictutorialstate.h"
#include "vuicabortstate.h"
#include "vuicerrorstate.h"

#include "vuicdatastorage.h"
#include "vuicuimodel.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuicglobalprogressdialog.h"

#include "vuivoiceicondefs.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CRecordState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRecordState* CRecordState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CRecordState* self = new (ELeave) CRecordState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CRecordState::~CRecordState()
    {
    RUBY_DEBUG0( "CRecordState::~CRecordState START" );
    
    // If recording has not ended we should cancel recording
    if ( iInternalState != EFinished )
        {
        DataStorage().RecognitionHandler()->Cancel();
        }        

    delete iProgressDialog;
    
    RUBY_DEBUG0( "CRecordState::~CRecordState EXIT" );
    }
    
// ---------------------------------------------------------
// CRecordState::HandleEventL
// ---------------------------------------------------------
//       
void CRecordState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CRecordState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == EStarted )
                {
                iInternalState = ERecording;
                nextState = this;
                }
            else if ( iInternalState == ERecording )
                {
                iInternalState = EStopped;
                }
            else if ( iInternalState == EStopped )
                {
                iInternalState = EFinished;
                nextState = CTutorialState::NewL( DataStorage(), UiModel() );
                }
            break;
            
        case KErrNoMatch:
        case KErrRecogFailed:
        case KErrNoSpeech:
        case KErrTooEarly:
        case KErrTooLong:
        case KErrTooShort:
        case KErrGeneral:
        
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
            
        case EEndCallKeypress:
        
            // Do nothing
            break;
            
        default:
        
            nextState = CAbortState::NewL( DataStorage(), UiModel() );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CRecordState::ExecuteL
// ---------------------------------------------------------
//       
void CRecordState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CRecordState::ExecuteL" );
    
    if ( iInternalState == ENotStarted )
        {
        // Show the "Speak now" prompt and start the voice recording
        MNssRecognitionHandler* handler = DataStorage().RecognitionHandler();
        User::LeaveIfError( handler->Recognize( DataStorage().VoiceRecognitionImpl() ) );
    
        iProgressDialog = CGlobalProgressDialog::NewL();
   
        iProgressDialog->SetImageL( AknIconUtils::AvkonIconFileName(),
                                    EMbmAvkonQgn_note_voice,
                                    EMbmAvkonQgn_note_voice_mask );
                                    
        iProgressDialog->SetSkinIds( KAknsIIDQgnNoteVoice, EMbmAvkonQgn_note_voice,
                                     EMbmAvkonQgn_note_voice_mask, KAknsIIDQgnStatBtBlank );

        iProgressDialog->RegisterForKeyCallback( DataStorage().VoiceRecognitionImpl() );     
        
        iProgressDialog->SetSecondaryDisplayDataL( SecondaryDisplay::KCatVoiceUi,
                                                   SecondaryDisplay::ECmdShowVoiceSpeakNowNote,
                                                   NULL );
                                                   
        BringToForeground();

        HBufC* speakMsg = StringLoader::LoadLC( R_TEXT_VOICE_SPEAK_NOW );
        iProgressDialog->ShowProgressDialogL( *speakMsg, KNullDesC, R_AVKON_SOFTKEYS_QUIT );
        CleanupStack::PopAndDestroy( speakMsg );
        
        iInternalState = EStarted;
        }
    else if ( iInternalState == ERecording )
        {
        StartTimerL( *this, KRecogDelay, KKRecogInterval * iTModel.iHundreths );
        }
    }

// ---------------------------------------------------------
// CRecordState::CRecordState
// ---------------------------------------------------------
//              
CRecordState::CRecordState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iInternalState( ENotStarted ),
   iTickCount( 0 )
    {    
    iTModel.iFinalValue = KRecogFinal;
    iTModel.iHundreths = KRecogInterval;
    iTModel.iIncrement = KRecogIncrement;
    iTModel.iRunning = EFalse;
    }
    
// ---------------------------------------------------------
// CRecordState::DoTimedEventL
// ---------------------------------------------------------
//
void CRecordState::DoTimedEventL()
   {
   if ( ( ++iTickCount != iTModel.iFinalValue + 1 ) && iProgressDialog )
      {
      iProgressDialog->UpdateProgressDialogL( iTickCount, iTModel.iFinalValue ); 
      }
   else
      {
      StopTimer();
      }
   }
    
// End of File

