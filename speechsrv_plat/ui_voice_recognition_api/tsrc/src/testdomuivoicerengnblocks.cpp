/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  For test ui voice recognition api modules
*
*/



// [INCLUDE FILES]
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <vuicvoicerecog.h>
#include <vuicvoicerecogdialog.h>

#include "testdomuivoicerengn.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestDOMUiVoiceRengn::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestDOMUiVoiceRengn::RunMethodL( CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestVuicVocRecogNewL", 
               CTestDOMUiVoiceRengn::TestVuicVocRecogNewL ),
        ENTRY( "TestVuicVocRecogNewLCL", 
                CTestDOMUiVoiceRengn::TestVuicVocRecogNewLCL ),
        ENTRY( "TestVoceRegDalgCVoceRegDialogL", 
                CTestDOMUiVoiceRengn::TestVoceRegDalgCVoceRegDialogL ),
        ENTRY( "TestVoceRegDalgDestructor", 
                CTestDOMUiVoiceRengn::TestVoceRegDalgDestructor ),
        ENTRY( "TestVoceRegDalgExecuteLDL", 
                CTestDOMUiVoiceRengn::TestVoceRegDalgExecuteLDL ),
        ENTRY( "TestVoceRegDalgCancel", 
                CTestDOMUiVoiceRengn::TestVoceRegDalgCancel ),
        // [test cases entries]

        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//  [End of File]
