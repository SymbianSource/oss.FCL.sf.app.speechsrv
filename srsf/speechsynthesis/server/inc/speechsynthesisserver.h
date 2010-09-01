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
* Description:  Speech synthesis server
*
*/


#ifndef SPEECHSYNTHESISSERVER_H
#define SPEECHSYNTHESISSERVER_H

// INCLUDES

#include <e32base.h>
#include <nssttsutility.h>
#include <speechsynthesis.h>

#include "speechsynthesisclientserver.h"
#include "speechsynthesisserverutilities.h"


// DEFINITION OF SERVER POLICY 

// Total number of ranges
const TUint KSpeechSynthesisRangeCount = 4;

// Definition of the ranges of IPC numbers
const TInt KSpeechSynthesisRanges[KSpeechSynthesisRangeCount] = 
        {
        0,                  // Messages 0..19
        ESetAudioPriority,  // Message 20
        ESetAudioOutput,    // Message 21
        ESetAudioOutput+1   // Messages 22..
        }; 

// Policy to implement for the above range        
const TUint8 KSpeechSynthesisElementsIndex[KSpeechSynthesisRangeCount] = 
        {
        CPolicyServer::EAlwaysPass,     // Applies to 0th range
        0,                              // Applies to 1st range 
        CPolicyServer::ECustomCheck,    // Applies to 2nd range
        CPolicyServer::EAlwaysPass,     // Applies to 3rd range
        };

// Specific capability checks
const CPolicyServer::TPolicyElement KSpeechSynthesisElements[] = 
        {
        // Only clients having Nokia VID capability passes this policy element. 
        { _INIT_SECURITY_POLICY_V0( VID_DEFAULT ), CPolicyServer::EFailClient },
        };

