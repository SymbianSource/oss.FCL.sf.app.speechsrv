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
* Description:  This file contains definition of the DevASR private interface.
*
*/


#ifndef DEVASRSRSALGORITHMMANAGER_H
#define DEVASRSRSALGORITHMMANAGER_H

//#define AUDIOBUFFER_TO_FILE

//  INCLUDES
#include <sounddevice.h>
#include <nsssispeechrecognitiondatadevasr.h>
#include <nssdevasrcommon.h>
#include "devasrutil.h"
#include "devasrrecognitionalgmgr.h"
#include "devasrresourcehandler.h"
#include "devasrvmalgorithmmanager.h"


#ifdef AUDIOBUFFER_TO_FILE
#include <f32file.h>
#endif

// DATA TYPES

// Possible DevASR states
enum TDevASRState
    {
    EDASRIdle,
    EDASRInitTrain,
    EDASRTrain,
    EDASRSampling,
    EDASRTrainSampling,
    EDASRPlayback,
    EDASRRecognitionPlayback,
    EDASRPreRecognition,
    EDASRInitRecognition,
    EDASRRecognition,
    EDASRRecognitionSampling,
    EDASRCancel
    };

// Possible DevSound States
enum TDevSoundState
    {
    ESoundDeviceNotInitialized,
    ESoundDeviceInitialized,
    ESoundDeviceRecord,
    ESoundDevicePlayback,
    ESoundDeviceStopped
    };

// FORWARD DECLARATIONS
class MDevASRObserver;
class CRecognitionAlgMgr;

// CLASS DECLARATION

