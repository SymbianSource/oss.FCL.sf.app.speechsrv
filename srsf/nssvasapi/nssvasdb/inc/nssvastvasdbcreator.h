/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef TNSSVASDBCREATOR_H
#define TNSSVASDBCREATOR_H

#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>
#include <s32file.h>
#include <d32dbms.h>

#include "nssvasdbkonsts.h"
#include "nssvascvasdbsrvdef.h"
// CLASS DECLARATION

/**
*
*  The TNssVasDbCreator class definition. 
* 
*  @lib VASInternal.lib
*  @since 2.0
*/
class TNssVasDbCreator
{
public:
	
	/**
    * Constructor 
    * @since 2.0
    * @param 
    * @return 
    */
	TNssVasDbCreator();

	/**
    * Method to create vasdatabase.db and its tables 
    * @since 2.0
    * @param "RDbs& iDbSession" Database session handle
    * @return none
    */
	void CreateVasDatabaseL( RDbs& iDbSession );

private:
    /**
    * Creates the tables and indices of the database
    */
	void CreateTablesAndIndiciesL( RDbNamedDatabase& aDatabase );

    /**
    * Creates the context table
    */
	void CreateContextTableL( RDbNamedDatabase& aDatabase );

    /**
    * Creates the index of the context table
    */
	void CreateContextIndexL( RDbNamedDatabase& aDatabase );

    /**
    * Creates the tag table
    */
    void CreateTagTableL( RDbNamedDatabase& aDatabase );

    /**
    * Creates the index of the tag table
    */
	void CreateTagIndexL( RDbNamedDatabase& aDatabase );

    /**
    * Creates the RRD table
    */
    void CreateRRDTableL( RDbNamedDatabase& aDatabase );
    
    /**
    * Creates indeices for RRD tables
    */
    void CreateRRDIndexL( RDbNamedDatabase& aDatabase );

    /**
    * Creates the write lock table
    */
    void CreateWriteLockTableL( RDbNamedDatabase& aDatabase );
    
    /**
    * Creates index for a given table and column. Leaves if creation fails.
    * Leave codes are the same as error codes in RDbNamedDatabase::CreateIndex + 
    * general out of memory
    * @param anIndexName Name of the index to create
    * @param aColumnName Name of the column to index on
    * @param aUnique ETrue if index should be unique, EFalse otherwise
    * @param aTableName Name of the table for which the index should be created
    */
    void CreateSingleColumnIndexL( RDbNamedDatabase& aDatabase, const TDesC& anIndexName,
                 const TDesC& aColumnName, const TBool aUnique, const TDesC& aTableName);
    
    /**
    * Creates index for a given table and columns. Leaves if creation fails.
    * Leave codes are the same as error codes in RDbNamedDatabase::CreateIndex + 
    * general out of memory
    * @param anIndexName Name of the index to create
    * @param aFirstColumnName Name of the first column to index on
    * @param aSecondColumnName Name of the second column to index on
    * @param aUnique ETrue if index should be unique, EFalse otherwise
    * @param aTableName Name of the table for which the index should be created
    */
    void CreateTwoColumnIndexL( const TDesC& anIndexName, 
                                const TDesC& aFirstColumnName, 
                                const TDesC& aSecondColumnName, 
                                const TBool aUnique, const TDesC& aTableName);

};

#endif // TVASDBCREATOR_H
