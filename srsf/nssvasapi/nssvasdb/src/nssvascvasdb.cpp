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
* Description:  Implementation of VAS database handling
*
*/


#include "nssvascvasdb.h"
#include <e32test.h>
#include <e32std.h>
#include <e32svr.h>
#include "rubydebug.h"
#include "nssvascoreconstant.h"
#include "nssvascrrd.h"

#include "nssvasbackupobserver.h"

// CONSTANTS

// Name of the DB lock mutex
_LIT( KLockMutex, "VASDATABASE" );

#ifdef _DEBUG
// used in UDEB macros only
_LIT( KVasDbPanic, "VasCNssVasDb.cpp");
#endif // _UDEB

// Maximum size of tag array
static const TInt KMaxTagArraySize = 50;

const TInt KContextListGranularity = 5;

const TInt KSqlStatementmaxLength = 120;

// MACROS

// Complementary macro for TRAPD
#define REACT( err, codeblock ) if ( err != KErrNone ) { codeblock; }

// ============================ Methods ===============================

// -----------------------------------------------------------------------------
// CNssVasDb::NewL
// 2-level constructor
// -----------------------------------------------------------------------------
//
CNssVasDb* CNssVasDb::NewL( CNssContextBuilder& contextBuilder,
	                        CNssSpeechItemBuilder& speechItemBuilder )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::NewL" );

    CNssVasDb* self = new(ELeave) CNssVasDb( contextBuilder,
                                             speechItemBuilder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::~CNssVasDb
// Standard C++ destructor
// -----------------------------------------------------------------------------
//
CNssVasDb::~CNssVasDb()
    {
    RUBY_DEBUG0( "CNssVasDb::~CNssVasDb" );

    if ( iLocked )
        {
        RUBY_DEBUG0( "Signalling mutex in CNssVasDb::~CNssVasDb" );
        iMutex.Signal();
        }
    else
        {
        RUBY_DEBUG0( "NOT signalling mutex in CNssVasDb::~CNssVasDb iLocked = EFalse" );
        }

    if ( iClientHasOpenedDatabase )
        {
        CloseDatabase();
        }

    iMutex.Close();
    delete iBackupObserver;
    iCriticalSection.Close();
    RUBY_DEBUG0( "CNssVasDb::~CNssVasDb Mutex handle closed" );
    }
	

// -----------------------------------------------------------------------------
// CNssVasDb::CNssVasDb
// Standard C++ contructor
// -----------------------------------------------------------------------------
//
CNssVasDb::CNssVasDb( CNssContextBuilder& aContextBuilder,
	                  CNssSpeechItemBuilder& aSpeechItemBuilder )
: iClientHasOpenedDatabase( EFalse ),
  iLocked( EFalse ),
  iContextBuilder( aContextBuilder ),
  iSpeechItemBuilder( aSpeechItemBuilder )
    {
    // empty
    }

// -----------------------------------------------------------------------------
// CNssVasDb::ConstructL
// Symbian second constructor.
// -----------------------------------------------------------------------------
//
void CNssVasDb::ConstructL()
    {
    TInt err = iMutex.OpenGlobal( KLockMutex );
    if ( err != KErrNone )
        {
        RUBY_DEBUG0( "CNssVasDb::ConstructL Creating new global mutex" );
        iMutex.CreateGlobal( KLockMutex );
        }
    else
        {
        RUBY_DEBUG0( "CNssVasDb::ConstructL Using existing global mutex" );
        }
    iCriticalSection.CreateLocal();
    iBackupObserver = CNssVasBackupObserver::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::RollbackCleanupFunction
// This function is used to form iRollbackCleanupItem. When this item is
// popped from the cleanup stack, it makes a rollback on VAS DB.
// -----------------------------------------------------------------------------
//
void CNssVasDb::RollbackCleanupFunction( TAny* aArg )
    {
    RUBY_DEBUG0( "CNssVasDb::RollbackCleanupFunction" );

    CNssVasDb* me = (CNssVasDb*)aArg;

    me->RollbackTransaction();
    }

// -----------------------------------------------------------------------------
// CNssVasDb::CreateDatabaseL
// Creates a new database.
// from a resource file.
// -----------------------------------------------------------------------------
//
void CNssVasDb::CreateDatabaseL()
    {
    User::LeaveIfError( iDbSession.Connect() );
    CleanupClosePushL( iDbSession );
    iDbcreator.CreateVasDatabaseL( iDbSession );
    CleanupStack::PopAndDestroy( &iDbSession );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::OpenDatabaseL
// Opens the database. Reads the path and file name of the database
// from a resource file.
// -----------------------------------------------------------------------------
//
void CNssVasDb::OpenDatabaseL()
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::OpenDatabaseL" );

    if ( !iClientHasOpenedDatabase )
        {
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL ( fs ); // Stack: file session

        // Open the database
	    User::LeaveIfError( iDbSession.Connect() );
	    
        User::LeaveIfError( iDatabase.Open( iDbSession, 
                            KVasDatabaseName, KVasDatabaseFormatString ) );
        User::LeaveIfError( fs.CharToDrive( KVasDatabaseDrive, iDrive ) );

        if ( iDatabase.IsDamaged() )
            {
            RUBY_DEBUG0( "CNssVasDb::OpenDatabaseL Recovering" );
            User::LeaveIfError( iDatabase.Recover() );
            }
            
        CleanupStack::PopAndDestroy( &fs ); 

	    iClientHasOpenedDatabase = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CNssVasDb::CloseDatabase
// Closes the database.
// -----------------------------------------------------------------------------
//
TInt CNssVasDb::CloseDatabase()
    {
    RUBY_DEBUG0( "CNssVasDb::CloseDatabase" ); 
  
    iDatabase.Close();
    iDbSession.Close();
    iClientHasOpenedDatabase = EFalse;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetModelBankIdLexiconIdL
// Returns the model bank ID and the lexicon ID.
// -----------------------------------------------------------------------------
//
TBool CNssVasDb::GetModelBankIdLexiconIdL( TUint32& aModelBankId, TUint32& aLexiconId )
    {
    TBool ret = EFalse;
    RDbView view;
  
    _LIT( KSQLGetModelBankLexicon, "select modelbankid,lexiconid from contexttable" );

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( 
                            KSQLGetModelBankLexicon, EDbCompareNormal ) ) );
    User::LeaveIfError( view.EvaluateAll() );

    // There is no ModelbankId or LexiconId 
    if ( view.IsEmptyL() )
        {
        User::Leave( KErrNotFound );
        }
    // There is a ModelBankId and a LexiconId 
    ret = ETrue;
    view.FirstL();
    view.GetL();

    // Get column set
    CDbColSet* columns = view.ColSetL();
    // Get column ordinals
    TDbColNo modelbankid_col = columns->ColNo( KModelBankIdCol );
    TDbColNo lexiconid_col = columns->ColNo( KLexiconIdCol );
    // Cleanup column set
    delete columns;

    aModelBankId = view.ColUint32( modelbankid_col );
    aLexiconId   = view.ColUint32( lexiconid_col );
 
    // Close view
    CleanupStack::PopAndDestroy( &view );

    // Release lock
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );

    return ret;
    }


// -----------------------------------------------------------------------------
// CNssVasDb::TagExistsL
// Checks if a context is empty.
// -----------------------------------------------------------------------------
//
TBool CNssVasDb::TagExistL( TInt aContextId )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::TagExistL" );
 
    TBool   ret( EFalse );
    RDbView view;

    CleanupClosePushL(view);

    _LIT( KSQLGetTags, "select * from tagtable " );
    _LIT( KSQLWhereContextId, "where contextid=" );

    iSQLStatement = KSQLGetTags;
    iSQLStatement.Append( KSQLWhereContextId );
    iSQLStatement.AppendNumUC( aContextId );
    
    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
    User::LeaveIfError( view.EvaluateAll() );

    if ( view.IsEmptyL() )
        {
        ret = EFalse;
        }
    else
        {
        ret = ETrue;
        }

    // Close view
    CleanupStack::PopAndDestroy( &view );

    return ret;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::TagCountL
// Returns the number of tags.
// -----------------------------------------------------------------------------
//
TInt CNssVasDb::TagCountL( TInt aContextId )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::TagCountL" );
        
    RDbView view;
    TInt    numberOfTags = 0;

    CleanupClosePushL( view );

    _LIT( KSQLGetTags, "select * from tagtable " );
    _LIT( KSQLWhereContextId, "where contextid=" );

    iSQLStatement = KSQLGetTags;
    iSQLStatement.Append( KSQLWhereContextId );
    iSQLStatement.AppendNumUC( aContextId );
    
    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
    User::LeaveIfError( view.EvaluateAll() );

    numberOfTags = view.CountL();
   
    view.Close();
    CleanupStack::PopAndDestroy( &view );
   
    return numberOfTags;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::CommitIfSuccess
// Commits or rolls back accoring to success parameter.
// -----------------------------------------------------------------------------
//
void CNssVasDb::CommitIfSuccess( RDbDatabase /*aDatabase*/, TInt aSuccess, TBool aCompactIfCommit )
    {
    if ( aSuccess >= 0 )
        {
        CommitTransaction( aCompactIfCommit );
        }
    else
        {
        // Otherwise, roll back the transaction and release the lock.
        RollbackTransaction();
        }
    }

// -----------------------------------------------------------------------------
// CNssVasDb::SaveContextL
// Saves a newly created context. The Context ID is assigned and returned
// using the reference parameter.
// -----------------------------------------------------------------------------
//
void CNssVasDb::SaveContextL( CNssContext& aContext, TInt& aContextId )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::SaveContextL" );
  
    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }

    // Locks the database for the use of this process only
    TInt error = StartTransaction();

    if ( error != KErrNone )
        {
        RUBY_DEBUG0( "CNssVasDb error: could not lock database." );
        
        User::Leave( KErrGeneral );
        }

    TRAP( error, SaveContextL( &aContext, aContextId ) );
    REACT( error, RUBY_DEBUG0( "Context saving failed" ) );

    if ( error == KErrNone )
        {
        TRAP ( error, DoUpdateContextClientDataL( aContext ) );
        }
    
    CommitIfSuccess( iDatabase, error, ETrue );
    
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UpdateContextL
// Updates the context to VAS DB.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateContextL( CNssContext& aContext )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::UpdateContextL" );

    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }

    TInt error = StartTransaction();

    if ( error != KErrNone )
        {
        RUBY_DEBUG0( "CNssVasDb error: could not lock database." );
        
        User::Leave( KErrGeneral );
        }

    TRAP( error, DoUpdateContextL( aContext ) );
    REACT( error, RUBY_DEBUG0( "Context updating failed" ) );

    CommitIfSuccess( iDatabase, error, ETrue );
    
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UpdateContextInsideTransactionL
// Updates the context to VAS DB.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateContextInsideTransactionL( CNssContext& aContext )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::UpdateContextInsideTransactionL" );

    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }

    DoUpdateContextL( aContext );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UpdateContextClientDataL
