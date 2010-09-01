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
* Description:  Speech synthesis server, client side code
*
*/


// @todo Add better file write support to Klatt and HQ plugins
// @todo Add support for lipsync information

#include <e32svr.h>
#include <f32file.h> 
#include <s32mem.h>
#include <speechsynthesis.h>

#include "speechsynthesisclientserver.h"

// CONSTANTS

const TInt KMajorVersion = 1;
const TInt KMinorVersion = 0;
const TInt KBuildNumber  = 1;

// ----------------------------------------------------------------------------
// StartServer
// Server startup code
// ----------------------------------------------------------------------------
// 
static TInt StartServer()
    {
    RProcess server;

    TInt error = server.Create( KSpeechSynthesisServerImg, KNullDesC );

    if ( !error )
        {
        // Server started
        TRequestStatus status;
        server.Rendezvous( status );
        
        if ( status != KRequestPending )
            {
            server.Kill( KErrGeneral ); // Abort startup
            }
        else
            {
            server.Resume();            // Logon OK - start the server
            }
            
        User::WaitForRequest( status ); // Wait for start or death
        
        error = ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
        
        // Close handle
        server.Close();
        }
        
    return error;
    }
    
    
// ----------------------------------------------------------------------------
// RSpeechSynthesis::RSpeechSynthesis
// Constructor
// ----------------------------------------------------------------------------
//
EXPORT_C RSpeechSynthesis::RSpeechSynthesis()
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::Open
// Creates a session to server
// ----------------------------------------------------------------------------
//
EXPORT_C TInt RSpeechSynthesis::Open()
    {
    const TInt KMessageSlots( 4 );  // 1 async, 1 sync, 1 cancel, 1 kernel
    
    const TVersion requiredServerVersion( KMajorVersion, 
                                          KMinorVersion, 
                                          KBuildNumber );

    // Try to create session with following parameters:
    // 1) Name of the server
    // 2) The lowest version of the server with which this client is compatible
    // 3) The number of message slots 
    TInt error = CreateSession( KSpeechSynthesisServerName, 
                                requiredServerVersion, KMessageSlots );
    
    if ( error == KErrNotFound || error == KErrServerTerminated )
        {
        // Server not running -> try to start it
        error = StartServer();
        
        if ( error == KErrNone || error == KErrAlreadyExists )
            {
            // Server startup succeeded -> try to create session to it
            error = CreateSession( KSpeechSynthesisServerName, 
                                   requiredServerVersion, KMessageSlots );

            }
        }

    return error;
    }
    
// ----------------------------------------------------------------------------
// RSpeechSynthesis::Close
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::Close()
    {
    RSessionBase::Close();  
    }
   
// ----------------------------------------------------------------------------
// RSpeechSynthesis::InitialiseSynthesis
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::InitialiseSynthesis( const TDesC& aText, 
                                                     TRequestStatus& aStatus )
    {
    if ( Handle() )
        {
        SendReceive( EPrime, TIpcArgs( &aText ), aStatus ); 
        }
    else
        {
        TRequestStatus *statusPointer = &aStatus;
        User::RequestComplete( statusPointer, KErrBadHandle );
        }
    }
    
// ----------------------------------------------------------------------------
// RSpeechSynthesis::InitialiseSynthesis
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::InitialiseSynthesis( const TDesC& aText, 
                                                     const TFileName& aFileName,
                                                     TRequestStatus& aStatus )
    {
    TRAPD( error, DoInitialiseSynthesisL( aText, aFileName, aStatus ) );
    if ( error )
        {
        TRequestStatus *statusPointer = &aStatus;
        User::RequestComplete( statusPointer, error );
        }
    }
    
// ----------------------------------------------------------------------------
// RSpeechSynthesis::InitialiseSynthesis
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::InitialiseSynthesis( const TDesC& aText,
                                                     TDes8& aDataBuffer, 
                                                     TRequestStatus& aStatus )
    {
    if ( Handle() )
        {
        SendReceive( EPrimeStreamOutput, TIpcArgs( &aText, &aDataBuffer ), aStatus ); 
        }
    else
        {
        TRequestStatus *statusPointer = &aStatus;
        User::RequestComplete( statusPointer, KErrBadHandle );
        }
    }
    
