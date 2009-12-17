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
* Description:  This file contains definition of the DevASR API.
*
*/


#ifndef NSSDEVASRAPI_H
#define NSSDEVASRAPI_H

//#define AUDIOBUFFER_TO_FILE

//  INCLUDES
#include <e32base.h>
#include <nsssispeechrecognitiondatadevasr.h>
#include <nssdevasrcommon.h>

// FORWARD DECLARATIONS
class CSRSAlgorithmManager;
class CVMAlgorithmManager;
class TMMFPrioritySettings;

/**
*  Interface for the DevASR callbacks that are implemented by DevASR observers.
*
*  @lib NssDevASR.lib
*  @since 2.8
*/
class MDevASRObserver
    {
    public:
        /**
        * DevASR calls this method to get configuration data.
        *
        * @since 2.8
        * @param "TUint32 aPackageType" Type identifier.
        * @param "TUint32 aPackageID" Identifier of package.
        * @param "TUint32 aStartPosition" Starting position within package in bytes.
        * @param "TUint32 aEndPosition" Ending position within package in bytes.
        * @return Pointer to created buffer containing the request data.
        */
        virtual HBufC8* ConfigurationData( TUint32 aPackageType, 
                                           TUint32 aPackageID,
                                           TUint32 aStartPosition = 0,
                                           TUint32 aEndPosition = KMaxTUint32 ) = 0;
        
        /**
        * DevASR notifies object implementing this interface whenever a spontaneous or non-spontaneous
        * event occurs in DevASR.
        *
        * @since 2.8
        * @param "TDevASREvent aEvent" DevASR event code.
        * @param "TDevASRError aError" DevASR error code.
        */
        virtual void DevASREvent( TDevASREvent aEvent, TDevASRError aError ) = 0;
        
        /**
        * DevASR notifies object implementing this interface when feature vectors are available.
        *
        * @since 2.8
        * @param "TDesC16& aFV" Buffer containing the feature vector data.
        * @param "TInt32 aSNR" Signal-to-noise ratio.
        * @param "TInt32 aPosition" Indicates whether the feature vector is the first, middle, or end.
        */
        virtual void FeatureVectorDataRcvd( const TDesC8& aFV, TInt32 aSNR, TInt32 aPosition ) = 0;
        
        /**
        * DevASR calls this method to send a message in response to a custom command.
        *
        * @since 2.8
        * @param "TDesC8& aMsg" Buffer containing the message.
        */
        virtual void DevASRMessage( TDesC8& aMsg ) = 0;
        
        /**
        * Called when DevASR needs a SI lexicon.
        *
        * @since 2.8
        * @param "TSILexiconID aLexiconID" Lexicon ID of requested lexicon.
        * @return Pointer to created lexicon.
        */
        virtual CSILexicon* SILexiconL( TSILexiconID aLexiconID ) = 0;

        /**
        * Invoked by DevASR when it needs utterance data. The response is sent
        * using SendSpeechData().
        *
        * @since 2.8
        */
        virtual void RequestSpeechData() = 0;
    };

