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
#include <hwrmdomainpskeys.h>

#include "vuimvoicerecogdialogcallback.h"

#include "vuicfoldobserver.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CFoldObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFoldObserver* CFoldObserver::NewL( MVoiceRecognitionDialogCallback* aCallback )
    {
    CFoldObserver* self = new (ELeave) CFoldObserver( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    } 

// Destructor       
CFoldObserver::~CFoldObserver()
    {
    RUBY_DEBUG0( "CFoldObserver::~CFoldObserver START" );
    
    Cancel();
    iFoldEvent.Close();
    
    RUBY_DEBUG0( "CFoldObserver::~CFoldObserver EXIT" );
    }

// ---------------------------------------------------------
// CFoldObserver::DoCancel
// ---------------------------------------------------------
// 
void CFoldObserver::DoCancel()
    {
    RUBY_DEBUG0( "CFoldObserver::DoCancel START" );
    
    iFoldEvent.Cancel();
    
    RUBY_DEBUG0( "CFoldObserver::DoCancel EXIT" );
    }

// ---------------------------------------------------------
// CFoldObserver::RunL
// ---------------------------------------------------------
//     
void CFoldObserver::RunL()
    {
    RUBY_DEBUG_BLOCK( "CFoldObserver::RunL" );
    
    TInt status = iStatus.Int();
    
    iFoldEvent.Subscribe( iStatus );
    SetActive();
    
    if ( status == KErrNone )
        {
        TInt newFoldStatus;
        iFoldEvent.Get( newFoldStatus );
        
        if ( newFoldStatus == EPSHWRMFlipClosed && newFoldStatus != iFoldStatus )
            {
            iCallback->DialogDismissed();
            }
        iFoldStatus = newFoldStatus;
        }
    }

// ---------------------------------------------------------
// CFoldObserver::CFoldObserver
// ---------------------------------------------------------
//              
CFoldObserver::CFoldObserver( MVoiceRecognitionDialogCallback* aCallback )
 : CActive( EPriorityStandard ), iCallback( aCallback )
    {
    }
    
// ---------------------------------------------------------
// CFoldObserver::ConstructL
// ---------------------------------------------------------
//           
void CFoldObserver::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CFoldObserver::ConstructL" );
    
    User::LeaveIfError( iFoldEvent.Attach( KPSUidHWRM, KHWRMFlipStatus ) );
    CActiveScheduler::Add( this );
    
    iFoldEvent.Get( iFoldStatus );
    
    iFoldEvent.Subscribe( iStatus );
    SetActive();
    }    

    
// End of File

