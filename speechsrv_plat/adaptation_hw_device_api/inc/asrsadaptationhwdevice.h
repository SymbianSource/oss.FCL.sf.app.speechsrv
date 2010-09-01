/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     ARM hardware device for grammar compiling
*
*/






#ifndef ASRADAPTATIONHWDEVICE_H
#define ASRADAPTATIONHWDEVICE_H

//  INCLUDES
#include <asrshwdevice.h>
#include <nsssispeechrecognitiondatadevasr.h>

// Implementation
class CAsrAdaptationAlgorithm;
 
// CLASS DECLARATION
/**
*  Callback class for adaptation.
*
*  !!@lib asrsadaptationhwdevice.lib
*  @since 2.8
*/
class MASRAdaptationHwDeviceObserver
    {
public:
	    /**!!!
        *  
        * @since 2.8
        * @param none
        * @return  
        */
     
	virtual void MaahdAdaptationComplete(TInt aError) = 0; 
};



/**
*  Adaptation class 
*
*  !!@lib asrsadaptationhwdevice.lib
*  @since 2.8
*/
class  CASRSAdaptHwDevice : public CASRSHwDevice
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CASRSAdaptHwDevice* NewL(         
         MASRAdaptationHwDeviceObserver& aObserver  );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CASRSAdaptHwDevice();

    public: // New functions

		/**
        * Retrieve a custom interface
        * @since 2.8
        * @param none
        * @return Pointer to a custom interface
        */
        IMPORT_C TAny* CustomInterface(TUid aInterfaceId);

		/**
        * Initializes the device. Subsequent calls reset the device.
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void InitializeL();

		/**
        * Clears the device.
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void Clear();

	 
		/**
        * Start the adaptation.  It will adapt the acoustic models given the feature vector set and phoneme sequences 
		* of a correctly recognized utterance
        * @since 2.8
 		* @param 		aAdaptationData: Buffer that stores data(feature vector) for the speaker adaptation.
		* @param 		aModelBank: Model bank, which is the acoustic model to be adapted.
		* @param 		aPronunciation: Pronunciation of the adaptation utterance.
		* @param 		aLanguage : adaptation language
        * @return		none
        */
        IMPORT_C void StartAdaptationL(const TDesC8& aAdaptationData, 
										CSIModelBank& aModelBank, 
										  CSIPronunciation& aPronunciation,TLanguage aLanguage);
		
	 

	 	/**
        * Cancel the Adaptation process. No callback is sent to the client.
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void CancelAdaptation();

		/**
        * Cancels adaptation. No callback is sent to the client.
        * @since 2.8
        * @param none
        * @return none
        */
/*        IMPORT_C void CancelCombination();*/
 
    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
         CASRSAdaptHwDevice(MASRAdaptationHwDeviceObserver& anObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


		
       
        // Prohibit copy constructor if not deriving from CBase.
        // CASRGrCompilerHwDevice( const CASRGrCompilerHwDevice& );
        // Prohibit assigment operator if not deriving from CBase.
        // CASRGrCompilerHwDevice& operator=( const CASRGrCompilerHwDevice& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // pronunciation tree builder algorithm
        // CPTBAlgorithm* iPtb;

        // The observer
        MASRAdaptationHwDeviceObserver& iObserver;

        // Implementation-specific functions and data
        CAsrAdaptationAlgorithm *iAlgorithm;
 
        // Has Initialize() been called
        TBool iIsInitialised;

        // Reserved pointer for future extension.
        TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
#endif      // ?INCLUDE_H   
            
// End of File
