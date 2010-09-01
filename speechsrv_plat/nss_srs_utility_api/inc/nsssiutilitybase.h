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
* Description:  Base class for TTS utility API
*
*/


#ifndef NSSSIUTILITYBASE_H
#define NSSSIUTILITYBASE_H

// INCLUDES
#include <e32base.h>
#include <nsssispeechrecognitiondataclient.h>
#include <nsssispeechrecognitionutilityobserver.h>

// CLASS DECLARATION

/**
* MSISpeechRecognitionUtilityBase
* Interface class for CSISpeechRecognitionUtility
* @since 3.1
*/
class MSISpeechRecognitionUtilityBase
    {
    public:     
   
        /**
        * Second phase construction.
        * Needed when dynamically loading nsssispeechrecognitionutility.dll
        * Direct users of CSISpeechRecognitionUtility should not use this.
        *
        * @since 3.1
        * @param aClientUid Client UID for data ownership, other clients won't
        *                   be able to modify the data registered to certain client
        * @param aPluginUid UID of the plugin which will be used, KNullUid to 
        *                    search for suitable plugin
        * @param aObserver Callback observer reference
        */
        virtual void CreateInstanceSecondPhaseL( TUid aClientUid, 
                                                 TUid aPluginUid, 
                                                 MSISpeechRecognitionUtilityObserver& aObserver ) = 0;
                                                 
        /**
        * Destructor. Defined here so that client can use M-class pointer to destroy
        * the concrete C-object
        */                                         
        virtual ~MSISpeechRecognitionUtilityBase() {};
           
        /**
        * Returns the engine properties specified by the engine property 
        * ID array. On return aPropertyValue contains an array of engine 
        * properties.
        *
        * @since 3.1
        * @param aPropertyId constant reference to array of engine property Ids
        * @param aPropertyValue reference to array of engine property values
        * @return system wide error code
        */
        virtual TInt GetEngineProperties( const RArray<TInt>& aPropertyId, 
                                          RArray<TInt>& aPropertyValue ) = 0;

        /**
        * Adapts the speaker independent models to a specific speaker's voice.
        *
        * @since 3.1
        * @param aResultSet Recognition result N-Best
        * @param aCorrect Index to N-Best pointing the correct result.
        * @return system wide error code
        */
        virtual TInt Adapt( const CSIClientResultSet& aResultSet,
                            TInt aCorrect ) = 0;

        /**
        * Adds a new pronunciation for the given text phrase into the 
        * specified lexicon.
        *
        * @since 3.1
        * @param aLexiconID lexicon Id, where the new pronunciation is added to
        * @param aTextForTraining Text to be trained
        * @param aLanguage Pronunciation language
        * @param aPronunciationID reference where a new pronunciation Id is assigned to
        * @return system wide error code
        */
        virtual TInt AddPronunciation( TSILexiconID aLexiconID,
                                       const TDesC& aTextForTraining,
                                       TLanguage aLanguage,
                                       TSIPronunciationID& aPronunciationID ) = 0;

        /**
        * Adds a new rule for the given pronunciation into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aGrammarID grammar Id, where the new rule is added to
        * @param aLexiconID lexicon Id
        * @param aPronunciationID pronunciation Id
        * @param aRuleID reference where a new rule Id is assigned to
        * @return system wide error code
        */
        virtual TInt AddRule( TSIGrammarID aGrammarID,
                              TSILexiconID aLexiconID,
                              TSIPronunciationID aPronunciationID,
                              TSIRuleID& aRuleID ) = 0;

        /**
        * Adds a new rule variant for the given pronunciation into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aGrammarID grammar Id, where the new rule is added to
        * @param aRuleID the rule Id, which will contain this variant
        * @param aPronunciationIDs the phrase is the concatenation of these pronunciation IDs
        * @param aRuleVariantID reference where a new rule variant Id is assigned to
        * @return system wide error code
        */
        virtual TInt AddRuleVariant( TSIGrammarID aGrammarID,
                                     TSILexiconID aLexiconID,
                                     RArray<TSIPronunciationID>& aPronunciationIDs,
                                     TSIRuleID aRuleID,
                                     TSIRuleVariantID& aRuleVariantID ) = 0;

        /**
        * Adds a new rule for the given phrase into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aTextArray the text to be trained, divided in to words.
        *                   The phrase is the concatenation of these words.
        * @param aLanguageArray pronunciations will be generated for these languages
        * @param aLexiconID lexicon, where to add pronunciations
        * @param aGrammarID grammar, where to add rules
        * @param aRuleID the ID of the rule will be placed on this variable.
        * @return system wide error code
        */
        virtual TInt AddVoiceTag( MDesCArray& aTextArray,
                                  const RArray<TLanguage>& aLanguageArray,
                                  TSILexiconID aLexiconID,
                                  TSIGrammarID aGrammarID,
                                  TSIRuleID& aRuleID ) = 0;

        /**
        * Adds a new rules for the given phrases into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aTextArrayArray Array of text arrays. The phrase k is the concatenation
        *                        of texts in array k.
        * @param aLanguageArray pronunciations will be generated for these languages
        * @param aLexiconID lexicon, where to add pronunciations
        * @param aGrammarID grammar, where to add rules
        * @param aRuleID the ID of the rule will be placed on this variable.
        *                If some of the names fail while others succeed,
        *                KInvalidRuleID will be placed to the array.
        * @return system wide error code
        */
        virtual TInt AddVoiceTags( const RPointerArray<MDesCArray> &aTextArrayArray,
                                   const RArray<TLanguage> &aLanguageArray,
                                   TSILexiconID aLexiconID,
                                   TSIGrammarID aGrammarID, 
                                   RArray<TSIRuleID>& aRuleIDArray ) = 0;

        /**
        * Adds a new rule for the given phrase into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aTextArray The text to be trained, divided in to words.
        *                   The phrase is the concatenation of these words.
        * @param aLanguageArray Pronunciations will be generated for these languages
        * @param aLexiconID Lexicon, where to add pronunciations
        * @param aGrammarID Grammar, where to add rules
        * @param aRuleID The ID of the rule will be placed on this variable.
        * @return System wide error code
        */
        virtual TInt AddVoiceTag( MDesCArray& aTextArray,
                                  const RArray<RLanguageArray>& aLanguageArray,
                                  TSILexiconID aLexiconID,
                                  TSIGrammarID aGrammarID,
                                  TSIRuleID& aRuleID ) = 0;

        /**
        * Adds a new rules for the given phrases into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aTextArrayArray Array of text arrays. The phrase k is the concatenation
        *                        of texts in array k.
        * @param aLanguageArray pronunciations will be generated for these languages
        * @param aLexiconID lexicon, where to add pronunciations
        * @param aGrammarID grammar, where to add rules
        * @param aRuleID the ID of the rule will be placed on this variable.
        *                If some of the names fail while others succeed,
        *                KInvalidRuleID will be placed to the array.
        * @return System wide error code
        */
        virtual TInt AddVoiceTags( const RPointerArray<MDesCArray>& aTextArrayArray,
                                   const RArray<RLanguageArray>& aLanguageArray,
                                   TSILexiconID aLexiconID,
                                   TSIGrammarID aGrammarID, 
                                   RArray<TSIRuleID>& aRuleIDArray ) = 0;

        /**
        * Cancels the current operation
        *    
        * @since 3.1
        */
        virtual void Cancel() = 0;

        /**
        * Saves the changes into a permanent storage.
        *
        * @since 3.1
        * @return System-wide errors
        */
        virtual TInt CommitChanges() = 0;

        /**
        * Creates a new grammar.
        *    
        * @since 3.1
        * @param aGrammarID reference where grammar ID is stored
        * @return system wide error codes
        */
        virtual TInt CreateGrammar( TSIGrammarID& aGrammarID ) = 0;

        /**
        * Creates a new lexicon.
        *    
        * @since 3.1
        * @param aLexiconID reference where lexicon ID is stored
        * @return system wide error codes
        */
        virtual TInt CreateLexicon( TSILexiconID& aLexiconID ) = 0;

        /**
        * Creates a new rule.
        *    
        * @since 3.1
        * @param aGrammarID grammar, where the new rule is to be added
        * @param aRuleID reference where rule ID is stored
        * @return system wide error codes
        */
        virtual TInt CreateRule( TSIGrammarID aGrammarID, TSIRuleID& aRuleID ) = 0;

        /**
        * Loads the specified model bank into the recognizer.
        * done prior to recognition.
        *
        * @since 3.1
        * @param ModelBankID model bank Id
        * @return system wide error codes
        */
        virtual TInt LoadModels( TSIModelBankID aModelBankID ) = 0;

        /**
        * Loads the specified grammar into the recognizer.
        * done prior to recognition.
        *
        * @since 3.1
        * @param aGrammarID grammar ID
        * @return system wide error codes
        */
        virtual TInt LoadGrammar( TSIGrammarID aGrammarID ) = 0;

        /**
        * Reactivates a previously deactivated grammar.
        *
        * @since 3.1
        * @param aGrammarID grammar ID
        * @return system wide error codes
        */
        virtual TInt ActivateGrammar( TSIGrammarID aGrammarID ) = 0;

        /**
        * Deactivates a loaded grammar, so that it is not recognized.
        *
        * @since 3.1
        * @param aGrammarID grammar ID
        * @return system wide error codes
        */
        virtual TInt DeactivateGrammar( TSIGrammarID aGrammarID ) = 0;

        /**
        * Loads the specified lexicon into the recognizer; 
        * done prior to recognition.
        *
        * @since 3.1
        * @param aLexiconID lexicon ID
        * @return system wide error codes
        */
        virtual TInt LoadLexicon( TSILexiconID aLexiconID ) = 0;

        /**
        * Returns the number of models in the specified model bank.
        *
        * @since 3.1
        * @param aModelBankID model bank Id
        * @param aCount reference where the number of 
        *               models is set
        * @return system wide error codes
        */
        virtual TInt GetModelCount( TSIModelBankID aModelBankID, 
                                    TInt& aModelCount ) = 0;

        /**
        * Ends the current recognition session. Resources allocated 
        * for recognition are freed.
        *
        * @since 3.1
        * @param aModelBankID reference where a new model bank Id is assigned to
        * @return system wide error codes
        */
        virtual TInt EndRecSession() = 0;

        /**
        * Starts a new recognition session.
        *
        * @since 3.1
        * @param aMode recognition mode
        * @return system wide error codes
        */
        virtual TInt StartRecSession( TNSSRecognitionMode aMode ) = 0;

        /**
        * Initiates speaker independent recognition; performed
        * following loading of model bank, lexicon, and grammar.
        *
        * @since 3.1
        * @param aResultSet reference where the recognition result is set
        * @return system wide error codes
        */
        virtual TInt Recognize( CSIClientResultSet& aResultSet ) = 0;

        /**
        * Records utterance for recognition.
        *
        * @since 3.1
        * @param aRecordTime recording time in microseconds
        * @return system wide error codes
        */
        virtual TInt Record( TTimeIntervalMicroSeconds32 aRecordTime ) = 0;

        /**
        * Stops the recording. The difference with Cancel is that if
        * enough samples were recorded, a recognition result will be produced.
        *
        * @since 3.1
        * @param aRecordTime recording time in microseconds
        * @return system wide error codes
        */
        virtual TInt EndRecord() = 0;

        /**
        * Pre-starts samping before Record call.
        * 
        * @since 3.2
        * @return System wide error code
        */
        virtual TInt PreStartSampling() = 0;

        /**
        * Removes the specified grammar from the permanent storage.
        * Removing a grammar will remove all rules within the grammar.
        *
        * @since 3.1
        * @param aGrammarID grammar Id
        * @return system wide error codes
        */
        virtual TInt RemoveGrammar( TSIGrammarID aGrammarID ) = 0;

        /**
        * Removes the specified pronunciation from the 
        * specified lexicon permanently.
        *
        * @since 3.1
        * @param aLexiconID lexicon Id
        * @param aPronunciationID pronunciation Id
        * @return system wide error codes
        */
        virtual TInt RemovePronunciation( TSILexiconID aLexiconID, 
                                          TSIPronunciationID aPronunciationID ) = 0;

        /**
        * Removes the specified lexicon from the permanent storage.
        * Removing a lexicon will remove all pronunciations within the 
        * lexicon.
        * 
        * @since 3.1
        * @param aLexiconID lexicon Id
        * @return system wide error codes
        */
        virtual TInt RemoveLexicon( TSILexiconID aLexiconID ) = 0;

        /**
        * Removes the specified model from the specified model bank 
        * permanently.
        *
        * @since 3.1
        * @param aModelBankID model bank Id
        * @param aModelID model Id
        * @return system wide error codes
        */
        virtual TInt RemoveModel( TSIModelBankID aModelBankID, 
                                  TSIModelID aModelID ) = 0;

        /**
        * Removes the specified rule from the specified grammar permanently.
        *
        * @since 3.1
        * @param aGrammarID grammar Id
        * @param aRuleID rule Id
        * @return system wide error codes
        */
        virtual TInt RemoveRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID ) = 0;

        /**
        * Removes multiple rules from the specified grammar permanently.
        *
        * @since 3.1
        * @param aGrammarID grammar Id
        * @param aRuleID rule Id
        * @return system wide error codes
        */
        virtual TInt RemoveRules( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs ) = 0;

        /**
        * Unloads the specified rule from the specified grammar
        * in temporary memory, previously loaded with LoadGrammarL.  
        * The rule in the permanent storage remains intact.
        *
        * @since 3.1
        * @param aGrammarID grammar Id
        * @param aRuleID rule Id
        * @return system wide error codes
        */
        virtual TInt UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID ) = 0;

        /**
        * Unloads the specified grammar from the list of grammars
        * in temporary memory, previously loaded with LoadGrammarL.  
        * The grammar in the permanent storage remains intact.
        *
        * @since 3.1
        * @param aGrammarID grammar Id
        * @return system wide error codes
        */
        virtual TInt UnloadGrammar( TSIGrammarID aGrammarID ) = 0;

        /**
        * Set the event handler for asynchronous events
        *
        * @since 3.1
        * @param aSpeechRecognitionUtilityObserver pointer to observer
        */
        virtual void SetEventHandler( MSISpeechRecognitionUtilityObserver*
                                      aSpeechRecognitionUtilityObserver ) = 0;

        /**
        * Returns all pronunciation Ids that exist in the specified lexicon.
        *
        * @since 3.1
        * @param aLexiconID lexicon Id
        * @param aPronunciationIDs reference where pronunciation Ids are stored
        * @return system wide error codes
        */
        virtual TInt GetAllPronunciationIDs( TSILexiconID aLexiconID, 
                                             RArray <TSIPronunciationID>& aPronunciationIDs ) = 0;

        /**
        * Returns all grammar Ids that belong to the current client.
        *
        * @since 3.1
        * @param aGrammarIDs reference where grammar Ids are stored
        * @return System-wide errors
        */
        virtual TInt GetAllClientGrammarIDs( RArray<TSIGrammarID>& aGrammarIDs ) = 0;

        /**
        * Returns all lexicon Ids that belong to the current client.
        *
        * @since 3.1
        * @param aLexiconIDs reference where lexicon Ids are stored
        * @return System-wide errors
        */
        virtual TInt GetAllClientLexiconIDs( RArray<TSILexiconID>& aLexiconIDs ) = 0;

        /**
        * Returns all model bank Ids that belong to the current client.
        *
        * @since 3.1
        * @param aModelBankIDs reference where model bank Ids are stored
        * @return System-wide errors
        */
        virtual TInt GetAllClientModelBankIDs( RArray<TSIModelBankID>& aModelBankIDs ) = 0;

        /**
        * Returns all grammar Ids that exist (for all clients).
        *
        * @since 3.1
        * @param aGrammarIDs reference where grammar Ids are stored
        * @return System-wide errors
        */
        virtual TInt GetAllGrammarIDs( RArray<TSIGrammarID>& aGrammarIDs ) = 0;

        /**
        * Returns all lexicon Ids that exist (for all clients).
        *
        * @since 3.1
        * @param aLexiconIDs reference where lexicon Ids are stored
        * @return System-wide errors
        */
        virtual TInt GetAllLexiconIDs( RArray<TSILexiconID>& aLexiconIDs ) = 0;

        /**
        * Returns all model bank Ids that exist (for all clients).
        *
        * @since 3.1
        * @param aModelBankIDs reference where model bank Ids are stored
        * @return System-wide errors
        */
        virtual TInt GetAllModelBankIDs( RArray<TSIModelBankID>& aModelBankIDs ) = 0;

        /**
        * Returns all model Ids that exist in the specified model bank.
        * 
        * @since 3.1
        * @param aModelBankID model bank Id
        * @param aModelIDs reference where model Ids are stored
        * @return system wide error codes
        */
        virtual TInt GetAllModelIDs( TSIModelBankID aModelBankID, 
                                     RArray <TSIModelID>& aModelIDs ) = 0;

        /**
        * Checks if the rule is valid or not.
        *
        * @since 3.1
        * @param aGrammarID grammar Id
        * @param aRuleID rule Id
        * @param aValid reference where the validity of the rule is set
        * @return system wide error codes
        */
        virtual TInt GetRuleValidity( TSIGrammarID aGrammarID, TSIRuleID aRuleID,
                                      TBool& aValid ) = 0; 

        /**
        * Returns all rule Ids that exist in the specified grammar.
        *
        * @since 3.1
        * @param aGrammarID grammar Id
        * @param aRuleIDs reference where rule Ids are stored
        * @return system wide error codes
        */
        virtual TInt GetAllRuleIDs( TSIGrammarID aGrammarID, 
                                    RArray <TSIRuleID>& aRuleIDs ) = 0;

        /**
        * Creates a new model bank.
        *
        * @since 3.1
        * @param aModelBankID reference where a new model bank Id is assigned to
        * @return system wide error codes
        */
        virtual TInt CreateModelBank( TSIModelBankID& aModelBankID ) = 0;

        /**
        * Removes the specified model bank from the permanent storage.
        * Removing a model bank will remove all models within the model bank.
        *
        * @since 3.1
        * @param aModelBankID model bank Id
        * @return system wide error codes
        */
        virtual TInt RemoveModelBank( TSIModelBankID aModelBankID ) = 0;

        /**
        * Uploads the specified parameters into the engine.
        *
        * @since 3.1
        * @param aParameterId array of parameter IDs
        * @param aParameterValue array of parameter values
        * @return system wide error codes
        */
        virtual TInt LoadEngineParameters( const RArray<TInt>& aParameterId, 
                                           const RArray<TInt>& aParameterValue ) = 0;

        /**
        * Set the audio priority and preference recognition
        * 
        * @since 3.1
        * @param aPriority priority for voice recognition
        * @param aTrainPreference preference for training
        * @param aPlaybackPreference preference for playback
        * @param aRecognitionPreference preference for recognition
        * @return system wide error codes
        */
        virtual TInt SetAudioPriority( TInt aPriority, TInt aTrainPreference, 
                                       TInt aPlaybackPreference, TInt aRecognitionPreference ) = 0;

    };

#endif // NSSSIUTILITYBASE_H

// End of file
