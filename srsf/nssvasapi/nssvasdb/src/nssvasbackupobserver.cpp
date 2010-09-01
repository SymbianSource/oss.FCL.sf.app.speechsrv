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


#include "nssvasbackupobserver.h"
#include "rubydebug.h"

#include <sbdefs.h>

// ---------------------------------------------------------
// CNssTag::NewL
// NewL
// ---------------------------------------------------------
//
CNssVasBackupObserver* CNssVasBackupObserver::NewL( CNssVasDb& aDb )
    {
    CNssVasBackupObserver* self = new (ELeave) CNssVasBackupObserver( aDb );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CNssVasBackupObserver::CNssVasBackupObserver
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
CNssVasBackupObserver::CNssVasBackupObserver( CNssVasDb& aDb ) :
    CActive( CActive::EPriorityHigh ),
    iDb( aDb )
    {
    // Nothing
    }
    
// ---------------------------------------------------------
// CNssVasBackupObserver::ConstructL
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
void CNssVasBackupObserver::ConstructL() 
    {
    RUBY_DEBUG_BLOCK( "CNssVasBackupObserver::ConstructL" );
    User::LeaveIfError( iProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey ) );
       
    // initial subscription and process current property value
    iProperty.Subscribe( iStatus );
    
    CActiveScheduler::Add( this ); 
    SetActive();   
    }
    
// ---------------------------------------------------------
// CNssVasBackupObserver::~CNssVasBackupObserver
// Destructor
// ---------------------------------------------------------
//
CNssVasBackupObserver::~CNssVasBackupObserver()
    {
    Cancel();
    iProperty.Close();
    }
// ---------------------------------------------------------
// CNssVasBackupObserver::DoCancel
// Cancel listening now
// ---------------------------------------------------------
//    
void CNssVasBackupObserver::DoCancel() 
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------
// CNssVasBackupObserver::RunL
// Is called, when property changed
// ---------------------------------------------------------
//
void CNssVasBackupObserver::RunL()
    {
    RUBY_DEBUG_BLOCK( "CNssVasBackupObserver::RunL" );
    
    if( iStatus != KErrNone ) 
        {
        // At least report the error
        RUBY_ERROR1( "CNssVasBackupObserver::RunL iStatus is [%d]", iStatus.Int() );
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
	        // Database can be used again
	        RUBY_DEBUG0( "CNssVasBackupObserver::RunL Unlock transactions" );
	        iDb.UnlockTransactionsL();
	        break;
	        
	    case conn::EBURBackupFull:
	    case conn::EBURBackupPartial:
	    case conn::EBURRestoreFull:
	    case conn::EBURRestorePartial:
	        // Lock the use of database
	        RUBY_DEBUG0( "CNssVasBackupObserver::RunL Lock transactions" );
	        iDb.LockTransactionsL();
	        break;
	        
	    case conn::EBURUnset:
	    default:
	        RUBY_ERROR1( "CNssVasBackupObserver::RunL backup flag unknown %h ", backupFlag );
	    }
    }

//  End of File
