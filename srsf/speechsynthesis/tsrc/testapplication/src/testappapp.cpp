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
#include <eikstart.h>

#include    "TestAppApp.h"
#include    "TestAppDocument.h"


LOCAL_C CApaApplication* NewApplication()
    {
    return new CTestAppApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
    
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTestAppApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CTestAppApp::AppDllUid() const
    {
    return KUidTestApp;
    }
   
// ---------------------------------------------------------
// CTestAppApp::CreateDocumentL()
// Creates CTestAppDocument object
// ---------------------------------------------------------
//
CApaDocument* CTestAppApp::CreateDocumentL()
    {
    return CTestAppDocument::NewL( *this );
    }

// End of File  
