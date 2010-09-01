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
* Description:     Vocabulary pruning functions for speech recognition
*
*/






#ifndef ASRSVOCMANHWDEVICE_H
#define ASRSVOCMANHWDEVICE_H

//  INCLUDES
#include <asrshwdevice.h>
#include <nsssispeechrecognitiondatadevasr.h>

// FORWARD DECLARATIONS
class CVocManAlgorithm;

// CLASS DECLARATION

/**
*  Class for pruning
*  When the grammar starts to get big, this class can prune
*  least used rule variants away before adding new rules.
*
*  @lib AsrsVocmanHwDevice.dll
*  @since ?Series60_version
*/
class CASRSVocManHwDevice : public CASRSHwDevice
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CASRSVocManHwDevice* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CASRSVocManHwDevice();

    public: // Functions from base classes

        /**
        * From CASRHwDevice
        * Gets a custom interface
        * @since 2.8
        * @param aInterfaceUid Uid of the interface.
        * @return Pointer to the interface. Ownership depends on UID.
        */
        TAny* CustomInterface(TUid aInterfaceUid);

        /**
        * From CASRHwDevice
        * Initializes the HW Device
        * @since 2.8
        * @return None
        */
        void InitializeL();

        /**
        * From CASRHwDevice
        * Clear HW Device's allocated memory
        * @since 2.8
        * @return None
        */
        void Clear();

    public: // New functions
        
        /**
        * Use to adapt rule after recognition
        * @since 2.6
        * @param aCorrectRuleVariantID Correctly recognized rule variant
		* @param aRule Rule including correct variant
        */
        IMPORT_C void AdaptL( const TSIRuleVariantID aCorrectRuleVariantID, 
								    CSIRule& aRule );

        /**
        * Use to prune grammar.
        * @since 2.6
        * @param aGrammar Grammar to be pruned
		* @param aMinNumber Minimum number of items to be pruned
		* @param aPrunedItems Information which items can be removed
		* @return ETrue pruning successful, EFalse cannot be pruned
        */
		IMPORT_C TBool Prune( const CSIGrammar& aGrammar, 
							  TUint32 aMinNumber,
							  RArray<TSIRuleVariantInfo>& aPrunedItems);

        /**
        * Use to determine number of new rule variants
        * @since 2.6
        * @param aGrammar Grammar where new variants should be added to
		* @param aTargetNRuleVariants Target number of rule variants in the grammar
		* @param aMaxNRuleVariants Maximum number of rule variants in the grammar
		* @param aNewRuleScores Score for each new rule
		* @param aNNRuleVariants Number of rule variants for each rule. 
		*        Array must be initialized as maximum number of rule variants per a rule.
		* @return None
        */
		IMPORT_C void ComputeNRuleVariantsL( const CSIGrammar& aGrammar, 
											 const TUint32 aTargetNRuleVariants, 
											 const TUint32 aMaxNRuleVariants, 
											 const RArray<TUint>& aNewRuleScores, 
											 RArray<TUint>& aNNewRuleVariants, 
											 TUint32& aNPrune );
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
        CASRSVocManHwDevice();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CASRVocManHwDevice( const CASRVocManHwDevice& );
        // Prohibit assigment operator if not deriving from CBase.
        // CASRVocManHwDevice& operator=( const CASRVocManHwDevice& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        CVocManAlgorithm* iVMAlgorithm;
         
        // Reserved pointer for future extension
        TAny* iReserved;

    };

#endif      // ASRSVOCMANHWDEVICE_H
            
// End of File
