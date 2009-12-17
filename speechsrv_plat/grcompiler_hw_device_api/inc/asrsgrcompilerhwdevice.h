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
* Description:     ARM hardware device for grammar compiling
*
*/






#ifndef ASRSGRCOMPILERHWDEVICE_H
#define ASRSGRCOMPILERHWDEVICE_H

//  INCLUDES
#include <asrshwdevice.h>
#include <nsssispeechrecognitiondatadevasr.h>

// FORWARD DECLARATIONS
class CAsrsGrCompilerAlgorithm;
class CSDCompiledGrammar; // For future extensions; the class doesn't exist yet.
class CSDLexicon;
class CSDModelBank;
class CSDResultSet;

// CLASS DECLARATION
/**
*  Callback class for Grammar Compiler.
*
*  @lib asrgrcompilerhwdevice.lib
*  @since 2.8
*/
class MASRSGrCompilerHwDeviceObserver
    {
public: // New functions

    /**
    * Called after grammar compilation has finished or stopped.
    * @since 2.8
    * @param aError KErrNone or a system-wide error code
    * @return none
    */
    virtual void MghdoGrammarCompilerComplete(TInt aError) = 0;

    /**
    * Called after grammar combination has finished or stopped.
    * @since 2.8
    * @param aResult The compiled grammar. Format dependes on the technology provider.
    * @param aError KErrNone or a system-wide error code
    * @return none
    */
    virtual void MghdoGrammarCombinerComplete(HBufC8* aResult, TInt aError) = 0;

    /**
    * Combination and compilation operations require lexicons. To provide those
    * lexicons when needed, the client must implement these functions.
    * CASRGrCompilerHwDevice takes ownership of the data.
    * @since 2.8
    * @param aID The lexicon identifier
    * @return The lexicon. CASRGrCompiler takes ownership.
    */
    virtual CSILexicon* MghdoSILexiconL(TSILexiconID aID) = 0;

    /**
    * Callback function to load configuration data.
    * Client provides an implementation.
    *
    * @param aPackageType Type identifier.
    * @param aPackageID Package identifier.
    * @param aStartPosition First byte.
    * @param aEndPosition Last byte. If greater than the size of 
    *                               the data, rest of the data is given.
    * @return Pointer to the data buffer, HW Device takes ownership.
    */	
    virtual HBufC8* MghdoConfigurationData( TUint32 aPackageType,
                                            TUint32 aPackageID,
                                            TUint32 aStartPosition = 0,
                                            TUint32 aEndPosition = KMaxTUint32 ) = 0;

    };

/**
*  Grammar compilation algortithms
*
*  @lib asrgrcompilerhwdevice.lib
*  @since 2.8
*/
class CASRSGrCompilerHwDevice : public CASRSHwDevice
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CASRSGrCompilerHwDevice* NewL(
            MASRSGrCompilerHwDeviceObserver& aObserver
            );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CASRSGrCompilerHwDevice();

    public: // New functions

		/**
        * Compiles a speaker independent grammar. Asynchronous.
        * Callback is MghdoGrammarCompilerComplete.
        * @since 2.8
        * @param aSICompiledGrammar The grammar to be compiled.
        * @return none
        */
        IMPORT_C void CompileGrammarL( CSICompiledGrammar& aSICompiledGrammar );

		/**
        * Combines several speaker independent grammars. The result is a combined grammar
        * in internal data format.
        * If some rules are excluded (blacklisted), then they are absent from the combined
        * grammar, but the compiled grammars are left untouched.
        * @since 2.8
        * @param aCompiledGrammars Array of previously compiled grammar
        * @param aExcludedRules    Rules to be blacklisted.
        * @return none
        */
        IMPORT_C void CombineGrammarL(const RPointerArray<CSICompiledGrammar>& aCompiledGrammars,
                                      const RPointerArray<TSIRuleVariantInfo>& aExcludedRules);

		/**
        * Cancels grammar compilation. No callback is sent to the client.
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void CancelCompilation();

		/**
        * Cancels grammar combination. No callback is sent to the client.
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void CancelCombination();

		/**
        * Decodes an N-Best list from internal format into a recognition result.
        * @since 2.8
        * @param aNBestIDs     - N-Best result, as it came from the Recognition HW Device
        * @param aSIResultSet  - Empty recognition result to be populated.
        * @param aSICompiledGrammar - The grammars, which were combined to get aCombinedGrammar
        * @param aCombinedData - The combined grammar, which was used in recognition
        * @return none
        */
        IMPORT_C void ResolveResult(const RArray<TUint>& aNBestIDs,
                                    CSIResultSet& aSIResultSet,
                                    const RPointerArray<CSICompiledGrammar>& aSICompiledGrammar,
                                    const TDesC8& aCombinedData
                                    ) const;

    public: // Functions from base classes

		/**
        * From CASRSHwDevice
        * Retrieve a custom interface
        * @since 2.8
        * @param aInterfaceId - UID of the interface
        * @return Pointer to a custom interface
        */
        IMPORT_C TAny* CustomInterface(TUid aInterfaceId);

		/**
        * From CASRSHwDevice
        * Initializes the device. Subsequent calls reset the device.
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void InitializeL();

		/**
        * From CASRSHwDevice
        * Clears the device.
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void ClearL();
        
    private:

        /**
        * C++ default constructor.
        */
        CASRSGrCompilerHwDevice(MASRSGrCompilerHwDeviceObserver& aObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        CASRSGrCompilerHwDevice( const CASRSGrCompilerHwDevice& );
        // Prohibit assigment operator if not deriving from CBase.
        CASRSGrCompilerHwDevice& operator=( const CASRSGrCompilerHwDevice& );

    private:    // Data

        // The observer
        MASRSGrCompilerHwDeviceObserver& iObserver;

        // Implementation-specific functions and data
        CAsrsGrCompilerAlgorithm *iAlgorithm;

        // Has Initialize() been called
        TBool iIsInitialised;

        // Reserved pointer for future extension.
        TAny* iReserved;
    };

#endif      // ASRSGRCOMPILERHWDEVICE_H
            
// End of File
