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


// INCLUDES

#include "speechsynthesissession.h"
#include "speechsynthesisserver.h"
#include "speechsynthesisclientserver.h"
#include "rubydebug.h"


// CONSTANTS

const TInt KMinVolume = 0;
const TInt KMinSpeakingRate = 1;

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::CSpeechSynthesisSession
// Constructor 
// ----------------------------------------------------------------------------
//
CSpeechSynthesisSession::CSpeechSynthesisSession():
    iSessionState( ESessionIdle )
    {
    RUBY_DEBUG0( "CSpeechSynthesisSession::CSpeechSynthesisSession" );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::CreateL
// 2nd phase construct for sessions - called by the CServer framework
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::CreateL()
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisSession::CreateL" );
 
    Server().GetDefaultSettingsL( iCurrentVoice, 
                                  iSpeakingRate, iMaxSpeakingRate,
                                  iVolume, iMaxVolume, 
                                  iAudioPriority, iAudioPreference,
                                  iAudioOutput );
    
    iTmpAudioOutput = iAudioOutput;
    iTmpVolume = iVolume;
    
    Server().AddSession();
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::~CSpeechSynthesisSession
// Destructor
// ----------------------------------------------------------------------------
//
CSpeechSynthesisSession::~CSpeechSynthesisSession()
    {
    RUBY_DEBUG0( "CSpeechSynthesisSession::~CSpeechSynthesisSession" );
    
    iLanguages.Close();
    iVoices.Close();
    iCustomCommands.Close();
    
    Server().Release( this );
    Server().DropSession();
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::MsssInitComplete
// Callback from server
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::MsssInitComplete( TInt aError, 
                                                const TTimeIntervalMicroSeconds& aDuration )
    {
    RUBY_DEBUG1( "CSpeechSynthesisSession::MsssInitComplete [%d]", aError );
    
    if ( aError )
        {
        iSessionState = ESessionIdle;
        iFileOutputActivated = EFalse; 
    
        iDuration = 0;
        iFile.Close();
        
        Server().Release( this );
        }
    else
        {
        iSessionState = ESessionPrimed;
        
        iDuration = aDuration;
        }
    
    if ( iStoredMessage.IsNull() )
        {
        RUBY_ERROR0( "iStoredMessage.IsNull() == ETrue" );
        }
    else
        {
        iStoredMessage.Complete( aError );
        RUBY_DEBUG0( "Message completed" );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::MsssPlayComplete
// Callback from server
// ----------------------------------------------------------------------------
//      
void CSpeechSynthesisSession::MsssPlayComplete( TInt aError )
    {
    RUBY_DEBUG1( "CSpeechSynthesisSession::MsssPlayComplete [%d]", aError );
    
    iSessionState = ESessionIdle;
    iFileOutputActivated = EFalse; 
    
    iDuration = 0;
    iFile.Close();
    
    Server().Release( this );

    if ( iStoredMessage.IsNull() )
        {
        RUBY_ERROR0( "iStoredMessage.IsNull() == ETrue" );
        }
    else
        {
        iStoredMessage.Complete( aError );
        RUBY_DEBUG0( "Message completed" );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::MsssOperationAborted
// Callback from server
// ----------------------------------------------------------------------------
//      
void CSpeechSynthesisSession::MsssOperationAborted()
    {
    RUBY_DEBUG0( "CSpeechSynthesisSession::MsssOperationAborted" );
    
    iSessionState = ESessionIdle;
    iFileOutputActivated = EFalse; 
    
    iDuration = 0;
    iFile.Close();

    if ( !iStoredMessage.IsNull() )
        {
        RUBY_DEBUG0( "Completing message with KErrDied" );
        iStoredMessage.Complete( KErrDied );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::Server
//
// ----------------------------------------------------------------------------
//
inline CSpeechSynthesisServer& CSpeechSynthesisSession::Server()
    {
    return *static_cast<CSpeechSynthesisServer*>(
                const_cast<CServer2*>( CSession2::Server() ) );
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::ServiceL
// Handle a client request.
// Leaving is handled by CSpeechSynthesisSession::ServiceError() which reports
// the error code to the client
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::ServiceL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisSession::ServiceL" );
    RUBY_DEBUG1( "aMessage.Function(): %d", aMessage.Function() );
    
    TInt message( aMessage.Function() );
    
    if ( message == EPrime || message == EPrimeFileOutput || 
         message == EPrimeStreamOutput || message == ESynthesise )
        {
        Server().ReserveL( this, iAudioPriority );

        TRAPD( error, DoAsyncServiceL( aMessage ) );
        if( error ) 
            {
            Server().Release( this );
            User::Leave( error );
            }
        }
    else
        {
        DoSyncServiceL( aMessage );
        }
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoAsyncServiceL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoAsyncServiceL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisSession::DoAsyncServiceL" );
    
    TInt function( aMessage.Function() );
    
    if ( function == ESynthesise )
        {
        if ( iSessionState == ESessionPrimed )
            {
            if ( !iFileOutputActivated )
                { // Values may be changed when synthesis is paused. 
                if ( iTmpVolume != iVolume )
                    {
                    Server().SetVolume( iVolume );
                    iTmpVolume = iVolume;
                    }
                
                if ( iTmpAudioOutput != iAudioOutput )
                    {
                    Server().SetAudioOutputL( iAudioOutput );
                    iTmpAudioOutput = iAudioOutput;
                    }
                }
                
            Server().Synthesize();
            }
        else
            {
            User::Leave( KErrNotReady );
            }
        }
    else if ( function == EPrimeStreamOutput )
        {
        User::Leave( KErrNotSupported );
        }
    else  // Prime or EPrimeFileOutput
        {
        iSessionState = ESessionIdle;
        Server().Stop();
        
        if ( iClosePlugin || Server().PreviousSession() != this )
            {
            iClosePlugin = EFalse; 
            
            Server().ClosePlugin();
            Server().SetVoiceL( iCurrentVoice, iSpeakingRate );
            
            // Set custom commands, plugin ingnores commands if the state
            // is incorrect for some command
            for ( TInt i( 0 ); i < iCustomCommands.Count(); i++ )
                {
                Server().CustomCommand( iCustomCommands[i].iCommand, iCustomCommands[i].iValue );
                }
            }
        else
            {
            Server().SetVoiceL( iCurrentVoice, iSpeakingRate );
            }
            
        // Fetch text to be synthesised
        HBufC* text = HBufC::NewLC( aMessage.GetDesLengthL( 0 ) );
        TPtr ptr = text->Des();
        aMessage.ReadL( 0, ptr );
    
        if ( function == EPrime )
            {
            Server().SetAudioPriorityL( iAudioPriority, iAudioPreference );
            
            Server().SetVolume( iVolume );
            iTmpVolume = iVolume;
            
            Server().SetAudioOutputL( iAudioOutput );
            iTmpAudioOutput = iAudioOutput;
            
            Server().PrimeL( ptr );
            
            // Set custom commands, plugin ingnores commands if the state
            // is incorrect for some command
            for ( TInt i( 0 ); i < iCustomCommands.Count(); i++ )
                { 
                Server().CustomCommand( iCustomCommands[i].iCommand, iCustomCommands[i].iValue );
                }
            
            iFileOutputActivated = EFalse; 
            }
        else
            {
            Server().PrimeL( ptr, iFile ); 
            
            iFileOutputActivated = ETrue;
            }

        CleanupStack::PopAndDestroy( text );
        }

    iStoredMessage = aMessage;
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoSyncServiceL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoSyncServiceL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "CSpeechSynthesisSession::DoSyncServiceL" );
    
    switch ( aMessage.Function() ) 
        {
        case ETransferFileHandle:
            
            User::LeaveIfError( iFile.AdoptFromClient( aMessage, 
                                                       KFsHandleIndex, 
                                                       KFileHandleIndex ) ); 
            
            break;
            
        case EStopSynthesis:
            
            DoStopSynthesisL();
            
            break;
            
        case EPauseSynthesis:
            
            DoPauseSynthesisL();
            
            break;

        case EGetDuration: 
        
            DoGetDurationL( aMessage );
        
            break;

        case EGetLanguages:
            
            DoGetLanguagesL( aMessage );

            break;
            
        case EGetLanguageCount:
            
            DoGetLanguageCountL( aMessage );
            
            break;
            
        case EGetVoices:
            
            DoGetVoicesL( aMessage );
            
            break;
        
        case EGetVoiceCount:
            
            DoGetVoiceCountL( aMessage );
            
            break;            

        case EVoice:
            
            DoVoiceL( aMessage );
            
            break;

        case ESetVoice:
            
            DoSetVoiceL( aMessage );
            
            break;

        case EMaxSpeakingRate:
           
            DoMaxSpeakingRateL( aMessage );
            
            break;
        
        case ESpeakingRate:
            
            DoSpeakingRateL( aMessage );
            
            break;

        case ESetSpeakingRate:
            
            DoSetSpeakingRateL( aMessage );
            
            break;
            
        case EMaxVolume:
            
            DoMaxVolumeL( aMessage );
            
            break;

        case EVolume:
            
            DoVolumeL( aMessage );
            
            break;

        case ESetVolume:
            
            DoSetVolumeL( aMessage );
            
            break;

        case ESetAudioPriority:
            
            DoSetAudioPriority( aMessage );
            
            break;

        case ESetAudioOutput:
            
            DoSetAudioOutputL( aMessage );
            
            break;

        case ECustomCommand:
            
            DoCustomCommandL( aMessage );
            
            break;
            
        default:
            
            RUBY_ERROR1( "CSpeechSynthesisSession::ServiceL - %d",
                         aMessage.Function() );
                         
            PanicClient( aMessage, EPanicIllegalFunction );
            
            break;
        }
    
    aMessage.Complete( KErrNone ); 
    }

// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoStopSynthesisL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoStopSynthesisL()
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // Check we have permission to use server. 
    Server().ReserveL( this );
    
    // Synthesis is ongoing
    Server().Stop();
    
    iSessionState = ESessionIdle;
    
    iDuration = 0;
    iFile.Close();
    
    // Release server and complete the previous message
    Server().Release( this );
    
    if ( !iStoredMessage.IsNull() )
        {
        iStoredMessage.Complete( KErrAbort );
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoPauseSynthesisL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoPauseSynthesisL()
    {
    RUBY_DEBUG_BLOCK( "" );
    
    if ( iSessionState == ESessionPrimed && !iStoredMessage.IsNull() )
        {
        if ( iFileOutputActivated )
            {
            User::Leave( KErrNotSupported );
            }
        
        // Check we have permission to use server. 
        Server().ReserveL( this );
        
        // Synthesis is ongoing
        Server().Pause();
        
        iStoredMessage.Complete( KErrCancel );
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoGetDurationL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoGetDurationL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    if ( iSessionState != ESessionPrimed )
        {
        User::Leave( KErrNotReady ); 
        }
    else if ( iDuration.Int64() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    
    TPckgBuf<TTimeIntervalMicroSeconds> pkg( iDuration );
    aMessage.WriteL( 0, pkg );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoGetLanguagesL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoGetLanguagesL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // DoGetLanguagesL is called immediately after EGetLanguageCount
    if ( iLanguages.Count() > 0 ) 
        { 
        const TInt dataSize( iLanguages.Count() * sizeof(TLanguage) );
        
        HBufC8* tmp = HBufC8::NewLC( dataSize );
        TPtr8 tmp_ptr( tmp->Des() );
        
        tmp_ptr.Append( (TUint8*)&iLanguages[0], dataSize );

        aMessage.WriteL( 0, tmp_ptr );
        
        CleanupStack::PopAndDestroy( tmp );
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoGetLanguageCountL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoGetLanguageCountL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    iLanguages.Reset();
    
    Server().GetSupportedLanguagesL( iLanguages );
    
    TPckgBuf<TInt> pkg_int( iLanguages.Count() ); 
    aMessage.WriteL( 0, pkg_int );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoGetVoicesL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoGetVoicesL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // DoGetVoicesL is called immediately after EGetVoiceCount
    if ( iVoices.Count() > 0 ) 
        {
        TInt dataSize( iVoices.Count() * sizeof(TVoice) );
        
        HBufC8* tmp = HBufC8::NewLC( dataSize );
        TPtr8 tmp_ptr( tmp->Des() );
        
        tmp_ptr.Append( (TUint8*)&iVoices[0], dataSize );

        aMessage.WriteL( 0, tmp_ptr );
        
        CleanupStack::PopAndDestroy( tmp );
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoGetVoiceCountL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoGetVoiceCountL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    iVoices.Reset();
    TLanguage language( static_cast<TLanguage> ( aMessage.Int1() ) );
    
    Server().GetSupportedVoicesL( language, iVoices );
    
    TPckgBuf<TInt> pkg_int( iVoices.Count() ); 
    aMessage.WriteL( 0, pkg_int );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoVoiceL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoVoiceL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    if ( !Server().IsVoiceValidL( iCurrentVoice ) )
        {
        Server().GetDefaultVoiceL( iCurrentVoice );
        }
        
    TPckgBuf<TVoice> pkg_voice( iCurrentVoice );
    aMessage.WriteL( 0, pkg_voice );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoSetVoiceL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoSetVoiceL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    TPckgBuf<TVoice> pkg_voice;
    aMessage.ReadL( 0, pkg_voice );
    
    TVoice voice( pkg_voice() );
    
    if ( Server().IsVoiceValidL( voice ) )
        {
        iCurrentVoice = voice;
        
        // Custom commands are valid until new voice is set
        iCustomCommands.Close();
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoMaxSpeakingRateL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoMaxSpeakingRateL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    TPckgBuf<TInt> pkg_int( iMaxSpeakingRate );
    aMessage.WriteL( 0, pkg_int );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoSpeakingRateL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoSpeakingRateL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    TPckgBuf<TInt> pkg_int( iSpeakingRate ); 
    aMessage.WriteL( 0, pkg_int );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoSetSpeakingRateL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoSetSpeakingRateL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    TInt newValue( aMessage.Int0() );
    
    if ( newValue < KMinSpeakingRate || newValue > iMaxSpeakingRate )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        iSpeakingRate = newValue;
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoMaxVolumeL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoMaxVolumeL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    TPckgBuf<TInt> pkg_int( iMaxVolume ); 
    aMessage.WriteL( 0, pkg_int );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoVolumeL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoVolumeL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    TPckgBuf<TInt> pkg_int( iVolume ); 
    aMessage.WriteL( 0, pkg_int );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoSetVolumeL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoSetVolumeL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    TInt newValue( aMessage.Int0() );
    
    if ( newValue < KMinVolume || newValue > iMaxVolume )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        iVolume = newValue;
        
        if ( !iStoredMessage.IsNull() )
            {
            // Check first we have permission to use server. 
            TRAP_IGNORE(
                Server().ReserveL( this );
                Server().SetVolume( iVolume );
                iTmpVolume = iVolume;
                );
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoSetAudioPriority
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoSetAudioPriority( const RMessage2& aMessage )
    {
    RUBY_DEBUG0( "" );
    
    iAudioPriority = aMessage.Int0();
    iAudioPreference = aMessage.Int1();
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoSetAudioOutputL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoSetAudioOutputL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    iAudioOutput = aMessage.Int0();
    
    if ( !iStoredMessage.IsNull() )
        {
        // Check first we have permission to use server. 
        TRAPD( error, Server().ReserveL( this ) );
        if ( error == KErrNone )
            {
            Server().SetAudioOutputL( iAudioOutput );
            iTmpAudioOutput = iAudioOutput;
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::DoCustomCommandL
// 
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::DoCustomCommandL( const RMessage2& aMessage )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    iClosePlugin = ETrue;
    
    TInt command( aMessage.Int0() );
    TInt value( aMessage.Int1() );
    
    TBool found( EFalse );
    TInt count( iCustomCommands.Count() );
    
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iCustomCommands[i].iCommand == command )
            {
            iCustomCommands[i].iValue = value;
            found = ETrue;
            break;
            }
        }
    
    if ( !found )
        {
        TCustomCommand newItem;
        newItem.iCommand = command;
        newItem.iValue = value;
        
        iCustomCommands.AppendL( newItem );
        }
    
    Server().CustomCommand( command, value );
    }
    
// ----------------------------------------------------------------------------
// CSpeechSynthesisSession::ServiceError
// Handle an error from CSpeechSynthesisSession::ServiceL()
// ----------------------------------------------------------------------------
//
void CSpeechSynthesisSession::ServiceError( const RMessage2& aMessage, 
                                            TInt aError )
    {
    RUBY_DEBUG1( "CSpeechSynthesisSession::ServiceError %d", aError );
    
    CSession2::ServiceError( aMessage, aError );
    } 

// End of file
