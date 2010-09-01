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
* Description:     Algorithms for text normalization
*
*/





#ifndef NLPHWDEVICE_H
#define NLPHWDEVICE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <nssttscommon.h>
#include <asrshwdevice.h>

// FORWARD DECLARATIONS
class CNlpAlgorithm;

// CLASS DECLARATION
/**
*  Class to define callback functions
*
*  @lib asrsttshwdevice.lib
*  @since 3.0
*/
class MNlpHwDeviceObserver
    { 
    public: // New functions

        /**
        * Called by the HW device when the asynchronous text processing 
        * has been completed.
        * @since 3.0
        * @param aStatus Error code, KErrNone if success
        * @return none
        */
        virtual void MnhdoNlpCompleted( TInt aStatus ) = 0;


        /**
        * Invoked by Nlp Hw Device when it needs a configuration data package.
        * @since 3.0
        * @param aPackageType Type identifier. 
        * @param aPackageID Identifier of package
        * @param aStartPosition Start index in bytes
        * @param aEndPosition End index in bytes. If the position is bigger 
        *         than the size of data package, the end part will be returned.
        * @return Data package in a buffer
        */
        virtual HBufC8* MnhdoConfigurationData( TUint32 aPackageType, 
                                                TUint32 aPackageID, 
                                                TUint32 aStartPosition = 0, 
                                                TUint32 aEndPosition = KMaxTUint32 
                                              ) = 0;
                                              

        /**
        * Returns style given the style id.
        * @since 3.0
        * @param aStyleID The style id.
        * @return A reference to TTtsStyle.
        */
        virtual const TTtsStyle& MnhdoStyleL( TTtsStyleID aStyleID ) = 0; 

    };


/**
*  Class to provide text normalization
*
*  @lib asrsnlphwdevice.lib
*  @since 3.0
*/
class CNlpHwDevice : public CASRSHwDevice
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aObserver Observer which implements callback functions.
        * @param aSamplingRate Sampling rate for output audio
        * @param aBufferSize Buffer size for output audio
        */
        IMPORT_C static CNlpHwDevice* NewL( MNlpHwDeviceObserver& aObserver );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNlpHwDevice();

    public: // New functions

        /**
        * Ask if a language is supported or not.
        * @since 3.0
        * @param aLanguage Synthesizing language.
        * @return ETrue language is supported otherwise EFalse
        */
        IMPORT_C TBool IsLanguageSupported( TLanguage aLanguage );

	
		/**
        * Normalize given text, text is not splits into segments.
        * @since 3.0
        * @param aText 
        */
        IMPORT_C void NormalizeTextL( CTtsParsedText& aText );


		/**
        * Normalize given text asynchronously, text is not splits into segments.
        * @since 3.0
        * @param aText 
        */
        IMPORT_C void NormalizeTextAsyncL( CTtsParsedText& aText );

    
        /**
        * Normalize given text, text is split into segments
        * @since 3.0
        * @param aText 
        */      
       	IMPORT_C void NormalizeAndSegmentTextL( CTtsParsedText& aText );
       	
        /**
        * Normalize given text asynchronously, text is split into segments
        * @since 3.0
        * @param aText 
        */      
       	IMPORT_C void NormalizeAndSegmentTextAsyncL( CTtsParsedText& aText );
       
    private:

        /**
        * C++ default constructor.
        */
        CNlpHwDevice();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MNlpHwDeviceObserver& aObserver );

    private: // Data

		// Actual algorithm where all processing happens
		CNlpAlgorithm* iAlgorithm;

		// Reserved pointer for future extension
        TAny* iReserved;

    };

#endif // NLPHWDEVICE_H   
            
// End of File
