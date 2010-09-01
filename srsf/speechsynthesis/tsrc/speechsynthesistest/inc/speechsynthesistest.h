/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


#ifndef SPEECHSYNTHESISTEST_H
#define SPEECHSYNTHESISTEST_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#include "speechsynthesistester.h"

// MACROS

// Logging path
_LIT( KSpeechSynthesisTestLogPath, "\\logs\\testframework\\SpeechSynthesisTest\\" ); 
// Log file
_LIT( KSpeechSynthesisTestLogFile, "SpeechSynthesisTest.txt" ); 



/**
*  CSpeechSynthesisTest test class for STIF Test Framework TestScripter.
*/
NONSHARABLE_CLASS( CSpeechSynthesisTest ) : public CScriptBase, 
                                            public MTesterObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSpeechSynthesisTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSpeechSynthesisTest();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:
    
        // Callbacks from iTester
        void SynthesisInitComplete( TInt aError );    
        void SynthesisComplete( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CSpeechSynthesisTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        */
        void Delete();

        /**
        * Test methods 
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt OpenL( CStifItemParser& aItem );
        virtual TInt CloseL( CStifItemParser& aItem );
        
        virtual TInt SynthesisInitL( CStifItemParser& aItem );
        virtual TInt SynthesisInitFileL( CStifItemParser& aItem );
        virtual TInt SynthesiseL( CStifItemParser& aItem );
        virtual TInt StopL( CStifItemParser& aItem );
        virtual TInt PauseL( CStifItemParser& aItem );
        
        virtual TInt GetSupportedLanguagesL( CStifItemParser& aItem );
        
        virtual TInt GetSupportedVoicesL( CStifItemParser& aItem );
        virtual TInt VoiceL( CStifItemParser& aItem );
        virtual TInt SetVoiceL( CStifItemParser& aItem );
        
        virtual TInt MaxVolumeL( CStifItemParser& aItem );
        virtual TInt VolumeL( CStifItemParser& aItem );
        virtual TInt SetVolumeL( CStifItemParser& aItem );
        
        virtual TInt MaxSpeakingRateL( CStifItemParser& aItem );
        virtual TInt SpeakingRateL( CStifItemParser& aItem );
        virtual TInt SetSpeakingRateL( CStifItemParser& aItem );

        virtual TInt SetAudioPriorityL( CStifItemParser& aItem );
        virtual TInt CustomCommandL( CStifItemParser& aItem );
        
        TInt ReadExpectedResultL( CStifItemParser& aItem );
        void CheckResultL( TInt aExpectedResult, TInt aResult );

    private:
        
        CSpeechSynthesisTester* iTester;
        
        TBuf<2048> iText; 
        
        TInt iExpectedResult;
    };

#endif      // SPEECHSYNTHESISTEST_H

// End of File
