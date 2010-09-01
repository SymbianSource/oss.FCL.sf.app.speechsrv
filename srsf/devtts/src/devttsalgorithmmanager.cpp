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
* Description:  Implementations for CTTSAlgorithmManager methods.
*
*/


// INCLUDE FILES
#include <barsc.h>
#include <bautils.h>
#include <barsread.h>
#include <nssdevtts.rsg>
#include <AudioPreference.h>
#include <data_caging_path_literals.hrh>
#include "devttsalgorithmmanager.h"
#include "rubydebug.h"

#ifdef _DEBUG
#include <e32svr.h>
#endif

// CONSTANTS
_LIT( KResourceFileName, "nssdevtts.rsc" );

// Default audio priority
const TInt KDefaultPriority = KAudioPriorityVoiceDial;

#ifdef DEVTTS_AUDIOBUFFER_TO_FILE
const TFileName KAudioBufferFileName( _L( "c:\\documents\\devttsaudio" ) );
const TFileName KAudioBufferFileNameExtension( _L( ".raw" ) );
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::CTTSAlgorithmManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTTSAlgorithmManager::CTTSAlgorithmManager( MDevTTSObserver& aObserver ):
        iPaused( EFalse ),
        iObserver( aObserver ),
        iAudio( NULL ),
        iAudioState( ETTSAudioPlayerNotInitialized ),
        iHw( NULL ),
        iError( KErrNone ),
        iState( EDevTTSNotInitialized ),
        iMode( EDevTTSSoundDeviceMode ),
        iLastBufferFromTts( EFalse ),
        iTtpHw( NULL ), 
#ifdef DEVTTS_3DB_HACK
		iApplyGain( ETrue ),