// ----------------------------------------------------------------------------
// RSpeechSynthesis::Synthesise
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::Synthesise( TRequestStatus& aStatus )
    {
    if ( Handle() )
        {
        SendReceive( ESynthesise, aStatus );
        }
    else
        {
        TRequestStatus *statusPointer = &aStatus;
        User::RequestComplete( statusPointer, KErrBadHandle );
        }
    }
    
// ----------------------------------------------------------------------------
// RSpeechSynthesis::StopL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::StopL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    User::LeaveIfError( SendReceive( EStopSynthesis ) ); 
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::PauseL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::PauseL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    User::LeaveIfError( SendReceive( EPauseSynthesis ) );
    }     

// ----------------------------------------------------------------------------
// RSpeechSynthesis::DurationL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C TTimeIntervalMicroSeconds RSpeechSynthesis::DurationL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TTimeIntervalMicroSeconds> pckg;
    
    User::LeaveIfError( SendReceive( EGetDuration, TIpcArgs( &pckg ) ) ); 
    
    return pckg();
    }
    
// ----------------------------------------------------------------------------
// RSpeechSynthesis::GetLanguagesL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::GetLanguagesL( RArray<TLanguage>& aLanguages )
    {
	CleanupClosePushL( aLanguages ); 
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TInt> pckg;
    
    User::LeaveIfError( SendReceive( EGetLanguageCount, TIpcArgs( &pckg ) ) );
    
    const TInt languageCount( pckg() );

    if ( languageCount > 0 )    
        {
        const TInt tlanguageSize( sizeof(TLanguage) );
    
        HBufC8* data = HBufC8::NewLC( languageCount * tlanguageSize );
        TPtr8 data_ptr( data->Des() );
    
        User::LeaveIfError( SendReceive( EGetLanguages, TIpcArgs( &data_ptr ) ) );
        
        for ( TInt i( 0 ); i < languageCount; i++ )
            {
            TPtr8 tmp = data_ptr.MidTPtr( i * tlanguageSize, tlanguageSize );
            
            const TLanguage* lang = (TLanguage*)tmp.Ptr();
            
            aLanguages.AppendL( *lang );
            }

        CleanupStack::PopAndDestroy( data );
        }
    CleanupStack::Pop(); 
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::GetVoicesL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::GetVoicesL( RArray<TVoice>& aVoices,
                                            TLanguage aLanguage )
    {
	CleanupClosePushL( aVoices ); 
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TInt> pckg;
    
    User::LeaveIfError( SendReceive( EGetVoiceCount, 
                                     TIpcArgs( &pckg, aLanguage ) ) );
    
    const TInt voiceCount( pckg() );
    
    if ( voiceCount > 0 ) 
        {
        const TInt tvoiceSize( sizeof(TVoice) );
        
        HBufC8* data = HBufC8::NewLC( voiceCount * tvoiceSize );
        TPtr8 data_ptr( data->Des() );
    
        User::LeaveIfError( SendReceive( EGetVoices, TIpcArgs( &data_ptr ) ) );
        
        for ( TInt i( 0 ); i < voiceCount; i++ )
            {
            TPtr8 tmp = data_ptr.MidTPtr( i * tvoiceSize, tvoiceSize );
            
            const TVoice* voice = (TVoice*)tmp.Ptr();
            
            aVoices.AppendL( *voice );
            }
        
        CleanupStack::PopAndDestroy( data );   
        }
    CleanupStack::Pop(); 
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::VoiceL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C TVoice RSpeechSynthesis::VoiceL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TVoice> pckg;
    
    User::LeaveIfError( SendReceive( EVoice, TIpcArgs( &pckg ) ) );
    
    return pckg();
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::SetVoiceL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::SetVoiceL( const TVoice& aVoice )
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TVoice> pckg = aVoice;
    
    User::LeaveIfError( SendReceive( ESetVoice, TIpcArgs( &pckg ) ) );
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::MaxSpeakingRateL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C TInt RSpeechSynthesis::MaxSpeakingRateL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TInt> pckg;
    
    User::LeaveIfError( SendReceive( EMaxSpeakingRate, TIpcArgs( &pckg ) ) );
    
    return pckg();
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::SpeakingRateL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C TInt RSpeechSynthesis::SpeakingRateL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TInt> pckg;
    
    User::LeaveIfError( SendReceive( ESpeakingRate, TIpcArgs( &pckg ) ) );
    
    return pckg();
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::SetSpeakingRateL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::SetSpeakingRateL( TInt aSpeakingRate )
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    User::LeaveIfError( SendReceive( ESetSpeakingRate, 
                                     TIpcArgs( aSpeakingRate ) ) );
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::MaxVolumeL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C TInt RSpeechSynthesis::MaxVolumeL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TInt> pckg;
    
    User::LeaveIfError( SendReceive( EMaxVolume, TIpcArgs( &pckg ) ) );
    
    return pckg();
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::VolumeL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C TInt RSpeechSynthesis::VolumeL()
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    TPckgBuf<TInt> pckg;
    
    User::LeaveIfError( SendReceive( EVolume, TIpcArgs( &pckg ) ) );
    
    return pckg();
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::SetVolumeL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::SetVolumeL( TInt aVolume )
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    User::LeaveIfError( SendReceive( ESetVolume, TIpcArgs( aVolume ) ) );
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::SetAudioPriorityL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::SetAudioPriorityL( TInt aPriority, 
                                                   TInt aPreference )
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    User::LeaveIfError( SendReceive( ESetAudioPriority, 
                                     TIpcArgs( aPriority, aPreference ) ) );
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::SetAudioOutputL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::SetAudioOutputL( TTtsAudioOutput aAudioOutput )
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    User::LeaveIfError( SendReceive( ESetAudioOutput, TIpcArgs( aAudioOutput ) ) );
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::CustomCommandL
// 
// ----------------------------------------------------------------------------
//
EXPORT_C void RSpeechSynthesis::CustomCommandL( TInt aCommand, TInt aValue )
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    
    User::LeaveIfError( SendReceive( ECustomCommand, 
                                     TIpcArgs( aCommand, aValue ) ) );
    }

// ----------------------------------------------------------------------------
// RSpeechSynthesis::DoInitialiseSynthesisL
// 
// ----------------------------------------------------------------------------
//
void RSpeechSynthesis::DoInitialiseSynthesisL( const TDesC& aText, 
                                               const TFileName& aFileName, 
                                               TRequestStatus& aStatus ) 
    {
    if ( Handle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
        
    // @todo Add support for all supported audio formats 
    TFileName tmpName( aFileName );
    tmpName.LowerCase();
            
    TParse parser;
    User::LeaveIfError( parser.Set( tmpName, NULL, NULL ) );
    
    TPtrC extension = parser.Ext(); 
    
    if ( extension != _L(".raw") ) 
        {
        User::Leave( KErrArgument );
        }

    RFs fs;
    RFile file;

    User::LeaveIfError( fs.Connect() ); 
    CleanupClosePushL( fs );    
    User::LeaveIfError( fs.ShareProtected() ); 
    
    User::LeaveIfError( file.Replace( fs, aFileName, EFileWrite ) ); 
    CleanupClosePushL( file );
    
    TIpcArgs ipcArgs;
    User::LeaveIfError( file.TransferToServer( ipcArgs, KFsHandleIndex, KFileHandleIndex ) ); 
    User::LeaveIfError( SendReceive( ETransferFileHandle, ipcArgs ) );
    
    CleanupStack::PopAndDestroy( &file ); 
    CleanupStack::PopAndDestroy( &fs ); 
    
    SendReceive( EPrimeFileOutput, TIpcArgs( &aText ), aStatus ); 
    }
    
// End of file
