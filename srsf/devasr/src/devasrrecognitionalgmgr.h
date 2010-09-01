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
* Description:  This class implements the recognition algorithm manager.
*
*/


#ifndef DEVASRRECOGNITIONALGMGR_H
#define DEVASRRECOGNITIONALGMGR_H

//  INCLUDES
#include <nsssispeechrecognitiondatadevasr.h>
#include <asrsrecognitionhwdevice.h>
#include <nssdevasrcommon.h>
#include <nssdevasr.h>
#include <asrsadaptationhwdevice.h>

// CLASS DECLARATION

/**
*  Interface class to be implemented by observer of the recognition algorithm manager.
*
*  @since 2.0
*/
class MRecAlgMgrObserver
    {
    public:

        /**
        * Invoked by the adaptation hw device when model adaptation has
        * finished its processing.
        *
        * @since 2.8
        * @param "TInt aResultCode" Error code of operation.
        */
        virtual void AdaptComplete( TInt aResultCode ) = 0;


        /**
        * Feature vector received event from the front-end algorithm.
        *
        * @since 2.0
        * @param "TDesC8& aFV" Buffer containing a feature vector.
        * @param "TInt32 aSNR" Signal-to-noise ratio.
        * @param "TInt32 aPosition" Indicates whether this is the first, 
        *        subsequent or last feature vector.
        */
        virtual void FeatureVectorDataRcvd( const TDesC8& aFV, 
                                            TInt32 aSNR, 
                                            TInt32 aPosition ) = 0;

        /**
        * Invoked by the front-end when EOU has been detected
        *
        * @since 2.8
        * @param "TInt aResultCode" Result of front-end processing.
        */
        virtual void EouDetected( TInt aResultCode ) = 0;

        /**
        * Invoked by the front-end when initialization has completed.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of front-end initialization.
        */
        virtual void InitFEComplete( TInt aResultCode ) = 0;

        /**
        * Invoked by the recognizer back-end when initialization has completed.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of recognizer back-end initialization.
        */
        virtual void InitRecognizerBEComplete( TInt aResultCode ) = 0;

        /**
        * Invoked by the recognizer back-end when grammar loading has completed.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of grammar loading
        */
        virtual void LoadGrammarComplete( TInt aResultCode ) = 0;

        /**
        * Invoked when grammar has been unloaded.
        *
        * @since 2.8
        * @param "TInt aResultCode" Result of grammar loading
        */
        virtual void UnloadGrammarComplete( TInt aResultCode ) = 0;

        /**
        * Invoked when grammar has been activated.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of grammar activation
        */
        virtual void ActivateGrammarComplete( TInt aResultCode ) = 0;

        /**
        * Invoked when grammar has been deactivated.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of grammar deactivation
        */
        virtual void DeActivateGrammarComplete( TInt aResultCode ) = 0;

        /**
        * Invoked by the recognizer back-end when lexicon loading has completed.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of lexicon loading.
        */
        virtual void LoadLexiconComplete( TInt aResultCode ) = 0;

        /**
        * Invoked by the recognizer back-end when model loading has completed.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of model loading.
        */
        virtual void LoadModelsComplete( TInt aResultCode ) = 0;

        /**
        * Invoked by the recognizer when recognition process has completed.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of the recognition process.
        */
        virtual void RecognitionComplete( TInt aResultCode ) = 0;

        /**
        * Invoked by the training module when the training process has completed.
        *
        * @since 2.0
        * @param "TInt aResultCode" Result of training process.
        */
        virtual void TrainComplete( TInt aResultCode ) = 0;

        /**
        * Invoked when rule unloading has been done.
        *
        * @since 2.8
        * @param "TInt aResultCode" Result of training process.
        */
        virtual void UnloadRuleComplete( TInt aResultCode ) = 0;

        /**
        * Invoked when speech data is needed.
        *
        * @since 2.8
        */
        virtual void RequestSpeechData() = 0;

        /**
        * Invoked when result resolving is needed.
        *
        * @since 2.8
        * @param "RArray<TUint>& aNBestIDs"
        * @param "CSIResultSet& aSIResultSet"
        * @param "RPointerArray<CSICompiledGrammar>& aSICompiledGrammar"
        * @param "TDesC8& aCombinedData"
        */
        virtual void ResolveResult( const RArray<TUint>& aNBestIDs,
                                    CSIResultSet& aSIResultSet,
                                    const RPointerArray<CSICompiledGrammar>& aSICompiledGrammar,
                                    const TDesC8& aCombinedData/*,
                                    CSIModelBank& iSIModelBank*/ ) = 0;

        /**
        * Invoked when grammar combining is needed
        *
        * @since 2.8
        * @param "RPointerArray<CSICompiledGrammar>& aCompiledGrammars" 
        *        Array of previously compiled grammar
        * @param "const RPointerArray<TSIRuleVariantInfo>& aExcludedRules" 
        *        Rules to be blacklisted.
        */
        virtual void CombineGrammarL( const RPointerArray<CSICompiledGrammar>& aCompiledGrammars,
                                      const RPointerArray<TSIRuleVariantInfo>& aExcludedRules ) = 0;

    };


