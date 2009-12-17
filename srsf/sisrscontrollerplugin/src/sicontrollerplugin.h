/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the main implementation of the SI Controller Plugin.
*               Finite State Machine is implemented here.
*
*/


#ifndef SICONTROLLERPLUGIN_H
#define SICONTROLLERPLUGIN_H

// ============================================================================
// Uncomment the following directive for Bit Exact Testing.
// ============================================================================
//
 
// INCLUDES
#include "srsfbldvariant.hrh"
#include <nsssispeechrecognitiondataclient.h>
#include <nsssispeechrecognitiondatacommon.h>
#include <nssdevasr.h>
#include <d32dbms.h>

// FORWARD DECLARATIONS
class CSIControllerPluginInterface;
class CDevASR;
class CSIResourceHandler;
class CSDClientResultSet;

class CSIGrammarDB; 
class CSIModelBankDB;
class CSILexiconDB; 
class CDataLoader;
class CSIDatabase;

#ifdef __SINDE_TRAINING
class CSindeTrainer;
#endif // __SINDE_TRAINING

// CLASS DECLARATION

/**
*  This is the main class of SI Controller Plugin
*
*  @lib SIControllerPlugin.lib
*  @since 2.0
*/
class CSIControllerPlugin : public CActive, public MDevASRObserver
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.    CSIControllerPluginInterface& aControllerIf
    */
    static CSIControllerPlugin* NewL( CSIControllerPluginInterface& aControllerInterface );
    
    /**
    * Destructor.
    */
    virtual ~CSIControllerPlugin();
    
public: // Functions from base classes
    
    /**
    * From MDevASRObserver
    * Spontaneous or non-spontaneous event received from DevASR.
    * @since 2.0
    * @param    aEvent                event code
    * @param    aError                error code
    */
    void DevASREvent( TDevASREvent aEvent, TDevASRError aError );
    
    /**
    * From MDevASRObserver
    * Notification from DevASR when feature vectors are available.
    * @since 2.0
    * @param    aFV                    buffer containing the feature vector data
    * @param    aSNR                signal-to-noise ratio
    * @param    aPosition            indicates whether the feature vector is the first, middle, or end.
    */
    void FeatureVectorDataRcvd( const TDesC8& aFV, TInt32 aSNR, TInt32 aPosition ); 
    
    /**
    * From MDevASRObserver
    * A message in response to a custom command.
    * @since 2.0
    * @param    aMsg                buffer containing the message
    */
    void DevASRMessage( TDesC8& aMsg );

    /**
    * From MDevASRObserver
    * DevASR calls this method to get lexicon data.
    * @since 2.8
    * @param    anID lexicon ID
    * @return    Lexicon object
    */
    CSILexicon* SILexiconL( TSILexiconID anID );
    
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
    HBufC8* ConfigurationData( TUint32 aPackageType, 
                               TUint32 aPackageID,
                               TUint32 aStartPosition,
                               TUint32 aEndPosition );
    
    /**
    * Get speech data from audio buffer
    *
    * @since 2.8
    */    
    void RequestSpeechData();    
    
#ifdef DEVASR_KEEP_SD
    CSDLexicon* SDLexiconL( TSILexiconID anID ) ;
#endif
    
