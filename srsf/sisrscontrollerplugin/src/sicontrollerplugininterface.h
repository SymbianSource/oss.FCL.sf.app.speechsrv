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
* Description:  This is the main interface of the SI Controller Plugin.
*
*/



#ifndef SICONTROLLERPLUGININTERFACE_H
#define SICONTROLLERPLUGININTERFACE_H

// INCLUDES
#include <mmfcontroller.h>
#include <nsssispeechrecognitioncustomcommandimplementor.h>
#include <nsssispeechrecognitiondataclient.h>

// FORWARD DECLARATIONS
class CSIControllerPlugin;
class CSDClientResultSet;


// CLASS DECLARATION

/**
*  This is the main interface class of SI Controller Plugin
*
*  @lib SIControllerPlugin.lib
*  @since 2.8
*/
class CSIControllerPluginInterface : public CMMFController,
public MSISpeechRecognitionCustomCommandImplementor
{
public:  // Constructors and destructor
	
    /**
    * Two-phased constructor.
	*/
	static CSIControllerPluginInterface* NewL();
	
	/**
	* Destructor.
	*/
	virtual ~CSIControllerPluginInterface();
	
public: // Functions from base classes
	
    /**
    * From CMMFController
    * Add a data source to the controller plugin.
    * @since 2.8
    * @param aDataSource reference to the data source to be added
	*/
	void AddDataSourceL( MDataSource& aDataSource );
	
	/**
	* From CMMFController
	* Add a data sink to the controller plugin. Not supported.
	* @since 2.8
	* @param	aDataSink			reference to the data sink to be added
	*/
	void AddDataSinkL( MDataSink& aDataSink );
	
	/**
	* From CMMFController
	* Remove a data source from the controller plugin. Not supported.
	* @since 2.8
	* @param	aDataSource			reference to the data source to be removed
	*/
	void RemoveDataSourceL( MDataSource& aDataSource );
	
	/**
	* From CMMFController
	* Remove a data sink from the controller plugin. Not supported.
	* @since 2.8
	* @param	aDataSink			reference to the data sink to be removed
	*/
	void RemoveDataSinkL( MDataSink& aDataSink );
	
	/**
	* From CMMFController
	* Reset the controller plugin. Not supported.
	* @since 2.8
	*/
	void ResetL();
	
	/**
	* From CMMFController
	* Prime the controller plugin. Not supported.
	* @since 2.8
	*/
	void PrimeL();
	
	/**
	* From CMMFController
	* Commence playback. Not supported.
	* @since 2.8
	*/
	void PlayL();
	
	/**
	* From CMMFController
	* Pause the controller plugin. Not supported.
	* @since 2.8
	*/
	void PauseL();
	
	/**
	* From CMMFController
	* Stop the controller plugin. Not supported.
	* @since 2.8
	*/
	void StopL();
	
	/**
	* From CMMFController
	* Get the current position. Not supported.
	* @since 2.8
	*/
	TTimeIntervalMicroSeconds PositionL() const;
	
	/**
	* From CMMFController
	* Set the current position. Not supported.
	* @since 2.8
	* @param	aPosition			desired position in microseconds
	*/
	void SetPositionL( const TTimeIntervalMicroSeconds& aPosition );
	
	/**
	* From CMMFController
	* Get the duration of the clip. Not supported.
	* @since 2.8
	*/
	TTimeIntervalMicroSeconds DurationL() const;
	
	/**
	* From CMMFController
	* Set the priority settings for this controller.
	* @since 2.8
	* @param	aPrioritySettings	new priority settings
	*/
	void SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings );
	
	/**
	* From CMMFController
	* Retrieve the number of meta data entries in the clip. Not supported.
	* @since 2.8
	* @param	aNumberOfEntries	reference where the number of entries is set
	*/
	void GetNumberOfMetaDataEntriesL( TInt& aNumberOfEntries );
	
	/**
	* From CMMFController
	*	Retrieve a meta data entry from the clip. Not supported.
	* @since 2.8
	* @param	aIndex				index of the meta data entry to retrieve
	*/
	CMMFMetaDataEntry* GetMetaDataEntryL( TInt aIndex );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Adds a new pronunciation for the given model into the specified lexicon.
	* @since 2.8
	* @param	aLexiconID			lexicon Id, where the new pronunciation is added to
	* @param	aModelBankID		model bank Id
	* @param	aModelID			model Id
	* @param	aPronunciationID	reference where a new pronunciation Id is assigned to
	*/
	//SD Remove
	void MSrAddPronunciationL( TSILexiconID aLexiconID, TSIModelBankID aModelBankID,
		TSIModelID aModelID,TSIPronunciationID& aPronunciationID );
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Adds a new rule for the given pronunciation into the specified grammar.
    * @since 2.8
    * @param	aGrammarID			grammar Id, where the new rule is added to
    * @param	aLexiconID			lexicon Id
    * @param	aPronunciationID	pronunciation Id
    * @param	aRuleID				reference where a new rule Id is assigned to
	*/
	//SD Remove
	void MSrAddRuleL( TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
		TSIPronunciationID aPronunciationID, TSIRuleID& aRuleID );
	
	/**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Cancels the current activity and returns the plugin to Idle state.
    * @since 2.8
	*/
	void MSrCancel();
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Commits (saves) the current trained model into a permanent storage.
	* @since 2.8
	*/
	void MSrCommitChangesL();
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Creates a new grammar.
	* @since 2.8
	* @param	aGrammarID			reference where a new rule Id is assigned to
	*/
	void MSrCreateGrammarL( TSIGrammarID& aGrammarID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Creates a new lexicon.
	* @since 2.8
	* @param	aLexiconID			reference where a new lexicon Id is assigned to
	*/
	void MSrCreateLexiconL( TSILexiconID& aLexiconID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Creates a new model bank.
	* @since 2.8
	* @param	aModelBankID		reference where a new model bank Id is assigned to
	*/
	void MSrCreateModelBankL( TSIModelBankID& aModelBankID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Ends the current recognition session.  Resources allocated for recognition are freed.
	* @since 2.8
	* @param	aModelBankID		reference where a new model bank Id is assigned to
	*/
	void MSrEndRecSessionL();
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all grammar Ids that belong to the current client, whose UID was set
	* with MSrSetClientUid().
	* @since 2.8
	* @param	aGrammarIDs			reference where grammar Ids are stored
	*/
	void MSrGetAllClientGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all lexicon Ids that belong to the current client, whose UID was set
	* with MSrSetClientUid().
	* @since 2.8
	* @param	aLexiconIDs			reference where lexicon Ids are stored
	*/
	void MSrGetAllClientLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all model bank Ids that belong to the current client, whose UID was set
	* with MSrSetClientUid().
	* @since 2.8
	* @param	aModelBankIDs		reference where model bank Ids are stored
	*/
	void MSrGetAllClientModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all grammar Ids that exist (for all clients).
	* @since 2.8
	* @param	aGrammarIDs			reference where grammar Ids are stored
	*/
	void MSrGetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all lexicon Ids that exist (for all clients).
	* @since 2.8
	* @param	aLexiconIDs			reference where lexicon Ids are stored
	*/
	void MSrGetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all model bank Ids that exist (for all clients).
	* @since 2.8
	* @param	aModelBankIDs		reference where model bank Ids are stored
	*/
	void MSrGetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all model Ids that exist in the specified model bank.
	* @since 2.8
	* @param	aModelBankID		model bank Id
	* @param	aModelIDs			reference where model Ids are stored
	*/
	//SD
	void MSrGetAllModelIDsL( TSIModelBankID aModelBankID, RArray<TSIModelID>& aModelIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns all pronunciation Ids that exist in the specified lexicon.
	* @since 2.8
	* @param	aLexiconID			lexicon Id
	* @param	aPronunciationIDs	reference where pronunciation Ids are stored
	*/
	void MSrGetAllPronunciationIDsL( TSILexiconID aLexiconID,
		RArray<TSIPronunciationID>& aPronunciationIDs );
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Returns all rule Ids that exist in the specified grammar.
    * @since 2.8
    * @param	aGrammarID			grammar Id
    * @param	aRuleIDs			reference where rule Ids are stored
	*/
	void MSrGetAllRuleIDsL( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns the number of models available for training system wide, based on
	* available disk space.
	* @since 2.8
	* @param	aCount				reference where the number of available models is set
	*/
	//SD
	void MSrGetAvailableStorageL( TInt& aCount );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns the engine properties specified by the engine property ID array.
	* On return aPropertyValue contains an array of engine properties.
	* @since 2.8
	* @param	aPropertyId			constant reference to array of engine property Ids
	* @param	aPropertyValue		reference to array of engine property values
	*/
	void MSrGetEnginePropertiesL( const RArray<TInt>& aPropertyId,
		RArray<TInt>& aPropertyValue );
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Returns the number of models in the specified model bank.
    * @since 2.8
    * @param	aModelBankID		model bank Id
    * @param	aCount				reference where the number of models is set
	*/
	//SD
	void MSrGetModelCountL( TSIModelBankID aModelBankID, TInt& aCount );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Checks if the rule is valid or not.
	* @since 2.8
	* @param	aGrammarID			grammar Id
	* @param	aRuleID				rule Id
	* @param	aValid				reference where the validity of the rule is set
	*/
	void MSrGetRuleValidityL( TSIGrammarID aGrammarID, TSIRuleID aRuleID, TBool& aValid );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Returns the duration of the utterance for the specified model.
	* @since 2.8
	* @param	aModelBankID		model bank Id
	* @param	aModelID			model Id
	* @param	aDuration			reference where the duration of utterance is set
	*/
	//SD
	void MSrGetUtteranceDurationL( TSIModelBankID aModelBankID, TSIModelID aModelID,
		TTimeIntervalMicroSeconds32& aDuration );
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Loads the specified recognizer parameter(s).  These parameters are used to
    * alter the recognizer's default parameters.  The parameters are specified as
    * attribute and value pairs.
    * @since 2.8
    * @param	aParameterID		list of parameter Ids
    * @param	aParameterValue		list of parameter values
	*/
	void MSrLoadEngineParametersL( const RArray<TInt>& aParameterId,
		const RArray<TInt>& aParameterValue );
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Loads the specified grammar into the recognizer; done prior to recognition.
    * @since 2.8
    * @param	aGrammarID			grammar Id
	*/
	void MSrLoadGrammarL( TSIGrammarID aGrammarID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Loads the specified lexicon into the recognizer; done prior to recognition.
	* @since 2.8
	* @param	aLexiconID			lexicon Id
	*/
	void MSrLoadLexiconL( TSILexiconID aLexiconID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Loads the specified model bank into the recognizer; done prior to recognition.
	* @since 2.8
	* @param	aModelBankID		model bank Id
	*/
	void MSrLoadModelsL( TSIModelBankID aModelBankID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Plays the previously trained utterance.
	* @since 2.8
	* @param	aModelBankID		model bank Id
	* @param	aModelID			model Id, whose utterance is played
	*/
	//SD
	void MSrPlayUtteranceL( TSIModelBankID aModelBankID, TSIModelID aModelID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Initiates recognition; performed following loading of model bank, lexicon, and grammar.
	* @since 2.8
	* @param	aResultSet			reference where the recognition result is set
	*/
	//SD
	void MSrRecognizeL( CSDClientResultSet& aResultSet );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Records uder utterance for training and recognition.
	* @since 2.8
	* @param	aRecordTime			recording time in microseconds
	*/
	void MSrRecordL( TTimeIntervalMicroSeconds32 aRecordTime );
	
    /**
	* From MSISpeechRecognitionCustomCommandImplementor.
	* Starts sampling before MSrRecordL call.
	*
	* @since 3.2
	*/
	void MSrPreStartSamplingL();	
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Removes the specified grammar from the permanent storage.
	* Removing a grammar will remove all rules within the grammar.
	* @since 2.8
	* @param	aGrammarID			grammar Id
	*/
	void MSrRemoveGrammarL( TSIGrammarID aGrammarID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Removes the specified lexicon from the permanent storage.
	* Removing a lexicon will remove all pronunciations within the lexicon.
	* @since 2.8
	* @param	aLexiconID			lexicon Id
	*/
	void MSrRemoveLexiconL( TSILexiconID aLexiconID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Removes the specified model bank from the permanent storage.
	* Removing a model bank will remove all models within the model bank.
	* @since 2.8
	* @param	aModelBankID		model bank Id
	*/
	void MSrRemoveModelBankL( TSIModelBankID aModelBankID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Removes the specified model from the specified model bank.
	* @since 2.8
	* @param	aModelBankID		model bank Id
	* @param	aModelID			model Id
	*/
	//SD
	void MSrRemoveModelL( TSIModelBankID aModelBankID, TSIModelID aModelID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Removes the specified pronunciation from the specified lexicon.
	* @since 2.8
	* @param	aLexiconID			lexicon Id
	* @param	aPronunciationID	pronunciation Id
	*/
	void MSrRemovePronunciationL( TSILexiconID aLexiconID, TSIPronunciationID aPronunciationID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Removes the specified rule from the specified grammar.
	* @since 2.8
	* @param	aGrammarID			grammar Id
	* @param	aRuleID				rule Id
	*/
	void MSrRemoveRuleL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
	
    /**
	* From MSISpeechRecognitionCustomCommandImplementor
    * Removes the specified rules from the specified grammar permanently.
    * If the rule contains rule variants, they are also destoryed.
    * @since	2.8
    * @param	aGrammarID			grammar Id
    * @param	aRuleIDs			array of rule ids
    */
    void MSrRemoveRulesL( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs );

	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Sets the client's UID for data ownership identification.
	* @since 2.8
	* @param	aClientUid			client's UID
	*/
	void MSrSetClientUid( TUid aClientUid );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Starts a new recognition session.
	* @since 2.8
	* @param	aMode				recognition mode
	*/
	void MSrStartRecSessionL( TNSSRecognitionMode aMode );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Trains a new model into the specified model bank.
	* @since 2.8
	* @param	aModelBankID		model bank Id
	* @param	aModelID			reference where a new model Id is assigned to
	*/
	//SD
	void MSrTrainL( TSIModelBankID aModelBankID, TSIModelID& aModelID );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Unloads the specified rule from the specified grammar in temporary memory, previously
	* loaded with LoadGrammarL.  The rule in the permanent storage remains intact.
	* @since 2.8
	* @param	aGrammarID			grammar Id
	* @param	aRuleID				rule Id
	*/
	void MSrUnloadRuleL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
	
	///SI Component
	
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Does speaker adapation to speaker independent models
	* @since 2.8
	* @param	aResultSet			a recognition result containing adaptation data
	* @param	aCorrect			identifies the correct recognition result from the N-best list.
	*/           
	void MSrAdaptL( CSIClientResultSet& aResultSet ,TInt aCorrect);

	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Adds a new pronunciation for the given model into the specified lexicon.
	* @since 2.8
	* @param	aLexiconID			lexicon Id
	* @param	aTrainText:			the text to be trained
 	* @param	aLanguage			the pronunciation language	
	* @param	aPronunciationID: reference where a new pronunciation identifier is set.
	*/
	void MSrAddPronunciationL(TSILexiconID aLexiconID, const TDesC& aTrainText,
		TLanguage aLanguage, TSIPronunciationID& aPronunciationID) ;
	
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Adds a new rule variant for the given pronunciation into the specified grammar.
    * @since 2.8
    * @param	aGrammarID			grammar Id, where the new rule is added to
    * @param	aLexiconID			lexicon Id
    * @param	aPronunciationIDs	pronunciation Id array
    * @param	aRuleVariantID		reference where a new rule Variant Id is assigned to
	*/
	void MSrAddRuleVariantL(TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
		RArray<TSIPronunciationID>& aPronunciationIDs, TSIRuleID aRuleID, TSIRuleVariantID& aRuleVariantID );
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Trains a new voice tag. The recognition phrase is the concatenation of words in the aTrainArray. 
    * Pronunciations are created with all given languages, which are supported.
    * @since 2.8
    * @param	aTrainArray:		Recognition phrase is formed by concatenating the descriptors in this array.
    * @param	aLanguageArray		Pronunciations are created with these languages. Not all languages are supported. The argument can be NULL.
    * @param	aLexiconID			lexicon Id
    * @param	aGrammarID			grammar Id, where the new rule is added to
    * @param	aRuleID		reference where a new rule Id is assigned to
	*/
	void MSrAddVoiceTagL( MDesCArray& aTrainArray,  RArray<TLanguage>& aLanguageArray, 
		TSILexiconID aLexiconID, TSIGrammarID aGrammarID, TSIRuleID& aRuleID);
	
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Adds several new voice tags.
    * @since 2.8
    * @param	aTrainArray : An array of MDesCArrays. The recognition phrases are formed by concatenating the descriptors in the MDesCArrays.
    * @param	aLanguageArray		Pronunciations are created with these languages. Not all languages are supported. The argument can be NULL.
    * @param	aLexiconID			lexicon Id
    * @param	aGrammarID			grammar Id, where the new rule is added to
    * @param	aRuleID		reference where a new rule Id is assigned to
	*/
	void MSrAddVoiceTagsL( RPointerArray<MDesCArray>* aTrainArray,  RArray<TLanguage>& aLanguageArray, TSILexiconID aLexiconID,
		TSIGrammarID aGrammarID,  RArray<TSIRuleID>& aRuleID );
	
	/**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Creates a new empty rule.
    * @since 2.8
    * @param	aGrammarID			grammar Id, where the new rule is added to
    * @param	aRuleID		reference where a new rule Id is assigned to
	*/
	void MSrCreateRuleL( TSIGrammarID aGrammarID, TSIRuleID& aRuleID );

	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Initiates recognition; performed following loading of model bank, lexicon, and grammar.
	* @since 2.8
	* @param	aResultSet: reference where the recognition result is set. 
	*/		
	void MSrRecognizeL( CSIClientResultSet& aResultSet );
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Ends recording. Unlike Cancel(), this function may return a recognition 
	* result if adequate number of samples was already recorded.		
	* @since 2.8
	*/				
	void MSrEndRecordL();
	
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Unloads the specified grammar from the temporary memory, 
	* previously loaded with MSrLoadGrammarL. The grammar in the permanent storage remains intact. 
	* result if adequate number of samples was already recorded.		
	* @since 2.8
	* @param	aGrammarID			grammar Id,  
	*/	
	void MSrUnloadGrammarL( TSIGrammarID aGrammarID );
		
	/**
	* From MSISpeechRecognitionCustomCommandImplementor
	* Activiate specified grammar after deactivating. 	
	* @since 2.8
	* @param	aGrammarID			grammar Id,  
	*/	
	void MSrActivateGrammarL( TSIGrammarID aGrammarID );
			
	/**
	* 
	* Deactivates a specified grammar.
	* @since 2.8
	* @param	aGrammarID			grammar Id,  
	*/	
	void MSrDeactivateGrammarL( TSIGrammarID aGrammarID );
				
	/**
	* 
	* Get the number of Pronunciations in the lexicon  
	* @since 2.8
	* @param	aLexiconID			lexicon Id, 
	* @param	aCount				number of pronunciations, 		
	*/	
	void MSrGetPronunciationCountL( TSILexiconID aLexiconID, TInt& aCount );
	
	/**
	* 
	* Get the number of rules in the Grammar 
	* @since 2.8
	* @param	aGrammarID			grammar Id, 
	* @param	aCount				number of rules, 		
	*/	
	void MSrGetRuleCountL( TSIGrammarID aGrammarID, TInt& aCount );

    
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Trains one SINDE voice tag. Adds the text to the lexicon, and 
    * creates the required rules and rule variants.
    * 
    * @since 3.1
    * @param aTrainArray The phrase to be trained, split into subwords
    * @param aLanguageArray The pronunciation languages for each of the 
    * @param aLexiconID Lexicon identifier
    * @param aGrammarID Grammar identifier
    * @param aRuleID Reference, where the new rule identifier is assigned to.
    */
    void MSrAddSindeVoiceTagL( MDesCArray& aTrainArray,
                               RArray<RLanguageArray>& aLanguageArray,
                               TSILexiconID aLexiconID,
                               TSIGrammarID aGrammarID,
                               TSIRuleID& aRuleID );
    
    /**
    * From MSISpeechRecognitionCustomCommandImplementor
    * Trains SINDE voice tags. If the training fails for some of the names
    * (but not all), it is signaled by setting the Rule ID to KInvalidRuleID.
    * 
    * @since 3.1
    * @param aTrainArray The phrase to be trained, split into subwords
    * @param aLanguageArray The pronunciation languages
    * @param aLexiconID Lexicon identifier
    * @param aGrammarID Grammar identifier
    * @param aRuleID Referenceto an array, where the new
    *                rule identifiers are assigned to.
    */
    void MSrAddSindeVoiceTagsL( RPointerArray<MDesCArray>* aTrainArray,
                                RArray<RLanguageArray>& aLanguageArray,
                                TSILexiconID aLexiconID,
                                TSIGrammarID aGrammarID,
                                RArray<TSIRuleID>& aRuleID );
    
    
	public: // New functions
		
        /**
        * Sends an event to the client.
        * @since 2.8
        * @param	aEvent				event
        * @param	aResult				result
        */
        void SendSrsEvent( TUid aEvent, TInt aResult );
	
    private:
		
        /**
        * C++ default constructor.
        */
        CSIControllerPluginInterface();
		
        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
		
    private:    // Data
		
		// SI Controller Plugin Implementation
		CSIControllerPlugin* iController;

    };
	
#endif // SICONTROLLERPLUGININTERFACE_H
	
// End of File
