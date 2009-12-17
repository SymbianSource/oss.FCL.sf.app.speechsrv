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
* Description:  This file contains the implementation of Algorithm Manager,
*               a submodule of DevASR.
*
*/


// INCLUDE FILES
#include <mmfdatabuffer.h>
#include <AudioPreference.h>
#include <nssdevasr.h>
#include "devasrsrsalgorithmmanager.h"
#include "devasrvmalgorithmmanager.h"
#include "rubydebug.h"
#include "devasrcenrep.h"
#include "srsfbldvariant.hrh"

// CONSTANTS
const TInt KPlay = 1;
const TInt KRecord = KPlay + 1;

// Extra error values
const TInt KErrTooEarly = -100; // if speech was started too early
const TInt KErrNoSpeech = -101; // if it was silent 
const TInt KErrTooLong  = -102; // if speech did not stop before timeout
const TInt KErrTooShort = -103; // if speech was too short

// Default priority for recording
const TInt KDefaultPriority = KAudioPriorityVoiceDial;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::CSRSAlgorithmManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSRSAlgorithmManager::CSRSAlgorithmManager( MDevASRObserver& aObserver )
                                           : CActive( CActive::EPriorityStandard ),
                                             iDevASRObserver( &aObserver ),
                                             iDevSound( NULL ),
                                             iCurrentVolume( 0 ),
                                             iCurrentGain( 0 ),
                                             iRecognitionAlgMgr( NULL ),
                                             iVMAlgorithmManager( NULL ),
                                             iStartRecognition( EFalse ),
                                             iPendingRequestSpeechData( EFalse ),
                                             iStartPoint( 0 ),
                                             iStopPoint( 0 ),
                                             iAudioBuffer( NULL ),
                                             iPtr( NULL, 0, 0 ),
                                             iOverallLength( 0 ),
                                             iOverallSampled( 0 ),
                                             iProcessingUtterance( EFalse ),
                                             iEndFlag( 0 ),
                                             iStartFrame( 0 ),
                                             iEndFrame( 0 ),
                                             iFrameLength( 0 ),
                                             iBufferStartPoint( 0 ),
                                             iBufferEndPoint( 0 ),
                                             iDevASRState( EDASRIdle ),
                                             iDevSoundState( ESoundDeviceNotInitialized ),
                                             iRequestFunction( 0 ),
                                             iBufferUnderConstruction( NULL ),
                                             iConstructionPoint( 0 ),
                                             iPreSamplingStarted( EFalse )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::CSRSAlgorithmManager()" );
#ifdef AUDIOBUFFER_TO_FILE
    iFileCreated = EFalse;
#endif

    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::ConstructL()" );

    // Resource handler reads the DevASR resource file
    iResourceHandler = CDevASRResourceHandler::NewL();

    iRecognitionAlgMgr = CRecognitionAlgMgr::NewL( *this );
    iVMAlgorithmManager = CVMAlgorithmManager::NewL( *iDevASRObserver, *this );

    // Queue containing audio buffers
    iAudioBufferQue = new ( ELeave ) TSglQue<CQueItem>( _FOFF( CQueItem, iLink ) );

    // Set default priority settings
    iPrioritySettings.iState = EMMFStateRecording;
    iPrioritySettings.iPriority = KDefaultPriority;
    iPrioritySettings.iPref = ( TMdaPriorityPreference ) KAudioPrefVocosRecog;

    // CenRep utility class
    CDevAsrCenRep* cenRep = CDevAsrCenRep::NewL();
    CleanupStack::PushL( cenRep );
    
    iRecognitionAlgMgr->SetRejection( cenRep->RejectionValue() );
    
    CleanupStack::PopAndDestroy( cenRep );

    CActiveScheduler::Add( this );

    RUBY_DEBUG1( "associated DevSound instance [%x]", iDevSound );
    RUBY_DEBUG1( "associated Recognition AlgMgr instance [%x]", iRecognitionAlgMgr );
    RUBY_DEBUG1( "associated Vocabulary AlgMgr instance [%x]", iRecognitionAlgMgr );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSRSAlgorithmManager* CSRSAlgorithmManager::NewL( MDevASRObserver& aObserver )
    {
    CSRSAlgorithmManager* self = new( ELeave ) CSRSAlgorithmManager( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::~CSRSAlgorithmManager
// Destructor.
// -----------------------------------------------------------------------------
//
CSRSAlgorithmManager::~CSRSAlgorithmManager()
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::~CSRSAlgorithmManager()" );

    //delete iAudioBuffer;
    delete iQueItem;

    // Empty audio queue if there happens to be something
    if ( iAudioBufferQue )
        {
        while ( !iAudioBufferQue->IsEmpty() )
            {
            iQueItem = iAudioBufferQue->First();
            iAudioBufferQue->Remove( *iQueItem );
            delete iQueItem;
            }

        delete iAudioBufferQue;
        }

    delete iBufferUnderConstruction;
    iBufferUnderConstruction = NULL;
    iConstructionPoint = 0;

    delete iDevSound;
    delete iRecognitionAlgMgr;
    delete iVMAlgorithmManager;
    delete iResourceHandler;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::AdaptL
// Adapts vocabulary and models based on correct result.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::AdaptL( const CSIResultSet& aResultSet, 
                                   TInt aResultIndex )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::AdaptL()" );

    // Adapt vocab
    iVMAlgorithmManager->AdaptVocabL( aResultSet, aResultIndex );

    // Find language of correct result
    const CSIResult& iCorrectResult = aResultSet.AtL( aResultIndex );
    TSIGrammarID grammarID = iCorrectResult.GrammarID();
    TSIRuleID ruleID = iCorrectResult.RuleID();
    TSIRuleVariantID ruleVariantID = iCorrectResult.RuleVariantID();
    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;
    CSICompiledGrammar* siGrammar = NULL;
  
    TRAPD( error, GetGrammarL( grammarID, &siActiveGrammar, &siNonActiveGrammar ) );
    User::LeaveIfError( error );

    TInt index = 0;

    if ( siActiveGrammar != NULL )
        {
        siGrammar = siActiveGrammar;
        }
    else if ( siNonActiveGrammar != NULL )
        {
        siGrammar = siNonActiveGrammar;
        }
    else
        {
        User::Leave( KErrNotFound );	
        }
    
    index = siGrammar->Find( ruleID );
    User::LeaveIfError( index );
    CSIRule& rule = siGrammar->AtL( index );

    index = rule.Find( ruleVariantID );
    User::LeaveIfError( index );
    CSIRuleVariant& ruleVariant = rule.AtL( index );

    // Adapt models
    iRecognitionAlgMgr->AdaptModelsL( aResultSet, aResultIndex, ruleVariant.Language() );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::CompileGrammarL
// Forward call to CVMAlgorithmManager.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::CompileGrammarL( CSICompiledGrammar& aGrammar/*,
                                            CSIModelBank& aModelBank*/ )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::CompileGrammarL()" );

    iVMAlgorithmManager->CompileGrammarL( aGrammar/*, aModelBank*/ );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ComputeNewGrammarSizeL
