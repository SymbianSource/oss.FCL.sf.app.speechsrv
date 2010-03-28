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
* Description:  This is the definition of the server side for the
*               speaker independent controller.
*
*/


#ifndef NSSSISPEECHRECOGNITIONCUSTOMCOMMANDPARSER_H
#define NSSSISPEECHRECOGNITIONCUSTOMCOMMANDPARSER_H

// INCLUDE FILES
#include <mmfcontroller.h>
#include <nsssispeechrecognitioncustomcommandimplementor.h>

// FORWARD DECLARATIONS
class CSDClientResultSet;

// CLASS DECLARATION

/**
*  This is the main class of Speaker Independent Custom Commands Parser
*
*  @lib nsssispeechrecognitioncustomcommands.lib
*  @since 2.8
*/
class CSISpeechRecognitionCustomCommandParser : public CMMFCustomCommandParserBase
{

public:
	    /**
        * 
        *	Factory function for creating this class.
        * @since 2.0
        * @param 	aImplementor		reference to custom commands 
		*								implementor
        * @return	pointer to CSISpeechRecognitionCustomCommandParser object
        */
		IMPORT_C static CSISpeechRecognitionCustomCommandParser* NewL(
							MSISpeechRecognitionCustomCommandImplementor& 
							aImplementor);

		/**
        * 
        *	Destructor
        * @since 2.0
        */
		IMPORT_C  ~CSISpeechRecognitionCustomCommandParser();

		/**
        * 
        *	Handles the request from the client
        * @since 2.0
        * @param 	aMessage			message containing the information of the
		*								request.
        */
		void HandleRequest(TMMFMessage& aMessage);

private:
	    /**
        * 
        *	C++ constructor for CSISpeechRecognitionCustomCommandParser class.
        * @since 2.0
        * @param 	aImplementor		reference to custom commands 
		*								implementor
        */
		CSISpeechRecognitionCustomCommandParser(
			MSISpeechRecognitionCustomCommandImplementor& aImplementor);

		/**
        * 
        *	Private method to handle the request from the client
        * @since 2.0
        * @param 	aMessage			message containing the information
		*								of the request.
        */
		void  DoHandleRequestL(TMMFMessage& aMessage);

		/**
        * 
		*	Adds a new pronunciation for the given model into the specified
		*	lexicon.
        * @since	2.0
        * @param 	aMessage			message containing the information of 
		*								the request.
		*/
		void DoAddPronunciationL(TMMFMessage& aMessage);

		/**
		*	Adds a new rule for the given pronunciation into the 
		*	specified grammar.
        * @since	2.0
        * @param 	aMessage			message containing the information
		*								of the request.
		*/
		void DoAddRuleL(TMMFMessage& aMessage);

		/**
        * 
        *	Cancels the current operation
        * @since 2.0
        * @param 	-
		*/
		void DoCancel();

        /**
		*	Saves the current changes into a permanent storage.
        * @since 2.0
        * @param	-
        */
		void DoCommitChangesL();

		/**
        * 
        *	Creates a new grammar.
        * @since 2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoCreateGrammarL(TMMFMessage& aMessage);

		/**
        * 
        *	Creates a new lexicon.
        * @since 2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoCreateLexiconL(TMMFMessage& aMessage);

        /**
		*	Creates a new model bank.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCreateModelBankL(TMMFMessage& aMessage);

		/**
		*	Ends the current recognition session. Resources allocated for 
		*	recognition are freed.
        * @since	2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoEndRecSessionL();

		/**
        *	Returns all grammar Ids that belong to the current client, 
		*	whose UID was set with SetClientUid().
        * @since 2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetAllClientGrammarIDsL();

		/**
 		*	Returns all lexicon Ids that belong to the current client, 
		*	whose UID was set with SetClientUid().
        * @since 2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetAllClientLexiconIDsL();

		/**
        *	Returns all model bank Ids that belong to the current client, 
		*	whose UID was set with SetClientUid().
        * @since 2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetAllClientModelBankIDsL();

		/**
		*	Returns all grammar Ids that exist (for all clients).
        * @since 2.0
        */
		void DoGetAllGrammarIDsL();

		/**
		*	Returns all lexicon Ids that exist (for all clients).
        * @since 2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetAllLexiconIDsL();

		/**
 		*	Returns all model bank Ids that exist (for all clients).
        * @since 2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetAllModelBankIDsL();

		/**
		*	Returns all model Ids that exist in the specified model bank.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoGetAllModelIDsL(TMMFMessage& aMessage);

		/**
		*	Returns all pronunciation Ids that exist in the specified lexicon.
        * @since	2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetAllPronunciationIDsL(TMMFMessage& aMessage);

		/**
		*	Returns all rule Ids that exist in the specified grammar.
        * @since	2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetAllRuleIDsL(TMMFMessage& aMessage);

		/**
		*	Returns the number of models available for training system wide, 
		*	based on available disk space.
        * @since	2.0
        * @param 	aMessage				message containing the information of
		*									the request.
        */
		void DoGetAvailableStorageL(TMMFMessage& aMessage);

