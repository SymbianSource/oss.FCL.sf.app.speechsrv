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

#include "speechsynthesistester.h"
#include "rubydebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSpeechSynthesisTester* CSpeechSynthesisTester::NewL( MTesterObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::NewL" );
    
    CSpeechSynthesisTester* self =
        new ( ELeave ) CSpeechSynthesisTester( aObserver );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    
    return self;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::CSpeechSynthesisTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSpeechSynthesisTester::CSpeechSynthesisTester( MTesterObserver& aObserver ):
    CActive( EPriorityStandard ),
    iTestStatus( EIdle ),
    iObserver( aObserver )
    {
    RUBY_DEBUG0( "CSpeechSynthesisTester::CSpeechSynthesisTester" );
    
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CSpeechSynthesisTester::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::~CSpeechSynthesisTester
// Destructor.
// -----------------------------------------------------------------------------
//
CSpeechSynthesisTester::~CSpeechSynthesisTester()
    {
    RUBY_DEBUG0( "CSpeechSynthesisTester::~CSpeechSynthesisTester" );
    
    Cancel();
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::Open
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTester::Open()
    {
    RUBY_DEBUG0( "CSpeechSynthesisTester::Open" );
    
    return iSpeechSynthesis.Open();
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::Close
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::Close()
    {
    RUBY_DEBUG0( "CSpeechSynthesisTester::Close" );
    
    iSpeechSynthesis.Close();
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::SynthesisInit
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::SynthesisInit( TDesC& aText, const TDesC& aFileName )
    {
    RUBY_DEBUG0( "CSpeechSynthesisTester::SynthesisInit" );
    
    Cancel();
    SetActive();
    
    iTestStatus = EPriming;
    
    if ( aFileName.Length() > 0 )
        {
        iSpeechSynthesis.InitialiseSynthesis( aText, aFileName, iStatus );
        }
    else
        {
        iSpeechSynthesis.InitialiseSynthesis( aText, iStatus );
        }
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::Synthesise
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::Synthesise()
    {
    RUBY_DEBUG0( "CSpeechSynthesisTester::Synthesise" );
    
    Cancel();
    SetActive();
        
    iTestStatus = EPlaying;
    
    iSpeechSynthesis.Synthesise( iStatus );
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::StopL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::StopL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::StopL" );
    
    iSpeechSynthesis.StopL();
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::PauseL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::PauseL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::PauseL" );
    
    iSpeechSynthesis.PauseL();
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::GetSupportedLanguagesL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::GetSupportedLanguagesL(  RArray<TLanguage>& aLanguages )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::GetSupportedLanguagesL" );
    
    iSpeechSynthesis.GetLanguagesL( aLanguages );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::GetSupportedVoicesL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::GetSupportedVoicesL( RArray<TVoice>& aVoices, 
                                                  TLanguage aLanguage )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::GetSupportedVoicesL" );
    
    iSpeechSynthesis.GetVoicesL( aVoices, aLanguage );
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::VoiceL
// 
// -----------------------------------------------------------------------------
//
TVoice CSpeechSynthesisTester::VoiceL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::VoiceL" );
    
    return iSpeechSynthesis.VoiceL();
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::SetVoiceL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::SetVoiceL( TVoice& aVoice )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::SetVoiceL" );
    
    iSpeechSynthesis.SetVoiceL( aVoice );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::MaxVolumeL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTester::MaxVolumeL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::MaxVolumeL" );
    
    return iSpeechSynthesis.MaxVolumeL();
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::VolumeL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTester::VolumeL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::VolumeL" );
    
    return iSpeechSynthesis.VolumeL();
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::SetVolumeL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::SetVolumeL( TInt aVolume )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::SetVolumeL" );
    
    iSpeechSynthesis.SetVolumeL( aVolume );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::MaxSpeakingRateL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTester::MaxSpeakingRateL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::MaxSpeakingRateL" );
    
    return iSpeechSynthesis.MaxSpeakingRateL();
    }
        
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::SpeakingRateL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTester::SpeakingRateL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::SpeakingRateL" );
    
    return iSpeechSynthesis.SpeakingRateL();
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::SetSpeakingRateL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::SetSpeakingRateL( TInt aRate )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::SetSpeakingRateL" );
    
    iSpeechSynthesis.SetSpeakingRateL( aRate );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::SetAudioPriorityL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::SetAudioPriorityL( TInt aPriority, TInt aPreference )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::SetAudioPriorityL" );
    
    iSpeechSynthesis.SetAudioPriorityL( aPriority, aPreference );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::CustomCommandL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::CustomCommandL( TInt aCommand, TInt aValue )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::CustomCommandL" );
    
    iSpeechSynthesis.CustomCommandL( aCommand, aValue );
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::RunL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::RunL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisTester::RunL" );
    
    switch( iTestStatus )
        {
        case EPriming:
        
            iObserver.SynthesisInitComplete( iStatus.Int() ); 
         
            break;
            
        case EPlaying:
        
            iObserver.SynthesisComplete( iStatus.Int() ); 
            
            break;
        
        default:
        
            RUBY_ERROR0( "CSpeechSynthesisTester::RunL - In default!" );
             
        }

    iTestStatus = EIdle;
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTester::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTester::DoCancel()
    {
    RUBY_DEBUG0( "CSpeechSynthesisTester::DoCancel" );
    
    iTestStatus = EIdle;
    }
    
// End of File
