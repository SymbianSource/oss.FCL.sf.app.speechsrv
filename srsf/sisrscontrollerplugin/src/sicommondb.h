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
* Description:  This class is the baseclass for all SI Controller Plugin DB classes.
*				It implements the common database functionalities.
*
*/


#ifndef SICOMMONDB_H
#define SICOMMONDB_H

//  INCLUDES
#include <d32dbms.h>
#include <e32svr.h>
#include <sysutil.h> // Critical Level check
#include <f32file.h>
#include "s32strm.h" // Streams
#include "s32mem.h"  // Buffer streams
#include "nsssispeechrecognitiondatacommon.h"
#include "nsssispeechrecognitiondatadevasr.h"

// CONSTANTS
const TInt KSDDatabaseDrive = EDriveC;

_LIT(KClientUidColumn, "clientUid");
_LIT(KUsedColumn, "used");
_LIT(KCounterColumn, "counter");

_LIT(KOpenParen, " (");
_LIT(KCloseParen, ")");
_LIT(KEqual, " = ");
_LIT(KNext, ", ");


_LIT(KSIGrammarTable, "siGrammarTable");
_LIT(KBinaryGrammarColumn, "BinaryGrammarId");
_LIT(KBinaryGrammarColumnSize, "BinaryGrammarSize");
_LIT(KGrammarIdTable, "siGrammarIdTable");
_LIT(KGrammarIdColumn, "grammarId");
_LIT(KGrammarIndex, "grammarIndex");

_LIT(KSILexiconTable, "siLexiconTable");
_LIT(KBinaryLexiconColumn, "BinaryLexiconId");
_LIT(KBinaryLexiconColumnSize, "BinaryLexiconSize");
_LIT(KLexiconIdTable, "siLexiconIdTable");
_LIT(KLexiconIdColumn, "lexiconId");
_LIT(KLexiconIndex, "lexiconIndex");

_LIT(KSIModelBankTable, "siModelBankTable");
_LIT(KBinaryModelBankColumn, "BinaryModelBankId");
_LIT(KBinaryModelBankColumnSize, "BinaryModelBankSize");
_LIT(KModelBankIdTable, "siModelBankIdTable");
_LIT(KModelBankTable, "siModelBankTable");
_LIT(KModelBankIdColumn, "modelBankId");

_LIT(KHmmColumn, "hmm");
_LIT(KHmmChecksumColumn, "hmmChecksum");
_LIT(KModelBankIndex, "modelBankIndex");
_LIT(KModelIndex, "modelIndex");

const TInt KNotUsed = 0;
const TInt KUsed = 1;
const TSIModelID KModelID=1;
// CLASS DECLARATION

/**
*  This class is the base class for all SI Controller Plugin DB classes.
*  It provides the common functionality for the DB classes.
*
*  @lib SIControllerPlugin.lib
*  @since 2.0
*/
class CSICommonDB : public CBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CSICommonDB( RDbNamedDatabase& aDatabase, RDbs& aDbSession, TInt aDrive );

 		/**
        * Destructor.
        */
        virtual ~CSICommonDB();

    public: // New functions

        /**
        * Checks if the specified ID table exists or not.  If it doesn't exist, it creates one.
        * @since 2.0
        * @param	aIdTable		descriptor containing tabel name
		* @param	aIdColumn		descriptor containing ID column name
		* @param	aIndex			descriptor containing table index name
        * @return	-
        */
		void CreateIDTableL( const TDesC& aIdTable, const TDesC& aIdColumn, const TDesC& aIndex );

        /**
        * Creates a new ID in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing table name
		* @param	aIdColumn		descriptor containing ID column name
		* @param	aClientUid		client Uid for data ownership
        * @return	A new ID
        */
		TUint32 CreateNewIDL( const TDesC& aTableName, const TDesC& aIdColumn, TUid aClientUid );

        /**
        * Returns the counter column value after searching for the aKey in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing tabel name
		* @param	aIndex			descriptor containing table index name
		* @param	aKey			key to search in the table
        * @return	Counter
        */
		TInt CountL( const TDesC& aTableName, const TDesC& aIndex, TUint aKey );

        /**
        * Checks if an unused Id exists in the given table.  If found, row is set to
		* the found row.
        * @since 2.0
        * @param	"RDbTable& aDbTable"
        * @return	ETrue if found
        */
        TBool FindUnusedIDL( RDbTable& aDbTable );

        /**
        * Returns all Ids in the specified table that belong to the specified client.
        * @since 2.0
        * @param	aTableName		descriptor containing table name
		* @param	aIdColumn		descriptor containing ID column name
        * @param	aClientUid		client's Uid for data ownership
		* @param	aIDs			reference where Ids are stored
        */
		void GetAllClientIDsL( const TDesC& aIdTable, const TDesC& aIdColumn, TUid aClientUid, RArray<TUint32>& aIDs );

        /**
        * Returns all Ids in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing table name
		* @param	aIdColumn		descriptor containing ID column name
		* @param	aIDs			reference where Ids are stored
        */
		void GetAllIDsL( const TDesC& aIdTable, const TDesC& aIdColumn, RArray<TUint32>& aIDs );

        /**
        * Checks if the specified key exists in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing tabel name
		* @param	aIndex			descriptor containing table index name
		* @param	aKey			key to search in the table
        * @return	ETrue if found.
        */
		TBool IsValidL( const TDesC& aTableName, const TDesC& aIndex, TUint aKey );

        /**
        * Sets the specified ID as unused in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing tabel name
		* @param	aIdColumn		descriptor containing ID column name
		* @param	aId				ID to be marked as unused
        */
		void ReleaseIdL( const TDesC& aTableName, const TDesC& aIdColumn, TUint32 aId );

        /**
        * Updates the counter value after finding the aKey in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing tabel name
		* @param	aIndex			descriptor containing table index name
		* @param	aKey			key to search in the table
		* @param	aIncrement		Increment if ETrue, decrement otherwise.
        */
		void UpdateCounterL( const TDesC& aTableName, const TDesC& aIndex, TUint aKey,
					TBool aIncrement );

        /**
        * Checks for data ownership.  Leaves with KErrAsrDataRightViolation if not
		* owned by this client.
        * @since 2.0
		* @param	aClientUid		client's Uid for data ownership
        * @param	aTableName		descriptor containing tabel name
		* @param	aIndex			descriptor containing table index name
		* @param	aKey			key to search in the table
        */
		void VerifyOwnershipL( TUid aClientUid, const TDesC& aTableName, const TDesC& aIndex,
					TUint aKey );

    public:    // Data

		// Database
        RDbNamedDatabase& iDb;

        // DB session
        RDbs& iDbSession;

        // drive
        TInt iDrive;
        
    private:  // New functions
    
        /**
        * Creates a new ID in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing table name
		* @param	aIdColumn		descriptor containing ID column name
		* @param	aClientUid		client Uid for data ownership
        * @return	A new ID
        */
		TUint32 DoCreateNewIDL( const TDesC& aTableName, const TDesC& aIdColumn, TUid aClientUid );
		
		/**
        * Sets the specified ID as unused in the specified table.
        * @since 2.0
        * @param	aTableName		descriptor containing tabel name
		* @param	aIdColumn		descriptor containing ID column name
		* @param	aId				ID to be marked as unused
        */
		void DoReleaseIdL( const TDesC& aTableName, const TDesC& aIdColumn, TUint32 aId );
		
    private:  // Data
    
        // Mutex for DB write lock
        // Makes sure that only one process gets the DB lock
        RMutex iMutex;
    };

#endif // SICOMMONDB_H

// End of File
