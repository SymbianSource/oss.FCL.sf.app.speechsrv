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


// INCLUDE FILES
#include "sigrammardb.h"
#include "rubydebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIGrammarDB::CSIGrammarDB
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIGrammarDB::CSIGrammarDB( RDbNamedDatabase& aDatabase, 
                            RDbs& aDbSession,
                            TInt aDrive )
: CSICommonDB(aDatabase, aDbSession, aDrive )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::ConstructL()
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIGrammarDB* CSIGrammarDB::NewL( RDbNamedDatabase& aDatabase, 
                                  RDbs& aDbSession,
                                  TInt aDrive )
    {
    RUBY_DEBUG_BLOCK( "CSIGrammarDB::NewL" );
    
    CSIGrammarDB* self 
        = new( ELeave ) CSIGrammarDB( aDatabase, aDbSession, aDrive );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );	
    return self;
    }
  
// -----------------------------------------------------------------------------
// CSIGrammarDB::~CSIGrammarDB
// Destructor
// -----------------------------------------------------------------------------
//
CSIGrammarDB::~CSIGrammarDB()
    {
    RUBY_DEBUG0( "CSIGrammarDB::~CSIGrammarDB" );
	// Delete all elements of the array before deleting the array
	iGrammarArray.ResetAndDestroy();
	iGrammarArray.Close();	
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::CreateGrammarL
// Creates a new grammar  in the database.
// -----------------------------------------------------------------------------
//
TSIGrammarID CSIGrammarDB::CreateGrammarL( TUid aClientUid )
    {
    RUBY_DEBUG_BLOCK( "CSIGrammarDB::CreateGrammarL" );

    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, sizeof( CSIGrammar ) ) );
	
	TSIGrammarID grammarID = STATIC_CAST(TSIGrammarID,CreateNewIDL(KGrammarIdTable, KGrammarIdColumn, aClientUid));
	// Construct the table name using the new grammar ID
	TBuf<40> KGrammarName( KSIGrammarTable );
    KGrammarName.AppendNumUC( grammarID );
    RUBY_DEBUG1( "CSIGrammarDB::CreateGrammarL grammar ID: %i", grammarID );
	
	// Create a table definition
	CDbColSet* columns = CDbColSet::NewLC();
	
	// add the columns  
	// 1) Binary data, 2) binary data size
	columns->AddL( TDbCol( KBinaryGrammarColumn, EDbColLongBinary ) );
	columns->AddL( TDbCol( KBinaryGrammarColumnSize, EDbColUint32 ) );
	
	// Create a table
	TInt err =iDb.CreateTable( KGrammarName, *columns );
	
	if ( err != KErrNone )
	    {
		// Failed to create the table.
		// Need to release the new grammar ID and leave.
		ReleaseIdL( KGrammarIdTable, KGrammarIdColumn, grammarID );
		User::Leave( err );
	    }

	// Cleanup the column set
	CleanupStack::PopAndDestroy( columns );
	
	// Construct the table name using the provided grammar ID
	// Declare a literal string to hold the SQL statement
	// SELECT KBinaryGrammarColumn , KBinaryGrammarColumn FROM  KGrammarName
 	_LIT(KSQLSelect1,"select  ");	
	_LIT(KSQLSelect2," from  ");		
	TBuf<120> KSQLStatement;		
	KSQLStatement.Append(KSQLSelect1  );	
	KSQLStatement.Append(KBinaryGrammarColumn );
	KSQLStatement.Append(KNext);
	KSQLStatement.Append(KBinaryGrammarColumnSize );	
	KSQLStatement.Append(KSQLSelect2  );
	KSQLStatement.Append(KGrammarName); 
	
	// Create a view on the database
	RDbView view;
	User::LeaveIfError(view.Prepare(iDb,TDbQuery(KSQLStatement,EDbCompareNormal)));
	User::LeaveIfError(view.EvaluateAll());
	
	// Get the structure of rowset
	CDbColSet* colSet = view.ColSetL(); 
 	// Insert a row
	view.InsertL();
	view.PutL();
 	// Close the view
	view.Close();
	delete colSet; 

	// Put a empty grammar into the database
	CSICompiledGrammar *aSICompiledGrammar=CSICompiledGrammar::NewL(grammarID);
 	CleanupStack::PushL(aSICompiledGrammar);
	UpdateGrammarL(aClientUid,aSICompiledGrammar);
	CleanupStack::PopAndDestroy(aSICompiledGrammar);

    iDbSession.FreeReservedSpace( iDrive );

 	return grammarID;
    } 

