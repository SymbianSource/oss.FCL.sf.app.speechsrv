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
*               lexicons.  It is also responsible for allocating memory when
*               loading lexicons into the recognizer.
*
*/


// INCLUDE FILES
#include "silexicondb.h"
#include "rubydebug.h"

// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSILexiconDB::CSILexiconDB
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSILexiconDB::CSILexiconDB( RDbNamedDatabase& aDatabase, 
                            RDbs& aDbSession,
                            TInt aDrive )
:	CSICommonDB(aDatabase, aDbSession, aDrive )
{
}

// -----------------------------------------------------------------------------
// CSILexiconDB::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSILexiconDB::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CSILexiconDB::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSILexiconDB* CSILexiconDB::NewL( RDbNamedDatabase& aDatabase, 
                                  RDbs& aDbSession,
                                  TInt aDrive )
    {
    RUBY_DEBUG_BLOCK( "CSILexiconDB::NewL" );
    CSILexiconDB* self 
        = new( ELeave ) CSILexiconDB( aDatabase, aDbSession, aDrive );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );	
    return self;
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::~CSILexiconDB
// Destructor
// -----------------------------------------------------------------------------
//
CSILexiconDB::~CSILexiconDB()
    {
    // Delete all elements of the array before deleting the array
    //	iLexiconArray.ResetAndDestroy();
    //	iLexiconArray.Close();
    RUBY_DEBUG0( "CSILexiconDB::~CSILexiconDB" );
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::CreateLexiconL
// Creates a new lexicon table in the database.
// -----------------------------------------------------------------------------
//
TSILexiconID CSILexiconDB::CreateLexiconL( TUid aClientUid )
    {
    RUBY_DEBUG_BLOCK( "CSILexiconDB::CreateLexiconL" );
    
    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, sizeof( CSILexicon ) ) );
    
    // Generate a new Lexicon ID
    TSILexiconID lexiconID = STATIC_CAST(TSILexiconID,CreateNewIDL(KLexiconIdTable, KLexiconIdColumn, aClientUid ));
    
    // Construct the table name using the new lexicon ID
    TBuf<40> KLexiconName( KSILexiconTable );
    KLexiconName.AppendNumUC( lexiconID );
    RUBY_DEBUG1( "CSILexiconDB::CreateLexiconL lexicon ID: %i", lexiconID );
	
    
    // Create a table definition
    CDbColSet* columns = CDbColSet::NewLC();
    
    // add the columns  
    // 1) Binary data, 2) binary data size
    columns->AddL( TDbCol( KBinaryLexiconColumn, EDbColLongBinary ) );
    columns->AddL( TDbCol( KBinaryLexiconColumnSize, EDbColUint32 ) );
    
    // Create a table
    TInt err =iDb.CreateTable( KLexiconName, *columns );
    
    if ( err != KErrNone )
        {
        // Failed to create the table.
        // Need to release the new Lexicon ID and leave.
        ReleaseIdL( KLexiconIdTable, KLexiconIdColumn, lexiconID );
        User::Leave( err );
        }
    
    // cleanup the column set
    CleanupStack::PopAndDestroy( columns );
    // Construct the table name using the provided grammar ID
    // Declare a literal string to hold the SQL statement
    // SELECT KBinaryLexiconColumn , KBinaryLexiconColumn FROM  KLexiconName
    _LIT(KSQLSelect1,"select  ");	
    _LIT(KSQLSelect2," from  ");		
    TBuf<120> KSQLStatement;		
    KSQLStatement.Append(KSQLSelect1  );	
    KSQLStatement.Append(KBinaryLexiconColumn );
    KSQLStatement.Append(KNext);
    KSQLStatement.Append(KBinaryLexiconColumnSize );	
    KSQLStatement.Append(KSQLSelect2  );
    KSQLStatement.Append(KLexiconName); 
    
    // create a view on the database
    RDbView view;
    User::LeaveIfError(view.Prepare(iDb,TDbQuery(KSQLStatement,EDbCompareNormal)));
    User::LeaveIfError(view.EvaluateAll());
    
    // Get the structure of rowset
    CDbColSet* colSet = view.ColSetL(); 
    view.InsertL();
    view.PutL();
    
    
    // close the view
    view.Close();
    delete colSet; 
    
    // add an empty lexicon
    CSILexicon *aLexicon=CSILexicon::NewL(lexiconID);	
    CleanupStack::PushL(aLexicon);
    UpdateLexiconL(aClientUid,aLexicon);
    CleanupStack::PopAndDestroy(aLexicon);	
    
    iDbSession.FreeReservedSpace( iDrive );
    
    return lexiconID;
    }


