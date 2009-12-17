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
* Description:  Implementation of CVCAccessoryButtonMonitor
*
*/



// INCLUDE FILES
#include "accessorybuttonmonitor.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCAccessoryButtonMonitor::CVCAccessoryButtonMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVCAccessoryButtonMonitor::CVCAccessoryButtonMonitor()
    {
    }

// -----------------------------------------------------------------------------
// CVCAccessoryButtonMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVCAccessoryButtonMonitor::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CVCAccessoryButtonMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCAccessoryButtonMonitor* CVCAccessoryButtonMonitor::NewL()
    {
    CVCAccessoryButtonMonitor* self = new( ELeave ) CVCAccessoryButtonMonitor;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CVCAccessoryButtonMonitor::~CVCAccessoryButtonMonitor()
    {
    }


//  End of File  