/**
*  Interface Class for the DevASR API.
*
*  @lib NssDevASR.lib
*  @since 2.8
*/
class CDevASR : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructors.
        *
        * @param "MDevASRObserver& aObserver" Reference to callback observer.
        */
        IMPORT_C static CDevASR* NewL( MDevASRObserver& aObserver );
        IMPORT_C static CDevASR* NewLC( MDevASRObserver& aObserver );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CDevASR();
        
    public: // New functions
        
        /**
        * Activates a grammar.
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" A grammar identifier.
        */
        IMPORT_C void ActivateGrammarL( TSIGrammarID aGrammarID );
        
        /**
        * Adapts vocabulary and models after successful recognition.
        *
        * @since 2.8
        * @param "CSIResultSet& aResultSet" Reco result set.
        * @param "TInt aResultIndex" Index of correct result.
        */
        IMPORT_C void AdaptL( const CSIResultSet& aResultSet, TInt aResultIndex );
        
        /**
        * Cancels the current or started tasks.
        * @since 2.8
        */
        IMPORT_C void Cancel();
        
        /**
        * Compiles an SI grammar.
        *
        * @since 2.8
        * @param "CSICompiledGrammar& aGrammar" Reference to a grammar to be compiled.
        */
        IMPORT_C void CompileGrammarL( CSICompiledGrammar& aGrammar );

        /**
        * Computes number of rule variants for grammar.
        *
        * @since 2.8
        * @param "CSIGrammar& aGrammar" A grammar where to add new rules.
        * @param "TUint32 aTargetNRuleVariants" Preferable target number of all 
        *         rule variants after adding new rule variants.
        * @param "TUint32 aMaxNRuleVariants" Maximum number of all rule variants.
        * @param "RArray<TUint>& aNewRuleScores" Scores for each new rule. The 
        *         size of array tells also how many new rules will be added. 
        *         Based on the scores some rules can get more rule variants than
        *         others. If there is no scores available, the array should 
        *         initialize with constant values.
        * @param "RArray<TUint>& aNewRuleVariants" Array where to add number 
        *         of new rule variants for new rules. The order is equivalent 
        *         to aNewRuleScores.
        * @param "TUint32& aNPrune" Number of rule variants which should be 
        *         pruned before adding new rule variants.
        */
        IMPORT_C void ComputeNewGrammarSizeL( const CSIGrammar& aGrammar, 
											  const TUint32 aTargetNRuleVariants,
                                              const TUint32 aMaxNRuleVariants,
                                              const RArray<TUint>& aNewRuleScores,
                                              RArray<TUint>& aNewRuleVariants,
                                              TUint32& aNPrune );

        /**
        * Use to request a custom interface implementation from the component.
        * through the DevASR API.
        *
        * @since 2.8
        * @param "TUid aInterfaceId" Interface UID, defined with the custom 
        *         interface.
        * @return A pointer to the interface implementation, or NULL if the 
        *         requested interface is not implemented. The return value must
        *         be cast to the correct type by the user.
        */
        IMPORT_C TAny* CustomInterface( TUid aInterfaceId );
        
        /**
        * Deactivates a grammar.
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" A grammar identifier.
        */
        IMPORT_C void DeactivateGrammarL( TSIGrammarID aGrammarID );
        
        /**
        * Requests to end recording and a result will be resolved.
        *
        * @since 2.8
        */
        IMPORT_C void EndRecord();

        /**
        * This method is used to indicate the end of a recognition session. All loaded grammars,
        * lexicon, and models become invalid after this call.
        *
        * @since 2.8
        */
        IMPORT_C void EndRecSession();
        
        /**
        * Retreive the properties of the underlying speech recognition engine.
        *
        * @since 2.8
        * @param "RArray<TInt>& aPropertyId" An array of identifiers being querried.
        * @param "RArray<TInt>& aPropertyValue" An array of values corresponding
        *         to the querried identifiers.
        */
        IMPORT_C void GetEnginePropertiesL( const RArray<TInt>& aPropertyId, 
                                            RArray<TInt>& aPropertyValue );
        
        /**
        * Retreive the raw audio data accumulated during recording.
        *
        * @since 2.8
        * @param "TDes8* aBuffer" The buffer where utterance data will be 
        *        written.
        * @param "TTimeIntervalMicroSeconds32& aDuration" Duration of the 
        *        utterance.
        */
        IMPORT_C void GetUtteranceData( TDes8* aBuffer, 
                                        TTimeIntervalMicroSeconds32& aDuration );
        
        /**
        * Initializes the front-end module in the speech recognition engine.
        * The frontend module used during training/recognition functions is started
        * as a result. This method is intended to be used in conjunction with InitTrainBE().
        *
        * @since 2.8
        * @param "TRecognizerMode aMode" Mode of the recognizer.
        */
        IMPORT_C void InitFrontEnd( TRecognizerMode aMode );
        
        /**
        * Initialize the recognition engine back-end. The module responsible for recognition
        * function is started as a result. This method must be used before any recognition
        * operations and intended to be used in conjunction with InitFrontEnd().
        *
        * @since 2.8
        * @param "CSIResultSet& aResult" A reference to an object where the 
        *        recognition result will be written.
        */
        IMPORT_C void InitRecognizerBE( CSIResultSet& aResult );
        
        /**
        * Load the specified recognizer parameter(s). These parameters are used to alter the
        * recognizer's default parameters. The parameters are specified as attribute-value pairs.
        *
        * @since 2.8
        * @param "RArray<TInt>& aParameterId" An array of parameter identifiers.
        * @param "RArray<TInt>& aParameterValue" An array of parameter values.
        */
        IMPORT_C void LoadEngineParametersL( const RArray<TInt>& aParameterId, 
                                             const RArray<TInt>& aParameterValue );
        
        /**
        * Load the specified grammar into the recognizer.
        *
        * @since 2.8
        * @param "CSIGrammar& aGrammar" A reference to a SI grammar.
        */
        IMPORT_C void LoadGrammar( const CSIGrammar& aGrammar );
        
        /**
        * Load the specified grammar into the recognizer.
        *
        * @since 2.8
        * @param "CSICompiledGrammar& aGrammar" A reference to a compiled 
        *         SI grammar
        */
        IMPORT_C void LoadGrammar( const CSICompiledGrammar& aGrammar );
        
        /**
        * Unloads the specified grammar from the recognizer.
        *
        * @since 2.8
        * @param "CSIGrammar& aGrammar" A reference to a SI grammar.
        */
        IMPORT_C void UnloadGrammar( const CSIGrammar& aGrammar );
        
        /**
        * Unloads the specified grammar from the recognizer.
        *
        * @since 2.8
        * @param "CSICompiledGrammar& aGrammar" A reference to a compiled 
        *         SI grammar
        */
        IMPORT_C void UnloadGrammar( const CSICompiledGrammar& aGrammar );

        /**
        * Load the specified SI lexicon into the recognizer.
        *
        * @since 2.8
        * @param "CSILexicon& aLexicon" A reference to a SI lexicon.
        */
        IMPORT_C void LoadLexicon( const CSILexicon& aLexicon );
        
        /**
        * Load the specified model bank into the recognizer.
        *
        * @since 2.8
        * @param "CSIModelBank& aModels" A reference to a SI model bank.
        */
        IMPORT_C void LoadModels( const CSIModelBank& aModels );
        
        /**
        * Determines which rule variants can be removed.
        *
        * @since 2.8
        * @param "CSIGrammar& aGrammar" A grammar to be pruned.
        * @param "TUint32 aMinNumber" Minimum number of pruned items.
        * @param "RArray<TSIRuleVariantInfo>" A array where to put information 
        *        which variants can be pruned.
        */
        IMPORT_C TBool PruneGrammar( const CSIGrammar& aGrammar, 
                                     const TUint32 aMinNumber, 
                                     RArray<TSIRuleVariantInfo>& aPrunedRuleVariants );
      
        /**
        * Request to begin recognition.
        *
        * @since 2.8
        * @param "TTimeIntervalMicroSeconds32 aRecordDuration" Length of time
        *        to record, expressed in microseconds. Zero means infinite
        *        duration.
        */
        IMPORT_C void Record( TTimeIntervalMicroSeconds32 aRecordDuration );

        /**
        * Starts sampling before Record call.
        * 
        * @since 3.2
        */
        IMPORT_C void PreStartSamplingL();

        /**
        * Use to send the utterance data as a response to the RequestSpeechData().
        *
        * @since 2.8
        * @param "TDesC8& aBuffer" Buffer containing utterance data.
        * @param "TBool aEnd" Flag to tell if given buffer is the last one.
        */
        IMPORT_C void SendSpeechData( TPtrC8& aBuffer, TBool aEnd );
        
        /**
        * Use to set the priority of the sound device.
        *
        * @since 2.8
        * @param "TMMFPrioritySettings& aPrioritySettings" Priority settings 
        *        structure.
        */
        IMPORT_C void SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings );
       
        /**
        * This method is used to start a recognition session with the recognition engine.
        *
        * @since 2.8
        * @param "TRecognizerMode aMode" Mode of the recognizer.
        * @return Result code of operation.
        */
        IMPORT_C TInt StartRecSession( TRecognizerMode aMode );
        
        /**
        * Starts training from text
        *
        * @since 2.8
        * @param "CSITtpWordList& aWordList" List of text entries for training.
        * @param "RArray<TLanguage>& aDefaultLanguage" Default language for 
        *        pronunciations.
        * @param "RArray<TUint>& aMaxNPronunsForWord" Maximum number of 
        *        pronunciation per word.
        */
        IMPORT_C void StartTrainingFromTextL( CSITtpWordList& aWordList, 
                                              const RArray<TLanguage>& aDefaultLanguage, 
                                              const RArray<TUint32>& aMaxNPronunsForWord );
   
        /**
        * Request to unload the specified rule in the grammar from recognizer.
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" Identifer of the grammar to operate on.
        * @param "TSIRuleID aRuleID" Identifier of the rule to remove.
        */
        IMPORT_C void UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
        
        /**
        * Checks if grammar is loaded or not
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" Grammar ID
        * @return ETrue if grammar is currently loaded, EFalse otherwise.
        */
        IMPORT_C TBool IsGrammarLoaded( TSIGrammarID aGrammarID );
        
        /**
        * Checks if grammar is active or not
        *
        * @since 2.8
        * @param "TSIGrammarID aGrammarID" Grammar ID
        * @return ETrue if grammar is active, EFalse otherwise.
        */
        IMPORT_C TBool IsGrammarActive( TSIGrammarID aGrammarID );

    private:
        
        /**
        * C++ default constructor.
        */
        CDevASR();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MDevASRObserver& aObserver);
        
    private: // Data
        
        // Pointer to recognition algorithm manager
        CSRSAlgorithmManager* iSRSAlgorithmManager;
       
        // Reserved for future use
        TAny* iReserved1;
        TAny* iReserved2;

    };
    
#endif // NSSDEVASRAPI_H
    
// End of File
