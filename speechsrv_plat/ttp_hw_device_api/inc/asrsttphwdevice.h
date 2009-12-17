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
* Description:     Exported interface towards TTP HW Device.
*
*/





#ifndef ASRSTTPHWDEVICE_H
#define ASRSTTPHWDEVICE_H

// INCLUDES
// Base class for all HW Devices
#include <asrshwdevice.h>
// SI Data
#include <nsssispeechrecognitiondatadevasr.h>

// FORWARD DECLARATIONS
class CTTPAlgorithm;
class CTtpCallback;


// CLASS DECLARATION

/**
* Class which defines callback functions. Client must implement this in order
* to receive callbacks.
*
* @lib asrsttphwdevice.lib
*/
class MASRSTtpHwDeviceObserver 
    {
    public: // New functions
        
        /**
        * Callback function to load configuration data.
        * Client provides an implementation.
        *
        * @param aPackageType Type identifier.
        * @param aPackageID Package identifier.
        * @param aStartPosition First byte.
        * @param aEndPosition Last byte. If greater than the size of 
        *                     the data, rest of the data is given.
        * @return Pointer to the data buffer, TTP HW Device takes ownership.
        */	
        virtual HBufC8* MathdoConfigurationData( TUint32 aPackageType, TUint32 aPackageID,
                                                 TUint32 aStartPosition = 0,
                                                 TUint32 aEndPosition = KMaxTUint32 ) = 0;

		/**
        * Callback function to notify that word list is converted.
        * Client provides an implementation.
        *
        * @param aError Error code, KErrNone if successful.
        */	
		virtual void MathdoWordListReady( const TInt aError ) = 0;
	};

/**
* TTP HW Device interface.
*
* @lib asrsttphwdevice.lib
*/
class CASRSTtpHwDevice : public CASRSHwDevice
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aObserver Reference to callback observer.
        */
        IMPORT_C static CASRSTtpHwDevice* NewL( MASRSTtpHwDeviceObserver& aObserver );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CASRSTtpHwDevice();

    public: // New functions
        
        /**
        * Use to convert text to phonemes.
        *
        * @param aText Text to be converted
		* @param aLanguage Used language
        * @return Phoneme sequence, client gains ownership
        */
		IMPORT_C HBufC8* ConvertTextL( const TDesC& aText, 
							           TLanguage& aLanguage );

		/**
        * Use to convert list of words to pronunciation sequences.
        * 
        * @param aWordList Initialized word list to where 
        *        pronunciations will be added. Client will keep the ownership
        *        of CSITtpWordList also after this call.
		* @param aDefaultLanguage Language which overrides the first identified language. 
        *        Iflanguage is ELangOther, language identification will be used.
    	* @param aMaxNPronunsForWord Maximum number of pronunciations (languages) for each word.
        */
		IMPORT_C void ConvertWordListL( CSITtpWordList& aWordList,
								        const RArray<TLanguage> aDefaultLanguage,
								        const RArray<TUint32> aMaxNPronunsForWord );
		
        /**
        * Use to cancel running word list conversion.
        */
		IMPORT_C void CancelWordListConversion();

        /**
        * Sets the phoneme notation used in output.
        *
        * @param aNotation Phoneme notation identifier.
        */
        IMPORT_C void SetPhonemeNotationL( const TDesC& aNotation );

        /**
        * Gets the current phoneme notation indentifier.
        *
        * @return Phoneme notation identifier.
        */
        IMPORT_C const TDesC& PhonemeNotation();

    public: // From CASRSHwDevice

        /**
        * From CASRSHwDevice
        * Custom interface
        *
        * @param aInterfaceId ID of wanted custom interface
        * @return A pointer to the interface implementation, NULL if not found.
        */
        IMPORT_C TAny* CustomInterface( TUid aInterfaceId );

        /**
        * From CASRSHwDevice
        * Initializes Hw Device.
        */
        IMPORT_C void InitializeL();

        /**
        * From CASRSHwDevice
        * Clears Hw Device, counter part for InitializeL()
        */
        IMPORT_C void Clear();

    public: // New methods for SIND increment 3

        /**
        * Tries to identify the language of given text
        *
        * @param "const TDesC& aText" Text
        * @param "TInt aNumberOfGuesses" Maximum number of guesses that will be provided
        * @param "RArray<TLanguage>& aLanguages" Output parameter, will contain list of guessed languages
        * @param "RArray<TInt>& aScores" Output parameter, scores
        */
        IMPORT_C void LanguageIdentificationL( const TDesC& aText,
                                               TInt aNumberOfGuesses,
                                               RArray<TLanguage>& aLanguages,
                                               RArray<TInt>& aScores );
 
    private:

        /**
        * C++ default constructor.
        * @param aObserver Reference to observer.
        */
        CASRSTtpHwDevice( MASRSTtpHwDeviceObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        CASRSTtpHwDevice( const CASRSTtpHwDevice& );
        // Prohibit assigment operator if not deriving from CBase.
        CASRSTtpHwDevice& operator=( const CASRSTtpHwDevice& );

		/**
        * Function to notify that word list is converted
        * Called by CTtpCallback.
        *
        * @param aError Error code
        */	
		void WordListReady( const TInt aError );

        /**
        * Function to notify that configuration data is needed.
        * Called by CTtpCallback.
        *
        * @param aPackageType Type identifier.
        * @param aPackageID Package identifier.
        * @param aStartPosition First byte.
        * @param aEndPosition Last byte. If greater than the size of 
        *                     the data, rest of the data is given.
        * @return Pointer to the data buffer.
        */	
        virtual HBufC8* ConfigurationData( TUint32 aPackageType, TUint32 aPackageID,
                                           TUint32 aStartPosition = 0,
                                           TUint32 aEndPosition = KMaxTUint32 );

    private: // Data
		// Observer
		MASRSTtpHwDeviceObserver& iObserver;

        // algorithm implementation
        CTTPAlgorithm* iTTPAlgorithm;
         
		// Implements call backs from CTTPAlgorithm
		CTtpCallback* iTTPAlgObserver;

        // Reserved pointer for future extension
        TAny* iReserved;

    private: // Friend classes
        friend class CTtpCallback;

    };

#endif // ASRSTTPHWDEVICE_H  
            
// End of File
