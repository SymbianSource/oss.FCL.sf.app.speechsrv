/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef TESTAPPENGINE_H
#define TESTAPPENGINE_H

// INCLUDES

#include <e32base.h>
#include <speechsynthesis.h>

#include "TestAppAppUi.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* 
*/
class CTestAppEngine : public CActive
    {
    public: // Constructors and destructor

        static CTestAppEngine* NewL( CTestAppAppUi& aAppUi );

        virtual ~CTestAppEngine();

        void ConnectL();
        void DisconnectL();
        
        void InitTextL();
        void InitTextToFileL();
        
        void PlayL();
        void PauseL();
        void StopL();
        
        TVoice ChangeVoiceL();
        
        TInt VolumeDownL();
        TInt VolumeUpL();
        
        TInt RateUpL();
        
        void ChangeAudioOutputL();

    protected: // From CActive
    
       /**
        * CActive::RunL
        */
        void RunL();

        /**
        * CActive::DoCancel()
        */
        void DoCancel();
                
    private:    
    
        CTestAppEngine( CTestAppAppUi& aAppUi );

        void ConstructL();
                
        void FetchSupportedLanguagesAndVoicesL();
                
    private: // Data

        enum TState
            {
            EIdle,
            EPriming,
            ESpeaking
            };

        CTestAppAppUi&              iAppUi;
        
        RSpeechSynthesis            iServer;
        TState                      iState; 
        
        TVoice                      iVoice;
        
        RArray<TVoice>              iVoices;
        TInt                        iVoiceIndex;
        
        TInt                        iRate; 
        TInt                        iVolume;
        
        TFileName                   iFileName;
        
        TInt                        iCurrentAudioOutput;
        
    };

#endif

// End of File
