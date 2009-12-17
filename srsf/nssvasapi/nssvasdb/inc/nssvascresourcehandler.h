/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Reads the VAS resource file and keeps the information in
*               member variables for later use.
*
*/


#ifndef NSSVASCRESOURCEHANDLER_H
#define NSSVASCRESOURCEHANDLER_H

#include <e32base.h>

// CLASS DECLARATION

/**
*  This class encapsulates resource file handling functions
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
NONSHARABLE_CLASS( CNssVasResourceHandler ) : public CBase
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNssVasResourceHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CNssVasResourceHandler();

    private:

        /**
        * C++ default constructor.
        */
        CNssVasResourceHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Reads the data from the resource file.
        */
		void ReadResourceFileL();

	public: // Data

        // Path of the database
        HBufC* iDatabasePath;

        // Database filename
        HBufC* iDatabaseName;
	};

#endif // NSSVASCRESOURCEHANDLER_H

// End of File
