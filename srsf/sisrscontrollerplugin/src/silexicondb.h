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
* Description:  This class handles the storage and access of speaker independent
*               lexicons.  It is also responsible for allocating memory when
*               loading lexicons into the recognizer.
*
*/



#ifndef SILEXICONDB_H
#define SILEXICONDB_H

//  INCLUDES
#include <nsssispeechrecognitiondatadevasr.h>
#include "sicommondb.h"

// CLASS DECLARATION
/**
*  This class implements Speaker Inependent Lexicon Database.
*
*  @lib SIControllerPlugin.lib
*  @since 2.0
*/
class CSILexiconDB : public CSICommonDB
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSILexiconDB* NewL( RDbNamedDatabase& aDatabase, 
                                   RDbs& aDbSession,
                                   TInt aDrive );
        
        /**
        * Destructor.
        */
        virtual ~CSILexiconDB();
        
    public:  
        
        /**
        * Adds a new pronunciation into the specified lexicon.
        * @since 2.0
        * @param    aClientUid            client's Uid for data ownership
        * @param    aLexiconID            lexicon Id
        * @param    aModelBankID        model bank Id
        * @param    aPhonemeID            phoneme Id
        * @return    new pronunciation Id
        */
        TSIPronunciationID AddPronunciationL( TUid aClientUid, TSILexiconID aLexiconID,
                                              TDesC8& aPronunciation, TSIModelBankID aModelBankID );
     
        /**
        * Inserts the externalized SI Lexicon into the specified grammar table.
        * @since 2.0
        * @param    aClientUid            client's Uid for data ownership
        * @param    aSILexicon            lexicon to be updated
        */
        void  UpdateLexiconL( TUid aClientUid, CSILexicon* aSILexicon );
                
        /**
        * Creates a new lexicon ID table in the database.
        * @since 2.0
        */
        void CreateIDTableL();
        
        /**
        * Creates a new lexicon in the database.
        * @since 2.0
        * @param    aClientUid            client's Uid for data ownership
        * @return    new lexicon Id
        */
        TSILexiconID CreateLexiconL( TUid aClientUid );
        
            
        /**
        * Returns all lexicon Ids that belong to the specified client.
        * @since 2.0
        * @param    aClientUid            client's Uid for data ownership
        * @param    aLexiconIDs            reference where lexicon Ids are stored
        */
        void GetAllClientLexiconIDsL( TUid aClientUid, RArray<TSILexiconID>& aLexiconIDs );
        
        /**
        * Returns all lexicon Ids in the database.
        * @since 2.0
        * @param    aLexiconIDs            reference where lexicon Ids are stored
        */
        void GetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs );
        
        /**
        * Returns all pronunciation Ids that exist in the specified lexicon.
        * @since 2.0
        * @param    aLexiconID            lexicon Id
        * @param    aPronunciationIDs    reference where pronunciation Ids are stored
        */
        void GetAllPronunciationIDsL( TSILexiconID aLexiconID, RArray<TSIPronunciationID>& aPronunciationIDs );
    
        /**
        * Returns the number of pronunciations in the specified lexicon.
        * @since 2.0
        * @param    aLexiconID            lexicon Id
        * @return    Number of pronunciations
        */
        TInt PronunciationCountL( TSILexiconID aLexiconID );
    
        /**
        * Checks if the pronunciation is valid or not.
        * @since 2.0
        * @param    aLexiconID            lexicon Id
        * @param    aPronunciationID    pronunciation Id
        * @return    ETrue if the pronunciation is valid
        */
        TBool IsPronunciationValidL( TSILexiconID aLexiconID, TSIPronunciationID aPronunciationID );
        
        /**
        * Removes the specified lexicon from the database.
        * Removing a lexicon will remove all pronunciations within the lexicon.
        * @since 2.0
        * @param    aClientUid            client's Uid for data ownership
        * @param    aLexiconID            lexicon Id
        */
        void RemoveLexiconL( TUid aClientUid, TSILexiconID aLexiconID );
        
        /**
        * Removes the specified pronunciation from the database.
        * @since 2.0
        * @param    aClientUid            client's Uid for data ownership
        * @param    aLexiconID            lexicon Id
        * @param    aPronunciationID    pronunciation Id
        */ 
        void RemovePronunciationL(  TUid aClientUid,  TSILexiconID aLexiconID, TSIPronunciationID aPronunciationID );
        
        /**
        * Deallocates the temporary memory containing the lexicon object created with AllPronunciationsL.
        * @since 2.0
        */
        void ResetAndDestroy();
            
        /**
        * Get a new uniq ID
        * @since 2.0
        * @param    Rule ID array
        * @param    
        * @return   Error code
        */
        TInt GetNewID( RArray<TSIRuleID>& aMyIds ) ;
    
        /**
        * This function returns lexicon based on given id
        * @since 2.0
        * @param    aLexiconID            lexicon Id
        * @return Lexicon containing all pronunciations
        */
        CSILexicon* LexiconL( TSILexiconID aLexiconID );
    
    private:
            
        /**
        * C++ default constructor.
        */
        CSILexiconDB( RDbNamedDatabase& aDatabase, RDbs& aDbSession, TInt aDrive );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
            
    private:    // Data
            
        // Temporary memory where loaded lexicons are held during recognition session
        RPointerArray<CSILexicon> iLexiconArray;
        CSILexicon* iLexicon;
    };
    
#endif // SILEXICONDB_H
    
// End of File
