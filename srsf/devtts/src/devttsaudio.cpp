/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Audio playback wrapper for DevTTS.
*
*/


// INCLUDE FILES
#include "devttsaudio.h"
#include "devttscenrep.h"
#include <AudioPreference.h>
#include <AudioOutput.h>
#include <mmfbase.h>
#include "rubydebug.h"

// CONSTANTS
// Sample rates
const TUint KSampleRate8k = 8000;
const TUint KSampleRate11k = 11025;
const TUint KSampleRate16k = 16000;
const TUint KSampleRate22k = 22050;
const TUint KSampleRate32k = 32000;
const TUint KSampleRate44k = 44100;
const TUint KSampleRate48k = 48000;
const TUint KSampleRate88k = 88200;
const TUint KSampleRate96k = 96000;

// Bits per sample
const TUint KBitsPerSample8 = 8;
const TUint KBitsPerSample16 = 16;

// Volume min
const TUint KMinVolume = 0;

const TUint KBalanceMiddle = 100;

// Default audio priority

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDevTTSAudio::CDevTTSAudio
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevTTSAudio::CDevTTSAudio( MDevTTSAudioObserver& aObserver, TInt aPriority ) : 
        iObserver( aObserver ), 
        iDevsound( NULL ),
        iBuffer( NULL ),
        iPriority( aPriority ),
        iBalance( KDevTTSBalanceMiddle ),
        iVolume( KMinVolume )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CDevTTSAudio::ConstructL" );

    // Construct DevSound object
    iDevsound = CMMFDevSound::NewL();
    
#ifndef __WINS__// This is not supported in emulator
    iAudioOutput = CAudioOutput::NewL( *iDevsound );
