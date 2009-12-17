/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer voice headset key presses
*
*/


#include "vcmanagervoiceheadsetlauncher.h"
#include "rubydebug.h"
#include <apgcli.h>
#include <apgtask.h>
#include <apacmdln.h>
#include <remconinterfaceselector.h>
#include <RemConCallHandlingTarget.h>
#include <voiceuidomainpskeys.h>
#include <vuivoicerecogdefs.h>
#include <ctsydomainpskeys.h>
#include <coreapplicationuisdomainpskeys.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::CVCommandManagerVoiceHeadSetLauncher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVCommandManagerVoiceHeadSetLauncher::CVCommandManagerVoiceHeadSetLauncher() 
    {
    }

// -----------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandManagerVoiceHeadSetLauncher::ConstructL" );
    iSelector = CRemConInterfaceSelector::NewL();
    iCallHandlingTarget = CRemConCallHandlingTarget::NewL( *iSelector, *this );
    // Connect to Remote Control server as target. After succesfull call
    // client is ready to receive commands. Call this method only once in a
    // process.
    iSelector->OpenTargetL();
    }

// -----------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCommandManagerVoiceHeadSetLauncher* CVCommandManagerVoiceHeadSetLauncher::NewL()
    {
    CVCommandManagerVoiceHeadSetLauncher* self = new( ELeave ) CVCommandManagerVoiceHeadSetLauncher;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CVCommandManagerVoiceHeadSetLauncher::~CVCommandManagerVoiceHeadSetLauncher()
    {
    RUBY_DEBUG0( "CVCommandManagerVoiceHeadSetLauncher::~CVCommandManagerVoiceHeadSetLauncher - START" );
    
    // iCallHandlingTarget will be destroyed when iSelector is destroyed.  
    delete iSelector;
    
    RUBY_DEBUG0( "CVCommandManagerVoiceHeadSetLauncher::~CVCommandManagerVoiceHeadSetLauncher - EXIT" );
    }


// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::AnswerCall
// ---------------------------------------------------------------------------
//    
void CVCommandManagerVoiceHeadSetLauncher::AnswerCall()
    {
    RUBY_DEBUG0( "" );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExtAnswerCall, KErrNone );
    User::WaitForRequest( response );
    }
    
// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::AnswerCall
// ---------------------------------------------------------------------------
//    
void CVCommandManagerVoiceHeadSetLauncher::AnswerEndCall()
    {
    RUBY_DEBUG0( "" );
    iProperty.Set( KPSUidVoiceUiAccMonitor, KVoiceUiAccessoryEvent, KVoiceUiShortPressEvent );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExtAnswerEnd, KErrNone );
    User::WaitForRequest( response );
    }
    
    
// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::EndCall
// ---------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::EndCall()
    {
    RUBY_DEBUG0( "" );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response,  ERemConExtEndCall, KErrNone );
    User::WaitForRequest( response );
    }
    
// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::LastNumberRedial
// ---------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::LastNumberRedial()
    {
    RUBY_DEBUG0( "" );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExtLastNumberRedial, KErrNone );
    User::WaitForRequest( response );
    }
    
// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::VoiceDial
// ---------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::VoiceDial( const TBool /*aActivate*/ )
    {
    RUBY_DEBUG0( "" );
    if ( GetAutoLockValue() > EAutolockOff )
        {
        TRemConExtCmdSource source;
        iCallHandlingTarget->GetCommandSourceInfo( source );
        
        if ( source == ERemConExtCmdSourceBluetooth )
            {
            TRAP_IGNORE( LaunchVoiceUiL( ETrue ) );
            }
        }
    else
        {
        TRAP_IGNORE( LaunchVoiceUiL( EFalse ) );
        }
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExtVoiceDial, KErrNone );
    User::WaitForRequest( response );
    }

// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::DialCall
// ---------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::DialCall( const TDesC8& /*aTelNumber*/ )
    {
    RUBY_DEBUG0( "CVCommandManagerVoiceHeadSetLauncher::DialCall" );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExtDialCall, KErrNone );
    User::WaitForRequest( response );
    }    
    
// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::MultipartyCalling
// ---------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::MultipartyCalling( const TDesC8& /*aData*/ )
    {
    RUBY_DEBUG0( "" );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExt3WaysCalling, KErrNone );
    User::WaitForRequest( response );
    }
    
// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::GenerateDTMF
// ---------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::GenerateDTMF( const TChar /*aChar*/ )
    {
    RUBY_DEBUG0( "" );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExtGenerateDTMF, KErrNone );
    User::WaitForRequest( response );
    }
    
// ---------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::SpeedDial
// ---------------------------------------------------------------------------
//
void CVCommandManagerVoiceHeadSetLauncher::SpeedDial( const TInt /*aIndex*/ )
    {
    RUBY_DEBUG0( "" );
    TRequestStatus response( KErrNone );
    iCallHandlingTarget->SendResponse( response, ERemConExtSpeedDial, KErrNone );    
    User::WaitForRequest( response );
    }

    
// ---------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::LaunchVoiceUiL
// ---------------------------------------------------------
//     
void CVCommandManagerVoiceHeadSetLauncher::LaunchVoiceUiL( TBool aDeviceLockMode )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // Check that phone or video call is not currently active
    TInt state = CheckCallState();
    if ( ( state == EPSCTsyCallStateNone ||
           state == EPSCTsyCallStateUninitialized ||
           state == KErrUnknown ) && !IsVideoCall() )
        {
        TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
        TApaTask apaTask = apaTaskList.FindApp( KVoiceUiUID );

        if ( apaTask.Exists() )
            {
            apaTask.BringToForeground();
            
            iProperty.Set( KPSUidVoiceUiAccMonitor, KVoiceUiAccessoryEvent, KVoiceUiLongPressEvent );
            }
        else
            {
            RApaLsSession apaLsSession;
            User::LeaveIfError( apaLsSession.Connect() );
            
            TApaAppInfo appInfo;
            User::LeaveIfError( apaLsSession.GetAppInfo( appInfo, KVoiceUiUID ) );
            
            TFileName appName = appInfo.iFullName;
            CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();

            apaCommandLine->SetExecutableNameL( appName );
            apaCommandLine->SetCommandL( EApaCommandRunWithoutViews );
            
            if ( aDeviceLockMode )
                {
                // Command line parameters
                apaCommandLine->SetTailEndL( KVoiceUiMode );
                }
            
            User::LeaveIfError ( apaLsSession.StartApp( *apaCommandLine ) );
            CleanupStack::PopAndDestroy( apaCommandLine );

            apaLsSession.Close();
            }    
        }
    }
    
// ---------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::CheckCallState
// ---------------------------------------------------------
//
TInt CVCommandManagerVoiceHeadSetLauncher::CheckCallState()
    {
    RUBY_DEBUG0( "START" );
    
    TInt callState;
    TInt err = RProperty::Get( KPSUidCtsyCallInformation,
                               KCTsyCallState,
                               callState );
    if ( err == KErrNotFound )
        {
        callState = KErrNotFound;
        }
    
    RUBY_DEBUG0( "EXIT" );
    
    return callState;    
    }    
    
// -----------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::IsVideoCall 
// -----------------------------------------------------------------------------
//
TBool CVCommandManagerVoiceHeadSetLauncher::IsVideoCall() 
    {
    RUBY_DEBUG0( "START" );
    
    TInt callType;
    RProperty::Get( KPSUidCtsyCallInformation,
                    KCTsyCallType, 
                    callType );// Ignore errors  

    RUBY_DEBUG0( "EXIT" );

    return callType == EPSCTsyCallTypeH324Multimedia;
    }

// -----------------------------------------------------------------------------
// CVCommandManagerVoiceHeadSetLauncher::GetAutoLockValue 
// -----------------------------------------------------------------------------
//
TInt CVCommandManagerVoiceHeadSetLauncher::GetAutoLockValue()
    {
    RUBY_DEBUG0( "CVCommandManagerVoiceHeadSetLauncher::GetAutoLockValue START" );
  
    TInt autoLockValue;
    TInt err = RProperty::Get( KPSUidCoreApplicationUIs,
                               KCoreAppUIsAutolockStatus,
                               autoLockValue );
    if ( err == KErrNotFound )
        {
        autoLockValue = KErrNotFound;
        }
        
    RUBY_DEBUG0( "CVCommandManagerVoiceHeadSetLauncher::GetAutoLockValue EXIT" );
    
    return autoLockValue;
    }
    
//  End of File
