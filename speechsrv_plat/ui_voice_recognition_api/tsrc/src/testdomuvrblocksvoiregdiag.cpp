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

// -----------------------------------------------------------------------------
// CTestDOMUiVoiceRengn::TestVoceRegDalgCVoceRegDialogL
// -----------------------------------------------------------------------------
//
TInt CTestDOMUiVoiceRengn::TestVoceRegDalgCVoceRegDialogL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( Ktestdomuivoicerecgnin, "testdomuivoicerecgnin" );
    _LIT( KTestVoceRegDalgCVoceRegDialog, "In TestVoceRegDalgCVoceRegDialog" );
    TestModuleIf().Printf( 0, Ktestdomuivoicerecgnin, KTestVoceRegDalgCVoceRegDialog );
    // Print to log file
    iLog->Log( KTestVoceRegDalgCVoceRegDialog );

    TInt Err = KErrNone;
    
    iVoiceRecogDialog = new (ELeave) CVoiceRecognitionDialog();
    STIF_ASSERT_NOT_NULL( iVoiceRecogDialog );
    
    return Err;
    }
// -----------------------------------------------------------------------------
// CTestDOMUiVoiceRengn::TestVoceRegDalgDestructor
// -----------------------------------------------------------------------------
//
TInt CTestDOMUiVoiceRengn::TestVoceRegDalgDestructor( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( Ktestdomuivoicerecgnin, "testdomuivoicerecgnin" );
    _LIT( KTestVoceRegDalgDestructor, "In TestVoceRegDalgDestructor" );
    TestModuleIf().Printf( 0, Ktestdomuivoicerecgnin, KTestVoceRegDalgDestructor );
    // Print to log file
    iLog->Log( KTestVoceRegDalgDestructor );

    TInt Err = KErrNone;
      
    delete iVoiceRecogDialog;
    iVoiceRecogDialog = NULL;
    
    return Err;
    }

// -----------------------------------------------------------------------------
// CTestDOMUiVoiceRengn::TestVoceRegDalgExecuteLDL
// -----------------------------------------------------------------------------
//
TInt CTestDOMUiVoiceRengn::TestVoceRegDalgExecuteLDL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( Ktestdomuivoicerecgnin, "testdomuivoicerecgnin" );
    _LIT( KTestVoceRegDalgExecuteLD, "In TestVoceRegDalgExecuteLD" );
    TestModuleIf().Printf( 0, Ktestdomuivoicerecgnin, KTestVoceRegDalgExecuteLD );
    // Print to log file
    iLog->Log( KTestVoceRegDalgExecuteLD );

    TInt Err = KErrNone;
      
    iVoiceRecogDialog->ExecuteLD();
    iVoiceRecogDialog = NULL;
    
    return Err;
    }

// -----------------------------------------------------------------------------
// CTestDOMUiVoiceRengn::TestVoceRegDalgCancel
// -----------------------------------------------------------------------------
//
TInt CTestDOMUiVoiceRengn::TestVoceRegDalgCancel( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( Ktestdomuivoicerecgnin, "testdomuivoicerecgnin" );
    _LIT( KTestVoceRegDalgCancel, "In TestVoceRegDalgCancel" );
    TestModuleIf().Printf( 0, Ktestdomuivoicerecgnin, KTestVoceRegDalgCancel );
    // Print to log file
    iLog->Log( KTestVoceRegDalgCancel );

    TInt Err = KErrNone;
      
    iVoiceRecogDialog->Cancel();
    iVoiceRecogDialog = NULL;
    
    return Err;
    }
