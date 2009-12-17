/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Watches changes of the PC-Suite data sync state
*
*/


#include "vcmanagerbackupobserver.h"
#include "rubydebug.h"

#include "srsfprivatecrkeys.h"

#include <sbdefs.h>
#include <centralrepository.h>

// CONSTANTS

static const TInt KUndefinedLanguage = -1;

/**
 @todo generalize the common parts of this class, CVCommandManagerBackupObserver
       and CNssVasBackupObserver from VAS API
       At the moment the code is copy-pasted and duplicated
*/

// ---------------------------------------------------------
// CVCommandManagerBackupObserver::NewL
// ---------------------------------------------------------
//
CVCommandManagerBackupObserver* CVCommandManagerBackupObserver::NewL()
    {
    CVCommandManagerBackupObserver* self = new (ELeave) CVCommandManagerBackupObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CVCommandManagerBackupObserver::CVCommandManagerBackupObserver
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
CVCommandManagerBackupObserver::CVCommandManagerBackupObserver()
 : CActive( CActive::EPriorityHigh ), iRestoreStarted( EFalse )
    {
    // Nothing
    }
    
// ---------------------------------------------------------
// CVCommandManagerBackupObserver::ConstructL
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
void CVCommandManagerBackupObserver::ConstructL() 
    {
    RUBY_DEBUG_BLOCK( "CVCommandManagerBackupObserver::ConstructL" );
    User::LeaveIfError( iProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey ) );
       
    // initial subscription and process current property value
    iProperty.Subscribe( iStatus );
    
    CActiveScheduler::Add( this ); 
    SetActive();   
    }
    
// ---------------------------------------------------------
// CVCommandManagerBackupObserver::~CVCommandManagerBackupObserver
// ---------------------------------------------------------
//
CVCommandManagerBackupObserver::~CVCommandManagerBackupObserver()
    {
    Cancel();
    iProperty.Close();
    }
    
// ---------------------------------------------------------
// CNssVasBackupObserver::DoCancel
// Cancel listening now
// ---------------------------------------------------------
//    
void CVCommandManagerBackupObserver::DoCancel() 
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------
// CVCommandManagerBackupObserver::RunL
// Is called, when property changed
// ---------------------------------------------------------
//
void CVCommandManagerBackupObserver::RunL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandManagerBackupObserver::RunL" );
    
    if( iStatus != KErrNone ) 
        {
        // At least report the error
        RUBY_ERROR1( "CVCommandManagerBackupObserver::RunL - iStatus is [%d]", iStatus.Int() );
        }
        
    // resubscribe before processing new value to prevent missing updates
    // even if some error happened
    iProperty.Subscribe( iStatus );
    SetActive();
    // property updated, get new value
	TInt backupFlag;
	User::LeaveIfError( iProperty.Get( backupFlag ) );
	switch ( backupFlag & conn::KBURPartTypeMask )
	    {
	    case conn::EBURNormal:
	        RUBY_DEBUG0( "CVCommandManagerBackupObserver::RunL - Backup/restore ended" );
	        SetVoiceCommandRetrainingL();
	        iRestoreStarted = EFalse;
	        break;
	        
	    case conn::EBURBackupFull:
	    case conn::EBURBackupPartial:
	        RUBY_DEBUG0( "CVCommandManagerBackupObserver::RunL - Backup started" );
            iRestoreStarted = EFalse;
	        break;
	    
	    case conn::EBURRestoreFull:
	    case conn::EBURRestorePartial:
	        RUBY_DEBUG0( "CVCommandManagerBackupObserver::RunL - Restore started" );
            iRestoreStarted = ETrue;
	        break;
	        
	    case conn::EBURUnset:
	    default:
	        RUBY_ERROR1( "CVCommandManagerBackupObserver::RunL - Backup flag unknown %h ", backupFlag );
	    }
    }

// ---------------------------------------------------------
// CVCommandManagerBackupObserver::SetVoiceCommandRetrainingL
// Enables voice command retraining
// ---------------------------------------------------------
//    
void CVCommandManagerBackupObserver::SetVoiceCommandRetrainingL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandManagerBackupObserver::SetVoiceCommandRetrainingL" );
    
    if ( iRestoreStarted )
        {
        CRepository* client = CRepository::NewLC( KCRUidSRSFSettings );
        User::LeaveIfError( client->Set( KSRSFUiLanguage, KUndefinedLanguage ) );
        CleanupStack::PopAndDestroy( client );
        }
    }

//  End of File
