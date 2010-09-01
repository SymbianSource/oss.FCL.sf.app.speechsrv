/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer voice key presses
*
*/


#include "vcmanagervoicekeylauncher.h"
#include "rubydebug.h"
#include <featmgr.h>
#include <eikappui.h>
#include <apgcli.h>
#include <apgtask.h>
#include <apacmdln.h>
#include <apgwgnam.h>


// CONSTANTS
const TInt KCapturePriority( 255 );
const TUint KLongVoiceKey( 0x102818E7 ); // unique (UID of this exe)
const TUid KVoiceUiUID = { 0x101F8543 };

// ---------------------------------------------------------
// CVCommandManagerVoiceKeyLauncher::NewL
// ---------------------------------------------------------
//
CVCommandManagerVoiceKeyLauncher* CVCommandManagerVoiceKeyLauncher::NewL()
    {
    CVCommandManagerVoiceKeyLauncher* self = new (ELeave) CVCommandManagerVoiceKeyLauncher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CVCommandManagerVoiceKeyLauncher::CVCommandManagerVoiceKeyLauncher
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
CVCommandManagerVoiceKeyLauncher::CVCommandManagerVoiceKeyLauncher()
    : CActive( EPriorityStandard )
    {
    // nothing
    }
    
// ---------------------------------------------------------
// CVCommandManagerVoiceKeyLauncher::ConstructL
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
void CVCommandManagerVoiceKeyLauncher::ConstructL() 
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // Check if the voice key exist
    iKeyEnabled = FeatureManager::FeatureSupported( KFeatureIdKeypadNoVoiceKey );
    if ( iKeyEnabled )
        {
        User::LeaveIfError( iWsSession.Connect() );
        
        iWindowGroup = RWindowGroup ( iWsSession );
        iWindowGroup.Construct( (TUint32) &iWindowGroup, EFalse );
        
        iWindowGroup.SetOrdinalPosition( -1 );
        iWindowGroup.EnableReceiptOfFocus( EFalse );
    
        iKeyCaptureHandle = iWindowGroup.CaptureKey( EKeyDevice6, 0, 0 );
        iLongKeyCaptureHandle = iWindowGroup.CaptureLongKey( (TUint)EKeyDevice6, 
                                    KLongVoiceKey, 0, 0, KCapturePriority, ELongCaptureNormal );
                                    
        CApaWindowGroupName* name = CApaWindowGroupName::NewLC( iWsSession );
        name->SetHidden( ETrue );
        name->SetWindowGroupName( iWindowGroup );
        CleanupStack::PopAndDestroy( name );
                                    
        iWsSession.EventReady( &iStatus );
     
        CActiveScheduler::Add( this );
        SetActive();

        }
  
    }
    
// ---------------------------------------------------------
// CVCommandManagerVoiceKeyLauncher::~CVCommandManagerVoiceKeyLauncher
// ---------------------------------------------------------
//
CVCommandManagerVoiceKeyLauncher::~CVCommandManagerVoiceKeyLauncher()
    {
    RUBY_DEBUG0( "START" );
    if ( iKeyEnabled )
        {        
        Cancel();
        iWindowGroup.Close();        
        iWsSession.Close();        
        }
        
    RUBY_DEBUG0( "EXIT" );
    }
    

// ---------------------------------------------------------
// CVCommandManagerVoiceKeyLauncher::DoCancel
// ---------------------------------------------------------
// 
void CVCommandManagerVoiceKeyLauncher::DoCancel()
    {
    RUBY_DEBUG0( "START" );
    
    
    if ( iKeyEnabled )
        {
        iWsSession.EventReadyCancel();
        iWindowGroup.CancelCaptureKey( iKeyCaptureHandle );
        iWindowGroup.CancelCaptureKey( iLongKeyCaptureHandle );       
        }    
    
    RUBY_DEBUG0( "EXIT" );
    }

// ---------------------------------------------------------
// CVCommandManagerVoiceKeyLauncher::RunL
// ---------------------------------------------------------
//     
void CVCommandManagerVoiceKeyLauncher::RunL()
    {
    RUBY_DEBUG_BLOCK( "" );
    
    if ( iStatus.Int() == KErrNone )
        {
        TWsEvent event;
        iWsSession.GetEvent( event );
        
        iWsSession.EventReady( &iStatus );
        SetActive();
        
        if ( event.Key()->iCode == KLongVoiceKey )
            {
            //iWsSession.SendEventToOneWindowGroupsPerClient( event );
            
            LaunchVoiceUiL();
            }
        }
    }

    
// ---------------------------------------------------------
// CVCommandManagerVoiceKeyLauncher::LaunchVoiceUiL
// ---------------------------------------------------------
//     
void CVCommandManagerVoiceKeyLauncher::LaunchVoiceUiL()
    {
    RUBY_DEBUG_BLOCK( "" );
   
    TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
    TApaTask apaTask = apaTaskList.FindApp( KVoiceUiUID );
    
    if ( apaTask.Exists() )
        {
        apaTask.BringToForeground();
        }
    else
        {
        RApaLsSession apaLsSession;
        User::LeaveIfError( apaLsSession.Connect() );
        CleanupClosePushL(apaLsSession);
        
        TApaAppInfo appInfo;
        User::LeaveIfError( apaLsSession.GetAppInfo( appInfo, KVoiceUiUID ) );
        
        TFileName appName = appInfo.iFullName;
        CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();

        apaCommandLine->SetExecutableNameL( appName );
        apaCommandLine->SetCommandL( EApaCommandRunWithoutViews );
        User::LeaveIfError ( apaLsSession.StartApp( *apaCommandLine ) );
        CleanupStack::PopAndDestroy( apaCommandLine );
    
        CleanupStack::PopAndDestroy(&apaLsSession);
        }    
    }
    
//  End of File
