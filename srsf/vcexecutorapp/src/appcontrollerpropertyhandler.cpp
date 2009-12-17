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
* Description:  Active object for subscribing to Publish & Suscribe properties
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "appcontrollerpropertyhandler.h"
#include "appcontroller.h"

// ---------------------------------------------------------
// CAppControllerPropertyHandler::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
CAppControllerPropertyHandler* CAppControllerPropertyHandler::NewL(
    TUid aCategory, TUint aKey, CVCApplicationControllerImpl* aController)
    {
    CAppControllerPropertyHandler* self = new ( ELeave ) CAppControllerPropertyHandler( aController );
    CleanupStack::PushL( self );
    self->ConstructL( aCategory, aKey );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CAppControllerPropertyHandler::CAppControllerPropertyHandler
// C++ constructor
// ---------------------------------------------------------
//
CAppControllerPropertyHandler::CAppControllerPropertyHandler( CVCApplicationControllerImpl* aController )
													: CActive( EPriorityStandard ), 
													iController( aController )
    {
    }

// ---------------------------------------------------------
// CAppControllerPropertyHandler::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CAppControllerPropertyHandler::ConstructL(TUid aCategory, TUint aKey )
    {
    User::LeaveIfError( iProperty.Attach( aCategory, aKey ) );

    CActiveScheduler::Add(this);
    // Initial subscription and process current property value
    RunL();

    iRequestIssued = ETrue;
    }

// Destructor
CAppControllerPropertyHandler::~CAppControllerPropertyHandler()
    {
    Cancel();
    iProperty.Close();
    }


// ---------------------------------------------------------
// CAppControllerPropertyHandler::DoCancel   
// Cancel the request to receive events
// ---------------------------------------------------------
//
void CAppControllerPropertyHandler::DoCancel()
    {
    iProperty.Cancel();
    }


// ---------------------------------------------------------
// CAppControllerPropertyHandler::RunL
// Handle a change event.
// ---------------------------------------------------------
//
void CAppControllerPropertyHandler::RunL()
    {
    // Resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();

    if ( iRequestIssued )
        {
        iController->HandleCurrentCallStatusChange();
        }
    }


// ---------------------------------------------------------
// CAppControllerPropertyHandler::Value
// Get current property value
// ---------------------------------------------------------
//
TInt CAppControllerPropertyHandler::Value()
    {
    TInt value = 0;
    iProperty.Get( value );
    return value;
    }


// End of File
