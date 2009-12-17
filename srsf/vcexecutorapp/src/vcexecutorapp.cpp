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
* Description:  Application class
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <eikstart.h>
#include "vcexecutorapp.h"
#include "vcexecutorappdocument.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVCExecutorApp::AppDllUid
// Returns application UID
// ---------------------------------------------------------
//
TUid CVCExecutorApp::AppDllUid() const
    {
    return KUidVcExecutorApp;
    }

   
// ---------------------------------------------------------
// CVCExecutorApp::CreateDocumentL
// Creates document object
// ---------------------------------------------------------
//
CApaDocument* CVCExecutorApp::CreateDocumentL()
    {
    return CVCExecutorAppDocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// NewApplication
// Creates CVCExecutorApp object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CVCExecutorApp;
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