#endif
        iNlp( NULL ),
        iNlpInitialized( EFalse )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::ConstructL(/*CConsoleBase* console*/)
    {
    RUBY_DEBUG_BLOCK( "CTTSAlgorithmManager::ConstructL" );
    
    TBool boolStereo( EFalse );
    TUint intStereo( 0 );
    TUint bitsPerSample( 16 );
    TUint samplingRate( 16000 );
    TUint bufferSize( 4096 );
    TInt defaultPriority( 0 );
    
    // Try to find resource file first from MMC, then from C-drive and only 
    // after that from flash
    // letters for drives in search order
    const TBuf<2> KResourceDrives = _L("cz"); 
    
    // load resources
    RResourceFile resourceFile;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    TFileName name;
    TInt i( 0 );
    // try to find from the first driver
    name.Append( KResourceDrives[i] );
    name.Append( ':' );
    name.Append( KDC_RESOURCE_FILES_DIR );
    name.Append( KResourceFileName );

    TBool found( EFalse );
    
    while ( !found && i < KResourceDrives.Length() )
        {
        name[0] = KResourceDrives[i++];
       
        if ( BaflUtils::FileExists(fs, name) )
            {
            // open resource
            resourceFile.OpenL( fs, name );
            CleanupClosePushL( resourceFile );
            found = ETrue;
            }
        }
            
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

    HBufC8* configurations = resourceFile.AllocReadLC( R_DEVTTS_CONFIGURATIONS );
    
    TResourceReader resReader;
    resReader.SetBuffer( configurations );
    
    // Sampling rate
    samplingRate = resReader.ReadUint32();

    // Bits per sample
    bitsPerSample = resReader.ReadUint32();
    
    // Mono / Stereo
    intStereo = resReader.ReadUint32();

    // Buffer size
    bufferSize = resReader.ReadUint32();

#ifdef __WINS__
    // CMMFDevSound uses currently buffer size of 16384 in emulator. 
    RUBY_DEBUG0( "CTTSAlgorithmManager::ConstructL - Hard coded buffer size in use!" );
    bufferSize = 16384;
#endif

    // Priority
    defaultPriority = KDefaultPriority;
    
    if ( intStereo == 0 )
        {
        boolStereo = EFalse;
        }
    else
        {
        boolStereo = ETrue;
        }
        
    // Cleanup configurations, resourceFile, fs
    CleanupStack::PopAndDestroy( configurations );    
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( &fs );

    // Create new TtsHwDevice instance
    iHw = CTtsHwDevice::NewL( *this, samplingRate, bufferSize );
    
    // Create audio playback instance
    iAudio = CDevTTSAudio::NewL( *this, defaultPriority );
    
    iState = EDevTTSStopped;
    
    // Start audio device initialization process
    iAudio->InitializeL();

    // Wait for callback only if it is asynchronous
    // (state will be something else if it is synchronous)
    if ( iAudioState == ETTSAudioPlayerNotInitialized )
        {
        if ( !iAudioWait.IsStarted() )
            {
            RUBY_DEBUG0( "CTTSAlgorithmManager::ConstructL iAudioWait.Start()" );
            iAudioWait.Start();
            }
        }
        
    iAudio->ConfigL( samplingRate, bitsPerSample, boolStereo, bufferSize);

    // Create & initialize TtpHwDevice
    iTtpHw = CASRSTtpHwDevice::NewL( *this );
    iTtpHw->InitializeL();
    iTtpHw->SetPhonemeNotationL( _L( "NIPA8" ) );
    
    // Create NlpHwDevice
    TRAPD( error, iNlp = CNlpHwDevice::NewL( *this ) );
    if ( error == KErrNone )
        {
        iNlp->InitializeL();
        iNlpInitialized = ETrue;
        }
    else
        {
        iNlpInitialized = EFalse;
        }

#ifdef DEVTTS_AUDIOBUFFER_TO_FILE
    User::LeaveIfError( iSession.Connect() );
    iFileOpen = EFalse;
    iFileCounter = 0;
#endif

    RUBY_DEBUG1( "CTTSAlgorithmManager config, sampling rate: %d", samplingRate );
    RUBY_DEBUG1( "CTTSAlgorithmManager config, bits per sample: %d", bitsPerSample);
    RUBY_DEBUG1( "CTTSAlgorithmManager config, mono/stereo: %d", intStereo);
    RUBY_DEBUG1( "CTTSAlgorithmManager config, buffer size: %d", bufferSize);
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTTSAlgorithmManager* CTTSAlgorithmManager::NewL( MDevTTSObserver& aObserver/*, CConsoleBase* console*/ )
    {
    CTTSAlgorithmManager* self = new( ELeave ) CTTSAlgorithmManager(aObserver);
    
    CleanupStack::PushL( self );
    self->ConstructL( /*console*/ );
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::~CTTSAlgorithmManager
// Destructor.
// -----------------------------------------------------------------------------
//
CTTSAlgorithmManager::~CTTSAlgorithmManager()
    {
    if ( iAudio )
        {
        iAudio->Clear();
        delete iAudio;
        iAudio = NULL;
        }
    if ( iHw )
        {
        delete iHw;
        iHw = NULL;
        }
    if ( iTtpHw )
        {
        iTtpHw->Clear();
        delete iTtpHw;
        iTtpHw = NULL;
        }
    if ( iNlp )
        {
        iNlp->Clear();
        delete iNlp;
        iNlp = NULL;
        }
#ifdef DEVTTS_AUDIOBUFFER_TO_FILE
    iSession.Close();
#endif
    }


// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::AddStyleL
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
TTtsStyleID CTTSAlgorithmManager::AddStyleL( const TTtsStyle& aStyle )
    {
    return iHw->AddStyleL( aStyle );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::Balance
// Forwards call to the CDevTTSAudio.
// -----------------------------------------------------------------------------
//
TInt CTTSAlgorithmManager::Balance()
    {
    return iAudio->Balance();
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::BufferProcessed
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::BufferProcessed( const TDesC8& aBuffer )
    {
    // Client should call this only when buffers are provided to client
    if ( iMode == EDevTTSClientMode )
        {
        iHw->BufferProcessed( aBuffer );
        }
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::CustomInterface
// Not in use currently. Now returns NULL.
// -----------------------------------------------------------------------------
//
TAny* CTTSAlgorithmManager::CustomInterface( TUid /*aInterfaceID*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::DeleteStyleL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::DeleteStyleL( TTtsStyleID aStyleID )
    {
    iHw->DeleteStyleL( aStyleID );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::GetPositionL
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::GetPositionL( TTimeIntervalMicroSeconds& aTime ) const
    {
    iHw->GetPositionL( aTime );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::GetPositionL
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::GetPositionL( TTtsSegment& aSegment, TInt& aWordIndex ) const
    {
    iHw->GetPositionL( aSegment, aWordIndex );
    }


// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::IsLanguageSupported
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
TBool CTTSAlgorithmManager::IsLanguageSupported( TLanguage aLanguage ) const
    {
    return iHw->IsLanguageSupported( aLanguage );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MaxVolume
// Forwards call to the CDevTTSAudio.
// -----------------------------------------------------------------------------
//
TInt CTTSAlgorithmManager::MaxVolume()
    {
    return iAudio->MaxVolume();
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::NumberOfStyles
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
TUint16 CTTSAlgorithmManager::NumberOfStyles() const
    {
    return iHw->NumberOfStyles();
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::Pause
// Pauses synthesis process in TtsHwDevice and also audio playback if needed.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::Pause()
    {
    // Do nothing if not synthesizing currently
    if ( iState != EDevTTSSynthesizing )
        {
        return;
        }
    
    // Stop synthesis and playback
    iHw->Pause();

    if( iMode == EDevTTSSoundDeviceMode )
        {
        // Do a complete stop to the audio player instead of pause.
        // Pausing and resuming after that leads to the situation where all of
        // the data won't be played...
        iAudio->Stop();
        iAudioState = ETTSAudioPlayerPaused;
        }
    
    iState = EDevTTSPrimed;
    iPaused = ETrue;
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::PrimeSynthesisL
// Prepares a synthesizing process, data will be read from client using a 
// stream.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::PrimeSynthesisL( MTtsSegmentStream& aStream ) 
    {
    // Not yet in use, leave every time
    // @todo: Remove this when functionality is available
    User::Leave( KErrNotSupported );
    
    // Do the state check    
    if ( iState == EDevTTSNotInitialized )
        {
        User::Leave( KErrNotReady );
        }
    
    if ( iState != EDevTTSStopped )
        {
        User::Leave( KErrInUse );
        }
    
    iHw->PrimeSynthesisL( aStream );
    
    iState = EDevTTSPrimed;
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::PrimeSynthesisL
// Prepares a synthesizing process.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::PrimeSynthesisL( CTtsParsedText& aText )
    {
    RUBY_DEBUG_BLOCK( "CTTSAlgorithmManager::PrimeSynthesisL" );

    // Do the state check
    
    if ( iState == EDevTTSNotInitialized )
        {
        User::Leave( KErrNotReady );
        }
    
    if (iState != EDevTTSStopped)
        {
        User::Leave( KErrInUse );
        }
    
    // 1. Analyze structure if no segments in the parsed text object
    if ( aText.NumberOfSegments() == 0 )
        {
        iHw->AnalyzeStructureL( aText );
        }

    // 2. Call NLP if there is written text
    if ( ( aText.Text() != KNullDesC16 ) && iNlpInitialized )
        {
        
        TBool useNlp( ETrue );
        TInt counter( 0 );
        
        // Loop through content types of segments to find out if NLP should be used
        for ( counter = 0; counter < aText.NumberOfSegments(); counter++ )
            {
            TTtsSegment segment = aText.SegmentL( counter );
            TTtsStyleID styleID = segment.StyleID();
            TTtsStyle& style = StyleL( styleID );
            
            if ( !style.iNlp )
                {
                useNlp = EFalse;                
                }
            }
        
        if ( useNlp )
            {
            // If language is not specified, run LID before NLP
            for ( counter = 0; counter < aText.NumberOfSegments(); counter++ )
                {
                TTtsSegment segment = aText.SegmentL( counter );
                TTtsStyleID styleID = segment.StyleID();
                TTtsStyle& style = StyleL( styleID );

                if ( style.iLanguage == ELangOther )
                    {
                    RArray<TLanguage> languages;
                    RArray<TInt> scores;
                
                    // Only one result is needed
                    iTtpHw->LanguageIdentificationL( segment.TextPtr(), 1, languages, scores );

                    if ( languages.Count() == 0 )
                        {
                        languages.Close();
                        scores.Close();
                        User::Leave( KErrGeneral );
                        }

                    style.iLanguage = languages[0];

                    RUBY_DEBUG1( "CTTSAlgorithmManager::PrimeSynthesisL called LID [lang id: %d] before NLP", style.iLanguage );

                    languages.Close();
                    scores.Close();
                    }
                }
            }
            
        if ( useNlp )
            {
            iNlp->NormalizeTextL( aText );
            }
        }

    // Set the phoneme notation which is given within parsed text into TTP
    // Do this only if no phonemesequence is given and if notation is something
    // else than the default
    if ( ( aText.PhonemeNotation() != KNullDesC16 ) && ( aText.PhonemeSequence() == KNullDesC8 ) )
        {
        iTtpHw->SetPhonemeNotationL( aText.PhonemeNotation() );
        }

    // 3. Do the TTP if phoneme sequence not available
    if ( aText.PhonemeSequence() == KNullDesC8 )
        {
        // If also text is NULL, argument is not ok
        if ( aText.Text() == KNullDesC16 )
            {
            User::Leave( KErrArgument );
            }
        
#ifdef __SIND_MULTIPLE_SEGMENTS
        RArray<TInt> segmentPhonemeLengths;
        CleanupClosePushL(segmentPhonemeLengths);
#endif // __SIND_MULTIPLE_SEGMENTS
        // Find the style attached to each segment
        for ( TInt i = 0; i < aText.NumberOfSegments(); i++ )
            {
            TTtsSegment segment = aText.SegmentL( i );
            TTtsStyleID styleID = segment.StyleID();
            TTtsStyle& style = StyleL( styleID );
            
            RUBY_DEBUG1( "CTTSAlgorithmManager::PrimeSynthesisL calling TTP with lang id [%d]", style.iLanguage );

            HBufC8* phonemeBuffer = iTtpHw->ConvertTextL( segment.TextPtr(), style.iLanguage );
            
            if ( phonemeBuffer == NULL )
                {
                RUBY_DEBUG0( "CTTSAlgorithmManager::PrimeSynthesisL TTP returned NULL, calling again with ELangOther" );

                // Try with another approach, use ELangOther as language and let
                // the LID to do the job
                if ( style.iLanguage != ELangOther )
                    {
                    style.iLanguage = ELangOther;
                    phonemeBuffer = iTtpHw->ConvertTextL( segment.TextPtr(), style.iLanguage );
                    RUBY_DEBUG1( "CTTSAlgorithmManager::PrimeSynthesisL LID returned lang id [%d]", style.iLanguage );
                    if ( phonemeBuffer == NULL )
                        {
                        User::Leave( KErrGeneral );
                        }
                    }
                else
                    {
                    User::Leave( KErrGeneral );
                    }
                }

            CleanupStack::PushL( phonemeBuffer );

#ifdef __SIND_MULTIPLE_SEGMENTS
            // Append segment's phone sequence to CTtsparsedText's phoneme sequence
            HBufC8*  phonemeSeq = HBufC8::NewLC( aText.PhonemeSequence().Length() + phonemeBuffer->Des().Length() );
            phonemeSeq->Des().Append( aText.PhonemeSequence() );
            phonemeSeq->Des().Append( phonemeBuffer->Des() );
            // Set new phoneme sequence
            aText.SetPhonemeSequenceL( *phonemeSeq );
            CleanupStack::PopAndDestroy( phonemeSeq );
            
            // Save the length of segment's phoneme sequence
            segmentPhonemeLengths.AppendL( phonemeBuffer->Des().Length() );

#else
            aText.SetPhonemeSequenceL( phonemeBuffer->Des() );
            
            segment.SetPhonemeSequencePtr( aText.PhonemeSequence() );
            // Add modified segment to parsed text
            aText.DeleteSegmentL( i );
            aText.AddSegmentL( segment, i );
#endif // __SIND_MULTIPLE_SEGMENTS
            
            // phonemeBuffer
            CleanupStack::PopAndDestroy( phonemeBuffer );
            }
        
#ifdef __SIND_MULTIPLE_SEGMENTS
        // Put pointers of the whole phone sequence in CTtsParsedText
        // to their respective segments 
        TInt position( 0 );
        for ( TInt i = 0; i < aText.NumberOfSegments(); i++ )
            {
            TTtsSegment segment = aText.SegmentL( i );
            segment.SetPhonemeSequencePtr( aText.PhonemeSequence().Mid( position, segmentPhonemeLengths[i] ) );
            // Add modified segment to parsed text
            aText.DeleteSegmentL( i );
            aText.AddSegmentL( segment, i );
            // Move the position to next segments starting position
            position += segmentPhonemeLengths[i];
            }
        
        // Release resources allocated by RArray    
        CleanupStack::PopAndDestroy( &segmentPhonemeLengths );
#endif // __SIND_MULTIPLE_SEGMENTS
        
        }

#ifdef DEVTTS_3DB_HACK
	iApplyGain = ETrue;
#endif


    // 4. Analyze prosody
    iHw->AnalyzeProsodyL( aText );    
    
    // 5. Prime TtsHwDevice
    iHw->PrimeSynthesisL( aText );
    
    iState = EDevTTSPrimed;
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SetAudioPriority
// Forwards call to the CDevTTSAudio
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SetAudioPriority( TInt aPriority, TDevTTSAudioPreference aPref )
    {
    iAudio->SetAudioPriority( aPriority, aPref );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SetAudioOutputL
// Forwards call to the CDevTTSAudio
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SetAudioOutputL( TInt aAudioOutput )
    {
    iAudio->SetAudioOutputL( aAudioOutput );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SetBalance
// Forwards call to the CDevTTSAudio
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SetBalance( TInt aBalance )
    {
    iAudio->SetBalance( aBalance );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SetPositionL
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SetPositionL( const TTimeIntervalMicroSeconds& aTime )
    {
    iHw->SetPositionL( aTime );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SetPositionL
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SetPositionL( const TTtsSegment& aSegment, 
                                         TInt aWordIndex )
    {
    iHw->SetPositionL( aSegment, aWordIndex );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SetVolume
// Forwards call to the CDevTTSAudio
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SetVolume( TInt aVolume )
    {
    iAudio->SetVolume( aVolume );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SetVolumeRamp
// Forwards call to the CDevTTSAudio
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration )
    {
    iAudio->SetVolumeRamp( aRampDuration );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::Stop
// Stop synthesis process as well as audio playback if running.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::Stop()
    {
    if ( iState == EDevTTSNotInitialized )
        {
        // Just to play it safe, do nothing if not initialized
        return;
        }
    
    if ( iAudioState == ETTSAudioPlayerPlaying )
        {
        iAudio->Stop();
        iAudioState = ETTSAudioPlayerIdle;
        }
    if ( iState == EDevTTSSynthesizing || iState == EDevTTSPrimed )
        {
        iHw->Stop();
        }
    
    iState = EDevTTSStopped;
    
    // Clear paused flag. might be that we are stopping after pausing
    iPaused = EFalse;
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::StyleL
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
TTtsStyle& CTTSAlgorithmManager::StyleL( TTtsStyleID aStyleID ) const
    {
    return iHw->StyleL( aStyleID );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::StyleL
// Forwards call to TtsHwDevice
// -----------------------------------------------------------------------------
//
TTtsStyle& CTTSAlgorithmManager::StyleL( TUint16 aIndex ) const
    {
    return iHw->StyleL(  aIndex );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::SynthesizeL
// Starts or resumes synthesis process. 
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::SynthesizeL( TDevTTSOutputMode aOutputMode ) 
    {
    
    if ( (iState == EDevTTSNotInitialized) || (iState == EDevTTSStopped) )
        {
        User::Leave( KErrNotReady );
        }
    
    if ( iState != EDevTTSPrimed )
        {
        User::Leave( KErrInUse );
        }
    
    // If iPaused flag is set, we know that there is an
    // ongoing synthesis process and we should resume that instead of starting
    // a new one.
    if ( iPaused )
        {
        // Resume synthesis by calling hwdevice
        iHw->SynthesizeL();
        
        if ( iMode == EDevTTSSoundDeviceMode )
            {
            iAudio->PlayInitL();
            
            iAudioState = ETTSAudioPlayerPlaying;
            }
        iState = EDevTTSSynthesizing;
        iPaused = EFalse;
        return;
        }
    
    iMode = aOutputMode;
    
    // Start synthesizing by calling hwdevice
    iHw->SynthesizeL();
    iLastBufferFromTts = EFalse;
    
    if ( iMode == EDevTTSSoundDeviceMode )
        {
        // Check the audio player state
        if ( iAudioState == ETTSAudioPlayerNotInitialized || 
            iAudioState == ETTSAudioPlayerError )
            {
            User::Leave( KErrNotReady );
            }
        
        if ( iAudioState == ETTSAudioPlayerPlaying )
            {
            User::Leave( KErrInUse );
            }
        
        // Initialize audio device so that it is ready for playback
        iAudio->PlayInitL();
        
        iAudioState = ETTSAudioPlayerPlaying;
        }
    
    iState = EDevTTSSynthesizing;

    RUBY_DEBUG0( "CTTSAlgorithmManager::SynthesizeL started" );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::Volume
// Forwards call to the CDevTTSAudio.
// -----------------------------------------------------------------------------
//
TInt CTTSAlgorithmManager::Volume()
    {
    return iAudio->Volume();
    }


// ========================== CALLBACKS FROM DEVTTSAUDIO =======================


// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::InitializeComplete
// Audio has been initialized.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MdtaoInitializeComplete(TInt aError) 
    {
    if ( aError == KErrNone )
        {
        iAudioState = ETTSAudioPlayerIdle;
        RUBY_DEBUG0( "CTTSAlgorithmManager::InitializeComplete ok" );
        }
    else
        {
        iAudioState = ETTSAudioPlayerError;
        RUBY_DEBUG0( "CTTSAlgorithmManager::InitializeComplete ERROR" );
        }
        
    if ( iAudioWait.IsStarted() )
        {
        RUBY_DEBUG0( "DEVTTS iAudioWait.AsyncStop()" );                	
        iAudioWait.AsyncStop();
        }                   
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MdtaoBufferToBeFilled
// Audio player wants a new buffer so that it can be played.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MdtaoBufferToBeFilled( TDes8& aBuffer, 
                                                  TInt aSizeRequested )
    {
    RUBY_DEBUG1( "CTTSAlgorithmManager::MdtaoBufferToBeFilled size of wanted audio buffer: %d bytes", aSizeRequested );
    
    // Sometimes it might happen that we get still one callback even though
    // DevSound is not anymore playing
    if ( iAudioState != ETTSAudioPlayerPlaying )
        {
        RUBY_DEBUG0( "CTTSAlgorithmManager::MdtaoBufferToBeFilled called even though iAudioState != ETTSAudioPlayerPlaying" );
        return;
        }

    const TUint8* buffer = iPlaybackBuffer.Ptr();
    
    if ( iStartPoint >= iStopPoint )
        {
        // Underflow, nothing to be sent to audio out
        aBuffer.FillZ( aSizeRequested );
        aBuffer.SetLength( aSizeRequested );
        
        iAudio->PlayData( iLastBufferFromTts );
        return;
        }
    
    TInt copyLength = 0;
    // Tells if the whole buffer given by TtsHwDevice has been played
    TBool last = EFalse;
    
    if ( ( iStartPoint + aSizeRequested ) >= iStopPoint )
        {
        // We get to the end of the data that needs to be played
        last = ETrue;
        copyLength = iStopPoint - iStartPoint + 1;
        
        // Empty buffer
        aBuffer.FillZ();
        }
    else
        {
        // Fill the whole requested size
        copyLength = aSizeRequested;
        }
    
    // Copy data to audio out buffer
    aBuffer.Copy( &buffer[iStartPoint], copyLength);

#ifdef DEVTTS_3DB_HACK
	if( iApplyGain ) 
	{
		const TInt32 KSqrt2FXP = 0x00002d41L;
		const TInt32 KSqrt2Shift = 13;
		TPtr16 audioData((TUint16*)(aBuffer.Ptr()), copyLength/2, copyLength/2);
		for( int i=0; i<copyLength/2; ++i )
		{
			TInt16 sample = audioData[i];
			TInt32 newvalue = (sample * KSqrt2FXP) >> KSqrt2Shift; 
			if(newvalue > 0x7fffL | newvalue < -0x8000L) 
			{
				
				iApplyGain = EFalse;
				break;
			}
			audioData[i] = newvalue;
		}
	}
#endif
    
    // Notify TtsHwDevice
    if ( last )
        {
        iHw->BufferProcessed( iPlaybackBuffer );
        }
    
    RUBY_DEBUG1( "CTTSAlgorithmManager::MdtaoBufferToBeFilled Audio buffer size filled: %d bytes\n", aBuffer.Length() );
    
    iStartPoint = iStartPoint + copyLength;
    
    iAudio->PlayData( iLastBufferFromTts );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::PlayFinished
// Checks the incoming error code and sets the audio state accordingly.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MdtaoPlayFinished( TInt aError )
    {
    iAudioState = ETTSAudioPlayerIdle;
    
    if ( aError == KErrNone ) 
        {
        // Don't put error code to aError since MthdTtsCompleted can
        // be called with error code before this function is called
        RUBY_DEBUG0( "CTTSAlgorithmManager::MdtaoPlayFinished ok" );
        }
    else
        {
        // Audio is interrupted by something with higher priority.
        // Keep idle state for audio player since application might
        // try again with better luck. (When there is no other apps trying
        // to access audio out.)
        iError = aError;
        
        // Stop TTS Hw device too
        iHw->Stop();
        
        RUBY_DEBUG0( "CTTSAlgorithmManager::MdtaoPlayFinished audio interrupted" );
        }
 
    // Move automatically to stopped state
    iState = EDevTTSStopped;
    
    // Call the client
    iObserver.MdtoEvent( EDevTTSEventComplete, iError );
    }



// ========================== CALLBACKS FROM TtsHwDevice =======================


// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MthdTtsCompleted
// Called by TtsHwDevice when synthesis has completed
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MthdTtsCompleted( TInt aStatus )
    {
    iState = EDevTTSStopped;
    
    // Mark that previous received buffer is the last one
    iLastBufferFromTts = ETrue;
    
    iError = aStatus;
    
    // Do the callback to client, handled by MdtaoPlayFinished if in sound device mode
    if ( iMode == EDevTTSClientMode )
        {
        iObserver.MdtoEvent( EDevTTSEventComplete, aStatus );
        }

#ifdef DEVTTS_AUDIOBUFFER_TO_FILE
    iFile.Close();
    iFileOpen = EFalse;
#endif
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MthdCustomEvent
// Custom event from TtsHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MthdCustomEvent( TInt /*aEvent*/, TInt /*aStatus*/, 
                                           const TDesC8& /*aParameter*/ )
    {
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MthdProcessBuffer
// Prepares buffer for playback or forwards callback to client, depends on the
// operation mode.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MthdProcessBuffer( const TDesC8& aBuffer )
    {
    RUBY_DEBUG0( "CTTSAlgorithmManager::MthdProcessBuffer" );
    
#ifdef DEVTTS_AUDIOBUFFER_TO_FILE
    if ( iFileOpen == EFalse )
        {
        TFileName filename;
        filename.Append( KAudioBufferFileName );
        filename.AppendNum( iFileCounter );
        filename.Append( KAudioBufferFileNameExtension );

        iFile.Replace( iSession, filename, EFileWrite );
        iFileOpen = ETrue;
        iFileCounter++;
        }
    TInt size( 0 );
    iFile.Size( size );
    iFile.Seek( ESeekEnd, size );
    iFile.Write( aBuffer );
#endif

    // Handle buffer internally if in sound device mode, otherwise just 
    // forward the callback to client
    if ( iMode == EDevTTSSoundDeviceMode )
        {
        // Store pointer to buffer
        iPlaybackBuffer.Set( aBuffer );
        // Mark start and stop points for the audio playback
        iStartPoint = 0;
        iStopPoint = aBuffer.Size() - 1;
        }
    else
        {
        iObserver.MdtoProcessBuffer( aBuffer );
        }
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MthdoConfigurationData
// Forwards callback to client.
// -----------------------------------------------------------------------------
//
HBufC8* CTTSAlgorithmManager::MthdoConfigurationData( TUint32 aPackageType, 
                                                      TUint32 aPackageID,
                                                      TUint32 aStartPosition,
                                                      TUint32 aEndPosition )
    {
    return iObserver.MdtoConfigurationData( aPackageType, aPackageID, 
                                            aStartPosition, aEndPosition );
    }


// ====================== CALLBACKS FROM MASRSTtpHwDeviceObserver ==============

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MathdoConfigurationData
// Forwards callback to client.
// -----------------------------------------------------------------------------
//
HBufC8* CTTSAlgorithmManager::MathdoConfigurationData( TUint32 aPackageType, 
                                                       TUint32 aPackageID,
                                                       TUint32 aStartPosition,
                                                       TUint32 aEndPosition )
    {
    return iObserver.MdtoConfigurationData( aPackageType, aPackageID, 
                                            aStartPosition, aEndPosition );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MathdoWordListReady
// Should not be called since we always use synchrnous TTP.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MathdoWordListReady( const TInt /*aError*/ )
    {
    // Do nothing
    }



// ====================== CALLBACKS FROM MNlpHwDeviceObserver ==================


// -----------------------------------------------------------------------------
// MNlpHwDeviceObserver::MnhdNlpCompleted
// Called when asynchronous NLP processing has been completed
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::MnhdoNlpCompleted( TInt /*aStatus*/ )
    {
    // Should not be called currently since NLP is handled synchronously
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MnhdoConfigurationData
// NLP calls when it needs its configuration data to be loaded
// -----------------------------------------------------------------------------
//
HBufC8* CTTSAlgorithmManager::MnhdoConfigurationData( TUint32 aPackageType, 
                                                      TUint32 aPackageID, 
                                                      TUint32 aStartPosition, 
                                                      TUint32 aEndPosition )
    {
    return iObserver.MdtoConfigurationData( aPackageType, aPackageID, 
                                            aStartPosition, aEndPosition );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::MnhdoStyleL
// Called when NLP needs style information
// -----------------------------------------------------------------------------
//
const TTtsStyle& CTTSAlgorithmManager::MnhdoStyleL( TTtsStyleID styleID )
    {
    // Call TtsHwDevice to get the wanted style
    return iHw->StyleL( styleID );
    }


// ====================== New additions for SIND inc3 ==========================


// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::LanguageIdentificationL
// Does language identification.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::LanguageIdentificationL( CTtsParsedText& aText, 
                                                    TInt aNumberOfGuesses, 
                                                    RArray<TLanguage>& aLanguages,
                                                    RArray<TInt>& aScores )
    {
    CheckStateL();

    // Check that at least some text is given
    if ( aText.Text() == KNullDesC16 )
        {
        User::Leave( KErrArgument);
        }
 
    // Call TtpHwDevice
    iTtpHw->LanguageIdentificationL( aText.Text(), aNumberOfGuesses, aLanguages, aScores );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::NormalizeTextL
// Normalizes text using NLPHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::NormalizeTextL( CTtsParsedText& aText )
    {
    if ( iNlp == NULL || !iNlpInitialized )
        {
        User::Leave( KErrNotSupported );
        }

    CheckStateL();

    iNlp->NormalizeTextL( aText );
    }

// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::NormalizeAndSegmentTextL
// Normalizes and segments the text using NLPHwDevice
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::NormalizeAndSegmentTextL( CTtsParsedText& aText )
    {
    if ( iNlp == NULL || !iNlpInitialized )
        {
        User::Leave( KErrNotSupported );
        }

    CheckStateL();

    iNlp->NormalizeAndSegmentTextL( aText );
    }


// ========================== PRIVATE FUNCTIONS ================================


// -----------------------------------------------------------------------------
// CTTSAlgorithmManager::CheckStateL
// Checks that state of DevTTS is idle.
// -----------------------------------------------------------------------------
//
void CTTSAlgorithmManager::CheckStateL()
    {
    if ( iState == EDevTTSNotInitialized )
        {
        User::Leave( KErrNotReady );
        }
    
    if ( iState != EDevTTSStopped )
        {
        User::Leave( KErrInUse );
        }
    }


// End of File
