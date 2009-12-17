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
* Description:  This is the definition of the speech recognition utility.
*
*/


#ifndef NSSSISPEECHRECOGNITIONUTILITY_H
#define NSSSISPEECHRECOGNITIONUTILITY_H

// INCLUDES

#include <e32base.h>
#include <mmfcontroller.h>
#include <nsssispeechrecognitioncustomcommands.h>
#include <nsssispeechrecognitionutilityobserver.h>
#include <nsssispeechrecognitiondataclient.h>
#include <nsssispeechrecognitiondatacommon.h>
#include <nsssiutilitybase.h>

// FORWARD DECLARATIONS
class CSDClientResultSet;

// CLASS DECLARATION

/**
*  This is the main class of Speech Recognition Utility 
*
*  @lib nsssispeechrecognitionutility.lib
*  @since 2.0
*/

class CSISpeechRecognitionUtility : public CBase,
                                    public MSISpeechRecognitionUtilityBase, 
                                    public MMMFControllerEventMonitorObserver
{

public:

        /*
        * Creates instance of CSISpeechRecognitionUtility
        * Needed when dynamically loading nsssispeechrecognitionutility.dll
        * Direct users of CSISpeechRecognitionUtility should not use this.
        * Returns M-class pointer to MSISpeechRecognitionUtilityBase as TAny*
        *
        * @return Created instance
        */        
        IMPORT_C static TAny* CreateInstanceL();

        /**
        * Second phase of construction after CreateInstanceL call.
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
        IMPORT_C void CreateInstanceSecondPhaseL( TUid aClientUid, 
                                                  TUid aPluginUid, 
                                                  MSISpeechRecognitionUtilityObserver& aObserver );

        /**
        * Factory function for creating this class.
        *
        * @since 2.0
        * @param    aSpeechRecognitionUtilityObserver    reference to event 
        *                                                observer class
        * @param    aClientUid                           client UID for data 
        *                                                ownership
        * @return   pointer to CSISpeechRecognitionUtility object
        */
        IMPORT_C static CSISpeechRecognitionUtility* NewL( 
                            MSISpeechRecognitionUtilityObserver& 
                            aSpeechRecognitionUtilityObserver, TUid aClientUid );

        /**
        * Factory function for creating this class.
        * Searches for a plugin, which supports the given
        * recognition mode. If no such plugin is found, leaves.
        *
        * @since 2.8
        * @param     aSpeechRecognitionUtilityObserver    reference to the event observer class
        * @param    aClientUid                            client UID for data ownership
        * @param    aPreferredMode                      Recognition technology (speaker
        *                                               independent or dependent)
        * @return    pointer to CSISpeechRecognitionUtility object
        */
        IMPORT_C static CSISpeechRecognitionUtility* NewL( 
                            MSISpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver,
                            TUid aClientUid,
                            TNSSRecognitionMode aPreferredMode );

        /**
        * Factory function for creating this class.
        * Loads the plugin with the given ID.
        *
        * @since 2.8
        * @param     aSpeechRecognitionUtilityObserver    reference to the event observer class
        * @param     aClientUid                           client UID for data ownership
        * @param     aMmfPluginUid                        UID of the speech recognition plugin
        * @return    pointer to CSISpeechRecognitionUtility object
        */
        IMPORT_C static CSISpeechRecognitionUtility* NewL( 
                            MSISpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver,
                            TUid aClientUid,
                            TUid aMmfPluginUid );

        /**
        * Factory function for creating this class. Leaves a pointer 
        * on the cleanup 
        *
        * @since 2.0
        * @param    aSpeechRecognitionUtilityObserver     reference to event
        *                                                 observer class
        * @param    aClientUid                            client UID for data 
        *                                                 ownership
        * @return    pointer to CSISpeechRecognitionUtility object
        */
        IMPORT_C static CSISpeechRecognitionUtility* NewLC( 
                            MSISpeechRecognitionUtilityObserver& 
                            aSpeechRecognitionUtilityObserver, TUid aClientUid );

        /**
        * Destructor
        *
        * @since 2.0
        */
        IMPORT_C ~CSISpeechRecognitionUtility();

        /**
        * Returns the engine properties specified by the engine property 
        * ID array. On return aPropertyValue contains an array of engine 
        * properties.
        *
        * @since 2.0
        * @param    aPropertyId      constant reference to array of engine 
        *                            property Ids
        * @param    aPropertyValue   reference to array of engine property 
        *                            values
        * @return   system wide error code
        */
        IMPORT_C TInt GetEngineProperties( const RArray<TInt>& aPropertyId, 
                        RArray<TInt>& aPropertyValue );

        /**
        * Adapts the speaker independent models to a specific speaker's voice.
        *
        * @since  2.8
        * @param  aResultSet     Recognition result N-Best
        * @param  aCorrect       Index to N-Best pointing the correct result.
        * @return system wide error code
        */
        IMPORT_C TInt Adapt( const CSIClientResultSet& aResultSet,
                             TInt aCorrect );

        /**
        * Adds a new pronunciation for the given text phrase into the 
        * specified lexicon.
        *
        * @since    2.8
        * @param    aLexiconID          lexicon Id, where the new 
        *                               pronunciation is added to
        *           aTextForTraining    Text to be trained
        *           aLanguage           Pronunciation language
        *           aPronunciationID    reference where a new pronunciation 
        *                               Id is assigned to
        * @return   system wide error code
        */
        IMPORT_C TInt AddPronunciation( TSILexiconID aLexiconID,
                        const TDesC& aTextForTraining,
                        TLanguage aLanguage,
                        TSIPronunciationID& aPronunciationID);

        /**
        * Adds a new rule for the given pronunciation into the 
        * specified grammar.
        *
        * @since    2.0
        * @param    aGrammarID            grammar Id, where the new rule is 
        *                                added to
        * @param    aLexiconID            lexicon Id
        * @param    aPronunciationID    pronunciation Id
        * @param    aRuleID                reference where a new rule Id is 
        *                                assigned to
        * @return    system wide error code
        */
        IMPORT_C TInt AddRule( TSIGrammarID aGrammarID,
                        TSILexiconID aLexiconID,
                        TSIPronunciationID aPronunciationID,
                        TSIRuleID& aRuleID );

        /**
        * Adds a new rule variant for the given pronunciation into the 
        * specified grammar.
        *
        * @since    2.8
        * @param    aGrammarID            grammar Id, where the new rule is 
        *                                added to
        * @param    aRuleID                the rule Id, which will contain this variant
        * @param    aPronunciationIDs    the phrase is the concatenation
        *                               of these pronunciation IDs
        * @param    aRuleVariantID        reference where a new rule variant
        *                               Id is assigned to
        * @return    system wide error code
        */
        IMPORT_C TInt AddRuleVariant( TSIGrammarID aGrammarID,
                        TSILexiconID aLexiconID,
                        RArray<TSIPronunciationID>& aPronunciationIDs,
                        TSIRuleID aRuleID,
                        TSIRuleVariantID& aRuleVariantID );

        /**
        * Adds a new rule for the given phrase into the 
        * specified grammar.
        *
        * @since    2.8
        * @param    aTextArray            the text to be trained, divided in to words.
        *                               The phrase is the concatenation of these words.
        * @param    aLanguageArray      pronunciations will be generated for these languages
        * @param    aLexiconID          lexicon, where to add pronunciations
        * @param    aGrammarID          grammar, where to add rules
        * @param    aRuleID             the ID of the rule will be placed on this variable.
        * @return    system wide error code
        */
        IMPORT_C TInt AddVoiceTag( MDesCArray& aTextArray,
                                   const RArray<TLanguage>& aLanguageArray,
                                   TSILexiconID aLexiconID,
                                   TSIGrammarID aGrammarID,
                                   TSIRuleID& aRuleID );

        /**
        * Adds a new rules for the given phrases into the 
        * specified grammar.
        *
        * @since    2.8
        * @param    aTextArrayArray     Array of text arrays. The phrase k is the concatenation
        *                               of texts in array k.
        * @param    aLanguageArray      pronunciations will be generated for these languages
        * @param    aLexiconID          lexicon, where to add pronunciations
        * @param    aGrammarID          grammar, where to add rules
        * @param    aRuleID             the ID of the rule will be placed on this variable.
        *                               If some of the names fail while others succeed,
        *                               KInvalidRuleID will be placed to the array.
        * @return    system wide error code
        */
        IMPORT_C TInt AddVoiceTags( const RPointerArray<MDesCArray> &aTextArrayArray,
                                    const RArray<TLanguage> &aLanguageArray,
                                    TSILexiconID aLexiconID,
                                    TSIGrammarID aGrammarID, 
                                    RArray<TSIRuleID>& aRuleIDArray );

        /**
        * Adds a new rule for the given phrase into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aTextArray        The text to be trained, divided in to words.
        *                          The phrase is the concatenation of these words.
        * @param aLanguageArray    Pronunciations will be generated for these languages
        * @param aLexiconID        Lexicon, where to add pronunciations
        * @param aGrammarID        Grammar, where to add rules
        * @param aRuleID           The ID of the rule will be placed on this variable.
        *
        * @return System wide error code
        */
        IMPORT_C TInt AddVoiceTag( MDesCArray& aTextArray,
                                   const RArray<RLanguageArray>& aLanguageArray,
                                   TSILexiconID aLexiconID,
                                   TSIGrammarID aGrammarID,
                                   TSIRuleID& aRuleID );

        /**
        * Adds a new rules for the given phrases into the 
        * specified grammar.
        *
        * @since 3.1
        * @param aTextArrayArray     Array of text arrays. The phrase k is the concatenation
        *                            of texts in array k.
        * @param aLanguageArray      pronunciations will be generated for these languages
        * @param aLexiconID          lexicon, where to add pronunciations
        * @param aGrammarID          grammar, where to add rules
        * @param aRuleID             the ID of the rule will be placed on this variable.
        *                            If some of the names fail while others succeed,
        *                            KInvalidRuleID will be placed to the array.
        * @return System wide error code
        */
        IMPORT_C TInt AddVoiceTags( const RPointerArray<MDesCArray>& aTextArrayArray,
                                    const RArray<RLanguageArray>& aLanguageArray,
                                    TSILexiconID aLexiconID,
                                    TSIGrammarID aGrammarID, 
                                    RArray<TSIRuleID>& aRuleIDArray );

        /**
        * Cancels the current operation
        *    
        * @since 2.0
        */
        IMPORT_C void Cancel();

        /**
        * Saves the changes into a permanent storage.
        *
        * @since 2.0
        * @return    System-wide errors
        */
        IMPORT_C TInt CommitChanges();

        /**
        * Creates a new grammar.
        *    
        * @since 2.0
        * @param     aGrammarID            reference where grammar ID is stored
        * @return    system wide error codes
        */
        IMPORT_C TInt CreateGrammar( TSIGrammarID& aGrammarID );

        /**
        * Creates a new lexicon.
        *    
        * @since 2.0
        * @param     aLexiconID            reference where lexicon ID is stored
        * @return    system wide error codes
        */
        IMPORT_C TInt CreateLexicon( TSILexiconID& aLexiconID );

        /**
        * Creates a new rule.
        *    
        * @since 2.8
        * @param    aGrammarID          grammar, where the new rule is to be added
        * @param     aRuleID                reference where rule ID is stored
        * @return    system wide error codes
        */
        IMPORT_C TInt CreateRule( TSIGrammarID aGrammarID, TSIRuleID& aRuleID );

        /**
        * Loads the specified model bank into the recognizer;
        * done prior to recognition.
        *
        * @since 2.0
        * @param     ModelBankID            model bank Id
        * @return    system wide error codes
        */
        IMPORT_C TInt LoadModels( TSIModelBankID aModelBankID );

        /**
        * Loads the specified grammar into the recognizer;
        * done prior to recognition.
        *
        * @since 2.0
        * @param     aGrammarID            grammar ID
        * @return    system wide error codes
        */
        IMPORT_C TInt LoadGrammar( TSIGrammarID aGrammarID );

        /**
        * Reactivates a previously deactivated grammar.
        *
        * @since 2.0
        * @param     aGrammarID            grammar ID
        * @return    system wide error codes
        */
        IMPORT_C TInt ActivateGrammar( TSIGrammarID aGrammarID );

        /**
        * Deactivates a loaded grammar, so that it is not recognized.
        *
        * @since 2.0
        * @param     aGrammarID            grammar ID
        * @return    system wide error codes
        */
        IMPORT_C TInt DeactivateGrammar( TSIGrammarID aGrammarID );

        /**
        * Loads the specified lexicon into the recognizer; 
        * done prior to recognition.
        *
        * @since 2.0
        * @param     aLexiconID            lexicon ID
        * @return    system wide error codes
        */
        IMPORT_C TInt LoadLexicon( TSILexiconID aLexiconID );

        /**
        * Returns the number of models in the specified model bank.
        * @since    2.0
        * @param    aModelBankID        model bank Id
        * @param    aCount                reference where the number of 
        *                                models is set
        * @return    system wide error codes
        */
        IMPORT_C TInt GetModelCount( TSIModelBankID aModelBankID, 
                            TInt& aModelCount );

        /**
        * Ends the current recognition session. Resources allocated 
        * for recognition are freed.
        *
        * @since    2.0
        * @param    aModelBankID        reference where a new model bank Id is 
        *                                assigned to
        * @return    system wide error codes
        */
        IMPORT_C TInt EndRecSession();

        /**
        * Starts a new recognition session.
        *
        * @since    2.0
        * @param    aMode                recognition mode
        * @return    system wide error codes
        */
        IMPORT_C TInt StartRecSession( TNSSRecognitionMode aMode );

        /**
        * Initiates speaker independent recognition; performed
        * following loading of model bank, lexicon, and grammar.
        *
        * @since    2.8
        * @param    aResultSet            reference where the recognition
        *                                result is set
        * @return    system wide error codes
        */
        IMPORT_C TInt Recognize( CSIClientResultSet& aResultSet );

        /**
        * Records uder utterance for training and recognition.
        *
        * @since    2.0
        * @param    aRecordTime            recording time in microseconds
        * @return    system wide error codes
        */
        IMPORT_C TInt Record( TTimeIntervalMicroSeconds32 aRecordTime );

        /**
        * Stops the recording. The difference with Cancel is that if
        * enough samples were recorded, a recognition result will be produced.
        *
        * @since    2.8
        * @param    aRecordTime            recording time in microseconds
        * @return    system wide error codes
        */
        IMPORT_C TInt EndRecord();

        /**
        * Pre-starts samping before Record call.
        * 
        * @since 3.2
        * @return System wide error code
        */
        IMPORT_C TInt PreStartSampling();

        /**
        * Removes the specified grammar from the permanent storage.
        * Removing a grammar will remove all rules within the grammar.
        *
        * @since    2.0
        * @param    aGrammarID            grammar Id
        * @return    system wide error codes
        */
        IMPORT_C TInt RemoveGrammar( TSIGrammarID aGrammarID );

        /**
        * Removes the specified pronunciation from the 
        * specified lexicon permanently.
        *
        * @since    2.0
        * @param    aLexiconID            lexicon Id
        * @param    aPronunciationID    pronunciation Id
        * @return    system wide error codes
        */
        IMPORT_C TInt RemovePronunciation( TSILexiconID aLexiconID, 
                        TSIPronunciationID aPronunciationID );

        /**
        * Removes the specified lexicon from the permanent storage.
        * Removing a lexicon will remove all pronunciations within the 
        * lexicon.
        *
        * @since    2.0
        * @param    aLexiconID            lexicon Id
        * @return    system wide error codes
        */
        IMPORT_C TInt RemoveLexicon( TSILexiconID aLexiconID );

        /**
        * Removes the specified model from the specified model bank 
        * permanently.
        *
        * @since    2.0
        * @param    aModelBankID        model bank Id
        * @param    aModelID            model Id
        * @return    system wide error codes
        */
        IMPORT_C TInt RemoveModel( TSIModelBankID aModelBankID, 
                        TSIModelID aModelID );

        /**
        * Removes the specified rule from the specified grammar permanently.
        *
        * @since    2.0
        * @param    aGrammarID            grammar Id
        * @param    aRuleID                rule Id
        * @return    system wide error codes
        */
        IMPORT_C TInt RemoveRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID );

        /**
        * Removes multiple rules from the specified grammar permanently.
        *
        * @since    2.0
        * @param    aGrammarID            grammar Id
        * @param    aRuleID                rule Id
        * @return    system wide error codes
        */
        IMPORT_C TInt RemoveRules( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs );

        /**
        * Unloads the specified rule from the specified grammar
        * in temporary memory, previously loaded with LoadGrammarL.  
        * The rule in the permanent storage remains intact.
        *
        * @since    2.0
        * @param    aGrammarID            grammar Id
        * @param    aRuleID               rule Id
        * @return   system wide error codes
        */
        IMPORT_C TInt UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID );

        /**
        * Unloads the specified grammar from the list of grammars
        * in temporary memory, previously loaded with LoadGrammarL.  
        * The grammar in the permanent storage remains intact.
        *
        * @since    2.8
        * @param    aGrammarID            grammar Id
        * @return   system wide error codes
        */
        IMPORT_C TInt UnloadGrammar( TSIGrammarID aGrammarID );

        /**
        * Set the event handler for asynchronous events
        *
        * @since    2.0
        * @param    aSpeechRecognitionUtilityObserver        pointer to observer
        */
        IMPORT_C void SetEventHandler( MSISpeechRecognitionUtilityObserver*
                        aSpeechRecognitionUtilityObserver );

        /**
        * Returns all pronunciation Ids that exist in the specified lexicon.
        *
        * @since    2.0
        * @param    aLexiconID            lexicon Id
        *           aPronunciationIDs     reference where pronunciation 
        *                                 Ids are stored
        * @return   system wide error codes
        */
        IMPORT_C TInt GetAllPronunciationIDs( TSILexiconID aLexiconID, 
                        RArray <TSIPronunciationID>& aPronunciationIDs );

        /**
        * Returns all grammar Ids that belong to the current client.
        *
        * @since 2.0
        * @param    aGrammarIDs            reference where grammar Ids are stored
        * @return    System-wide errors
        */
        IMPORT_C TInt GetAllClientGrammarIDs( RArray<TSIGrammarID>& aGrammarIDs );

        /**
        * Returns all lexicon Ids that belong to the current client.
        *
        * @since 2.0
        * @param    aLexiconIDs            reference where lexicon Ids are stored
        * @return    System-wide errors
        */
        IMPORT_C TInt GetAllClientLexiconIDs( RArray<TSILexiconID>& aLexiconIDs );

        /**
        *    Returns all model bank Ids that belong to the current client.
        * @since 2.0
        * @param    aModelBankIDs        reference where model bank 
        *                                Ids are stored
        * @return    System-wide errors
        */
        IMPORT_C TInt GetAllClientModelBankIDs( 
                            RArray<TSIModelBankID>& aModelBankIDs );

        /**
        * Returns all grammar Ids that exist (for all clients).
        *
        * @since 2.0
        * @param    aGrammarIDs            reference where grammar Ids are stored
        * @return    System-wide errors
        */
        IMPORT_C TInt GetAllGrammarIDs( RArray<TSIGrammarID>& aGrammarIDs );

        /**
        * Returns all lexicon Ids that exist (for all clients).
        *
        * @since 2.0
        * @param    aLexiconIDs            reference where lexicon Ids are stored
        * @return    System-wide errors
        */
        IMPORT_C TInt GetAllLexiconIDs( RArray<TSILexiconID>& aLexiconIDs );

        /**
        * Returns all model bank Ids that exist (for all clients).
        *
        * @since 2.0
        * @param    aModelBankIDs        reference where model bank 
        *                                Ids are stored
        * @return    System-wide errors
        */
        IMPORT_C TInt GetAllModelBankIDs( 
                            RArray<TSIModelBankID>& aModelBankIDs );

        /**
        * Returns all model Ids that exist in the specified model bank.
        *
        * @since    2.0
        * @param    aModelBankID        model bank Id
        *            aModelIDs            reference where model Ids are stored
        * @return    system wide error codes
        */
        IMPORT_C TInt GetAllModelIDs( TSIModelBankID aModelBankID, 
                        RArray <TSIModelID>& aModelIDs );

        /**
        * Checks if the rule is valid or not.
        *
        * @since    2.0
        * @param    aGrammarID            grammar Id
        *            aRuleID                rule Id
        *            aValid                reference where the validity of 
        *                                the rule is set
        * @return    system wide error codes
        */
        IMPORT_C TInt GetRuleValidity( TSIGrammarID aGrammarID, TSIRuleID aRuleID,
                        TBool& aValid ); 

        /**
        * Returns all rule Ids that exist in the specified grammar.
        *
        * @since    2.0
        * @param    aGrammarID            grammar Id
        *            aRuleIDs            reference where rule Ids are stored
        * @return    system wide error codes
        */
        IMPORT_C TInt GetAllRuleIDs( TSIGrammarID aGrammarID, 
                        RArray <TSIRuleID>& aRuleIDs );

        /**
        * Creates a new model bank.
        *
        * @since    2.0
        * @param    aModelBankID        reference where a new model 
        *                                bank Id is assigned to
        * @return    system wide error codes
        */
        IMPORT_C TInt CreateModelBank( TSIModelBankID& aModelBankID );

        /**
        *    Removes the specified model bank from the permanent storage.
        *    Removing a model bank will remove all models within the model bank.
        * @since    2.0
        * @param    aModelBankID        model bank Id
        * @return    system wide error codes
        */
        IMPORT_C TInt RemoveModelBank( TSIModelBankID aModelBankID );

        /**
        * Uploads the specified parameters into the engine.
        *
        * @since    2.0
        * @param    aParameterId        array of parameter IDs
        * @param    aParameterValue        array of parameter values
        * @return    system wide error codes
        */
        IMPORT_C TInt LoadEngineParameters( const RArray<TInt>& aParameterId, 
                                    const RArray<TInt>& aParameterValue );

        /**
        * Set the audio priority and preference for train, playback 
        *  and recognition
        *
        * @since    2.0
        * @param    aPriority                priority for voice recognition
        * @param    aTrainPreference        preference for training
        * @param    aPlaybackPreference        preference for playback
        * @param    aRecognitionPreference    preference for recognition
        * @return    system wide error codes
        */
        IMPORT_C TInt SetAudioPriority( TInt aPriority, TInt aTrainPreference, 
                        TInt aPlaybackPreference, TInt aRecognitionPreference );

        /**
        * From MSISpeechRecognitionUtilityObserver
        *    
        * @since    2.0
        * @param    aEvent                    event code indicating what 
        *                                    asynchronous event occurred
        */
        void HandleEvent( const TMMFEvent& aEvent );

