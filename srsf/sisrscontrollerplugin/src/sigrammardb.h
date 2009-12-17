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
* Description:  This class handles the storage and access of speaker independent
*               grammars.  It is also responsible for allocating memory when
*               loading grammars into the recognizer.
*
*/


#ifndef SIGRAMMARDB_H
#define SIGRAMMARDB_H

//  INCLUDES 
#include <nsssispeechrecognitiondatadevasr.h>
#include "sicommondb.h"

// CLASS DECLARATION
/**
*  This class implements Speaker Independent Grammar Database.
*
*  @lib SIControllerPlugin.lib
*  @since 2.0
*/
class CSIGrammarDB : public CSICommonDB
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIGrammarDB* NewL( RDbNamedDatabase& aDatabase, 
                                   RDbs& aDbSession,
                                   TInt aDrive );

        /**
        * Destructor.
        */
        virtual ~CSIGrammarDB();

    public: // New functions

        /**
        * Adds a new rule variant into the specified grammar.
        * @since 2.0
		* @param	aClientUid			client's Uid for data ownership
        * @param	aGrammarID			grammar Id
		* @param	aLexiconID			lexicon Id
		* @param	aPronunciationID	pronunciation Id
        * @return	-
        */         
		void AddRuleVariantL(TUid aClientUid,
			TSIGrammarID aGrammarID, 										 
			TSILexiconID aLexiconID,
			const RArray<TSIPronunciationID>& aPronunciationIDs,
			TSIRuleID aRuleID,
			TSIRuleVariantID& aRuleVariantID) ;

		/**
		* Returns a grammar object containing all speaker independent rules of the specified grammar.
		* The grammar is added to iGrammarArray
		* @since 2.0
		* @param	aGrammarID			grammar Id
		* @return	pointer to a grammar object
        */
        const CSICompiledGrammar* LoadGrammarL(TSIGrammarID aGrammarID );
		
		/**
        * Creates a new rule in the database.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
		* @param	aGrammarID			grammar Id        
		* @param	aRuleID		        rule Id of the created rule        
        * @return	 -
        */        
		void CreateRuleL(TUid aClientUid,TSIGrammarID aGrammarID, TSIRuleID& aRuleID) ;
		
		/**
        * Creates a new grammar in the database.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
        * @return	a new grammar Id
        */
        TSIGrammarID CreateGrammarL( TUid aClientUid );

        /**
        * Creates a new grammar ID table in the database.
        * @since 2.0
        * @param	-
        * @return	-
        */
        void CreateIDTableL();

        /**
        * Returns all grammar IDs that belong to the specified client.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
		* @param	aGrammarIDs			reference where grammar Ids are stored
        * @return	-
        */
	 	void  GetAllClientGrammarIDsL(
										   TUid aClientUid,
										   RArray<TSIGrammarID>& aGrammarIDs );
        /**
        * Returns all grammar Ids in the database.
        * @since 2.0
		* @param	aGrammarIDs			reference where grammar Ids are stored
        * @return	-
        */
		void GetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs );

	 
        /**
        * Returns all rule Ids that exist in the specified grammar.
        * @since 2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleIDs			reference where rule Ids are stored
        * @return	-
        */
        void GetAllRuleIDsL( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs );

        /**
        * Checks if a valid grammar is loaded.
        * @since 2.0
        * @param	-
        * @return	ETrue if a valid grammar is loaded
        */
		TBool IsGrammarLoaded();

		/**
        * Checks if the rule is valid or not.
        * @since 2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
        * @return	ETrue if the rule is valid
        */
        TBool IsRuleValidL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );

		/**
        * Removes the specified grammar from the database.
		* Removing a grammar will remove all rules within the grammar.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
        * @param	aGrammarID			grammar Id
        * @return	-
        */
        void RemoveGrammarL( TUid aClientUid, TSIGrammarID aGrammarID );

		/**
        * Removes the specified rule from the database.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
		* @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
        * @return	-
        */
        void RemoveRuleL(TUid aClientUid,TSIGrammarID aGrammarID, TSIRuleID aRuleID );

		/**
        * Deallocates the temporary memory containing the grammar object created with AllRulesL.
        * @since 2.0
        * @param	-
        * @return	-
        */
        void ResetAndDestroy();

		/**
        * Unloads the specified rule from the specified grammar in temporary memory, previously
		* loaded with AllRulesL.  The rule in the database remains intact.
        * @since 2.0
        * @param	aGrammarID			grammar Id
		* @param	aRuleID				rule Id
        * @return	-
        */
        void UnloadRuleL( TSIGrammarID aGrammarID, TSIRuleID aRuleID );
		
		/**
        * Unloads the specified grammar from the temporary memory, previously
		* loaded with AllRulesL.  The grammar in the database remains intact.
        * @since 2.0
        * @param	aGrammarID			grammar Id
        * @return	-
        */
		void UnloadGrammarL( TSIGrammarID aGrammarID);

		/**
        * Update the specified grammar  in the database
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
        * @param	aSICompiledGrammar  pointer to a updated grammar object
 		* @return	-
        */
		void UpdateGrammarL(TUid aClientUid,CSICompiledGrammar* aSICompiledGrammar);
		
		/**
        * Returns a grammar object containing all speaker independent rules of the specified grammar.
		* The pointer to the object is not put in the iGrammarArray
        * @since 2.0
        * @param	aGrammarID			grammar Id
        * @param	
 		* @return	Pointer to a compiled grammar object
        */
		const CSICompiledGrammar* GrammarL(TSIGrammarID aGrammarID );
		
		/**
        * Find a grammar from loaded grammar array
        * @since 2.0
        * @param	aGrammarID			grammar Id
        * @param	
 		* @return	Pointer to a compiled grammar object
        */
		CSICompiledGrammar*  FindGrammarL(TSIGrammarID aGrammarID) ;
 		
		 
		/**
        * Get a new uniq ID
        * @since 2.0
        * @param	Rule ID array
        * @param	
 		* @return	Created ID
        */
		TInt GetNewID( RArray<TSIRuleID>& aMyIds );
		
		/**
        * Returns the number of rules in the specified grammar.
        * @since 2.0
        * @param	aGrammarID			grammar Id
        * @return	Number of rules
        */
		TInt RuleCountL( TSIGrammarID aGrammarID );

     private:

        /**
        * C++ default constructor.
        */
        CSIGrammarDB( RDbNamedDatabase& aDatabase, RDbs& aDbSession, TInt aDrive );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

     private:    // Data
		// Temporary memory where loaded grammars are held during recognition session
		RPointerArray<CSICompiledGrammar> iGrammarArray;
    };

#endif // SIGRAMMARDB_H

// End of File
