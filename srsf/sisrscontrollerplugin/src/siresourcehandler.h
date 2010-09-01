/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class reads the resource file and provides access to the
*				 information read from the resource file to the controller.
*
*/



#ifndef CSIRESOURCEHANDLER_H
#define CSIRESOURCEHANDLER_H

// CLASS DECLARATION
/**
*  This class implements resource file handling functions.
*
*  @lib SIControllerPlugin.lib
*  @since 2.8
*/
class CSIResourceHandler : public CBase
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIResourceHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CSIResourceHandler();

    private:

        /**
        * C++ default constructor.
        */
        CSIResourceHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Reads the data from the resource file.
        * @since 2.8
        * @param	-
        * @return	-
        */
		void ReadResourceFileL();

	public:    // Data
 
		// Maximum number of models per bank
		TInt iMaxModelsPerBank;

		// Maximum number of models in system
		TInt iMaxModelsSystem;

		// Plugin database file name 
		HBufC* iDbFileName;
		// Language package data file name information

		// Language package data file path and prefix
		HBufC* iDataFilenamePrefix;	  

		// Language package data file postfix
		HBufC* iDataFilenamePostfix;	  

		// Language package data file name seperator
		HBufC* iDataFilenameSeperator; 
	}; 
#endif      // CSIRESOURCEHANDLER_H
// End of File
