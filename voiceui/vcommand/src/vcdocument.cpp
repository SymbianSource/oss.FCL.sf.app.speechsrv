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
* Description:  Document class for vcommand app
*
*/


// INCLUDE FILES
#include "vcdocument.h"
#include "vcappui.h"


// ================= MEMBER FUNCTIONS =======================

// constructor
CVCDocument::CVCDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CVCDocument::~CVCDocument()
    {
    }

// Default constructor can leave.
void CVCDocument::ConstructL()
    {
    }

// Two-phased constructor.
CVCDocument* CVCDocument::NewL(
        CEikApplication& aApp)     // CVCommandApp reference
    {
    CVCDocument* self = new (ELeave) CVCDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ----------------------------------------------------
// CVCDocument::CreateAppUiL()
// constructs CVCAppUi
// ----------------------------------------------------
//
CEikAppUi* CVCDocument::CreateAppUiL()
    {
    return new (ELeave) CVCAppUi;
    }



// End of File
