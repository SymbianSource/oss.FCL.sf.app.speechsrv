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
* Description:  
*
*/



// INCLUDE FILES
#include <stiftestinterface.h>

#include "speechsynthesistest.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::CSpeechSynthesisTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSpeechSynthesisTest::CSpeechSynthesisTest( CTestModuleIf& aTestModuleIf ):
    CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTest::ConstructL()
    {
    iLog = CStifLogger::NewL( KSpeechSynthesisTestLogPath, 
                          KSpeechSynthesisTestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    iTester = CSpeechSynthesisTester::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSpeechSynthesisTest* CSpeechSynthesisTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CSpeechSynthesisTest* self = new (ELeave) CSpeechSynthesisTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CSpeechSynthesisTest::~CSpeechSynthesisTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CSpeechSynthesisTest::NewL( aTestModuleIf );
    }

//  End of File