// -----------------------------------------------------------------------------
// CSIGrammarDB::UpdateGrammarL
// Inserts the externalized SI compiled grammar  into the specified grammar table.
// Save the grammar into the database
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::UpdateGrammarL( TUid aClientUid, 
                                   CSICompiledGrammar *aSICompiledGrammar )
    {
	RUBY_DEBUG_BLOCK( "CSIGrammarDB::UpdateGrammarL" );

	// verify ownership 
	TSIGrammarID aGrammarID=aSICompiledGrammar->GrammarID();
	VerifyOwnershipL(aClientUid, KGrammarIdTable, KGrammarIndex, aGrammarID);
	
	// Construct the table name using the provided grammar ID
	// Declare a literal string to hold the SQL statement
	// SELECT KBinaryGrammarColumn , KBinaryGrammarColumn FROM  KGrammarName
	TBuf<40> KGrammarName(KSIGrammarTable);
    KGrammarName.AppendNumUC(aGrammarID);	
	_LIT(KSQLSelect1,"select  ");	
	_LIT(KSQLSelect2," from  ");		
	TBuf<120> KSQLStatement;		
	KSQLStatement.Append(KSQLSelect1  );	
	KSQLStatement.Append(KBinaryGrammarColumn );
	KSQLStatement.Append(KNext);
	KSQLStatement.Append(KBinaryGrammarColumnSize );	
	KSQLStatement.Append(KSQLSelect2  );
	KSQLStatement.Append(KGrammarName); 
	
	// Create a view on the database
	RDbView view;
	User::LeaveIfError( view.Prepare( iDb, TDbQuery( KSQLStatement,EDbCompareNormal ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	CleanupClosePushL( view );
	
	// Get the structure of rowset
	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL( colSet );
	view.FirstL(); 
	view.UpdateL();
  
	// Externalize a compiled grammar
	CBufFlat* dataCopyBuffer = CBufFlat::NewL( 100 ); // 100 = expand 100 bytes
    CleanupStack::PushL( dataCopyBuffer );              // when the buffer is full
 	RBufWriteStream stream;
	stream.Open( *dataCopyBuffer );
    CleanupClosePushL( stream );
	aSICompiledGrammar->ExternalizeL( stream );
	CleanupStack::PopAndDestroy( &stream );
	
	TPtr8 aWriteBuf( dataCopyBuffer->Ptr( 0 ) );
	TInt BufSize = aWriteBuf.Size();
	
	// add binary buffer by Using the stream  
	RDbColWriteStream out;
	TDbColNo col = colSet->ColNo( KBinaryGrammarColumn ); // Ordinal position of long column	
	out.OpenLC( view, col );
	out.WriteL( aWriteBuf );	
	out.Close();	
	CleanupStack::PopAndDestroy(); // out

	// add  size of the buffer
	col = colSet->ColNo( KBinaryGrammarColumnSize ); // Ordinal position of  size	
	view.SetColL( col, BufSize ); 	
	view.PutL();
 
	CleanupStack::PopAndDestroy( dataCopyBuffer );
	CleanupStack::PopAndDestroy( colSet );

	// close the view
	CleanupStack::PopAndDestroy( &view ); // Close view
    }


// -----------------------------------------------------------------------------
// CSIGrammarDB::FindGrammarL
// Find a grammar from loaded grammar array
// -----------------------------------------------------------------------------
CSICompiledGrammar*  CSIGrammarDB::FindGrammarL(TSIGrammarID aGrammarID) 
    {
    RUBY_DEBUG_BLOCK( "CSIGrammarDB::FindGrammarL" );
    
    TInt i(0);
    CSICompiledGrammar* aGrammar=NULL;
    
    // Check if grammar already loaded
    for ( i = 0; i < iGrammarArray.Count(); i++ )
        {
        if ( iGrammarArray[i]->GrammarID()==aGrammarID  )
            { 
            aGrammar=iGrammarArray[i];
            break;
            }		
        }
    // Can not found in the loaded grammar array
    if(i==iGrammarArray.Count())
        User::Leave(KErrNotFound);
    
    return aGrammar;
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::GetNewID
// Get a new uniq ID 
// -----------------------------------------------------------------------------
TInt CSIGrammarDB::GetNewID( RArray<TSIRuleID>& aMyIds )
    {
    RArray<TSIRuleID> myIDs = aMyIds;
    TInt Count = myIDs.Count();
    TInt id = 0;
    if ( Count == 0 )
        {
        id = 1; // empty ,first id will be 1
        }
    else
        { 
        // Find a unique  ID
        myIDs.SortUnsigned();
        id = myIDs[myIDs.Count() - 1] + 1; //by default , the last one 
        for ( TInt i = 0; i < myIDs.Count(); i++ )
            {
            TInt index = i + 1; 
            TInt s = myIDs[i];
            if ( s > index )
                {
                id = index;
                break;
                }
            }
        }
    return id;
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::CreateRuleL
// Creates a new empty rule.
// -----------------------------------------------------------------------------
void CSIGrammarDB::CreateRuleL( TUid aClientUid, TSIGrammarID aGrammarID, 
                                TSIRuleID& aRuleID )
    {
    RUBY_DEBUG_BLOCK( "CSIGrammarDB::CreateRuleL" );

    //__UHEAP_MARK;
    VerifyOwnershipL( aClientUid, KGrammarIdTable, KGrammarIndex, aGrammarID );
    // find the grammar from the database 
    CSICompiledGrammar* aGrammar = (CSICompiledGrammar*) GrammarL( aGrammarID );
    CleanupStack::PushL( aGrammar );
    
    TInt aID = 0;
    TInt Count=aGrammar->Count();
    
    // existing myRuleVariantID
    RArray<TSIRuleID> myIDs;
    myIDs.Reset();
    for(TInt i = 0; i < Count; i++ )
        {
        CSIRule* aRule = &( aGrammar->AtL( i ) );
        myIDs.Append( aRule->RuleID() );
        } 
    
    // Find a uniq new id 
    aID=GetNewID( myIDs );
    myIDs.Close();
    aRuleID=aID;
    
    // add the rule to the grammar	
    CSIRule* rule = CSIRule::NewL( aRuleID );
    CleanupStack::PushL( rule );	
    aGrammar->AddL( rule );
    CleanupStack::Pop( rule );
    
    UpdateGrammarL( aClientUid, aGrammar );
    CleanupStack::PopAndDestroy( aGrammar );
    //__UHEAP_MARKEND;
    } 

// -----------------------------------------------------------------------------
// CSIGrammarDB::AddRuleVariantL
// Add Rule Variant to the array that hold the grammar array
// -----------------------------------------------------------------------------
void CSIGrammarDB::AddRuleVariantL(TUid aClientUid,
								   TSIGrammarID aGrammarID, 								 
								   TSILexiconID aLexiconID,
								   const RArray<TSIPronunciationID>& aPronunciationIDs, 								     
								   TSIRuleID aRuleID,
								   TSIRuleVariantID& aRuleVariantID) 
    {
    VerifyOwnershipL(aClientUid, KGrammarIdTable, KGrammarIndex, aGrammarID);
    
    CSICompiledGrammar* aGrammar=(CSICompiledGrammar*)GrammarL(aGrammarID);
    CleanupStack::PushL(aGrammar);
    
    CSIRule* aRule=&(aGrammar->RuleL(aRuleID));
    // existing myRuleVariantID
    RArray<TSIRuleID> myID;
    myID.Reset();
    for( TInt i = 0; i < aRule->Count(); i++ )
        {
        CSIRuleVariant* aRuleVariant=&(aRule->AtL(i));
        myID.Append(aRuleVariant->RuleVariantID());
        }
    
    // Find a uniq new id 
    aRuleVariantID=STATIC_CAST(TSIRuleVariantID,GetNewID(myID));
    myID.Close();	
    
    // add the rule variant to the rule
    CSIRuleVariant* ruleVariant = CSIRuleVariant::NewL( aRuleVariantID, aLexiconID );
    CleanupStack::PushL( ruleVariant );	
    ruleVariant->SetPronunciationIDsL(aPronunciationIDs);
    aRule->AddL( ruleVariant );
    CleanupStack::Pop( ruleVariant );
    UpdateGrammarL( aClientUid, aGrammar );
    CleanupStack::PopAndDestroy( aGrammar );	  
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::LoadGrammarL
// Loads all rules within the specified grammar into a grammar object, which
// is loaded into recognizer during recognition session.
// Note : Will leave if already loaded
// -----------------------------------------------------------------------------
//
const CSICompiledGrammar* CSIGrammarDB::LoadGrammarL( TSIGrammarID aGrammarID )
    {
    // Construct the table name using the provided grammar ID
    // Construct the table name using the provided grammar ID
    // Declare a literal string to hold the SQL statement
    // SELECT KBinaryGrammarColumn , KBinaryGrammarColumn FROM  KGrammarName
    TBuf<40> KGrammarName(KSIGrammarTable);
    KGrammarName.AppendNumUC(aGrammarID);
    
    // Create newGrammar object	
    CSICompiledGrammar* grammar = CSICompiledGrammar::NewLC( aGrammarID );	
    _LIT(KSQLSelect1,"select  ");	
    _LIT(KSQLSelect2," from  ");
    TBuf<120> KSQLStatement;		
    KSQLStatement.Append(KSQLSelect1  );		
    KSQLStatement.Append(KBinaryGrammarColumn );
    KSQLStatement.Append(KNext);
    KSQLStatement.Append(KBinaryGrammarColumnSize );
    KSQLStatement.Append(KSQLSelect2  );
    KSQLStatement.Append(KGrammarName); 
    
    // create a view on the database
    RDbView view;
    CleanupClosePushL( view );
    
    User::LeaveIfError(view.Prepare(iDb,TDbQuery(KSQLStatement,EDbCompareNormal)));
    User::LeaveIfError(view.EvaluateAll());
    
    // Get the structure of the rowset 
    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL( colSet );
    
    // iterate across the row set, one row only
    for (view.FirstL();view.AtRow();view.NextL())
        {
        
        // retrieve the row
        view.GetL();		
        
        // first retrieve the size of binary package 
        TDbColNo col = colSet->ColNo(KBinaryGrammarColumnSize); // Ordinal position of long column 
        TInt32 size = view.ColUint32( col);
        
        TUint8* buf = new (ELeave) TUint8[size];		
        CleanupStack::PushL( buf);			
        TPtr8 readBuf( buf, size, size);
        CBufFlat* dataCopyBuffer = CBufFlat::NewL( 100 ); // 100 = expand 100 bytes
        CleanupStack::PushL(dataCopyBuffer);              // when the buffer is full
        
        RBufReadStream stream;
        CleanupClosePushL( stream );
        stream.Open(*dataCopyBuffer);
        
        // and a stream for long columns
        RDbColReadStream in;
        CleanupClosePushL( in );
        col = colSet->ColNo(KBinaryGrammarColumn); // Ordinal position of long column 
        
        
        in.OpenLC(view, col);
        in.ReadL( readBuf, view.ColLength(col));
        dataCopyBuffer->InsertL(0,readBuf);
        grammar->InternalizeL( stream );
        
        CleanupStack::PopAndDestroy( col );
        CleanupStack::PopAndDestroy( &in );
        CleanupStack::PopAndDestroy( &stream ); 
        CleanupStack::PopAndDestroy( dataCopyBuffer );
        CleanupStack::PopAndDestroy( buf );
        } 
        
    CleanupStack::PopAndDestroy( colSet );
    CleanupStack::PopAndDestroy( &view );
    
    // Keep the reference of the grammar object
    User::LeaveIfError(iGrammarArray.Append(grammar));
    // Cleanup grammar
    CleanupStack::Pop(grammar );
    return grammar;
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::GrammarL
// Loads all rules within the specified grammar into a grammar object, which
// is loaded into recognizer during recognition session.
// Note : Will leave if already loaded
// -----------------------------------------------------------------------------
//
const CSICompiledGrammar* CSIGrammarDB::GrammarL( TSIGrammarID aGrammarID )
    {
 	  
    // Construct the table name using the provided grammar ID
    // Construct the table name using the provided grammar ID
    // Declare a literal string to hold the SQL statement
    // SELECT KBinaryGrammarColumn , KBinaryGrammarColumn FROM  KGrammarName
    TBuf<40> KGrammarName(KSIGrammarTable);
    KGrammarName.AppendNumUC(aGrammarID);
    // Create newGrammar object	 
    CSICompiledGrammar* aGrammar = CSICompiledGrammar::NewLC( aGrammarID );
    _LIT(KSQLSelect1,"select  ");
    _LIT(KSQLSelect2," from  ");
    
    TBuf<120> KSQLStatement;		
    KSQLStatement.Append(KSQLSelect1  );		
    KSQLStatement.Append(KBinaryGrammarColumn );
    KSQLStatement.Append(KNext);
    KSQLStatement.Append(KBinaryGrammarColumnSize );
    KSQLStatement.Append(KSQLSelect2  );
    KSQLStatement.Append(KGrammarName); 
    
    // create a view on the database
    RDbView view;
    User::LeaveIfError(view.Prepare(iDb,TDbQuery(KSQLStatement,EDbCompareNormal)));
    
    User::LeaveIfError(view.EvaluateAll());
    
    // Get the structure of the rowset 
    CDbColSet* colSet = view.ColSetL();
    
    // iterate across the row set, one row only
    for (view.FirstL();view.AtRow();view.NextL())
        {
        
        // retrieve the row
        view.GetL();
        
        // first retrieve the size of binary package 
        TDbColNo col = colSet->ColNo(KBinaryGrammarColumnSize); // Ordinal position of long column 
        TInt32 Size = view.ColUint32( col);
        
        TUint8* aBuf = new (ELeave) TUint8[Size];		
        CleanupStack::PushL(aBuf);			
        TPtr8 aReadBuf(aBuf, Size, Size);
        CBufFlat* dataCopyBuffer = CBufFlat::NewL( 100 ); // 100 = expand 100 bytes
        CleanupStack::PushL(dataCopyBuffer);              // when the buffer is full
        
        RBufReadStream stream;
        stream.Open(*dataCopyBuffer);
        
        // and a stream for long columns
        RDbColReadStream in;
        col = colSet->ColNo(KBinaryGrammarColumn); // Ordinal position of long column 
        
        
        in.OpenLC(view, col);
        in.ReadL(aReadBuf, view.ColLength(col));
        dataCopyBuffer->InsertL(0,aReadBuf);
        aGrammar->InternalizeL( stream );
        
        in.Close(); 		
        CleanupStack::PopAndDestroy(3); // in dataCopyBuffer aBuf
        }
    //! clean the memory
    delete colSet;
    view.Close();  
    
    // Keep the reference of the grammar object
    // Cleanup grammar
    CleanupStack::Pop( aGrammar );
    
    return aGrammar;
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::CreateIDTableL
// Creates a new grammar ID table in the database.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::CreateIDTableL()
    {
    // Invoke function in the base class CSICommonDB.
    CSICommonDB::CreateIDTableL(KGrammarIdTable, KGrammarIdColumn, KGrammarIndex);
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::GetAllClientGrammarIDsL
// This function returns all grammar IDs owned by the specified client.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::GetAllClientGrammarIDsL( TUid aClientUid,
                                            RArray<TSIGrammarID>& aGrammarIDs )
    {
    RArray<TUint32> ix; 
    ix.Reset();	
    GetAllClientIDsL(KGrammarIdTable, KGrammarIdColumn, aClientUid, ix);
    for(TInt i=0;i<ix.Count();i++) {
        aGrammarIDs.Append(STATIC_CAST(TSIGrammarID,ix[i]));
        }
    ix.Close();
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::GetAllGrammarIDsL
// This function returns all grammar IDs in the database.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::GetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs )
    {
    // This is a hack to get the id aligned to 4-byte boundary,
    // for some reason this does not happen in winscw build if 
    // TSIGrammarID is taken from stack.
    TSIGrammarID* id = new (ELeave) TSIGrammarID;
    
    CleanupStack::PushL( id );
    
    RArray<TUint32> ix;
    //! Reset
    ix.Reset(); 
    GetAllIDsL(KGrammarIdTable, KGrammarIdColumn,  ix);
    for(TInt i=0;i<ix.Count();i++) 
        {
        *id = STATIC_CAST( TSIGrammarID, ix[i] );
        aGrammarIDs.Append( *id );
        }
    ix.Close();
    CleanupStack::PopAndDestroy( id );
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::GetAllRuleIDsL
// This function returns all rule IDs within the specified grammar.
// grammar have to be loaded 
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::GetAllRuleIDsL( TSIGrammarID aGrammarID,
                                   RArray<TSIRuleID>& aRuleIDs )
    {
	CleanupClosePushL( aRuleIDs );
	
    // Construct the table name using the provided grammar ID
    TBuf<40> KGrammarName(KSIGrammarTable);
    KGrammarName.AppendNumUC(aGrammarID);
    
    CSICompiledGrammar* newgrammar =( CSICompiledGrammar* )GrammarL( aGrammarID) ;// Load the grammar from database	
    CleanupStack::PushL(newgrammar);
    TSIRuleID	RuleId;
    
    for (TInt  i=0;i<newgrammar->Count();i++) {
        CSIRule* aRule=&(newgrammar->AtL(i));
        RuleId=aRule->RuleID();		
        User::LeaveIfError(aRuleIDs.Append(RuleId));
        } 
    CleanupStack::PopAndDestroy(newgrammar);
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::RemoveGrammarL
// Deletes a grammar table from the database.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::RemoveGrammarL( TUid aClientUid,
                                   TSIGrammarID aGrammarID )
    {
    VerifyOwnershipL(aClientUid, KGrammarIdTable, KGrammarIndex, aGrammarID);
    
    TInt diskSpace = ( RuleCountL(aGrammarID) * sizeof( CSIRule) ) 
        + sizeof( CSIGrammar );
    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, diskSpace ) );
    
    // Construct the table name using the provided grammar ID
    TBuf<40> KGrammarName(KSIGrammarTable);
    KGrammarName.AppendNumUC(aGrammarID);
    
    TBuf<50> KSQLStatement;
    // Declare a literal string to hold the SQL statement
    // DROP TABLE KGrammarName
    _LIT(KSQLDelete1, "DROP TABLE ");
    
    KSQLStatement = KSQLDelete1;
    KSQLStatement.Append(KGrammarName);
    
    User::LeaveIfError(iDb.Execute(KSQLStatement));
    
    // Release the grammar ID
    ReleaseIdL(KGrammarIdTable, KGrammarIdColumn, aGrammarID);
    
    // Cancel free disk space request
    iDbSession.FreeReservedSpace( iDrive );
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::RuleCountL
// Returns the number of rules in the specified grammar.
// Grammar have to be loaded
// -----------------------------------------------------------------------------
//
TInt CSIGrammarDB::RuleCountL( TSIGrammarID aGrammarID )
    {
    // Load a grammar
    CSICompiledGrammar* aGrammar=(CSICompiledGrammar*)GrammarL(aGrammarID);
    CleanupStack::PushL(aGrammar);  
    // Count the number of the rules inside the grammar
    TInt RuleCount=aGrammar->Count();	 
    CleanupStack::PopAndDestroy(aGrammar);
    return RuleCount;
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::RemoveRuleL
// Deletes Rule from the database.
// Grammar have to be loaded
// -----------------------------------------------------------------------------
//  
void CSIGrammarDB::RemoveRuleL( TUid aClientUid, 
                                TSIGrammarID aGrammarID,
                                TSIRuleID aRuleID )
    {
    VerifyOwnershipL(aClientUid, KGrammarIdTable, KGrammarIndex, aGrammarID);
    CSICompiledGrammar* aGrammar=(CSICompiledGrammar*)GrammarL(aGrammarID);	 
    
    CleanupStack::PushL(aGrammar);  
    if (aGrammar->Find(aRuleID)==KErrNotFound)
        {
        User::Leave(KErrNotFound);
        }
    else
        {
        aGrammar->DeleteL(aRuleID);
        }
    UpdateGrammarL(aClientUid,aGrammar);
    CleanupStack::PopAndDestroy(aGrammar);
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::IsGrammarLoaded
// Checks to see if a valid grammar is loaded into the recognizer.  This is
// useful to check if any rules are still loaded after performing UnloadRule().
// -----------------------------------------------------------------------------
//
TBool CSIGrammarDB::IsGrammarLoaded()
    {
    for ( TInt i = 0; i < iGrammarArray.Count(); i++ )
        {
        if ( iGrammarArray[i]->Count() > 0 )
            {
            // As long as there is still a rule.
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::IsRuleValidL
// Checks if the rule is valid or not.
// -----------------------------------------------------------------------------
//
TBool CSIGrammarDB::IsRuleValidL( TSIGrammarID aGrammarID, TSIRuleID aRuleID )
    {
    CSICompiledGrammar* aGrammar=(CSICompiledGrammar*)GrammarL(aGrammarID);
    CleanupStack::PushL(aGrammar);  
    if (aGrammar->Find(aRuleID)==KErrNotFound)
        {
        CleanupStack::PopAndDestroy(aGrammar);
        return EFalse; 
        }
    else {
        CleanupStack::PopAndDestroy(aGrammar);
        return ETrue;	
        }
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::ResetAndDestroy
// Deallocates the temporary memory containing the grammar object created with
// AllRulesL.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::ResetAndDestroy()
    {
    iGrammarArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::UnloadRule
// Unloads the specified rule from the specified grammar in temporary memory,
// previously loaded with AllRulesL.  The rule in the database remains intact.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::UnloadRuleL( TSIGrammarID aGrammarID,
                                TSIRuleID aRuleID )
    {
    CSIGrammar* grammar;
    
    for( TInt i = 0; i < iGrammarArray.Count(); i++ )
        {
        grammar = iGrammarArray[i];
        if ( grammar->GrammarID() == aGrammarID )
            {
            grammar->DeleteL(aRuleID);
            return;
            }
        }
    User::Leave(KErrNotFound);
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::UnloadGrammar
// Unloads  the specified grammar in temporary memory,
// previously loaded with AllRulesL.  The grammar in the database remains intact.
// -----------------------------------------------------------------------------
//
void CSIGrammarDB::UnloadGrammarL( TSIGrammarID aGrammarID)
    {
    CSIGrammar* grammar;
    
    for( TInt i = 0; i < iGrammarArray.Count(); i++ )
        {
        grammar = iGrammarArray[i];
        if ( grammar->GrammarID() == aGrammarID )
            {
            //! delete the object 
            delete iGrammarArray[i];
            // remove the pointer
            iGrammarArray.Remove(i);
            return;
            }
        }
    User::Leave(KErrNotFound);
    }

//  End of File
