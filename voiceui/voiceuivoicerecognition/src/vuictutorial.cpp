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
* Description:  Handles tutorial mode resources and dialog showing
*
*/


// INCLUDE FILES

#include <avkon.rsg>
#include <avkon.hrh>
#include <centralrepository.h>
#include <vcommanddomaincrkeys.h>
#include <e32property.h>
#include <ScreensaverInternalPSKeys.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuictutorial.h"
#include "vuictutorialmessagedialog.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CTutorial::InstanceL
// -----------------------------------------------------------------------------
//
CTutorial* CTutorial::NewL()
    {
    CTutorial* self = new ( ELeave ) CTutorial();
    CleanupStack::PushL( self ); 
    self->ConstructL(); 
    CleanupStack::Pop( self ); 
    return self; 
    }
    
// Destructor       
CTutorial::~CTutorial()
    {
    RUBY_DEBUG0( "CTutorial::~CTutorial START" );
    
    // Save tutorial mode settings before exiting
    TRAP_IGNORE( SaveSettingsL() );
    
    DestroyMessage();
    
    RUBY_DEBUG0( "CTutorial::~CTutorial EXIT" );
    }

// -----------------------------------------------------------------------------
// CTutorial::CreateMessageL
// -----------------------------------------------------------------------------
//
void CTutorial::CreateMessageL()
    {
    RUBY_DEBUG_BLOCK( "CTutorial::CreateMessageL" );
    
    if ( iDlg )
        {
        User::Leave( KErrAlreadyExists );
        }
       
    iDlg = CTutorialMessageDialog::NewL();
    }

// -----------------------------------------------------------------------------
// CTutorial::CreateAndShowMessageL
// -----------------------------------------------------------------------------
//
TInt CTutorial::CreateAndShowMessageL()
    {
    RUBY_DEBUG_BLOCK( "CTutorial::CreateAndShowMessageL" );
    
    CreateMessageL();
    return ShowMessageL();
    }

// -----------------------------------------------------------------------------
// CTutorial::ShowMessageL
// -----------------------------------------------------------------------------
//
TInt CTutorial::ShowMessageL( TBool aDeviceLockMode )
    {
    RUBY_DEBUG_BLOCK( "CTutorial::ShowMessageL" );
    
    TInt returnValue = KErrGeneral;    
    
    switch( iState )
        {
        case EIntroState:
            
            if ( IsTutorialModeEnabled() && iTutorialMode )
                {
                iState = ECommandState;
                returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_INTRO_TITLE,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_INTRO_TEXT_UP,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_INTRO_TEXT_DOWN,
                                                        R_SOFTKEYS_NEXT_CANCEL__NEXT );
                }
            else if ( IsTutorialModeEnabled() && !aDeviceLockMode )
                {
                iState = ETryState;
                returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_INTRO_TITLE,
                                                        R_TEXT_SIND_TUTORIAL_FIRST_TIME_TEXT_UP,
                                                        R_TEXT_SIND_TUTORIAL_FIRST_TIME_TEXT_DOWN,
                                                        R_SOFTKEYS_ACTIVATE_CANCEL__ACTIVATE,
                                                        SecondaryDisplay::ECmdShowFirstTimeActivationQuery );
                if ( returnValue == EVoiceInfoSoftKeyActivate )
                    {
                    DisableFirstTimeMode();
                    }
                }
            else 
                {
                iState = ETryState;
                }
            break;
            
        case ECommandState:
            
            iState = EDialState;
            returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_COMMAND_TITLE,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_COMMAND_TEXT_UP,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_COMMAND_TEXT_DOWN,
                                                    R_SOFTKEYS_NEXT_CANCEL__NEXT );
            break;
            
        case EDialState:
            
            iState = EOtherState;
            returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_DIAL_TITLE,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_DIAL_TEXT_UP,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_DIAL_TEXT_DOWN,
                                                    R_SOFTKEYS_NEXT_CANCEL__NEXT );
            break;

        case EOtherState:
            
            iState = ETryState;
            returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_OTHER_TITLE,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_OTHER_TEXT_UP,
                                                    R_TEXT_SIND_TUTORIAL_PAGE_OTHER_TEXT_DOWN,
                                                    R_SOFTKEYS_NEXT_CANCEL__NEXT );
            break;

        case ETryState:
            
            iState = EBestState;
            if ( IsTutorialModeEnabled() && !aDeviceLockMode )
                {
                returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_TRY_TITLE,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_TRY_TEXT_UP,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_TRY_TEXT_DOWN,
                                                        R_SOFTKEYS_ACTIVATE_CANCEL__ACTIVATE );
                }
            break;

        case EBestState:
            
            iState = EActivationState;
            if ( IsTutorialModeEnabled() && !aDeviceLockMode )
                {
                returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_BEST_TITLE,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_BEST_TEXT_UP,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_BEST_TEXT_DOWN,
                                                        R_SOFTKEYS_NEXT_CANCEL__NEXT );
                }
            break;

        case EActivationState:
            
            iState = EFinishedState;
            if ( IsTutorialModeEnabled() && !aDeviceLockMode )
                {
                returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_ACTIVATION_TITLE,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_ACTIVATION_TEXT,
                                                        KErrNone,
                                                        R_AVKON_SOFTKEYS_QUIT );
                }
            break;

        case EErrorState:
            
            iState = EFinishedState;
            if ( IsTutorialModeEnabled() && !aDeviceLockMode )
                {
                returnValue = iDlg->ShowMessageDialogL( R_MESSAGE_QUERY,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_ERROR_TITLE,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_ERROR_TEXT_UP,
                                                        R_TEXT_SIND_TUTORIAL_PAGE_ERROR_TEXT_DOWN,
                                                        R_SOFTKEYS_NEXT_CANCEL__NEXT );
                if ( returnValue == EVoiceInfoSoftKeyNext )
                    {
                    iState = ETryState;
                    }
                }
            break;
            
        default:
            returnValue = KErrCancel;
            break;
        }
        
        if ( returnValue == EVoiceInfoSoftKeyCancel || returnValue == EAknSoftkeyQuit )
            {
            iTutorialMode = EFalse;
            returnValue = KErrCancel;
            }
        else if ( returnValue == KErrNone )
            {
            // Return general error because dialog has been canceled without any keypress
            returnValue = KErrGeneral;
            }
        else if ( returnValue != KErrCancel )
            {
            returnValue = KErrNone;
            }
        
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CTutorial::DestroyMessage
// -----------------------------------------------------------------------------
//
void CTutorial::DestroyMessage()
    {
    RUBY_DEBUG0( "CTutorial::DestroyMessage START" );
    
    if ( iDlg )
        {
        delete iDlg;
        iDlg = NULL;
        }
    
    RUBY_DEBUG0( "CTutorial::DestroyMessage EXIT" );    
    }