public: // New functions
    
    /**
    * Adds a new pronunciation for the given model into the specified lexicon.
    * @since 2.0
    * @param    aLexiconID            lexicon Id, where the new pronunciation is added to
    * @param    aModelBankID        model bank Id
    * @param    aModelID            model Id
    * @param    aPronunciationID    reference where a new pronunciation Id is assigned to
    */
    void AddPronunciationL( TSILexiconID aLexiconID, TSIModelBankID aModelBankID,
        TSIModelID aModelID, TSIPronunciationID& aPronunciationID );
    
    void AddPronunciationL( TSILexiconID aLexiconID, TSIModelBankID aModelBankID,
        const TDesC8& aPronunciationPr, TSIPronunciationID& aPronunciationID );
    
    /**
    * Adds a new rule for the given pronunciation into the specified grammar.
    * @since 2.0
    * @param    aGrammarID            grammar Id, where the new rule is added to
    * @param    aLexiconID            lexicon Id
    * @param    aPronunciationID    pronunciation Id
    * @param    aRuleID                reference where a new rule Id is assigned to
    */
    void AddRuleL( TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
        TSIPronunciationID aPronunciationID, TSIRuleID& aRuleID );
    
    /**
    * Cancels the current activity and returns the plugin to Idle state.
    * @since 2.0
    */
    void Cancel();
    
    /**
    * Commits (saves) the current trained model into a permanent storage.
    * @since 2.0
    */
    void CommitChangesL( TBool aCompact );
    
    /**
    * Creates a new grammar.
    * @since 2.0
    * @param    aGrammarID            reference where a new rule Id is assigned to
    */
    void CreateGrammarL( TSIGrammarID& aGrammarID );
    
    /**
    * Creates a new lexicon.
    * @since 2.0
    * @param    aLexiconID            reference where a new lexicon Id is assigned to
    */
    void CreateLexiconL( TSILexiconID& aLexiconID );
    
    /**
    * Creates a new model bank.
    * @since 2.0
    * @param    aModelBankID        reference where a new model bank Id is assigned to
    */
    void CreateModelBankL( TSIModelBankID& aModelBankID );
    
    /**
    * Ends the current recognition session.  Resources allocated for recognition are freed.
    * @since 2.0
    * @param    -
    */
    void EndRecSessionL();
    
    /**
    * Returns all grammar Ids that belong to the current client, whose UID was set
    * with SetClientUidL().
    * @since 2.0
    * @param    aGrammarIDs            reference where grammar Ids are stored
    */
    void GetAllClientGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs );
    
    /**
    * Returns all lexicon Ids that belong to the current client, whose UID was set
    * with SetClientUidL().
    * @since 2.0
    * @param    aLexiconIDs            reference where lexicon Ids are stored
    */
    void GetAllClientLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs );
    
    /**
    * Returns all model bank Ids that belong to the current client, whose UID was set
    * with SetClientUidL().
    * @since 2.0
    * @param    aModelBankIDs        reference where model bank Ids are stored
    */
    void GetAllClientModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs );
    
    /**
    * Returns all grammar Ids that exist (for all clients).
    * @since 2.0
    * @param    aGrammarIDs            reference where grammar Ids are stored
    */
    void GetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs );
    
    /**
    * Returns all lexicon Ids that exist (for all clients).
    * @since 2.0
    * @param    aLexiconIDs            reference where lexicon Ids are stored
    */
    void GetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs );
    
    /**
    * Returns all model bank Ids that exist (for all clients).
    * @since 2.0
    * @param    aModelBankIDs        reference where model bank Ids are stored
    */
    void GetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs );
    
    /**
    * Returns all model Ids that exist in the specified model bank.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    * @param    aModelIDs            reference where model Ids are stored
    */
    void GetAllModelIDsL( TSIModelBankID aModelBankID, RArray<TSIModelID>& aModelIDs );
    
    /**
    * Returns all pronunciation Ids that exist in the specified lexicon.
    * @since 2.0
    * @param    aLexiconID            lexicon Id
    * @param    aPronunciationIDs    reference where pronunciation Ids are stored
    */
    void GetAllPronunciationIDsL( TSILexiconID aLexiconID,
        RArray<TSIPronunciationID>& aPronunciationIDs );
    
    /**
    * Returns all rule Ids that exist in the specified grammar.
    * @since 2.0
    * @param    aGrammarID            grammar Id
    * @param    aRuleIDs            reference where rule Ids are stored
    */
    void GetAllRuleIDsL( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs );
    
    /**
    * Returns the number of models available for training system wide, based on
    * available disk space.
    * @since 2.0
    * @param    aCount                reference where the number of available models is set
    */
    void GetAvailableStorageL( TInt& aCount );
    
    /**
    * Returns the engine properties specified by the engine property ID array.
    * On return aPropertyValue contains an array of engine properties.
    * @since 2.0
    * @param    aPropertyId            constant reference to array of engine property Ids
    * @param    aPropertyValue        reference to array of engine property values
    */
    void GetEnginePropertiesL( const RArray<TInt>& aPropertyId,
        RArray<TInt>& aPropertyValue );
    
    /**
    * Returns the number of models in the specified model bank.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    * @param    aCount                reference where the number of models is set
    */
    void GetModelCountL( TSIModelBankID aModelBankID, TInt& aCount );

    /**
    * Returns the number of pronunciationin the specified lexicon.
    * @since 2.0
    * @param    aLexiconID            lexicon Id
    * @param    aCount                reference where the number of models is set
    */
    
    void GetPronunciationCountL(TSILexiconID aLexiconID, TInt& aCount );

    /**
    * Returns the number of rules in the specified grammar.
    * @since 2.0
    * @param    aGrammarID        Grammar Id
    * @param    aCount                reference where the number of models is set
    */
    void GetRuleCountL(TSIGrammarID aGrammarID, TInt& aCount );
    
    /**
    * Checks if the rule is valid or not.
    * @since 2.0
    * @param    aGrammarID            grammar Id
    * @param    aRuleID                rule Id
    * @param    aValid                reference where the validity of the rule is set
    */
    void GetRuleValidityL( TSIGrammarID aGrammarID, TSIRuleID aRuleID, TBool& aValid );
    
    /**
    * Returns the duration of the utterance for the specified model.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    * @param    aModelID            model Id
    * @param    aDuration            reference where the duration of utterance is set
    */
    void GetUtteranceDurationL( TSIModelBankID aModelBankID, TSIModelID aModelID,
        TTimeIntervalMicroSeconds32& aDuration );
    
    /**
    * Loads the specified recognizer parameter(s).  These parameters are used to
    * alter the recognizer's default parameters.  The parameters are specified as
    * attribute and value pairs.
    * @since 2.0
    * @param    aParameterID        list of parameter Ids
    * @param    aParameterValue        list of parameter values
    */
    void LoadEngineParametersL( const RArray<TInt>& aParameterId,
        const RArray<TInt>& aParameterValue );
    
    /**
    * Loads the specified grammar into the recognizer; done prior to recognition.
    * @since 2.0
    * @param    aGrammarID            grammar Id
    */
    void LoadGrammarL( TSIGrammarID aGrammarID );
    
    /**
    * Loads the specified lexicon into the recognizer; done prior to recognition.
    * @since 2.0
    * @param    aLexiconID            lexicon Id
    */
    void LoadLexiconL( TSILexiconID aLexiconID );
    
    /**
    * Loads the specified model bank into the recognizer; done prior to recognition.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    */
    void LoadModelsL( TSIModelBankID aModelBankID );
    
    /**
    * Plays the previously trained utterance.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    * @param    aModelID            model Id, whose utterance is played
    */
    void PlayUtteranceL( TSIModelBankID aModelBankID, TSIModelID aModelID );
    
    /**
    * Initiates recognition; performed following loading of model bank, lexicon, and grammar.
    * @since 2.0
    * @param    aClientResultSet    reference where the recognition result is set
    */
    void RecognizeL( CSDClientResultSet& aClientResultSet );
    
    /**
    * Records uder utterance for training and recognition.
    * @since 2.0
    * @param    aRecordTime            recording time in microseconds
    */
    void RecordL( TTimeIntervalMicroSeconds32 aRecordTime );
    
    /**
    * Pre-starts sampling.
    * @since 3.2
    */
    void PreStartSamplingL();
    
    /**
    * Removes the specified grammar from the permanent storage.
    * Removing a grammar will remove all rules within the grammar.
    * @since 2.0
    * @param    aGrammarID            grammar Id
    */
    void RemoveGrammarL( TSIGrammarID aGrammarID );
    
    /**
    * Removes the specified lexicon from the permanent storage.
    * Removing a lexicon will remove all pronunciations within the lexicon.
    * @since 2.0
    * @param    aLexiconID            lexicon Id
    */
    void RemoveLexiconL( TSILexiconID aLexiconID );
    
    /**
    * Removes the specified model bank from the permanent storage.
    * Removing a model bank will remove all models within the model bank.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    */
    void RemoveModelBankL( TSIModelBankID aModelBankID );
    
    /**
    * Removes the specified model from the specified model bank permanently.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    * @param    aModelID            model Id
    */
    void RemoveModelL( TSIModelBankID aModelBankID, TSIModelID aModelID );
    
    /**
    * Removes the specified pronunciation from the specified lexicon permanently.
    * @since 2.0
    * @param    aLexiconID            lexicon Id
    * @param    aPronunciationID    pronunciation Id
    */
    void RemovePronunciationL( TSILexiconID aLexiconID, TSIPronunciationID aPronunciationID );
    
    /**
    * Removes the specified rule from the specified grammar permanently.
    * @since 2.0
    * @param    aGrammarID            grammar Id
    * @param    aRuleID                rule Id
    */
    void RemoveRuleL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );

    /**
    * Removes the specified rules from the specified grammar permanently.
    * @since 2.0
    * @param    aGrammarID            grammar Id
    * @param    aRuleIDs            rule Ids
    */
    void RemoveRulesL(TSIGrammarID aGrammarID,RArray<TSIRuleID>& aRuleIDs );
    
    /**
    * Sets the client's UID for data ownership identification.
    * @since 2.0
    * @param    aClientUid            client's UID
    */
    void SetClientUid( TUid aClientUid );
    
    /**
    * Sets the priority settings for this controller.
    * @since 2.0
    * @param    aPrioritySettings    new priority settings
    */
    void SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings );
    
    /**
    * Starts a new recognition session.
    * @since 2.0
    */
    void StartRecSessionL();
    
    /**
    * Trains a new model into the specified model bank.
    * @since 2.0
    * @param    aModelBankID        model bank Id
    * @param    aModelID            reference where a new model Id is assigned to
    */
    void TrainL( TSIModelBankID aModelBankID, TSIModelID& aModelID );
    
    /**
    * Unloads the specified rule from the specified grammar in temporary memory, previously
    * loaded with LoadGrammarL.  The rule in the permanent storage remains intact.
    * @since 2.0
    * @param    aGrammarID            grammar Id
    * @param    aRuleID                rule Id
    */
    void UnloadRuleL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
    
    
    // SI component
    
    /**
    *  
    * Does speaker adapation to speaker independent models
    * @since 2.8
    * @param    aResultSet            a recognition result containing adaptation data
    * @param    aCorrect            identifies the correct recognition result from the N-best list.
    */
    void AdaptL( CSIClientResultSet& aResultSet ,TInt aCorrect );

    /**
    * Adds a new pronunciation for the given model into the specified lexicon.
    * @since 2.0
    * @param    aLexiconID            lexicon Id
    * @param    aTrainText:            the text to be trained
    * @param    aTrainText:            the text to be trained        
    * @param    aLanguage            the pronunciation language    
    * @param    aPronunciationID: reference where a new pronunciation identifier is set.
    */
    void AddPronunciationL( TSILexiconID aLexiconID,
                            const TDesC& aTrainText, 
                            TLanguage aLanguage, 
                            TSIPronunciationID& aPronunciationID );

    /**
    * From MSpeechRecognitionCustomCommandImplementor
    * Adds a new rule variant for the given pronunciation into the specified grammar.
    * @since 2.0
    * @param    aGrammarID            grammar Id, where the new rule is added to
    * @param    aLexiconID            lexicon Id
    * @param    aPronunciationIDs    pronunciation Id array
    * @param    aRuleVariantID        reference where a new rule Variant Id is assigned to
    */
    void AddRuleVariantL( TSIGrammarID aGrammarID, 
                          TSILexiconID aLexiconID,
                          RArray<TSIPronunciationID>& aPronunciationIDs, 
                          TSIRuleID aRuleID,TSIRuleVariantID& aRuleVariantID );

    /**
    * Trains a new voice tag. The recognition phrase is the concatenation of words in the aTrainArray. 
    * Pronunciations are created with all given languages, which are supported.
    * @since 2.8
    * @param    aTrainArray:        Recognition phrase is formed by concatenating the descriptors in this array.
    * @param    aLanguageArray        Pronunciations are created with these languages. Not all languages are supported. The argument can be NULL.
    * @param    aLexiconID            lexicon Id
    * @param    aGrammarID            grammar Id, where the new rule is added to
    * @param    aRuleID        reference where a new rule Id is assigned to
    */
    void AddVoiceTagL( MDesCArray& aTrainArray, 
                       RArray<TLanguage>& aLanguageArray, 
                       TSILexiconID aLexiconID, 
                       TSIGrammarID aGrammarID, 
                       TSIRuleID& aRuleID );
    
    /**
    * Adds several new voice tags.
    * @since 2.8
    * @param    aTrainArray : An array of MDesCArrays. The recognition phrases are formed by concatenating the descriptors in the MDesCArrays.
    * @param    aLanguageArray        Pronunciations are created with these languages. Not all languages are supported. The argument can be NULL.
    * @param    aLexiconID            lexicon Id
    * @param    aGrammarID            grammar Id, where the new rule is added to
    * @param    aRuleID        reference where a new rule Id is assigned to
    */
    void AddVoiceTagsL( RPointerArray<MDesCArray>& aTrainArray,
                        RArray<TLanguage>& aLanguageArray, 
                        TSILexiconID aLexiconID, 
                        TSIGrammarID aGrammarID,
                        RArray<TSIRuleID>& aRuleID );
    
