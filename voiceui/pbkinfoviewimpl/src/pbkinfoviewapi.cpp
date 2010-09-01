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
* Description:  Contains functions that are exported from PbkInfoView for 
*               PbkInfoView AIW provider.
*
*/


#include <eikenv.h>
#include "pbkinfoviewapi.h"
#include <pbkinfoview.rsg>
#include "pbkinfoviewdialog.h"
#include "rubydebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbkInfoViewApi::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPbkInfoViewApi* CPbkInfoViewApi::NewL()
    {
    CPbkInfoViewApi* self = new (ELeave) CPbkInfoViewApi();
    CleanupStack::PushL(self);
	self->ConstructL();
    CleanupStack::Pop(self);

	return self;
    }


// -----------------------------------------------------------------------------
// Destructor.
//
// -----------------------------------------------------------------------------
//
CPbkInfoViewApi::~CPbkInfoViewApi()
    {
    }


// -----------------------------------------------------------------------------
// CPbkInfoViewApi::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewApi::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CPbkInfoViewApi::CPbkInfoViewApi
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbkInfoViewApi::CPbkInfoViewApi()
    {
    }


// -----------------------------------------------------------------------------
// CPbkInfoViewApi::ShowInfoViewL
// Shows info view displaying voice commands related to the contact.
// @param aContactId Contact id from contacts db.
// @return None.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPbkInfoViewApi::ShowInfoViewL( TInt aContactId )
    {
    RUBY_DEBUG_BLOCKL( "CPbkInfoViewApi::ShowInfoViewL" );
    
    CPbkInfoViewDialog* infoViewDialog = CPbkInfoViewDialog::NewL( aContactId );
    CleanupStack::PushL( infoViewDialog );

    // Framework handles the destruction of infoViewDialog;
    infoViewDialog->ExecuteLD( R_INFOVIEW_DIALOG );

    CleanupStack::Pop( infoViewDialog );
	}


// End of file
