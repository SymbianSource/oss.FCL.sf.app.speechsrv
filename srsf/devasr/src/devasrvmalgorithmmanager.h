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
* Description:  Algorithm manager for grammar handling algorihtms.
*
*/


#ifndef DEVASRVMALGORITHMMANAGER_H
#define DEVASRVMALGORITHMMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <nsssispeechrecognitiondatadevasr.h>
#include <asrsgrcompilerhwdevice.h>
#include <asrsvocmanhwdevice.h>
#include <asrsttphwdevice.h>

//#include "DevASRSrsAlgorithmManager.h"

// DATA TYPES
enum TTtpHwState
    {
    ETtpStateUninitialized = 0,
    ETtpStateIdle,
    ETtpStateBusy
    };

enum TGrCompilerState
    {
    EGrStateUninitialized = 0,
    EGrStateIdle,
    EGrStateBusyCompiling,
    EGrStateBusyCombining
    };


// FORWARD DECLARATIONS
class MDevASRObserver;
class MSDGrammarCompiler;
//class CASRGrCompilerHwDevice;
//class CASRSVocManHwDevice;
//class CASRSTtpHwDevice;

// CLASS DECLARATIONS

/**
* Observer mixin.
*
* @lib NssDevAsr.lib
* @since 2.8
*/
class MVmAlgMgrObserver
    {
    public:
        /**
        * Called when grammars have been combined.
        */
        virtual void CombineComplete( HBufC8* aResult, TInt aError ) = 0;

        /**
        * Called by CVMAlgorithmManager when it needs a grammar object.
        * 
        * @param "TSIGrammarID aGrammarID" Grammar identifier which is needed.
        * @param "CSICompiledGrammar** aSIActiveGrammar" Output parameter which will
        *        contain the grammar object pointer if grammar is currently active.
        * @param "CSICompiledGrammar** aSIDeActivatedGrammar" Output parameter which will
        *        contain the grammar object pointer if grammar is currently not active.
        */
        virtual void GetGrammarL( const TSIGrammarID aGrammarID, 
                                  CSICompiledGrammar** aSIActiveGrammar,
                                  CSICompiledGrammar** aSIDeActivatedGrammar ) = 0;
    };

