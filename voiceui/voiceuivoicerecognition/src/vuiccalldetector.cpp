/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <ctsydomainpskeys.h>

#include "vuiccalldetector.h"
#include "vuimcalldetectorobserver.h"

#include "rubydebug.h"

// ---------------------------------------------------------------------------
// CCallDetector::NewL
//
// ---------------------------------------------------------------------------
//
CCallDetector* CCallDetector::NewL( MCallDetectorObserver* aObserver )
    {
    RUBY_DEBUG_BLOCK( "CCallDetector::NewL" );
    
    CCallDetector* self = new( ELeave ) CCallDetector( aObserver );        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------------------------
//
CCallDetector::~CCallDetector()
    {
    RUBY_DEBUG0( "CCallDetector::~CCallDetector" );
    
    Cancel();
    iTsyProperty.Close();
    }
    
// ---------------------------------------------------------------------------
// CCallDetector::RequestNotification
// 
// ---------------------------------------------------------------------------
// 
void CCallDetector::RequestNotification()
    {
    RUBY_DEBUG0( "CCallDetector::RequestNotification" );
    
    iTsyProperty.Subscribe( iStatus );
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// Constructor
//
// ---------------------------------------------------------------------------
//
CCallDetector::CCallDetector( MCallDetectorObserver* aObserver )
 : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    }
   
// ---------------------------------------------------------------------------
// CCallDetector::ConstructL
//
// ---------------------------------------------------------------------------
//
void CCallDetector::ConstructL()
    {
    User::LeaveIfError( iTsyProperty.Attach( KPSUidCtsyCallInformation, KCTsyCallState ) );
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// CCallDetector::RunL
// Called when the voice line status changes.
// ---------------------------------------------------------------------------
//
void CCallDetector::RunL()
    {
    TInt status = iStatus.Int();
    
    RequestNotification();
    
    if ( status == KErrNone )
        {
        TInt status;
        iTsyProperty.Get( status );
        
        if ( status != EPSCTsyCallStateNone )
            {
            iObserver->CallDetectedL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCallDetector::DoCancel
//
// ---------------------------------------------------------------------------
//
void CCallDetector::DoCancel()
    {
    RUBY_DEBUG0( "CCallDetector::DoCancel" );

    iTsyProperty.Cancel();
    }

// End of File

