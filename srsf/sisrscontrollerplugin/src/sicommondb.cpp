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
* Description:  This class is the baseclass for all SI Controller Plugin DB
*               classes. It implements the common database functionalities.
*
*/



// INCLUDE FILES
#include "sicommondb.h"
#include "rubydebug.h"

// CONSTANTS

// Name of the DB lock mutex
_LIT( KLockMutex, "SIGRAMMAR" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSICommonDB::CSICommonDB
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSICommonDB::CSICommonDB( RDbNamedDatabase& aDatabase, 
                          RDbs& aDbSession, TInt aDrive )
: iDb( aDatabase ),
  iDbSession( aDbSession ),
  iDrive( aDrive )
    {
    
    TInt err = iMutex.OpenGlobal( KLockMutex );
    if ( err != KErrNone )
        {
        RUBY_DEBUG0( "CSICommonDB::CSICommonDB Creating new global mutex" );
        iMutex.CreateGlobal( KLockMutex );
        }
    else
        {
        RUBY_DEBUG0( "CSICommonDB::CSICommonDB Using existing global mutex" );
        }
    
    }

// -----------------------------------------------------------------------------
// CSICommonDB::CSICommonDB
// Destructor 
// -----------------------------------------------------------------------------
//
CSICommonDB::~CSICommonDB()
    {
    if ( iMutex.IsHeld() )
        {
        iMutex.Signal();
        }
    iMutex.Close();    
    }

// -----------------------------------------------------------------------------
// CSICommonDB::CheckIDTableL
// Each of controller's database files has an ID table.  If the database has no
// table, this is the first time the database is open, so the ID table is created.
// -----------------------------------------------------------------------------
//
void CSICommonDB::CreateIDTableL( const TDesC& aIdTable,
                                  const TDesC& aIdColumn,
                                  const TDesC& aIndex )
    {
    RUBY_DEBUG_BLOCK( "CSICommonDB::CheckIDTableL - create ID table" );
    
    TBuf<150> KSQLStatement;
    // Declare a literal string to hold the SQL statement
    // CREATE TABLE aIdTable (aIdColumn COUNTER, KClientUidColumn INTEGER NOT NULL,
    // KCounterColumn INTEGER NOT NULL, KUsedColumn BIT NOT NULL)
    _LIT(KSQLCreate1, "CREATE TABLE ");
    _LIT(KSQLCreate2, " COUNTER, ");
    _LIT(KSQLCreate3, " INTEGER NOT NULL, ");
    _LIT(KSQLCreate4, " BIT NOT NULL)");
    
    KSQLStatement = KSQLCreate1;
    KSQLStatement.Append(aIdTable);
    KSQLStatement.Append(KOpenParen);
    KSQLStatement.Append(aIdColumn);
    KSQLStatement.Append(KSQLCreate2);
    KSQLStatement.Append(KClientUidColumn);
    KSQLStatement.Append(KSQLCreate3);
    KSQLStatement.Append(KCounterColumn);
    KSQLStatement.Append(KSQLCreate3);
    KSQLStatement.Append(KUsedColumn);
    KSQLStatement.Append(KSQLCreate4);
    
    User::LeaveIfError(iDb.Execute(KSQLStatement));
    
    // Create an index on the 'Id' column to ensure 'Id' values are unique
    // CREATE UNIQUE INDEX aIndex ON aIdTable (aIdColumn)
    _LIT(KSQLIndex1, "CREATE UNIQUE INDEX ");
    _LIT(KSQLIndex2, " ON ");
    
    KSQLStatement = KSQLIndex1;
    KSQLStatement.Append(aIndex);
    KSQLStatement.Append(KSQLIndex2);
    KSQLStatement.Append(aIdTable);
    KSQLStatement.Append(KOpenParen);
    KSQLStatement.Append(aIdColumn);
    KSQLStatement.Append(KCloseParen);
    
    User::LeaveIfError(iDb.Execute(KSQLStatement));
    }

// -----------------------------------------------------------------------------
// CSICommonDB::CreateNewIDL
// This function first checks to see if there is an "unused" ID.  If one is not
// found, it creates a new row in the table for a new ID.
// -----------------------------------------------------------------------------
//
TUint32 CSICommonDB::CreateNewIDL(
	const TDesC& aTableName,
	const TDesC& aIdColumn,
	TUid aClientUid )
    {
    RUBY_DEBUG_BLOCKL( "CSICommonDB::CreateNewIDL" );
    
	TUint32 newID( 0 );
	
	iMutex.Wait();
	
	TRAPD( error, newID = DoCreateNewIDL( aTableName, aIdColumn, aClientUid ) );
	
	iMutex.Signal();
	
    User::LeaveIfError( error );

	return newID;
	}

// -----------------------------------------------------------------------------
// CSICommonDB::CountL
// This function first searches the row with aKey and returns the counter value
// given that the row is set as "used".
// -----------------------------------------------------------------------------
//
TInt CSICommonDB::CountL( const TDesC& aTableName, const TDesC& aIndex,
                          TUint aKey )
	{
    RUBY_DEBUG_BLOCK( "CSICommonDB::CountL" );
	RDbTable dbTable;		// Provides access to table data as a rowset
	TInt count = 0;

	// Open model table
	User::LeaveIfError(dbTable.Open(iDb, aTableName, RDbTable::EReadOnly));
	CleanupClosePushL(dbTable);
	User::LeaveIfError(dbTable.SetIndex(aIndex));

	TDbSeekKey key(aKey);
	// Return true, if a matching row found
	if ( dbTable.SeekL(key) )
		{
		dbTable.GetL();
		// Get column set for column ordinals
		CDbColSet* columns = dbTable.ColSetL();
		TDbColNo counter_col = columns->ColNo(KCounterColumn);
		TDbColNo used_col = columns->ColNo(KUsedColumn);
		delete columns;

		// Check if this row is currently in use
		TBool used = dbTable.ColInt(used_col);
		if ( used )
			{
			count = dbTable.ColInt(counter_col);
			}
		else
			{
            RUBY_DEBUG1( "CSICommonDB::CountL - key=%d is unused", aKey );
			User::Leave(KErrNotFound);
			}
		}
	else
		{
        RUBY_DEBUG1( "CSICommonDB::CountL - can't find key=%d", aKey );
		User::Leave(KErrNotFound);
		}

    // Cleanup dbTable
    CleanupStack::PopAndDestroy();
	return count;
	}

// -----------------------------------------------------------------------------
// CSICommonDB::CreateDatabaseL
// Creates a database file if one does not exist yet.
// -----------------------------------------------------------------------------
//
TBool CSICommonDB::FindUnusedIDL(
	RDbTable& aDbTable )
	{
	TBuf<30> KSQLQuery;
	// Declare a literal string to hold the SQL search-condition for 'unused' row
	// KUsedColumn = KNotUsed
	KSQLQuery = KUsedColumn;
	KSQLQuery.Append(KEqual);
	KSQLQuery.AppendNum(KNotUsed);

	// Return true, if the table is not empty and a matching row found
	return (aDbTable.FirstL()  &&  aDbTable.FindL(RDbTable::EForwards, TDbQuery(KSQLQuery)) != KErrNotFound);
	}

// -----------------------------------------------------------------------------
// CSICommonDB::GetAllClientIDsL
// This function returns all Ids in the specified table that belong to the specified client.
// -----------------------------------------------------------------------------
//
void CSICommonDB::GetAllClientIDsL(
	const TDesC& aIdTable,
	const TDesC& aIdColumn,
	TUid aClientUid,
	RArray<TUint32>& aIDs )
	{
	CleanupClosePushL( aIDs ); 
	
	TBuf<100> KSQLStatement;
	// Declare a literal string to hold the SQL statement
	// SELECT aIdColumn, KUsedColumn FROM aIdTable WHERE KClientUidColumn = uid
	// ORDER BY aIdColumn
	_LIT(KSQLSelect1, "SELECT ");
	_LIT(KSQLSelect2, " FROM ");
	_LIT(KSQLSelect3, " WHERE ");
	_LIT(KSQLSelect4, " ORDER BY ");

	KSQLStatement = KSQLSelect1;
	KSQLStatement.Append(aIdColumn);
	KSQLStatement.Append(KNext);
	KSQLStatement.Append(KUsedColumn);
	KSQLStatement.Append(KSQLSelect2);
	KSQLStatement.Append(aIdTable);
	KSQLStatement.Append(KSQLSelect3);
	KSQLStatement.Append(KClientUidColumn);
	KSQLStatement.Append(KEqual);
	KSQLStatement.AppendNum((TInt) aClientUid.iUid);
	KSQLStatement.Append(KSQLSelect4);
	KSQLStatement.Append(aIdColumn);

	RDbView view;
	CleanupClosePushL(view);
	User::LeaveIfError(view.Prepare(iDb, TDbQuery(KSQLStatement, EDbCompareNormal)));
	User::LeaveIfError(view.EvaluateAll());

	// Get column set for column ordinals
    CDbColSet* columns = view.ColSetL();
    TDbColNo id_col = columns->ColNo(aIdColumn);
	TDbColNo used_col = columns->ColNo(KUsedColumn);
	delete columns;

	TUint32 id;
	TBool used;
	// After evaluation, the first call to NextL() is equivalent to FirstL()
	while ( view.NextL() )
		{
		// Retrieve the current row
		view.GetL();
		// Check if this row is currently in use
		used = view.ColInt(used_col);
		if ( used )
			{
			id = view.ColUint32(id_col);
			User::LeaveIfError(aIDs.Append(id));
			}
		}

	// Cleanup view
    CleanupStack::PopAndDestroy();
    CleanupStack::Pop();
	}

// -----------------------------------------------------------------------------
// CSICommonDB::GetAllIDsL
// This function returns all Ids in the specified table.
// -----------------------------------------------------------------------------
//
void CSICommonDB::GetAllIDsL(
	const TDesC& aIdTable,
	const TDesC& aIdColumn,
	RArray<TUint32>& aIDs )
	{
	CleanupClosePushL( aIDs ); 
	
	TBuf<100> KSQLStatement;
	// Declare a literal string to hold the SQL statement
	// SELECT aIdColumn FROM aIdTable WHERE KUsedColumn = KUsed
	// ORDER BY aIdColumn
	_LIT(KSQLSelect1, "SELECT ");
	_LIT(KSQLSelect2, " FROM ");
	_LIT(KSQLSelect3, " WHERE ");
	_LIT(KSQLSelect4, " ORDER BY ");

	KSQLStatement = KSQLSelect1;
	KSQLStatement.Append(aIdColumn);
	KSQLStatement.Append(KSQLSelect2);
	KSQLStatement.Append(aIdTable);
	KSQLStatement.Append(KSQLSelect3);
	KSQLStatement.Append(KUsedColumn);
	KSQLStatement.Append(KEqual);
	KSQLStatement.AppendNum(KUsed);
	KSQLStatement.Append(KSQLSelect4);
	KSQLStatement.Append(aIdColumn);

	RDbView view;
	CleanupClosePushL(view);
	User::LeaveIfError(view.Prepare(iDb, TDbQuery(KSQLStatement, EDbCompareNormal)));
	User::LeaveIfError(view.EvaluateAll());

	// Get column set for column ordinals
    CDbColSet* columns = view.ColSetL();
    TDbColNo id_col = columns->ColNo(aIdColumn);
	delete columns;

	TUint32 id;
	// After evaluation, the first call to NextL() is equivalent to FirstL()
	while ( view.NextL() )
		{
		// Retrieve the current row
		view.GetL();
		id = view.ColUint32(id_col);
		User::LeaveIfError(aIDs.Append(id));
		}

	// Cleanup view
    CleanupStack::PopAndDestroy();
    CleanupStack::Pop(); 
	}

// -----------------------------------------------------------------------------
// CSICommonDB::IsValidL
// Checks to see if aKey exists in the table and that it's set to "used".
// -----------------------------------------------------------------------------
//
TBool CSICommonDB::IsValidL( const TDesC& aTableName, const TDesC& aIndex,
                             TUint aKey )
	{
    RUBY_DEBUG_BLOCK( "CSICommonDB::IsValidL" );
	RDbTable dbTable;		// Provides access to table data as a rowset
	TBool valid = EFalse;

	// Open table
	User::LeaveIfError(dbTable.Open(iDb, aTableName, RDbTable::EReadOnly));
	CleanupClosePushL(dbTable);
	User::LeaveIfError(dbTable.SetIndex(aIndex));

	TDbSeekKey key(aKey);
	// Return true, if a matching row found
	if ( dbTable.SeekL(key) )
		{
        RUBY_DEBUG1( "CSICommonDB::IsValidL - %d is found", aKey );
		dbTable.GetL();
		// Get column set for column ordinals
		CDbColSet* columns = dbTable.ColSetL();
		TDbColNo used_col = columns->ColNo(KUsedColumn);
		delete columns;

		// Check if this row is currently in use
		valid = dbTable.ColInt(used_col);
		}

    // Cleanup dbTable
    CleanupStack::PopAndDestroy();
	return valid;
	}

// -----------------------------------------------------------------------------
// CSICommonDB::ReleaseIdL
// Releases the ID by marking it as "unused".
// -----------------------------------------------------------------------------
//
void CSICommonDB::ReleaseIdL(
	const TDesC& aTableName,
	const TDesC& aIdColumn,
	TUint32 aId )
	{
	RUBY_DEBUG_BLOCKL( "CSICommonDB::ReleaseIdL" );
    	
	iMutex.Wait();
	
	TRAPD( error, DoReleaseIdL( aTableName, aIdColumn, aId ) );
	
	iMutex.Signal();
	
    User::LeaveIfError( error );
	}

// -----------------------------------------------------------------------------
// CSICommonDB::UpdateCounterL
// Updates the counter value of the specified row.
// -----------------------------------------------------------------------------
//
void CSICommonDB::UpdateCounterL( const TDesC& aTableName, const TDesC& aIndex,
                                  TUint aKey, TBool aIncrement )
	{
    RUBY_DEBUG_BLOCK( "CSICommonDB::UpdateCounterL" );
	RDbTable dbTable;		// Provides access to table data as a rowset

	// Open table
	User::LeaveIfError(dbTable.Open(iDb, aTableName, RDbTable::EUpdatable));
	CleanupClosePushL(dbTable);
	User::LeaveIfError(dbTable.SetIndex(aIndex));

	TDbSeekKey key(aKey);
	// Return true, if a matching row found
	if ( dbTable.SeekL(key) )
		{
		dbTable.GetL();
		// Get column set for column ordinals
		CDbColSet* columns = dbTable.ColSetL();
		TDbColNo counter_col = columns->ColNo(KCounterColumn);
		delete columns;

		TInt counter = dbTable.ColInt(counter_col);

		if ( aIncrement )
			{
			counter++;
			}
		else
			{
			counter--;
			}
		// Update found row
		dbTable.UpdateL();
		dbTable.SetColL(counter_col, counter);

		// Write the updated row
		TRAPD(err, dbTable.PutL());
		if( err != KErrNone )
			{
			// Error: cancel update
			dbTable.Cancel();
			dbTable.Reset();
			User::Leave(err);
			}
		}
	else
		{
        RUBY_DEBUG1( "CSICommonDB::UpdateCounterL - can't find key=%d", aKey );
		User::Leave(KErrNotFound);
		}

    // Cleanup dbTable
    CleanupStack::PopAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSICommonDB::VerifyOwnershipL
// Checks for data ownership.
// -----------------------------------------------------------------------------
//
void CSICommonDB::VerifyOwnershipL( TUid aClientUid, const TDesC& aTableName,
                                    const TDesC& aIndex, TUint aKey )
	{
    RUBY_DEBUG_BLOCK( "CSICommonDB::VerifyOwnershipL" );
	RDbTable dbTable;		// Provides access to table data as a rowset

	// Open table
	User::LeaveIfError(dbTable.Open(iDb, aTableName, RDbTable::EReadOnly));
	CleanupClosePushL(dbTable);
	User::LeaveIfError(dbTable.SetIndex(aIndex));

	TDbSeekKey key(aKey);
	// Return true, if a matching row found
	if ( dbTable.SeekL(key) )
		{
        RUBY_DEBUG1( "CSICommonDB::VerifyOwnershipL - %d is found", aKey );
		dbTable.GetL();
		// Get column set for column ordinals
		CDbColSet* columns = dbTable.ColSetL();
		TDbColNo client_uid_col = columns->ColNo(KClientUidColumn);
		TDbColNo used_col = columns->ColNo(KUsedColumn);
		delete columns;

		// Check if this row is currently in use
		TBool used = dbTable.ColInt(used_col);
		if ( used )
			{
			if ( dbTable.ColInt(client_uid_col) != aClientUid.iUid )
				{
				User::Leave(KErrAsrDataRightViolation);
				}
			}
		else
			{
            RUBY_DEBUG1( "CSICommonDB::VerifyOwnershipL - key=%d is unused", aKey );
			User::Leave(KErrNotFound);
			}
		}
	else
		{
        RUBY_DEBUG1( "CSICommonDB::VerifyOwnershipL - can't find key=%d", aKey );
		User::Leave(KErrNotFound);
		}

    // Cleanup dbTable
    CleanupStack::PopAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSICommonDB::DoReleaseIdL
// Releases the ID by marking it as "unused".
// -----------------------------------------------------------------------------
//
void CSICommonDB::DoReleaseIdL(
	const TDesC& aTableName,
	const TDesC& aIdColumn,
	TUint32 aId )
	{
	TBuf<100> KSQLStatement;
	// Declare a literal string to hold the SQL statement
	// UPDATE aTableName SET KUsedColumn = KNotUsed WHERE aIdColumn = aId
	_LIT(KSQLUpdate1, "UPDATE ");
	_LIT(KSQLUpdate2, " SET ");
	_LIT(KSQLUpdate3, " WHERE ");

	KSQLStatement = KSQLUpdate1;
	KSQLStatement.Append(aTableName);
	KSQLStatement.Append(KSQLUpdate2);
	KSQLStatement.Append(KUsedColumn);
	KSQLStatement.Append(KEqual);
	KSQLStatement.AppendNum(KNotUsed);
	KSQLStatement.Append(KSQLUpdate3);
	KSQLStatement.Append(aIdColumn);
	KSQLStatement.Append(KEqual);
	KSQLStatement.AppendNumUC(aId);

	User::LeaveIfError(iDb.Execute(KSQLStatement));
	}
	
// -----------------------------------------------------------------------------
// CSICommonDB::DoCreateNewIDL
// This function first checks to see if there is an "unused" ID.  If one is not
// found, it creates a new row in the table for a new ID.
// -----------------------------------------------------------------------------
//
TUint32 CSICommonDB::DoCreateNewIDL(
	const TDesC& aTableName,
	const TDesC& aIdColumn,
	TUid aClientUid )
	{
	RDbTable dbTable;		// Provides access to table data as a rowset

	// Open table
	User::LeaveIfError(dbTable.Open(iDb, aTableName, RDbTable::EUpdatable));
	CleanupClosePushL(dbTable);

    // Get column set for column ordinals
    CDbColSet* columns = dbTable.ColSetL();
    TDbColNo id_col = columns->ColNo(aIdColumn);
	TDbColNo uid_col = columns->ColNo(KClientUidColumn);
    TDbColNo counter_col = columns->ColNo(KCounterColumn);
    TDbColNo used_col = columns->ColNo(KUsedColumn);
    delete columns;

    // Is there an unused ID?
    if( FindUnusedIDL(dbTable) )
    	{
		// Yes, update found row
		dbTable.UpdateL();
		}
	else
		{
		// No, insert new row
		dbTable.InsertL();
		}

	dbTable.SetColL(uid_col, aClientUid.iUid);
	dbTable.SetColL(counter_col, 0);
	dbTable.SetColL(used_col, KUsed);

	// Write the updated row
	TRAPD(err, dbTable.PutL());
	if( err != KErrNone )
		{
		// Error: cancel update
		dbTable.Cancel();
		dbTable.Reset();
		User::Leave(err);
		}

	TUint32 newID = dbTable.ColUint32(id_col);
    // Cleanup dbTable
    CleanupStack::PopAndDestroy();
	return newID;
	}


//  End of File