/**
* Algorithm manager to handle VocMan, Ttp and Grammar Compiler
*
* @lib NssDevAsr.lib
* @since 2.8
*/
class CVMAlgorithmManager : public CBase, public MASRSTtpHwDeviceObserver, public MASRSGrCompilerHwDeviceObserver
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVMAlgorithmManager* NewL( MDevASRObserver& aObserver, MVmAlgMgrObserver& aVmObserver );
        
        /**
        * Destructor.
        */
        virtual ~CVMAlgorithmManager();
        
    public: // New functions
        

        /**
        * Use to adapt rule after recognition.
        *
        * @since 2.8
        * @param "CSIResultSet& aResultSet" Result set.
        * @param "TInt aResultIndex" Index of correct result.
        */
        void AdaptVocabL( const CSIResultSet& aResultSet, 
                          TInt aResultIndex );

        /**
        * Cancels training.
        *
        * @since 2.8
        */
        void Cancel();
        
        /**
        * Computes number of rule variants for grammar.
        *
        * @since 2.8
        * @param "CSIGrammar& aGrammar" A grammar where to add new rules.
        * @param "TUint32 aTargetNRuleVariants" Preferable target number of 
        *        all rule variants after adding new rule variants
        * @param "TUint32 aMaxNRuleVariants" Maximum number of all rule variants.
        * @param "RArray<TUint>& aNewRuleScores" Scores for each new rule. The 
        *         size of array tells also how many	new rules will be added. 
        *         Based on the scores some rules can get more rule variants 
        *         than others. If there is no scores available, the array 
        *         should initialize with constant values.
        * @param "RArray<TUint>& aNNeNRuleVariants" Array where to add number 
        *         of new rule variants for new rules. The order is equivalent 
        *         to aNewRuleScores.
        * @param "TUint32& aNPrune" Number of rule variants which should be 
        *         pruned before adding new rule variants.
        */
        void ComputeNewGrammarSizeL( const CSIGrammar& aGrammar, 
                                     const TUint32 aTargetNRuleVariants, 
                                     const TUint32 aMaxNRuleVariants, 
                                     const RArray<TUint>& aNewRuleScores, 
                                     RArray<TUint>& aNNeNRuleVariants, 
                                     TUint32& aNPrune );
        

        /**
        * Compiles SI or SD grammar to internal format.
        *
        * @since 2.8
        * @param "aGrammar" Reference to grammar object.
        * @param "aModelBank" Reference to model bank.
        */
        void CompileGrammarL( CSICompiledGrammar& aGrammar );

        /**
        * Invoked when grammar combining is needed
        *
        * @since 2.8
        * @param "RPointerArray<CSICompiledGrammar>& aCompiledGrammars" 
        *        Array of previously compiled grammar
        * @param "const RPointerArray<TSIRuleVariantInfo>& aExcludedRules" 
        *        Rules to be blacklisted.
        */
        void CombineGrammarL( const RPointerArray<CSICompiledGrammar>& aCompiledGrammars,
                              const RPointerArray<TSIRuleVariantInfo>& aExcludedRules );


        /**
        * Determines which rule variants can be removed.
        *
        * @since 2.8
        * @param "CSIGrammar& aGrammar" A grammar to be pruned.
        * @param "TUint32 aMinNumber" Minimum number of pruned items
        * @param "RArray<TSIRuleVariantInfo> aPrunedRuleVariants" A array where 
        *         to put information which variants can be pruned.
        */
        TBool PruneGrammar( const CSIGrammar& aGrammar, 
                            const TUint32 aMinNumber, 
                            RArray<TSIRuleVariantInfo>& aPrunedRuleVariants );
        
        /**
        * Resolves result using Grammar Compiler.
        *
        * @since 2.8
        * @param "RArray<TUint>& aNBestIDs" List of N-best results
        * @param "
        */
        void ResolveResult( const RArray<TUint>& aNBestIDs,
                            CSIResultSet& aSIResultSet,
                            const RPointerArray<CSICompiledGrammar>& aSICompiledGrammar,
                            const TDesC8& aCombinedData );

        /**
        * Starts training from text.
        *
        * @since 2.8
        * @param "CSITtpWordList& aWordList" Text for training.
        * @param "RArray<TLanguage>& aDefaultLanguage" Default language for 
        *        pronunciations.
        * @param "RArray<TUint32>& aMaxNPronunsForWord" Maximum number of 
        *        pronunciation per word.
        */
        void TrainFromTextL( CSITtpWordList& aWordList, 
                             const RArray<TLanguage>& aDefaultLanguage, 
                             const RArray<TUint32>& aMaxNPronunsForWord );

		/**
        * Retreive the properties of the engines.
        *
        * @since 2.0
        * @param "RArray<TInt>& aPropertyId" An array of identifiers being querried.
        * @param "RArray<TInt>& aPropertyValue" An array of values corresponding
        *        to the querried identifiers.
        */
        void GetEnginePropertiesL( const RArray<TInt>& aPropertyId, 
                                   RArray<TInt>& aPropertyValue );

        /**
        * Load the specified parameter(s) to the engines. 
        *
        * @param "RArray<TInt>& aParameterId" An array of parameter identifiers.
        * @param "RArray<TInt>& aParameterValue" An array of parameter values.
        */
        void LoadEnginePropertiesL( const RArray<TInt>& aParameterId,
                                    const RArray<TInt>& aParameterValue );

        
    public: // From MASRTtpHwDeviceObserver
        
        /**
        * Callback function to load configuration data.
        *
        * @param "TUint32 aPackageType" Type identifier.
        * @param "TUint32 aPackageID" Package identifier.
        * @param "TUint32 aStartPosition" First byte.
        * @param "TUint32 aEndPosition" Last byte. If greater than the size of 
        *                               the data, rest of the data is given.
        * @return Pointer to the data buffer, TTP HW Device takes ownership.
        */	
        HBufC8* MathdoConfigurationData( TUint32 aPackageType, TUint32 aPackageID,
                                         TUint32 aStartPosition = 0,
                                         TUint32 aEndPosition = KMaxTUint32 );

		/**
        * Callback function to notify that word list is converted.
        *
        * @param "TInt aError" Error code, KErrNone if successful.
        */	
		void MathdoWordListReady( const TInt aError );

    public: // From MASRSGrCompilerObserver

        /**
        * Grammar has been compiled.
        *
        * @param "TInt aError" An error code.
        */
        void MghdoGrammarCompilerComplete( TInt aError );

        /**
        * Grammars have been combined.
        *
        * @param "HBufC8* aResult" Contains the compiled grammar, NULL if error
        *         happened.
        * @param "TInt aError" KErrNone if successful.
        */
        void MghdoGrammarCombinerComplete( HBufC8* aResult, TInt aError );

        /**
        * Called when GrCompilerHwDevice needs an SI lexicon.
        *
        * @param "TSILexiconID anID" Requested lexicon id.
        * @return Pointer to created lexicon.
        */
        CSILexicon* MghdoSILexiconL( TSILexiconID anID );

        /**
        * Called when GrCompiler needs configuration data package
        */
        HBufC8* MghdoConfigurationData( TUint32 aPackageType,
                                        TUint32 aPackageID,
                                        TUint32 aStartPosition = 0,
                                        TUint32 aEndPosition = KMaxTUint32 );
             
    private:
        
/*        void DeleteData( TPtrC8& aData );*/
        
        /**
        * C++ default constructor.
        */
        CVMAlgorithmManager( MDevASRObserver& aObserver, MVmAlgMgrObserver& aVmObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // Prohibit copy constructor
        CVMAlgorithmManager( const CVMAlgorithmManager& );
        // Prohibit assigment operator
        CVMAlgorithmManager& operator=( const CVMAlgorithmManager& );
        
    private: // Data
        // Grammar compiler HW device
        CASRSGrCompilerHwDevice* iGrCompiler;
        
        // Vocabulary management HW device
        CASRSVocManHwDevice* iVocMan;
        
        // TTP HW device
        CASRSTtpHwDevice* iTtp;
        
        // Observer
        MDevASRObserver& iObserver;
        
        // Array for data pointers
//        RPointerArray<HBufC8> iDataArray;
        
        // TtpHwDevice state
        TTtpHwState iTtpState;

        // GrCompilerHwDeviceState
        TGrCompilerState iGrState;

        // Our own observer
        MVmAlgMgrObserver& iVmAlgObserver;
       
    };
    
#endif // DEVASRVMALGORITHMMANAGER_H
    
// End of File
