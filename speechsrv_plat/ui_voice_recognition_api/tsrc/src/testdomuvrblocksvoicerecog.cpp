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
// CTestDOMUiVoiceRengn::TestVuicVocRecogNewL
// -----------------------------------------------------------------------------
//
TInt CTestDOMUiVoiceRengn::TestVuicVocRecogNewL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( Ktestdomuivoicerecgnin, "testdomuivoicerecgnin" );
    _LIT( KTestVuicVocRecogNewL, "In TestVuicVocRecogNewL" );
    TestModuleIf().Printf( 0, Ktestdomuivoicerecgnin, KTestVuicVocRecogNewL );
    // Print to log file
    iLog->Log( KTestVuicVocRecogNewL );

    TInt Err = KErrNone;
    
    iVoiceRecog = CVoiceRecog::NewL();
    STIF_ASSERT_NOT_NULL( iVoiceRecog );
    
    return Err;
    }

// -----------------------------------------------------------------------------
// CTestDOMUiVoiceRengn::TestVuicVocRecogNewLCL
// -----------------------------------------------------------------------------
//
TInt CTestDOMUiVoiceRengn::TestVuicVocRecogNewLCL( CStifItemParser& /*aItem*/ )
    {

    // Print to UI
    _LIT( Ktestdomuivoicerecgnin, "testdomuivoicerecgnin" );
    _LIT( KTestVuicVocRecogNewLC, "In TestVuicVocRecogNewLC" );
    TestModuleIf().Printf( 0, Ktestdomuivoicerecgnin, KTestVuicVocRecogNewLC );
    // Print to log file
    iLog->Log( KTestVuicVocRecogNewLC );

    TInt Err = KErrNone;
    
    iVoiceRecog = CVoiceRecog::NewLC();
    CleanupStack::Pop( iVoiceRecog );
    STIF_ASSERT_NOT_NULL( iVoiceRecog );
    
    return Err;
    }
