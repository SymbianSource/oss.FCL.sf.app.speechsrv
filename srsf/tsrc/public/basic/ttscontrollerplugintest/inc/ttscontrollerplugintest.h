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


#ifndef TTSCONTROLLERPLUGINTEST_H
#define TTSCONTROLLERPLUGINTEST_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>

#include <mdaaudiosampleplayer.h>


// MACROS

// Logging path
_LIT( KttscontrollerplugintestLogPath, "\\logs\\testframework\\ttscontrollerplugintest\\" ); 
// Log file
_LIT( KttscontrollerplugintestLogFile, "ttscontrollerplugintest.txt" ); 
_LIT( KttscontrollerplugintestLogFileWithTitle, "ttscontrollerplugintest_[%S].txt" );


// FORWARD DECLARATIONS
class Cttscontrollerplugintest;


// CLASS DECLARATION

/**
*
*/
NONSHARABLE_CLASS(Cttscontrollerplugintest) : public CScriptBase, 
                                              public MMdaAudioPlayerCallback
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cttscontrollerplugintest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cttscontrollerplugintest();

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

    private:

        /**
        * C++ default constructor.
        */
        Cttscontrollerplugintest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */
        virtual TInt CreateL();
        virtual TInt CreateFilePlayerL      ( CStifItemParser& aItem );
        virtual TInt CreateDesPlayerL       ( CStifItemParser& aItem );
        virtual TInt OpenFileL              ( CStifItemParser& aItem );
        virtual TInt OpenFileHandleL        ( CStifItemParser& aItem );
        virtual TInt OpenDesL               ( CStifItemParser& aItem );
        virtual TInt Play();
        virtual TInt SetVolumeAndRampL();
        virtual TInt Destroy();

    private:
        
        CMdaAudioPlayerUtility*     iAudioPlayer;
        
        RFs                         iFs;
        RFile                       iFile;
        
        HBufC8*                     iInputText;
        
    };

#endif      // TTSCONTROLLERPLUGINTEST_H

// End of File
