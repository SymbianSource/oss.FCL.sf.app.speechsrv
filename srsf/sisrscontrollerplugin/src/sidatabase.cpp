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
* Description:  This class implements the SI Controller Plugin database.
*
*/


// INCLUDE FILES
#include "sidatabase.h"
#include "rubydebug.h"
#include "nssbackupobserver.h"


// CONSTANTS
// Estimate size in bytes for an empty database
// const TInt KSizeEstimateDb = 650;

// Plugin database format string for secure DBMS
_LIT( KPluginDatabaseFormatString, "SECURE[10201AFF]" );

// Name of the DB lock mutex
_LIT( KLockMutex, "SIDATABASE" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIDatabase::CSIDatabase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIDatabase::CSIDatabase( TFileName aDatabaseFileName )
                        : iOpenTransaction( EFalse ),
                          iDatabaseFileName( aDatabaseFileName ),
                          iDatabaseOpen( EFalse )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CSIDatabase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIDatabase::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CSIDatabase::ConstructL" );
    // Checks to see if the Database already exists.  If not, it creates one.
    User::LeaveIfError( iDbSession.Connect() );
    CreateDatabaseL();
    OpenDatabaseL();
    
    TInt err = iMutex.OpenGlobal( KLockMutex );
    if ( err != KErrNone )
        {
        RUBY_DEBUG0( "CSIDatabase::ConstructL Creating new global mutex" );
        iMutex.CreateGlobal( KLockMutex );
        }
    else
        {
        RUBY_DEBUG0( "CSIDatabase::ConstructL Using existing global mutex" );
        }
        
    iCriticalSection.CreateLocal();
    iBackupObserver = CNssBackupObserver::NewL( *this );
    
    }

