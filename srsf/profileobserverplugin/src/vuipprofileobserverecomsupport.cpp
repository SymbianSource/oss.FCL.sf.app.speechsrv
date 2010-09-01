/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  profile observer vuipf-plugin
*
*/


#include "vuipprofileobserverplugin.h"

#include <implementationproxy.h>
#include <nssvuipf.hrh>


const TImplementationProxy ImplementationTable[] = 
    {
    { {0x10281CED}, (TProxyNewLPtr) CVUIPprofileobserverplugin::NewL }
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    } 
 

// End of File

