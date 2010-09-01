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
* Description:  VCommandManager document class
*
*/


// INCLUDE FILES

#include <apgwgnam.h>

#include "vcmanagerdocument.h"
#include "vcmanagerappui.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CVCommandManagerDocument::CVCommandManagerDocument
// Default constructor
// ----------------------------------------------------------------------------
//
CVCommandManagerDocument::CVCommandManagerDocument( CEikApplication& aApp ): 
                                                    CAknDocument( aApp )    
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// CVCommandManagerDocument::~CVCommandManagerDocument()
// Destructor
// ----------------------------------------------------------------------------
//
CVCommandManagerDocument::~CVCommandManagerDocument()
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// CVCommandManagerDocument::UpdateTaskNameL
// Updates task name
// ----------------------------------------------------------------------------
//
void CVCommandManagerDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    CAknDocument::UpdateTaskNameL( aWgName );
    aWgName->SetHidden( ETrue ); 
    }

// ----------------------------------------------------------------------------
// CVCommandManagerDocument::ConstructL()
// Second phase constructor
// ----------------------------------------------------------------------------
//
void CVCommandManagerDocument::ConstructL()
    {
    // Nothing
    }

// ----------------------------------------------------------------------------
// CVCommandManagerDocument::NewL
// Symbian constructor
// ----------------------------------------------------------------------------
//
CVCommandManagerDocument* CVCommandManagerDocument::NewL( CEikApplication& aApp )
    {
    CVCommandManagerDocument* self = new (ELeave) CVCommandManagerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------
// CVCommandManagerDocument::CreateAppUiL
// Instantiates AppUi
// ----------------------------------------------------
//
CEikAppUi* CVCommandManagerDocument::CreateAppUiL()
    {
    return new (ELeave) CVCommandManagerAppUi;
    }

// End of File