// Updates the client data of the context.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateContextClientDataL( CNssContext& aContext )   
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::UpdateContextClientDataL" );
	
    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }

    TInt error = StartTransaction();

    if ( error != KErrNone )
        {
        RUBY_DEBUG0( "CNssVasDb error: could not lock database." );
        
        User::Leave( KErrGeneral );
        }

    TRAP( error, DoUpdateContextClientDataL( aContext ) );

    CommitIfSuccess( iDatabase, error, ETrue );
    
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetDefaultModelBankIdL
// Usually, all contexts use the same Model Bank. This function
// returns the first model bank ID it finds.
// -----------------------------------------------------------------------------
//
void CNssVasDb::GetDefaultModelBankIdL( TUint32& aId )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::GetDefaultModelBankIdL" );
      
    _LIT( KSQLGetAll,"select modelbankid from contexttable");

    iSQLStatement = KSQLGetAll;

    if ( !iClientHasOpenedDatabase )
        {
        User::Leave( KErrDbNotSet );
        }

    StartTransaction();
    CreatePushRollbackItemLC();

    RDbView view;

    CleanupClosePushL(view);

    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
    User::LeaveIfError( view.EvaluateAll() );

    if ( view.IsEmptyL() )
        {
	    User::Leave( KErrNotFound );
        }

    // Get column set
    CDbColSet* columns = view.ColSetL();
    // Get model bank id column ordinal
	TDbColNo modelbankid_col= columns->ColNo( KModelBankIdCol );
	delete columns;

	view.FirstL();
    view.GetL(); // Retrieve the current row - actually reads the row from the database.

    aId = view.ColUint32( modelbankid_col );

    CleanupStack::PopAndDestroy( &view );// close view

    // Rollback cleanup
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::ResetModelBankL
// Usually, all contexts use the same Model Bank. When the model bank is reseted,
// (=speaker independent models are resotred and speaker adaptation is destroyed)
// the model bank ID changes for all contexts.
// -----------------------------------------------------------------------------
//
void CNssVasDb::ResetModelBankL( TUint32 aNewId )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::ResetModelBankL" );

    _LIT( KSQLUpdateModelbank,"update contexttable set modelbankid=");

    iSQLStatement = KSQLUpdateModelbank;
    iSQLStatement.AppendNumUC( aNewId );


    if ( !iClientHasOpenedDatabase )
        {
        User::Leave( KErrDbNotSet );
        }

    StartTransaction();
    CreatePushRollbackItemLC();

    User::LeaveIfError( iDatabase.Execute( iSQLStatement ) );

    CleanupStack::Pop();  // Rollback cleanup item

    User::LeaveIfError( CommitTransaction( ETrue ) );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetContextByNameL
// Reads a context from VAS DB by name.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssContext>* CNssVasDb::GetContextByNameL(const TDesC& aName)
    {
	RUBY_DEBUG_BLOCK("CNssVasDb::GetContextByNameL");

    _LIT( KSQLGetAll,"select * from contexttable ");
    _LIT( KSQLWhereName,"where name='");
    _LIT( KTick,"' ");

    iSQLStatement = KSQLGetAll;
    iSQLStatement.Append( KSQLWhereName );
    iSQLStatement.Append( aName );
    iSQLStatement.Append( KTick );

    return GetContextL( iSQLStatement );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetGlobalContexts
// Reads all global contexts from VAS DB.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssContext>* CNssVasDb::GetGlobalContexts()
    {
	RUBY_DEBUG0("CNssVasDb::GetGlobalContexts");
	    
    _LIT( KSQLGetAll,"select * from contexttable ");
    _LIT( KSQLWhereGlobal,"where global=1");

    iSQLStatement = KSQLGetAll;
    iSQLStatement.Append( KSQLWhereGlobal );

    CArrayPtrFlat<CNssContext>* ret( NULL );
    TRAPD( error, ret = GetContextL( iSQLStatement ) );
    if ( error != KErrNone )
        {
        return NULL;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetAllContexts
// Reads all contexts from VAS DB.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssContext>* CNssVasDb::GetAllContexts()
    {   
    RUBY_DEBUG0("CNssVasDb::GetAllContexts");
    
    _LIT( KSQLGetAll,"select * from contexttable ");

    iSQLStatement = KSQLGetAll;

    CArrayPtrFlat<CNssContext>* ret( NULL );
    TRAPD( error, ret = GetContextL( iSQLStatement ) );
    if ( error != KErrNone )
        {
        return NULL;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CNssVasDb::GetContextL
// Executes the SQL query. After the query has selected a list of contexts,
// these contexts are read from DB to memory.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssContext>* CNssVasDb::GetContextL(const TDesC& aSQLStatement)
    {
    RUBY_DEBUG_BLOCK("CNssVasDb::GetContextL");

    __ASSERT_DEBUG( aSQLStatement.Left( 6 ).Compare( _L("select") ) == 0,
                    User::Panic( KVasDbPanic, __LINE__ ) );

    if ( !iClientHasOpenedDatabase )
        {
        User::Leave( KErrDbNotSet );
        }

    TInt ret = StartTransaction();
    RUBY_DEBUG1("Started transaction. Error code [%d]", ret);
    
    CreatePushRollbackItemLC();
    CArrayPtrFlat<CNssContext>* contextList = GetContextInsideTransactionL( aSQLStatement );

    // Rollback cleanup (just to avoid automatic locking)
    CleanupStack::Pop();  // Rollback cleanup item 
    CommitTransaction( EFalse );

    return contextList;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetContextInsideTransaction
// Reads a context by name. Assumes that a transaction has already been started.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssContext>* CNssVasDb::GetContextInsideTransactionL( const TDesC& aSQLStatement )
    {
    RUBY_DEBUG_BLOCK("CNssVasDb::GetContextInsideTransactionL");
    RDbView view;

    CleanupClosePushL(view);

    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( aSQLStatement, EDbCompareNormal ) ) );
    RUBY_DEBUG0("CNssVasDb::GetContextInsideTransactionL Prepared query");
    User::LeaveIfError( view.EvaluateAll() );
    RUBY_DEBUG0("CNssVasDb::GetContextInsideTransactionL Evaluated query");

    if ( view.IsEmptyL() )
        {
        RUBY_DEBUG0("CNssVasDb::GetContextInsideTransactionL Empty view. Leaving with KErrNotFound");
	    User::Leave( KErrNotFound );
        }

    CArrayPtrFlat<CNssContext>* contextList = 
            new (ELeave) CArrayPtrFlat<CNssContext>( KContextListGranularity );
    CleanupStack::PushL( contextList );
    CleanupResetAndDestroyPushL( *contextList );
    RUBY_DEBUG0("CNssVasDb::GetContextInsideTransactionL Before FillContextListArrayL");

    FillContextListArrayL(view, *contextList);
   
    CleanupStack::Pop( contextList ); // ResetAndDestroy
    CleanupStack::Pop( contextList );  // array itself
    CleanupStack::PopAndDestroy( &view );// close view

    return( contextList );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetTagReferenceList
// Fetches tag references for all tags in a context.
// -----------------------------------------------------------------------------
//
TNssTagReferenceListArray* CNssVasDb::GetTagReferenceListL( TInt aContextId )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::GetTagReferenceListL" );
    
    RDbView view;
    TNssTagReference tagRef;

    TNssTagReferenceListArray* tagArray = 
            new (ELeave) TNssTagReferenceListArray( KMaxTagArraySize );
    CleanupStack::PushL( tagArray );

    if ( !iClientHasOpenedDatabase )
        {
        User::Leave( KErrDbNotSet );
        }

    // Start transaction - lock database
    StartTransaction();
    CreatePushRollbackItemLC();

    // Make a query, which lists all tags in the context.
    // Making a CNssTagReferece only requires tagid and name.
    _LIT( KSQLGetTag, "select tagid,name from tagtable where contextid=" );
    iSQLStatement = KSQLGetTag;
    iSQLStatement.AppendNumUC( aContextId );

    CleanupClosePushL(view);

    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
    User::LeaveIfError( view.EvaluateAll() );

    if ( view.IsEmptyL() )
        {
	    User::Leave( KErrNotFound );
        }

    // Get column set
    CDbColSet* columns = view.ColSetL();
    // Get column ordinals
    TDbColNo name_col  = columns->ColNo( KNameCol );
    TDbColNo tagid_col = columns->ColNo( KTagIdCol );

    delete columns;

    for( view.FirstL(); view.AtRow(); view.NextL() )
        {
        view.GetL();
	    tagRef.iTagName = view.ColDes( name_col );
	    tagRef.iTagId   = view.ColUint32( tagid_col );
	    tagArray->AppendL( tagRef );
        }

    // Close view
    CleanupStack::PopAndDestroy( &view );

    // Finish transaction - release lock
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );

    // Return result
    CleanupStack::Pop( tagArray );

    return( tagArray );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::SaveTagL
// Saves a newly created tag. When a tag is saved, a Tag ID is assigned to it.
// This Tag ID is returned in the reference parameter.
// -----------------------------------------------------------------------------
//
void CNssVasDb::SaveTagL( CNssTag& aTag, TInt& aNewId )
    {
    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }

    // Locks the database for the use of this process only	
    
    TInt error = StartTransaction();

    if ( error != KErrNone )
        {
        RUBY_DEBUG0( "CNssVasDb error: could not lock database." );
        
        User::Leave( KErrGeneral );
        }

    TInt contextId; // Discard context ID 
    
    CNssContext* context = static_cast<CNssContext*> ( aTag.Context() );
    TRAP( error, SaveContextL( context, contextId ) );
	
    if ( error == KErrItemAlreadyExist || error == KErrNone )
        {
        TRAP( error, SaveTagL( &aTag, aNewId ) );
        }
    else 
        {
        RollbackTransaction(); 
        User::Leave( error );
        }
    
    // Check SavetagL result
    if ( error != KErrNone )
        {
        RollbackTransaction(); 	
        User::Leave( error );
        }
    else
        {
        CNssRRD* rrd = static_cast<CNssRRD*>( aTag.RRD() );
        rrd->SetTagId( aNewId );

        TRAPD( error, SaveRRDL( rrd ) );
	    REACT( error, RUBY_DEBUG0( "RRD saving failed." ) );

        CommitIfSuccess( iDatabase, error, ETrue );
        }
        
    User::LeaveIfError( error );
    }

// ---------------------------------------------------------
// CNssVasDb::SaveTagsL
// Saves several tags in one commit.
// ---------------------------------------------------------
//
void CNssVasDb::SaveTagsL( CArrayPtrFlat<CNssTag>* aTagArray, RArray<TInt>& aTagIdArray )
    {
	CleanupClosePushL( aTagIdArray );

    TInt k( 0 );
    TInt error( KErrNone );

    if ( !iClientHasOpenedDatabase )
        {
        User::Leave( KErrDbNotSet );
        }

    // Locks the database for the use of this process only	
    User::LeaveIfError( StartTransaction() );

    CreatePushRollbackItemLC(); // Stack: rollback cleanup

    RArray<TInt> contextIds;
    CleanupClosePushL( contextIds ); // Stack: rollback cleanup, contextIds

    for ( k = 0; k < aTagArray->Count(); k++ )
        {
        CNssTag* tag = (*aTagArray)[k];
        CNssContext* context = static_cast<CNssContext*> ( tag->Context() );

        if ( contextIds.Find( context->ContextId() ) == KErrNotFound )
            {
            TInt contextId;
            TRAP( error, SaveContextL( context, contextId ) );

            // We accept leaves with error KErrAlreadyExists
            if ( error != KErrNone && error != KErrAlreadyExists )
                {
                break;
                }

            contextIds.Append( context->ContextId() );
            }
        }

    // Close context id array
    CleanupStack::PopAndDestroy( &contextIds ); // Stack: Rollback cleanup

    // We accept leaves with error KErrAlreadyExists
    if ( error != KErrNone && error != KErrAlreadyExists )
        {
        User::Leave( error );
        }

    TInt id;
    for ( k = 0; k < aTagArray->Count(); k++ )
        {
        CNssTag* tag = (*aTagArray)[k];

        if ( tag->TagId() == KNssVASDbDefaultValue )
            {
            SaveTagL( tag, id );

            CNssRRD* rrd = static_cast<CNssRRD*>( tag->RRD() );
            rrd->SetTagId( id );

	        SaveRRDL( rrd );
            User::LeaveIfError( aTagIdArray.Append( id ) );
            }
        else{
            UpdateTagInTransactionL( *tag );
            User::LeaveIfError( aTagIdArray.Append( tag->TagId() ) );
            }
        }

    User::LeaveIfError( CommitTransaction( ETrue ) );

    // Pop rollback cleanup
    // (which would have made database rollback, if a leave had happened)
    CleanupStack::Pop();  // Rollback cleanup item
	CleanupStack::Pop();
    }

// ---------------------------------------------------------
// CNssVasDb::DeleteTagsL
// Deletes several tags in one commit.
// ---------------------------------------------------------
//
void CNssVasDb::DeleteTagsL( const RArray<TUint32>& aTagIdArray )
    {
    TInt k( 0 );
   
    if ( !iClientHasOpenedDatabase )
        {
        User::Leave( KErrDbNotSet );
        }

    // Locks the database for the use of this process only	
    User::LeaveIfError( StartTransaction() );

    CreatePushRollbackItemLC();

    for ( k = 0; k < aTagIdArray.Count(); k++ )
        {
        DeleteTagInsideTransactionL( aTagIdArray[k] );
        }

    User::LeaveIfError( CommitTransaction( ETrue ) );

    // Pop rollback cleanup
    // (which would have made database rollback, if a leave had happened)
    CleanupStack::Pop();  // Rollback cleanup item
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UpdateTagL
// Updates the data of an existing tag.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateTagL( CNssTag& aTag )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::UpdateTagL" );	

    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }
	
    // Locks the database for the use of this process only
    User::LeaveIfError( StartTransaction() );

    TRAPD( error, UpdateTagInTransactionL( aTag ) );

    CommitIfSuccess( iDatabase, error, ETrue );

    User::LeaveIfError( error );
    }


// -----------------------------------------------------------------------------
// CNssVasDb::UpdateTagRuleIDs
// Changes the Rule IDs of some tags. The Rule ID changes, when
// and already trained tags is retrained.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateTagRuleIDsL( const RArray<TNssSpeechItem>& aRetrainedTags )
    {
    TInt error( KErrNone );
    if ( !iClientHasOpenedDatabase )
        {
        User::Leave( KErrDbNotSet );
        }
   
    _LIT( KSQLUpdateTag,"update tagtable set ");
    _LIT( KEqual,"=");
    _LIT( KSQLWhereTagIdIs," where tagid = ");

    TInt count = aRetrainedTags.Count();

    if ( count > 0 )
        {
        return;
        }

    // Start transaction
    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    // Make the changes to Rule IDs
    for ( TInt k( 0 ); k < count; k++ )
        {
        // "update tagtable set "
        iSQLStatement = KSQLUpdateTag;

        // "ruleid=1234"
        iSQLStatement.Append( KRuleIdCol );
        iSQLStatement.Append( KEqual );
        iSQLStatement.AppendNumUC( aRetrainedTags[k].iRuleId );

        // " where tagid = 5678"
        iSQLStatement.Append( KSQLWhereTagIdIs );
        iSQLStatement.AppendNumUC( aRetrainedTags[k].iTagId );

        error =  iDatabase.Execute( iSQLStatement );
        if ( error < KErrNone )
            {
            ReleaseDiskSpace();
            User::Leave( error );
            }
        }

    // Commit transaction
    error = CommitTransaction( ETrue );
    if ( error )
        {
        ReleaseDiskSpace();
        User::Leave( error );
        }

    // Pop rollback cleanup object
    CleanupStack::Pop();  // Rollback cleanup item

    ReleaseDiskSpace();    
    }


// -----------------------------------------------------------------------------
// CNssVasDb::SaveContextL
// Saves a newly created context. When a context is saved, a Context ID is
// assigned to it. This id is returned using the reference parameter.
// -----------------------------------------------------------------------------
//
void CNssVasDb::SaveContextL( CNssContext* aContext, TInt& aContextId )
    {
    TBuf<KNssVasDbContextName> name;
    RDbView view1;

    _LIT( KSQLAdd, "select * from contexttable " );
    _LIT( KSQLWhereName, "where name='" );
    _LIT( KTick, "' " );

    //Check to see if a context already exist with the new name
    iSQLStatement = KSQLAdd;
    iSQLStatement.Append( KSQLWhereName );
    iSQLStatement.Append( aContext->ContextName() );
    iSQLStatement.Append( KTick );
	
    CleanupClosePushL( view1 );
   
    User::LeaveIfError( view1.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );	
    User::LeaveIfError( view1.EvaluateAll() );

    if ( !view1.IsEmptyL() ) // context with that name already exist so return status
        {  
        // Get column set
        CDbColSet* columns1 = view1.ColSetL();
        TDbColNo contextid_col = columns1->ColNo( KContextIdCol );

	    delete columns1;

        view1.FirstL();
        view1.GetL();
	    //Get the  unique contextid key just generated
	    aContextId = view1.ColUint32(contextid_col);
	    aContext->SetContextId( aContextId );

	    view1.Close();	  
	    CleanupStack::PopAndDestroy( &view1 );
        
        User::Leave( KErrAlreadyExists );
        }
    else{
        view1.Close();
	    CleanupStack::PopAndDestroy( &view1 );
	   
	    RDbView view2;

        CleanupClosePushL( view2 );

	    User::LeaveIfError( view2.Prepare( iDatabase, TDbQuery(KSQLAdd,EDbCompareNormal)));	
        User::LeaveIfError( view2.EvaluateAll() );
  
	    // Get column set
        CDbColSet* columns = view2.ColSetL();
        // Get column ordinals
        TDbColNo name_col = columns->ColNo( KNameCol );
        TDbColNo global_col = columns->ColNo( KGlobalCol );
	    TDbColNo grammarid_col = columns->ColNo( KGrammarIdCol );
	    TDbColNo lexiconid_col = columns->ColNo( KLexiconIdCol );
	    TDbColNo modelbankid_col = columns->ColNo( KModelBankIdCol );
	    TDbColNo contextid_col = columns->ColNo( KContextIdCol );

	    delete columns;

	    // Begin process of inserting a row...
	    view2.InsertL();
	    // set column values...
	    view2.SetColL( name_col, aContext->ContextName() );
	    view2.SetColL( global_col, aContext->IsGlobal() );
	    view2.SetColL( lexiconid_col, aContext->LexiconId() );
	    view2.SetColL( grammarid_col, aContext->GrammarId() );
	    view2.SetColL( modelbankid_col, aContext->ModelBankId() );
	
	    // add the row to the table...
	    view2.PutL();
	             
	    view2.GetL();
        //Get the  unique contextid key just generated
	    aContextId = view2.ColUint32( contextid_col );

	    aContext->SetContextId( aContextId );

	    name = view2.ColDes( name_col );
	  
	    view2.Close();
	    CleanupStack::PopAndDestroy( &view2 );
        }
    }

// -----------------------------------------------------------------------------
// CNssVasDb::SaveTagL
// Saves a newly created tag. When a tag is saved, a Tag ID is assigned to it.
// This id is returned using the reference parameter.
// -----------------------------------------------------------------------------
//
void CNssVasDb::SaveTagL( CNssTag* aTag, TInt& aNewId )
    {
	RDbView view;
 
    _LIT( KSQLGet,"select * from tagtable");
	
	CleanupClosePushL(view);

	User::LeaveIfError( view.Prepare( iDatabase, TDbQuery(KSQLGet,EDbCompareNormal)));	
	User::LeaveIfError( view.EvaluateAll());
    
	// Get column set
    CDbColSet* columns = view.ColSetL();
    // Get column ordinals
    TDbColNo name_col           = columns->ColNo( KNameCol );
	TDbColNo tagid_col          = columns->ColNo( KTagIdCol );
	TDbColNo contextid_col      = columns->ColNo( KContextIdCol );
	TDbColNo ruleid_col         = columns->ColNo( KRuleIdCol );

	delete columns;
	  
	// Begin process of inserting a row...
	view.InsertL();
	// set column values...
	
	CNssContext* context = static_cast<CNssContext*> ( aTag->Context() );
	CNssSpeechItem* speechItem = static_cast<CNssSpeechItem*> ( aTag->SpeechItem() );
	
	#ifdef __SIND_EXTENSIONS
	    view.SetColL( name_col, aTag->SpeechItem()->RawText() ); 
	#else
	    view.SetColL( name_col, aTag->SpeechItem()->Text() ); 
	#endif  // __SIND_EXTENSIONS
	view.SetColL( contextid_col, context->ContextId() );
	view.SetColL( ruleid_col, speechItem->RuleID() );
	
	// add the row to the table...
	view.PutL();

    aNewId = view.ColUint32( tagid_col );

	// finished - so close the view
	view.Close();
	CleanupStack::PopAndDestroy( &view );
    }


// -----------------------------------------------------------------------------
// CNssVasDb::SaveRRDL
// Reads RRD from DB to memory. Utility function when reading tags.
// -----------------------------------------------------------------------------
//
void CNssVasDb::SaveRRDL( CNssRRD* aRRD )
    {
    TInt position;
	RDbView view1,view2;
   
	_LIT( KSQLAddRRDText, "select * from rrdtexttable");
	_LIT( KSQLAddRRDId,   "select * from rrdinttable");
	
    // Save the integer array, if we have one.
    if ( aRRD->IntArray() )
        {
	    CleanupClosePushL(view1);
	
	    User::LeaveIfError( view1.Prepare( iDatabase, TDbQuery( KSQLAddRRDId, EDbCompareNormal ) ) );	
	    User::LeaveIfError( view1.EvaluateAll());

	    // Get column set	
        CDbColSet* columns1 = view1.ColSetL();
        // Get column ordinals
        TDbColNo tagid_col           = columns1->ColNo( KTagIdCol );
	    TDbColNo rrdint_col          = columns1->ColNo( KRRDIntCol );
        TDbColNo rrdposition_col1    = columns1->ColNo( KRRDPositionCol );

	    delete columns1;

	    // Begin process of inserting a row...
	    // set column values for id table...
        for( position=0; position<aRRD->IntArray()->Count(); position++ )
            {
	        view1.InsertL();
	        view1.SetColL( tagid_col,aRRD->TagId() );
	        view1.SetColL(rrdint_col,aRRD->IntArray()->At( position ) );
	        view1.SetColL(rrdposition_col1,position);
	        // add the row to the table...
	        view1.PutL();	   
	        }	
	
	    // finished - so close the view
	    view1.Close(); 	
	    CleanupStack::PopAndDestroy( &view1 );
        }

    // Save the text array, if we have one
    if ( aRRD->TextArray() )
        {
	    CleanupClosePushL( view2 );

        User::LeaveIfError( view2.Prepare( iDatabase, 
                            TDbQuery( KSQLAddRRDText, EDbCompareNormal ) ) );	
	    User::LeaveIfError( view2.EvaluateAll() );
    
        // Get column set
        CDbColSet* columns2 = view2.ColSetL();
        // Get column ordinals
        TDbColNo tagid_col2       = columns2->ColNo( KTagIdCol );
	    TDbColNo rrdtext_col2     = columns2->ColNo( KRRDTextCol );
        TDbColNo rrdposition_col2 = columns2->ColNo( KRRDPositionCol );

	    delete columns2;

	    //set colum values for text table
	    for( position = 0; position < aRRD->TextArray()->Count(); position++ )
            {
	        view2.InsertL();
	        view2.SetColL( tagid_col2, aRRD->TagId() );
	        view2.SetColL( rrdtext_col2, aRRD->TextArray()->At( position ) );
	        view2.SetColL( rrdposition_col2, position );
	        view2.PutL();	   
            }	
	    // add the row to the table...
	
	    // finished - so close the view
	    view2.Close();  
	    CleanupStack::PopAndDestroy( &view2 );
        }
    }

// -----------------------------------------------------------------------------
// CNssVasDb::FillRRDL
// Reads RRD from DB to memory. Utility function when reading tags.
// -----------------------------------------------------------------------------
//
void CNssVasDb::FillRRDL( TUint32 aTagId, CNssRRD& aRRD )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::FillRRDL" );
    
    TInt position;
	RDbView view1,view2;

	_LIT( KSQLAddRRDText, "select * from rrdtexttable ");
	_LIT( KSQLAddRRDId,   "select * from rrdinttable ");
	_LIT( KSQLWhereTagId,"where tagid=");

	iSQLStatement = KSQLAddRRDId;
	iSQLStatement.Append( KSQLWhereTagId );
	iSQLStatement.AppendNumUC( aTagId );

	CleanupClosePushL(view1);

	User::LeaveIfError( view1.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
	User::LeaveIfError( view1.EvaluateAll());

	// Get column set
    CDbColSet* columns1 = view1.ColSetL();
    // Get column ordinals
    TDbColNo tagid_col           = columns1->ColNo( KTagIdCol );
	TDbColNo rrdint_col          = columns1->ColNo( KRRDIntCol );
    TDbColNo rrdposition_col1    = columns1->ColNo( KRRDPositionCol );
	// Cleanup column1 set
	delete columns1;

	// Begin process of inserting a row...
	TInt intarraysize = view1.CountL();
    if ( intarraysize > 0 )
        {
        CArrayFixFlat<TInt>* intArray = new (ELeave) CArrayFixFlat<TInt>(intarraysize);
        CleanupStack::PushL( intArray );
        intArray->ResizeL( intarraysize );
        
        // set column values for id table...
        for ( view1.FirstL(); view1.AtRow(); view1.NextL() )
            {
            view1.GetL();
            aRRD.SetTagId( view1.ColUint32( tagid_col ) );
            position = view1.ColUint32( rrdposition_col1 );
            if ( position < 0 || position >= intarraysize )
                {
                RUBY_ERROR2("CNssVasDb::FillRRDL position out of bounds. 0 < %i < %i", 
                    position, intarraysize );
                User::Leave( KErrCorrupt );
                }
            intArray->At( position ) = view1.ColUint32( rrdint_col );
            }

        aRRD.SetIntArrayL( intArray );
        CleanupStack::Pop( intArray );
        intArray->Reset();
        delete intArray;
        }
    
	// finished - so close the view
	view1.Close();
	CleanupStack::PopAndDestroy( &view1 );
 
	CleanupClosePushL(view2);

	iSQLStatement = KSQLAddRRDText;
	iSQLStatement.Append( KSQLWhereTagId );
	iSQLStatement.AppendNumUC( aTagId );
    User::LeaveIfError( view2.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
	User::LeaveIfError( view2.EvaluateAll() );
    
    // Get column set
    CDbColSet* columns2 = view2.ColSetL();
    // Get column ordinals
    TDbColNo tagid_col2       = columns2->ColNo( KTagIdCol );
	TDbColNo rrdtext_col2     = columns2->ColNo( KRRDTextCol );
    TDbColNo rrdposition_col2 = columns2->ColNo( KRRDPositionCol );
   
	delete columns2;
    
	TInt textarraysize = view2.CountL();
    if ( textarraysize > 0 )
        {
        CArrayFixFlat<NssRRDText>* textArray = 
                    new (ELeave) CArrayFixFlat<NssRRDText>(textarraysize);
        CleanupStack::PushL( textArray );
        textArray->ResizeL( textarraysize );

        //set colum values for text table
        for ( view2.FirstL(); view2.AtRow(); view2.NextL() )
            {
            view2.GetL();
            aRRD.SetTagId( view2.ColUint32( tagid_col2 ) );
            position      = view2.ColUint32( rrdposition_col2 );
            if ( position < 0 || position >= textarraysize )
                {
                RUBY_ERROR2("CNssVasDb::FillRRDL position out of bounds. 0 < %i < %i", 
                    position, textarraysize );
                User::Leave( KErrCorrupt );
                }
            textArray->At( position ) = view2.ColDes( rrdtext_col2 );
            }	

        aRRD.SetTextArrayL( textArray );
        CleanupStack::Pop( textArray );
        textArray->Reset();
        delete textArray;
        }

	// finished - so close the view
	view2.Close();
        
    CleanupStack::PopAndDestroy( &view2 );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetTagL
// Gets a tag by grammar ID and rule ID.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagL( TNssGrammarIdRuleId aGrammarIdRuleId )
    {
	CNssContext* context = iContextBuilder.CreateContextL();
	CleanupStack::PushL( context );
	RDbView view;

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

	CleanupClosePushL(view);

	_LIT( KSQLGetContext, "select * from contexttable where grammarid=");
	iSQLStatement.Copy(KSQLGetContext);
	iSQLStatement.AppendNumUC(aGrammarIdRuleId.iGrammarId);

    TDbQuery dbQuery(iSQLStatement, EDbCompareNormal);
	User::LeaveIfError( view.Prepare( iDatabase, dbQuery ) );
	User::LeaveIfError( view.EvaluateAll() );

	view.FirstL(); 
    FillContextL(view, *context);

	_LIT( KSQLGetTag, "select * from tagtable where ruleid=");
	_LIT( KSQLAnd," and contextid=");
	iSQLStatement = KSQLGetTag;
	iSQLStatement.AppendNumUC( aGrammarIdRuleId.iRuleId );
    iSQLStatement.Append( KSQLAnd );
	iSQLStatement.AppendNumUC( context->ContextId() );

    // Close view
    CleanupStack::PopAndDestroy( &view );

    CArrayPtrFlat<CNssTag>* res = GetTagListByQueryL( iSQLStatement );

    // Pop rollback cleanup item
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );
    
    CleanupStack::PopAndDestroy( context );

    return( res );
    }
    
// -----------------------------------------------------------------------------
// CNssVasDb::GetTagsL
// Gets a list of tags by grammar ID and rule ID.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagsL( TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds )
	{
	CNssContext* context = iContextBuilder.CreateContextL();
	CleanupStack::PushL( context );
	RDbView view;

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    Mem::FillZ( context,     sizeof(*context)   );

	CleanupClosePushL(view);
	
	_LIT( KSQLGetContext, "select * from contexttable where ");
	_LIT( KGrammarIdParam, "grammarid=" );
	_LIT( KOr, " OR ");
	iSQLStatement.Copy(KSQLGetContext);
	
	for( TInt i = 0; i < aGrammarIdRuleIds.Count(); i++)
		{
		if( i !=0 )
			{
			iSQLStatement.Append(KOr);
			}
		iSQLStatement.Append(KGrammarIdParam);
		iSQLStatement.AppendNumUC(aGrammarIdRuleIds[i].iGrammarId);
		// same grammar ids usually appear, but that's ok and should not decrease performance
		}
		
	TDbQuery dbQuery(iSQLStatement, EDbCompareNormal);
	User::LeaveIfError( view.Prepare( iDatabase, dbQuery ));
	User::LeaveIfError( view.EvaluateAll());
	
	_LIT( KSQLGetTags, "select * from tagtable where (");
	_LIT( KRuleId, "ruleid=");
	_LIT( KContextId,"contextid=");
	_LIT( KBracketAnd, " AND (");
	_LIT( KBracketOr, ") OR (");
	_LIT( KFinalBracket, ")");
	
	// Get column set
    CDbColSet* columns = view.ColSetL();
	TDbColNo contextid_col= columns->ColNo( KContextIdCol );
	TDbColNo grammarid_col= columns->ColNo( KGrammarIdCol );
	delete columns;
	TUint32 currContextId;
	TUint32 currGrammarId;
	TUint32 currRuleId;
	
	iSQLStatement = KSQLGetTags;
	TBool firstPair = ETrue;
	while( view.NextL())
		{
		view.GetL();
		// before each iteration, but first, we add ") OR ("
		if( firstPair )
			{
			firstPair = EFalse;
			}
		else 
			{
			iSQLStatement.Append(KBracketOr);
			}
		currContextId = view.ColUint32( contextid_col );
		currGrammarId = view.ColUint32( grammarid_col );
		iSQLStatement.Append( KContextId );
		iSQLStatement.AppendNumUC( currContextId );
		iSQLStatement.Append( KBracketAnd );
		// we need to find corresponding rule id
		TBool someGrammarFoundAlready = EFalse;
		for( TInt i=0; i<aGrammarIdRuleIds.Count(); i++)
			{
			if( currGrammarId == aGrammarIdRuleIds[i].iGrammarId )
				{
				if( someGrammarFoundAlready )
					{
					iSQLStatement.Append( KOr );
					}
				else 
					{
					someGrammarFoundAlready = ETrue;
					}
				currRuleId = aGrammarIdRuleIds[i].iRuleId;
				iSQLStatement.Append( KRuleId );
				iSQLStatement.AppendNumUC( currRuleId );	
				}
			}  // for
		iSQLStatement.Append(KFinalBracket);  // closing ruleid ORs
		}  // while
	// Close view
    CleanupStack::PopAndDestroy( &view );
    iSQLStatement.Append( KFinalBracket );
    
    CArrayPtrFlat<CNssTag>* res = GetTagListByQueryL( iSQLStatement );

    // Pop rollback cleanup item
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );

    CleanupStack::PopAndDestroy( context );

    return( res );
	}

// -----------------------------------------------------------------------------
// CNssVasDb::GetTagListByQuery
// Executes the query and reads matches from DB to memory.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CNssVasDb::GetTagListByQuery
// Executes the query and reads matches from DB to memory.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagListByQueryL( const TDesC& aSqlQuery )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::GetTagListByQueryL" );

    __ASSERT_DEBUG( aSqlQuery.Left(6).Compare( _L("select") ) == 0, User::Panic( KVasDbPanic, __LINE__ ) );

	CNssContext* context = NULL;
	CNssSpeechItem* speechItem = NULL;
	CNssRRD* rrd = NULL;
    TInt contextId = -1;

    // 'context' variable is the context of the previous tag.
    // When a new tag is read, 'context' is updated only if context changes.
    // Set the ID invalid value in order to force the first tag to read a context.
    //context.iContextId = (TUint32)-1;
    //context->SetContextId( -1 );

	RDbView view;
	CleanupClosePushL( view );

    CArrayPtrFlat<CNssTag>* tagList = new(ELeave)CArrayPtrFlat<CNssTag>( KMaxTagArraySize );
    CleanupStack::PushL( tagList );
    CleanupResetAndDestroyPushL( *tagList );

	User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( aSqlQuery, EDbCompareNormal ) ) );
	User::LeaveIfError( view.EvaluateAll() );

	if ( !view.IsEmptyL() )
	    {
   	    for (view.FirstL(); view.AtRow(); view.NextL())
	        {
	        context = iContextBuilder.CreateContextL();
	        CleanupStack::PushL( context );
	        
	        speechItem = iSpeechItemBuilder.CreateEmptySpeechItemL( *context );
	        CleanupStack::PushL( speechItem );
	        
            FillSpeechItemL( view, *speechItem, contextId );

            // Get context, if we haven't already done it.
            if ( context->ContextId() != (TUint32)contextId )
                {                
                _LIT( KContextQuery, "select * from contexttable where contextid=" );

                iSQLStatement.Copy( KContextQuery );
                iSQLStatement.AppendNumUC( contextId );

                CArrayPtrFlat<CNssContext>* contextList = 
                            GetContextInsideTransactionL( iSQLStatement );
                CleanupStack::PushL( contextList );

                User::LeaveIfNull( contextList );

                if ( contextList->Count() != 1 )
                    {
                    User::Leave( KErrCorrupt );
                    }

                CleanupStack::Pop( contextList );
                CleanupStack::PopAndDestroy( speechItem );                
                CleanupStack::PopAndDestroy( context );
                
                context = (*contextList)[0];                
                delete contextList;
                
                CleanupStack::PushL( context );
                
	            speechItem = iSpeechItemBuilder.CreateEmptySpeechItemL( *context );
	            CleanupStack::PushL( speechItem );
	        	        
                FillSpeechItemL( view, *speechItem, contextId );
                }
                
            rrd = CNssRRD::NewL();
            CleanupStack::PushL( rrd );
            
            FillRRDL( speechItem->TagId(), *rrd );
            
            // Append tag to the array 
            FillTagListArrayL( *tagList, context, speechItem, rrd );
            CleanupStack::PopAndDestroy( rrd );
            CleanupStack::PopAndDestroy( speechItem );
            CleanupStack::PopAndDestroy( context );
            }
        }
	else
	    {
        User::Leave( KErrNotFound );
	    }

	CleanupStack::Pop( tagList );  // ResetAndDestroy
    CleanupStack::Pop( tagList );  // array itself
	// Finished with the view - so close it.
    CleanupStack::PopAndDestroy( &view );

    return tagList;
    }
    
