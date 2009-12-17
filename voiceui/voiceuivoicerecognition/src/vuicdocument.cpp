/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The Voice Recognition application document
*
*/


// INCLUDE FILES
#include "vuicdocument.h"
#include "vuicappui.h"


// ================= MEMBER FUNCTIONS =======================

// Constructor
CDocument::CDocument( CEikApplication& aApp )
 : CAknDocument( aApp )    
    {
    }

// Destructor
CDocument::~CDocument()
    {
    }

// Default constructor can leave.
void CDocument::ConstructL()
    {
    }

// Two-phased constructor.
CDocument* CDocument::NewL( CEikApplication& aApp )
    {
    CDocument* self = new (ELeave) CDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------
// CDocument::CreateAppUiL()
// Constructs CAppUi
// ----------------------------------------------------
//
CEikAppUi* CDocument::CreateAppUiL()
    {
    return new (ELeave) CAppUi;
    }



// End of File