		/**
        * 
        *	Returns the engine properties.
        * @since 2.0
        * @param 	aMessage				message containing the information of
		*									the request.
        */
		void DoGetEnginePropertiesL(TMMFMessage& aMessage);

		/**
		*	Returns the number of models in the specified model bank.
        * @since	2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoGetModelCountL(TMMFMessage& aMessage);

		/**
		*	Checks if the rule is valid or not.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoGetRuleValidityL(TMMFMessage& aMessage);

		/**
        * 
        *	Returns the duration of the utterance for the specified model.
        * @since 2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoGetUtteranceDurationL(TMMFMessage& aMessage);

		/**
        * Loads a gramamr. Grammars must be loaded prior to recognition.
        *
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoLoadGrammarL(TMMFMessage& aMessage);

		/**
		*	Activates a previously deactivated gramamr.
        *
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoActivateGrammarL(TMMFMessage& aMessage);

		/**
        *   Deactivates a loaded gramamr, so that it is not recognized.
        *
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoDeactivateGrammarL(TMMFMessage& aMessage);

        /**
        * 
        *	Loads the specified lexicon into the recognizer; done prior to 
		*	recognition.
        * @since 2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoLoadLexiconL(TMMFMessage& aMessage);

		/**
        * 
        *	Loads the specified model bank into the recognizer; done prior 
		*	to recognition..
        * @since 2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoLoadModelsL(TMMFMessage& aMessage);

		/**
		*	Plays the previously trained utterance.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoPlayUtteranceL(TMMFMessage& aMessage);

		/**
		*	Initiates recognition; performed following loading of model bank, 
		*	lexicon, and grammar.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoRecognizeL(TMMFMessage& aMessage);

		/**
		*	Records uder utterance for training and recognition.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoRecordL(TMMFMessage& aMessage);

		/**
		*	Removes the specified grammar from the permanent storage.
		*	Removing a grammar will remove all rules within the grammar.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoRemoveGrammarL(TMMFMessage& aMessage);

		/**
		*	Removes the specified lexicon from the permanent storage.
		*	Removing a lexicon will remove all pronunciations within 
		*	the lexicon.
        * @since	2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoRemoveLexiconL(TMMFMessage& aMessage);

		/**
		*	Removes the specified model bank from the permanent storage.
		*	Removing a model bank will remove all models within the model
		*	bank.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoRemoveModelBankL(TMMFMessage& aMessage);

		/**
		*	Removes the specified model from the specified model bank 
		*	permanently.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoRemoveModelL(TMMFMessage& aMessage);

		/**
		*	Removes the specified pronunciation from the specified 
		*	lexicon permanently.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoRemovePronunciationL(TMMFMessage& aMessage);

		/**
		*	Removes the specified rule from the specified grammar permanently.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoRemoveRuleL(TMMFMessage& aMessage);
		
		/**
		* Removes the specified rules from the specified grammar permanently
		* @param 	aMessage				message containing the information
		*									of the request.
		*/
		void DoRemoveRulesL( TMMFMessage& aMessage );	


		/**
        *	Sets the client's UID for data ownership identification.
        * @since 2.0
        * @param	aMessage				message containing the information 
		*									of the request.
        */
		void DoSetClientUidL(TMMFMessage& aMessage);

		/**
		*	Starts a new recognition session.
        * @since	2.0
        * @param 	aMessage				message containing the information 
		*									of the request.
        */
		void DoStartRecSessionL(TMMFMessage& aMessage);

        /**
		*	Trains a new model into the specified model bank.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoTrainL(TMMFMessage& aMessage);

		/**
		*	Unloads the specified rule from the specified grammar in temporary 
		*	memory, previously loaded with LoadGrammarL.  The rule in the permanent 
		*	storage remains intact.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoUnloadRuleL(TMMFMessage& aMessage);

		/**
		*	Loads the specified parameters into the engine.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoLoadEngineParametersL(TMMFMessage& aMessage);

// ================================ SI Extensions ============================ //

		/**
		*	Adapts acoustic models to a user's voice.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoAdaptL(TMMFMessage& aMessage);

		/**
		*	Adds a speaker independent pronunciation.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoAddPronunciationSIL(TMMFMessage& aMessage);

		/**
		*	Adds a rule variant.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoAddRuleVariantL(TMMFMessage& aMessage);

		/**
		*	Adds a voice tag.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoAddVoiceTagL(TMMFMessage& aMessage);

		/**
		*	Adds several voice tags.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoAddVoiceTagsL(TMMFMessage& aMessage);

		/**
		*	Creates an empty rule.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoCreateRuleL(TMMFMessage& aMessage);

		/**
		*	Stops recording (recognizing).
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoEndRecordL(TMMFMessage& aMessage);

		/**
		*	Starts speaker independent recognition.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoRecognizeSIL(TMMFMessage& aMessage);

		/**
		*	Deactivates a grammar from the recognition vocabulary.
        * @since	2.8
        * @param 	aMessage				message containing the information
		*									of the request.
        */
        void DoUnloadGrammarL(TMMFMessage& aMessage);

