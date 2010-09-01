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
* Description:  VCommandManager Application UI class
*
*/


// INCLUDE FILES
#include <aknenv.h>
#include <akntitle.h>
#include <f32file.h>
#include <apgwgnam.h>
#include <AknSgcc.h>
#include <nssvasccontacthandler.h>
#include <data_caging_path_literals.hrh>
#include "nssvoiceuipluginhandler.h"
#include "rubydebug.h"
#include "vcmanagerappui.h"
#include "vcommandrecogmanager.h"
#include "vcommandmanager.h"
#include "vcmanagervoicekeylauncher.h"
#include "vcmanagervoiceheadsetlauncher.h"
#include "vcmanagercontactselecthandler_aiw.h"

#include "vcmanagerbackupobserver.h"

// CONSTANTS
_LIT( KVCThreadName, "VCManagerThread" );
const TThreadPriority KVCThreadPriority = EPriorityAbsoluteLow;

// Thread heap min-max
const TInt KVCHeapMinSize = KMinHeapSize;
const TInt KVCHeapMaxSize = 0x200000;       // Max size is set to 2MB

// -----------------------------------------------------------------------------
// CVCommandManagerAppUi::StartServicesL
// Starts SIND services
// -----------------------------------------------------------------------------
//
void CVCommandManagerAppUi::StartServicesL()
    {
    RUBY_DEBUG0( "CVCommandManagerAppUi::StarServices" );

    TInt error = KErrNone;

    TSecureId sid = RThread().SecureId();
    RUBY_DEBUG1( "VCommandManager secure id [%x]", sid.iId );

    // Create thread which starts to run the xml parsing
    RThread parserThread;

    error = parserThread.Create( KVCThreadName, CVCommandManager::VCMThreadFunction, 
                                 KDefaultStackSize, KVCHeapMinSize, KVCHeapMaxSize, NULL );
    CleanupClosePushL( parserThread );
                             
    if ( error == KErrNone )
        {
        parserThread.SetPriority( KVCThreadPriority );
        
        // Create VoiceUI Plug-in handler
        iPluginHandler = CNssVoiceUIPluginHandler::NewL();
    
        // Recognition selection handling
        iRecogManager = CVCRecognitionManager::NewL();
       
        // Create backup/restore observer
        iBackupObserver = CVCommandManagerBackupObserver::NewL();
        
        // Create dialer
        RUBY_DEBUG0( "CVCommandManagerAppUi::StarServices creating dialer" ); 
        
        iVasDbManager = CNssVASDBMgr::NewL();
        iVasDbManager->InitializeL();
        iContextManager = iVasDbManager->GetContextMgr();
      
        iSelectNotificationBuilder = CNssSelectNotificationBuilder::NewL();
        iSelectNotificationBuilder->InitializeL();
        iTagSelectNotification = iSelectNotificationBuilder->GetTagSelectNotification();

        iContactSelectHandler = CVcManagerContactSelectHandler::NewL( iContextManager, 
                                                                      iTagSelectNotification );
                                                                              
        delete iVasDbManager;
        iVasDbManager = NULL;
        
        iVoiceKeyLauncher = CVCommandManagerVoiceKeyLauncher::NewL();
        iVoiceHeadSetLauncher = CVCommandManagerVoiceHeadSetLauncher::NewL();
        
        // Dialer created
        RUBY_DEBUG0( "CVCommandManagerAppUi::StarServices dialer created" );        
        
        // Create VAS contact handler
        iContactHandler = CNssContactHandler::NewL();
        
        // Start XML parsing
        parserThread.Resume();
        
        CleanupStack::PopAndDestroy( &parserThread );
        }
    else
        {        	
        RUBY_ERROR0( "CVCommandManagerAppUi::StarServices cannot create thread" );
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCommandManagerAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVCommandManagerAppUi::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandManagerAppUi::ConstructL version 9" );
    
    // Remember the idle status pane resource id
    iInitialStatuspaneResourceId = CAknSgcClient::CurrentStatuspaneResource();
        
    // Cannot set ENoScreenFurniture here since after that StatusPane() function
    // returns NULL. "Voice mailbox" command requires status pane to be available
    // when executing.
    // ENoAppResourceFile seems to have no effect -> not setting that either
    BaseConstructL( EAknEnableSkin );

    // Set this as system application    
    iEikonEnv->SetSystem( ETrue );
    
    iEikonEnv->SetAutoForwarding( ETrue );
    
    UpdateStatusPaneL();
   
    StartServicesL();
    }

// -----------------------------------------------------------------------------
// CVCommandManagerAppUi::~CVCommandManagerAppUi
// Destructor
// -----------------------------------------------------------------------------
//
CVCommandManagerAppUi::~CVCommandManagerAppUi()
    {  
    RUBY_DEBUG0( "CVCommandManagerAppUi::~CVCommandManagerAppUi" );
    // Destroy VAS contact handler
    delete iContactHandler;
    // Destroy VAS global context
    delete iRecogManager;
    // Destroy VoiceUI Plug-in handler
    delete iPluginHandler;
    
    delete iContactSelectHandler;
    delete iSelectNotificationBuilder;
    delete iVasDbManager;    
    delete iBackupObserver;
    delete iVoiceKeyLauncher;
    delete iVoiceHeadSetLauncher;
    }
    
// -----------------------------------------------------------------------------
// CVCommandManagerAppUi::EikonEnv
// Returns the CEikonEnv
// -----------------------------------------------------------------------------
//    
CEikonEnv* CVCommandManagerAppUi::EikonEnv() const
    {
    return iEikonEnv;
    }    
    
// -----------------------------------------------------------------------------
// CVCommandManagerAppUi::UpdateStatusPaneL
// Switch to initial status pane
// -----------------------------------------------------------------------------
//     
void CVCommandManagerAppUi::UpdateStatusPaneL()
    {
    // Use same status pane as idle has
    StatusPane()->SwitchLayoutL( iInitialStatuspaneResourceId );
    
    // Title pane is set as empty
    CAknTitlePane* title =
        static_cast<CAknTitlePane*>(
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        
    //const TDesC* temp = title->Text();
    title->SetTextL( KNullDesC );
    }

// -----------------------------------------------------------------------------
// CVCommandManagerAppUi::HandleScreenDeviceChangedL
// -----------------------------------------------------------------------------
//     
void CVCommandManagerAppUi::HandleScreenDeviceChangedL()
    {
    CAknAppUiBase::HandleScreenDeviceChangedL();
    }

// -----------------------------------------------------------------------------
// CVCommandManagerAppUi::HandleCommandL
// Handle events from app framework
// -----------------------------------------------------------------------------
//      
void CVCommandManagerAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            Exit();
            break;
            }

        default:
            break;
        }
    }  
      
    
//  End of File