#ifdef __SINDE_TRAINING
    /**
    * Trains one new SINDE voice tag. The recognition phrase is the 
    * concatenation of words in the aTrainArray. Pronunciations are 
    * created with all given languages, which are supported.
    *
    * @since 3.1
    * @param aTrainArray Recognition phrase is formed by concatenating 
    *                    the descriptors in this array.
    * @param aLanguageArray Pronunciations are created with these languages..
    * @param aLexiconID Lexicon Id
    * @param aGrammarID Grammar Id, where the new rule is added to.
    * @param aRuleID Reference where a new rule Id is assigned to.
    */
    void AddSindeVoiceTagL( MDesCArray& aTrainArray, 
                            RArray<RLanguageArray>& aLanguageArray, 
                            TSILexiconID aLexiconID, 
                            TSIGrammarID aGrammarID, 
                            TSIRuleID& aRuleID );
    
    /**
    * Adds several new SINDE voice tags.
    *
    * @since 3.1
    * @param aTrainArray Recognition phrase is formed by concatenating 
    *                    the descriptors in this array.
    * @param aLanguageArray Pronunciations are created with these languages..
    * @param aLexiconID Lexicon Id
    * @param aGrammarID Grammar Id, where the new rule is added to.
    * @param aRuleID Reference where a new rule Ids are assigned to.
    */
    void AddSindeVoiceTagsL( RPointerArray<MDesCArray>& aTrainArray,
                             RArray<RLanguageArray>& aLanguageArray, 
                             TSILexiconID aLexiconID, 
                             TSIGrammarID aGrammarID,
                             RArray<TSIRuleID>& aRuleID );

