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


#include "nsschbackupobserver.h"
#include "rubydebug.h"

#include <sbdefs.h>

/**
 @todo generalize the common parts of this class and CNssVasBackupObserver from VAS API
       At the moment the code is copy-pasted and duplicated
*/

// ---------------------------------------------------------
// CNssChBackupObserver::NewL
// NewL
// ---------------------------------------------------------
//
CNssChBackupObserver* CNssChBackupObserver::NewL( CNssContactHandlerImplementation& aHost )
    {
    CNssChBackupObserver* self = new (ELeave) CNssChBackupObserver( aHost );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CNssChBackupObserver::CNssChBackupObserver
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
CNssChBackupObserver::CNssChBackupObserver( CNssContactHandlerImplementation& aHost ) :
    CActive( CActive::EPriorityHigh ),
    iHost( aHost )
    {
    // Nothing
    }
    
// ---------------------------------------------------------
// CNssChBackupObserver::ConstructL
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
void CNssChBackupObserver::ConstructL() 
    {
    RUBY_DEBUG_BLOCK( "CNssVasBackupObserver::ConstructL" );
    User::LeaveIfError( iProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey ) );
       
    // initial subscription and process current property value
    iProperty.Subscribe( iStatus );
    
    CActiveScheduler::Add( this ); 
    SetActive();   
    }
    
// ---------------------------------------------------------
// CNssChBackupObserver::~CNssChBackupObserver
// Destructor
// ---------------------------------------------------------
//
CNssChBackupObserver::~CNssChBackupObserver()
    {
    Cancel();
    iProperty.Close();
    }
// ---------------------------------------------------------
// CNssVasBackupObserver::DoCancel
// Cancel listening now
// ---------------------------------------------------------
//    
void CNssChBackupObserver::DoCancel() 
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------
// CNssChBackupObserver::RunL
// Is called, when property changed
// ---------------------------------------------------------
//
void CNssChBackupObserver::RunL()
    {
    RUBY_DEBUG_BLOCK( "CNssChBackupObserver::RunL" );
    if( iStatus != KErrNone ) 
        {
        // At least report the error
        RUBY_ERROR1( "CNssChBackupObserver::RunL iStatus is [%d]", iStatus.Int() );
        }
        
    // resubscribe before processing new value to prevent missing updates
    // even if some error happened
    iProperty.Subscribe( iStatus );
    SetActive();
    // property updated, get new value
	TInt backupFlag;
	User::LeaveIfError( iProperty.Get( backupFlag ) );
    TBool restoreRelated = EFalse;
	switch ( backupFlag & conn::KBURPartTypeMask )
	    {
	    case conn::EBURNormal:
	        RUBY_DEBUG0( "CNssChBackupObserver::RunL backup/restore not in progress" );
	        iInProgress = EFalse;
	        break;
	        
	    case conn::EBURBackupFull:
	    case conn::EBURBackupPartial:
            restoreRelated = EFalse;
            iInProgress = ETrue;
        break;           
	    case conn::EBURRestoreFull:
	    case conn::EBURRestorePartial:
	        RUBY_DEBUG0( "CNssChBackupObserver::RunL backup/restore in progress" );
            restoreRelated = ETrue;
	        iInProgress = ETrue;
	        break;
	        
	    case conn::EBURUnset:
	    default:
	        RUBY_ERROR1( "CNssChBackupObserver::RunL backup flag unknown %h ", backupFlag );
	    }
	    iHost.BackupRestoreStateChanged( restoreRelated, InProgress() );
    }
    
// ---------------------------------------------------------
// @return ETrue if some backup/restore action is in progress
//         EFalse otherwise
// ---------------------------------------------------------
TBool CNssChBackupObserver::InProgress()
	{
	return iInProgress;
	}

//  End of File
