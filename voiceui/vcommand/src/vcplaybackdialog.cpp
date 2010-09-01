/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CVCPlaybackDialog
*
*/



// INCLUDE FILES

#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <aknsoundsystem.h>
#include <aknappui.h>
#include <avkon.hrh>
#include <vcommandapi.h>
#include "vcplaybackdialog.h"
#include "vcmodel.h"
#include "rubydebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCPlaybackDialog::CVCPlaybackDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVCPlaybackDialog::CVCPlaybackDialog( CVCommandHandler& aService,
                                      const CVCommandUiEntry& aCommand ) : 
                                      CAknProgressDialog( NULL ),
                                      iService( aService ),
                                      iCommand( aCommand )
    {
    iTModel.iFinalValue = KVoiceTagRecordLength;
    iTModel.iHundreths = KInterval;
    iTModel.iIncrement = KVoiceTagIncrement;
    iTModel.iRunning = EFalse;
    iVisibilityDelayOff = ETrue;
    }


// Destructor
CVCPlaybackDialog::~CVCPlaybackDialog()
    {
    if (iProgressBarTimer)
        {
        iProgressBarTimer->Cancel();
        delete iProgressBarTimer;
        }
    if (iProgressInfo)
        {
        iProgressInfo = NULL;
        }
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void CVCPlaybackDialog::PreLayoutDynInitL()
    {
    RUBY_DEBUG_BLOCKL( "CVCPlaybackDialog::PreLayoutDynInitL" );
    
    HBufC* text = StringLoader::LoadLC( R_QTN_VC_PLAYING );
    SetTextL( *text );
    CleanupStack::PopAndDestroy( text );

    ButtonGroupContainer().SetCommandSetL( R_SOFTKEYS_QUIT );   
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::PostLayoutDynInitL
// ---------------------------------------------------------
//
void CVCPlaybackDialog::PostLayoutDynInitL()
    {
    RUBY_DEBUG_BLOCKL( "CVCPlaybackDialog::PostLayoutDynInitL" );
    
    // FIrst check if a user edited command exists
    if( iCommand.AlternativeSpokenText().Length() > 0 )
        {
        iCommand.PlayAlternativeSpokenTextL( iService, *this );
        }
    else
        {
        iCommand.PlaySpokenTextL( iService, *this );        
        }
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CVCPlaybackDialog::OkToExitL( TInt aButtonId )
    {
    RUBY_DEBUG_BLOCKL( "CVCPlaybackDialog::OkToExitL" );
    
    // Finished playing
    if ( iExitWhenCalledBack )
        {
        return ETrue;
        }

    if ( ( iTickCount < 1 ) && ( !iExitFlag ) )
        {
        return EFalse;
        }

    // Check what button was pressed
    switch ( aButtonId )
        {
        case EAknSoftkeyOk:
        break;

        case EKeyPhoneEnd:
        case EKeyApplication:
        case EAknSoftkeyQuit:
            {
            iOkToExit = ETrue;
            
            if ( iExitFlag )
            	{
            	iCommand.CancelPlaybackL( iService );
            	}
        	
        	break;
            }
            
        //case EVCommandSoftKeyQuit:
        case EAknSoftkeyDone:
            {
            iOkToExit = ETrue;

            iCommand.CancelPlaybackL( iService );
            
            break;
            }

        default:
        	iOkToExit = EFalse;  
        break;
        }

    return iOkToExit;
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVCPlaybackDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    RUBY_DEBUG_BLOCKL( "CVCPlaybackDialog::OfferKeyEventL" );
    
    // Only react to red and application key
    if ( aType == EEventKey )
        {             
        if ( aKeyEvent.iCode == EKeyApplication )
            {
            iExitFlag = ETrue;
            TryExitL( EKeyApplication );
            return EKeyWasConsumed;
            }
        else if ( aKeyEvent.iCode == EKeyPhoneEnd )
            {
            iExitFlag = ETrue;
            TryExitL( EKeyPhoneEnd );
            return EKeyWasConsumed;
            }
        }
    
    return EKeyWasConsumed;
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::StartTimerL
// ---------------------------------------------------------
//
void CVCPlaybackDialog::StartTimerL()
   {
   RUBY_DEBUG_BLOCKL( "CVCPlaybackDialog::StartTimerL" );
   
   iTickCount=0;
   iProgressBarTimer = CPeriodic::NewL( CActive::EPriorityStandard );
   TTimeIntervalMicroSeconds32 delay( KDelay );
   TTimeIntervalMicroSeconds32 interval( KMicroSecondsInterval*iTModel.iHundreths );
   iProgressBarTimer->Start( delay,interval, TCallBack( OnTick, this ) );
   }

// ---------------------------------------------------------
// CVCPlaybackDialog::StopTimer
// ---------------------------------------------------------
//
void CVCPlaybackDialog::StopTimer()
    {
    if ( iProgressBarTimer )
        {
        iProgressBarTimer->Cancel();
        delete iProgressBarTimer;
        iProgressBarTimer = NULL;
        }
    }

// ---------------------------------------------------------
// CVCPlaybackDialog::OnTick
// ---------------------------------------------------------
//
TInt CVCPlaybackDialog::OnTick( TAny* aObject )
    { 
    STATIC_CAST( CVCPlaybackDialog*, aObject )->DoTick();
    return 1; // magic
    }

// ---------------------------------------------------------
// CVCPlaybackDialog::DoTick
// ---------------------------------------------------------
//
void CVCPlaybackDialog::DoTick()
    {
    if ( iTickCount != iTModel.iFinalValue+1 )
        {
        iProgressInfo->IncrementAndDraw( iTModel.iIncrement );
        iTickCount++;
        }

    if (iExitFlag) 
        {
        RUBY_DEBUG0( "CVCPlaybackDialog::DoTick: Exit due to iExitFlag" );
        StopTimer();
        }
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::DoHandlePlayErrorL
// ---------------------------------------------------------
//
void CVCPlaybackDialog::DoHandlePlayErrorL( TNssPlayResult aResult )
    {
    RUBY_DEBUG1( "CVCPlaybackDialog::DoHandlePlayErrorL: [%d]", aResult );
    HandlePlayComplete( aResult );
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::HandlePlayStarted
// ---------------------------------------------------------
//
void CVCPlaybackDialog::HandlePlayStarted(
    TTimeIntervalMicroSeconds32 aDuration )
    {
    TRAP_IGNORE( DoHandlePlayStartedL( aDuration ) );
    }

// ---------------------------------------------------------
// CVCPlaybackDialog::DoHandlePlayStartedL
// ---------------------------------------------------------
//
void CVCPlaybackDialog::DoHandlePlayStartedL(
    TTimeIntervalMicroSeconds32 aDuration )
    {
    RUBY_DEBUG_BLOCKL( "CVCPlaybackDialog::DoHandlePlayStartedL" );
    
    iTModel.iFinalValue = aDuration.Int()/KMicroSecondsInterval;
    iProgressInfo = GetProgressInfoL();
    iProgressInfo->SetFinalValue( iTModel.iFinalValue );
    StartTimerL();
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::HandlePlayComplete
// ---------------------------------------------------------
//
#ifdef _DEBUG
void CVCPlaybackDialog::HandlePlayComplete( TNssPlayResult aErrorCode )
#else
void CVCPlaybackDialog::HandlePlayComplete( TNssPlayResult /*aErrorCode*/ )
#endif
    {
    RUBY_DEBUG1( "CVCPlaybackDialog::HandlePlayComplete: [%d]", aErrorCode );
    iExitWhenCalledBack = ETrue;
    StopTimer();
    TRAP_IGNORE( TryExitL( EAknSoftkeyOk) );
    }


// ---------------------------------------------------------
// CVCPlaybackDialog::DisplayErrorNoteL
// ---------------------------------------------------------
//
void CVCPlaybackDialog::DisplayErrorNoteL()
    {
    RUBY_DEBUG_BLOCKL( "CVCPlaybackDialog::DisplayErrorNoteL" );
    
    iExitFlag = ETrue;
    iOkToExit = ETrue; 
    HBufC* text = StringLoader::LoadLC( R_TEXT_VOICE_SYSTEM_ERROR );
    CAknErrorNote* dlg = new (ELeave) CAknErrorNote( ETrue );
    dlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }


//  End of File  
