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


#include "nssbackupobserver.h"
#include "rubydebug.h"

#include <sbdefs.h>

// ---------------------------------------------------------
// CNssTag::NewL
// NewL
// ---------------------------------------------------------
//
CNssBackupObserver* CNssBackupObserver::NewL( CSIDatabase& aDb )
    {
    CNssBackupObserver* self = new (ELeave) CNssBackupObserver( aDb );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CNssBackupObserver::CNssBackupObserver
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
CNssBackupObserver::CNssBackupObserver( CSIDatabase& aDb ) :
    CActive( CActive::EPriorityHigh ),
    iDb( aDb )
    {
    // Nothing
    }
    
// ---------------------------------------------------------
// CNssBackupObserver::ConstructL
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
void CNssBackupObserver::ConstructL() 
    {
    RUBY_DEBUG_BLOCK( "CNssBackupObserver::ConstructL" );
    User::LeaveIfError( iProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey ) );
    
    // check the current value
    TInt backupFlag( conn::EBURUnset ); 
	User::LeaveIfError( iProperty.Get( backupFlag ) );
	
    switch ( backupFlag & conn::KBURPartTypeMask )
	    {
	    case conn::EBURNormal:
	        // normal nothing to do
	        break;
	        
	    case conn::EBURBackupFull:
	    case conn::EBURBackupPartial:
	    case conn::EBURRestoreFull:
	    case conn::EBURRestorePartial:
	        // Lock the use of database
	        RUBY_DEBUG0( "CNssBackupObserver::ConstructL Lock transactions" );
	        iDb.LockTransactionsL();
	        break;
	        
	    case conn::EBURUnset:
	    default:
	        RUBY_DEBUG0( "CNssBackupObserver::ConstructL unknown value" );
	    }   
	     
    // initial subscription and process current property value
    iProperty.Subscribe( iStatus );
    
    CActiveScheduler::Add( this ); 
    SetActive();   
    }
    
// ---------------------------------------------------------
// CNssBackupObserver::~CNssBackupObserver
// Destructor
// ---------------------------------------------------------
//
CNssBackupObserver::~CNssBackupObserver()
    {
    Cancel();
    iProperty.Close();
    }
// ---------------------------------------------------------
// CNssBackupObserver::DoCancel
// Cancel listening now
// ---------------------------------------------------------
//    
void CNssBackupObserver::DoCancel() 
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------
// CNssBackupObserver::RunL
// Is called, when property changed
// ---------------------------------------------------------
//
void CNssBackupObserver::RunL()
    {
    RUBY_DEBUG_BLOCK( "CNssBackupObserver::RunL" );
    
    if( iStatus != KErrNone ) 
        {
        // At least report the error
        RUBY_ERROR1( "CNssBackupObserver::RunL iStatus is [%d]", iStatus.Int() );
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
	        RUBY_DEBUG0( "CNssBackupObserver::RunL Unlock transactions" );
	        iDb.UnlockTransactionsL();
	        break;
	        
	    case conn::EBURBackupFull:
	    case conn::EBURBackupPartial:
	    case conn::EBURRestoreFull:
	    case conn::EBURRestorePartial:
	        // Lock the use of database
	        RUBY_DEBUG0( "CNssBackupObserver::RunL Lock transactions" );
	        iDb.LockTransactionsL();
	        break;
	        
	    case conn::EBURUnset:
	    default:
	        RUBY_ERROR1( "CNssBackupObserver::RunL backup flag unknown %h ", backupFlag );
	    }
    }

//  End of File
