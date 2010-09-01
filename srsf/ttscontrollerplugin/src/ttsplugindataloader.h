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
* Description:  Utility class for TTS Plugin Body to read TTP data from file.
*
*/


#ifndef TTSPLUGINDATALOADER_H
#define TTSPLUGINDATALOADER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "srsfbldvariant.hrh"

// CLASS DECLARATION

/**
* Class to load TTP Data
*
* @lib TTSAlgorithm.lib
*/
class CTTSDataLoader : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTTSDataLoader* NewL( RFs& iFs, 
                                     TFileName& aGeneralFileName,
                                     TFileName& aPrefixOfTTPFileName,
                                     TFileName& aPostfixOfTTPFilename,
                                     TFileName& aTTSFilename,
                                     TFileName& aPostfixOfTTSFilename );
        
        /**
        * Destructor.
        */
        virtual ~CTTSDataLoader();
        
    public: // New functions
        
        /**
        * Function to load general configuration data.
        *
        * @param "TInt aDataID" Identifier of data package.
        * @return Pointer to created buffer, NULL if not found.
        */	
        HBufC8* LoadGeneralData( const TInt aPackageType, 
                                 const TInt aDataID,
                                 TUint32 aStartPosition, 
                                 TUint32 aEndPosition );
        
        /**
        * Function to load configuration data of TTP method .
        * 
        * @param "TLanguage aLanguage" Language specifying which data will be loaded
        * @return Pointer to created buffer, NULL if not found.
        */	
        HBufC8* LoadLanguageData( const TInt aPackageType, 
                                  TLanguage aLanguage,
                                  TUint32 aStartPosition, 
                                  TUint32 aEndPosition );

        /**
        * Function to load TTS configuration data
        */
        HBufC8* LoadTtsData( const TInt aPackageType, 
                             const TInt aDataID,
                             TUint32 aStartPosition, 
                             TUint32 aEndPosition );
        
        /**
        * Function to add supported languages to aLanguages
        */
        void GetSupportedLanguagesL( RArray<TLanguage>& aLanguages );
        
    protected:

        /**
        * Accesses the file.
        */
        HBufC8* DoLoadL( TFileName& iFile,
                         TUint32 aStartPosition, 
                         TUint32 aEndPosition );

    private:
        
        /**
        * C++ default constructor.
        */
        CTTSDataLoader( RFs& iFs, 
                        TFileName& aGeneralFileName,
                        TFileName& aPrefixOfTTPFileName,
                        TFileName& aPostfixOfTTPFilename,
                        TFileName& aTTSFilename,
                        TFileName& aPostfixOfTTSFilename );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Helper function to add supported languages to aLanguages
        */
        void DoGetSupportedLanguagesL( RArray<TLanguage>& aLanguages,
                                       TFileName& aMask );
                                       
        /**
        * Helper function to create file name
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
        
        // File name for general data
        TFileName iGeneralFileName;
        
        // Prefix of file name for ttp method data
        TFileName iPrefixOfTTPFileName;
        
        // Postfix of file name for ttp method data
        TFileName iPostfixOfTTPFileName;
        
        // TTS filename
        TFileName iTTSFileName;

        // Postfix of file name for tts data
        TFileName iPostfixOfTTSFileName;

        // File session
        RFs& iFs;

    };
    
#endif // TTSPLUGINDATALOADER_H

// End of File
