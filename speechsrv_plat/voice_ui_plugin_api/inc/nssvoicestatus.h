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
* Description:  Status values for VUIPF
*
*/


#ifndef NSSVOICESTATUS_H
#define NSSVOICESTATUS_H

// Status values which extend standard error values. They are used 
// in execution and event plug-ins. 

// Status used when an event was not used e.g. captured key can be forwarded
const TInt KErrNoneEventNotConsumed = 1;

// Status used when an event has been consumed and the a plug-in allows another
// plug-in to handle the event also. Used in VUIP Handler and execution plug-ins.
const TInt KErrNoneForwardEvent = 2;


#endif // NSSVOICESTATUS_H