        /**
        * Adds one SINDE voice tag.
        *
        * @since 3.1
        * @param aMessage Message containing the information of the request.
        */
        void DoAddSindeVoiceTagL( TMMFMessage& aMessage );

        /**
        * Adds an array of voice tags.
        *
        * @since 3.1
        * @param aMessage Message containing the information of the request.
        */
        void DoAddSindeVoiceTagsL( TMMFMessage& aMessage );

        /**
        * Pre-starts sampling
        *
        * @since 3.2
        * @param aMessage Message containing the information of the request.
        */
        void DoPreStartSamplingL( TMMFMessage& aMessage );

private:

		/**
		*	Copy the pronunciation ID array to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCopyPronunciationIDArrayL(TMMFMessage& aMessage);

		/**
		*	Copy the model ID array to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCopyModelIDArrayL(TMMFMessage& aMessage);

		/**
		*	Copy the rule ID array to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCopyRuleIDArrayL(TMMFMessage& aMessage);
	
		/**
		*	Copy the grammar ID array to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCopyGrammarIDArrayL(TMMFMessage& aMessage);

		/**
		*	Copy the lexicon ID array to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCopyLexiconIDArrayL(TMMFMessage& aMessage);

		/**
		*	Copy the model bank ID array to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCopyModelBankIDArrayL(TMMFMessage& aMessage);

		/**
		*	Externalize the recognition result set.
        * @since	2.0
        */
		void DoExternalizeResultSetL();

		/**
		*	Externalize the speaker independent recognition result set.
        * @since	2.0
        */
        void DoExternalizeSIResultSetL();

		/**
		*	Copy the recognition result set to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
        */
		void DoCopyResultSetL(TMMFMessage& aMessage);


		/**
		*	Extract an integer array from client data 1.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
		* @param	aArray					RArray to contain the data on exit.
        */
		void DoExtractIntArrayFromData1L(TMMFMessage& aMessage, RArray<TInt>& aArray);
	

		/**
		*	Extract an integer array from client data 2.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
		* @param	aArray					RArray to contain the data on exit.
        */
		void DoExtractIntArrayFromData2L(TMMFMessage& aMessage, RArray<TInt>& aArray);

		/**
		*	Extract an integer array from a descriptor into an RArray.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
		* @param	aArray					RArray to contain the data on exit.
        */
		void DoExtractIntArrayL(TDes8& aDescriptor, RArray<TInt>& aArray);

		/**
		*	Copy data from an RArray to the client.
        * @since	2.0
        * @param 	aMessage				message containing the information
		*									of the request.
		* @param	aArray					RArray copied to the client.
        */
		void DoCopyIntArrayL(TMMFMessage& aMessage, const RArray<TInt>& aArray);

		// reference to the Custom Command Implementor class
		MSISpeechRecognitionCustomCommandImplementor& iImplementor; 

		// Contains the array of pronunciation IDs
		RArray<TSIPronunciationID> iPronunciationIDs;
		// Contains the array of model IDs
		RArray<TSIModelID> iModelIDs;
		// Contains the array of rule IDs
		RArray<TSIRuleID> iRuleIDs;
		// Contains the array of grammar IDs
		RArray<TSIGrammarID> iGrammarIDs;
		// Contains the array of lexicon IDs
		RArray<TSILexiconID> iLexiconIDs;
		// Contains the array of model bank IDs
		RArray<TSIModelBankID> iModelBankIDs;
        // Contains the language array
        RArray<TLanguage> iLanguageArray;
        // Contains the array of language arrays
        RArray<RLanguageArray> iLanguageArrayArray;
		// Contains the client result set !!!! Obsolete
		CSDClientResultSet* iResultSet;
        // Contains the client result set for SI recognition
        CSIClientResultSet* iSIResultSet;
		// temporary copy buffer
		CBufFlat* iResultCopyBuffer;
        // Contains the word array of AddVoiceTag
        MDesCArray* iWordArray;
        // Contains the result set, which is used in adaptation.
        CSIClientResultSet* iAdaptationSet;

		TAny* iReservedPtr_1;	// reserved for future expansion
		TAny* iReservedPtr_2;	// reserved for future expansion

};

#endif  // NSSSISPEECHRECOGNITIONCUSTOMCOMMANDPARSER_H

// End of file