// Package all the above together into a policy
const CPolicyServer::TPolicy KSpeechSynthesisPolicy =
        {
        CPolicyServer::EAlwaysPass,     // Specifies all connect attempts should pass
        KSpeechSynthesisRangeCount,     // Number of ranges                                   
        KSpeechSynthesisRanges,         // Ranges array
        KSpeechSynthesisElementsIndex,  // Elements<->ranges index
        KSpeechSynthesisElements,       // Array of elements
        };
    

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer
// Server implementation
// ----------------------------------------------------------------------------
class CSpeechSynthesisServer : public CPolicyServer, 
                               public MConfigurationObserver,
                               public MTtsClientUtilityObserver
    {
    
    public: // New functions
    
        /**
        * 2nd phase constructor
        */
        static CServer2* NewLC();
        
        /**
        * Destructor
        */        
        virtual ~CSpeechSynthesisServer();
                
        /**
        * Add new sessios
        */
        void AddSession();
        
        /**
        * Remove session
        */
        void DropSession();

        /**
        * Reserve server for aSession
        */
        void ReserveL( MSpeechSynthesisServerObserver* aSession );
        void ReserveL( MSpeechSynthesisServerObserver* aSession, TInt aPriority );
        
        /**
        * Release server to be used by other sessions
        */
        void Release( const MSpeechSynthesisServerObserver* aSession );
        
        /**
        * Used to ask server's default settings
        */
        void GetDefaultSettingsL( TVoice& aVoice, 
                                  TInt& aSpeakingRate, TInt& aMaxSpeakingRate,
                                  TInt& aVolume, TInt& aMaxVolume, 
                                  TInt& aAudioPriority, TInt& aAudioPreference,
                                  TInt& aAudioOutput );
        
        /**
        * Used to ask server's default voice
        */
        void GetDefaultVoiceL( TVoice& aVoice );

        /**
        * Prime the synthesizer
        *
        * @param aText Text to be synthesized
        *
        */
        void PrimeL( const TDesC& aText );
    
        /**
        * Prime the synthesizer
        *
        * @param aText Text to be synthesized
        * @param aFile Open handle to file
        *
        */     
        void PrimeL( const TDesC& aText, const RFile& aFile );
         
        /**
        * Synthesize text
        */     
        void Synthesize();
         
        /**
        * Stop synthesis
        */
        void Stop();
         
        /**
        * Pause synthesis
        */
        void Pause();
              
        /**
        * Get available languages
        *
        * @param aLanguages Contains supported languages after function call
        */
        void GetSupportedLanguagesL( RArray<TLanguage>& aLanguages );

        /**
        * Get available voices
        *
        * @param aLanguage Language
        * @param aVoices Contains voices after the function call
        */
        void GetSupportedVoicesL( TLanguage aLanguage, RArray<TVoice>& aVoices );
        
        /**
        * Change voice. 
        *
        * @param aVoice New voice
        * @param aSpeakingRate Speaking rate to be used
        */
        void SetVoiceL( const TVoice aVoice, TInt aSpeakingRate );

        /**
        * Change volume. 
        *
        * @param aVolume New volume
        */
        void SetVolume( TInt aVolume );

        /**
        * Set audio priority
        * 
        * @param aPriority New priority
        * @param aPreference New preference
        */ 
        void SetAudioPriorityL( TInt aPriority, TInt aPreference );
        
        /**
        * Set audio output
        */        
        void SetAudioOutputL( TInt aAudioOutput );
        
        /**
        * Custom command
        *
        * @param aCommand A custom command
        * @param aValue Value
        * @return Error code
        */
        TInt CustomCommand( TInt aCommand, TInt aValue );
        
        /**
        * Checks if given voice exists
        * 
        * @param aVoice Voice to be checked
        * @return ETrue if voice exists, EFalse otherwise
        */
        TBool IsVoiceValidL( const TVoice& aVoice );
        
        /*
         * Returns pointer to the previous session. Plugin is closed if it doesn't 
         * match new session in order to avoid using settings from other session. 
         */
        const MSpeechSynthesisServerObserver* PreviousSession(); 
        
        /*
         * Closes open plugin 
         */
        void ClosePlugin();

    public: // From base class MConfigurationObserver
         
        /**
        * This is called when installation or removal of SIS package is detected 
        */
        void McoConfigurationChanged();
         
    public: // From base class MTtsClientUtilityObserver

        /**
        * Called when asynchronous custom command finishes.
        */
        void MapcCustomCommandEvent( TInt aEvent, TInt aError );
        
        /**
        * Callback when initialized
        */
        void MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration );

        /**
        * Callback when playback finished
        */
        void MapcPlayComplete( TInt aError );
        
    public:
    
        /**
        * Callback function called when server has been idle certain time. 
        */
        static TInt ClosePlugin( TAny* aAny );
        
    private:

        // Constructor
        CSpeechSynthesisServer();
        
        // 2nd phase constructor
        void ConstructL();
        
        // Create new session
        CSession2* NewSessionL( const TVersion& aVersion, 
                                const RMessage2& aMessage ) const;
        
        // Custom access policy check
        TCustomResult CustomSecurityCheckL( const RMessage2& aMsg, 
                                            TInt& aAction, 
                                            TSecurityInfo& aMissing );
        
        // Updates configuration if needed
        void UpdateSynthesisConfigurationIfNeededL();

        // Updates configuration 
        void UpdateSynthesisConfigurationL();
        
    private:

        // Pointer to active session observer, not owned. 
        MSpeechSynthesisServerObserver* iSession;
        
        // Pointer to previous session observer, not owned. 
        MSpeechSynthesisServerObserver* iPreviousSession;
        
        // Pointer to tts, owned. 
        CTtsUtility*                    iTtsUtility; 
        
        // Object used to stop the server when it has been idle for a while. 
        // Owned
        CShutdown*                      iShutdown; 
        
        // Object for closing tts plugin when server has been idle some time.
        // Frees memory and lets tts components to be removed. 
        CPeriodic*                      iPluginCloser;
        
        // SIS package installer listener, owned. 
        CConfigurationListener*         iConfigurationListener;
        
        // MMC status listener, owned. 
        CMMCListener*                   iMMCListener;
        
        // Supported voice configurations
        RArray<TVoiceConfiguration>     iTtsVoiceConfigs;

        // Uid of open tts plugin
        TUid                            iOpenTtsPlugin;
                
        // Number of sessions using this server
        TInt                            iSessionCount;
        
        // 
        TInt                            iSessionPriority;
        
        // Default settings
        TVoice                          iDefaultVoice;
        TInt                            iDefaultSpeakingRate;
        TInt                            iDefaultVolume;
        TInt                            iDefaultMaxVolume;
        TInt                            iDefaultAudioPriority;
        TInt                            iDefaultAudioPreference;
        
        // If ETrue updates to voice configuration and 
        // default settings are needed. 
        TBool                           iUpdateNeeded;
    };

#endif // SPEECHSYNTHESISSERVER_H
