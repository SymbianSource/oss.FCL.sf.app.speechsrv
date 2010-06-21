/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles the tone playing for the VoiceUIRecognition
*
*/


// INCLUDE FILES
#include <aknsoundsystem.h>
#include <aknappui.h>
#include <AccessoryServer.h>        //RAccessoryServer
#include <AccessoryConnection.h>    //RAccessoryConnection

#include <centralrepository.h>

#include <StringLoader.h>
#include <AudioPreference.h>

#include <srsfdomaincrkeys.h>

#include <vuivoicerecognition.rsg>
#include "vuictoneplayer.h"

#include "rubydebug.h"

// CONSTANTS

// Maximum value of the volume setting in Central Repository
const TInt KMaxRepositoryVolume = 10;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTonePlayer* CTonePlayer::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CTonePlayer* CTonePlayer::NewL()
    {
    CTonePlayer* self = new (ELeave) CTonePlayer;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CTonePlayer::~CTonePlayer()
    {
    RUBY_DEBUG0( "CTonePlayer::~CTonePlayer START" );

    if ( iAudioPlayer )
        {
        iAudioPlayer->Close();
        delete iAudioPlayer;
        }

    delete iStartSound;
    delete iConfirmationSound;
    delete iBtStartSound;

    iObserver = NULL;
    
    RUBY_DEBUG0( "CTonePlayer::~CTonePlayer EXIT" );
    }

// -----------------------------------------------------------------------------
// CTonePlayer::CTonePlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTonePlayer::CTonePlayer()
    {
    }

// -----------------------------------------------------------------------------
// CTonePlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTonePlayer::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CTonePlayer::ConstructL" );

    iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this,
                                                 KAudioPriorityVoiceDial, 
                                                 TMdaPriorityPreference( KAudioPrefVoiceStarting ) );

    iVolume = KErrGeneral;
    
    CRepository* client = CRepository::NewLC( KCRUidSRSFSettings );
    client->Get( KSRSFPlaybackVolume, iVolume );
    CleanupStack::PopAndDestroy( client );

    iStartSound = StringLoader::LoadL( R_VOICE_START_SOUND_PATH );
    iConfirmationSound = StringLoader::LoadL( R_VOICE_CONFIRMATION_SOUND_PATH );
    iBtStartSound = StringLoader::LoadL( R_VOICE_BT_START_SOUND_PATH );
    iBTAcc = EFalse;
    
    ChekcAccesoryStatusL();
    }
  
 // ---------------------------------------------------------
// CTonePlayer::RegisterObserver
// Sets the observer
// ---------------------------------------------------------
//  
void CTonePlayer::RegisterObserver( MMdaAudioPlayerCallback* aObserver )
    {
    RUBY_DEBUG0( "CTonePlayer::RegisterObserver START" );
    
    iObserver = aObserver;
    
    RUBY_DEBUG0( "CTonePlayer::RegisterObserver EXIT" );
    }

// ---------------------------------------------------------
// CTonePlayer::InitToneL
// Perform initialization for Playing the appropriate tone
// ---------------------------------------------------------
//
void CTonePlayer::InitToneL( TInt aSid )
   {
    RUBY_DEBUG_BLOCK( "CTonePlayer::PlayToneInitL" );
    
    iAudioPlayer->Stop();
    iAudioPlayer->Close();

    if ( aSid == EAvkonSIDNameDiallerStartTone )
        {
        RUBY_DEBUG0( "CTonePlayer::InitToneL - case: EAvkonSIDNameDiallerStartTone" );
        TInt err = KErrNone;
        if ( iBTAcc )
            {
            TRAP( err, iAudioPlayer->OpenFileL( *iBtStartSound ) );
            }
        else
            {
            TRAP( err, iAudioPlayer->OpenFileL( *iStartSound ) );
            }
        if ( err )
            {
            RUBY_ERROR1( "CTonePlayer::InitToneL - err [%d]", err );
                    
            // Cannot play sound, but observer still needs to be notified
            iObserver->MapcInitComplete( err, 0 );
            }
        }
    else if ( aSid == EAvkonSIDConfirmationTone )
        {
        RUBY_DEBUG0( "CTonePlayer::InitToneL - case: EAvkonSIDConfirmationTone" );

        TRAPD( err, iAudioPlayer->OpenFileL( *iConfirmationSound ) );
        if ( err )
            {
            RUBY_ERROR1( "CTonePlayer::InitToneL - err [%d]", err );
                    
            // Cannot play sound, but observer still needs to be notified
            iObserver->MapcInitComplete( err, 0 );
            }
        }
    }

