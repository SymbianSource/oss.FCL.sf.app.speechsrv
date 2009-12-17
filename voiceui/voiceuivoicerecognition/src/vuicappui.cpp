/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The Voice Recognition Application Ui
*
*/


// INCLUDE FILES
#include <featmgr.h>

#include <e32property.h>
#include <ScreensaverInternalPSKeys.h>
#include <coreapplicationuisdomainpskeys.h>

#include <vuivoicerecogdefs.h>

#include "vuicappui.h"
#include "vuicvoicerecogdialogimpl.h"

#include "vuiccalldetector.h"
#include "vuickeygrabber.h"
#include "vuicfoldobserver.h"
#include "vuicnotificationobserver.h"

#include "rubydebug.h"

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------------------------
// CAppUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CAppUi::ConstructL()
    {
    SetFullScreenApp( EFalse );

    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();
    
    RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 1 );
    RProperty::Set( KPSUidScreenSaver, KScreenSaverNoPowerSaveMode, 1 );
    RProperty::Set( KPSUidCoreApplicationUIs, KLightsControl, ELightsOn );
    
    iNotificationObserver = CNotificationObserver::NewL();
    
    iCallDetector = CCallDetector::NewL( this );
    iCallDetector->RequestNotification();
    
    iKeyGrabber = CKeyGrabber::NewL( this );
    
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
	    {
        iFoldObserver = CFoldObserver::NewL( this );
        }
    
    SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort | CAknAppUiBase::EDisableSendKeyLong );
    
    iEikonEnv->SetAutoForwarding( ETrue );
    
    iRecognitionDialog = CVoiceRecognitionDialogImpl::NewL( this );
    
    iRecognitionDialog->ExecuteL();
    }

// ----------------------------------------------------------------------------
// CAppUi::~CAppUi()
// Destructor
// ----------------------------------------------------------------------------
//
CAppUi::~CAppUi() 
    {
    RProperty::Set( KPSUidScreenSaver, KScreenSaverNoPowerSaveMode, 0 );
    RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 0 );

    FeatureManager::UnInitializeLib();
    
    delete iRecognitionDialog;
    
    delete iNotificationObserver;
    
    delete iKeyGrabber;
    delete iFoldObserver;
    
    if ( iCallDetector )
        {
        iCallDetector->Cancel();
        delete iCallDetector;
        }
    }

// ----------------------------------------------------------------------------
// CAppUi::DialogDismissed
// ----------------------------------------------------------------------------
//
void CAppUi::DialogDismissed()
    {
    RUBY_DEBUG0( "CAppUi::DialogDismissed START" );
    
    TRAPD( error, HandleCommandL( EAknSoftkeyExit ) );
    if ( error != KErrNone )
        {
        Exit();
        }
    
    RUBY_DEBUG0( "CAppUi::DialogDismissed EXIT" );
    }

// ----------------------------------------------------------------------------
// CAppUi::DialogDismissedL
// ----------------------------------------------------------------------------
//    
void CAppUi::CallDetectedL()
    {
    RUBY_DEBUG_BLOCK( "CAppUi::CallDetectedL" );
    
    HandleCommandL( EAknSoftkeyExit );
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleKeypressL
// ----------------------------------------------------------------------------
//     
void CAppUi::HandleKeypressL( TInt /*aSoftkey*/ )
    {
    RUBY_DEBUG_BLOCK( "CAppUi::HandleKeypressL" );
    
    HandleCommandL( EAknSoftkeyExit );
    }

// ----------------------------------------------------------------------------
// CAppUi::HandleCommandL
// ----------------------------------------------------------------------------
//
void CAppUi::HandleCommandL( TInt aCommand )
    {
    RUBY_DEBUG_BLOCK( "CAppUi::HandleCommandL" );
    
    switch ( aCommand )
        {
        // Both exit commands should do the same thing
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
            
        default:
            {
            CAknAppUi::HandleCommandL( aCommand );
            break;              
            }
        }
    }

// -----------------------------------------------------------------------------
// CAppUi::ProcessCommandParametersL
// -----------------------------------------------------------------------------
//         
TBool CAppUi::ProcessCommandParametersL( CApaCommandLine& aCommandLine )
    {
    RUBY_DEBUG_BLOCK( "CAppUi::ProcessCommandParametersL" );

    TPtrC8 argumentPtr( aCommandLine.TailEnd() );
    
    if ( argumentPtr.Length() && !argumentPtr.Compare( KVoiceUiMode ) )
        {
        iEikonEnv->SetAutoForwarding( EFalse );
        iRecognitionDialog->SetOnlyCallsAllowed();
        }

    return ETrue;
    }
    
// ----------------------------------------------------------------------------
// CAppUi::PrepareToExit
// ----------------------------------------------------------------------------
//
void CAppUi::PrepareToExit( )
    {
    RUBY_DEBUG0( "CAppUi::PrepareToExit START" );

    delete iRecognitionDialog;
    iRecognitionDialog = NULL;
    
    CAknAppUi::PrepareToExit();
    	 
	RUBY_DEBUG0( "CAppUi::PrepareToExit Exit" );
    } 
// End of File  

