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
* Description:  Text query dialog for editing commands
*
*/


// INCLUDE FILES
#include "vctextquerydialog.h"

// -----------------------------------------------------------------------------
// CVCTextQueryDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCTextQueryDialog* CVCTextQueryDialog::NewL( TDes& aDataText, const TTone& aTone )
    {
    CVCTextQueryDialog* self = new (ELeave) CVCTextQueryDialog( aDataText, aTone );
    return self;
    }   
    
// Destructor       
CVCTextQueryDialog::~CVCTextQueryDialog()
    {
    }

// ---------------------------------------------------------
// CVCTextQueryDialog::CheckIfEntryTextOk
// ---------------------------------------------------------
//      
TBool CVCTextQueryDialog::CheckIfEntryTextOk() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CVCTextQueryDialog::UpdateLeftSoftKeyL
// ---------------------------------------------------------
//   
void CVCTextQueryDialog::UpdateLeftSoftKeyL()
    {
    CAknTextQueryDialog::UpdateLeftSoftKeyL();

    MakeLeftSoftkeyVisible(CheckIfEntryTextOk());
    }

// ---------------------------------------------------------
// CVCTextQueryDialog::CVCTextQueryDialog
// ---------------------------------------------------------
//              
CVCTextQueryDialog::CVCTextQueryDialog( TDes& aDataText, const TTone& aTone )
 : CAknTextQueryDialog( aDataText, aTone )
    {
    }
    
// End of File

