/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include    "rubydebug.h"
#include    "nssvcapplauncher.hrh"
#include    "nssvcapplauncher.h"
#include    <e32std.h>
#include    <bacline.h>

//=============================================================================

TInt ReadArgumentL( TInt& aArgument )
    {
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    // First argument is program name. Therefore there must be two arguments
    if ( args->Count() < 2 )
        {
        User::Leave( KErrArgument ); 
        }
    // We are interested in only the second argument (command to be executed).
    TPtrC argumentPrt( args->Arg( 1 ) );
    // Convert argument to TInt
    TLex lex( argumentPrt );
    User::LeaveIfError( lex.Val( aArgument ) );
    
    CleanupStack::PopAndDestroy( args );
    
    return KErrNone;
    }

//=============================================================================

void ExecuteL()
    {
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    
    // Command line argument
    TInt clArgument( -1 );
    ReadArgumentL( clArgument );
    
    if ( clArgument >= ECalendar && clArgument <= EMediaVolumeDown )
        {
        __UHEAP_MARK;
        CNssVCAppLauncher* appLauncher = CNssVCAppLauncher::NewL();
        CleanupStack::PushL( appLauncher );
        
        appLauncher->ExecuteCommandL( clArgument );
        
        CleanupStack::PopAndDestroy ( appLauncher ); 
        __UHEAP_MARKEND;
        }
        
    // Delete active scheduler
    CleanupStack::PopAndDestroy( scheduler );
    }
    
//=============================================================================

GLDEF_C TInt E32Main()
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    RUBY_TRAP_IGNORE( ExecuteL() );
    
    delete cleanup;
    
    return KErrNone;
    }

// End of File
