/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Starts VCommand modules at start up.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <nssvasccontacthandler.h>
#include <eikstart.h>
#include "nssvoiceuipluginhandler.h"
#include "vcmanagerapp.h"
#include "vcmanagerdocument.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVCommandManagerApp::AppDllUid
// Returns application UID
// ---------------------------------------------------------
//
TUid CVCommandManagerApp::AppDllUid() const
    {
    return KUidVcManager;
    }

   
// ---------------------------------------------------------
// CVCommandManagerApp::CreateDocumentL
// Creates document object
// ---------------------------------------------------------
//
CApaDocument* CVCommandManagerApp::CreateDocumentL()
    {
    return CVCommandManagerDocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// NewApplication
// Creates CVCommandManagerApp object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CVCommandManagerApp;
    }
    
// ---------------------------------------------------------
// E32Main
// Main
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File