#endif // __SINDE_TRAINING

    /**
    * Creates a new empty rule.
    * @since 2.8
    * @param    aGrammarID            grammar Id, where the new rule is added to
    * @param    aRuleID        reference where a new rule Id is assigned to
    */
    void CreateRuleL( TSIGrammarID aGrammarID, TSIRuleID& aRuleID );

    /**
    * Initiates recognition; performed following loading of model bank, lexicon, and grammar.
    * @since 2.8
    * @param    aResultSet: reference where the recognition result is set. 
    */   
    void RecognizeL( CSIClientResultSet& aResultSet );

    /**
    * Ends recording. Unlike Cancel(), this function may return a recognition 
    * result if adequate number of samples was already recorded.        
    * @since 2.8
    */         
    void EndRecordL();

    /**
    * Unloads the specified grammar from the temporary memory, 
    * previously loaded with MSrLoadGrammarL. The grammar in the permanent storage remains intact. 
    * result if adequate number of samples was already recorded.        
    * @since 2.0
    * @param    aGrammarID            grammar Id, where the new rule is added to
    */ 
    void UnloadGrammarL( TSIGrammarID aGrammarID );

    /**
     * Activate the grammar for the recognition         
    * @since 2.8
    * @param    aGrammarID            grammar Id 
    */
    void  ActivateGrammarL( TSIGrammarID aGrammarID ) ;
    /**
     * Deactivate the grammar          
    * @since 2.8
    * @param    aGrammarID            grammar Id
    */    
    void DeactivateGrammarL( TSIGrammarID aGrammarID );     
        
    /**
    * Create a empty rule into a given grammar
    * @since 2.8
    * @param    aGrammar  : a compiled grammar 
    * @return    Created rule
    */    
    CSIRule* CreateNewRuleL( CSICompiledGrammar* aGrammar );
    
    /**
    * Create a pronunciation into a given lexicon
    * @since 2.8
    * @param    aLexicon          : a lexicon  
    * @param    aPronunciationPr  : phoneme sequence of the pronunciation
    * @param    aModelBankID      : modelbank id 
    * @return    Created pronunciation id
    */            
    TSIPronunciationID CreateNewPronunciationL( CSILexicon* aLexicon,
                                                TDesC8& aPronunciationPr,
                                                TSIModelBankID aModelBankID );
    
    /**
    * Add rule variants into a given rule
    * @since 2.8
    * @param    aRule              :    a given rule
    * @param    aLexiconID          : a lexicon ID, the added rule variant's pronunciations belong to this lexicon
    * @param    aPronunciationIDs : array of Pronunciation IDs,
    * @param    aParameters          : other parameters,such as language ID, for rule variants
    */            
    void AddNewRuleVariantL( CSIRule& aRule,
                             TSILexiconID aLexiconID,
                             RArray<TSIPronunciationID>& aPronunciationIDs, 
                             CSIParameters& aParameters );    
    
    // Test functions
    CSIRule*  CreateRule1L(void);    
    CSIRule*  CreateRule2L(void);
    void CheckRule1L(const CSIRule* aRule);
    void CheckRule2L(const CSIRule* aRule);