/**
*  Interface Class to be used by the DevASR thin client implementation.
*
*  @lib NssDevASR.lib
*/
class CSRSAlgorithmManager : public CActive, 
                             public MDevSoundObserver, 
                             public MRecAlgMgrObserver,
                             public MVmAlgMgrObserver
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructors.
        */
        static CSRSAlgorithmManager* NewL(MDevASRObserver& aObserver);
        static CSRSAlgorithmManager* NewLC(MDevASRObserver& aObserver);
        
        /**
        * Destructor.
        */
        virtual ~CSRSAlgorithmManager();
        
    public: // New functions
            

        /**
        * Does grammar compilation.
        */
        void CompileGrammarL( CSICompiledGrammar& aGrammar );

        /**
        * Computes new grammar size with given number of variants.
        */
        void ComputeNewGrammarSizeL( const CSIGrammar& aGrammar, 
                                     const TUint32 aTargetNRuleVariants, 
                                     const TUint32 aMaxNRuleVariants, 
                                     const RArray<TUint>& aNewRuleScores, 
                                     RArray<TUint>& aNNeNRuleVariants, 
                                     TUint32& aNPrune );

        /**
        * Prunes unnecessay rule variants away from grammar.
        */
        TBool PruneGrammar( const CSIGrammar& aGrammar, 
                            const TUint32 aMinNumber, 
                            RArray<TSIRuleVariantInfo>& aPrunedRuleVariants );

        /**
        * Starts speaker independent training from text.
        */  
        void StartTrainingFromTextL( CSITtpWordList& aWordList, 
                                     const RArray<TLanguage>& aDefaultLanguage, 
                                     const RArray<TUint32>& aMaxNPronunsForWord );



        /**
        * Adapts recognition models
        *
        * @param "CSIResultSet& aResultSet" Result set
        * @param "TInt aResultIndex" Index of correct result
        */
        void AdaptL( const CSIResultSet& aResultSet, TInt aResultIndex );

        /**
        * Cancels the current or started tasks.
        */
        void Cancel();
        
        /**
        * This method is used to indicate the end of a recognition session.
        */
        void EndRecSession();
        
        /**
        * Retreive the properties of the underlying speech recognition engine.
        *
        * @param "RArray<TInt>& aPropertyId" An array of identifiers being 
        *         querried.
        * @param "RArray<TInt>& aPropertyValue" An array of values corresponding 
        *        to the querried identifiers
        */
        void GetEnginePropertiesL( const RArray<TInt>& aPropertyId,
                                   RArray<TInt>& aPropertyValue );
        
        /**
        * Initializes the front-end module in the speech recognition engine.
        * The frontend module used during training/recognition functions is started
        * as a result. This method is intended to be used in conjunction with InitTrainBE().
        * 
        * @param "TRecognizerMode aFeMode" Mode of the recognizer.
        */
        void InitFrontEnd( TRecognizerMode aFeMode );
    
        /**
        * Initialize the recognition engine back-end. The module responsible for recognition
        * function is started as a result. This method must be used before any recognition
        * operations and intended to be used in conjunction with InitFrontEnd().
        * 
        * @param "CSIResultSet& aResult" A reference to an object where the 
        *        recognition result will be written
        */
        void InitRecognizerBE( CSIResultSet& aResult );
        
        /**
        * Load the specified recognizer parameter(s). These parameters are used to alter
        * the recognizer's default parameters. The parameters are specified as attribute-value
        * pairs.
        * 
        * @param "RArray<TInt>& aParameterId" An array of parameter identifiers.
        * @param "RArray<TInt>& aParameterValue" An array of parameter values.
        */
        void LoadEnginePropertiesL( const RArray<TInt>& aParameterId,
                                    const RArray<TInt>& aParameterValue );
        
        /**
        * Load the specified grammar into the recognizer.
        * 
        * @param "aGrammar" A reference to a grammar in an internal format.
        */
        void LoadGrammarL( const CSIGrammar& aGrammar );
        void LoadGrammarL( const CSICompiledGrammar& aGrammar );

        /**
        * Unloads the specified grammar from the recognizer.
        * 
        * @param "aGrammar" A reference to a grammar in an internal format.
        */
        void UnloadGrammarL( const CSIGrammar& aGrammar );
        void UnloadGrammarL( const CSICompiledGrammar& aGrammar );

        /**
        * Activates a grammar
        *
        * @param "TSIGrammarID aGrammarID" A grammar identifier.
        */
        void ActivateGrammarL( TSIGrammarID aGrammarID );
        
        /**
        * Deactivates a grammar
        *
        * @param "TSIGrammarID aGrammarID" A grammar identifier.
        */
        void DeactivateGrammarL( TSIGrammarID aGrammarID );
        
        /**
        * Load the specified lexicon into the recognizer.
        * 
        * @param "aLexicon" A reference to a lexicon.
        */
        void LoadLexiconL( const CSILexicon& aLexicon );
        
        /**
        * Load the specified models into the recognizer.
        * 
        * @param "aModels" A reference to a model bank.
        */
        void LoadModelsL( const CSIModelBank& aModels );
        
        /**
        * Request to begin recording.
        * 
        * @param "TTimeIntervalMicroSeconds32 aRecordDuration" Length of time 
        *        to record, expressed in microseconds.
        */
        void StartRecognition( TTimeIntervalMicroSeconds32 aRecordDuration );
        
        /**
        * Ends recording process
        */
        void StopRecognition();

        /**
        * Use to set the priority of the sound device
        *
        * @param "TMMFPrioritySettings& aPrioritySettings" Priority settings
        *        structure.
        */
        void SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings );
        
        /**
        * Request to start a recognition session.
        * 
        * @param "TRecognizerMode aMode" Recognizer mode.
        * @return result code of request
        */
        TInt StartRecSession( TRecognizerMode aMode );
        
        /**
        * Call to give speech data to recognizer.
        *
        * @param "TDesC8& aBuffer" Filled buffer.
        * @param "TBool aEnd" Flag to tell if this is the last buffer.
        */
        void SendSpeechData( TPtrC8& aBuffer, TBool aEnd );

        /**
        * Request to unload the specified rule in the grammar from recognizer.
        *
        * @param "TSIGrammarID aGrammarID" Identifer of the grammar to operate on.
        * @param "TSIRuleID aRuleID" Identifier of the rule to remove.
        */
        void UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
        
        /**
        * Retreive the raw audio data accumulated during recording.
        *
        * @param "TDes8* aBuffer" Reference to a buffer containing the data.
        * @param "TTimeIntervalMicroSeconds32& aDuration" Duration of the 
        *        utterance.
        */
        void GetUtteranceData( TDes8* aBuffer, 
                               TTimeIntervalMicroSeconds32& aDuration );

        /**
        * Checks if grammar is loaded or not
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" Grammar ID
        * @return ETrue if grammar is currently loaded, EFalse otherwise.
        */
        TBool IsGrammarLoaded( TSIGrammarID aGrammarID );
        
        /**
        * Checks if grammar is active or not
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" Grammar ID
        * @return ETrue if grammar is active, EFalse otherwise.
        */
        TBool IsGrammarActive( TSIGrammarID aGrammarID );

        /**
        * Prints DevASR state information for debugging
        */
        void DevASRState();
        
        /**
        * Pre-starts recording before StartRecording call.
        *
        * @since 3.2
        */
        void PreStartSamplingL();

        // ===================================
        // DevSound Observer MIXIN begins
        // ===================================
        
        /**
        * Called by DevSound when a buffer is available to be read.
        *
        * @param "CMMFBuffer* aBuffer" Buffer containing data to be processed
        */
        void BufferToBeEmptied( CMMFBuffer* aBuffer );
        
        /**
        * Called by DevSound when a buffer is available to be written.
        *
        * @param "CMMFBuffer* aBuffer" Buffer to which data is written
        */
        void BufferToBeFilled( CMMFBuffer* aBuffer );
        
        /**
        * Not used by DevASR
        */
        void ConvertError( TInt aError );
        
        /**
        * Handles device event.
        *
        * @param "TDesC8& aMsg" A message packed in the descriptor format.
        */
        void DeviceMessage( TUid aMessageType, const TDesC8& aMsg );
        
        /**
        * Called by DevSound when initialization is completed.
        *
        * @param aError Error code, KErrNone if successful.
        */
        void InitializeComplete( TInt aError );
        
        /**
        * Handles record completion or cancel event from DevSound.
        *
        * @param aError Error code, KErrNone if successful.
        */
        void RecordError( TInt aError );
        
        /**
        * Handles play completion or cancel event from DevSound.
        *
        * @param "TInt aError" Error code, KErrNone if successful.
        */
        void PlayError( TInt aError );
        
        /**
        * Handles tone play completion or cancel event from DevSound
        * Tone is not used by DevASR so this event is not expected.
        *
        * @param "TInt aError" Error code, KErrNone if successful.
        */
        void ToneFinished( TInt aError );
        
       	/**
        * Handles audio policy events from DevSound.
        *
        * @param "TMMFEvent&" aEvent An audio policy event.
        */
        void SendEventToClient( const TMMFEvent& aEvent );
        
        // ===================================
        // DevSound Observer MIXIN ends
        // ===================================
        
        // ============================================
        // RecognitionAlgMgr Observer MIXIN begins
        // ============================================
         
        /**
        * Invoked by the adaptation hw device when model adaptation has
        * finished its processing.
        *
        * @since 2.8
        * @param "TInt aResultCode" Error code of operation.
        */
        void AdaptComplete( TInt aResultCode );

        /**
        * Invoked by the front-end when EOU has been detected.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void EouDetected( TInt aResultCode );
        
        /**
        * Invoked by the algorithm manager when the frontend initialization
        * is completed.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void InitFEComplete( TInt aResultCode );
        
        /**
        * Invoked by the algorithm manager when the recognition backend
        * initialization is completed.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void InitRecognizerBEComplete( TInt aResultCode );
        
        /**
        * Invoked by the algorithm manager when loading grammar
        * is completed.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void LoadGrammarComplete( TInt aResultCode );

        /**
        * Invoked when grammar has been unloaded.
        *
        * @param "TInt aResultCode" Result of grammar loading
        */
        void UnloadGrammarComplete( TInt aResultCode );
        
        /**
        * Invoked when grammar has been activated.
        *
        * @param "TInt aResultCode" Result of grammar activation
        */
        void ActivateGrammarComplete( TInt aResultCode );

        /**
        * Invoked when grammar has been deactivated.
        *
        * @param "TInt aResultCode" Result of grammar deactivation
        */
        void DeActivateGrammarComplete( TInt aResultCode );
        
        /**
        * Invoked by the algorithm manager when loading lexicon
        * is completed.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void LoadLexiconComplete( TInt aResultCode );
        
        /**
        * Invoked by the algorithm manager when loading models
        * is completed.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void LoadModelsComplete( TInt aResultCode );
        
        /**
        * Invoked when the recognition process is completed.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void RecognitionComplete( TInt aResultCode );
        
        /**
        * Invoked when the training process is completed.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void TrainComplete( TInt aResultCode );

        /**
        * Invoked when rule unloading has been done.
        *
        * @param "TInt aResultCode" Result code, KErrNone if successful.
        */
        void UnloadRuleComplete( TInt aResultCode );
        
        /**
        * Invoked when a feature vector is available.
        *
        * @param "TDesC8& aFV" Buffer containing a feature vector.
        * @param "TInt32 aSNR" Signal-to-noise ratio.
        * @param "TInt32 aPosition" Indicates whether this is the first,
        *         subsequent or last feature vector.
        */
        void FeatureVectorDataRcvd( const TDesC8& aFV, 
                                    TInt32 aSNR, 
                                    TInt32 aPosition );
        
        /**
        * Invoked by the front-end when utterance data is needed.
        *
        * @since 2.8
        */
        void RequestSpeechData();

     
        /**
        * Resolves result.
        */
        void ResolveResult( const RArray<TUint>& aNBestIDs,
                            CSIResultSet& aSIResultSet,
                            const RPointerArray<CSICompiledGrammar>& aSICompiledGrammar,
                            const TDesC8& aCombinedData );

        /**
        * Invoked when grammar combining is needed
        *
        * @since 2.8
        * @param "RPointerArray<CSICompiledGrammar>& aCompiledGrammars" 
        *        Array of previously compiled grammar
        * @param "const RPointerArray<TSIRuleVariantInfo>& aExcludedRules" 
        *        Rules to be blacklisted.
        */
        void CombineGrammarL( const RPointerArray<CSICompiledGrammar>& aCompiledGrammars,
                              const RPointerArray<TSIRuleVariantInfo>& aExcludedRules );

        // ============================================
        // RecognitionAlgMgr Observer MIXIN ends
        // ============================================
        
        // ============================================
        // MVmAlgMgrObserver Observer MIXIN begins
        // ============================================

        /**
        * Notifies that grammar combination has been done.
        */
        void CombineComplete( HBufC8* aResult, TInt aError );

        /**
        * Get the specified grammar.
        * 
        * @param "TSIGrammarID aGrammarID" Identifier of grammar to be found.
		* @param "CSICompiledGrammar** aSIActiveGrammar" Pointer to found SI 
        *        active grammar, NULL if not found.
		* @param "CSICompiledGrammar** aSIDeActivatedGrammar" Pointer to found SI 
        *        non-active grammar, NULL if not found
        */
        void GetGrammarL( const TSIGrammarID aGrammarID, 
                          CSICompiledGrammar** aSIActiveGrammar,
                          CSICompiledGrammar** aSIDeActivatedGrammar );

        // ============================================
        // MVmAlgMgrObserver Observer MIXIN begins
        // ============================================
        
    private:
        
        /**
        * C++ default constructor.
        *
        * @param "MDevASRObserver& aObserver" Reference to observer.
        */
        CSRSAlgorithmManager( MDevASRObserver& aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * From CActive.
        */
        void RunL();
        void DoCancel();
        
        /**
        * Empties and copies the data to holding buffer.
        *
        * @param "CMMFDataBuffer& aBuffer" Buffer containing utterance data
        * @param "TInt aBufferLength" Length of buffer
        */
        TInt EmptyBuffer( CMMFDataBuffer& aBuffer, TInt aBufferLength );
        
        /**
        * Fill the received buffer with utterance data upto the specified length.
        *
        * @param "CMMFDataBuffer& aBuffer" Buffer containing utterance data
        * @param "TInt aBufferLength" Length of buffer
        */
        TInt FillBuffer( CMMFDataBuffer& aBuffer, TInt aBufferLength );
       
        /**
        * Transition from current state to target state if possible.
        *
        * @param "TDevASRState aState" A target state
        * @return ETrue if successful.
        */
        TBool StateTransition( TDevASRState aState );
        
        /**
        * Translate system error codes to DevASR cause codes.
        *
        * @param "TInt aError" System error constant
        * @return A DevASR error code
        */
        TDevASRError TranslateError( TInt aError );
        
        /**
        * Async handler for a play request.
        */
        void HandlePlayL();
        
        /**
        * Async handler for a record request.
        */
        void HandleRecordL();
       
        /**
        * Async handler for a UtteranceDataProcessed event.
        */
        void HandleUtteranceDataProcessed();
       
        /**
        * Completes a request with specified status.
        *
        * @param "TInt aStatus" Operation status.
        */
        void Ready( const TInt aStatus );
        
        /**
        * Configure the sound device according to parameters in the resource file.
        */
        void ConfigureSoundDeviceL();
        
        /**
        * Initializes DevSound for sampling
        */
        void InitializeDevSoundL();
        
        /**
        * Starts sampling with DevSound
        */
        void StartDevSoundL();
        
    private:    // Data
        
        // DevASR Observer, the SRS Plugin
        MDevASRObserver* iDevASRObserver;
        
        // ----- DevSound related data -----
        
        // Pointer to DevSound
        CMMFDevSound* iDevSound;
        
        // structure of capabilities for DevSound instance
        TMMFCapabilities iDevSoundCapabilities;
        
        // structure of priorities for DevSound instance
        TMMFPrioritySettings iPrioritySettings;
        
        // stores the current volume and gain
        TInt iCurrentVolume;
        TInt iCurrentGain;
        
        // ----- Algorithm managers ----

        // Pointer to Recognition Algorithm Manager
        CRecognitionAlgMgr* iRecognitionAlgMgr;

        // Pointer to Vocabulary algorithm manager
        CVMAlgorithmManager* iVMAlgorithmManager;

        // Flag to tell if recognition should be started when first audio
        // buffer has been sampled.
        TBool iStartRecognition;

        // Flag to tell if there is a pending RequestSpeechData() call from 
        // recognition hw device.
        TBool iPendingRequestSpeechData;
        
        // --- SD Training audio buffer managemet ---
        // Start and stop points in the audio buffer, used during recording
        TUint32 iStartPoint;
        TUint32 iStopPoint;
        // Audio buffer to hold PCM data coming from DevSound
        TUint8* iAudioBuffer;
        TPtr8 iPtr;


        // ----- Buffer management -----
        
        // Number of ms to record
        TTimeIntervalMicroSeconds32 iRecordDuration;
        

        // Overall number of bytes that need to be sampled.
        // Zero if we should sample until EndRecord() is called.
        TInt iOverallLength;

        // Number of samples that are recorded.
        TInt iOverallSampled;
        
        // Indicates if utterance processing is currently being done
        TBool iProcessingUtterance;
        
        
        // Queue of audio buffer descriptors to be processed
        TSglQue<CQueItem>* iAudioBufferQue;

        // An item in the queue of audio buffer descriptors
        CQueItem* iQueItem;

        // Flag to tell if recording should be ended
        TBool iEndFlag;

        // Recognizer mode
        TRecognizerMode iMode;

        
        // ----- End-pointing usage -----
        
        // Start and end frames in audio buffer
        TUint32 iStartFrame;
        TUint32 iEndFrame;
        TReal iFrameLength;
        
        // Start and end point in the audio buffer after end-pointing.
        TUint32 iBufferStartPoint;
        TUint32 iBufferEndPoint;
        
        // ----- State management -----
        
        // State of DevASR
        TDevASRState iDevASRState;
        
        // State of DevSound
        TDevSoundState iDevSoundState;
        
        // ----- Active object handling -----
        TInt iRequestFunction;
        
        // resource file handler
        CDevASRResourceHandler* iResourceHandler;

        
        TUint8* iBufferUnderConstruction;
        TInt iConstructionPoint;

		const CSIModelBank* iSIModelBank;
		const CSILexicon* iSILexicon;
        
        // Flag which tells if pre-sampling has been started
        TBool iPreSamplingStarted;
        
        // Wait loop to wait for audio initialization callback
        CActiveSchedulerWait iAudioWait;
        
        // The following are used when dumping captured audio to file for analysis
#ifdef AUDIOBUFFER_TO_FILE
        RFs iFs;
        RFile iBufferDataFile;
        TBool iFileCreated;
#endif
        
    };
    
#endif // DEVASRSRSALGORITHMMANAGER_H
    
// End of file
