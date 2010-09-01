/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "texttospeechengine.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CTextToSpeechEngine::NewL()
// 
// ----------------------------------------------------------
//
CTextToSpeechEngine* CTextToSpeechEngine::NewL()
    {
    CTextToSpeechEngine* self = new( ELeave ) CTextToSpeechEngine();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;    
    }
    
// ----------------------------------------------------------
// CTextToSpeechEngine::ConstructL()
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::ConstructL() 
    {
    // Add ourselves to active scheduler
    CActiveScheduler::Add( this );
    
    User::LeaveIfError( iServer.Open() );
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::CTextToSpeechEngine()
// 
// ----------------------------------------------------------
//
CTextToSpeechEngine::CTextToSpeechEngine() :
    CActive( EPriorityStandard )
    {
    }
    
// ----------------------------------------------------------
// CTextToSpeechEngine::~CTextToSpeechEngine()
// Destructor
// Frees reserved resources
// ----------------------------------------------------------
//
CTextToSpeechEngine::~CTextToSpeechEngine()
    {
    Cancel();
    
    iServer.Close();
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::InitAndPlayText
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::InitAndPlayText( const TDesC& aText )
    {
    Cancel();
    SetActive();
    iState = EPriming;
    
    iServer.InitialiseSynthesis( aText, iStatus );
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::InitAndPlayTextToFile
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::InitAndPlayTextToFile( const TDesC& aText,
                                                 const TFileName& aFileName )
    {
    iFileName = aFileName;
    
    Cancel();
    SetActive();
    iState = EPriming;
    
    iServer.InitialiseSynthesis( aText, iFileName, iStatus );
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::PauseL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::PauseL()
    {
    iServer.PauseL();
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::Play
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::Play()
    {
    Cancel();
    SetActive();
    iState = ESpeaking;
        
    iServer.Synthesise( iStatus );
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::StopL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::StopL()
    {
    iServer.StopL();
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::VolumeL
// 
// ----------------------------------------------------------
//
TInt CTextToSpeechEngine::VolumeL()
    {
    return iServer.VolumeL();
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::SetVolumeL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::SetVolumeL( TInt aVolume )
    {
    iServer.SetVolumeL( aVolume );
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::MaxVolumeL
// 
// ----------------------------------------------------------
//
TInt CTextToSpeechEngine::MaxVolumeL()
    {
    return iServer.MaxVolumeL();
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::SpeakingRateL
// 
// ----------------------------------------------------------
//
TInt CTextToSpeechEngine::SpeakingRateL()
    {
    return iServer.SpeakingRateL();
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::SetSpeakingRateL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::SetSpeakingRateL( TInt aRate )
    {
    iServer.SetSpeakingRateL( aRate );
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::MaxSpeakingRateL
// 
// ----------------------------------------------------------
//
TInt CTextToSpeechEngine::MaxSpeakingRateL()
    {
    return iServer.MaxSpeakingRateL();
    }

// ----------------------------------------------------------
// CTextToSpeechEngine::VoiceL
// 
// ----------------------------------------------------------
//
TVoice CTextToSpeechEngine::VoiceL()
    {
    return iServer.VoiceL();
    }
    
// ----------------------------------------------------------
// CTextToSpeechEngine::SetVoiceL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::SetVoiceL( const TVoice& aVoice )
    {
    iServer.SetVoiceL( aVoice );
    }
    
// ----------------------------------------------------------
// CTextToSpeechEngine::GetLanguagesL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::GetLanguagesL( RArray<TLanguage>& aLanguages )
    {
    iServer.GetLanguagesL( aLanguages );
    }
    
// ----------------------------------------------------------
// CTextToSpeechEngine::GetVoicesL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::GetVoicesL( RArray<TVoice>& aVoices, 
                                      TLanguage aLanguage )
    {
    iServer.GetVoicesL( aVoices, aLanguage );
    }
    
// ----------------------------------------------------------
// CTextToSpeechEngine::RunL
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::RunL()
    {
    switch ( iState )
        {
        case EPriming:
        
            if ( iStatus == KErrNone ) 
                {
                Play();
                }
            else
                {
                User::Leave( iStatus.Int() );
                }
                
            break;
        
        case ESpeaking:
        
            if ( iStatus != KErrNone && iStatus != KErrCancel )
                {
                User::Leave( iStatus.Int() );
                }
            
            iState = EIdle;
            
            break;
            
        default:
            
            break;
            
        }
    }
    
// ----------------------------------------------------------
// CTextToSpeechEngine::DoCancel
// 
// ----------------------------------------------------------
//
void CTextToSpeechEngine::DoCancel()
    {
    TRAP_IGNORE( iServer.StopL() );
    }


// End of File  
