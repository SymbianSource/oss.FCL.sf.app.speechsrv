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
* Description:  Utility class for SI Plugin to read data from file.
*
*/


#ifndef ASRPLUGINDATALOADER_H
#define ASRPLUGINDATALOADER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "srsfbldvariant.hrh"


// FORWARD DECLARATION
class MSysConfigProvider;

// CLASS DECLARATION

/**
* Class for SI Plugin to load language package file.
*
*  @lib SIControllerPlugin.lib
*  @since 2.8
*/
class CDataLoader : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDataLoader* NewL( const TDesC& aPrefixOfFileName,
                                  const TDesC& aSeparator,
                                  const TDesC& aPostfixOfFileName,
                                  MSysConfigProvider *aProvider = NULL );
        
        /**
        * Destructor.
        */
        virtual ~CDataLoader();
        
    public: // New functions
        
       /**
		* Load language package file. It resovles the name of the langage package
		*
		* @since 2.8
		* @param  aPackageType   Type identifier.
		* @param  aPackageID     Identifier of package.
		* @param  aStartPosition Starting position within package in bytes.
		* @param  aEndPosition   Ending position within package in bytes.
		* @return Pointer to created buffer containing the request data.
        *         NULL if error occurs during loading.	
		*/
        HBufC8* LoadData( TUint32 aPackageType, 
                          TUint32 aPackageID,
                          TUint32 aStartPosition ,
                          TUint32 aEndPosition );

    protected:

       /**
		* Actual file loading function.
		*
		* @since 2.8
		* @param  aFile			  Data file name
		* @param  aStartPosition  Starting position within package in bytes.
		* @param  aEndPosition    Ending position within package in bytes.
		* @return Pointer to created buffer containing the request data.
		* 		  NULL if error occurs during loading.	
		*/
        HBufC8* DoLoadL( TFileName& aFile,
                         TUint32 aStartPosition, 
                         TUint32 aEndPosition );

    private:
        
        /**
        * C++ default constructor.
        */
        CDataLoader();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aPrefixOfFileName,
						 const TDesC& aSeparator,                                     
                         const TDesC& aPostfixOfFileName,
                         MSysConfigProvider *aProvider);

		/**
		* Helper function for creating data file names
		* @param  aFileName	      TFileName where to write the result
		* @param  aPackageType    Type identifier.
		* @param  aPackageID      Identifier of package.
		*/
		void PackageName (TFileName &aFileName, TUint32 aPackageType, TUint32 aPackageID);


#ifdef __SIND_AUTOCONF_LID
		/**
		* Special loader for language identification data
		* @return Pointer to the loaded data
		*         NULL if error occurs during loading
		*/ 
		HBufC8* ConstructLidDataL();


		/**
		* Loads a file to the end of a buffer
		* @param  aBuffer         The buffer where to load the data
		* @param  aPackageType    Type identifier.
		* @param  aPackageID      Identifier of package.
		*/
		void AppendLoadL( CBufBase *aBuffer, TUint32 aPackageType, TUint32 aPackageID );


		/**
		* Obtains the list of installed languages in the system and loads the corresponding data files
		* to the buffer.
		* @param  aBuffer         The buffer where to load the data
		*/
		void SysLangPackagesToBufferL( CBufBase *aBuffer );

#endif  // __SIND_AUTOCONF_LID

        
    private: // Data
        
        // seperator
        HBufC* iSeparator ;
        
        // Prefix of file name 
        HBufC* iPrefixOfFileName;
        
        // Postfix of file name
        HBufC* iPostfixOfFileName;
        
        // File session
        RFs iFs;

        // Provides language configuration
        MSysConfigProvider *iProvider;
    };
    
#endif // ASRPLUGINDATALOADER_H
// End of File
