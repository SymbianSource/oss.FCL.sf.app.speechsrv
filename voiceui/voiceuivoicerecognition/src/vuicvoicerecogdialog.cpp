/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The Voice Recognition dialog for clients
*
*/


// INCLUDE FILES
#include <coemain.h>
#include <apgcli.h>
#include <apgtask.h>
#include <apacmdln.h>

#include "vuicvoicerecogdialog.h"

#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVoiceRecognitionDialog::CVoiceRecognitionDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CVoiceRecognitionDialog::CVoiceRecognitionDialog()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialog::CVoiceRecognitionDialog" );
    // Nothing
    }

// Destructor
EXPORT_C CVoiceRecognitionDialog::~CVoiceRecognitionDialog()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialog::~CVoiceRecognitionDialog START" );
        
    RUBY_DEBUG0( "CVoiceRecognitionDialog::~CVoiceRecognitionDialog EXIT" );
    }


// ---------------------------------------------------------
// CVoiceRecognitionDialog::ExecuteLD
// ---------------------------------------------------------
//
EXPORT_C TInt CVoiceRecognitionDialog::ExecuteLD()
    {
    RUBY_DEBUG_BLOCK( "CVoiceRecognitionDialog::ExecuteLD" );
    
    TUid uid = { 0x101F8543 };

    TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
    TApaTask apaTask = apaTaskList.FindApp( uid );
    
    if ( apaTask.Exists() )
        {
        apaTask.BringToForeground();
        }
    else
        {
        RApaLsSession apaLsSession;
        User::LeaveIfError( apaLsSession.Connect() );
        
        TApaAppInfo appInfo;
        User::LeaveIfError( apaLsSession.GetAppInfo( appInfo, uid ) );
        
        TFileName appName = appInfo.iFullName;
        CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();

        apaCommandLine->SetExecutableNameL( appName );
        apaCommandLine->SetCommandL( EApaCommandRun );
        User::LeaveIfError ( apaLsSession.StartApp( *apaCommandLine ) );
        CleanupStack::PopAndDestroy( apaCommandLine );
    
        apaLsSession.Close();
        }
        
    delete this;

    return KErrNone;
    }

// ---------------------------------------------------------
// CVoiceRecognitionDialog::Cancel
// ---------------------------------------------------------
//
EXPORT_C void CVoiceRecognitionDialog::Cancel()
    {
    RUBY_DEBUG0( "CVoiceRecognitionDialog::Cancel START" );
    
    delete this;
    
    RUBY_DEBUG0( "CVoiceRecognitionDialog::Cancel EXIT" );
    }