// Forward call to CVMAlgorithmManager.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::ComputeNewGrammarSizeL( const CSIGrammar& aGrammar, 
                                                   const TUint32 aTargetNRuleVariants, 
                                                   const TUint32 aMaxNRuleVariants, 
                                                   const RArray<TUint>& aNewRuleScores, 
                                                   RArray<TUint>& aNNeNRuleVariants, 
                                                   TUint32& aNPrune )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::ComputeNewGrammarSizeL()" );

    iVMAlgorithmManager->ComputeNewGrammarSizeL( aGrammar,
                                                 aTargetNRuleVariants,
                                                 aMaxNRuleVariants,
                                                 aNewRuleScores,
                                                 aNNeNRuleVariants,
                                                 aNPrune );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::PruneGrammar
// Forward call to CVMAlgorithmManager.
// -----------------------------------------------------------------------------
//
TBool CSRSAlgorithmManager::PruneGrammar( const CSIGrammar& aGrammar, 
                                          const TUint32 aMinNumber, 
                                          RArray<TSIRuleVariantInfo>& aPrunedRuleVariants )
    {
    return iVMAlgorithmManager->PruneGrammar( aGrammar, aMinNumber, aPrunedRuleVariants );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::StartTrainingFromTextL
// Forward call to CVMAlgorithmManager.
// -----------------------------------------------------------------------------
// 
void CSRSAlgorithmManager::StartTrainingFromTextL( CSITtpWordList& aWordList, 
                                                   const RArray<TLanguage>& aDefaultLanguage, 
                                                   const RArray<TUint32>& aMaxNPronunsForWord )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::StartTrainingFromTextL()" );

    iVMAlgorithmManager->TrainFromTextL( aWordList, aDefaultLanguage, aMaxNPronunsForWord );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ResolveResult
// Forward call to CVMAlgorithmManager.
// -----------------------------------------------------------------------------
// 
void CSRSAlgorithmManager::ResolveResult( const RArray<TUint>& aNBestIDs,
                                          CSIResultSet& aSIResultSet,
                                          const RPointerArray<CSICompiledGrammar>& aSICompiledGrammar,
                                          const TDesC8& aCombinedData/*,
                                          CSIModelBank& iSIModelBank*/ )
    {
    iVMAlgorithmManager->ResolveResult( aNBestIDs, aSIResultSet, 
                                        aSICompiledGrammar, aCombinedData/*,
                                        iSIModelBank*/ );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::CombineGrammarL
// Forward call to CVMAlgorithmManager.
// -----------------------------------------------------------------------------
// 
void CSRSAlgorithmManager::CombineGrammarL( const RPointerArray<CSICompiledGrammar>& aCompiledGrammars,
                                            const RPointerArray<TSIRuleVariantInfo>& aExcludedRules )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::CombineGrammarL()" );

    iVMAlgorithmManager->CombineGrammarL( aCompiledGrammars, aExcludedRules );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ConfigureSoundDeviceL
// Configure sound device according to resource file parameters
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::ConfigureSoundDeviceL()
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::ConfigureSoundDeviceL()" );

	// Fetch the default configuration from DevSound
    iDevSoundCapabilities = iDevSound->Config();

	// Now, configure it according to resource file parameters
    switch ( iResourceHandler->iSamplingRate )
		{
		case 8000:
			iDevSoundCapabilities.iRate = EMMFSampleRate8000Hz;
			break;
		case 11025:
			iDevSoundCapabilities.iRate = EMMFSampleRate11025Hz;
			break;
		case 16000:
			iDevSoundCapabilities.iRate = EMMFSampleRate16000Hz;
			break;
		case 22050:
			iDevSoundCapabilities.iRate = EMMFSampleRate22050Hz;
			break;
		case 32000:
			iDevSoundCapabilities.iRate = EMMFSampleRate32000Hz;
			break;
		case 44100:
			iDevSoundCapabilities.iRate = EMMFSampleRate44100Hz;
			break;
		case 48000:
			iDevSoundCapabilities.iRate = EMMFSampleRate48000Hz;
			break;
		case 88200:
			iDevSoundCapabilities.iRate = EMMFSampleRate88200Hz;
			break;
		case 96000:
			iDevSoundCapabilities.iRate = EMMFSampleRate96000Hz;
			break;
		default:
			iDevSoundCapabilities.iRate = EMMFSampleRate8000Hz;
			break;
		}

    iDevSoundCapabilities.iBufferSize = iResourceHandler->iBufferSize;

    if ( iResourceHandler->iBitsPerSample == 8 )
        {
        iDevSoundCapabilities.iEncoding = EMMFSoundEncoding8BitPCM;
        }
    else
        {
        iDevSoundCapabilities.iEncoding = EMMFSoundEncoding16BitPCM;
        }

	// Now, configure DevSound according to new parameters.
    iDevSound->SetConfigL( iDevSoundCapabilities );

    RUBY_DEBUG1( "DevSound Max Volume = %d", iDevSound->MaxVolume() );
    RUBY_DEBUG1( "DevSound Max Gain = %d", iDevSound->MaxGain() );

    // In order to avoid truncation, computation must be done this way.
    iCurrentVolume = ( ( iDevSound->MaxVolume() * iResourceHandler->iSpeakerVolume ) + 50 ) / 100;
    iCurrentGain = ( ( iDevSound->MaxGain() * iResourceHandler->iMicrophoneGain ) + 50 ) / 100;

    // The speaker volume is set to the percentage of max volume supported.
    iDevSound->SetVolume( iCurrentVolume );
    // The gain is set to the percentage of max gain supported.
    iDevSound->SetGain( iCurrentGain );

    iDevSoundCapabilities = iDevSound->Config();

    RUBY_DEBUG1( "DevSound Set Volume = %d", iCurrentVolume );
    RUBY_DEBUG1( "DevSound Set Gain = %d", iCurrentGain );
    RUBY_DEBUG1( "DevSound Buffer Size = %d", iDevSoundCapabilities.iBufferSize );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::GetEnginePropertiesL
// Retreive the properties of the underlying speech recognition engine.
// An array of values corresponding to the querried identifiers will be populated.
// The function may leave with KErrArgument.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::GetEnginePropertiesL( const RArray<TInt>& aPropertyId,
                                                 RArray<TInt>& aPropertyValue )
    {
    RUBY_DEBUG_BLOCK( "SRSAlgorithmManager::EnginePropertiesL()" );

    // Check that arrays are ok
    if ( aPropertyId.Count() != aPropertyValue.Count() )
        {
        User::Leave( KErrArgument );
        }

	// Run through the engine properties and fill in known values
    for ( TInt index = 0; index < aPropertyId.Count(); index++)
        {
        switch( aPropertyId[index] )
            {
            case KSamplingRate:
                aPropertyValue[index] = iResourceHandler->iSamplingRate;
                break;

            case KBitsPerSample:
                aPropertyValue[index] = iResourceHandler->iBitsPerSample;
                break;

            default:
                break;

            }
        }
	// Get engine properties from other sources.
    iRecognitionAlgMgr->GetEnginePropertiesL( aPropertyId, aPropertyValue );
    iVMAlgorithmManager->GetEnginePropertiesL( aPropertyId, aPropertyValue );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::InitFrontEnd
// Initializes the recognition front end.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::InitFrontEnd( TRecognizerMode aMode )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::InitFrontEnd()" );

	iProcessingUtterance = EFalse;

    if ( iMode != aMode )
        {
        return;
        }

    StateTransition( EDASRInitRecognition );

    iRecognitionAlgMgr->InitFrontEnd( aMode );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::InitRecognizerBE
// Initializes the recognition back-end. The module responsible for recognition
// function is started as a result.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::InitRecognizerBE( CSIResultSet& aResult )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::InitRecognizerBE()" );

    iRecognitionAlgMgr->InitRecognizerBE( aResult );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::IsGrammarLoaded
// Checks if grammar has been loaded.
// -----------------------------------------------------------------------------
//
TBool CSRSAlgorithmManager::IsGrammarLoaded( TSIGrammarID aGrammarID )
    {
    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;
  
    TRAPD( error, GetGrammarL( aGrammarID, &siActiveGrammar, &siNonActiveGrammar ) );
    if ( error != KErrNone )
        {
        return EFalse;
        }

    if ( siActiveGrammar != NULL )
        {
        return ETrue;
        }
    else if ( siNonActiveGrammar != NULL )
        {
        return ETrue;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::IsGrammarActive
// Checks if grammar is currently active.
// -----------------------------------------------------------------------------
//
TBool CSRSAlgorithmManager::IsGrammarActive( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::IsGrammarActive()" );

    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;
  
    TRAPD( error, GetGrammarL( aGrammarID, &siActiveGrammar, &siNonActiveGrammar ) );
    if ( error != KErrNone )
        {
        return EFalse;
        }

    if ( siActiveGrammar != NULL )
        {
        return ETrue;
        }
    else if ( siNonActiveGrammar != NULL )
        {
        return EFalse;
        }

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadGrammarL
// Load the specified grammar into the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadGrammarL( const CSIGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::LoadGrammarL(si non-compiled)" );

    iRecognitionAlgMgr->LoadGrammarL( aGrammar );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadGrammarL
// Load the specified grammar into the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadGrammarL( const CSICompiledGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::LoadGrammarL(si compiled)" );

    iRecognitionAlgMgr->LoadGrammarL( aGrammar );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadGrammarL
// Unloads the specified grammar from the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::UnloadGrammarL( const CSIGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::UnloadGrammarL(si non-compiled)" );

    iRecognitionAlgMgr->UnloadGrammarL( aGrammar );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::UnloadGrammarL
// Unloads the specified grammar from the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::UnloadGrammarL( const CSICompiledGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::UnloadGrammarL(si compiled)" );

    iRecognitionAlgMgr->UnloadGrammarL( aGrammar );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ActivateGrammarL
// Unload the specified grammar from the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::ActivateGrammarL( const TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::ActivateGrammarL()" );

    iRecognitionAlgMgr->ActivateGrammarL( aGrammarID );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::DeactivateGrammarL
// Unload the specified grammar from the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::DeactivateGrammarL( const TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::DeactivateGrammarL()" );

    iRecognitionAlgMgr->DeactivateGrammarL( aGrammarID );
    }


// -----------------------------------------------------------------------------
// CSRSRecognitionAlgMgr::GetGrammar
// Find grammar based on identifier
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::GetGrammarL( const TSIGrammarID aGrammarID, 
                                        CSICompiledGrammar** aSIActiveGrammar,
                                        CSICompiledGrammar** aSIDeActivatedGrammar )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::GetGrammarL()" );

    iRecognitionAlgMgr->GetGrammarL( aGrammarID, 
                                     aSIActiveGrammar,
                                     aSIDeActivatedGrammar );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::UnloadRule
// Request to unload rule.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::UnloadRule( TSIGrammarID aGrammarID,
                                       TSIRuleID aRuleID )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::UnloadRule()" );

    iRecognitionAlgMgr->UnloadRule( aGrammarID, aRuleID );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadEngineParametersL
// Load the specified recognizer parameter(s).
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadEnginePropertiesL( const RArray<TInt>& aParameterId,
                                                  const RArray<TInt>& aParameterValue )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::LoadEnginePropertiesL()" );

    // Check that parameter ids are valid, settable values
    for ( TInt i = 0; i < aParameterId.Count(); i++ )
        {
        if ( ( aParameterId[i] != KDevASRSendFeatures ) && 
             ( aParameterId[i] != KDevASRAdaptation ) )
            {
            User::Leave( KErrNotSupported );
            }
        }
    iRecognitionAlgMgr->LoadEnginePropertiesL( aParameterId, aParameterValue );
    iVMAlgorithmManager->LoadEnginePropertiesL( aParameterId, aParameterValue );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadLexiconL
// Load the specified lexicion into the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadLexiconL( const CSILexicon& aLexicon )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::LoadLexiconL()" );

    iSILexicon = &aLexicon;

    iRecognitionAlgMgr->LoadLexiconL( aLexicon );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadModelsL
// Load the specified models into the recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadModelsL( const CSIModelBank& aModels )
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::LoadModelsL()" );

    iSIModelBank = &aModels;

    iRecognitionAlgMgr->LoadModelsL( aModels );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::UtteranceData
// Get the recorded audio data accumulated during training.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::GetUtteranceData( TDes8* /*aBuffer*/, 
                                             TTimeIntervalMicroSeconds32& /*aDuration*/ )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::GetUtteranceData()" );
    // Not supported
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::HandlePlay
// Asynchronously handles the play request.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::HandlePlayL()
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::HandlePlayL()" );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::StartRecognition
// Request to begin recording.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::StartRecognition( TTimeIntervalMicroSeconds32 aRecordDuration )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::StartRecognition()" );

    iEndFlag = EFalse;
    iRecordDuration = aRecordDuration;
    iRequestFunction = KRecord;
    Ready( KErrNone );
    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::PreStartSamplingL
// Pre-starts sampling
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::PreStartSamplingL()
    {
    RUBY_DEBUG_BLOCK( "" );
#ifdef __FULLDUPLEX_CHANGE
    InitializeDevSoundL();
    StartDevSoundL();
    iPreSamplingStarted = ETrue;
#endif // __FULLDUPLEX_CHANGE
    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::StopRecognition
// Request to stop recording.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::StopRecognition()
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::StopRecognition()" );

    iEndFlag = ETrue;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::HandleRecord
// Asynchronously handles the recording request.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::HandleRecordL()
    {
    RUBY_DEBUG_BLOCK( "CSRSAlgorithmManager::HandleRecordL()" );

    if ( StateTransition( EDASRSampling ) ) // Attempt to change state
        {
        // Allocate a buffer big enough for recording.

        if ( iRecordDuration.Int() == 0 )
            {
            iOverallLength = 0;
            }
        else
            {
            TInt bytesPerSecond = ( TInt ) ( iResourceHandler->iSamplingRate * ( iResourceHandler->iBitsPerSample / 8 ) );
            iOverallLength = ( TInt) ( iRecordDuration.Int() * 0.000001 * bytesPerSecond );
            }

        iOverallSampled = 0;

        TInt err = KErrNone;


        // Start the devSound only if client does not provide utterance data.
        if ( iMode == ESiRecognition )
            {
#ifdef __FULLDUPLEX_CHANGE
            if ( !iPreSamplingStarted )
                {
                TRAP( err, StartDevSoundL() );
                }
#else            
            iDevSound->SetPrioritySettings( iPrioritySettings );
            TRAP( err, iDevSound->RecordInitL() );
#endif // __FULLDUPLEX_CHANGE

            // Mark that recognition should be started when first buffer arrives
            iStartRecognition = ETrue;
            }
        else
            {
            TRAPD ( error, iRecognitionAlgMgr->StartRecognitionL() );
            if ( error != KErrNone )
                {
                iRecognitionAlgMgr->Cancel();
                StateTransition( EDASRIdle );
                iDevASRObserver->DevASREvent( EDevASRRecord, error );
                }
            }

        if ( err == KErrNone )
            {
            iDevSoundState = ESoundDeviceRecord;
            iDevASRObserver->DevASREvent( EDevASRRecordStarted, KErrNone );
            
            // Empty audio buffer queue if there happens to be some garbage 
            // from previous recognition.
            while ( !iAudioBufferQue->IsEmpty() )
                {
                iQueItem = iAudioBufferQue->First();
                iAudioBufferQue->Remove( *iQueItem );
                delete iQueItem;
                iQueItem = NULL;
                }
            delete iBufferUnderConstruction;
            iBufferUnderConstruction = NULL;
            iConstructionPoint = 0;
            iPendingRequestSpeechData = EFalse;
            }
        else
            {
            RUBY_DEBUG1( "RecordInitL() Error: %d", err );
            iRecognitionAlgMgr->Cancel();
            StateTransition( EDASRIdle );
            iDevASRObserver->DevASREvent( EDevASRRecord, err );
            }
        }
    else // Wrong state to handle this request
        {
        iDevASRObserver->DevASREvent( EDevASRRecord, KErrAsrInvalidState );
        
        RUBY_DEBUG1( "HandleRecord in wrong state: %d", iDevASRState );
        }

    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::StartRecSession
// Signal the start of a recognition sesion.
// -----------------------------------------------------------------------------
//
TInt CSRSAlgorithmManager::StartRecSession( TRecognizerMode aMode )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::StartRecSession()" );

    TInt err( KErrNone );

#ifdef __FULLDUPLEX_CHANGE
    if ( !iPreSamplingStarted )
        {
        TRAP( err, InitializeDevSoundL() );
        }
#else
    TRAP( err, InitializeDevSoundL() );
#endif // __FULLDUPLEX_CHANGE

    if ( err == KErrNone )
        {
        iMode = aMode;

        err = iRecognitionAlgMgr->StartRecSession( aMode );
        if ( err == KErrNone )
            {
            StateTransition( EDASRPreRecognition );
            }
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::EndRecSession
// Signal the end of a recognition sesion.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::EndRecSession()
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::EndRecSession()" );

    iRecognitionAlgMgr->EndRecSession();
    StateTransition( EDASRIdle );
    if ( iDevASRState == EDASRIdle )
        {
	    iSIModelBank = NULL;
		iSILexicon = NULL;
        }
    
    delete iDevSound;
    iDevSound = NULL;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::SendSpeechData
// Gives data buffer to recognizer.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::SendSpeechData( TPtrC8& aBuffer, TBool aEnd )
    {
    if ( iMode == ESiRecognitionSpeechInput )
        {
        iRecognitionAlgMgr->SendSpeechData( aBuffer, aEnd );
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::Cancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::Cancel()
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::Cancel()" );

    iVMAlgorithmManager->Cancel();

    iRecognitionAlgMgr->Cancel();

    if( iDevASRState != EDASRIdle )
    	{
        //StateTransition( EDASRPreRecognition );
        // Force to pre recognition state
        iDevASRState = EDASRPreRecognition;
    	}

    if ( iDevSoundState != ESoundDeviceNotInitialized )
    	{
        RUBY_DEBUG0( "CSRSAlgorithmManager::Cancel(), Stopping DevSound..." );

        if ( iMode == ESiRecognition )
            {
            if ( iDevSound )
                {
                iDevSound->Stop();
                }
            }
		iDevSoundState = ESoundDeviceStopped;
    	}

    CActive::Cancel();
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::DoCancel
// Cancels the current and any on going CActive requests/tasks.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::DoCancel()
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::DoCancel()" );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::EmptyBuffer
// Copies the contents of the received buffer to the holding buffer. Subsequent
// call to EmptyBuffer will append to existing buffer.
// -----------------------------------------------------------------------------
//
TInt CSRSAlgorithmManager::EmptyBuffer( CMMFDataBuffer& aBuffer,
                                        TInt aBufferLength )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::EmptyBuffer(): Len=%d bytes", aBufferLength );

    TInt i = 0;
    TInt status = KErrNone;
    TBool iBufferCreated = EFalse;
#ifdef __WINS__
    TInt KNumberOfBuffers = iResourceHandler->iBuffersInWins;
#else
    TInt KNumberOfBuffers = iResourceHandler->iBuffersInThumb;
#endif

    TDes8& ReceivedData = aBuffer.Data();
    TInt iSize = ReceivedData.Length();

#ifdef __FULLDUPLEX_CHANGE
    // If state is not EDASRSampling, then end immediately
    // Samples will be negelected until real recognition starts
    if ( iDevASRState != EDASRRecognitionSampling )
        {
        iDevSound->RecordData();
        return status;
        }
    else
        {
        // Reset pre-sampling flag
        iPreSamplingStarted = EFalse; 
        }
#endif // __FULLDUPLEX_CHANGE

    // If DevSound has not filled the whole buffer, something 
    // is wrong in the sampling
    if( aBufferLength < ReceivedData.MaxLength() )
        {
        status = KErrEof;
        }

    // iOverallLength checks how much is sampled during all the callbacks
    if ( iOverallLength != 0 )
        {
        if ( iOverallSampled >= iOverallLength )
            {
            status = KErrOverflow;
            }
        }

    TInt silentFrames = iResourceHandler->iSilenceFrames;

    // Create new buffer if previous one has been queued
    if ( iBufferUnderConstruction == NULL )
        {
        iBufferUnderConstruction = new TUint8[ iSize * KNumberOfBuffers ];
        if ( !iBufferUnderConstruction )
            {
            return KErrNoMemory;
            }
        iConstructionPoint = 0;
        }

    // Copy data, put silence into the beginning of buffer if needed
    for ( i = 0; i < iSize; i++ )
        {
        if ( ( iOverallSampled + i ) < silentFrames )
            {
            // Do not copy zeros
            //iBufferUnderConstruction[ iConstructionPoint ] = 0;
            }
        else
            {
            iBufferUnderConstruction[ iConstructionPoint ] = ReceivedData[ i ];
            iConstructionPoint++;
            }
        }

    // Count the number of sampled bytes
    iOverallSampled = iOverallSampled + iSize;

    // If internal buffer overflowed or EOF, mark this the final block of data
    // Mark as the final block also if EndRecord() has been explicitly called
    if ( ( status == KErrOverflow ) || ( status == KErrEof ) || iEndFlag )
        {
        CQueItem* qItem( NULL );
        TRAPD( err, qItem = CQueItem::NewL( &iBufferUnderConstruction[0], iConstructionPoint ) );
        if ( err != KErrNone )
            {
            return err;
            }
        qItem->iFinalBlock = ETrue;
        iEndFlag = EFalse;
        iAudioBufferQue->AddLast( *qItem );
        iBufferUnderConstruction = NULL;
        iConstructionPoint = 0;
        iBufferCreated = ETrue;

        RUBY_DEBUG0( "CSRSAlgorithmManager::EmptyBuffer(): Sampling ended due to timeout!!" );
        }
    else
        {
        // Notify devsound
        iDevSound->RecordData();

        // If next buffer does not fit to the buffer under construction
        // put it to queue and start new when next callback arrives
        if ( iConstructionPoint + iSize > iSize * KNumberOfBuffers )
            {
            // Put data to queue
            CQueItem* qItem( NULL );
            TRAPD( err, qItem = CQueItem::NewL( &iBufferUnderConstruction[0], iConstructionPoint ) );
            if ( err != KErrNone )
                {
                return err;
                }
            qItem->iFinalBlock = EFalse;
            iAudioBufferQue->AddLast( *qItem );
            iBufferCreated = ETrue;
            }
        }

    if ( iBufferCreated )
        {

#ifdef AUDIOBUFFER_TO_FILE
        TInt connectError = iFs.Connect();
        if ( connectError != KErrNone )
            {
            return connectError;
            }
        _LIT( KBufferFileName, "C:\\documents\\devasraudiobuffer.raw" );
        if ( !iFileCreated )
            {
            iBufferDataFile.Replace( iFs, KBufferFileName, EFileWrite );
            iFileCreated = ETrue;
            }
        else
            {
            TInt openError = iBufferDataFile.Open( iFs, KBufferFileName, EFileWrite );
            if ( openError != KErrNone )
                {
                return openError;
                }
            }
        
        TPtr8 ptr( &iBufferUnderConstruction[0], iConstructionPoint, iConstructionPoint );
        TInt pos = 0;
        iBufferDataFile.Seek( ESeekEnd, pos );
        iBufferDataFile.Write( pos, ptr );
        iBufferDataFile.Close();
        iFs.Close();
#endif

        iBufferUnderConstruction = NULL;
        iConstructionPoint = 0;
        
        // Start recognition if it has not been already started
        if ( iStartRecognition )
            {
            TRAP( status, iRecognitionAlgMgr->StartRecognitionL() );
            iStartRecognition = EFalse;
            }
        
        // If RequestSpeechData() gets called when there is no data in queue, we
        // should call it again to deliver the new data to recognizer
        if ( iPendingRequestSpeechData )
            {
            iPendingRequestSpeechData = EFalse;
            RequestSpeechData();
            }
        }
    
    return status;
    }
    
    
// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::FillBuffer
// Copies a portion of the holding buffer to the received buffer.
// -----------------------------------------------------------------------------
//
TInt CSRSAlgorithmManager::FillBuffer( CMMFDataBuffer& /*aBuffer*/, 
                                       TInt /*aBufferLength*/ )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::FillBuffer()" );

    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::SetPrioritySettings
// Store the priority setting for the sound device.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings )
    {
    iPrioritySettings = aPrioritySettings;
    // Always use preference meant for recognition
    iPrioritySettings.iPref = ( TMdaPriorityPreference ) KAudioPrefVocosRecog;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::InitializeDevSoundL
// 
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::InitializeDevSoundL()
    {
    if ( !iDevSound )
        {
        // Instance
        iDevSound = CMMFDevSound::NewL();
        }
    
    // Initialization        
    iDevSound->InitializeL( *this, EMMFStateRecording );
    
    // Wait for initialization callback
    if ( !iAudioWait.IsStarted() )
        {
        iAudioWait.Start();
        }    

    // Configuration
    ConfigureSoundDeviceL();
    }
        
        
// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::StartDevSoundL
// 
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::StartDevSoundL()
    {
    RUBY_DEBUG_BLOCK( "" );
    iDevSound->SetPrioritySettings( iPrioritySettings );
    iDevSound->RecordInitL();
    iDevSoundState = ESoundDeviceRecord;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::StateTransition
// Change to the specified state if possible. If state change is invalid,
// EFalse is returned, otherwise, ETrue.
// -----------------------------------------------------------------------------
//
TBool CSRSAlgorithmManager::StateTransition( TDevASRState aTargetState )
    {

    TBool result = ETrue;

#ifdef _DEBUG
    DevASRState();
#endif

    switch ( aTargetState )
        {
        case EDASRIdle:
            iDevASRState = aTargetState;
            break;

        case EDASRInitTrain:
            if ( iDevASRState == EDASRIdle )
                {
                iDevASRState = aTargetState;
                }
            else
                {
                result = EFalse;
                }
            break;

        case EDASRTrain:
            if ( ( iDevASRState == EDASRTrain ) ||
                ( iDevASRState == EDASRInitTrain ) ||
                ( iDevASRState == EDASRTrainSampling ) )
                {
                iDevASRState = aTargetState;
                }
            else
                {
                result = EFalse;
                }
            break;

        case EDASRSampling:
            if ( iDevASRState == EDASRTrain )
                {
                iDevASRState = EDASRTrainSampling;
                }
            else if ( iDevASRState == EDASRRecognition )
                {
                iDevASRState = EDASRRecognitionSampling;
                }
            else
                {
                result = EFalse;
                }
            break;

        case EDASRPlayback:
            if ( iDevASRState == EDASRIdle )
                {
                iDevASRState = EDASRPlayback;
                }
            else if ( iDevASRState == EDASRPreRecognition )
                {
                iDevASRState = EDASRRecognitionPlayback;
                }
            else
                {
                result = EFalse;
                }
            break;

        case EDASRPreRecognition:
            if ( ( iDevASRState == EDASRIdle ) ||
                ( iDevASRState == EDASRRecognition ) ||
                ( iDevASRState == EDASRRecognitionPlayback ) )
                {
                iDevASRState = aTargetState;
                }
            else
                {
                result = EFalse;
                }
            break;

        case EDASRRecognitionPlayback:
            if (iDevASRState == EDASRPreRecognition)
                {
                iDevASRState = aTargetState;
                }
            else
                {
                result=EFalse;
                }
            break;

        case EDASRInitRecognition:
            if ((iDevASRState == EDASRInitRecognition) ||
                (iDevASRState == EDASRPreRecognition))
                {
                iDevASRState = aTargetState;
                }
            else
                {
                result=EFalse;
                }
            break;

        case EDASRRecognition:
            if ( ( iDevASRState == EDASRInitRecognition ) ||
                ( iDevASRState == EDASRRecognitionSampling ) )
                {
                iDevASRState = aTargetState;
                }
            else
                {
                result = EFalse;
                }
            break;

        case EDASRCancel:
            break;

        default:
            RUBY_DEBUG1( "Invalid state: %d", aTargetState );
            break;
        };

#ifdef _DEBUG
    DevASRState();
    iRecognitionAlgMgr->AlgorithmState();
#endif
    return result;
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::TranslateError
// Translate internal errors to system errors
// -----------------------------------------------------------------------------
//
TInt CSRSAlgorithmManager::TranslateError( TInt aError )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::TranslateError(%d)", aError );

    TInt error = aError;

     switch ( aError )
        {
        case KErrTooEarly:
            error = KErrAsrSpeechTooEarly;
            break;
        case KErrNoSpeech:
            error = KErrAsrNoSpeech;
            break;
        case KErrTooLong:
            error = KErrAsrSpeechTooLong;
            break;
        case KErrTooShort:
            error = KErrAsrSpeechTooShort;
            break;
        default:
            RUBY_DEBUG1( "Unmapped Error: %d", aError );
            break;
        };

    return error;
    }


// -----------------------------------------------------------------------------
// DevSound Observer MIXIN implementation begins
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::InitializeComplete
// Initialization of DevSound is complete.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::InitializeComplete( TInt aError )
    {
    RUBY_DEBUG1( "InitializeComplete[%d]", aError );

    if ( aError == KErrNone  )
        {
		iDevSoundState = ESoundDeviceInitialized;
		}
		
    // Free wait loop
    if ( iAudioWait.IsStarted() )
        {
        iAudioWait.AsyncStop();
        }                       
    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ToneFinished
// Finished playing tones
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::ToneFinished( TInt /*aError*/ )
    {
    // Should never be called because no tone services is required.
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::BufferToBeFilled
// Buffer from DevSound to be filled with audio data.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::BufferToBeFilled( CMMFBuffer* aBuffer )
    {
    CMMFDataBuffer* buffer = STATIC_CAST( CMMFDataBuffer*, aBuffer );

    if ( FillBuffer( *buffer, aBuffer->RequestSize() ) == KErrNone )
        {
        iDevSound->PlayData();
        }
    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::PlayError
// Playing of an audio sample has completed successfully or otherwise.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::PlayError( TInt aError )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::PlayError[%d]", aError );

    if ( iDevASRState != EDASRIdle )
		{

		if ( iDevASRState == EDASRRecognitionPlayback )
			{
			StateTransition( EDASRPreRecognition );
			}
		else
			{
			StateTransition( EDASRIdle );
			}

		if ( aError == KErrUnderflow )
			{
			iDevASRObserver->DevASREvent( EDevASRPlay, KErrNone );
			}
		else
			{
			iDevASRObserver->DevASREvent( EDevASRPlay, aError );
			}

		}

    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::BufferToBeEmptied
// An audio buffer is ready to be emptied.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::BufferToBeEmptied( CMMFBuffer* aBuffer )
    {
    RUBY_DEBUG0( "" );    
    if ( iDevASRState == EDASRIdle && !iPreSamplingStarted )
        {
        RUBY_DEBUG0( "stopping devsound" );
        iDevSound->Stop();
		iDevSoundState = ESoundDeviceStopped;
        return;
        }

    RUBY_DEBUG0( "processing callback" );
    CMMFDataBuffer* buffer = STATIC_CAST( CMMFDataBuffer*, aBuffer );
	EmptyBuffer( *buffer, aBuffer->RequestSize() );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::RecordError
// Recording has completed successfully or otherwise.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::RecordError( TInt aError )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::RecordError[%d]", aError );

    iPreSamplingStarted = EFalse;
    iRecognitionAlgMgr->Cancel();
    StateTransition( EDASRIdle );
    iDevASRObserver->DevASREvent( EDevASRRecord, aError );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ConvertError
// Conversion has completed successfully or otherwise.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::ConvertError( TInt /*aError*/ )
    {
    // This method should never be called.
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::RecordError
// Callback for custom commands
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::DeviceMessage( TUid /*aMessageType*/, 
                                          const TDesC8& /*aMsg*/ )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::DeviceMessage" );
    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::SendEventToClient
// Receives and event from DevSound send it to the appropriate handler.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::SendEventToClient( const TMMFEvent& aEvent )
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::SendEventToClient()" );

	if ( aEvent.iErrorCode != KErrNone )
		{
		if ( iDevSoundState == ESoundDeviceRecord )
			{
			RecordError( aEvent.iErrorCode );
			}
		else if ( iDevSoundState == ESoundDevicePlayback )
			{
			PlayError( aEvent.iErrorCode );
			}
		}

    }


// -----------------------------------------------------------------------------
// DevSound Observer MIXIN impl ends
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// RecognitionAlgMgr Observer MIXIN implementation begins
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::AdaptComplete
// Model adaptation has finished.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::AdaptComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::AdaptComplete(%d)", aResultCode );

    iDevASRObserver->DevASREvent( EDevASRAdapt, aResultCode );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::InitFEComplete
// Initialization of frontend algorithm has completed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::InitFEComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::InitFEComplete(%d)", aResultCode );

    // Ignore events if we're idle.
    if ( ( iDevASRState == EDASRInitTrain ) || ( iDevASRState == EDASRInitRecognition ) )
        {
        if ( aResultCode == KErrNone )
            {
            if ( iDevASRState == EDASRInitTrain )
                {
                iDevASRState = EDASRTrain;
                }
            else if ( iDevASRState == EDASRInitRecognition )
                {
                iDevASRState = EDASRRecognition;
                }
            iDevASRObserver->DevASREvent( EDevASRInitFrontend, aResultCode );
            }
        else
            {
            StateTransition( EDASRIdle );
            iDevASRObserver->DevASREvent( EDevASRInitFrontend, TranslateError( aResultCode ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::InitRecognizerBEComplete
// Initialization of backend algorithm for recognition has completed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::InitRecognizerBEComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::InitRecognizerBEComplete(%d)", aResultCode );

    if ( StateTransition( EDASRInitRecognition ) )
        {
        if ( aResultCode == KErrNone )
            {
            iDevASRObserver->DevASREvent( EDevASRInitRecognitionBackend, aResultCode );
            }
        else
            {
            StateTransition( EDASRIdle );
            iDevASRObserver->DevASREvent( EDevASRInitRecognitionBackend, TranslateError( aResultCode ) );
            }
        }

    }

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadGrammarComplete
// Loading of grammar into recognizer has completed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadGrammarComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::LoadGrammarComplete(%d)", aResultCode );

	TDevASREvent event = EDevASRLoadGrammar;
    
    if ( aResultCode == KErrNone )
        {
        iDevASRObserver->DevASREvent( event, aResultCode );
        }
    else
        {
        //StateTransition( EDASRIdle );
        iDevASRObserver->DevASREvent( event, TranslateError( aResultCode ) );
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::UnloadGrammarComplete
// Grammar has beed unloaded.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::UnloadGrammarComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::UnloadGrammarComplete(%d)", aResultCode );

	TDevASREvent event = EDevASRUnloadGrammar;
    
    if ( aResultCode == KErrNone )
        {
        iDevASRObserver->DevASREvent( event, aResultCode );
        }
    else
        {
        iDevASRObserver->DevASREvent( event, TranslateError( aResultCode ) );
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::ActivateGrammarComplete
// Grammar activation has been completed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::ActivateGrammarComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::ActivateGrammarComplete(%d)", aResultCode );

    if ( iDevASRState == EDASRPreRecognition )
        {
        iDevASRObserver->DevASREvent( EDevASRActivateGrammar, aResultCode );
        }  
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::DeActivateGrammarComplete
// Grammar deactivation has been completed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::DeActivateGrammarComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::DeActivateGrammarComplete(%d)", aResultCode );

    if ( iDevASRState == EDASRPreRecognition )
        {
        iDevASRObserver->DevASREvent( EDevASRDeactivateGrammar, aResultCode );
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadLexiconComplete
// Loading of lexicon into recognizer has completed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadLexiconComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::LoadLexiconComplete(%d)", aResultCode );
    
    if ( aResultCode == KErrNone )
        {
        iDevASRObserver->DevASREvent( EDevASRLoadLexicon, aResultCode );
        }
    else
        {
        //StateTransition( EDASRIdle );
        iDevASRObserver->DevASREvent( EDevASRLoadLexicon, TranslateError( aResultCode ) );
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::LoadModelsComplete
// Loading of models into recognizer has completed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::LoadModelsComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::LoadModelsComplete(%d)", aResultCode );
    
    if ( aResultCode == KErrNone )
        {
        iDevASRObserver->DevASREvent( EDevASRLoadModels, aResultCode );
        }
    else
        {
        //StateTransition( EDASRIdle );
        iDevASRObserver->DevASREvent( EDevASRLoadModels, TranslateError( aResultCode ) );
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::RequestSpeechData
// Speech data is needed.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::RequestSpeechData()
    {
    RUBY_DEBUG0( "CSRSAlgorithmManager::RequestSpeechData()" );

    if ( iMode == ESiRecognitionSpeechInput )
        {
        iDevASRObserver->RequestSpeechData();
        }
    else
        {
        // The processed sample descriptor can be deleted
        delete iQueItem; 
        iQueItem = NULL;
        
        if ( ( iDevASRState == EDASRTrainSampling ) || ( iDevASRState == EDASRRecognitionSampling ) )
            {
            // Check if there's more data to send and if so, send the next block of data
            if ( iAudioBufferQue->IsEmpty() )
                {
                iPendingRequestSpeechData = ETrue;
                iProcessingUtterance = EFalse;
                }
            else
                {
                iQueItem = iAudioBufferQue->First();
                iRecognitionAlgMgr->SendSpeechData( iQueItem->iDataBlock, iQueItem->iFinalBlock );
                iAudioBufferQue->Remove( *iQueItem );
                }        
            }
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::FeatureVectorDataRcvd
// Feature vector data received from frontend.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::FeatureVectorDataRcvd( const TDesC8& aFV,
                                                  TInt32 aSNR,
                                                  TInt32 aPosition )
    {
    iDevASRObserver->FeatureVectorDataRcvd( aFV, aSNR, aPosition );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::EouDetected
// Frontend finished extracting feature vectors from PCM data.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::EouDetected( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::EouDetected(%d)", aResultCode );

    if ( iDevASRState == EDASRTrainSampling )
        {
        
        if ( aResultCode != KErrNone )
            {
            RUBY_DEBUG1( "CSRSAlgorithmManager::EouDetected(): Training ERROR %d", aResultCode );

            StateTransition( EDASRIdle );
            iDevSound->Stop();
            iDevSoundState = ESoundDeviceStopped;
            iDevASRObserver->DevASREvent( EDevASRTrain, TranslateError( aResultCode ) );
            return;
            }
        
        // If we fail to get the utterance duration, there's probably not enough frames to calculate
        // utterance duration. We will fail training with "too short".
        if ( !iRecognitionAlgMgr->GetUtteranceDuration(iStartFrame, iEndFrame, iFrameLength) )
            {
            StateTransition(EDASRIdle);
            iDevSound->Stop();
            iDevSoundState = ESoundDeviceStopped;
            iDevASRObserver->DevASREvent( EDevASRTrain, KErrAsrSpeechTooShort );
            return;
            }
        
        RUBY_DEBUG1( "Start frame: %d", iStartFrame );
        RUBY_DEBUG1( "End frame: %d", iEndFrame );
        RUBY_DEBUG1( "Frame Length: %f", iFrameLength );

        // Now, calculate the start and end point in the audio buffer for encoding
        
        RUBY_DEBUG1( "Padding start frames: %d", iResourceHandler->iPadStartFrames );

        // Pad additional frame to start and end to prevent clipping during utterance playback.
        
        if( (iStartFrame - iResourceHandler->iPadStartFrames) <= 0 )
            {
            iStartFrame = 0;
            }
        else
            {
            iStartFrame = iStartFrame - iResourceHandler->iPadStartFrames;
            }
        
        iBufferStartPoint = (TUint32)(iStartFrame * iFrameLength * iResourceHandler->iSamplingRate * 2);
        
        RUBY_DEBUG1( "Padding start frames: %d", iResourceHandler->iPadEndFrames );

        iBufferEndPoint = (TUint32)((iEndFrame + iResourceHandler->iPadEndFrames) * iFrameLength *
            iResourceHandler->iSamplingRate * 2);
        
        if ( iBufferEndPoint >= iStartPoint - 1 ) // Do not pad beyond recorded point
            {
            iBufferEndPoint = iStartPoint - 1; // Set to the last byte recorded
            }
        
        if ( iBufferEndPoint > iStopPoint ) // Cannot pad more than buffer size allows
            {
            iBufferEndPoint = iStopPoint;
            }
        
        StateTransition(EDASRTrain);
        iDevSound->Stop();
        iDevSoundState = ESoundDeviceStopped;
        }
    
    // It is possible that the frontend has completed processing while the audio
    // buffer is not yet full.
    // The recording can be stopped as the frontend has detected end of utterance.
    
    else if ( iDevASRState == EDASRRecognitionSampling )
        {
        
        if ( ( aResultCode != KErrNone ) && ( aResultCode != KErrTooLong ) )
            {
            RUBY_DEBUG1( "CSRSAlgorithmManager::EouDetected(): Recognition ERROR %d", aResultCode );

            StateTransition( EDASRIdle );
            iRecognitionAlgMgr->Cancel();
            iDevSound->Stop();
            iDevASRObserver->DevASREvent( EDevASRRecognize, TranslateError( aResultCode ) );
            return;
            }
        
        StateTransition( EDASRRecognition );
      
        iDevSound->Stop();
       
        iDevSoundState = ESoundDeviceStopped;
        }
    else // We are in the wrong state to handle this event
        {
        RUBY_DEBUG0( "CSRSAlgorithmManager::EouDetected(): WRONG STATE" );
        return;
        }
    
    iDevASRObserver->DevASREvent( EDevASREouDetected, KErrNone );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::TrainComplete
// The training backend has finished processing feature vectors. A Model is
// created.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::TrainComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::TrainComplete(%d)", aResultCode );

    if ( iDevASRState == EDASRTrain )
        {
        // Training is complete, DevASR can return to Idle state.
        StateTransition( EDASRIdle );

        iDevASRObserver->DevASREvent( EDevASRTrain, TranslateError( aResultCode ) );
        }
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::UnloadRuleComplete
// Rule has been blacklisted successfully or otherwise.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::UnloadRuleComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::UnloadRuleComplete(%d)", aResultCode );
    
    iDevASRObserver->DevASREvent( EDevASRUnloadRule, aResultCode );
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::RecognitionComplete
// The recognition backend has finished processing feature vectors. A recognition
// result is created.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::RecognitionComplete( TInt aResultCode )
    {
    RUBY_DEBUG1( "CSRSAlgorithmManager::RecognitionComplete(%d)", aResultCode );

    // Recognition is complete, DevASR can return to pre-recognition state
    if ( StateTransition( EDASRPreRecognition ) )
        {
        // Mark that sampling can be ended if not stopped yet
        iEndFlag = ETrue;
		iDevASRObserver->DevASREvent( EDevASRRecognize, TranslateError( aResultCode ) );
        }
    else
        {
        if ( iDevASRState == EDASRRecognitionSampling )
            {
            StateTransition( EDASRRecognition );
            iDevSound->Stop();
			iDevSoundState = ESoundDeviceStopped;
            iDevASRObserver->DevASREvent( EDevASREouDetected, KErrNone );
            StateTransition( EDASRPreRecognition );
            iDevASRObserver->DevASREvent( EDevASRRecognize, TranslateError( aResultCode ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// RecAlgMgr Observer MIXIN impl ends
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// MVmAlgMgrObserver MIXIN impl starts
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::CombineComplete
// Grammar combination is ready.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::CombineComplete( HBufC8* aResult, TInt aError )
    {
    // Forward call to reco alg manager
    iRecognitionAlgMgr->CombineComplete( aResult, aError );
    }


// -----------------------------------------------------------------------------
// MVmAlgMgrObserver MIXIN impl ends
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Active object implementation begins
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::RunL
// Handle the requested function asynchronously.
// result is created.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::RunL()
    {

    switch ( iRequestFunction )
        {
        case KRecord:
            TRAPD( err, HandleRecordL() );
            if ( err )
                {
                iRecognitionAlgMgr->Cancel();
                StateTransition( EDASRIdle );
                iDevASRObserver->DevASREvent( EDevASRRecord, TranslateError( err ) );
                }
            break;

        case KPlay:

            TRAPD( err1, HandlePlayL() );
            if ( err1 )
                {
                StateTransition( EDASRIdle );
                iDevASRObserver->DevASREvent( EDevASRPlay, TranslateError( err1 ) );
                }
            break;

        default:
            break;
        };
    }


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::Ready
// Utility function to post a request complete.
// result is created.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::Ready( const TInt aStatus )
    {
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, aStatus );
    SetActive();
    }


// -----------------------------------------------------------------------------
// Active object impl ends
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CSRSAlgorithmManager::DevASRState
// Utility function print out the state of DevASR for unit testing.
// result is created.
// -----------------------------------------------------------------------------
//
void CSRSAlgorithmManager::DevASRState()
    {

#ifdef _DEBUG
    switch ( iDevASRState )
        {
        case EDASRIdle:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRIdle", iDevASRState );
            break;

        case EDASRInitTrain:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRInitTrain", iDevASRState );
            break;

        case EDASRTrain:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRTrain", iDevASRState );
            break;

        case EDASRRecognitionSampling:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRRecognitionSampling", iDevASRState );
            break;

        case EDASRTrainSampling:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRTrainSampling", iDevASRState );
            break;

        case EDASRPlayback:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRPlayback", iDevASRState );
            break;

        case EDASRPreRecognition:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRPreRecognition", iDevASRState );
            break;

        case EDASRRecognitionPlayback:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRRecognitionPlayback", iDevASRState );
            break;

        case EDASRInitRecognition:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRInitRecognition", iDevASRState );
            break;

        case EDASRRecognition:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRRecognition", iDevASRState );
            break;

        case EDASRCancel:
            RUBY_DEBUG1( "DevASR is: [%d] - EDASRCancel", iDevASRState );
        default:
            RUBY_DEBUG1( "DevASR is: [%d] - Unknown", iDevASRState );
            break;
        };
#endif

    }
