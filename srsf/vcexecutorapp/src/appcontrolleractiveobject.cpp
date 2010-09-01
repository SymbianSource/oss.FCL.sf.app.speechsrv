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
* Description:  Active object to handle global list query results.
*
*/



// INCLUDE FILES
#include "appcontrolleractiveobject.h"
#include "appcontroller.h"
#include <coemain.h>


// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAppControllerActiveObject::CAppControllerActiveObject
// 
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAppControllerActiveObject::CAppControllerActiveObject(CVCApplicationControllerImpl& aController)
:   CActive( EPriorityStandard ), iController(aController)
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CAppControllerActiveObject::~CAppControllerActiveObject
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CAppControllerActiveObject::~CAppControllerActiveObject()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CAppControllerActiveObject::DoCancel
// 
// 
// -----------------------------------------------------------------------------
//
void CAppControllerActiveObject::DoCancel()
    {
    }


// -----------------------------------------------------------------------------
// CAppControllerActiveObject::RunL
// 
// 
// -----------------------------------------------------------------------------
//
void CAppControllerActiveObject::RunL()
    {
	iController.HandleActiveObjectStatus( iStatus.Int() );
    Cancel();
    }

// -----------------------------------------------------------------------------
// CAppControllerActiveObject::RunError
// 
// 
// -----------------------------------------------------------------------------
//
TInt CAppControllerActiveObject::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CAppControllerActiveObject::SetActive
// 
// 
// -----------------------------------------------------------------------------
//
void CAppControllerActiveObject::SetActive()
    {
    if( !IsActive() )
        {
        CActive::SetActive();
        }
    }

// End of File
