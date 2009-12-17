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
* Description:  vcexecutorapp application UI class
*
*/


// INCLUDE FILES
#include <aknenv.h>
#include <akntitle.h>
#include <f32file.h>
#include <apgwgnam.h>
#include <AknSgcc.h>
#include <data_caging_path_literals.hrh>

#include <vcexecutorapp.rsg>

#include "vcexecutorappui.h"
#include "appcontroller.h"
#include "rubydebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCExecutorAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVCExecutorAppUi::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVCExecutorAppUi::ConstructL" );
    
    // Remember the idle status pane resource id
    iInitialStatuspaneResourceId = CAknSgcClient::CurrentStatuspaneResource();

   
    // Cannot set ENoScreenFurniture here since after that StatusPane() function
    // returns NULL. "Voice mailbox" command requires status pane to be available
    // when executing.
    // ENoAppResourceFile seems to have no effect -> not setting that either
    BaseConstructL( EAknEnableSkin | EAknEnableMSK /*ENoScreenFurniture*/ );

    // Set the title pane text to empty so that the default "vcexecutorapp" text
    // won't be shown when activating applications.
    CAknTitlePane* title =
        static_cast<CAknTitlePane*>( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        
    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC( rReader, R_EMPTY_TITLE_PANE );
    title->SetFromResourceL( rReader );
    CleanupStack::PopAndDestroy(); // rReader

    // Set this as system application    
    iEikonEnv->SetSystem( ETrue );
    
    UpdateStatusPaneL();
    }
    
// -----------------------------------------------------------------------------
// CVCExecutorAppUi::~CVCExecutorAppUi
// Destructor
// -----------------------------------------------------------------------------
//
CVCExecutorAppUi::~CVCExecutorAppUi()
    {  
    }
    
// -----------------------------------------------------------------------------
// CVCExecutorAppUi::EikonEnv
// Returns the CEikonEnv
// -----------------------------------------------------------------------------
//    
CEikonEnv* CVCExecutorAppUi::EikonEnv() const
    {
    return iEikonEnv;
    }    
    
// -----------------------------------------------------------------------------
// CVCExecutorAppUi::UpdateStatusPaneL
// Switch to initial status pane
// -----------------------------------------------------------------------------
//     
void CVCExecutorAppUi::UpdateStatusPaneL()
    {
    RUBY_DEBUG_BLOCKL( "CVCExecutorAppUi::UpdateStatusPaneL" );
    
    // Use same status pane as idle has
    StatusPane()->SwitchLayoutL( iInitialStatuspaneResourceId );
    
    // Title pane is set as empty
    CAknTitlePane* title = static_cast<CAknTitlePane*> ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( KNullDesC );
    
    StatusPane()->MakeVisible( EFalse ); 
    }

// -----------------------------------------------------------------------------
// CVCExecutorAppUi::HandleScreenDeviceChangedL
// Empty implementation to prevent crash when changing screen resolution
// -----------------------------------------------------------------------------
//     
void CVCExecutorAppUi::HandleScreenDeviceChangedL()
    {
    CAknAppUi::HandleScreenDeviceChangedL();
    }

// -----------------------------------------------------------------------------
// CVCExecutorAppUi::HandleCommandL
// Handle events from app framework
// -----------------------------------------------------------------------------
//      
void CVCExecutorAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }

        default:
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// CVCExecutorAppUi::ProcessCommandParametersL
//
// -----------------------------------------------------------------------------
//         
TBool CVCExecutorAppUi::ProcessCommandParametersL( CApaCommandLine& aCommandLine )
    {
    RUBY_DEBUG_BLOCKL( "CVCExecutorAppUi::ProcessCommandParametersL" );
    
    TPtrC8 argumentPtr( aCommandLine.TailEnd() );
    
    // Convert argument to TInt
    TLex8 lex( argumentPtr );
    TInt commandId;
    User::LeaveIfError( lex.Val( commandId ) );
    
    // Execute command with CVCApplicationController
    CVCApplicationControllerImpl* appController = CVCApplicationControllerImpl::NewL();
    CleanupStack::PushL( appController );
    appController->ExecuteCommandL( commandId, KNullDesC );
    
    CleanupStack::PopAndDestroy( appController );
    
    // Exit application
    Exit();
    
    return EFalse;
    }
    
//  End of File
