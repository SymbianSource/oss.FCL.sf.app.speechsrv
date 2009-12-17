/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DOS Server Monitor Class is used to monitor Audio Accessorries,
*                and encapsulate the interfaces with DOS Server.
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "voiceprogressdialog.h"
#include "playbackvoicecommanddialog.h"
#include "headsetbuttonmonitor.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::CVoiceTagHeadsetButtonMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHeadsetButtonMonitor::CHeadsetButtonMonitor()
   {
   }

// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::~CHeadsetButtonMonitor
// Destructor
// -----------------------------------------------------------------------------
//
CHeadsetButtonMonitor::~CHeadsetButtonMonitor()
    {

    // Cancel the outstanding listening
    CancelMonitor();

    // Close the connection to Dos Server
    iDosServerSession.Close();
    }

// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CHeadsetButtonMonitor::ConstructL" );

    User::LeaveIfError( iDosServerSession.Connect() );
    }


// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::NewLC
// Two-phased constructor.
// This is for new CVoiceTagHeadsetButtonMonitor
// -----------------------------------------------------------------------------
//
CHeadsetButtonMonitor* CHeadsetButtonMonitor::NewLC()
    {
    RUBY_DEBUG_BLOCKL( "CHeadsetButtonMonitor::NewLC" );
    
    CHeadsetButtonMonitor* self = new ( ELeave ) CHeadsetButtonMonitor();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


/// -----------------------------------------------------------------------------
// HeadsetButtonMonitor::NewL
// Two-phased constructor.
// This is for a new DosServer Monitor
// -----------------------------------------------------------------------------
//
CHeadsetButtonMonitor* CHeadsetButtonMonitor::NewL()
    {
    RUBY_DEBUG_BLOCKL( "CHeadsetButtonMonitor::NewL" );
    
    CHeadsetButtonMonitor* self = NewLC();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::RegisterForMonitor
// -----------------------------------------------------------------------------
//
TInt CHeadsetButtonMonitor::ActivateMonitorL()
    {
    RUBY_DEBUG_BLOCKL( "CHeadsetButtonMonitor::ActivateMonitorL" );

    // send Dos Server the new listening list
    StartListeningL( KHeadsetButtonChanged, sizeof( EPSButtonState ), EOnlyLast );

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::DeregisterForMonitor
// -----------------------------------------------------------------------------
//
TInt CHeadsetButtonMonitor::DeActivateMonitor()
    {
    // No more registered event. Stop the event listening
    Stop();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::CancelMonitor
// -----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::CancelMonitor()
    {
    // Stop the event listening
    Stop();
    }

// -----------------------------------------------------------------------------
// CHeadsetButtonMonitor::HeadsetButtonChangedL
// -----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::HeadsetButtonChangedL( EPSButtonState /*aState*/ )
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// CHeadsetButtonMonitor::HandleButtonDown
// ----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::HandleButtonDown()
   {
   iButtonDown = ETrue;
   }

// ----------------------------------------------------------------------------
// CHeadsetButtonMonitor::HandleButtonUp
// ----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::HandleButtonUp()
    {
    // Check iObserver for safety, also made sure we received a button down
    if( ( iPlaybackObserver ) && ( iButtonDown ) ) 
        {
        CancelMonitor();    // Stop the monitoring
        iPlaybackObserver->HandleShortKeypress();
        }
    else if ( ( iProgressObserver ) && ( iButtonDown ) ) 
        {
        CancelMonitor();    // Stop the monitoring
        iProgressObserver->HandleShortKeypress();
        }
  
    iButtonDown = EFalse;
    }


// ----------------------------------------------------------------------------
// CHeadsetButtonMonitor::HandleButtonDownLong
// ----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::HandleButtonDownLong()
    {
    // Check iObserver for safety, also made sure we received a button down
    if ( ( iPlaybackObserver ) && ( iButtonDown ) ) 
        {
        CancelMonitor();       // stop the monitoring 
        iPlaybackObserver->HandleLongKeypress();
        }
    else if ( ( iProgressObserver ) && ( iButtonDown ) ) 
        {
        CancelMonitor();       // stop the monitoring 
        iProgressObserver->HandleLongKeypress();
        }

    iButtonDown = EFalse;
    }


// ----------------------------------------------------------------------------
// CHeadsetButtonMonitor::Register
// ----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::Register( CVoiceProgressDialog* aProgressObserver,
   CPlaybackVoiceCommandDialog* aPlaybackObserver )
   {
   if ( aProgressObserver )
      {
      iProgressObserver = aProgressObserver;
      }
   if ( aPlaybackObserver )
      {
      iPlaybackObserver = aPlaybackObserver;
      }
   }

// ----------------------------------------------------------------------------
// CHeadsetButtonMonitor::DeRegister
// ----------------------------------------------------------------------------
//
void CHeadsetButtonMonitor::DeRegister( CVoiceProgressDialog* /*aProgressObserver*/,
   CPlaybackVoiceCommandDialog* /*aPlaybackObserver*/ )
   {
   iProgressObserver = NULL;
   iPlaybackObserver = NULL;
   }


// End of File
