/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the server side for the
*			    speaker independent controller.
*
*/



#ifndef NSSSISPEECHRECOGNITIONCUSTOMCOMMANDIMPLEMENTOR_H
#define NSSSISPEECHRECOGNITIONCUSTOMCOMMANDIMPLEMENTOR_H

// INCLUDES

#include <nsssispeechrecognitiondataclient.h>
#include <nsssispeechrecognitiondatacommon.h>

// CLASS DECLARATION

/**
*  This is the main class of Speaker Independent Custom Commands Implementor
*
*  @lib NssSiSpeechRecognitionCustomCommands.lib
*  @since 2.8
*/

class MSISpeechRecognitionCustomCommandImplementor
{

public:

		/**
		*	Adapts speaker independent models according using a correct recognition result.
        * 
        * @since	2.8
        * @param	aResultSet          The result N-Best containing feature vectors
		*			aCorrect            The correct index
		*/
        virtual void MSrAdaptL(CSIClientResultSet& aResultSet, TInt aCorrect) = 0;

		/**
		*	Adds a pronunciation to the given lexicon.
        * 
        * @since	2.8
		* @param	aLexiconID			The lexicon Id
        * @param    aTrainText          Text to be pronounced
        * @param    aLanguage           Pronunciation language
		* @param	aPronunciationID	Reference where a new pronunciation Id is assigned to.
		*/
        virtual void MSrAddPronunciationL(TSILexiconID aLexiconID,
                                          const TDesC& aTrainText,
                                          TLanguage aLanguage,
                                          TSIPronunciationID& aPronunciationID) = 0;

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
		*/
		virtual void MSrAddRuleVariantL(
            TSIGrammarID aGrammarID,
            TSILexiconID aLexiconID,
            RArray<TSIPronunciationID>& aPronunciationIDs,
            TSIRuleID aRuleID,
            TSIRuleVariantID& aRuleVariantID ) = 0;

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
        */
        virtual void MSrAddVoiceTagL(
            MDesCArray& aTrainArray,
            RArray<TLanguage>& aLanguageArray,
            TSILexiconID aLexiconID,
            TSIGrammarID aGrammarID,
            TSIRuleID& aRuleID) = 0;

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
        */
        virtual void MSrAddVoiceTagsL(
            RPointerArray<MDesCArray>* aTrainArray,
            RArray<TLanguage>& aLanguageArray,
            TSILexiconID aLexiconID,
            TSIGrammarID aGrammarID,
            RArray<TSIRuleID>& aRuleID) = 0;

        /**
        *	Creates a new rule.
        * 
        * @since 2.8
        * @param 	aGrammarID          Grammar identifier
        * @param 	aRuleID             Reference where rule ID is stored
        */
        virtual void MSrCreateRuleL(TSIGrammarID aGrammarID, TSIRuleID& aRuleID) = 0;

		/**
        *   Starts recognition
        *
        * @since 2.8
        * @param 	aResultSet          Recognition result is placed here, when ready.
        */
		virtual void MSrRecognizeL( CSIClientResultSet& aResultSet ) = 0;

        /**
		*	Stops recognition. Unlike Cancel(), this function gives
        *   the recognition result, if it can be guessed.
        *
        * @since	2.8
        */
		virtual void MSrEndRecordL() = 0;

		/**
		*   Deactivates a grammar.
		*
        * @since	2.8
        * @param	aGrammarID          Grammar identifier
        */
        virtual void MSrUnloadGrammarL( TSIGrammarID aGrammarID ) = 0;

        /**
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
        virtual void MSrAddSindeVoiceTagL( MDesCArray& aTrainArray,
                                           RArray<RLanguageArray>& aLanguageArray,
                                           TSILexiconID aLexiconID,
                                           TSIGrammarID aGrammarID,
                                           TSIRuleID& aRuleID ) = 0;

        /**
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
        virtual void MSrAddSindeVoiceTagsL( RPointerArray<MDesCArray>* aTrainArray,
                                            RArray<RLanguageArray>& aLanguageArray,
                                            TSILexiconID aLexiconID,
                                            TSIGrammarID aGrammarID,
                                            RArray<TSIRuleID>& aRuleID ) = 0;

/***************************** SD Legacy functions ***********************/

		/**
		*	Adds a new rule for the given pronunciation into the specified 
		*	grammar.
        * @since	2.0
        * @param	aGrammarID			grammar Id, where the new rule is 
		*								added to
		* @param	aLexiconID			lexicon Id
		* @param	aPronunciationID	pronunciation Id
		* @param	aRuleID				reference where a new rule Id is 
		*								assigned to
		*/
		virtual void MSrAddRuleL( TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
						TSIPronunciationID aPronunciationID, 
						TSIRuleID& aRuleID ) = 0;

		/**
        * 
        *	Cancels the current operation
        * @since 2.0
		*/
		virtual void MSrCancel() = 0;