// -----------------------------------------------------------------------------
// CSILexiconDB::UpdateLexiconL
// Inserts the externalized SI Lexicon into the specified grammar table.
// Save the Lexicon into the database
// -----------------------------------------------------------------------------
//
void CSILexiconDB::UpdateLexiconL( TUid aClientUid, 
								   CSILexicon *aSILexicon )
    {
    
    //	__UHEAP_MARK;
    TSILexiconID aLexiconID=aSILexicon->LexiconID();
    VerifyOwnershipL(aClientUid, KLexiconIdTable, KLexiconIndex, aLexiconID);
    
    // Construct the table name using the provided grammar ID
    // Declare a literal string to hold the SQL statement
    // SELECT KBinaryLexiconColumn , KBinaryLexiconColumn FROM  KLexiconName
    TBuf<40> KLexiconName(KSILexiconTable);
    KLexiconName.AppendNumUC(aLexiconID);	
    _LIT(KSQLSelect1,"select  ");	
    _LIT(KSQLSelect2," from  ");		
    TBuf<120> KSQLStatement;		
    KSQLStatement.Append(KSQLSelect1  );	
    KSQLStatement.Append(KBinaryLexiconColumn );
    KSQLStatement.Append(KNext);
    KSQLStatement.Append(KBinaryLexiconColumnSize );	
    KSQLStatement.Append(KSQLSelect2  );
    KSQLStatement.Append(KLexiconName); 
    
    // create a view on the database
    RDbView view;
    User::LeaveIfError(view.Prepare(iDb,TDbQuery(KSQLStatement,EDbCompareNormal)));
    User::LeaveIfError(view.EvaluateAll());
    
    // Get the structure of rowset
    CDbColSet* colSet = view.ColSetL(); 
    
    view.FirstL(); 
    view.UpdateL();
    
    
    // Externalize grammar
    CBufFlat* dataCopyBuffer = CBufFlat::NewL( 100 ); // 100 = expand 100 bytes
    CleanupStack::PushL(dataCopyBuffer);              // when the buffer is full
    //TPtr8 Buft((TUint8*) abuf,3,3);
    //dataCopyBuffer->InsertL(0,Buft);
    RBufWriteStream stream;
    stream.Open(*dataCopyBuffer);
    CleanupClosePushL(stream);
    aSILexicon->ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );
    
    TPtr8 aWriteBuf(dataCopyBuffer->Ptr(0));
    TInt BufSize =aWriteBuf.Size();
    
    // add binary buffer by Using the stream  
    RDbColWriteStream out;
    TDbColNo col = colSet->ColNo(KBinaryLexiconColumn); // Ordinal position of long column	
    out.OpenLC(view, col);
    out.WriteL(aWriteBuf);	
    out.Close();	
    CleanupStack::PopAndDestroy(); // out
    // add  size of the buffer
    col = colSet->ColNo(KBinaryLexiconColumnSize); // Ordinal position of  size	
    view.SetColL( col,BufSize); 
    
    view.PutL();
    
    // close the view
    view.Close();
    
    
    delete colSet; 
    CleanupStack::PopAndDestroy( dataCopyBuffer ); 
    //	__UHEAP_MARKEND;
    }

