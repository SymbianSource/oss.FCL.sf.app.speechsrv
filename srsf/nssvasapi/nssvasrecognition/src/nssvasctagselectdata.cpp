/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssTagSelectData Class contains the data for tag select 
*                notification.
*
*/


// INCLUDE FILES
#include "nssvasctagselectdata.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CNssTagSelectData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssTagSelectData* CNssTagSelectData::NewL(CNssContext* aContext, 
								     MNssTagSelectHandler* aTagSelectHandler)
{
	// version of NewLC which leaves nothing on the cleanup stack
	CNssTagSelectData* self=NewLC(aContext, 
							   aTagSelectHandler);
	CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CNssTagSelectData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssTagSelectData* CNssTagSelectData::NewLC(CNssContext* aContext, 
								      MNssTagSelectHandler* aTagSelectHandler)
{
    CNssTagSelectData* self=new (ELeave) CNssTagSelectData();
    // get new, leave if can't
    CleanupStack::PushL(self);
    // push onto cleanup stack
    // (in case self->ConstructL leaves)
    self->ConstructL(aContext, 
				     aTagSelectHandler); // use two-stage construct
    return self;
}

// -----------------------------------------------------------------------------
// CNssTagSelectData::~CNssTagSelectData
// Destructor
// -----------------------------------------------------------------------------
//
CNssTagSelectData::~CNssTagSelectData()
{
	delete iContext;
}

// -----------------------------------------------------------------------------
// CNssTagSelectData::Context
// Member function to get a pointer to the context.
// -----------------------------------------------------------------------------
//
CNssContext* CNssTagSelectData::Context()
{
	return iContext;
}

// -----------------------------------------------------------------------------
// CNssTagSelectData::TagSelectHandler
// Member function to get a pointer the TagSelectHandler event handler.
// -----------------------------------------------------------------------------
//
MNssTagSelectHandler* CNssTagSelectData::TagSelectHandler()
{
	return iTagSelectHandler;
}

// -----------------------------------------------------------------------------
// CNssTagSelectData::CNssTagSelectData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssTagSelectData::CNssTagSelectData()
{
}

// -----------------------------------------------------------------------------
// CNssTagSelectData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNssTagSelectData::ConstructL(CNssContext* aContext, 
								MNssTagSelectHandler* aTagSelectHandler)
{	
	iContext = aContext->CopyL();     // create a new context and make a copy

    iTagSelectHandler = aTagSelectHandler;
}