/**
*  This class is an aggregation of all related algorithms. It manages the algorithms
*  and coordinate transfer of data.
*
*  @lib NssDevASR.lib
*  @since 2.0
*/
class CRecognitionAlgMgr : public CActive,
                           public MASRSRecognitionHwDeviceObserver,
                           public MASRAdaptationHwDeviceObserver
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CRecognitionAlgMgr* NewL( MRecAlgMgrObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CRecognitionAlgMgr();

    public: // New functions

        /**
        * Adapts models
        *
        * @sinxe 2.8
        * @param "CSIResultSet& aResultSet" Result set reference.
        * @param "TInt aResultIndex" Index of correct result.
        * @param "TLanguage aLanguage" Language of correct result.
        */
        void AdaptModelsL( const CSIResultSet& aResultSet, TInt aResultIndex,
                           TLanguage aLanguage );

        /**
        * Cancels the current or started tasks.
        *
        * @since 2.0
        */
        void Cancel();

        /**
        * This method is used to indicate the end of a recognition session.
        * The EndRecSession() method should be used to end the session.
        *
        * @since 2.0
        */
        void EndRecSession();

        /**
        * Grammar combination has been completed.
        */
        void CombineComplete( HBufC8* aResult, TInt aError );

		/**
        * Retreive the properties of the underlying speech recognition engine.
        *
        * @since 2.0
        * @param "RArray<TInt>& aPropertyId" An array of identifiers being querried.
        * @param "RArray<TInt>& aPropertyValue" An array of values corresponding
        *        to the querried identifiers.
        */
        void GetEnginePropertiesL( const RArray<TInt>& aPropertyId, 
                                   RArray<TInt>& aPropertyValue );

        /**
        * Load the specified parameter(s) to the engines. 
        *
        * @param "RArray<TInt>& aParameterId" An array of parameter identifiers.
        * @param "RArray<TInt>& aParameterValue" An array of parameter values.
        */
        void LoadEnginePropertiesL( const RArray<TInt>& aParameterId,
                                    const RArray<TInt>& aParameterValue );

        /**
        * Retreive the duration of the utterance detected by the algorithm.
        *
        * @since 2.0
        * @param "TUint32& aStartFrame" Indicates the start frame of feature vector.
        * @param "TUint32& aEndFrame" Indicates the end frame of the feature vector.
        * @param "TReal& aFrameLength" The length of each frame
        * @return ETrue if successful
        */
        TBool GetUtteranceDuration( TUint32& aStartFrame, TUint32& aEndFrame, 
                                    TReal& aFrameLength );

        /**
        * Initializes the front-end module in the speech recognition engine.
        * The frontend module used during training/recognition functions is started
        * as a result. This method is intended to be used in conjunction with InitTrainBE().
        *
        * @since 2.0
        * @param "TRecognizerMode aFeMode" Mode of the recognizer.
        */
        void InitFrontEnd( TRecognizerMode aFeMode );

        /**
        * Initialize the recognition engine back-end. The module responsible for recognition
        * function is started as a result. This method must be used before any recognition
        * operations and intended to be used in conjunction with InitFrontEnd().
        *
        * @since 2.0
        * @param "aResult" A reference to an object where the recognition
        *        result will be written.
        */
		void InitRecognizerBE( CSIResultSet& aResult );

        /**
        * Load the specified grammar into the recognizer.
        *
        * @since 2.8
        * @param "aGrammar" A reference to a grammar in an internal format.
        */
		void LoadGrammarL( const CSIGrammar& aGrammar );
		void LoadGrammarL( const CSICompiledGrammar& aGrammar );

        /**
        * Unloads the specified grammar from the recognizer.
        *
        * @since 2.8
        * @param "aGrammar" A reference to a grammar.
        */
		void UnloadGrammarL( const CSIGrammar& aGrammar );
		void UnloadGrammarL( const CSICompiledGrammar& aGrammar );

        /**
        * Get the specified grammar.
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" Identifier of grammar to be found.
		* @param "CSDGrammar** aSDGrammar" Pointer to found SD grammar, 
        *         NULL if not found.
		* @param "CSICompiledGrammar** aSIActiveGrammar" Pointer to found SI 
        *        active grammar, NULL if not found.
		* @param "CSICompiledGrammar** aSIDeActivatedGrammar" Pointer to found SI 
        *        non-active grammar, NULL if not found
        */
		void GetGrammarL( const TSIGrammarID aGrammarID, 
                          CSICompiledGrammar** aSIActiveGrammar,
                          CSICompiledGrammar** aSIDeActivatedGrammar );

		/**
		* Activates a grammar.
        *
		* @since 2.8
		* @param "TSIGrammarID aGrammarID" A grammar identifier.
		*/
		void ActivateGrammarL( const TSIGrammarID aGrammarID );

		/**
		* Deactivates a grammar
        *
		* @since 2.8
		* @param "TSIGrammarID aGrammarID" A grammar identifier
		*/
		void DeactivateGrammarL( const TSIGrammarID aGrammarID );

        /**
        * Load the specified lexicon into the recognizer.
        *
        * @since 2.0
        * @param "aLexicon" A reference to a lexicon in an internal format.
        */
		void LoadLexiconL( const CSILexicon& aLexicon );

        /**
        * Load the specified model bank into the recognizer.
        *
        * @since 2.0
        * @param "aModels" A reference to a model bank.
        */
        void LoadModelsL( const CSDModelBank& aModels );
		void LoadModelsL( const CSIModelBank& aModels );

        /**
        * Use to send the utterance data as a response to the RequestSpeechData().
        *
        * @since 2.8
        * @param "TDesC8& aBuffer" Buffer containing utterance data.
        * @param "TBool aEnd" Flag to tell if given buffer is the last one.
        */
        void SendSpeechData( TPtrC8& aBuffer, TBool aEnd );

        /**
        * Request to start a recognition session.
        *
        * @since 2.0
        * @param "TRecognizerMode aMode" Mode of recognizer.
        * @return result code of request.
        */
        TInt StartRecSession( TRecognizerMode aMode );

        /**
        * Starts recognition.
        */
        void StartRecognitionL();

        /**
        * Request to unload the specified rule in the grammar from recognizer.
        *
        * @param "TSIGrammarID aGrammarID" Identifer of the grammar to operate on.
        * @param "TSIRuleID aRuleID" Identifier of the rule to remove.
        */
        void UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID );

        /**
        * Used during unit testing to print the state of the algorithm
        */
        void AlgorithmState();
        
        /**
        * Sets rejection value
        *
        * @param "TUint32 aRejection" Rejection threshold value.
        */
        void SetRejection( TUint32 aRejection );

    public: // MASRAdaptationHwDeviceObserver
        /**
        * Called when adaptation has been done
        */
        void MaahdAdaptationComplete( TInt aError );

    public: // From MASRSRecognitionHwDeviceObserver

        /**
        * Called by the hardware device when the InitializeL() method has completed.
        * @since Series60 2.8
        * @param aError Initialization result code. 
        *               KErrNone if successful otherwise a system-wide error code.
        * @return none
        */
        void MarhdoInitializationComplete( TInt aError );

        /**
        * Called by the hardware device when the  InitializeL() method has completed.
        * @since Series60 2.8
        * @param aError Initialization result code. 
        *               KErrNone if successful otherwise a system-wide error code.
        * @return none
        */
        void MarhdoInitRecognizerFEComplete( TInt aError );

        /**
        * Called by the hardware device when backend initialization is completed.
        * @since Series60 2.8
        * @param aError Initialization result code. 
        *               KErrNone if successful otherwise a system-wide error code.
        * @return none
        */
        void MarhdoInitRecognizerBEComplete( TInt aError );

        /**
        * Called by the hardware device when recognition result is available.
        * @since Series60 2.8
        * @param aError Recognition result code. KErrNone if successful.  
        *               KErrRejected if the recognition result is rejected, 
        *               otherwise a system-wide error code
        * @return none
        */ 
        void MarhdoRecognitionComplete( TInt aError );

        /**
        * Called by the hardware device when end-of-utterance is detected by the 
        * acoustic frontend. This method must be called before MarhdoRecognitionComplete().
        * @since Series60 2.8
        * @param aError Recognition result code. KErrNone if successful, otherwise KErrTooLong, 
        *               KErrTooShort, KErrAsrSpeechTooEarly, KErrNoSpeech
        * @return none
        */
        void MarhdoEouDetected( TInt aError );

        /**
        * Called by the hardware device when a feature vector is extracted by acoustic frontend.
        * @since Series60 2.8
        * @param aFV A buffer containing a feature vector.
        * @param aSNR Signal-to-noise ratio.
        * @param aPosition Indicates whether this is the first, subsequent or last feature vector in a series.
        * @return none
        */
        void MarhdoFeatureVector( const TDesC8& aFV, TInt32 aSNR, TInt32 aPosition );

        /**
        * Called by the hardware device to request for utterance data. 
        * The response to the request is sent in SendSpeechData().
        * @since Series60 2.8
        * @param none
        * @return none
        */
        void MarhdoRequestSpeechData();


    private:

        /**
        * C++ default constructor.
        */
        CRecognitionAlgMgr(MRecAlgMgrObserver& aObserver);

        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();

        void RunL();
        void DoCancel();

        /**
        * The following methods are asynchronous handlers for the corresponding
        * synchronous calls.
        */
        void HandleInitFrontEnd();
        void HandleInitTrainBE();
        void HandleInitRecognizerBE();
        void HandleLoadGrammar();
        void HandleLoadLexicon();
        void HandleLoadModels();
        void HandleUtteranceDataRcvd();
        void HandleActivateGrammar();
        void HandleDeActivateGrammar();
        void HandleUnloadRule();
        void HandleUnloadGrammar();

        /**
        * Used to complete a request
        */
        void Ready(const TInt aStatus);


    private: // Data

        /**
        * Current working state of the recognizer
        */
        enum TAlgState
            {
            EIdle=0,
            EProcessing,
            ECancel
        };

        // Pointer to Algorithm Manager Observer
        MRecAlgMgrObserver* iRecAlgMgrObserver;

		// Grammar handling
		RPointerArray<CSICompiledGrammar> iSIActiveGrammars;
		RPointerArray<CSICompiledGrammar> iSIDeActivatedGrammars;

        // Blacklisted rules
        RPointerArray<TSIRuleVariantInfo> iBlackList;

        // Result IDs
        RArray<TUint> iNBestList;

        // Result scores
        RArray<TInt> iScores;

        //CSICompiledGrammar* iCombinedGrammar;
        HBufC8* iCombinedGrammar;

        // Flag to tell if combining is needed for active grammars.
        TBool iCombineNeeded;

        // Flag to inform if recognition hw device has been successfully 
        // initialized.
        TBool iInitialized;

        // Pointer to Algorithms
        CASRSRecognitionHwDevice* iRecoHw;
        CASRSAdaptHwDevice* iAdaptHw;

        // Begin: data used in async handling of requests
        TRecognizerMode iMode;
        void* iModel;

        CSDModelBank* iSDModelBank;
		CSIModelBank* iSIModelBank;
        void* iSDResult;

		CSIResultSet* iSIResult;
        const void* iSDGrammar;
        const CSIGrammar* iSIGrammar;
		const CSICompiledGrammar* iSICompGrammar;
        const void* iSDCompGrammar;
        const void* iSDLexicon;

        const CSILexicon* iSILexicon;
        TSIGrammarID iGrammarID;
        TSIRuleID iRuleID;
        TUint32 iStartFrame;
        TUint32 iEndFrame;
        TReal   iFrameLength;
		TPtrC8 iBuffer;
        TBool iEnd;
        TInt iRequestFunction;
        // End: data used in async handling of requests

		// Algorithm states for front-end, back-end, and train back-end.
        TAlgState iFEState;
        TAlgState iBEState;

        // Should we send feature vectors to upper layers or not
        TBool iFeatures;

        // Should we include adaptation data to result set or not
        TBool iAdaptation;

        // Adaptation data
        HBufC8* iAdaptationData;
    };

#endif // DEVASRRECOGNITIONALGMGR_H

// End of File
