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
* Description:
*
*/


// INCLUDE FILES
#include "TestAppDocument.h"
#include "TestAppAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CTestAppDocument::CTestAppDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CTestAppDocument::~CTestAppDocument()
    {
    }

// EPOC default constructor can leave.
void CTestAppDocument::ConstructL()
    {
    }

// Two-phased constructor.
CTestAppDocument* CTestAppDocument::NewL(
        CEikApplication& aApp)     // CTestAppApp reference
    {
    CTestAppDocument* self = new (ELeave) CTestAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ----------------------------------------------------
// CTestAppDocument::CreateAppUiL()
// constructs CTestAppAppUi
// ----------------------------------------------------
//
CEikAppUi* CTestAppDocument::CreateAppUiL()
    {
    return new (ELeave) CTestAppAppUi;
    }

// End of File  