// -----------------------------------------------------------------------------
// CSIDatabase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIDatabase* CSIDatabase::NewL( TFileName aSDDatabaseFileName )
    {
    RUBY_DEBUG_BLOCKL( "CSIDatabase::NewL" );
    
    CSIDatabase* self = new( ELeave ) CSIDatabase( aSDDatabaseFileName );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::Rollback
// Rollback all changes to the database. 
// -----------------------------------------------------------------------------
//
void CSIDatabase::Rollback()
    {
    RUBY_DEBUG0( "CSIDatabase::Rollback" );
    if ( iOpenTransaction )
        {
        iDb.Rollback();
        // Try to recover the database, if it is damaged
        if( iDb.IsDamaged() )
            {
            iDb.Recover();
            }
        iMutex.Signal();
        iCriticalSection.Signal();
        iOpenTransaction = EFalse;
        }
    
    }

// -----------------------------------------------------------------------------
// CSIDatabase::~CSIDatabase
// Destructor 
// -----------------------------------------------------------------------------
//
CSIDatabase::~CSIDatabase()
    {
    // Rollback if there are any changes not commited
    if ( iOpenTransaction )
        {
        iDb.Rollback();
        // Try to recover the database, if it is damaged
        if( iDb.IsDamaged() )
            {
            iDb.Recover();
            }
        // Signal mutex if somebody happens to be waiting in BeginTransactionL()
        iMutex.Signal();
        }
        
    delete iBackupObserver;
    iCriticalSection.Close();

	iMutex.Close();
    iDb.Close();
    iDbSession.Close();
    iDatabaseOpen = EFalse;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::BeginTransactionL
// Begins a database transaction if not already. Waits until exclusive write lock
// has been gained.
// -----------------------------------------------------------------------------
//
void CSIDatabase::BeginTransactionL()
    {
   

    if ( iOpenTransaction )
        {
        // If we already have the lock, no need to take it again
        RUBY_DEBUG0( "CSIDatabase::BeginTransactionL DB is already locked by current process" );
        return;
        }
    
    // Take mutex to ensure that only one process tries to gain the lock
    iMutex.Wait();
    iCriticalSection.Wait();
    
    // Taking the global mutex makes it sure that this should succeed every time
    TInt readLockErr = iDb.Begin();

    if ( readLockErr == KErrNone )
        {
        iOpenTransaction = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CSIDatabase::CommitChangesL
// Commits all changes to the database.
// -----------------------------------------------------------------------------
//
void CSIDatabase::CommitChangesL( TBool aCompact )
    {
    if ( iOpenTransaction )
        {
        TInt err = iDb.Commit();
        
        RUBY_DEBUG1( "CSIDatabase::CommitChangesL iDb.Commit() [%d]", err );
        
        if ( err != KErrNone )
            {
            iDb.Rollback();
            // Try to recover the database, if it is damaged
            if( iDb.IsDamaged() )
                {
                iDb.Recover();
                }
                
            iMutex.Signal();
            iCriticalSection.Signal();  
            iOpenTransaction = EFalse;
            User::Leave( err );
            }
        
        if ( aCompact )
            {
            RUBY_DEBUG0( "CSIDatabase::CommitChangesL compacting DB" );
            TInt error = iDb.Compact();
            if ( error != KErrNone )
                {
                RUBY_DEBUG1( "CSIDatabase::CommitChangesL compact error: [%d]", error );
                }
            }
        
        // Signal waiting BeginTransactionL()
        iMutex.Signal();
        iCriticalSection.Signal();  
        iOpenTransaction = EFalse;
        
        User::LeaveIfError( err );
        }
        
          
    }

// -----------------------------------------------------------------------------
// CSIDatabase::DoIDTablesExistL
// Checks to see if ID tables exist.
// -----------------------------------------------------------------------------
//
TBool CSIDatabase::DoIDTablesExistL()
    {
    // Get db table names list
    CDbTableNames* table_names = iDb.TableNamesL();
    TInt count = table_names->Count();
    delete table_names;
    // 1 is for the lock table!
    return ( count > 1 );
    }

// -----------------------------------------------------------------------------
// CSIDatabase::DoesLockTableExistL
// Checks to see if lock table exists
// -----------------------------------------------------------------------------
//
TBool CSIDatabase::DoesLockTableExistL()
    {
    // Get db table names list
    CDbTableNames* table_names = iDb.TableNamesL();
    TInt count = table_names->Count();

    for ( TInt counter = 0 ; counter < count ; counter++ )
        {
        if ( (*table_names)[counter] == _L( "writelocktable" ) )
            {
            delete table_names;
            return ETrue;
            }
        }

    delete table_names;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::DoesModelBankTableExistL
// Checks to see if model bank table exists
// -----------------------------------------------------------------------------
//
TBool CSIDatabase::DoesModelBankTableExistL()
    {
    // Get db table names list
    CDbTableNames* table_names = iDb.TableNamesL();
    TInt count = table_names->Count();

    for ( TInt counter = 0 ; counter < count ; counter++ )
        {
        if ( (*table_names)[counter] == _L( "siModelBankIdTable" ) )
            {
            delete table_names;
            return ETrue;
            }
        }

    delete table_names;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::DoesLexiconTableExistL
// Checks if lexicon table exists
// -----------------------------------------------------------------------------
//
TBool CSIDatabase::DoesLexiconTableExistL()
    {
    // Get db table names list
    CDbTableNames* table_names = iDb.TableNamesL();
    TInt count = table_names->Count();

    for ( TInt counter = 0 ; counter < count ; counter++ )
        {
        if ( (*table_names)[counter] == _L( "siLexiconIdTable" ) )
            {
            delete table_names;
            return ETrue;
            }
        }

    delete table_names;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::DoesGrammarTableExistL
// Checks if grammar table exists
// -----------------------------------------------------------------------------
//
TBool CSIDatabase::DoesGrammarTableExistL()
    {
    // Get db table names list
    CDbTableNames* table_names = iDb.TableNamesL();
    TInt count = table_names->Count();

    for ( TInt counter = 0 ; counter < count ; counter++ )
        {
        if ( (*table_names)[counter] == _L( "siGrammarIdTable" ) )
            {
            delete table_names;
            return ETrue;
            }
        }

    delete table_names;
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CSIDatabase::SIDatabase
// Returns a reference to the database.
// -----------------------------------------------------------------------------
//
RDbNamedDatabase& CSIDatabase::SIDatabase()
    {
    return iDb;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::CreateDatabaseL
// Creates a database file if one does not exist yet.
// -----------------------------------------------------------------------------
//
void CSIDatabase::CreateDatabaseL()
    {
    RUBY_DEBUG_BLOCKL( "CSIDatabase::CreateDatabaseL" );
    
    // Get the drive of the database file
    iDatabaseDrive = EDriveC;
    if ( iDatabaseFileName.Locate( 'c' ) == 0 || iDatabaseFileName.Locate( 'C' ) == 0 )
        {
        iDatabaseDrive = EDriveC;
        }
    else if ( iDatabaseFileName.Locate( 'd' ) == 0 || iDatabaseFileName.Locate( 'D' ) == 0 )
        {
        iDatabaseDrive = EDriveD;
        }
    else if ( iDatabaseFileName.Locate( 'z' ) == 0 || iDatabaseFileName.Locate( 'Z' ) == 0 )
        {
        iDatabaseDrive = EDriveZ;
        }
    
    else if ( iDatabaseFileName.Locate( 'e' ) == 0 || iDatabaseFileName.Locate( 'E' ) == 0 )
        {
        iDatabaseDrive = EDriveE;
        }
    else
        {
        User::Leave( KErrPathNotFound );
        }
    
    RDbNamedDatabase database;
    TInt ret = database.Create( iDbSession, iDatabaseFileName, KPluginDatabaseFormatString );
    if ( ret == KErrAlreadyExists )
        {
        // Ignore KErrAlreadyExists, we will use the existing one if it is there
        ret = KErrNone;
        }
    database.Close();
    User::LeaveIfError( ret );
    }

// -----------------------------------------------------------------------------
// CSIDatabase::OpenDatabaseL
// Opens the database.
// -----------------------------------------------------------------------------
//
void CSIDatabase::OpenDatabaseL()
    {
    if ( !iDatabaseOpen )
        {
        
        User::LeaveIfError( iDb.Open( iDbSession, iDatabaseFileName, KPluginDatabaseFormatString ) );
        
        // Try to recover the database, if it is damaged
        if( iDb.IsDamaged() )
            {
            RUBY_DEBUG0( "CSIDatabase::OpenDatabaseL - Db is damaged. Attempt to recover." );
            
            User::LeaveIfError( iDb.Recover() );
            }
        
        iDatabaseOpen = ETrue;
        }
    }
    
// -----------------------------------------------------------------------------
// CSIDatabase::CloseDatabase
// Closes the database.
// -----------------------------------------------------------------------------
//
TInt CSIDatabase::CloseDatabase()
    {
    RUBY_DEBUG0( "CSIDatabase::CloseDatabase" ); 
  
    iDb.Close();
    iDatabaseOpen = EFalse;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::DbSession
// Return reference to DB session.
// -----------------------------------------------------------------------------
//
RDbs& CSIDatabase::DbSession()
    {
    return iDbSession;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::DbDrive
// Returns a drive for DB file.
// -----------------------------------------------------------------------------
//
TInt CSIDatabase::DbDrive()
    {
    return iDatabaseDrive;
    }

// -----------------------------------------------------------------------------
// CSIDatabase::CreateLockTableL
// Creates a dummy table to gain write-lock at the beginning of each transaction.
// -----------------------------------------------------------------------------
//
void CSIDatabase::CreateLockTableL()
    {
    // Create writelocktable
    // This table is accessed every time when transaction starts to gain
    // exclusive locking of the DB
    _LIT( KWriteLockTable, "writelocktable" );
    
    _LIT( KCol1, "temporary" );
    
    OpenDatabaseL();
    
    // Create the rrd tables definition (columnset).
    CDbColSet* columns = CDbColSet::NewLC();
    
    // The tag id 
    TDbCol dbCol1( KCol1, EDbColUint32 );
    columns->AddL( dbCol1 );
    
    User::LeaveIfError( iDb.CreateTable( KWriteLockTable, *columns ) );
    
    // Cleanup columns
    CleanupStack::PopAndDestroy( columns ); 
    }
    
// -----------------------------------------------------------------------------
// CSIDatabase::LockTransactionsL
// Permit transactions and release database
// -----------------------------------------------------------------------------
//
void CSIDatabase::LockTransactionsL()
    {
    // lock transactions
    RUBY_DEBUG_BLOCK( "CSIDatabase::LockTransactionsL" );
    
    iCriticalSection.Wait(); // wait until a possible transaction ends
            
    // release a database
    CloseDatabase();
    }

// -----------------------------------------------------------------------------
// CSIDatabase::UnlockTransactionsL
// Allow transactions and reserve database
// -----------------------------------------------------------------------------
//
void CSIDatabase::UnlockTransactionsL()
    {
    // unlock transactions
    RUBY_DEBUG_BLOCK( "CSIDatabase::UnlockTransactionsL" );
    OpenDatabaseL(); // open database
    
    if ( iCriticalSection.IsBlocked() )
        {        
        iCriticalSection.Signal(); // allow to do transactions
        }
    }

//  End of File