#ifdef __CONSOLETEST__
    
    // Unit Test
    public:
        
        // Returns reference to a database
        RDbNamedDatabase& GetDb();
        
        // Returns reference to model bank array
        RPointerArray<CSIModelBank>& ModelBankArray();
        
        // Returns current plugin state
        TInt PluginState();
#endif
        
    private:
        
        /**
        * C++ default constructor.
        */
        CSIControllerPlugin( CSIControllerPluginInterface& aControllerIf );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * RunL from CActive class.
        */
        void RunL();
        
        /**
        * Cancel handle from CActive class.
        */
        void DoCancel();
        
        /**
        * This method completes the request status and set the object active
        * to provide asynchronous behavior.
        * @since 2.0
        */
        void DoAsynch();
        
        /**
        * Plays the previously trained utterance.
        * @since 2.0
        * @param    aModelBankID        model bank Id
        * @param    aModelID            model Id, whose utterance is played
        */
        void PlayL( TSIModelBankID aModelBankID, TSIModelID aModelID );
        
        /**
        * Handles DevASR events received while in Training state.
        * @since 2.0
        * @param    aEvent                event code
        * @param    aError                error code
        */
        void HandleEventTrainL( TDevASREvent aEvent, TDevASRError aError );
        
        /**
        * Handles DevASR events received while in Recognition state.
        * @since 2.0
        * @param    aEvent                event code
        * @param    aError                error code
        */
        void HandleEventRecognize( TDevASREvent aEvent, TDevASRError aError );

        /**
        * Handles DevASR events received while in remove rule state.
        * @since 2.8
        * @param    aEvent                event code
        * @param    aError                error code
        */    
        void HandleEventRemoveRule( TDevASREvent aEvent, TDevASRError aError );

        /**
        * Handles DevASR events received while in remove rules state.
        * @since 2.8
        * @param    aEvent                event code
        * @param    aError                error code
        */    
        void HandleEventRemoveRules( TDevASREvent aEvent, TDevASRError aError );
        
        /**
        * Handles DevASR events received while in TTP state.
        * @since 2.8
        * @param    aEvent                event code
        * @param    aError                error code
        */        
        void HandleEventTTP( TDevASREvent aEvent,
                             TDevASRError aError );
        
        /**
        * Handles DevASR events received while in Play state.
        * @since 2.0
        * @param    aEvent                event code
        * @param    aError                error code
        */
        void HandleEventPlayL( TDevASREvent aEvent, TDevASRError aError );
     
        /**
        * Frees memory allocated during recognition and resets the state to Idle.
        * @since 2.0
        */
        void RecognitionReset();
        
        /**
        * Handles the acoustic model and the user utterance from DevASR received upon
        * TrainComplete event.
        * @since 2.0
        */
        void ProcessTrainCompleteL();
        
        /**
        * Handles the transfer of recognition result from DevASR into client's
        * recognition result.
        * @since 2.0
        * @return    Number of results to the client
        */
        TInt ProcessRecognitionComplete();
        
        /**
        * Save TTP result into the given grammar and lexicon of the plugin database
        * @since 2.8
        * @param    aSITtpWordList      :    data structure contains ttp results, word entries and their corresponding prounication
        * @param    aLexiconID          : a lexicon ID, the added rule variant's pronunciations belong to this lexicon
        * @param    aRuleIDs          : array of generated Rule IDs,
        * @param    aParameters          : other parameters,such as language ID, for rule variants
        */