private: // Functions

        /**
        * Private C++ constructor for this class.
        *
        * @since    2.0
        * @param    aSpeechRecognitionUtilityObserver    reference to event 
        *                                                observer class
        */
        CSISpeechRecognitionUtility( MSISpeechRecognitionUtilityObserver& 
                        aSpeechRecognitionUtilityObserver );

        /**
        * Default C++ constructor
        */
        CSISpeechRecognitionUtility();
        
        /**
        * Second phase constructor for this class.
        *
        * @since 2.0
        * @param aClientUid Client UID for data ownership, other clients won't
        *                   be able to modify the data registered to certain client
        * @param aPluginUid UID of the plugin which will be used, KNullUid to 
        *                    search for suitable plugin
        * @param aObserver Callback observer reference
        */
        void ConstructL( TUid aClientUid, 
                         TUid aPluginUid, 
                         MSISpeechRecognitionUtilityObserver& aObserver );


private: // Member variables

        // member variable MMF controller
        RMMFController iMMFController;

        // member variable for custom commands
        RSISpeechRecognitionCustomCommands iSrCustomCommands;
        // member variable pointer for observer class
        MSISpeechRecognitionUtilityObserver* iSpeechRecognitionUtilityObserver;
        // member variable pointer for controller event monitor class
        CMMFControllerEventMonitor* iControllerEventMonitor;
        // used to indicate if an event is the result of an internal command
        TBool iInternalCommand;

        // member variable pointer for array of rule IDs
        RArray <TSIRuleID>* iRuleIDs;
        // member variable pointer for array of pronunciation IDs
        RArray <TSIPronunciationID>* iPronunciationIDs;
        // member variable pointer for array of model IDs
        RArray <TSIModelID>* iModelIDs;
        // member variable pointer for array of grammar IDs
        RArray <TSIGrammarID>* iGrammarIDs;
        // member variable pointer for array of lexicon IDs
        RArray <TSILexiconID>* iLexiconIDs;
        // member variable pointer for array of model bank IDs
        RArray <TSIModelBankID>* iModelBankIDs;
        // member variable for model bank IDs
        TSIModelBankID iModelBankID;
        // member variable for grammar IDs
        TSIGrammarID iGrammarID;
        // member variable for lexicon IDs
        TSILexiconID iLexiconID;
        // member variable for pronunciation count
        TInt iPronunciationCount;
        // member variable for rule count
        TInt iRuleCount;
        // member variable for model count
        TInt iModelCount;
        
        // member variable for audio priority
        TInt iAudioPriority;
        // member variable for train preference
        TInt iTrainPreference; 
        // member variable for playback preference
        TInt iPlaybackPreference;
        // member variable for recognition preference
        TInt iRecognitionPreference;

        // member variable pointer for array of client result sets
        CSDClientResultSet* iResultSet;

        // member variable for client UID
        TUid iClientUid;

        // For SI: N-Best list
        CSIClientResultSet* iSIResultSet;

        // reserved for future expansion
        TAny* iReservedPtr_1;
        // reserved for future expansion
        TAny* iReservedPtr_2;
};

#endif    // of NSSSISPEECHRECOGNITIONUTILITY_H

// End of File
