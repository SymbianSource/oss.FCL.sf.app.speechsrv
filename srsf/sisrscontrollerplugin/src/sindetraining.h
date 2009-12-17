/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class which takes care of SINDE voice tag training.
*
*/


#ifndef SINDETRAINING_H
#define SINDETRAINING_H

// ============================================================================
// Uncomment the following directive for Bit Exact Testing.
// ============================================================================
//
 
// INCLUDES
#include "srsfbldvariant.hrh"
#include "sicontrollerplugininterface.h"
#include "sicontrollerplugin.h"
#include <e32base.h>
#include <bamdesca.h>
#include <nsssispeechrecognitiondatacommon.h>
#include <nssdevasr.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
* Class which takes care of SIND with Extensions training.
* Training is done in several batches to get optimal
* number of pronunciations for each type of word.
*
* @lib nsssicontrollerplugin.lib
* @since 3.1
*/
class CSindeTrainer : public CActive
    {
    public: // Constructors and destructor
    
	    /**
        * Two-phased constructor.
        * 
        * @param aDevAsr DevASR reference
        * @param aControllerIf Controller Interface to send callbacks to client
        * @param aPlugin Plugin reference
        * @param aDatabase SI Database
        * @param aGrammarDatabase Grammar Database
        * @param aLexiconDatabase Lexicon Database
	    */
	    static CSindeTrainer* NewL( CDevASR& aDevAsr, 
	                                CSIControllerPluginInterface& aControllerIf,
	                                CSIControllerPlugin& aPlugin,
	                                CSIDatabase& aDatabase,
                                    CSIGrammarDB& aGrammarDatabase,
                                    CSILexiconDB& aLexiconDatabase );
	
	    /**
	    * Destructor.
	    */
	    virtual ~CSindeTrainer();
	    
	public: // New functions
	
	    /**
	    * Starts the process of SINDE voice tag addition.
	    * Client thread is called is signalled directly when training completes.
	    *
	    * @param aTrainArrays Training texts
	    * @param aLanguageArray Languages which are used when training
	    * @param aLexiconId Lexicon ID where to add new pronunciations
	    * @param aGrammarId Grammar ID where to add new rules
	    * @param aModelBankId Model bank ID which is used in training process
	    * @param aClientUid UID of the client which is doing the update (to verify ownership)
	    * @param aRuleIds Output parameter, will contain the list IDs of the added rules
	    */
	    void AddSindeVoiceTagsL( RPointerArray<MDesCArray>* aTrainArrays,
	                             RArray<RLanguageArray>& aLanguageArray, 
                                 TSILexiconID aLexiconId,
                                 TSIGrammarID aGrammarId,
                                 TSIModelBankID aModelBankId,
                                 TUid aClientUid,
                                 RArray<TSIRuleID>& aRuleIds );
                                 
                                 
        /**
        * Handles an event originating from DevASR.
        *
        * @param aEvent Event code
        * @param aError Error code
        */
        void HandleEvent( TDevASREvent aEvent, TDevASRError aError );
	
	protected: // From CActive
	
        /**
        * RunL from CActive class.
        */
		void RunL();
		
        /**
        * Cancel handle from CActive class.
        */
		void DoCancel();

	private:

        /**
        * C++ default constructor.
        *
        * @param aDevAsr DevASR reference
        * @param aControllerIf Controller Interface to send callbacks to client
        * @param aPlugin Plugin reference
        * @param aDatabase SI Database
        * @param aGrammarDatabase Grammar Database
        * @param aLexiconDatabase Lexicon Database
        */
        CSindeTrainer( CDevASR& aDevAsr,
                       CSIControllerPluginInterface& aControllerIf,
                       CSIControllerPlugin& aPlugin,
                       CSIDatabase& aDatabase,
                       CSIGrammarDB& aGrammarDatabase,
                       CSILexiconDB& aLexiconDatabase );
		
        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
		/**
		* Sets the active object as active and ready to run.
		*/
		void SetReady();
		
		/**
		* Does one step of training.
		*/
		void DoOneTrainingStepL();
		
		/**
		* Does the updates to lexicon and grammar database.
		*/
		void DoDatabaseUpdateL();

        /**
        * Does the grammar compilation using DevASR.
        */		
		void DoGrammarCompilationL();
		
		/**
		* Saves compiled grammar into database.
		*/
		void StoreCompiledGrammar();

		/**
		* Utility function to add pronunciations from other batches than first one
		* to lexicon and grammar.
		*
		* @param aIndex Index within TTP word lists
		* @param aLexicon Lexicon where additions are done
		* @param aPronunIds Pronunciation id array where new ids are appended
		*/
		void AppendPronunciationsL( TInt aIndex, 
		                            CSILexicon& aLexicon, 
		                            RArray<TSIPronunciationID>& aPronunIds );

    private: // Data
    
        // States for this object
        enum TSindeTrainerState
            {
            ESindeTrainerIdle,
            ESindeTrainerTraining,
            ESindeTrainerDbUpdate,
            ESindeGrammarCompilation,
            ESindeTrainerFinished
            };
            
        // Current object state
        TSindeTrainerState iState;
        
        // Input for TTP via DevASR
        CSITtpWordList* iTtpWordList;
        
        // Position of language array processing
        TInt iLanguageIndex;
        
        // Position of text array processing
        TInt iTextIndex; 
	
	    // Stored parameters
    	RPointerArray<MDesCArray>* iTrainArrays;
    	RArray<RLanguageArray>* iLanguageArray;
    	RArray<TSIRuleID>* iRuleIds;
    	TSIGrammarID iGrammarId;
    	TSILexiconID iLexiconId;
    	TSIModelBankID iModelBankId;
        CSICompiledGrammar* iCompiledGrammar;
    	    	
    	// Reference to DevASR module
    	CDevASR& iDevAsr;
	
    	RArray<TUint32> iMaxPronunsForWord;
    	
    	// Array which contains collected set of TTP data structures
    	RPointerArray<CSITtpWordList> iTtpDataArray;
	    
	    // Asynchronous processing result, will be sent within callback
	    TInt iError;
	    
	    // To do callbacks to client side
	    CSIControllerPluginInterface& iControllerIf;
	    
	    // Controller plugin
	    CSIControllerPlugin& iPlugin;
	    
	    // Plugin database classes
		CSIDatabase& iDatabase;
        CSIGrammarDB& iGrammarDatabase;
        CSILexiconDB& iLexiconDatabase;
        
        // UID of the client
        TUid iClientUid;        
    };
	
#endif // SINDETRAINING_H
	
// End of File
