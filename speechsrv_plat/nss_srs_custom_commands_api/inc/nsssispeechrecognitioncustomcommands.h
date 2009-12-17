/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the client interface for the
*			    speaker independent extensions to speech recognition custom commands.
*
*/


#ifndef NSS_SI_SPEECH_RECOGNITION_CUSTOM_COMMANDS_H
#define NSS_SI_SPEECH_RECOGNITION_CUSTOM_COMMANDS_H

//  INCLUDES
#include	<mmfcontroller.h>
#include 	<nsssispeechrecognitiondataclient.h>

// FORWARD DECLARATIONS
class CSDClientResultSet;

/**
*  This is the main class of NSS Speaker Independent Speech Recognition Custom Commands
*
*  @lib nsssispeechrecognitioncustomcommands.lib
*  @since 2.8
*/

// CLASS DECLARATION
class RSISpeechRecognitionCustomCommands : public RMMFCustomCommandsBase
	{

	public:
		/**
		*	Public C++ constructor for RSpeechRecognitionCustomCommands class.
        *
        * @since	2.8
        * @param	aController			reference to RMMFController object
        * @return	-
		*/
		IMPORT_C RSISpeechRecognitionCustomCommands(RMMFController& aController);

		/**
		*	Adapts speaker independent models according using a correct recognition result.
        * 
        * @since	2.8
        * @param	aResultSet          The result N-Best containing feature vectors
		*			aCorrect            The correct index
        * @return	-
		*/
        IMPORT_C TInt Adapt(const CSIClientResultSet& aResultSet, TInt aCorrect);

		/**
		*	Adds a pronunciation to the given lexicon.
        * 
        * @since	2.8
		* @param	aLexiconID			The lexicon Id
        * @param    aTrainText          Text to be pronounced
        * @param    aLanguage           Pronunciation language
		* @param	aPronunciationID	Reference where a new pronunciation Id is assigned to.
		*								
        * @return	-
		*/
        IMPORT_C TInt AddPronunciation(TSILexiconID aLexiconID,
                                       const TDesC& aTrainText,
                                       TLanguage aLanguage,
                                       TSIPronunciationID& aPronunciationID);

		/**
        *	Adds a rule variant to a grammar.
        * 
        * @since 2.8
        * @param 	aGrammarID          Grammar identifier
        * @param    aRuleID             Rule identifier; variant is added here.
        * @param    aPronunciations     Sequence of pronunciations; these get concatenated.
        *                               in the rule variant.
        * @param    aRuleVariantID      Reference where the new variant
        *                               identifier is assigned to.
        * @return	-
		*/
		IMPORT_C TInt AddRuleVariant(
            TSIGrammarID aGrammarID,
            TSILexiconID aLexiconID,
            const RArray<TSIPronunciationID>& aPronunciationIDs,
            TSIRuleID aRuleID,
            TSIRuleVariantID& aRuleVariantID );

        /**
		*	Trains a voice tag. Adds the text to the lexicon, and creates the required
        *   rules and rule variants.
        * 
        * @since 2.8
        * @param	aTrainArray         The phrase to be trained, split into subwords
        * @param	aLanguageArray      The pronunciaiton languages
        * @param	aLexiconID          Lexicon identifier
        * @param	aGrammarID          Grammar identifier
        * @param	aRuleID             Reference, where the new rule identifier is assigned to.
        * @return	-
        */
        IMPORT_C TInt AddVoiceTag(
            const MDesCArray& aTrainArray,
            const RArray<TLanguage>& aLanguageArray,
            TSILexiconID aLexiconID,
            TSIGrammarID aGrammarID,
            TSIRuleID& aRuleID);

        /**
		*	Trains voice tags. If the training fails for some of the names (but not all),
        *   it is signaled by setting the Rule ID to KInvalidRuleID.
        * 
        * @since 2.8
        * @param	aTrainArray         The phrase to be trained, split into subwords
        * @param	aLanguageArray      The pronunciation languages
        * @param	aLexiconID          Lexicon identifier
        * @param	aGrammarID          Grammar identifier
        * @param	aRuleID             Referenceto an array, where the new
        *                               Rule identifiers are assigned to.
        * @return	-
        */
        IMPORT_C TInt AddVoiceTags(
            const RPointerArray<MDesCArray>& aTrainArray,
            const RArray<TLanguage>& aLanguageArray,
            TSILexiconID aLexiconID,
            TSIGrammarID aGrammarID,
            RArray<TSIRuleID>& aRuleID);

        /**
        *	Creates a new rule.
        * 
        * @since 2.8
        * @param 	aGrammarID          Grammar identifier
        * @param 	aRuleID             Reference where rule ID is stored
        * @return	-
        */
        IMPORT_C TInt CreateRule(TSIGrammarID aGrammarID, TSIRuleID& aRuleID);

		/**
        *   Starts recognition
        *
        * @since 2.8
        * @param 	aResultSet          Recognition result is placed here, when ready.
        * @return	-
        */
		IMPORT_C TInt Recognize( CSIClientResultSet& aResultSet );

        /**
		*	Stops recognition. Unlike Cancel(), this function gives
        *   the recognition result, if it can be guessed.
        *
        * @since	2.8
        * @param	- 
        * @return	-
        */
		IMPORT_C TInt EndRecord();

		/**
		*   Deactivates a grammar.
		*
        * @since	2.8
        * @param	aGrammarID          Grammar identifier
        * @return	-
        */
        IMPORT_C TInt UnloadGrammar( TSIGrammarID aGrammarID );

/************************ SD Legacy functions *******************************/

		/**
		*	Adds a new rule for the given pronunciation into 
		*	the specified grammar.
        * @since	2.0
        * @param	aGrammarID			grammar Id, where the new rule is 
		*								added to
		* @param	aLexiconID			lexicon Id
		* @param	aPronunciationID	pronunciation Id
		* @param	aRuleID				reference where a new rule Id is 
		*								assigned to
        * @return	System-wide errors
		*/
		IMPORT_C TInt AddRule(TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
						TSIPronunciationID aPronunciationID, TSIRuleID& aRuleID);

		/**
        *
        *	Cancels the current operation
        * @since 2.0
        * @param 	-
        * @return	-
		*/
		IMPORT_C void Cancel();

        /**
		*	Saves the current changes into permanent storage.
        * @since 2.0
        * @param	-
        * @return	System-wide errors
        */
		IMPORT_C TInt CommitChanges();

		/**
        *
        *	Creates a new grammar.
        * @since 2.0
        * @param 	aGrammarID			reference where grammar ID is stored
        * @return	System-wide errors
        */
		IMPORT_C TInt CreateGrammar(TSIGrammarID& aGrammarID);

		/**
        *
        *	Creates a new lexicon.
        * @since 2.0
        * @param 	aLexiconID			reference where lexicon ID is stored
        * @return	System-wide errors
        */
		IMPORT_C TInt CreateLexicon(TSILexiconID& aLexiconID);

        /**
		*	Creates a new model bank.
        * @since	2.0
        * @param	aModelBankID		reference where a new model 
		*								bank Id is assigned to
        * @return	System-wide errors
        */
		IMPORT_C TInt CreateModelBank(TSIModelBankID& aModelBankID);

		/**
		*	Ends the current recognition session.  Resources allocated for 
		*	recognition are freed.
        * @since	2.0
        * @param	aModelBankID		reference where a new model bank Id 
		*								is assigned to
        * @return	System-wide errors
        */
		IMPORT_C TInt EndRecSession();

		/**
        *	Returns all grammar Ids that belong to the current client.
        * @since 2.0
		* @param	-
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllClientGrammarIDs();

		/**
 		*	Returns all lexicon Ids that belong to the current client.
        * @since 2.0
		* @param	-
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllClientLexiconIDs();

		/**
        * Returns all model bank Ids that belong to the current client.
        * @since 2.0
		* @param	-
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllClientModelBankIDs();

		/**
		*	Returns all grammar Ids that exist (for all clients).
        * @since 2.0
		* @param	-
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllGrammarIDs();

		/**
		*	Returns all lexicon Ids that exist (for all clients).
        * @since 2.0
		* @param	-
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllLexiconIDs();

		/**
 		*	Returns all model bank Ids that exist (for all clients).
        * @since 2.0
		* @param	-
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllModelBankIDs();

		/**
		*	Returns all model Ids that exist in the specified model bank.
        * @since	2.0
        * @param	aModelBankID		model bank Id
		* @return	System-wide errors
        */
		IMPORT_C TInt GetAllModelIDs(TSIModelBankID aModelBankID);
						
		/**
		*	Returns all pronunciation Ids that exist in the specified lexicon.
        * @since	2.0
        * @param	aLexiconID			lexicon Id
		* @param	aPronunciationIDs	reference where pronunciation Ids 
		*								are stored
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllPronunciationIDs(TSILexiconID aLexiconID);
					
		/**
		*	Returns all rule Ids that exist in the specified grammar.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleIDs			reference where rule Ids are stored
        * @return	System-wide errors
        */
		IMPORT_C TInt GetAllRuleIDs(TSIGrammarID aGrammarID); 
		
		/**
        * 
        *	Returns the engine properties specified by the engine property 
		*	ID array. On return aPropertyValue contains an array of engine 
		*	properties.
        * @since 2.0
        * @param 	aPropertyId		constant reference to array of engine 
		*							property Ids
		* @param	aPropertyValue	reference to array of engine property 
		*							values
        * @return	system wide error code
        */
		IMPORT_C TInt GetEngineProperties(const RArray<TInt>& aPropertyId, 
						RArray<TInt>& aPropertyValue);

		/**
		*	Returns the number of models in the specified model bank.
        * @since	2.0
        * @param	aModelBankID		model bank Id
		* @param	aCount				reference where the number of models 
		*								is set
        * @return	System-wide errors
        */
		IMPORT_C TInt GetModelCount(TSIModelBankID aModelBankID, 
						TInt& aModelCount);

		/**
		*	Checks if the rule is valid or not.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
		* @param	aValid				reference where the validity of the 
		*								rule is set
        * @return	System-wide errors
        */
		IMPORT_C TInt GetRuleValidity(TSIGrammarID aGrammarID, TSIRuleID aRuleID, 
						TBool& aValid);

		/**
		*	Loads a grammar. Grammars are loaded between StartRecSession()
        *   and Recognize()
        *
        * @since	2.0
        * @param	aGrammarID			grammar Id
        * @return	System-wide errors
        */
		IMPORT_C TInt LoadGrammar( TSIGrammarID aGrammarID );

		/**
		*	Activates a loaded and deactivated grammar.
        *
        * @since	2.8
        * @param	aGrammarID			grammar Id
        * @return	System-wide errors
        */
		IMPORT_C TInt ActivateGrammar( TSIGrammarID aGrammarID );

		/**
        *   Deactivates a loaded grammar so that it is not recognized.
        *
        * @since	2.8
        * @param	aGrammarID			grammar Id
        * @return	System-wide errors
        */
		IMPORT_C TInt DeactivateGrammar( TSIGrammarID aGrammarID );

        /**
        *
        *	Loads the specified lexicon into the recognizer; 
		*	done prior to recognition.
        * @since 2.0
        * @param 	aLexiconID			lexicon ID
        * @return	System-wide errors
        */
		IMPORT_C TInt LoadLexicon(TSILexiconID aLexiconID);

		/**
        *
        *	Loads the specified model bank into the recognizer; done prior 
		*	to recognition..
        * @since 2.0
        * @param 	aModelBankID		model bank Id
        * @return	System-wide errors
        */
		IMPORT_C TInt LoadModels(TSIModelBankID aModelBankID);

		/**
		*	Records uder utterance for training and recognition.
        * @since	2.0
        * @param	aRecordTime			recording time in microseconds
        * @return	System-wide errors
        */
		IMPORT_C TInt Record(TTimeIntervalMicroSeconds32 aRecordTime);

		/**
		*	Removes the specified grammar from the permanent storage.
		*	Removing a grammar will remove all rules within the grammar.
        * @since	2.0
        * @param	aGrammarID			grammar Id
        * @return	System-wide errors
        */
		IMPORT_C TInt RemoveGrammar(TSIGrammarID aGrammarID);

		/**
		*	Removes the specified lexicon from the permanent storage.
		*	Removing a lexicon will remove all pronunciations within the 
		*	lexicon.
        * @since	2.0
        * @param	aLexiconID			lexicon Id
        * @return	-
        */
		IMPORT_C TInt RemoveLexicon(TSILexiconID aLexiconID);

		/**
		*	Removes the specified model bank from the permanent storage.
		*	Removing a model bank will remove all models within the model bank.
        * @since	2.0
        * @param	aModelBankID		model bank Id
        * @return	-
        */
		IMPORT_C TInt RemoveModelBank(TSIModelBankID aModelBankID);

		/**
		*	Removes the specified model from the specified model bank 
		*	permanently.
        * @since	2.0
        * @param	aModelBankID		model bank Id
		* @param	aModelID			model Id
        * @return	System-wide errors
        */
		IMPORT_C TInt RemoveModel(TSIModelBankID aModelBankID, 
									TSIModelID aModelID);

		/**
		*	Removes the specified pronunciation from the specified lexicon
		*	permanently.
        * @since	2.0
        * @param	aLexiconID			lexicon Id
		* @param	aPronunciationID	pronunciation Id
        * @return	System-wide errors
        */
		IMPORT_C TInt RemovePronunciation(TSILexiconID aLexiconID, 
									TSIPronunciationID aPronunciationID);

		/**
		*	Removes the specified rule from the specified grammar permanently.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
        * @return	System-wide errors
        */
		IMPORT_C TInt RemoveRule(TSIGrammarID aGrammarID, TSIRuleID aRuleID);

		/**
		*	Removes the specified rule from the specified grammar permanently.
        * @since	2.8
        * @param	aGrammarID			grammar Id
		* @param	aRuleIDs			array of rule ids
        * @return	System-wide errors
        */
		IMPORT_C TInt RemoveRules(TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs);

		/**
        *	Sets the client's UID for data ownership identification.
        * @since 2.0
        * @param	aClientUid			client's UID
        * @return	-
        */
		IMPORT_C TInt SetClientUid( TUid aClientUid );

		/**
		*	Starts a new recognition session.
        * @since	2.0
        * @param	aMode				recognition mode
        * @return	System-wide errors
        */
		IMPORT_C TInt StartRecSession(TNSSRecognitionMode aMode);

		/**
		*	Unloads the specified rule from the specified grammar in temporary 
		*	memory, previously loaded with LoadGrammarL. The rule in the 
		*	permanent storage remains intact.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
        * @return	System-wide errors
        */
		IMPORT_C TInt UnloadRule(TSIGrammarID aGrammarID, TSIRuleID aRuleID);

		/**
		*	Uploads the specified parameters into the engine.
        * @since	2.0
        * @param	aParameterId		array of parameter IDs
		* @param	aParameterValue		array of parameter values
        * @return	System-wide errors
        */
		IMPORT_C TInt LoadEngineParameters(const RArray<TInt>& aParameterId, 
									const RArray<TInt>& aParameterValue);

		/**
		*	Get the pronunciation ID array. On return aPronunciationIDs contains
		*	an array of pronunciation IDs.
        * @since	2.0
        * @param	aPronunciationIDs		array of pronunciation IDs
		* @return	-
        */
		IMPORT_C void GetPronunciationIDArrayL(RArray<TSIPronunciationID>& aPronunciationIDs);

		/**
		*	Get the rule ID array. On return aRuleIDs contains
		*	an array of rule IDs.
        * @since	2.0
        * @param	aRuleIDs				array of rule IDs
		* @return	-
        */
		IMPORT_C void GetRuleIDArrayL(RArray<TSIRuleID>& aRuleIDs);

		/**
		*	Get the model ID array. On return aModelIDs contains
		*	an array of model IDs.
        * @since	2.0
        * @param	aModelIDs				array of model IDs
		* @return	-
        */
		IMPORT_C void GetModelIDArrayL(RArray<TSIModelID>& aModelIDs);
	
		/**
		*	Get the grammar ID array. On return aGrammarIDs contains
		*	an array of rule IDs.
        * @since	2.0
        * @param	aGrammar				array of grammar IDs
		* @return	-
        */
		IMPORT_C void GetGrammarIDArrayL(RArray<TSIGrammarID>& aGrammarIDs);

		/**
		*	Get the lexicon ID array. On return aLexiconIDs contains
		*	an array of lexicon IDs.
        * @since	2.0
        * @param	aLexicon				array of lexicon IDs
		* @return	-
        */
		IMPORT_C void GetLexiconIDArrayL(RArray<TSILexiconID>& aLexiconIDs);

		/**
		*	Get the model bank ID array. On return aModelBankIDs contains
		*	an array of model bank IDs.
        * @since	2.0
        * @param	aModelBank				array of model bank IDs
		* @return	-
        */
		IMPORT_C void GetModelBankIDArrayL(RArray<TSIModelBankID>& aModelBankIDs);

		/**
		*	Get the client result set. This function should be called after recognition.
		*	On return aResultSet contains client result set.
        * @since	2.8
        * @param	aResultSet Result set to be populated.
		* @return	-
        */
		IMPORT_C void GetSIResultSetL(CSIClientResultSet& aResultSet);


private:
		/**
        * 
        *	Returns the engine properties specified by the engine property 
		*	ID array. On return aPropertyValue contains an array of engine 
		*	properties. This function does the actual work.
        * @since 2.0
        * @param 	aPropertyId		constant reference to array of engine 
		*							property Ids
		* @param	aPropertyValue	reference to array of engine property 
		*							values
        * @return	-
        */
		void DoGetEnginePropertiesL(const RArray<TInt>& aPropertyId, 
						RArray<TInt>& aPropertyValue);

		/**
		*	Uploads the specified parameters into the engine. This function does
		*	the actual work.
        * @since	2.0
        * @param	aParameterId		array of parameter IDs
		* @param	aParameterValue		array of parameter values
        * @return	-
        */
		void DoLoadEngineParametersL(const RArray<TInt>& aParameterId, 
									const RArray<TInt>& aParameterValue);

		/**
		*	Externalize an integer array.
        * @since	2.0
        * @param	aArray				array to externalize
        * @return	Pointer to externalized array.
        */
		CBufFlat* ExternalizeIntArrayL(const RArray<TInt>& aArray);

		/**
		*	Internalize an integer array.
        * @since	2.0
        * @param	aDes				descriptor containing the data to internalize
		* @param	aNumberElements		number of elements to internalize
		* @param	aArray				contains the internalized array on exit
        * @return	-
        */
		void InternalizeIntArrayL(TDes8& aDes, TInt aNumberElements, RArray<TInt>& aArray);

private:
		/**
		*	Initiates recognition; performed following loading of model bank,
		*	lexicon, and grammar. This function does the actual work and is trapped
		*	by the public Recognize() function.
        * @since	2.0
        * @param	aResultSet			reference where the recognition result 
		*								is set
        * @return	-
        */
		void DoRecognizeL(CSDClientResultSet& aResultSet);

		/**
		*	Initiates speaker independent recognition; performed following
        *   loading of model bank, lexicon, and grammar. This function does
        *   the actual work and is trapped by the public Recognize() function.
        * @since	2.8
        * @param	aResultSet			reference where the recognition result 
		*								is set
        * @return	-
        */
		void DoRecognizeSIL(CSIClientResultSet& aResultSet);

/****************************************************************************/
private:

		TAny* iReservedPtr_1;	// reserved for future expansion
		TAny* iReservedPtr_2;	// reserved for future expansion


};

#endif  // NSS_SI_SPEECH_RECOGNITION_CUSTOM_COMMANDS_H

// End of File
