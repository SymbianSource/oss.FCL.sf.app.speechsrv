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
* Description:  Application class for vcommand app
*
*/



// INCLUDE FILES
#include "vcapp.h"
#include "vcdocument.h"
#include <eikstart.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVCApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CVCApp::AppDllUid() const
    {
    return KUidVCommand;
    }

   
// ---------------------------------------------------------
// CVCApp::CreateDocumentL()
// Creates CVCDocument object
// ---------------------------------------------------------
//
CApaDocument* CVCApp::CreateDocumentL()
    {
    return CVCDocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


LOCAL_C CApaApplication* NewApplication()
    {
    return new CVCApp;
    }


GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// End of File