        /**
		*	Saves the current changes into a permanent storage.
        * @since 2.0
        */
		virtual void MSrCommitChangesL() = 0;

		/**
        * 
        *	Creates a new grammar.
        * @since 2.0
        * @param 	aGrammarID			reference where grammar ID is stored
        */
		virtual void MSrCreateGrammarL( TSIGrammarID& aGrammarID ) = 0;

		/**
        * 
        *	Creates a new lexicon.
        * @since 2.0
        * @param 	aLexiconID			reference where lexicon ID is stored
        */
		virtual void MSrCreateLexiconL( TSILexiconID& aLexiconID ) = 0;

        /**
		*	Creates a new model bank.
        * @since	2.0
        * @param	aModelBankID		reference where a new model bank Id 
		*								is assigned to
        */
		virtual void MSrCreateModelBankL( TSIModelBankID& aModelBankID ) = 0;

		/**
		*	Ends the current recognition session. Resources allocated 
		*	for recognition are freed.
        * @since	2.0
        * @param	aModelBankID		reference where a new model bank
		*								Id is assigned to
        */
		virtual void MSrEndRecSessionL() = 0;

		/**
        *	Returns all grammar Ids that belong to the current client, 
		*	whose UID was set with MSrSetClientUid().
        * @since 2.0
		* @param	aGrammarIDs			reference where grammar Ids are stored
        */
		virtual void MSrGetAllClientGrammarIDsL( 
						RArray<TSIGrammarID>& aGrammarIDs ) = 0;

		/**
 		*	Returns all lexicon Ids that belong to the current client,
		*	whose UID was set with MSrSetClientUid().
        * @since 2.0
		* @param	aLexiconIDs			reference where lexicon Ids are stored
        */
		virtual void MSrGetAllClientLexiconIDsL( 
						RArray<TSILexiconID>& aLexiconIDs ) = 0;

		/**
        *	Returns all model bank Ids that belong to the current client, 
		*	whose UID was set with MSrSetClientUid().
        * @since 2.0
		* @param	aModelBankIDs		reference where model bank Ids are stored
        */
		virtual void MSrGetAllClientModelBankIDsL( 
						RArray<TSIModelBankID>& aModelBankIDs ) = 0;

		/**
		*	Returns all grammar Ids that exist (for all clients).
        * @since 2.0
		* @param	aGrammarIDs			reference where grammar Ids are stored
        */
		virtual void MSrGetAllGrammarIDsL( 
						RArray<TSIGrammarID>& aGrammarIDs ) = 0;

		/**
		*	Returns all lexicon Ids that exist (for all clients).
        * @since 2.0
		* @param	aLexiconIDs			reference where lexicon Ids are stored
        */
		virtual void MSrGetAllLexiconIDsL( 
						RArray<TSILexiconID>& aLexiconIDs ) = 0;

		/**
 		*	Returns all model bank Ids that exist (for all clients).
        * @since 2.0
		* @param	aModelBankIDs		reference where model bank Ids are stored
        */
		virtual void MSrGetAllModelBankIDsL( 
						RArray<TSIModelBankID>& aModelBankIDs ) = 0;

		/**
		*	Returns all pronunciation Ids that exist in the specified lexicon.
        * @since	2.0
        * @param	aLexiconID			lexicon Id
		*			aPronunciationIDs	reference where pronunciation Ids 
		*			are stored
        */
		virtual void MSrGetAllPronunciationIDsL( TSILexiconID aLexiconID, 
						RArray<TSIPronunciationID>& aPronunciationIDs ) = 0;

		/**
		*	Returns all rule Ids that exist in the specified grammar.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		*			aRuleIDs			reference where rule Ids are stored
        */
		virtual void MSrGetAllRuleIDsL( TSIGrammarID aGrammarID, 
						RArray<TSIRuleID>& aRuleIDs ) = 0;

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
        */
		virtual void MSrGetEnginePropertiesL( const RArray<TInt>& aPropertyId, 
						RArray<TInt>& aPropertyValue ) = 0;
		
		/**
		*	Checks if the rule is valid or not.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		*			aRuleID				rule Id
		*			aValid				reference where the validity of the 
		*								rule is set
        */
		virtual void MSrGetRuleValidityL( TSIGrammarID aGrammarID, 
						TSIRuleID aRuleID, TBool& aValid ) = 0;

		/**
		*	Loads the specified grammar into the reocgnizer;
        *   done prior to recognition.
        * @since	2.0
        * @param	aGrammarID			grammar Id
        */
		virtual void MSrLoadGrammarL( TSIGrammarID aGrammarID ) = 0;

		/**
		*	Activates the specified grammar,
        *   which has been previously deactivated.
        *
        * @since	2.8
        * @param	aGrammarID			grammar Id
        */
		virtual void MSrActivateGrammarL( TSIGrammarID aGrammarID ) = 0;