#endif

    // Handler for the CenRep settings
    CDevTtsCenRep* cenrep = CDevTtsCenRep::NewL();
    CleanupStack::PushL( cenrep ); 
    
    // Set volume to default
    RUBY_DEBUG0( "CDevTTSAudio::ConstructL Set volume" );
    SetVolume( cenrep->Volume( MaxVolume() ) );
    
    CleanupStack::PopAndDestroy( cenrep );

    // Set default playback priority
    SetAudioPriority( KAudioPriorityVoiceDial, 
                      (TDevTTSAudioPreference) KAudioPrefVocosPlayback );
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDevTTSAudio* CDevTTSAudio::NewL( MDevTTSAudioObserver& aObserver, 
                                  TInt aPriority )
    {
    CDevTTSAudio* self = new( ELeave ) CDevTTSAudio( aObserver, aPriority );
 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// -----------------------------------------------------------------------------
// CDevTTSAudio::~CDevTTSAudio
// Destructor
// -----------------------------------------------------------------------------
//
CDevTTSAudio::~CDevTTSAudio()
    {
    delete iAudioOutput; 
    delete iDevsound; 
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::InitializeL
// Initializes audio playback device.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::InitializeL() 
    {
    iDevsound->InitializeL(*(this), EMMFStatePlaying );
    }
    
// -----------------------------------------------------------------------------
// CDevTTSAudio::ConfigL
// Configures audio playback device.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::ConfigL( TUint aSamplingRate, TUint aBitsPerSample, 
                            TBool aStereo, TUint aBufferSize )
    {
    // Resolve configuration
    iDevCap = iDevsound->Config();
    
    // Set the preferred buffer size
    iDevCap.iBufferSize = aBufferSize;
   
    // Resolve sampling rate
    switch ( aSamplingRate )
        {
        case KSampleRate8k:
            iDevCap.iRate = EMMFSampleRate8000Hz;
            break;
            
        case KSampleRate11k:
            iDevCap.iRate = EMMFSampleRate11025Hz;
            break;
            
        case KSampleRate16k:
            iDevCap.iRate = EMMFSampleRate16000Hz;
            break;
            
        case KSampleRate22k:
            iDevCap.iRate = EMMFSampleRate22050Hz;
            break;
            
        case KSampleRate32k:
            iDevCap.iRate = EMMFSampleRate32000Hz;
            break;
            
        case KSampleRate44k:
            iDevCap.iRate = EMMFSampleRate44100Hz;
            break;
            
        case KSampleRate48k:
            iDevCap.iRate = EMMFSampleRate48000Hz;
            break;
            
        case KSampleRate88k:
            iDevCap.iRate = EMMFSampleRate88200Hz;
            break;
            
        case KSampleRate96k:
            iDevCap.iRate = EMMFSampleRate96000Hz;
            break;
            
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    
    // Resolve bits per sample
    switch ( aBitsPerSample )
        {
        case KBitsPerSample8:
            iDevCap.iEncoding = EMMFSoundEncoding8BitPCM;
            break;
            
        case KBitsPerSample16:
            iDevCap.iEncoding = EMMFSoundEncoding16BitPCM;
            break;
            
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    
    // Resolve mono/stereo
    if ( aStereo )
        {
        iDevCap.iChannels = EMMFStereo;
        }
    else
        {
        iDevCap.iChannels = EMMFMono;
        }
    
    // Set DevSound configuration in place
    iDevsound->SetConfigL( iDevCap );
    
    // Set the audio priority settings to default
    iAudioPriority.iPriority = KAudioPriorityVoiceDial;
    iAudioPriority.iPref = ( TMdaPriorityPreference ) KAudioPrefVocosPlayback;
    iAudioPriority.iState = EMMFStatePlaying;
    
    iDevsound->SetPrioritySettings( iAudioPriority );
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::Clear
// No counter part for initialize in DevSound. Function is in this class just
// for future convenience.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::Clear()
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::PlayInitL
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::PlayInitL()
    {
    iDevsound->PlayInitL();
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::PlayData
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::PlayData( TBool aLastBuffer ) 
    {
    iBuffer->SetLastBuffer( aLastBuffer );
    iDevsound->PlayData();
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::SetAudioPriority
// Resolves arguments and forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::SetAudioPriority( TInt aPriority, TDevTTSAudioPreference aPref )
    {
    // Set the priority settings
    iPriority = aPriority;
    iAudioPriority.iPriority = aPriority;
    iAudioPriority.iState = EMMFStatePlaying;
    iAudioPriority.iPref = ( TMdaPriorityPreference ) aPref;
  
    iDevsound->SetPrioritySettings( iAudioPriority );
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::SetAudioOutputL
// 
// -----------------------------------------------------------------------------
// 
void CDevTTSAudio::SetAudioOutputL( TInt aAudioOutput )
    {
    RUBY_DEBUG1( "Setting output to %d", aAudioOutput );
    
    if ( iAudioOutput )
        {
        iAudioOutput->SetAudioOutputL( 
            (CAudioOutput::TAudioOutputPreference)aAudioOutput );
        }
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::Balance
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
TInt CDevTTSAudio::Balance()
    {
    return iBalance;
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::SetBalance
// Checks parameter validity and forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::SetBalance( TInt aBalance )
    {
    TInt right( 0 );
    TInt left( 0 );

    if ( ( aBalance > KDevTTSBalanceRight ) || ( aBalance < KDevTTSBalanceLeft ) )
        {
        return;
        }
    iBalance = aBalance;

    if ( aBalance < 0 )
        {
        left = KBalanceMiddle;
        right = KBalanceMiddle + aBalance;
        }
    else if ( aBalance > 0 )
        {
        left = KBalanceMiddle - aBalance;
        right = KBalanceMiddle;
        }
    else
        {
        left = KBalanceMiddle;
        right = KBalanceMiddle;
        }

    RUBY_DEBUG2( "CDevTTSAudio::SetBalance Balance setting left: %d, right: %d", left, right );

    TRAPD( error, iDevsound->SetPlayBalanceL( left, right ) );
    if ( error == KErrNone )
        {
        RUBY_DEBUG0( "CDevTTSAudio::SetBalance Balance set" );
        }
    else
        {
        RUBY_DEBUG1( "CDevTTSAudio::SetBalance Balance set error: %d", error );
        }
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::SetVolume
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::SetVolume( TInt aVolume )
    {
    if ( ( aVolume > iDevsound->MaxVolume() ) || ( aVolume < KMinVolume ) )
        {
        return;
        }
    
    iVolume = aVolume;
    RUBY_DEBUG1( "CDevTTSAudio::SetVolume %i", aVolume ); 

    iDevsound->SetVolume( aVolume );
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::SetVolumeRamp
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration )
    {
    iDevsound->SetVolumeRamp( aRampDuration );
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::MaxVolume
// Returns DevSound maximum volume.
// -----------------------------------------------------------------------------
//
TInt CDevTTSAudio::MaxVolume()
    {
    return iDevsound->MaxVolume();
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::Volume
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
TInt CDevTTSAudio::Volume()
    {
    return iVolume;
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::Stop
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::Stop()
    {
    iDevsound->Stop();
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::Pause
// Forwards call to DevSound.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::Pause()
    {
    iDevsound->Pause();
    }


// ========================== MDevSoundObserver CALLBACKS ======================

// -----------------------------------------------------------------------------
// CDevTTSAudio::InitializeComplete
// Forwards callback
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::InitializeComplete( TInt aError ) 
    {
    iObserver.MdtaoInitializeComplete( aError );
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::ToneFinished
// Not needed when using only waveform playback
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::ToneFinished(TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::BufferToBeFilled
// Forwards callback
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::BufferToBeFilled( CMMFBuffer* aBuffer )
    {
    iBuffer = aBuffer;
    CMMFDataBuffer* buffer = STATIC_CAST( CMMFDataBuffer*, aBuffer );
    
    iObserver.MdtaoBufferToBeFilled( buffer->Data(), aBuffer->RequestSize() );
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::PlayError
// Forwards callback.
// Spec says that KErrNone is used when audio playback is ok. In practise it
// seems that KErrUnderflow means the same thing.
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::PlayError( TInt aError )
    {
    if ( aError == KErrNone || aError == KErrUnderflow ) 
        {
        iObserver.MdtaoPlayFinished( KErrNone );
        }
    else
        {
        iObserver.MdtaoPlayFinished( aError );
        }
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::BufferToBeEmptied
// Not needed when using only waveform playback
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::BufferToBeEmptied( CMMFBuffer* /*aBuffer*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::RecordError
// Not needed when using only waveform playback
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::RecordError( TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::ConvertError
// Not needed when using only waveform playback
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::ConvertError( TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::DeviceMessage
// Not needed, no custom commands issued to audio hw device
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::DeviceMessage( TUid /*aMessageType*/, const TDesC8& /*aMsg*/ )
    {
    }

// -----------------------------------------------------------------------------
// CDevTTSAudio::SendEventToClient
// Called when an attempt to acquire a sound device is rejected by audio policy 
// server
// -----------------------------------------------------------------------------
//
void CDevTTSAudio::SendEventToClient( const TMMFEvent& /*aEvent*/ )
    {
    // Inform our own observer
    iObserver.MdtaoPlayFinished( KErrCancel );
    }

// End of File  
