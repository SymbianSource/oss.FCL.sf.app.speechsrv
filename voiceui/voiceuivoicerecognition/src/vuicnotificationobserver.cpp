/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CNBestListDialog
*
*/

// INCLUDE FILES
#include "vuicnotificationobserver.h"

#include <avkondomainpskeys.h>

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CNotificationObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNotificationObserver* CNotificationObserver::NewL()
    {
    CNotificationObserver* self = new (ELeave) CNotificationObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    } 

// Destructor       
CNotificationObserver::~CNotificationObserver()
    {
    RUBY_DEBUG0( "CNotificationObserver::~CNotificationObserver START" );
    
    Cancel();
    
    iNotificationState.Set( iNotificationStatus );
    
    iNotificationState.Close();
    
    RUBY_DEBUG0( "CNotificationObserver::~CNotificationObserver EXIT" );
    }

// ---------------------------------------------------------
// CNotificationObserver::DoCancel
// ---------------------------------------------------------
// 
void CNotificationObserver::DoCancel()
    {
    RUBY_DEBUG0( "CNotificationObserver::DoCancel START" );
    
    iNotificationState.Cancel();
    
    RUBY_DEBUG0( "CNotificationObserver::DoCancel EXIT" );
    }

// ---------------------------------------------------------
// CNotificationObserver::RunL
// ---------------------------------------------------------
//     
void CNotificationObserver::RunL()
    {
    RUBY_DEBUG_BLOCK( "CNotificationObserver::RunL" );
    
    TInt status = iStatus.Int();
    
    if ( status == KErrNone )
        {
        iNotificationState.Get( iNotificationStatus );
        
        if ( iNotificationStatus )
            {
            iNotificationState.Set( 0 );
            }
        }
        
    iNotificationState.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CNotificationObserver::CNotificationObserver
// ---------------------------------------------------------
//              
CNotificationObserver::CNotificationObserver()
 : CActive( EPriorityStandard )
    {
    }
    
// ---------------------------------------------------------
// CNotificationObserver::ConstructL
// ---------------------------------------------------------
//           
void CNotificationObserver::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CNotificationObserver::ConstructL" );
    
    User::LeaveIfError( iNotificationState.Attach( KPSUidAvkonDomain, KAknNotificationsInIdleAllowed ) );
    CActiveScheduler::Add( this );
    
    iNotificationState.Get( iNotificationStatus );
    
    if ( iNotificationStatus )
        {
        iNotificationState.Set( 0 );
        }
    
    iNotificationState.Subscribe( iStatus );
    SetActive();
    }    

    
// End of File

