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
* Description:  
*
*/


// INCLUDE FILES

#include <coreapplicationuisdomainpskeys.h>
#include <ctsydomainpskeys.h>
#include <PSVariables.h>        // Property values
#include <apgwgnam.h> 
#include <coemain.h>
#include <coedef.h>

#include "vuicstate.h"
#include "vuicprecheckstate.h"
#include "vuiccontextcheckstate.h"
#include "vuicexitstate.h"
#include "vuicerrorstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"

#include "rubydebug.h"
    
//fix for the error ECWG-7WDA9G
const TUid KAknnfysrvUid = {0x10281ef2};
// -----------------------------------------------------------------------------
// CPrecheckState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPrecheckState* CPrecheckState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CPrecheckState* self = new (ELeave) CPrecheckState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CPrecheckState::~CPrecheckState()
    {
    RUBY_DEBUG0( "CPrecheckState::~CPrecheckState START" );
    
    RUBY_DEBUG0( "CPrecheckState::~CPrecheckState EXIT" );
    }
    
// ---------------------------------------------------------
// CPrecheckState::HandleEventL
// ---------------------------------------------------------
//       
void CPrecheckState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CPrecheckState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            nextState = CContextCheckState::NewL( DataStorage(), UiModel() );
            break;
        
        case KErrGeneral:
        
            nextState = CExitState::NewL( DataStorage(), UiModel() );
            break;
        
        default:
        
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CPrecheckState::ExecuteL
// ---------------------------------------------------------
//       
void CPrecheckState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CPrecheckState::ExecuteL" );
    
    TInt error = KErrNone;

    // Check if autolock is on
    if ( GetAutoLockValue() > EAutolockOff )
        {
        if ( !DataStorage().DeviceLockMode() )
            {
            error = KErrGeneral;
            }
        }
    else
        {
        DataStorage().SetDeviceLockMode( EFalse );
        }
		
	// Check if lockphone dialog is active
    if ( IsLockPhoneDialogL() )
    	{
    	error = KErrGeneral;
    	}

    // Check if phone or video call is currently active
    TInt state = CheckCallState();
    if ( ( state != EPSCTsyCallStateNone &&
           state != EPSCTsyCallStateUninitialized &&
           state != KErrUnknown ) || IsVideoCall() )
        {
        error = KErrCallInProgress;
        }

    HandleEventL( error );
    }

// ---------------------------------------------------------
// CPrecheckState::CPrecheckState
// ---------------------------------------------------------
//              
CPrecheckState::CPrecheckState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel )
    {
    }
   
// ---------------------------------------------------------
// CPrecheckState::GetAutoLockValue
// ---------------------------------------------------------
//
TInt CPrecheckState::GetAutoLockValue()
    {
    RUBY_DEBUG0( "CPrecheckState::GetAutoLockValue START" );
  
    TInt autoLockValue;
    TInt err = RProperty::Get( KPSUidCoreApplicationUIs,
                               KCoreAppUIsAutolockStatus,
                               autoLockValue );
    if ( err == KErrNotFound )
        {
        autoLockValue = KErrNotFound;
        }
        
    RUBY_DEBUG0( "CPrecheckState::GetAutoLockValue EXIT" );
    
    return autoLockValue;
    }
    
// ---------------------------------------------------------
// CPrecheckState::CheckCallState
// ---------------------------------------------------------
//
TInt CPrecheckState::CheckCallState()
    {
    RUBY_DEBUG0( "CPrecheckState::CheckCallState START" );
    
    TInt callState;
    TInt err = RProperty::Get( KPSUidCtsyCallInformation,
                               KCTsyCallState,
                               callState );
    if ( err == KErrNotFound )
        {
        callState = KErrNotFound;
        }
    
    RUBY_DEBUG0( "CPrecheckState::CheckCallState EXIT" );
    
    return callState;    
    }    
    
// -----------------------------------------------------------------------------
// CPrecheckState::IsVideoCall 
// -----------------------------------------------------------------------------
//
TBool CPrecheckState::IsVideoCall() 
    {
    RUBY_DEBUG0( "CPrecheckState::IsVideoCall START" );
    
    TInt callType;
    RProperty::Get( KPSUidCtsyCallInformation,
                    KCTsyCallType, 
                    callType );// Ignore errors  

    RUBY_DEBUG0( "CPrecheckState::IsVideoCall EXIT" );

    return callType == EPSCTsyCallTypeH324Multimedia;
    }    

// -----------------------------------------------------------------------------
// CPrecheckState::IsLockPhoneDialogL
// -----------------------------------------------------------------------------
//
TBool CPrecheckState::IsLockPhoneDialogL()
	{
	//fix for the error ECWG-7WDA9G,if we found the first windows is "lockphone" dialog we will terminate the App.
	TBool islock = EFalse;
	RWsSession &ws = CCoeEnv::Static()->WsSession();
	TInt wgId = ws.GetFocusWindowGroup(); 
	CApaWindowGroupName *WindowsGroupName = CApaWindowGroupName::NewLC( ws, wgId );
	if ( ( KAknnfysrvUid == WindowsGroupName->AppUid() ) && 
	      ( ws.GetWindowGroupOrdinalPriority( wgId ) >= ECoeWinPriorityAlwaysAtFront ) )
	    {
	    islock = ETrue;
	    }
	CleanupStack::PopAndDestroy( WindowsGroupName );
	return islock;
	}
    
// End of File