// -----------------------------------------------------------------------------
// CTutorial::GetTutorialState
// -----------------------------------------------------------------------------
//
CTutorial::TTutorialState CTutorial::GetTutorialState()
    {
    RUBY_DEBUG0( "CTutorial::GetTutorialState" );
    return iState;
    }

// -----------------------------------------------------------------------------
// CTutorial::ShowError
// -----------------------------------------------------------------------------
//
void CTutorial::ErrorState()
    {
    RUBY_DEBUG0( "CTutorial::ErrorState" );
    iState = EErrorState;
    }

// -----------------------------------------------------------------------------
// CTutorial::IsTutorialModeEnabled
// -----------------------------------------------------------------------------
//
TBool CTutorial::IsTutorialModeEnabled()
    {
    RUBY_DEBUG0( "CTutorial::IsTutorialModeEnabled" );
    return iTutorialMode || iFirstTimeMode;
    }

// -----------------------------------------------------------------------------
// CTutorial::DisableTutorialMode
// -----------------------------------------------------------------------------
//
void CTutorial::DisableFirstTimeMode()
    {
    RUBY_DEBUG0( "CTutorial::DisableFirstTimeMode" );
    if ( !iTutorialMode )
        {
        iFirstTimeMode = EFalse;
        
        TRAP_IGNORE( SaveSettingsL() );
        }    
    }

// -----------------------------------------------------------------------------
// CTutorial::CTutorial
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTutorial::CTutorial()
 : iTutorialMode( EFalse ), iFirstTimeMode( EFalse ), iState( EIntroState )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTutorialMessageDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTutorial::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CTutorial::ConstructL" );
    
    LoadSettingsL();
    }

// ---------------------------------------------------------------------------
// CTutorial::LoadSettingsL
// ---------------------------------------------------------------------------
//
void CTutorial::LoadSettingsL()
    {
    RUBY_DEBUG_BLOCK( "CTutorial::LoadSettingsL" );
    
    CRepository* client = CRepository::NewLC( KCRUidVCommandSettings );
    
    User::LeaveIfError( client->Get( KVuiDemoFirstTime,
                                     iFirstTimeMode ) );
    
    User::LeaveIfError( client->Get( KVuiDemoMode,
                                     iTutorialMode ) );

    CleanupStack::PopAndDestroy( client );
    
    if ( iTutorialMode )
        {
        //Allow screensaver if in tutorial mode
        RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 0 );
        }
    }

// ---------------------------------------------------------------------------
// CTutorial::SaveSettingsL
// ---------------------------------------------------------------------------
//
void CTutorial::SaveSettingsL()
    {
    RUBY_DEBUG_BLOCK( "CTutorial::SaveSettingsL" );
    
    CRepository* client = CRepository::NewLC( KCRUidVCommandSettings );

    User::LeaveIfError( client->Set( KVuiDemoFirstTime,
                                     iFirstTimeMode ) );
                        
    User::LeaveIfError( client->Set( KVuiDemoMode,
                                     EFalse ) );

    CleanupStack::PopAndDestroy( client );
    }

// End of File

