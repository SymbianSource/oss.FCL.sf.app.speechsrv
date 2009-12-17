/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "testappengine.h"

#include "rubydebug.h"


_LIT( KText, "This is a text sample for TTS test. 1 2 3 45 End :) :-) :-(");

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CTestAppEngine::NewL()
// 
// ----------------------------------------------------------
//
CTestAppEngine* CTestAppEngine::NewL( CTestAppAppUi& aAppUi )
    {
    RUBY_DEBUG_BLOCK( "CTestAppEngine::NewL" );
    
    CTestAppEngine* self = new( ELeave ) CTestAppEngine( aAppUi );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;    
    }
    
// ----------------------------------------------------------
// CTestAppEngine::ConstructL()
// 
// ----------------------------------------------------------
//
void CTestAppEngine::ConstructL() 
    {
    // Add ourselves to active scheduler
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CTestAppEngine::CTestAppEngine()
// 
// ----------------------------------------------------
//
CTestAppEngine::CTestAppEngine( CTestAppAppUi& aAppUi ) :
    CActive( EPriorityStandard ),
    iAppUi( aAppUi ),
    iVoiceIndex( -1 ),
    iRate( -1 )
    {
    }
    
// ----------------------------------------------------
// CTestAppEngine::~CTestAppEngine()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CTestAppEngine::~CTestAppEngine()
    {
    Cancel();
    
    iServer.Close();    
    iVoices.Close();
    }

// ----------------------------------------------------
// CTestAppEngine::RunL
// 
// ----------------------------------------------------
//
void CTestAppEngine::RunL()
    {
    RUBY_DEBUG_BLOCK( "CTestAppEngine::RunL" );
    
    switch ( iState )
        {
        case EPriming:
        
            if ( iStatus == KErrNone ) 
                {
                TRAP_IGNORE(
                    TTimeIntervalMicroSeconds duration = iServer.DurationL();
                    iAppUi.ChangeTextL( 4, _L("Duration (ms)"), 
                                        duration.Int64() / 1000 );
                     );
                
                PlayL();
                }
            else
                {
                User::Leave( iStatus.Int() );
                }
                
            break;
        
        case ESpeaking:
        
            if ( iStatus == KErrNone )
                {
                iAppUi.ChangeTextL( 0, _L("Server: Connected") );
                iAppUi.ChangeTextL( 4, _L("Duration (ms)"), 0 );
                }
            else if ( iStatus == KErrCancel )
                {
                iAppUi.ChangeTextL( 0, _L("Server: Paused") );
                }
            else if ( iStatus == KErrAbort )
                {
                iAppUi.ChangeTextL( 0, _L("Server: Stopped") );
                }
            else
                {
                User::Leave( iStatus.Int() );
                }
            
            iState = EIdle;
            
            break;
            
        default:
            
            break;
            
        }
    }
    
// ----------------------------------------------------
// CTestAppEngine::DoCancel
// 
// ----------------------------------------------------
//
void CTestAppEngine::DoCancel()
    {
    RUBY_DEBUG0( "CTestAppEngine::DoCancel" );
    
    TRAP_IGNORE( iServer.StopL() );
    }

// ----------------------------------------------------
// Connect to the server
// 
// ----------------------------------------------------
//
void CTestAppEngine::ConnectL()
    {
    User::LeaveIfError( iServer.Open() );

    iVoice = iServer.VoiceL();
    iVolume = iServer.VolumeL();
    iRate = iServer.SpeakingRateL();
    
    iAppUi.ChangeTextL( 1, iVoice.iVoiceName, iVoice.iLanguage );
    iAppUi.ChangeTextL( 2, _L("Volume"), iVolume );
    iAppUi.ChangeTextL( 3, _L("Speking Rate"), iRate );
    }

// ----------------------------------------------------
// Disconnect from server    
// 
// ----------------------------------------------------
//
void CTestAppEngine::DisconnectL()
    {
    Cancel();
    
    iServer.Close();
    }

// ----------------------------------------------------
// CTestAppEngine::InitTextL
// 
// ----------------------------------------------------
//
void CTestAppEngine::InitTextL()
    {
    iAppUi.ChangeTextL( 0, _L("Server: Initializing") ); 
    
    iServer.SetVolumeL( iVolume );
    
    if ( iRate > 0 && iRate <= 100 )
        {
        iServer.SetSpeakingRateL( iRate );
        }
        
    Cancel();
    SetActive();
    iState = EPriming;
    
    iServer.InitialiseSynthesis( KText, iStatus );
    }

// ----------------------------------------------------
// CTestAppEngine::InitToTextFileL
// 
// ----------------------------------------------------
//
void CTestAppEngine::InitTextToFileL()
    {
    iFileName = _L("C:\\test.raw");
    
    iAppUi.ChangeTextL( 0, _L("Server: Initializing") ); 
        
    iServer.SetVolumeL( iVolume );
    iServer.SetSpeakingRateL( iRate );
    
    Cancel();
    SetActive();
    iState = EPriming;
    
    iServer.InitialiseSynthesis( KText, iFileName, iStatus );
    }

// ----------------------------------------------------
// CTestAppEngine::PauseL
// 
// ----------------------------------------------------
//
void CTestAppEngine::PauseL()
    {
    iAppUi.ChangeTextL( 0, _L("Server: Paused") );
    iServer.PauseL();
    }

// ----------------------------------------------------
// CTestAppEngine::PlayL
// 
// ----------------------------------------------------
//
void CTestAppEngine::PlayL()
    {
    iAppUi.ChangeTextL( 0, _L("Server: Speaking") );
    
    Cancel();
    SetActive();
    iState = ESpeaking;
        
    iServer.Synthesise( iStatus );
    }

// ----------------------------------------------------
// CTestAppEngine::StopL
// 
// ----------------------------------------------------
//
void CTestAppEngine::StopL()
    {
    iAppUi.ChangeTextL( 0, _L("Server: Stopped") );
    iServer.StopL();
    }

// ----------------------------------------------------
// CTestAppEngine::ChangeVoiceL
// 
// ----------------------------------------------------
//
TVoice CTestAppEngine::ChangeVoiceL()
    {
    if ( iVoiceIndex < 0 )
        {
        FetchSupportedLanguagesAndVoicesL();
        }
    
    iVoiceIndex++;
    
    if ( iVoiceIndex >= iVoices.Count() )
        {
        iVoiceIndex = 0;
        }
    
    iVoice = iVoices[iVoiceIndex];
    
    iServer.SetVoiceL( iVoice );
    
    return iVoice;
    }

// ----------------------------------------------------
// CTestAppEngine::VolumeDownL
// 
// ----------------------------------------------------
//
TInt CTestAppEngine::VolumeDownL()
    {
    TInt volumeStep = iServer.MaxVolumeL() / 10;
    
    iServer.SetVolumeL( iVolume - volumeStep );
    iVolume = iServer.VolumeL();

    return iVolume;
    }

// ----------------------------------------------------
// CTestAppEngine::VolumeUpL  
// 
// ----------------------------------------------------
//
TInt CTestAppEngine::VolumeUpL()
    {
    TInt volumeStep = iServer.MaxVolumeL() / 10;
    
    iServer.SetVolumeL( iVolume + volumeStep );
    iVolume = iServer.VolumeL();

    return iVolume;
    }

// ----------------------------------------------------
// CTestAppEngine::RateUpL  
// 
// ----------------------------------------------------
//
TInt CTestAppEngine::RateUpL()
    {
    TInt newRate = iRate + 10;
    
    if ( newRate > iServer.MaxSpeakingRateL() )
        {
        newRate = 10;
        }
    
    iRate = newRate;
    
    return iRate;
    }
    
// ----------------------------------------------------
// CTestAppEngine::ChangeAudioOutputL
// 
// ----------------------------------------------------
//
void CTestAppEngine::ChangeAudioOutputL()    
    {
    iCurrentAudioOutput++;
    
    if ( iCurrentAudioOutput > RSpeechSynthesis::ETtsOutputPublic )
        {
        iCurrentAudioOutput = 0;
        }
    
    iServer.SetAudioOutputL( (RSpeechSynthesis::TTtsAudioOutput) iCurrentAudioOutput );
    }
    
// ----------------------------------------------------
// CTestAppEngine::FetchSupportedLanguagesAndVoicesL
// 
// ----------------------------------------------------
//
void CTestAppEngine::FetchSupportedLanguagesAndVoicesL()
    {
    RArray<TLanguage> languages;
    RArray<TVoice> voices;

    iServer.GetLanguagesL( languages );
    
    for ( TInt i( 0 ); i < languages.Count(); i++ )
        {
        if ( languages[i] > 0 )
            {
            RUBY_DEBUG1( "%d", languages[i] );

            voices.Reset();
            iServer.GetVoicesL( voices, languages[i] );
    
            for ( TInt j( 0 ); j < voices.Count(); j++ )
                {            
                RUBY_DEBUG1( "%S", &voices[j].iVoiceName );

                iVoices.AppendL( voices[j] );
                }        
            }
        }
        
    voices.Close();
    languages.Close();
    }

// End of File  
