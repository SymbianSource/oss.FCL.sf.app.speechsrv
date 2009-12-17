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
* Description:  This class implements the SI Controller Plugin database.
*
*/



#ifndef SIDATABASE_H
#define SIDATABASE_H

//  INCLUDES
#include <d32dbms.h>
#include <e32svr.h>
#include <sysutil.h> // Critical Level check
#include <f32file.h>


class CNssBackupObserver;

// CLASS DECLARATION

/**
*  This class is the base class for all SI Controller Plugin DB classes.
*  It provides the common functionality for the DB classes.
*
*  @lib SIControllerPlugin.lib
*  @since 2.8
*/
class CSIDatabase : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIDatabase* NewL(TFileName aDatabaseFileName);

 		/**
        * Destructor.
        */
        virtual ~CSIDatabase();

    public: // New functions

        /**
        * Begins a database transaction if not already.
        * @since 2.8
        * @return	Reference to the database.
        */
		void BeginTransactionL();

        /**
        * Commits all changes to the database.
        * @since 2.8
        */
		void CommitChangesL( TBool aCompact );

        /**
        * Rollback all changes to the database.
        * @since 2.8
        */
		void Rollback();

        /**
        * Checks to see if ID tables exist.
        * @since 2.8
        * @return	ETrue if ID tables exist; EFalse otherwise.
        */
		TBool DoIDTablesExistL();

        /**
        * @todo Add comments
        */
        TBool DoesLockTableExistL();

        /**
        * @todo Add comments
        */
        TBool DoesModelBankTableExistL();

        /**
        * @todo Add comments
        */
        TBool DoesLexiconTableExistL();

        /**
        * @todo Add comments
        */
        TBool DoesGrammarTableExistL();


        /**
        * Returns a reference to the database.
        * @since 2.8
        * @return	Reference to the database.
        */
		RDbNamedDatabase& SIDatabase();

        /**
        * Returns a reference to DB session.
        * @since 2.8
        * @return	Reference to the database.
        */
		RDbs& DbSession();

        /**
        * Returns a drive for DB file.
        * @since 2.8
        * @return	Reference to the session.
        */
		TInt DbDrive();

        /**
        * Creates a small table to the database. Writing of this table is tried
        * when acquiring for a write lock.
        */
        void CreateLockTableL();

		/**
		* Permit transactions. Used when e.g. backup or restore begins 
		* @since 3.1
		*/
		void LockTransactionsL();

		/**
		* Allow transactions. Used when e.g. backup or restore ends 
		* @since 3.1
		*/
		void UnlockTransactionsL();
		
        /**
        * Opens a database session.  If database file doesn't exist, it creates one.
        * @since 2.8
        */
        void OpenDatabaseL();
        
        /**
	    * Close database.
	    * @since 3.1
	    * @return Symbian-wide error codes
	    */
		TInt  CloseDatabase();
		
     private:

        /**
        * C++ default constructor.
        */
        CSIDatabase(TFileName aDbFileName);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates a new database file.
        * @since 2.8
        */
		void CreateDatabaseL();




    public:    // Data

		// Database session
		RDbs iDbSession;

		// Database
        RDbNamedDatabase iDb;

		// Indication for outstanding transaction by this client
		TBool iOpenTransaction;

		// Name of the database file	
		TFileName iDatabaseFileName;

        // drive for the database file
        TInt iDatabaseDrive;

        // Indication if database connection is open
        TBool iDatabaseOpen;

        // Mutex for DB write lock
        // Makes sure that only one process gets the DB lock
        RMutex iMutex; 
        
        // Critical section is used in locking transactions inside one object
        RCriticalSection iCriticalSection;
        
        // backup and restore observer
        CNssBackupObserver* iBackupObserver;
        
    };

#endif // SIDATABASE_H

// End of File