//        void  UpdateGrammarAndLexiconL(CSITtpWordList* aSITtpWordList , TSILexiconID aLexiconID, TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs);
        
        /**
        * Save TTP result into the given grammar and lexicon of the plugin database
        * @since 2.8
        * @param    aSITtpWordList      :    data structure contains ttp results, word entries and their corresponding prounication
        * @param    aLexiconID          : a lexicon ID, the added rule variant's pronunciations belong to this lexicon
        * @param    aGrammarID          : a grammar Id                
        * @param    aRuleIDs          : array of generated Rule IDs,
        */
        void  UpdateGrammarAndLexiconDBL( CSITtpWordList* aSITtpWordList, 
                                          TSILexiconID aLexiconID, 
                                          TSIGrammarID aGrammarID, 
                                          RArray<TSIRuleID>& aRuleIDs );
         
        /**
        * Is the lexicon id valid in the database
        * @since 2.8
        * @param    aLexiconID          : a lexicon ID,  
        * @return    True or false
        */
        TBool  IsLexiconIDInvalid( TSILexiconID aLexiconID );
    
        /**
        * Is the Grammar id valid in the database
        * @since 2.8
        * @param    aGrammarID          : a grammar Id
        * @return    True or false
        */
        TBool  IsGrammarIDInvalid( TSIGrammarID aGrammarID );

        // ============================================================================
        // These methods are handlers for asynchronous messages that are called from
        // the RunL.  The parameters are identical to how they were initially received
        // under Public section of this class, therefore not repeated here.
        // ============================================================================
        //
//        void HandleAddPronunciationL( TSILexiconID aLexiconID, TSIModelBankID aModelBankID,
//            TSIModelID aModelID, TSIPronunciationID& aPronunciationID );
        
