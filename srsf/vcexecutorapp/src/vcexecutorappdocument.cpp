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
* Description:  Document class
*
*/


// INCLUDE FILES

#include <apgwgnam.h>

#include "vcexecutorappdocument.h"
#include "vcexecutorappui.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CVCExecutorAppDocument::CVCExecutorAppDocument
// Default constructor
// ----------------------------------------------------------------------------
//
CVCExecutorAppDocument::CVCExecutorAppDocument( CEikApplication& aApp ): 
                                                CAknDocument( aApp )    
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// CVCExecutorAppDocument::~CVCExecutorAppDocument()
// Destructor
// ----------------------------------------------------------------------------
//
CVCExecutorAppDocument::~CVCExecutorAppDocument()
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// CVCExecutorAppDocument::UpdateTaskNameL
// Updates task name
// ----------------------------------------------------------------------------
//
void CVCExecutorAppDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    CAknDocument::UpdateTaskNameL( aWgName );
    aWgName->SetHidden( ETrue ); 
    }

// ----------------------------------------------------------------------------
// CVCExecutorAppDocument::ConstructL()
// Second phase constructor
// ----------------------------------------------------------------------------
//
void CVCExecutorAppDocument::ConstructL()
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// CVCExecutorAppDocument::NewL
// Symbian constructor
// ----------------------------------------------------------------------------
//
CVCExecutorAppDocument* CVCExecutorAppDocument::NewL( CEikApplication& aApp )
    {
    CVCExecutorAppDocument* self = new (ELeave) CVCExecutorAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------
// CVCExecutorAppDocument::CreateAppUiL
// Instantiates AppUi
// ----------------------------------------------------
//
CEikAppUi* CVCExecutorAppDocument::CreateAppUiL()
    {
    return new (ELeave) CVCExecutorAppUi;
    }

// End of File
