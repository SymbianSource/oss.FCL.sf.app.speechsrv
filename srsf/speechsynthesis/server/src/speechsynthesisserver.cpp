/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDES

#include <AudioOutput.h>
#include <nssttscommon.h>

#include "speechsynthesisserver.h"

#include "speechsynthesisclientserver.h"
#include "speechsynthesisserverutilities.h"
#include "speechsynthesissession.h"

#include "rubydebug.h"


// CONSTANTS

// Time to wait before tts plugin is closed if no activity detected. 
const TTimeIntervalMicroSeconds32 KTtsPluginCloseWait = 30000000; // 30 sec.

// ----------------------------------------------------------------------------
// RunServerL
// Server startup code
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
// ----------------------------------------------------------------------------
//  
static void RunServerL()
    {
    RUBY_DEBUG_BLOCK( "SpeechSynthesisServer: RunServerL" );
    
    // Naming the server thread after the server helps to debug panics
    User::LeaveIfError( User::RenameThread( KSpeechSynthesisServerName ) );

    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    
    // Create the server (leave it on the cleanup stack)
    CServer2* server = CSpeechSynthesisServer::NewLC();
    
    // Initialisation complete, now signal the client
    RProcess::Rendezvous( KErrNone );

    RUBY_DEBUG0( "SpeechSynthesisServer: server fully running." );
    
    // Ready to run
    CActiveScheduler::Start();
    
    RUBY_DEBUG0( "SpeechSynthesisServer: server closing." );
    
    // Cleanup the server and scheduler
    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( scheduler );
    }

// ----------------------------------------------------------------------------
// E32Main
// Server process entry-point
// ----------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK;
    
    RUBY_DEBUG0( "SpeechSynthesisServer: E32Main" );
    
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TInt error( KErrNoMemory );
    
    if ( cleanup )
        {
        TRAP( error, RunServerL() );
        delete cleanup;
        }
        
    __UHEAP_MARKEND;
    
    return error;
    }
    
    
// ----------------------------------------------------------------------------
// LanguageOrder
// Used in CSpeechSynthesisServer::GetSupportedLanguagesL()
// ----------------------------------------------------------------------------
// 
static TInt LanguageOrder( const TLanguage& a, const TLanguage& b )
    {
    TInt result( 0 );
    
    if ( a < b )
        {
        result = -1;
        }
    else if ( a > b )
        {
        result = 1;
        }

    return result;
    }


// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::CSpeechSynthesisServer
// Constructor
// ----------------------------------------------------------------------------
inline CSpeechSynthesisServer::CSpeechSynthesisServer() :
    CPolicyServer( CActive::EPriorityStandard, 
                   KSpeechSynthesisPolicy, 
                   ESharableSessions ),
    iOpenTtsPlugin( KNullUid ),
    iSessionPriority( EMdaPriorityMin ), 
    iDefaultSpeakingRate( KTtsMaxSpeakingRate / 2 ),
    iDefaultAudioPriority( EMdaPriorityNormal ),
    iDefaultAudioPreference( EMdaPriorityPreferenceTimeAndQuality )
    {
    // Nothing
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::~CSpeechSynthesisServer
// Destructor
// ----------------------------------------------------------------------------
//
CSpeechSynthesisServer::~CSpeechSynthesisServer()
    {    
    delete iTtsUtility;
    delete iShutdown;
    delete iConfigurationListener;
    delete iMMCListener; 
    
    iTtsVoiceConfigs.Close();
    
    if ( iPluginCloser )
        {
        iPluginCloser->Cancel();
        delete iPluginCloser;
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::NewLC
// 
// ----------------------------------------------------------------------------
//
CServer2* CSpeechSynthesisServer::NewLC()
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::NewLC" );
    
    CSpeechSynthesisServer* self = new(ELeave) CSpeechSynthesisServer();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::ConstructL
// 2nd phase construction - ensure the timer and server objects are running
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::ConstructL" );
    
    StartL( KSpeechSynthesisServerName );
    
    iTtsUtility = CTtsUtility::NewL( *this );
    iTtsUtility->SetPriority( iDefaultAudioPriority, 
                              (TMdaPriorityPreference)iDefaultAudioPreference );
    
    // Create object for closing tts plugin when server has been idle for a while. 
    iPluginCloser = CPeriodic::NewL( CActive::EPriorityIdle );
     
    // Listens for changes in SIS installations
    iConfigurationListener = CConfigurationListener::NewL( *this );
    
    // Listens insertions and removals of MMC. 
    iMMCListener = CMMCListener::NewL( *this );
    
    UpdateSynthesisConfigurationL();
    
    // Ensure that the server still exits even if the 1st client fails to connect
    iShutdown = CShutdown::NewL();
    iShutdown->Cancel();
    iShutdown->Start();
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::AddSession
// A new session is being created
// Cancel the shutdown timer if it was running
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::AddSession()
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::AddSession" );
    
    iSessionCount++;
    
    iShutdown->Cancel();
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::DropSession
// A session is being destroyed
// Start the shutdown timer if it is the last session.
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::DropSession()
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::DropSession" );
    
    iSessionCount--;
    
    if ( iSessionCount == 0 )
        {
        iShutdown->Cancel();
        iShutdown->Start();
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::ReserveL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::ReserveL( MSpeechSynthesisServerObserver* aSession )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::ReserveL" );
    
    iPluginCloser->Cancel();
    
    if ( !iSession || iSession == aSession )
        {
        // Session is not in use or it's reserved already for this session
        iSession = aSession;
        }
    else
        {
        User::Leave( KErrInUse );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::ReserveL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::ReserveL( MSpeechSynthesisServerObserver* aSession,
                                       TInt aPriority )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::ReserveL" );
    
    iPluginCloser->Cancel();
    
    if ( !iSession || iSession == aSession )
        {
        // Session is not in use or it's reserved already for this session
        iSession = aSession;
        
        iSessionPriority = aPriority;
        }
    else if ( aPriority > iSessionPriority )
        {
        Stop();
        
        iSession->MsssOperationAborted();
        
        iPreviousSession = iSession;
        iSession = aSession; 
        iSessionPriority = aPriority; 
        }
    else 
        {
        User::Leave( KErrInUse ); 
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::Release
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::Release( const MSpeechSynthesisServerObserver* aSession )
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::Release" );
    
    if ( iSession == aSession )
        {
        iPreviousSession = iSession;
        iSession = NULL; 
        iSessionPriority = EMdaPriorityMin;
        
        TRAP_IGNORE( UpdateSynthesisConfigurationIfNeededL() );
        
        iPluginCloser->Cancel();
        iPluginCloser->Start( KTtsPluginCloseWait, KTtsPluginCloseWait,
                              TCallBack( ClosePlugin, this ) );
        
        RUBY_DEBUG0( "iPluginCloser started" );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::GetDefaultSettingsL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::GetDefaultSettingsL( TVoice& aVoice, 
                                  TInt& aSpeakingRate, TInt& aMaxSpeakingRate,
                                  TInt& aVolume, TInt& aMaxVolume, 
                                  TInt& aAudioPriority, TInt& aAudioPreference,
                                  TInt& aAudioOutput )
    {
    UpdateSynthesisConfigurationIfNeededL();
    
    aVoice           = iDefaultVoice;
    aSpeakingRate    = iDefaultSpeakingRate;
    aMaxSpeakingRate = KTtsMaxSpeakingRate;
    aVolume          = iDefaultVolume;
    aMaxVolume       = iDefaultMaxVolume;
    aAudioPriority   = iDefaultAudioPriority;
    aAudioPreference = iDefaultAudioPreference;
    aAudioOutput     = RSpeechSynthesis::ETtsOutputDefault;
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::GetDefaultVoiceL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::GetDefaultVoiceL( TVoice& aVoice )
    {
    UpdateSynthesisConfigurationIfNeededL();
    
    aVoice = iDefaultVoice;
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::PrimeL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::PrimeL( const TDesC& aText )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::PrimeL" );
    RUBY_ASSERT_DEBUG( iOpenTtsPlugin != KNullUid, User::Panic( KPanic, __LINE__ ) );
    
    iTtsUtility->OpenDesL( aText );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::PrimeL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::PrimeL( const TDesC& aText, const RFile& aFile )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::PrimeL" );
    RUBY_ASSERT_DEBUG( iOpenTtsPlugin != KNullUid, User::Panic( KPanic, __LINE__ ) );

    iTtsUtility->SetOutputFileL( aFile ); 
    iTtsUtility->OpenDesL( aText ); 
    } 
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::Synthesize
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::Synthesize()
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::Synthesize" );
    RUBY_ASSERT_DEBUG( iOpenTtsPlugin != KNullUid, User::Panic( KPanic, __LINE__ ) );
    
    iTtsUtility->Play();
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::Stop
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::Stop()
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::Stop" );

    if ( iOpenTtsPlugin != KNullUid )
        {
        iTtsUtility->Stop();
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::Pause
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::Pause()
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::Pause" );
    RUBY_ASSERT_DEBUG( iOpenTtsPlugin != KNullUid, User::Panic( KPanic, __LINE__ ) );
    
    iTtsUtility->Pause(); 
    }  

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::GetSupportedLanguagesL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::GetSupportedLanguagesL( RArray<TLanguage>& aLanguages )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::GetSupportedLanguagesL" );
    
    UpdateSynthesisConfigurationIfNeededL();
    
    TInt error( KErrNone );    
    TLinearOrder<TLanguage> order( LanguageOrder );
    
    for( TInt i( 0 ); i < iTtsVoiceConfigs.Count(); i++ )
        {
        // Add every language only once 
        error = aLanguages.InsertInOrder( iTtsVoiceConfigs[i].iLanguage, order );
        if ( error && error != KErrAlreadyExists )
            {
            User::Leave( error );
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::GetSupportedVoicesL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::GetSupportedVoicesL( TLanguage aLanguage, 
                                                  RArray<TVoice>& aVoices )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::GetSupportedVoicesL" );
    CleanupClosePushL( aVoices );
    
    UpdateSynthesisConfigurationIfNeededL();
    
    TVoice voice;
    
    for( TInt i( 0 ); i < iTtsVoiceConfigs.Count(); i++ )
        {
        if ( iTtsVoiceConfigs[i].iLanguage == aLanguage )
            {
            voice.iLanguage     = iTtsVoiceConfigs[i].iLanguage;
            voice.iVoiceName    = iTtsVoiceConfigs[i].iVoiceName;
            voice.iSamplingRate = iTtsVoiceConfigs[i].iSamplingRate;
            
            aVoices.AppendL( voice );
            }
        }
    CleanupStack::Pop(); 
    }
 
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::SetVoiceL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::SetVoiceL( TVoice aVoice, TInt aSpeakingRate )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::SetVoiceL" );
    
    UpdateSynthesisConfigurationIfNeededL();
    
    TTtsStyle newStyle;
    newStyle.iLanguage = aVoice.iLanguage;
    newStyle.iVoice    = aVoice.iVoiceName;
    newStyle.iRate     = aSpeakingRate;
    newStyle.iNlp      = ETrue;  // Turn NLP flag on, only Klatt will use it. 
            
    TInt voiceIndex( KErrNotFound );
    const TInt count ( 
    iTtsVoiceConfigs.Count() );
    TInt i( 0 );
    
    while ( i < count && voiceIndex < 0 )
        {        
        if ( iTtsVoiceConfigs[i].iLanguage == aVoice.iLanguage )
            {
            if ( aVoice.iVoiceName.Length() > 0 )
                {
                if ( iTtsVoiceConfigs[i].iVoiceName == aVoice.iVoiceName )
                    {
                    voiceIndex = i;
                    }
                }
            else
                {
                voiceIndex = i;
                }
            }
        
        i++;
        }

    if ( voiceIndex >= 0 )
        {
        if ( iOpenTtsPlugin != iTtsVoiceConfigs[voiceIndex].iPluginUid )
            {
            iTtsUtility->Close();
            iOpenTtsPlugin = KNullUid;
        
            iTtsUtility->OpenPluginL( iTtsVoiceConfigs[voiceIndex].iPluginUid );
            iOpenTtsPlugin = iTtsVoiceConfigs[voiceIndex].iPluginUid;
            }
        
        iTtsUtility->SetDefaultStyleL( newStyle );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::SetVolume
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::SetVolume( TInt aVolume )
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::SetVolumeL" );
    RUBY_ASSERT_DEBUG( iOpenTtsPlugin != KNullUid, User::Panic( KPanic, __LINE__ ) );
    
    iTtsUtility->SetVolume( aVolume );
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::SetAudioPriorityL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::SetAudioPriorityL( TInt aPriority, 
                                                TInt aPreference )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::SetAudioPriorityL" );
    RUBY_ASSERT_DEBUG( iOpenTtsPlugin != KNullUid, User::Panic( KPanic, __LINE__ ) );
    
    iTtsUtility->SetPriority( aPriority, (TMdaPriorityPreference)aPreference );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::SetAudioOutputL
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::SetAudioOutputL( TInt aAudioOutput )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::SetAudioOutput" );
    RUBY_ASSERT_DEBUG( iOpenTtsPlugin != KNullUid, User::Panic( KPanic, __LINE__ ) );
    
    TMMFMessageDestinationPckg destination( iOpenTtsPlugin );
    CAudioOutput::TAudioOutputPreference output;
    
    switch ( aAudioOutput )
        {        
        case RSpeechSynthesis::ETtsOutputAll: 
            
            output = CAudioOutput::EAll;
            
            break;

        case RSpeechSynthesis::ETtsOutputNoOutput: 
            
            output = CAudioOutput::ENoOutput;
            
            break;

        case RSpeechSynthesis::ETtsOutputPrivate: 
            
            output = CAudioOutput::EPrivate;
            
            break;

        case RSpeechSynthesis::ETtsOutputPublic: 
            
            output = CAudioOutput::EPublic;
            
            break;
            
        default:
            
            output = CAudioOutput::ENoPreference;
        }
            
    TBuf8<2> data;
    data.AppendNum( output );    

    User::LeaveIfError( iTtsUtility->CustomCommandSync( destination, 
                                                        ETtsCustomCommandSetAudioOutput, 
                                                        data, KNullDesC8 ) );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::CustomCommand
//
// ----------------------------------------------------------------------------
//
TInt CSpeechSynthesisServer::CustomCommand( TInt aCommand, TInt aValue )
    {
    RUBY_DEBUG0( "" );
    
    TInt result( KErrNotReady );
    
    if ( iOpenTtsPlugin != KNullUid )
        {
        TMMFMessageDestinationPckg destination( iOpenTtsPlugin );
        
        TBuf8<16> command;
        command.AppendNum( aCommand );
        
        TBuf8<16> value;
        value.AppendNum( aValue );
    
        result = iTtsUtility->CustomCommandSync( destination, ETtsCustomCommandSetPluginParameter, 
                                                 command, value );
        }
    
    return result;
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::IsVoiceValidL
//
// ----------------------------------------------------------------------------
//
TBool CSpeechSynthesisServer::IsVoiceValidL( const TVoice& aVoice )
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::IsVoiceValid" );
    
    UpdateSynthesisConfigurationIfNeededL();
    
    TBool found( EFalse );
    TInt index( 0 );
    TInt count( iTtsVoiceConfigs.Count() );
    
    while ( !found  && index < count )
        {        
        if ( iTtsVoiceConfigs[index].iLanguage == aVoice.iLanguage )
            {
            found = ETrue;
            
            if ( aVoice.iVoiceName.Length() > 0 &&
                 aVoice.iVoiceName != iTtsVoiceConfigs[index].iVoiceName )
                {
                // Voice name does not match
                found = EFalse;
                }
                
            if ( aVoice.iSamplingRate != KErrNotFound && 
                 aVoice.iSamplingRate != iTtsVoiceConfigs[index].iSamplingRate )
                {
                // Sampling rate does not match
                found = EFalse;
                }
            }
        
        index++;
        }

    return found;
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::McoConfigurationChanged
// Some SIS package has been installed or removed
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::McoConfigurationChanged()
    {
    RUBY_DEBUG0( "CSpeechSynthesisServer::McoConfigurationChanged" );
    
    iUpdateNeeded = ETrue;
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::MapcCustomCommandEvent
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::MapcCustomCommandEvent( TInt /*aEvent*/, 
                                                     TInt /*aError*/ )
    {
    RUBY_ERROR0( "CSpeechSynthesisServer::MapcCustomCommandEvent" );
    
    // Not used
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::MapcInitComplete
//
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::MapcInitComplete( TInt aError, 
        const TTimeIntervalMicroSeconds& aDuration )
    {
    RUBY_DEBUG1( "CSpeechSynthesisServer::MapcInitComplete [%x]", iSession );
    
    if ( iSession )
        {
        iSession->MsssInitComplete( aError, aDuration );
        }
    else
        {
        RUBY_ERROR1( "CSpeechSynthesisServer::MapcInitComplete - No session [%d]",
                     aError );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::MapcPlayComplete
//
// ----------------------------------------------------------------------------
//      
void CSpeechSynthesisServer::MapcPlayComplete( TInt aError )
    {
    RUBY_DEBUG1( "CSpeechSynthesisServer::MapcPlayComplete [%x]", iSession );
    
    if ( iSession )
        {
        iSession->MsssPlayComplete( aError );
        }
    else
        {
        RUBY_ERROR1( "CSpeechSynthesisServer::MapcPlayComplete - No session [%d]",
                     aError );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::ClosePlugin
// 
// ----------------------------------------------------------------------------
//
TInt CSpeechSynthesisServer::ClosePlugin( TAny* aAny )
    {
    CSpeechSynthesisServer* self = reinterpret_cast<CSpeechSynthesisServer*>( aAny );

    RUBY_DEBUG1( "Closing plugin %x", self->iOpenTtsPlugin );
    
    self->iPluginCloser->Cancel();
    
    self->iTtsUtility->Close();
    self->iOpenTtsPlugin = KNullUid;
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::PreviousSession
// 
// ----------------------------------------------------------------------------
//
const MSpeechSynthesisServerObserver* CSpeechSynthesisServer::PreviousSession()
    {
    RUBY_DEBUG1( "iPreviousSession = %x", iPreviousSession );
    
    return iPreviousSession;
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::ClosePlugin
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::ClosePlugin()
    {
    RUBY_DEBUG0( "" );
    
    iPluginCloser->Cancel();
    
    iTtsUtility->Close();
    iOpenTtsPlugin = KNullUid;
    }
        
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::NewSessionL
// Create a new client session.
// ----------------------------------------------------------------------------
//
CSession2* CSpeechSynthesisServer::NewSessionL( const TVersion& /*aVersion*/, 
                                                const RMessage2& /*aMessage*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::NewSessionL" );
    
    return new( ELeave ) CSpeechSynthesisSession();
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::CustomSecurityCheckL
// Custom access policy check
// ----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CSpeechSynthesisServer::CustomSecurityCheckL( 
    const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& aMissing )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    CPolicyServer::TCustomResult result( CPolicyServer::EFail ); 
    
    if ( aMsg.Function() == ESetAudioOutput )
        {
        if ( aMsg.Int0() == RSpeechSynthesis::ETtsOutputAll && 
             !aMsg.HasCapability( ECapabilityWriteDeviceData, 
                                  __PLATSEC_DIAGNOSTIC_STRING("Missing WriteDeviceData") ) )
            {
            result = CPolicyServer::EFail;
            
            aMissing.iCaps = ECapabilityWriteDeviceData;
            }
        else
            {
            result = CPolicyServer::EPass;
            }
        }
    
    return result;
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::UpdateSynthesisConfigurationIfNeededL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::UpdateSynthesisConfigurationIfNeededL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::UpdateSynthesisConfigurationIfNeededL" );
    
    if ( iUpdateNeeded )
        {
        RUBY_DEBUG0( "Update needed!" );
        
        if ( iSession )
            {
            RUBY_DEBUG0( "Cannot update - iSession not NULL" );
            }
        else            
            {
            UpdateSynthesisConfigurationL();
            
            iUpdateNeeded = EFalse;
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisServer::UpdateSynthesisConfigurationL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisServer::UpdateSynthesisConfigurationL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisServer::UpdateSynthesisConfigurationL" );
    
    iPluginCloser->Cancel();
    
    RArray<TLanguage> languages;
    CleanupClosePushL( languages );
    
    RArray<TTtsStyle> ttsVoices;
    CleanupClosePushL( ttsVoices );
    
    TVoiceConfiguration cnfg;
    
    TBool defaultsVoiceSet( EFalse );
    
    // UI language is selected as a default language if it is supported 
    TLanguage defaultLanguage( User::Language() ); 

    RArray<TUid> ttsPlugins;
    CleanupClosePushL( ttsPlugins ); 
    
    // Plugins are listed in priority order (highest first)
    iTtsUtility->ListPluginsL( ttsPlugins );    
    iTtsVoiceConfigs.Reset();
    
    // Loop all plugins
    for ( TInt i( 0 ); i < ttsPlugins.Count(); i++  )
        {
        cnfg.iPluginUid = ttsPlugins[i];
        
        iTtsUtility->Close();
        iOpenTtsPlugin = KNullUid;
        
        iTtsUtility->OpenPluginL( ttsPlugins[i] );
        iOpenTtsPlugin = ttsPlugins[i];
        
        iTtsUtility->GetSupportedLanguagesL( languages );

        if ( i == 0 )
            {
            // Maximum volume is same for all plugins. 
            iDefaultMaxVolume = iTtsUtility->MaxVolume();
            iDefaultVolume = iDefaultMaxVolume / 2;
            }

        // Loop supported languages
        for ( TInt j( 0 ); j < languages.Count(); j++ )
            {
            cnfg.iLanguage = languages[j];

            iTtsUtility->GetSupportedVoicesL( languages[j], ttsVoices ); 
            
            // Loop supported voices
            for ( TInt k( 0 ); k < ttsVoices.Count(); k++ )
                {
                cnfg.iVoiceName    = ttsVoices[k].iVoice;
                cnfg.iSamplingRate = ttsVoices[k].iSamplingRate;
                
                iTtsVoiceConfigs.AppendL( cnfg );

                if ( !defaultsVoiceSet && cnfg.iLanguage == defaultLanguage )
                    {
                    // Default voice is not set yet -> set it now
                    defaultsVoiceSet = ETrue;
                    
                    iDefaultVoice.iLanguage     = cnfg.iLanguage;
                    iDefaultVoice.iVoiceName    = cnfg.iVoiceName;
                    iDefaultVoice.iSamplingRate = cnfg.iSamplingRate;
                    }
                }
            
            ttsVoices.Reset();
            }
        
        languages.Reset();
        }
    
    CleanupStack::PopAndDestroy( &ttsPlugins );
    CleanupStack::PopAndDestroy( &ttsVoices );
    CleanupStack::PopAndDestroy( &languages );
    
    if ( !defaultsVoiceSet && iTtsVoiceConfigs.Count() > 0 )
        {
        iDefaultVoice.iLanguage     = iTtsVoiceConfigs[0].iLanguage;
        iDefaultVoice.iVoiceName    = iTtsVoiceConfigs[0].iVoiceName;
        iDefaultVoice.iSamplingRate = iTtsVoiceConfigs[0].iSamplingRate;
        } 
    
    if ( !iSession )
        {
        iPluginCloser->Start( KTtsPluginCloseWait, KTtsPluginCloseWait,
                              TCallBack( ClosePlugin, this ) );
        
        RUBY_DEBUG0( "iPluginCloser started" );
        }
    }

// End of file