// -----------------------------------------------------------------------------
// CSIGrammarDB::GetNewID
// Get a new uniq ID 
// -----------------------------------------------------------------------------
TInt CSILexiconDB::GetNewID( RArray<TSIRuleID>& aMyIds ) 
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
        // Find a unique Rulevariant ID
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
// CSILexiconDB::CreateIDTableL
// Creates a new lexicon ID table in the database.
// -----------------------------------------------------------------------------
//
void CSILexiconDB::CreateIDTableL()
    {
    // Invoke function in the base class CSICommonDB.
    CSICommonDB::CreateIDTableL(KLexiconIdTable, KLexiconIdColumn, KLexiconIndex);
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::GetAllClientLexiconIDsL
// This function returns all Lexicon IDs owned by the specified client.
// -----------------------------------------------------------------------------
//
void CSILexiconDB::GetAllClientLexiconIDsL( TUid aClientUid,
										    RArray<TSILexiconID>& aLexiconIDs )
    {
    //	GetAllClientIDsL(KLexiconIdTable, KLexiconIdColumn, aClientUid, aLexiconIDs);
    RArray<TUint32> ix;
    ix.Reset();
    GetAllClientIDsL(KLexiconIdTable, KLexiconIdColumn, aClientUid, ix);
    for(TInt i=0;i<ix.Count();i++) 
        aLexiconIDs.Append(STATIC_CAST(TSILexiconID,ix[i]));
    ix.Close();
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::GetAllLexiconIDsL
// This function returns all Lexicon IDs in the database.
// -----------------------------------------------------------------------------
//
void CSILexiconDB::GetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs )
    {
    // This is a hack to get the id aligned to 4-byte boundary,
    // for some reason this does not happen in winscw build if 
    // TSILexiconID is taken from stack.
    TSILexiconID* id = new (ELeave) TSILexiconID;
    CleanupStack::PushL( id );
    
    //	GetAllIDsL(KLexiconIdTable, KLexiconIdColumn, aLexiconIDs);
    RArray<TUint32> ix;
    ix.Reset();
    GetAllIDsL(KLexiconIdTable, KLexiconIdColumn, ix);
    for(TInt i=0;i<ix.Count();i++) 
        {
        *id = STATIC_CAST( TSILexiconID, ix[i] );
        aLexiconIDs.Append( *id );
        }
    ix.Close();
    CleanupStack::PopAndDestroy( id );
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::GetAllPronunciationIDsL
// This function returns all Pronunciation IDs within the specified Lexicon.
// Lexicon have to be loaded 
// -----------------------------------------------------------------------------
//
void CSILexiconDB::GetAllPronunciationIDsL( TSILexiconID aLexiconID, RArray<TSIPronunciationID>& aPronunciationIDs )
    {
	CleanupClosePushL( aPronunciationIDs ); 
	
    // Construct the table name using the provided Lexicon ID
    TBuf<40> KLexiconName(KSILexiconTable);
    KLexiconName.AppendNumUC(aLexiconID);
    
    CSILexicon* newLexicon = LexiconL( aLexiconID) ;// Load the Lexicon from database	
    CleanupStack::PushL(newLexicon);
    
    TSIPronunciationID	aPronunciationID;
    for (TInt  i=0;i<newLexicon->Count();i++) {
        CSIPronunciation* aPronunciation=&(newLexicon->AtL(i));
        aPronunciationID=aPronunciation->PronunciationID();		
        User::LeaveIfError(aPronunciationIDs.Append(aPronunciationID));
        } 
    
    CleanupStack::PopAndDestroy(newLexicon); // newLexicon
    //	GetAllIDsL(KLexiconName, KPronunciationIDColumn, aPronunciationIDs);
    CleanupStack::Pop(); 
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::RemoveLexiconL
// Deletes a Lexicon table from the database.
// -----------------------------------------------------------------------------
//
void CSILexiconDB::RemoveLexiconL( TUid aClientUid,
								   TSILexiconID aLexiconID )
    {
    
    VerifyOwnershipL(aClientUid, KLexiconIdTable, KLexiconIndex, aLexiconID);
    
    
    /* Some times removing of corrupted lexicon fails because of this:
    TInt diskSpace = ( PronunciationCountL(aLexiconID) * sizeof( CSIPronunciation ) )
        + sizeof( CSILexicon );
    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, diskSpace ) );
    */
    
    // Construct the table name using the provided Lexicon ID
    TBuf<40> KLexiconName(KSILexiconTable);
    KLexiconName.AppendNumUC(aLexiconID);
    
    TBuf<50> KSQLStatement;
    // Declare a literal string to hold the SQL statement
    // DROP TABLE KLexiconName
    _LIT(KSQLDelete1, "DROP TABLE ");
    
    KSQLStatement = KSQLDelete1;
    KSQLStatement.Append(KLexiconName);
    
    User::LeaveIfError(iDb.Execute(KSQLStatement));
    
    // Release the Lexicon ID
    ReleaseIdL(KLexiconIdTable, KLexiconIdColumn, aLexiconID);
    
    // Cancel free disk space request
    //iDbSession.FreeReservedSpace( iDrive );
    
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::PronunciationCountL
// Returns the number of Pronunciations in the specified Lexicon.
// -----------------------------------------------------------------------------
TInt CSILexiconDB::PronunciationCountL( TSILexiconID aLexiconID )
    {
    
    CSILexicon* newLexicon = LexiconL( aLexiconID) ;	
    CleanupStack::PushL(newLexicon);
    
    TInt PronunciationCount=newLexicon->Count();	 
    CleanupStack::PopAndDestroy(newLexicon); 	
    return PronunciationCount;

    }

// -----------------------------------------------------------------------------
// CSILexiconDB::IsPronunciationValidL
// Checks if the specified pronunciation exists in the specified loaded	.
// 
// -----------------------------------------------------------------------------
//
TBool CSILexiconDB::IsPronunciationValidL( TSILexiconID aLexiconID,
                                           TSIPronunciationID aPronunciationID )
    {
    CSILexicon* newLexicon = LexiconL( aLexiconID) ;		 
    CleanupStack::PushL(newLexicon);
    
    if (newLexicon->Find(aPronunciationID)==KErrNotFound) {
        CleanupStack::PopAndDestroy(newLexicon); // newLexicon
        return EFalse; 
        }
    else {
        CleanupStack::PopAndDestroy(newLexicon); // newLexicon
        return ETrue;	
        }
    }


// -----------------------------------------------------------------------------
// CSILexiconDB::ResetAndDestroy
// Deallocates the temporary memory containing the Lexicon object created with
// AllPronunciationsL.
// -----------------------------------------------------------------------------
//
void CSILexiconDB::ResetAndDestroy()
    {
    //	iLexiconArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::RemovePronunciationL
// Deletes Pronunciation from the database.
// -----------------------------------------------------------------------------
//
void CSILexiconDB::RemovePronunciationL(  TUid aClientUid, 
                                        TSILexiconID aLexiconID,
                                        TSIPronunciationID aPronunciationID
                                        )
    {
    // Check the ownership first
    VerifyOwnershipL(aClientUid, KLexiconIdTable, KLexiconIndex, aLexiconID);
    
    CSILexicon* newLexicon = LexiconL( aLexiconID);		 
    CleanupStack::PushL(newLexicon);
    
    if (newLexicon->Find(aPronunciationID)==KErrNotFound)
        User::Leave(KErrNotFound);				 
    else  
        newLexicon->DeleteL(aPronunciationID);
    UpdateLexiconL(aClientUid  , newLexicon);
    CleanupStack::PopAndDestroy(newLexicon); // newLexicon
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::AddPronunciationL
// Add Pronunciation to the database.
// -----------------------------------------------------------------------------
//
TSIPronunciationID CSILexiconDB::AddPronunciationL( TUid aClientUid, 
                                                   TSILexiconID aLexiconID,
                                                   TDesC8& aPronunciationPr,
                                                   TSIModelBankID aModelBankID)
    {
    // Check the ownership first
    VerifyOwnershipL( aClientUid, KLexiconIdTable, KLexiconIndex, aLexiconID );
    
    CSILexicon* aLexicon = LexiconL( aLexiconID );
    CleanupStack::PushL( aLexicon );
    
    // existing myPronunciationID
    RArray<TSIPronunciationID> myPronunciationID;
    myPronunciationID.Reset();
    for( TInt i = 0; i < aLexicon->Count(); i++ )
        {
        CSIPronunciation* aPronunciation = &( aLexicon->AtL( i ) );
        myPronunciationID.Append( aPronunciation->PronunciationID() );
        }
    
    // Find a uniq new id 
    TSIPronunciationID aPronunciationID = GetNewID( myPronunciationID );
    myPronunciationID.Close();
    
    // add the  phoneme sequence  to the Pronunciation
    CSIPronunciation* pronunciation = CSIPronunciation::NewL( aPronunciationID, aModelBankID );
    CleanupStack::PushL( pronunciation );	
    pronunciation->SetPhonemeSequenceL( aPronunciationPr );
    aLexicon->AddL( pronunciation );
    CleanupStack::Pop( pronunciation ); // aPronunciation
    UpdateLexiconL( aClientUid,aLexicon );
    CleanupStack::PopAndDestroy( aLexicon ); // aLexicon
    return aPronunciationID;
    }

// -----------------------------------------------------------------------------
// CSILexiconDB::LexiconL
// Loads all pronunciations within the specified lexicon into a lexicon object
// -----------------------------------------------------------------------------
//
CSILexicon* CSILexiconDB::LexiconL( TSILexiconID aLexiconID )
    {
    
    // Construct the table name using the provided grammar ID
    // Construct the table name using the provided grammar ID
    // Declare a literal string to hold the SQL statement
    // SELECT KBinaryLexiconColumn , KBinaryLexiconColumn FROM  KLexiconName
    TBuf<40> KLexiconName(KSILexiconTable);
    KLexiconName.AppendNumUC(aLexiconID);
    // Create newLexicon object	
    CSILexicon* lexicon = CSILexicon::NewLC( aLexiconID );	
    _LIT(KSQLSelect1,"select  ");	
    _LIT(KSQLSelect2," from  ");
    TBuf<120> KSQLStatement;		
    KSQLStatement.Append(KSQLSelect1  );		
    KSQLStatement.Append(KBinaryLexiconColumn );
    KSQLStatement.Append(KNext);
    KSQLStatement.Append(KBinaryLexiconColumnSize );
    KSQLStatement.Append(KSQLSelect2  );
    KSQLStatement.Append(KLexiconName); 
    
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
        TDbColNo col = colSet->ColNo(KBinaryLexiconColumnSize); // Ordinal position of long column 
        TInt32 size = view.ColUint32( col);
        
        TUint8* buf = new (ELeave) TUint8[size];		
        CleanupStack::PushL( buf);			
        TPtr8 readBuf( buf, size, size);
        CBufFlat* dataCopyBuffer = CBufFlat::NewL( 100 ); // 100 = expand 100 bytes
        CleanupStack::PushL( dataCopyBuffer );              // when the buffer is full
        
        RBufReadStream stream;
        CleanupClosePushL( stream );
        stream.Open(*dataCopyBuffer);
        
        // and a stream for long columns
        RDbColReadStream in;
        CleanupClosePushL( in );
        col = colSet->ColNo(KBinaryLexiconColumn); // Ordinal position of long column 
        
        
        in.OpenLC(view, col);
        in.ReadL(readBuf, view.ColLength(col));
        dataCopyBuffer->InsertL(0,readBuf);
        lexicon->InternalizeL( stream );
        
        CleanupStack::PopAndDestroy( col );
        CleanupStack::PopAndDestroy( &in );
        CleanupStack::PopAndDestroy( &stream ); 
        CleanupStack::PopAndDestroy( dataCopyBuffer );
        CleanupStack::PopAndDestroy( buf );
        }
    
    CleanupStack::PopAndDestroy( colSet );
    CleanupStack::PopAndDestroy( &view );
    

 
    // Cleanup lexicon
    CleanupStack::Pop( lexicon );
    return lexicon;
    }

//  End of File
