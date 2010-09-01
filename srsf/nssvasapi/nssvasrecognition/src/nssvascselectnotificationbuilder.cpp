/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssSelectNotificationBuilder class performs initialization 
*                for the CNssTagSelectNotification class. 
*
*/


// INCLUDE FILES

#include "nssvascselectnotificationbuilder.h"
#include "nssvasctagselectnotification.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNssSelectNotificationBuilder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssSelectNotificationBuilder* CNssSelectNotificationBuilder::NewL()
{
	// version of NewLC which leaves nothing on the cleanup stack
	CNssSelectNotificationBuilder* self=NewLC();
	CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CNssRecognitionHandlerBuilder::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssSelectNotificationBuilder* CNssSelectNotificationBuilder::NewLC()
{
    CNssSelectNotificationBuilder* self=new (ELeave) CNssSelectNotificationBuilder();
    CleanupStack::PushL(self);
    return self;
}

// -----------------------------------------------------------------------------
// CNssSelectNotificationBuilder::~CNssSelectNotificationBuilder
// Destructor
// -----------------------------------------------------------------------------
//
CNssSelectNotificationBuilder::~CNssSelectNotificationBuilder()
{
	delete iTagSelectNotification;
}

// -----------------------------------------------------------------------------
// CNssSelectNotificationBuilder::InitializeL
// Initialize Tag Select Notification
// -----------------------------------------------------------------------------
//
EXPORT_C void CNssSelectNotificationBuilder::InitializeL()
{
	iTagSelectNotification = CNssTagSelectNotification::NewL();
}

// -----------------------------------------------------------------------------
// CNssSelectNotificationBuilder::GetTagSelectNotification
// Get a Pointer to CNssTagSelectNotification
// -----------------------------------------------------------------------------
//
EXPORT_C MNssTagSelectNotification* 
			              CNssSelectNotificationBuilder::GetTagSelectNotification()
{
	return iTagSelectNotification;
}

// -----------------------------------------------------------------------------
// CNssSelectNotificationBuilder::CNssSelectNotificationBuilder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssSelectNotificationBuilder::CNssSelectNotificationBuilder()
{
}