//        void HandleAddPronunciationL( TSILexiconID aLexiconID, TSIModelBankID aModelBankID,
//            TDesC8& aPronunciationPr, TSIPronunciationID& aPronunciationID );
        
        void HandleAddRuleL( TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
            TSIPronunciationID aPronunciationID, TSIRuleID& aRuleID );
        
        void HandleCreateGrammarL( TSIGrammarID& aGrammarID );
        
        void HandleCreateLexiconL( TSILexiconID& aLexiconID );
        
        void HandleCreateModelBankL( TSIModelBankID& aModelBankID );
        
        void HandleGetAllClientGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs );
        
        void HandleGetAllClientLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs );
        
        void HandleGetAllClientModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs );
        
        void HandleGetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs );
        
        void HandleGetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs );
        
        void HandleGetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs );
        
        void HandleGetAllModelIDsL( TSIModelBankID aModelBankID, RArray<TSIModelID>& aModelIDs );
        
        void HandleGetAllPronunciationIDsL( TSILexiconID aLexiconID,
            RArray<TSIPronunciationID>& aPronunciationIDs );
        
        void HandleGetAllRuleIDsL( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs );
        
        void HandleGetAvailableStorageL( TInt& aCount );
        
        void HandleGetModelCountL( TSIModelBankID aModelBankID, TInt& aCount );
        
        void HandleGetPronunciationCountL(TSIModelBankID aLexiconID,TInt& aCount );
        
        void HandleGetRuleCountL(TSIGrammarID aGrammarID,TInt& aCount );

        void HandleGetRuleValidityL( TSIGrammarID aGrammarID, TSIRuleID aRuleID, TBool& aValid );
        
        void HandleGetUtteranceDurationL( TSIModelBankID aModelBankID, TSIModelID aModelID,
            TTimeIntervalMicroSeconds32& aDuration );
        
        void HandleLoadGrammarL( TSIGrammarID aGrammarID );
        
        void HandleLoadLexiconL( TSILexiconID aLexiconID );
        
        void HandleLoadModelsL( TSIModelBankID aModelBankID );
                
        void HandleRecognizeL();
        
        void HandleRecordL( TTimeIntervalMicroSeconds32 aRecordTime );
        
        void HandleRemoveGrammarL( TSIGrammarID aGrammarID );
        
        void HandleRemoveLexiconL( TSILexiconID aLexiconID );
        
        void HandleRemoveModelBankL( TSIModelBankID aModelBankID );
        
        void HandleRemoveModelL( TSIModelBankID aModelBankID, TSIModelID aModelID );
        
        void HandleRemovePronunciationL( TSILexiconID aLexiconID, TSIPronunciationID aPronunciationID );
        
        void HandleRemoveRuleL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
        
        void  HandleRemoveRulesL( TSIGrammarID aGrammarID,RArray<TSIRuleID>& aRuleIDs );
        
        void HandleTrainL( TSIModelBankID aModelBankID );
        
        void HandleUnloadRuleL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
        
        void HandlePlayUtteranceL(TSIModelBankID aModelBankID, TSIModelID aModelID); 
        // SI component
        
        void HandleAdaptL(CSIClientResultSet& aResultSet ,TInt aCorrect);
        
        
//        void HandleAddPronunciationL(TSILexiconID aLexiconID,  TPtrC& aTrainText,
//            TLanguage aLanguage, TSIPronunciationID* aPronunciationID) ;
        
        void HandleAddRuleVariantL(TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
            RArray<TSIPronunciationID>& aPronunciationIDs,TSIRuleID aRuleID, TSIRuleVariantID& aRuleVariantID );
        
        void HandleAddVoiceTagL( MDesCArray& aTrainArray,  RArray<TLanguage>& aLanguageArray, 
            TSILexiconID aLexiconID, TSIGrammarID aGrammarID, TSIRuleID* aRuleIDPtr);
        
        void HandleAddVoiceTagsL( RPointerArray<MDesCArray>* aTrainArray,  RArray<TLanguage>& aLanguageArray, TSILexiconID aLexiconID,
            TSIGrammarID aGrammarID/*, 
            RArray<TSIRuleID>& aRuleID*/);

#ifdef __SINDE_TRAINING            
        void HandleAddSindeVoiceTagsL( RPointerArray<MDesCArray>* aTrainArrays,
                                       RArray<RLanguageArray>& aLanguageArray,
                                       TSILexiconID aLexiconID,
                                       TSIGrammarID aGrammarID );
#endif // __SINDE_TRAINING
        
        void HandleCreateRuleL(TSIGrammarID aGrammarID, TSIRuleID& aRuleID) ;
        
//        void HandleRecognizeL( CSIClientResultSet& aResultSet );
        
        void HandleEndRecordL();
        
        void HandleUnloadGrammarL( TSIGrammarID aGrammarID );
        
