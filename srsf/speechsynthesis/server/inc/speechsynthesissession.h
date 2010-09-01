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
* Description:  Speech synthesis session
*
*/


#ifndef SPEECHSYNTHESISSESSION_H
#define SPEECHSYNTHESISSESSION_H

// INCLUDES

#include <e32base.h>
#include <f32file.h> 

#include "speechsynthesisserverutilities.h"


// FORWARD DECLARATIONS

class CSpeechSynthesisServer;

// Structure for saving custom command - value pairs 
class TCustomCommand
    {
    public:
        TInt iCommand;
        TInt iValue;
    };


// ----------------------------------------------------------------------------
// CSpeechSynthesisSession
// Provides synthesis session functionality
// ----------------------------------------------------------------------------
class CSpeechSynthesisSession : public CSession2, 
                                public MSpeechSynthesisServerObserver
    {

    public:
        
        /**
        * Constructor
        */
        CSpeechSynthesisSession();

        /**
        * Destructor
        */
        virtual ~CSpeechSynthesisSession();
        
        /**
        * 2nd phase construct for sessions - called by the CServer framework
        */    
        void CreateL();
    
    public: // From base class MSpeechSynthesisServerObserver

        /**
        * Callback when initialized
        */
        void MsssInitComplete( TInt aError, 
                               const TTimeIntervalMicroSeconds& aDuration );

        /**
        * Callback when playback finished
        */        
        void MsssPlayComplete( TInt aError );
        
        /**
        * Callback when operation aborted.
        */
        void MsssOperationAborted(); 

    private:
 
        // Leaves if access is denied
        void CheckAccessL( TInt aMessageType );
        
        // Returns reference to the server
        inline CSpeechSynthesisServer& Server();
        
        // Provides services
        void ServiceL( const RMessage2& aMessage );
        
        // Handles asynchronous services
        void DoAsyncServiceL( const RMessage2& aMessage );
        
        // Handles synchronous services
        void DoSyncServiceL( const RMessage2& aMessage );
        
        // Fuctions to provide actual services
        void DoStopSynthesisL();
        void DoPauseSynthesisL();
        void DoGetDurationL(      const RMessage2& aMessage );
        void DoGetLanguagesL(     const RMessage2& aMessage );
        void DoGetLanguageCountL( const RMessage2& aMessage );
        void DoGetVoicesL(        const RMessage2& aMessage );
        void DoGetVoiceCountL(    const RMessage2& aMessage );
        void DoVoiceL(            const RMessage2& aMessage );
        void DoSetVoiceL(         const RMessage2& aMessage );
        void DoMaxSpeakingRateL(  const RMessage2& aMessage );
        void DoSpeakingRateL(     const RMessage2& aMessage );
        void DoSetSpeakingRateL(  const RMessage2& aMessage );
        void DoMaxVolumeL(        const RMessage2& aMessage );
        void DoVolumeL(           const RMessage2& aMessage );
        void DoSetVolumeL(        const RMessage2& aMessage );
        void DoSetAudioPriority(  const RMessage2& aMessage );
        void DoSetAudioOutputL(   const RMessage2& aMessage );
        void DoCustomCommandL(    const RMessage2& aMessage );
        
        // Handles leave happened in ServiceL()
        void ServiceError( const RMessage2& aMessage, TInt aError );
 
    private:

        // Session state information values
        enum TSessionState
            {
            ESessionIdle,
            ESessionPrimed
            };

        // Asynchronous messages has to be kept in mind
        RMessage2                   iStoredMessage;
        
        // Duration information filled in MsssInitComplete()
        TTimeIntervalMicroSeconds   iDuration;
        
        // Current voice in use
        TVoice                      iCurrentVoice;

        // Current speking rate
        TInt                        iSpeakingRate;

        // Maximum value of speaking rate
        TInt                        iMaxSpeakingRate;
        
        // Current volume
        TInt                        iVolume;
        TInt                        iTmpVolume;
        
        // Maximum value of volume setting
        TInt                        iMaxVolume;
        
        // Current audio priority
        TInt                        iAudioPriority;
        
        // Current audio preference
        TInt                        iAudioPreference;
        
        // Current audio output
        TInt                        iAudioOutput;
        TInt                        iTmpAudioOutput;
        
        // Session state information
        TSessionState               iSessionState;
        
        // Used when client is asking supported languages. 
        RArray<TLanguage>           iLanguages; 
        
        // Used when client is asking supported voices. 
        RArray<TVoice>              iVoices;
        
        // Array for custom commands
        RArray<TCustomCommand>     iCustomCommands;
        
        // File handle used for saving output to file. 
        RFile iFile;
        
        // ETrue when file output is activated 
        TBool                       iFileOutputActivated;
        
        // ETrue when closing of a plugin has been requested
        TBool                       iClosePlugin;
        
    };

#endif // SPEECHSYNTHESISSESSION_H