// ---------------------------------------------------------
// CTonePlayer::PlayTone
// Plays the appropriate tone
// ---------------------------------------------------------
//
void CTonePlayer::PlayTone( TInt aSid )
    {
    RUBY_DEBUG0( "CTonePlayer::PlayTone START" );
    
    switch ( aSid )
        {
        case EAvkonSIDNameDiallerStartTone:
        case EAvkonSIDConfirmationTone:
            {
            iAudioPlayer->SetVolume( ScaledVolume() );
            iAudioPlayer->Play();
            break;
            }
            
        case EAvkonSIDNameDiallerAbortTone:
        case EAvkonSIDNameDiallerErrorTone:
            {
            if ( iAvkonAppUiBase )
                {               
                iAudioPlayer->Stop();
                iAudioPlayer->Close();
                
                iAvkonAppUiBase->KeySounds()->PlaySound( aSid );
                }

            MapcPlayComplete( KErrNone );
            break;
            }
        
        default:
            break;
        }
    
    RUBY_DEBUG0( "CTonePlayer::PlayTone EXIT" );
    }

// ---------------------------------------------------------------------------
// CTonePlayer::MapcInitComplete
// CMdaAudioPlayerUtility initialization complete
// ---------------------------------------------------------------------------
//
void CTonePlayer::MapcInitComplete( TInt aError,
                                    const TTimeIntervalMicroSeconds& aDuration )
    {
    RUBY_DEBUG0( "CTonePlayer::MapcInitComplete START" );

    iObserver->MapcInitComplete( aError, aDuration );
    
    RUBY_DEBUG0( "CTonePlayer::MapcInitComplete EXIT" );
    }

// ---------------------------------------------------------------------------
// CTonePlayer::MapcPlayComplete
// Playback complete, notify observer
// ---------------------------------------------------------------------------
//
void CTonePlayer::MapcPlayComplete( TInt aError )
    {
    RUBY_DEBUG0( "CTonePlayer::MapcPlayComplete START" );

    iAudioPlayer->Stop();
    iAudioPlayer->Close();
    iObserver->MapcPlayComplete( aError );

    RUBY_DEBUG0( "CTonePlayer::MapcPlayComplete EXIT" );
    }

// ---------------------------------------------------------------------------
// CTonePlayer::ScaledVolume
// ---------------------------------------------------------------------------
//    
TInt CTonePlayer::ScaledVolume()
    {
    TInt volume = iAudioPlayer->MaxVolume();
    if ( iVolume >= 0 )
        {
        // Scale value
        volume = ( iVolume * volume ) / KMaxRepositoryVolume;
        }
        
    return volume;
    }

// ---------------------------------------------------------
// CTonePlayer::ChekcAccesoryStatus
// ---------------------------------------------------------
//  
void CTonePlayer::ChekcAccesoryStatusL()
    {    
    // Connection to the accessory server
    RAccessoryServer accessoryServer;    
    // Subsession for server
    RAccessoryConnection accessoryConnection;    
    //Generic ID array for accessories
    TAccPolGenericIDArray accessoryArray;
    
    TInt err = KErrNone;
    //connect to accessory server
    err = accessoryServer.Connect();
    if ( err == KErrNone )
        {
        //create subsession to accessory server
        err = accessoryConnection.CreateSubSession( accessoryServer );
        if ( err == KErrNone )
            {
            accessoryConnection.GetAccessoryConnectionStatus( accessoryArray );
            
            TAccPolGenericID genId;
            for ( TInt i = 0; i < accessoryArray.Count(); i++ )
                {
                genId = accessoryArray.GetGenericIDL( i );

                if ( genId.PhysicalConnectionCaps( KPCBluetooth ) && 
                    ( genId.DeviceTypeCaps( KDTHeadset ) || genId.DeviceTypeCaps( KDTCarKit ) ) )
                    {
                    iBTAcc = ETrue;
                    }
                }
            }
        }

    accessoryConnection.CloseSubSession();
    accessoryServer.Close();
    }

//  End of File

