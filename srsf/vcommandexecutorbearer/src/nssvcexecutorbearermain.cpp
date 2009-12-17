/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Bearer plugin main file
*       
*
*/


// INCLUDE FILES
#include <ecom/implementationproxy.h>
#include "nssvcexecutorbearerimpl.h"
#include "nssvcexecutorbearer.hrh"

// CONSTANTS
// Define the private interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( NssVCExecutorBearerPluginImplementationUid, NssVCExecutorBearer::NewL ),
    };

// ---------------------------------------------------------
// ImplementationGroupProxy ()
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

//
// End of file