		/**
		*	Deactivates a loaded grammar so that it is not recognized.
        *
        * @since	2.8
        * @param	aGrammarID			grammar Id
        */
		virtual void MSrDeactivateGrammarL( TSIGrammarID aGrammarID ) = 0;

        /**
        * 
        *	Loads the specified lexicon into the recognizer; 
		*	done prior to recognition.
        * @since 2.0
        * @param 	aLexiconID			lexicon ID
        */
		virtual void MSrLoadLexiconL( TSILexiconID aLexiconID ) = 0;

		/**
        * 
        *	Loads the specified model bank into the recognizer; 
		*	done prior to recognition..
        * @since 2.0
        * @param 	ModelBankID			model bank Id
        */
		virtual void MSrLoadModelsL( TSIModelBankID aModelBankID ) = 0;

		/**
		*	Records an utterance for training and recognition.
        * @since	2.0
        * @param	aRecordTime			recording time in microseconds
        */
		virtual void MSrRecordL( TTimeIntervalMicroSeconds32 aRecordTime ) = 0;

        /**
        * Pre-starts sampling before MSrRecordL call.
        *
        * @since 3.2
        */
        virtual void MSrPreStartSamplingL() = 0;

		/**
		*	Removes the specified grammar from the permanent storage.
		*	Removing a grammar will remove all rules and rule variants
        *   within the grammar.
        * @since	2.0
        * @param	aGrammarID			grammar Id
        */
		virtual void MSrRemoveGrammarL( TSIGrammarID aGrammarID ) = 0;

		/**
		*	Removes the specified lexicon from the permanent storage.
		*	Removing a lexicon will remove all pronunciations within the 
		*	lexicon.
        * @since	2.0
        * @param	aLexiconID			lexicon Id
        */
		virtual void MSrRemoveLexiconL( TSILexiconID aLexiconID ) = 0;

		/**
		*	Removes the specified model bank from the permanent storage.
		*	Removing a model bank will remove all models within the model bank.
        * @since	2.0
        * @param	aModelBankID		model bank Id
        */
		virtual void MSrRemoveModelBankL( TSIModelBankID aModelBankID ) = 0;

		/**
		*	Removes the specified model from the specified model bank 
		*	permanently.
        * @since	2.0
        * @param	aModelBankID		model bank Id
		* @param	aModelID			model Id
        */
		virtual void MSrRemoveModelL( TSIModelBankID aModelBankID, 
						TSIModelID aModelID ) = 0;

		/**
		*	Removes the specified pronunciation from the specified lexicon 
		*	permanently.
        * @since	2.0
        * @param	aLexiconID			lexicon Id
		* @param	aPronunciationID	pronunciation Id
        */
		virtual void MSrRemovePronunciationL( TSILexiconID aLexiconID, 
						TSIPronunciationID aPronunciationID ) = 0;

		/**
		*	Removes the specified rule from the specified grammar permanently.
        *   If the rule contains rule variants, they are also destoryed.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
        */
		virtual void MSrRemoveRuleL( TSIGrammarID aGrammarID, 
						TSIRuleID aRuleID ) = 0;

		/**
		*	Removes the specified rules from the specified grammar permanently.
        *   If the rule contains rule variants, they are also destoryed.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleIDs			array of rule ids
        */
		virtual void MSrRemoveRulesL( TSIGrammarID aGrammarID, 
						RArray<TSIRuleID>& aRuleIDs ) = 0;

		/**
        *	Sets the client's UID for data ownership identification.
        * @since 2.0
        * @param	aClientUid			client's UID
        */
		virtual void MSrSetClientUid( TUid aClientUid ) = 0;

		/**
		*	Starts a new recognition session.
        * @since	2.0
        * @param	aMode				recognition mode
        */
		virtual void MSrStartRecSessionL( TNSSRecognitionMode aMode ) = 0;

		/**
		*	Unloads the specified rule from the specified grammar in temporary 
		*	memory, previously loaded with LoadGrammarL.  The rule in the 
		*	permanent storage remains intact.
        * @since	2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
        */
		virtual void MSrUnloadRuleL( TSIGrammarID aGrammarID, 
						TSIRuleID aRuleID ) = 0;

		/**
		*	Unloads the specified parameters to the engine.
        * @since	2.0
        * @param	aParameterId		array of parameter IDs
		* @param	aParameterValue		array of parameter values
        */
		virtual void MSrLoadEngineParametersL( 
						const RArray<TInt>& aParameterId, 
						const RArray<TInt>& aParameterValue ) = 0;

private:
		TAny* iReservedPtr_1;	// reserved for future expansion
		TAny* iReservedPtr_2;	// reserved for future expansion

};

#endif  // NSSSISPEECHRECOGNITIONCUSTOMCOMMANDIMPLEMENTOR_H