//        void  HandleUpdateGrammarAndLexiconL(CSITtpWordList* iSITtpWordList , TSILexiconID aLexiconID, TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleID);
        
        void HandlePreStartSamplingL();
        
    private:    // Data
        
        // SI Controller Plugin states
        enum TControllerState
            {
            ESiPluginIdle,
            ESdPluginTrain,
            ESiPluginRecognize,
            ESiPluginAdapt,
            ESiPluginTrainText,
            ESiPluginRemoveRule ,
            ESiPluginRemoveRules,
            ESiPluginPlay,
            ESiPluginTrainSinde
            };
        
        
         // SI Controller Plugin state
        TControllerState iState;
        // Result to be sent to the client
        TInt iResult;
        
        // Reference to Controller Interface
        CSIControllerPluginInterface& iControllerIf;

        // SI Database
        CSIDatabase* iSIDatabase;
        
        
        // SI part
        // Model Bank database
        CSIModelBankDB* iSIModelBankDB;
        // Lexicon database
        CSILexiconDB* iSILexiconDB;
        // Grammar database
        CSIGrammarDB* iSIGrammarDB;
        TBool iVoiceTagDBLocked;        
        // Resource file handler
        CSIResourceHandler* iResourceHandler;
        
        // Client's UID for data ownership identification
        TUid iClientUid;
        TBool iClientRegistered;
        
        // DevASR
        CDevASR* iDevASR;
        
        CSIResultSet* iSIResultSet;                    // Used during recognition
        
        /// Begin: data used in async handling of requests
        TInt iRequestFunction;
        TSIGrammarID iGrammarID;
        TSILexiconID iLexiconID;
        TSIModelBankID iModelBankID;
        TSIRuleID iRuleID;
        TSIRuleVariantID iRuleVariantID;
        TSIPronunciationID iPronunciationID;
        //RArray<TSIPronunciationID> iPronunciationIDs;
        TSIModelID iModelID;
        TTimeIntervalMicroSeconds32 iRecordTime;
        TSIGrammarID* iGrammarIDPtr;
        TSILexiconID* iLexiconIDPtr;
        
        TSIModelBankID* iModelBankIDPtr;
        TSIRuleID* iRuleIDPtr;
        TSIRuleVariantID* iRuleVariantIDPtr;
        TSIPronunciationID* iPronunciationIDPtr;
        TSIModelID* iModelIDPtr;
        TInt* iCountPtr;
        TBool* iValidPtr;
        TTimeIntervalMicroSeconds32* iDurationPtr;


     
        RArray<TSIGrammarID>* iGrammarIDs;
        RArray<TSILexiconID>* iLexiconIDs;

        RArray<TSIModelBankID>* iModelBankIDs;
        RArray<TSIModelID>* iModelIDs;
        RArray<TSIPronunciationID>* iPronunciationIDs;
        RArray<TSIRuleID>* iRuleIDs;

        // End: data used in async handling of requests
        
        
        // SI componet
        CSIClientResultSet* iSIClientResultSet ;

        TInt iCorrect;     
        TPtrC iTrainTextPtr;
        TLanguage iLanguage;
        RArray<TLanguage>* iLanguageArray;
        RArray<RLanguageArray>* iLanguageArrayArray;
        MDesCArray *iTrainArray ;
        RPointerArray<MDesCArray>* iTrainArrays;
        CSITtpWordList* iSITtpWordList;    
        RArray<TUint32> iMaxNPronunsForWord;
        TPtrC8 iPronunciationPr;
        CSICompiledGrammar *iSICompiledGrammar;
        TSIPronunciationID* iLastAddedPronunciationIDPtr;
        TSIRuleID* iLastAddedRuleIDPtr;

        // recompile grammar during remove rule .
        CSICompiledGrammar* iSICompiledGrammarRecompile; // Pointer to grammar to in remove rule
        TSIGrammarID iGrammarIDRecompile;
        TBool iIsGrammarRecompile;
        TSIRuleID iRuleIDRemove;
        TBool iIsRecompileGrammarActive;

        // TTP data loader
        
        //CTTPDataLoader* iTtpLoader;
        CDataLoader* iDataLoader;
        
        // Audio 
        HBufC8* iAudioBuffer;
        TInt iNSamplesSent;
        TPtrC8 iCurrentAudioBuffer;
        
        TTimeIntervalMicroSeconds32 iRecordDurations;
        //TSIRuleVariantID iRuleVariantID;
         RDbNotifier iRDbNotifier;
         
#ifdef __SINDE_TRAINING         
         // Class which takes care of SINDE type of training process
         CSindeTrainer* iSindeTrainer;
#endif // __SINDE_TRAINING
    };
    
#endif // __CSIControllerPlugin_H__
    
// End of File
