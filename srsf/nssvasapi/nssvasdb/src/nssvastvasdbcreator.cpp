/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TNssVasDbCreator is responsible for creating VasDatabase.db.
*
*/


#include "nssvastvasdbcreator.h"
#include <e32cons.h>
#include <s32file.h>
#include <d32dbms.h>
#include "nssvascoreconstant.h"

// in bytes
const TInt KClientDataColSize = 100;

// ============================ Methods ===============================


// -----------------------------------------------------------------------------
// TNssVasDbCreator::TNssVasDbCreator
// -----------------------------------------------------------------------------
//
TNssVasDbCreator::TNssVasDbCreator()
    {
    }

// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateVasDatabaseL
// -----------------------------------------------------------------------------
//
void TNssVasDbCreator::CreateVasDatabaseL( RDbs& iDbSession )
    {
    RDbNamedDatabase database; 
    TInt error = database.Create( iDbSession, KVasDatabaseName, KVasDatabaseFormatString );
    CleanupClosePushL( database );
    if ( error == KErrAlreadyExists )
        {
        // Ignore KErrAlreadyExists, we will use the existing one if it is there
        error = KErrNone;
        }
    else if ( error == KErrNone )
        {
        // Create tables
        CreateTablesAndIndiciesL( database );
        }
        
    CleanupStack::PopAndDestroy( &database ); // Close database
 
    User::LeaveIfError( error );
    }
    
// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateTablesAndIndiciesL
// -----------------------------------------------------------------------------
//	    
void TNssVasDbCreator::CreateTablesAndIndiciesL( RDbNamedDatabase& aDatabase )
    {
    CreateContextTableL( aDatabase );
    CreateContextIndexL( aDatabase );
    CreateTagTableL( aDatabase );
    CreateTagIndexL( aDatabase );
    CreateRRDTableL( aDatabase );
    CreateRRDIndexL( aDatabase );
    CreateWriteLockTableL( aDatabase );
    }

// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateContextTableL
// -----------------------------------------------------------------------------
//	   
/*                   Context Table
*
* Was up to 2.0:
*
*     name|global|contextid|lexiconid|grammarid|modelbankid
*     ------------------------------------------------------
*         |      |         |         |         |
*
* In 2.8:
*
*  |name|global|contextid|lexiconid|grammarid|modelbankid|clientdata|
*  |----------------------------------------------------------------|
*  |    |      |         |         |         |           |          |
*/
void TNssVasDbCreator::CreateContextTableL( RDbNamedDatabase& aDatabase )
    {
	_LIT(KContextTable, "contexttable");
	_LIT(KCol1,  "name");
	_LIT(KCol2,  "global");
	_LIT(KCol3,  "contextid");
	_LIT(KCol4,  "lexiconid");
	_LIT(KCol5,  "grammarid");
	_LIT(KCol6,  "modelbankid");
    _LIT(KCol8,  "clientdata"); // NSS Extension(tm)

	// Create the context table definition (columnset).
	CDbColSet* columns = CDbColSet::NewLC();

	// add column definitions

	// The name column first.
	TDbCol dbCol1(KCol1, EDbColText,KNssVasDbContextName); 
	dbCol1.iAttributes = TDbCol::ENotNull;
	columns->AddL(dbCol1);

	// The global flag column
	TDbCol dbCol2(KCol2, EDbColUint32);
	dbCol2.iAttributes = TDbCol::ENotNull;
	columns->AddL(dbCol2);

	// The context id flag  CDbKey* key = CDbKey::NewLC();
	TDbCol dbCol3(KCol3, EDbColUint32);
	dbCol3.iAttributes = TDbCol::EAutoIncrement|TDbCol::ENotNull;
	columns->AddL(dbCol3);

	// The lexicon id 
	TDbCol dbCol4(KCol4, EDbColUint32);
	dbCol4.iAttributes = TDbCol::ENotNull;
	columns->AddL(dbCol4);

    // The grammar id 
	TDbCol dbCol5( KCol5, EDbColUint32 );
	dbCol5.iAttributes = TDbCol::ENotNull;
	columns->AddL( dbCol5 );

	 // The modelbank id 
	TDbCol dbCol6( KCol6, EDbColUint32 );
	dbCol6.iAttributes = TDbCol::ENotNull;
	columns->AddL( dbCol6 );

	// 100 bytes of client data. New in 2.8.
	TDbCol dbCol8( KCol8, EDbColBinary, KClientDataColSize );
    // Can be null
	columns->AddL( dbCol8 );

    // Now actually create the table.
	User::LeaveIfError( aDatabase.CreateTable( KContextTable, *columns ) );

	// Cleanup columns
	CleanupStack::PopAndDestroy( columns );
    }

// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateContextIndexL
// -----------------------------------------------------------------------------
//	   
void TNssVasDbCreator::CreateContextIndexL( RDbNamedDatabase& aDatabase )
    {
	_LIT( KTable, "contexttable" );
	// Same as table name is a bit weird, but we leave it unchanged to avoid
	// potential incompatibility with some other code, that might access
	// index by name
	_LIT( KIndexName, "contexttable");  
	//_LIT( KCol3,  "contextid" );
	CreateSingleColumnIndexL( aDatabase, KIndexName, KContextIdCol, ETrue, KTable);
	
    }


// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateTagTableL
// -----------------------------------------------------------------------------
//	   
/*
*             Tag Table
*
*   tagid|contextid|name|ruleid|
*   -----------------------------
*        |         |    |      |
*
*/
void TNssVasDbCreator::CreateTagTableL( RDbNamedDatabase& aDatabase )
    {
   	_LIT(KTagTable,"tagtable");
	_LIT(KCol1,  "tagid");
	_LIT(KCol2,  "contextid");
	_LIT(KCol3,  "name");
	_LIT(KCol5,  "ruleid");


	// Create the tag table definition (columnset).
	CDbColSet* columns = CDbColSet::NewLC();

		// The tag id 
	TDbCol dbCol1(KCol1, EDbColUint32);
	dbCol1.iAttributes = TDbCol::EAutoIncrement|TDbCol::ENotNull;
	columns->AddL(dbCol1);

		// The context id 
	TDbCol dbCol2(KCol2, EDbColUint32);
	dbCol2.iAttributes = TDbCol::ENotNull;
	columns->AddL(dbCol2);

	// The speechitem/tag name column.
	TDbCol dbCol3(KCol3, EDbColText,KNssVasDbSpeechItemName); 
	dbCol3.iAttributes = TDbCol::ENotNull;
	columns->AddL(dbCol3);

    // The rule id 
	TDbCol dbCol5(KCol5, EDbColUint32);
	dbCol5.iAttributes = TDbCol::ENotNull;
	columns->AddL(dbCol5);

    // Now actually create the table.
	User::LeaveIfError ( aDatabase.CreateTable(KTagTable, *columns));

	// Cleanup columns
	CleanupStack::PopAndDestroy( columns );
    }

// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateTagIndexL
// -----------------------------------------------------------------------------
//	  
void TNssVasDbCreator::CreateTagIndexL( RDbNamedDatabase& aDatabase )
    {
	_LIT( KTable, "tagtable" );
	CreateSingleColumnIndexL(aDatabase, KTable, KTagIdCol, ETrue, KTable);
	_LIT( KContextIndexName, "tagtable_contextid_idx" );
	CreateSingleColumnIndexL(aDatabase, KContextIndexName, KContextIdCol, EFalse, KTable);
	_LIT( KRuleIndexName, "tagtable_ruleid_idx" );
	CreateSingleColumnIndexL(aDatabase, KRuleIndexName, KRuleIdCol, EFalse, KTable);
	_LIT( KIndexName, "tagtable_contextid_ruleid_idx" );
	CreateTwoColumnIndexL( KIndexName, KContextIdCol, KRuleIdCol, ETrue, KTable);
    }

// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateRRDTableL
// -----------------------------------------------------------------------------
//	  
/*   RRD Text Table       RRD Int Table
*   
*   tagid|text|pos        tagid|int|pos
*    --------------        -------------
*        |    |                |   |
*/
void TNssVasDbCreator::CreateRRDTableL( RDbNamedDatabase& aDatabase )
    {
	_LIT( KRRDTextTable, "rrdtexttable" );
	_LIT( KRRDIntTable, "rrdinttable" );

	_LIT( KCol1,    "tagid" );
	_LIT( KTextCol2,"rrdtext" );
    _LIT( KIdCol2,  "rrdint" );
	_LIT( KCol3,    "rrdposition" );
   
	// Create the rrd tables definition (columnset).
	CDbColSet* textcolumns = CDbColSet::NewLC();
	CDbColSet* intcolumns  = CDbColSet::NewLC();

		// The tag id 
	TDbCol dbCol1( KCol1, EDbColUint32 );
	//dbCol1.iAttributes = TDbCol::ENotNull;
	textcolumns->AddL( dbCol1 );
    intcolumns->AddL( dbCol1 );

		// The rrd text  
	TDbCol dbCol2( KTextCol2, EDbColText, KNssVasDbRRDText );
	//dbCol2.iAttributes = TDbCol::ENotNull;
	textcolumns->AddL( dbCol2 );

	   //The rrd int
	TDbCol iddbCol2( KIdCol2, EDbColUint32 );
	//iddbCol2.iAttributes = TDbCol::ENotNull;
	intcolumns->AddL( iddbCol2 );

		// The position 
	TDbCol dbCol3( KCol3, EDbColUint32 );
	//dbCol3.iAttributes = TDbCol::ENotNull;
	textcolumns->AddL( dbCol3 );
    intcolumns->AddL( dbCol3 );

	User::LeaveIfError( aDatabase.CreateTable( KRRDTextTable, *textcolumns ) );
	User::LeaveIfError( aDatabase.CreateTable( KRRDIntTable, *intcolumns ) );

	// Cleanup textcolumns & idcolumns
	CleanupStack::PopAndDestroy( intcolumns ); 
	CleanupStack::PopAndDestroy( textcolumns ); 
    }
    
// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateRRDIndexL
// -----------------------------------------------------------------------------
//	      
void TNssVasDbCreator::CreateRRDIndexL( RDbNamedDatabase& aDatabase )
	{
	_LIT( KRRDTextTable, "rrdtexttable" );
	_LIT( KRRDIntTable, "rrdinttable" );
	_LIT( KRRDTextTagIndex, "rrdtexttable_tagid_idx");
	_LIT( KRRDIntTagIndex, "rrdinttable_tagid_idx" );
	_LIT( KRRDTextTagPosIndex, "rrdtexttable_tagid_pos_idx");
	_LIT( KRRDIntTagPosIndex, "rrdinttable_tagid__pos_idx" );
	// constraints
	CreateTwoColumnIndexL(KRRDTextTagPosIndex, KTagIdCol, KRRDPositionCol, ETrue, KRRDTextTable);
	CreateTwoColumnIndexL(KRRDIntTagPosIndex, KTagIdCol, KRRDPositionCol, ETrue, KRRDIntTable);
	// and for actual searches
	CreateSingleColumnIndexL(aDatabase, KRRDTextTagIndex, KTagIdCol, EFalse, KRRDTextTable);
	CreateSingleColumnIndexL(aDatabase, KRRDIntTagIndex, KTagIdCol, EFalse, KRRDIntTable);
	}
    

// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateWriteLockTableL
// -----------------------------------------------------------------------------
//	    
/*   Write lock table
*   
*    temporary 
*   |---------|
*   |         |
*/
void TNssVasDbCreator::CreateWriteLockTableL( RDbNamedDatabase& aDatabase )
    {
	_LIT( KWriteLockTable, "writelocktable" );

	_LIT( KCol1,    "temporary" );
   
	// Create the rrd tables definition (columnset).
	CDbColSet* columns = CDbColSet::NewLC();

    // The tag id 
	TDbCol dbCol1( KCol1, EDbColUint32 );
	columns->AddL( dbCol1 );

	User::LeaveIfError( aDatabase.CreateTable( KWriteLockTable, *columns ) );

	// Cleanup columns
	CleanupStack::PopAndDestroy( columns ); 
    }
    
// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateSingleColumnIndexL
// -----------------------------------------------------------------------------
//	      
void TNssVasDbCreator::CreateSingleColumnIndexL( RDbNamedDatabase& aDatabase,
                                                 const TDesC& aIndexName,
                                                 const TDesC& aColumnName,
                                                 const TBool aUnique,
                                                 const TDesC& aTableName)
	{
	// Create the index key...
	CDbKey* key = CDbKey::NewLC();

	// and add the key columns.
	TDbKeyCol keyCol( aColumnName );
	key->AddL( keyCol );
	if( aUnique ) 
		{
		key->MakeUnique();
		}
	
	// Create the index
	User::LeaveIfError( aDatabase.CreateIndex( aIndexName, aTableName, *key ) );

	// Cleanup key
	CleanupStack::PopAndDestroy(key);
	}

// -----------------------------------------------------------------------------
// TNssVasDbCreator::CreateTwoColumnIndexL
// -----------------------------------------------------------------------------
//	      
void TNssVasDbCreator::CreateTwoColumnIndexL( const TDesC& /*anIndexName*/, 
                                              const TDesC& /*aFirstColumnName*/, 
                                              const TDesC& /*aSecondColumnName*/, 
                                              const TBool /*aUnique*/, 
                                              const TDesC& /*aTableName*/)
	{
	}
	