// -----------------------------------------------------------------------------
// CNssVasDb::GetTagL
// Gets a tag by name.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagL(const TDesC& aName)
    {
    _LIT( KTick,"' ");
	_LIT( KSQLGetTag, "select * from tagtable where name='");
	
	iSQLStatement = KSQLGetTag;
	iSQLStatement.Append( aName );
    iSQLStatement.Append( KTick );

    User::LeaveIfError( StartTransaction() );

    CArrayPtrFlat<CNssTag>* arr = GetTagListByQueryL( iSQLStatement );

    CommitTransaction( EFalse );

    return arr;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetTagL
// Gets all tags from a specified context.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagL( const CNssContext& aContext)
    {
    RDbView view;

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    CleanupClosePushL( view );

	// Validate the context
    _LIT( KSQLGetContext, "select * from contexttable where contextid=");
	iSQLStatement = KSQLGetContext;
	iSQLStatement.AppendNumUC( aContext.ContextId() );

	User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
	User::LeaveIfError( view.EvaluateAll() );

    if ( view.IsEmptyL() )
	    {
        User::Leave( KErrNotFound );
	    }

	// Finished with the view - so close it.
    CleanupStack::PopAndDestroy( &view );
	
	_LIT( KSQLGetTag, "select * from tagtable where contextid=" );
	iSQLStatement.Copy( KSQLGetTag );
	iSQLStatement.AppendNumUC( aContext.ContextId() );

    CArrayPtrFlat<CNssTag>* result = GetTagListByQueryL( iSQLStatement );

    // Transaction finished - free the transaction lock
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );

    return result;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::GetTagListFromViewL
// Reads the tags from DB to memory and appends them to the array.
// Earlier, the caller has made an SQL query and placed the matches in RDbView.
// -----------------------------------------------------------------------------
//
void CNssVasDb::GetTagListFromViewL( RDbView& aView, CArrayPtrFlat<CNssTag>* aTagList, 
                                     CNssContext* aContext )
    {
    TInt discardable( 0 );

	for ( aView.FirstL(); aView.AtRow(); aView.NextL() )
    	{
    	CNssSpeechItem* speechItem = iSpeechItemBuilder.CreateEmptySpeechItemL( *aContext );
    	CleanupStack::PushL( speechItem );
    	
	    FillSpeechItemL( aView, *speechItem, discardable );
		CNssRRD* rrd = CNssRRD::NewL();
		CleanupStack::PushL( rrd );
		FillRRDL( speechItem->TagId(), *rrd );
		    
		FillTagListArrayL( *aTagList, aContext, speechItem, rrd );
		
		CleanupStack::PopAndDestroy( rrd );
		CleanupStack::PopAndDestroy( speechItem );
	    }
    }

// -----------------------------------------------------------------------------
// CNssVasDb::DeleteTagL
// Gets a list of tags by RRD data.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagL( TInt aContextId, TInt aNum, TInt aPosition )
    {
  	RDbView     view;
	TUint32     tagId;
	CNssContext* context = iContextBuilder.CreateContextL();
	CleanupStack::PushL( context );

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    CArrayPtrFlat<CNssTag>* tagArray = new(ELeave)CArrayPtrFlat<CNssTag>(1);
    CleanupStack::PushL( tagArray );
    CleanupResetAndDestroyPushL( *tagArray );

    // Get the context based on ContextId
  	CleanupClosePushL(view);

	_LIT( KSQLGetContext, "select * from contexttable where contextid=" );
	iSQLStatement.Copy( KSQLGetContext );
	iSQLStatement.AppendNumUC( aContextId );

	User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	view.FirstL();
    view.GetL();
	TRAPD( error, FillContextL( view, *context ) );
	// Finished with the view - so close it.
	view.Close();
	CleanupStack::PopAndDestroy( &view );

    // If filling of the context failed, return with Not Found
	if ( error != KErrNone )
	    {
        User::Leave( KErrNotFound );
        }

    // Get the list of TagId from the RRD table based on Num and Position
	RDbView view1;
	CleanupClosePushL( view1 );

	_LIT( KSQLGetRRDInt, "select * from rrdinttable where rrdposition=" );
	_LIT( KSQLAndRRDInt, " and rrdint=" );
	iSQLStatement = KSQLGetRRDInt;
	iSQLStatement.AppendNumUC( aPosition );
	iSQLStatement.Append( KSQLAndRRDInt );
	iSQLStatement.AppendNumUC( aNum );

	User::LeaveIfError( view1.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
	User::LeaveIfError( view1.EvaluateAll() );
	
	if ( view1.IsEmptyL() )
	    {

        User::Leave( KErrNotFound );
	    }

    // Get column set
    CDbColSet* columns = view1.ColSetL();
    // Get column ordinals
    TDbColNo tagid_col= columns->ColNo( KTagIdCol );
    // Cleanup column set
    delete columns;

    // Select the valid tagId from the list
	for ( view1.FirstL(); view1.AtRow(); view1.NextL() )
        {   
        view1.GetL();
        tagId = view1.ColUint32( tagid_col );

        // Get the SpeechItem based on the tagId and ContextId
        RDbView view2;
		CleanupClosePushL( view2 );

        _LIT( KSQLGetSpeechItem, "select * from tagtable where tagid=" );
        _LIT( KSQLAndContextId," and contextid=" );
        iSQLStatement = KSQLGetSpeechItem;
        iSQLStatement.AppendNumUC( tagId );
	    iSQLStatement.Append( KSQLAndContextId );
        iSQLStatement.AppendNumUC( aContextId );

        User::LeaveIfError( view2.Prepare( iDatabase, 
                            TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
        User::LeaveIfError( view2.EvaluateAll() );
    
        if ( !view2.IsEmptyL() )
	        {
            GetTagListFromViewL( view2, tagArray, context );
		    }
        view2.Close();
        CleanupStack::PopAndDestroy( &view2 );
        }

    // close the view 1
	view1.Close();
	CleanupStack::PopAndDestroy( &view1 );

    CleanupStack::Pop( tagArray );  // ResetAndDestroy
    CleanupStack::Pop( tagArray );  // array itself

    // Rollback cleanup
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );
    
    CleanupStack::PopAndDestroy( context );

    return tagArray;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::DeleteTagL
// Gets a list of tags by RRD data.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagL( TInt aContextId, 
                                            const TDesC& aText, TInt aPosition )
    {
  	RDbView view;
	TUint32 tagId;
	CNssContext* context = iContextBuilder.CreateContextL();
	CleanupStack::PushL( context );
	
    CArrayPtrFlat<CNssTag>* tagArray = new(ELeave)CArrayPtrFlat<CNssTag>(1);
    CleanupStack::PushL( tagArray );
    CleanupResetAndDestroyPushL( *tagArray );

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    // Get the context based on ContextId
  	CleanupClosePushL( view );

	_LIT( KSQLGetContext, "select * from contexttable where contextid=" );
	iSQLStatement.Copy( KSQLGetContext );
	iSQLStatement.AppendNumUC( aContextId );

	User::LeaveIfError( view.Prepare(iDatabase, TDbQuery(iSQLStatement, EDbCompareNormal)));
	User::LeaveIfError( view.EvaluateAll());
	view.FirstL();
    view.GetL();
	TRAPD( error, FillContextL( view, *context ) );
	// Finished with the view - so close it.
	view.Close();
	CleanupStack::PopAndDestroy( &view );

    // If filling of the context failed, return with Not Found
	if ( error != KErrNone )
	    {
        User::Leave( KErrNotFound );
        }

    // Get the list of TagId from the RRD table based on text and Position
	RDbView view1;
	CleanupClosePushL( view1 );

	_LIT( KSQLGetRRDText, "select * from rrdtexttable where rrdposition=" );
	_LIT( KSQLAndRRDText, " and rrdtext='" );
	_LIT( KSQLTick, "'" );

	iSQLStatement = KSQLGetRRDText;
	iSQLStatement.AppendNumUC( aPosition );
	iSQLStatement.Append( KSQLAndRRDText );
	iSQLStatement.Append( aText );
    iSQLStatement.Append( KSQLTick );

	User::LeaveIfError( view1.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
	User::LeaveIfError( view1.EvaluateAll() );
	
	if ( view1.IsEmptyL() )
	    {
        User::Leave( KErrNotFound );
	    }

    // Get column set
    CDbColSet* columns = view1.ColSetL();
    // Get column ordinals
    TDbColNo tagid_col= columns->ColNo( KTagIdCol );
    // Cleanup column set
    delete columns;

    // Select the valid tagId from the list
	for ( view1.FirstL(); view1.AtRow(); view1.NextL() )
	    {
        view1.GetL();
        tagId = view1.ColUint32( tagid_col );

        // Get the SpeechItem based on the tagId and ContextId
        RDbView view2;
		CleanupClosePushL( view2 );

        _LIT( KSQLGetSpeechItem, "select * from tagtable where tagid=" );
        _LIT( KSQLAndContextId, " and contextid=" );
        iSQLStatement = KSQLGetSpeechItem;
        iSQLStatement.AppendNumUC( tagId );
	    iSQLStatement.Append( KSQLAndContextId );
        iSQLStatement.AppendNumUC( aContextId );

        User::LeaveIfError( view2.Prepare( iDatabase, 
                            TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
        User::LeaveIfError( view2.EvaluateAll() );

        if ( !view2.IsEmptyL() )
	        {
            GetTagListFromViewL( view2, tagArray, context );
		    }
        CleanupStack::PopAndDestroy( &view2 );
        }
    // close the view 1
	CleanupStack::PopAndDestroy( &view1 );

    // Roll back transaction
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );

    CleanupStack::Pop( tagArray );  // ResetAndDestroyPushL
    CleanupStack::Pop( tagArray );  // array itself
    CleanupStack::PopAndDestroy( context );

    return tagArray;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::DeleteTagL
// Get a tag by ID.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssTag>* CNssVasDb::GetTagL( TUint32 aTagId )
    {
	RDbView view;

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    CleanupClosePushL( view );


	_LIT( KSQLGetTag, "select * from tagtable where tagid=");

	iSQLStatement = KSQLGetTag;
	iSQLStatement.AppendNumUC( aTagId );

    CArrayPtrFlat<CNssTag>* result = GetTagListByQueryL( iSQLStatement );

    // Close view
    CleanupStack::PopAndDestroy( &view );

    // Roll back transaction
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( EFalse );

    return( result );
    }


// -----------------------------------------------------------------------------
// CNssVasDb::DeleteTagL
// Deletes a tag by name
// -----------------------------------------------------------------------------
//
void CNssVasDb::DeleteTagL( const TDesC& aName )
    {
    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }

    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    TUint32 tagId;
    RDbView view;
	
    CleanupClosePushL(view);

    _LIT( KTick,"' ");
    _LIT( KSQLGetTag, "select * from tagtable where name='");
	
    iSQLStatement = KSQLGetTag;
    iSQLStatement.Append( aName );
    iSQLStatement.Append( KTick );

    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery(iSQLStatement, EDbCompareNormal)));
    User::LeaveIfError( view.EvaluateAll());

    // Get column set
    CDbColSet* columns = view.ColSetL();
    // Get column ordinals
    TDbColNo tagid_col= columns->ColNo( KTagIdCol );
    // Cleanup column set
    delete columns;

    view.FirstL();
    view.GetL();
    tagId = view.ColUint32( tagid_col );

    // Close view
    CleanupStack::PopAndDestroy( &view );

    // Pop rollback item and do commit
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( ETrue );

    DeleteTagL( tagId );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::DeleteTagL
// Deletes a tag by ID
// -----------------------------------------------------------------------------
//
void CNssVasDb::DeleteTagL( TUint32 aTagId )
    {
    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "CNssVasDb error: database not opened." );
        
        User::Leave( KErrNotReady );
        }
   
    // Lock database
    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();

    DeleteTagInsideTransactionL( aTagId );

    // Commit transaction - unlock databaes
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( ETrue );
    }


// -----------------------------------------------------------------------------
// CNssVasDb::DeleteTagInsideTransactionL
// Deletes the RRD information of a tag.
// -----------------------------------------------------------------------------
//
void CNssVasDb::DeleteTagInsideTransactionL( TUint32 aTagId )
    {
    TInt error( KErrNone );

    _LIT( KSQLDeleteTag, "delete from tagtable where tagid=");

	ReserveDiskSpaceL( sizeof(CNssTag) );

    iSQLStatement = KSQLDeleteTag;
    iSQLStatement.AppendNumUC(aTagId);

    error = iDatabase.Execute( iSQLStatement );
    if ( error < KErrNone )
        {
        ReleaseDiskSpace();
        User::Leave( error );
        }

    TRAP( error, DeleteRRDL( aTagId ) );
    
    ReleaseDiskSpace();
    
    User::LeaveIfError( error );
    }


// -----------------------------------------------------------------------------
// CNssVasDb::DeleteRRDL
// Deletes the RRD information of a tag.
// -----------------------------------------------------------------------------
//
void CNssVasDb::DeleteRRDL(TUint32 aTagId)
    {
    TInt error( KErrNone );
    
    _LIT( KSQLAddRRDText, "delete from rrdtexttable ");
    _LIT( KSQLAddRRDId,   "delete from rrdinttable ");
    _LIT( KSQLWhereTagId,"where tagid=");

    iSQLStatement = KSQLAddRRDId;
    iSQLStatement.Append( KSQLWhereTagId );
    iSQLStatement.AppendNumUC( aTagId );
	
    error = iDatabase.Execute( iSQLStatement );
   
    if ( error < KErrNone && error != KErrNotFound )
        {
        User::Leave( error );
        }

    iSQLStatement = KSQLAddRRDText;
    iSQLStatement.Append( KSQLWhereTagId );
    iSQLStatement.AppendNumUC( aTagId );

    error = iDatabase.Execute( iSQLStatement );

    if ( error < KErrNone && error != KErrNotFound )
        {
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CNssVasDb::DeleteContextL
// Deletes a context.
// -----------------------------------------------------------------------------
//
void CNssVasDb::DeleteContextL( const TDesC& aName )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::DeleteContextL" );
    
    TInt error( KErrNone );
    TUint32 contextId;
    TUint32 tagId;
    RDbView view1,view2;
    
    // Begin transaction
    User::LeaveIfError( StartTransaction() );
    CreatePushRollbackItemLC();
    
    CleanupClosePushL( view1 );
    
    // Search the context from DB
    _LIT( KTick, "' " );
    _LIT( KSQLGetContext, "select * from contexttable where name='" );
    
    iSQLStatement = KSQLGetContext;
    iSQLStatement.Append( aName );
    iSQLStatement.Append( KTick );
    
    User::LeaveIfError( view1.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
    User::LeaveIfError( view1.EvaluateAll() );
    
    // Get column set
    CDbColSet* columns1 = view1.ColSetL();
    // Get column ordinals
    TDbColNo contextid_col= columns1->ColNo( KContextIdCol );
    // Cleanup column set
    delete columns1;
    
    view1.FirstL();
    
    // Doesn't exists, can't delete
    if ( !view1.AtRow() )
        {
        User::Leave( KErrNotFound );
        }
    
    view1.GetL();
    contextId = view1.ColUint32( contextid_col );
    
    view1.DeleteL();
    view1.Close();	
    CleanupStack::PopAndDestroy( &view1 );
    
    // Delete all tags, which were in the context.
    _LIT( KSQLGetTag, "select * from tagtable where contextid=" );
    
    iSQLStatement = KSQLGetTag;
    iSQLStatement.AppendNumUC( contextId );
    
    CleanupClosePushL( view2 );
    
    User::LeaveIfError( view2.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );
    User::LeaveIfError( view2.EvaluateAll() );
    
    ReserveDiskSpaceL( (sizeof(CNssTag) * view2.CountL()) + sizeof(CNssContext) );
    
    view2.FirstL();
    
    // If this context has any tags, remove the RRD entries from the RRD table.
    if ( view2.AtRow() )
        {
        // Get column set
        CDbColSet* columns2 = view2.ColSetL();
        
        // Get column ordinals
        TDbColNo tagid_col = columns2->ColNo( KTagIdCol );
        // Cleanup column2 set
        delete columns2;
        
        for ( view2.FirstL(); view2.AtRow(); view2.NextL() )
            {
            view2.GetL();
            tagId = view2.ColUint32( tagid_col );
            RUBY_DEBUG0( "CNssVasDb::DeleteContextL deleting RRD" );
            // Return value ignored since other tag ids should be iterated through
            // even though one RRD deletion fails
            TRAP( error, DeleteRRDL( tagId ) );
            }
        
        view2.Close();
        }
    
    // Remove tags from tag table
    _LIT( KSQLDeleteTag, "delete from tagtable where contextid=");
    iSQLStatement = KSQLDeleteTag;
    iSQLStatement.AppendNumUC( contextId );
    
    error = iDatabase.Execute( iSQLStatement );
    
    CleanupStack::PopAndDestroy( &view2 );
    
    if ( error < KErrNone && error != KErrNotFound )
        {
        ReleaseDiskSpace();
        // Roll back transaction
        CleanupStack::PopAndDestroy(); // Rollback cleanup item
        User::Leave( error );
        }
   
    CleanupStack::Pop();  // Rollback cleanup item
    CommitTransaction( ETrue );
   
    ReleaseDiskSpace();
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UpdateTagInTransactionL
// Updates a tag. Assumes that a transaction has been started earlier.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateTagInTransactionL( CNssTag& aTag )
    {
    CNssContext* context = static_cast<CNssContext*> ( aTag.Context() );
    UpdateContextInsideTransactionL( *context );
    CNssSpeechItem* speechItem = static_cast<CNssSpeechItem*>
        ( aTag.SpeechItem() );
    UpdateSpeechItemL( *speechItem );
    CNssRRD* rrd = static_cast<CNssRRD*> ( aTag.RRD() );
    UpdateRRDL( *rrd );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::DoUpdateContextL
// Updates an existing context.
// -----------------------------------------------------------------------------
//
void CNssVasDb::DoUpdateContextL( CNssContext& aContext )
    {
    _LIT( KSQLUpdateContext,"update contexttable set ");
    _LIT( KEqual,"=");
    _LIT( KCommon,",");
    _LIT( KTick,"'");
    _LIT( KSQLWhereContextId," where contextid = ");
   
    iSQLStatement = KSQLUpdateContext;
     
    iSQLStatement.Append( KNameCol );
    iSQLStatement.Append( KEqual );
    iSQLStatement.Append( KTick );
    iSQLStatement.Append( aContext.ContextName() );
    iSQLStatement.Append( KTick );
   
    iSQLStatement.Append( KCommon );
    iSQLStatement.Append( KGlobalCol );
    iSQLStatement.Append( KEqual );
    iSQLStatement.AppendNumUC( aContext.IsGlobal() );

    iSQLStatement.Append( KCommon );
    iSQLStatement.Append( KGrammarIdCol );
    iSQLStatement.Append( KEqual );
    iSQLStatement.AppendNumUC( aContext.GrammarId() );

    iSQLStatement.Append( KCommon );
    iSQLStatement.Append( KLexiconIdCol );
    iSQLStatement.Append( KEqual );
    iSQLStatement.AppendNumUC( aContext.LexiconId() );

    iSQLStatement.Append( KCommon );
    iSQLStatement.Append( KModelBankIdCol );
    iSQLStatement.Append( KEqual );
    iSQLStatement.AppendNumUC( aContext.ModelBankId() );

    iSQLStatement.Append( KSQLWhereContextId );
    iSQLStatement.AppendNumUC( aContext.ContextId() );
 
    User::LeaveIfError( iDatabase.Execute( iSQLStatement ) );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::DoUpdateUpdateContextClientDataL
// Updates the client data of a context.
// -----------------------------------------------------------------------------
//
void CNssVasDb::DoUpdateContextClientDataL( CNssContext& aContext )
    {
	__UHEAP_MARK;

	// Declare a literal string to hold the SQL statement
	// SELECT KBinaryLexiconColumn , KBinaryLexiconColumn FROM  KLexiconName

	_LIT( KSQLSelect1, "select clientdata from contexttable where name='" );

	TBuf<KSqlStatementmaxLength> sqlStatement;
	sqlStatement.Append( KSQLSelect1 );
    sqlStatement.Append( aContext.ContextName() );
    sqlStatement.Append( '\'' );

	// create a view on the database
	RDbView view;
    CleanupClosePushL( view ); // Stack: view

	User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( sqlStatement, EDbCompareNormal ) ) );
	User::LeaveIfError( view.EvaluateAll() );

	// Get the structure of rowset
	CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL( colSet ); // Stack: view, colset
    TInt error( KErrNone );
	
	if( view.FirstL() )
		{
		view.UpdateL();

    
		// add binary buffer by Using the stream  

		//RDbColWriteStream out;
		TDbColNo col = colSet->ColNo( _L("clientdata") ); // Ordinal position of client data

    	if ( col == KDbNullColNo )
        	{
        	User::Leave( KErrCorrupt );
        	}

    	view.SetColL( col, aContext.ClientData() ); 
	
		view.PutL();
	
		// close the view
    	view.Close();
		}
	else 
		{
		error = KErrNotFound;
		}
    
    CleanupStack::PopAndDestroy( colSet );
    CleanupStack::PopAndDestroy( &view );

	__UHEAP_MARKEND;

    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UpdateRRDL
// Updates an existing RRD.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateRRDL( CNssRRD& aRRD )
    {
    TRAPD( error, DeleteRRDL( aRRD.TagId() ) );

    if ( error != KErrNotFound)
        {
        error = KErrNone;
        }

    User::LeaveIfError( error );

    SaveRRDL( &aRRD );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UpdateSpeechItemL
// Updates an existing speech item.
// -----------------------------------------------------------------------------
//
void CNssVasDb::UpdateSpeechItemL( CNssSpeechItem& aSpeechItem )
    {
    RDbView view;
    CleanupClosePushL( view );
 
    _LIT( KSQLGet, "select * from tagtable where tagid=" );
    
    iSQLStatement = KSQLGet;
    iSQLStatement.AppendNumUC( aSpeechItem.TagId() );

    User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( iSQLStatement, EDbCompareNormal ) ) );	
    User::LeaveIfError( view.EvaluateAll());
    
    // Get column set
    CDbColSet* columns = view.ColSetL();
    // Get column ordinals
    TDbColNo name_col= columns->ColNo( KNameCol );

    delete columns;
	  
    if ( view.FirstL() )
        {
        // Begin process of updating a row...
        view.UpdateL();
        // set column values...
#ifdef __SIND_EXTENSIONS        
        view.SetColL( name_col, aSpeechItem.RawText() ); 
#else
        view.SetColL( name_col, aSpeechItem.Text() ); 
#endif

        // add the row to the table...
        view.PutL();
		
        // finished - so close the view
        view.Close();
		}
    else 
        {
        User::Leave( KErrNotFound );
        }	
	
    CleanupStack::PopAndDestroy( &view );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::FillSpeechItemL
// Reads a TNssSpeechItem from DB to memory. Earlier, the caller has run an SQL
// query and specifies the matching context in RDbView.
// -----------------------------------------------------------------------------
//
void CNssVasDb::FillSpeechItemL( RDbView& aView, CNssSpeechItem& aSpeechItem, TInt& aContextId )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::FillSpeechItemL" );

	if ( aView.IsEmptyL() )
	    {
		User::Leave( KErrNotFound );
	    }

	// Get column set
    CDbColSet* columns = aView.ColSetL();
    // Get column ordinals
 	TDbColNo tagid_col    = columns->ColNo( KTagIdCol );
 	TDbColNo contextid_col= columns->ColNo( KContextIdCol );
	TDbColNo ruleid_col   = columns->ColNo( KRuleIdCol );
	TDbColNo name_col     = columns->ColNo( KNameCol );

    // Cleanup column set
    delete columns;

	// Retrieve the current row
	aView.GetL();
	aSpeechItem.SetTagId( aView.ColUint32( tagid_col ) );
	aSpeechItem.SetRuleID( aView.ColUint32( ruleid_col ) );
	aSpeechItem.SetTextL( aView.ColDes( name_col ) );
    aContextId = aView.ColUint32( contextid_col );
    }


// -----------------------------------------------------------------------------
// CNssVasDb::FillContextL
// Reads a context from DB to memory. Earlier, the caller has run an SQL query
// and specifies the matching context in RDbView.
// -----------------------------------------------------------------------------
//
void CNssVasDb::FillContextL( RDbView& aView, CNssContext& aContext )
    {
    if ( aView.IsEmptyL() )
	    {
	    User::Leave( KErrNotFound );
	    }

	// Get column set
    CDbColSet* columns = aView.ColSetL();
    // Get column ordinals
 	TDbColNo contextid_col= columns->ColNo( KContextIdCol );
	TDbColNo name_col = columns->ColNo( KNameCol );
    TDbColNo global_col = columns->ColNo( KGlobalCol );
	TDbColNo grammarid_col= columns->ColNo( KGrammarIdCol );
	TDbColNo lexiconid_col= columns->ColNo( KLexiconIdCol );
	TDbColNo modelbankid_col= columns->ColNo( KModelBankIdCol );
    // Cleanup column set
    delete columns;

	// Retrieve the current row
	aView.GetL();
	aContext.SetContextId( aView.ColUint32( contextid_col ) );
	aContext.SetGlobal( aView.ColUint32( global_col ) );
	aContext.SetGrammarId( aView.ColUint32( grammarid_col ) );
	aContext.SetLexiconId( aView.ColUint32( lexiconid_col ) );
	aContext.SetModelBankId( aView.ColUint32( modelbankid_col ) );
	aContext.SetNameL( aView.ColDes( name_col ) );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::FillTagListArrayL
// Creates a TNssTag from context, speech item and RRD information.
// Adds a tag to aTagList.
// -----------------------------------------------------------------------------
//
void CNssVasDb::FillTagListArrayL( CArrayPtrFlat<CNssTag>& aTagList, CNssContext* aContext, 
                                   CNssSpeechItem* aSpeechItem, CNssRRD* aRRD )
    {    
    CNssContext* contextCopy = aContext->CopyL();
    CNssSpeechItem* speechItenCopy = aSpeechItem->CopyL( contextCopy );
    CNssRRD* rrdCopy = aRRD->CopyL();
    CNssTag* tag = new (ELeave) CNssTag( contextCopy, rrdCopy, speechItenCopy );
    tag->SetTagId( aSpeechItem->TagId() );
	
	aTagList.AppendL( tag );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::FillContextListArrayL
// Utility function to read contexts from DB to memory. Earlier, the caller has
// run an SQL query and the matches are listed in an RDbView.
// -----------------------------------------------------------------------------
//
void CNssVasDb::FillContextListArrayL( RDbView& aView, CArrayPtrFlat<CNssContext>& aContextList )
    {
    RUBY_DEBUG_BLOCK("CNssVasDb::FillContextListArrayL");
	TInt pos( 0 );

	// Clear the previous array.
	aContextList.Reset();

    // Get column set
    CDbColSet* columns = aView.ColSetL();

    // Get column ordinals
    TDbColNo name_col       = columns->ColNo( KNameCol );
    TDbColNo global_col     = columns->ColNo( KGlobalCol );
	TDbColNo grammarid_col  = columns->ColNo( KGrammarIdCol );
	TDbColNo lexiconid_col  = columns->ColNo( KLexiconIdCol );
	TDbColNo modelbankid_col= columns->ColNo( KModelBankIdCol );
	TDbColNo contextid_col  = columns->ColNo( KContextIdCol );

    // New in 2.8
    TDbColNo clientdata_col = columns->ColNo( KClientDataCol );

	delete columns;

	for (aView.FirstL(); aView.AtRow(); aView.NextL())
	    {
	    aView.GetL(); // Retrieve the current row - actually reads the row from the database.
       
        CNssContext* oneContext = iContextBuilder.CreateContextL();
        CleanupStack::PushL( oneContext );
       
        oneContext->SetNameL( aView.ColDes( name_col ) );
        oneContext->SetGlobal( aView.ColUint32( global_col ) );
        oneContext->SetContextId( aView.ColUint32( contextid_col ) );
        oneContext->SetLexiconId( aView.ColUint32( lexiconid_col ) );
        oneContext->SetGrammarId( aView.ColUint32( grammarid_col ) );
        oneContext->SetModelBankId( aView.ColUint32( modelbankid_col ) );
        oneContext->SetClientData( aView.ColDes8( clientdata_col ) );
       
	    // Now add the context object to the list (array) of context.
	    aContextList.InsertL( pos++, oneContext );
	   
	    // Copy has been created in aContextList, no need for pointer anymore
	    CleanupStack::Pop( oneContext );
	    }
}

// -----------------------------------------------------------------------------
// CNssVasDb::StartTransactionL
// Starts a new transaction.
// -----------------------------------------------------------------------------
//
TInt CNssVasDb::StartTransaction()
    {
    RUBY_DEBUG0( "CNssVasDb::StartTransaction" );
    
    if ( !iClientHasOpenedDatabase )
        {
        RUBY_DEBUG0( "No open DB. E.g. backup/restore in progress" );
        return KErrNotReady;
        }

    __ASSERT_DEBUG( iLocked == iDatabase.InTransaction(), User::Panic( KVasDbPanic, __LINE__ ) );
    
    iCriticalSection.Wait();
    if ( iLocked )
        {
        // If we already have the lock, no need to take it again
        RUBY_DEBUG0( "CNssVasDb::StartTransaction DB is already locked by current process" );
        return KErrNone;
        }

    // Wait for the mutex so that two processes don't try get the lock simultaneously
    RUBY_DEBUG0( "Waiting for mutex in CNssVasDb::StartTransaction" );
    iMutex.Wait();
    
    // check if backup is ongoing
	    
    TInt readLockErr = iDatabase.Begin();

    // Mutex should make it sure that we have exclusive lock to Database
    // Thus iDatabase.Begin() should succeed every time
    __ASSERT_DEBUG( readLockErr == KErrNone, User::Panic( KVasDbPanic, __LINE__ ) );
   
    if ( readLockErr == KErrNone )
        {
        iLocked = ETrue;
        }

    RUBY_DEBUG0( "CNssVasDb::StartTransaction completed" );        
        
    return readLockErr;
    }

// -----------------------------------------------------------------------------
// CNssVasDb::CommitTransaction
// Commits changes and releases the lock. If iShouldBeLocked flag is set,
// opens a new tranaction in order to keep the database locked.
// -----------------------------------------------------------------------------
//
TInt CNssVasDb::CommitTransaction( TBool aCompact )
    {
    RUBY_DEBUG0( "CNssVasDb::CommitTransaction" );

    __ASSERT_DEBUG( iLocked == iDatabase.InTransaction(), User::Panic( KVasDbPanic, __LINE__ ) );
    //__ASSERT_DEBUG( iLocked, User::Panic( _L("VasCNssVasDb.cpp"), __LINE__ ) );

    TInt ret = KErrNone;

    if ( iLocked )
        {
        ret = iDatabase.Commit();
        
        if ( iDatabase.IsDamaged() )
            {
            RUBY_DEBUG0( "CNssVasDb::CommitTransaction Recovering" );
            ret = iDatabase.Recover();
            }
        
        if ( aCompact )
            {
            RUBY_DEBUG0( "CNssVasDb::CommitTransaction compacting DB" );
            iDatabase.Compact(); 
            }
        
        iLocked = EFalse;
        
        RUBY_DEBUG0( "Signalling mutex in CNssVasDb::CommitTransaction" );
        iMutex.Signal();
        }
    else
        {
        RUBY_DEBUG0( "ERROR: CNssVasDb::CommitTransaction does nothing, not locked" );
        }
        
    iCriticalSection.Signal();
    return( ret );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::RollbackTransaction
// Discards changes and releases the lock.
// -----------------------------------------------------------------------------
//
TInt CNssVasDb::RollbackTransaction()
    {
    __ASSERT_DEBUG( iLocked == iDatabase.InTransaction(), User::Panic( KVasDbPanic, __LINE__ ) );
    __ASSERT_DEBUG( iLocked, User::Panic( KVasDbPanic, __LINE__ ) );

    TInt ret = KErrNone;

    iDatabase.Rollback();
    
    iLocked = EFalse;
    
    if ( iDatabase.IsDamaged() )
        {
        RUBY_DEBUG0( "CNssVasDb::RollbackTransaction Recovering" );
        ret = iDatabase.Recover();
        }


    RUBY_DEBUG0( "Signalling mutex in CNssVasDb::RollbackTransaction" );
    iMutex.Signal();
    iCriticalSection.Signal();
    
    return( ret );
    }
    

// -----------------------------------------------------------------------------
// CNssVasDb::LockTransactionsL
// Permit transactions and release database
// -----------------------------------------------------------------------------
//
void CNssVasDb::LockTransactionsL()
    {
    // lock transactions
    RUBY_DEBUG_BLOCK( "CNssVasDb::LockTransactionsL" );
    
    iCriticalSection.Wait(); // wait until a possible transaction ends
            
    // release a database
    CloseDatabase();
    }

// -----------------------------------------------------------------------------
// CNssVasDb::UnlockTransactionsL
// Allow transactions and reserve database
// -----------------------------------------------------------------------------
//
void CNssVasDb::UnlockTransactionsL()
    {
    // unlock transactions
    RUBY_DEBUG_BLOCK( "CNssVasDb::UnlockTransactionsL" );
    OpenDatabaseL(); // open database
    
    if ( iCriticalSection.IsBlocked() )
        {        
        iCriticalSection.Signal(); // allow to do transactions
        }
    }
    	
// -----------------------------------------------------------------------------
// CNssVasDb::ReserveDiskSpaceL(
// Reserves disk space. Leaves (KErrDiskFull), if not
// enough space.
// -----------------------------------------------------------------------------
//
void CNssVasDb::ReserveDiskSpaceL( TInt aRequestSize )
    {
    RUBY_DEBUG_BLOCK( "CNssVasDb::ReserveDiskSpaceL" );

#ifdef _DEBUG
    TInt error = iDbSession.ReserveDriveSpace( iDrive, aRequestSize );
    if ( error )
        {
        RUBY_DEBUG1( "CNssVasDb::ReserveDiskSpaceL fails %d", error );
        User::Leave( error );
        }
#else
    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, aRequestSize ) );
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CNssVasDb::ReleaseDiskSpace
// Releases the disk space
// -----------------------------------------------------------------------------
//
void CNssVasDb::ReleaseDiskSpace()
    {
    RUBY_DEBUG0( "CNssVasDb::ReleaseDiskSpace" );
    iDbSession.FreeReservedSpace( iDrive );
    }

// -----------------------------------------------------------------------------
// CNssVasDb::CreateRollbackItemLC
// Creates a cleanup item and pushes it to the cleanup stack. In case of a 
// leave, PopAndDestroying this item will call RollbackCleanupFunction
// -----------------------------------------------------------------------------
void CNssVasDb::CreatePushRollbackItemLC() 
    {
    TCleanupItem item( RollbackCleanupFunction, this );
    CleanupStack::PushL( item );
    }

// End of file
