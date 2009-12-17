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
* Description:  This class reads the resource file and provides access to the
*				 information read from the resource file.
*
*/


#ifndef CTTSPLUGINRESOURCEHANDLER_H
#define CTTSPLUGINRESOURCEHANDLER_H

// CLASS DECLARATION

/**
*  This class encapsulates resource file handling functions
*
*  @lib TtsPlugin.lib
*  @since 2.8
*/
class CTtsPluginResourceHandler : public CBase
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTtsPluginResourceHandler* NewL( RFs& aFs );

        /**
        * Destructor.
        */
        virtual ~CTtsPluginResourceHandler();

    private:

        /**
        * C++ default constructor.
        */
        CTtsPluginResourceHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RFs& aFs );

        /**
        * Reads the data from the resource file.
        */
		void ReadResourceFileL( RFs& aFs );

	public: // Data

        // General TTP filename
        HBufC* iTtpGeneralFilename;

        // Language TTP filename
        HBufC* iTtpLanguangeFilename;

        // Postfix of TTP files
        HBufC* iTtpFilenamePostfix;

        // Name of TTS files
        HBufC* iTtsFilename;

        // Postfix of TTS files
        HBufC* iTtsFilenamePostfix;
	};

#endif // CTTSPLUGINRESOURCEHANDLER_H

// End of File
