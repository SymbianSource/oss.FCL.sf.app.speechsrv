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
* Description:  The Voice Recognition application
*
*/


// INCLUDE FILES
#include <eikstart.h>

#include "vuicapp.h"
#include "vuicdocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CApp::AppDllUid() const
    {
    return KUidVoiceUi;
    }

   
// ---------------------------------------------------------
// CApp::CreateDocumentL()
// Creates CVCDocument object
// ---------------------------------------------------------
//
CApaDocument* CApp::CreateDocumentL()
    {
    return CDocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


LOCAL_C CApaApplication* NewApplication()
    {
    return new CApp;
    }


GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }


// End of File
