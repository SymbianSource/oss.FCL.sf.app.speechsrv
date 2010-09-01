/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef TTSUTILITYTEST_H
#define TTSUTILITYTEST_h

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#include <nssttsutility.h>


// MACROS

// Logging path
_LIT( KttsutilitytestLogPath, "\\logs\\testframework\\ttsutilitytest\\" ); 
// Log file
_LIT( KttsutilitytestLogFile, "ttsutilitytest.txt" ); 
_LIT( KttsutilitytestLogFileWithTitle, "ttsutilitytest_[%S].txt" );


// FORWARD DECLARATIONS
class Cttsutilitytest;


// CLASS DECLARATION

/**
*
*/
NONSHARABLE_CLASS(Cttsutilitytest) : public CScriptBase, 
                                              public MTtsClientUtilityObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cttsutilitytest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cttsutilitytest();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private: 
    
        void MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds &aDuration );
        
        void MapcPlayComplete( TInt aError );
        
        void MapcCustomCommandEvent( TInt /*aEvent*/, TInt /*aError*/ ) {};

    private:

        /**
        * C++ default constructor.
        */
        Cttsutilitytest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */
        virtual TInt CreateTtsUtilityL();
        virtual TInt DestroyTtsUtilityL();
        
        virtual TInt StyleSettingsL();
        virtual TInt PlaybackPropertiesL();
        
        virtual TInt Play();
        virtual TInt Pause();
        virtual TInt Stop();
        
        virtual TInt OpenDesL       ( CStifItemParser& aItem );
        virtual TInt OpenDes8L      ( CStifItemParser& aItem );
        virtual TInt OpenFileL      ( CStifItemParser& aItem );
        virtual TInt OpenParsedTextL( CStifItemParser& aItem );
        
        virtual TInt OpenAndPlayDesL       ( CStifItemParser& aItem );
        virtual TInt OpenAndPlayDes8L      ( CStifItemParser& aItem );
        virtual TInt OpenAndPlayFileL      ( CStifItemParser& aItem );
        virtual TInt OpenAndPlayParsedTextL( CStifItemParser& aItem );
        
        virtual TInt SynthesisToFileL( CStifItemParser& aItem );
        
        virtual TInt CustomCommandsL();
        virtual TInt CustomCommands2L();
        
        /*
        * Other functions
        */ 
        TUid OpenPluginL();

    private:
        
        CTtsUtility*                iTtsUtility;
        CTtsParsedText*             iParsedText;
        TTtsSegment                 iSegment; 
        TTtsStyle                   iStyle;
        
        RFs                         iFs;
        RFile                       iFile;

    };

#endif      // TTSUTILITYTEST_h

// End of File
