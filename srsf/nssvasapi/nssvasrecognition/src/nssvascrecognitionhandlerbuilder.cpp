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
* Description:  The CNssRecognitionHandlerBuilder class performs initialization 
*                for CNssRecognitionHandler the  class. 
*
*/


// INCLUDE FILES
#include "nssvascrecognitionhandlerbuilder.h"
#include "nssvascrecognitionhandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNssRecognitionHandlerBuilder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssRecognitionHandlerBuilder* CNssRecognitionHandlerBuilder::NewL()
{
	// version of NewLC which leaves nothing on the cleanup stack
	CNssRecognitionHandlerBuilder* self=NewLC();
	CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CNssRecognitionHandlerBuilder::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssRecognitionHandlerBuilder* CNssRecognitionHandlerBuilder::NewLC()
{
	CNssRecognitionHandlerBuilder* self = new (ELeave) CNssRecognitionHandlerBuilder();
    CleanupStack::PushL(self);
    return self;
}

// -----------------------------------------------------------------------------
// CNssRecognitionHandlerBuilder::~CNssRecognitionHandlerBuilder
// Destructor
// -----------------------------------------------------------------------------
//
CNssRecognitionHandlerBuilder::~CNssRecognitionHandlerBuilder()
{
	delete iRecognitionHandler;
}

// -----------------------------------------------------------------------------
// CNssRecognitionHandlerBuilder::InitializeL
// Initialize Recognition Handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CNssRecognitionHandlerBuilder::InitializeL()
{
	iRecognitionHandler = CNssRecognitionHandler::NewL();
}
			
// -----------------------------------------------------------------------------
// CNssRecognitionHandlerBuilder::GetRecognitionHandler
// Get a Pointer to Recognition Handler
// -----------------------------------------------------------------------------
//
EXPORT_C MNssRecognitionHandler* CNssRecognitionHandlerBuilder::GetRecognitionHandler()
{
	return iRecognitionHandler;
}

// -----------------------------------------------------------------------------
// CNssRecognitionHandlerBuilder::CNssRecognitionHandlerBuilder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssRecognitionHandlerBuilder::CNssRecognitionHandlerBuilder()
{
}

