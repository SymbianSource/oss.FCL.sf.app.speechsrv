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


#include "nssvasdatasyncwatcher.h"
#include "rubydebug.h"

// Destructors cannot be pure virtual
MNssDataSyncWatcher::~MNssDataSyncWatcher() 
    {
    // empty
    }



///////////////////////////////////
// CNssDataSyncWatcherFactory::ConstructDataSyncWatcherL
//
// Constructs and returns the data sync watcher according to what's
// available in the system. I.e RProperty based watcher or
// shared data client based watcher
// @param aHost Observer to be notified about data sync state changes
//
// @return Constructed and already started watcher
///////////////////////////////////
MNssDataSyncWatcher* CNssDataSyncWatcherFactory::ConstructDataSyncWatcherL( 
                                        MNssDataSyncStateObserver& aHost )
    {
    RUBY_DEBUG_BLOCKL( "ConstructDataSyncWatcherL" );
    MNssDataSyncWatcher* watcher = CNssDataSyncPropertyWatcher::NewL( aHost );
    return watcher;
    }

///////////////////////////////////
// CNssDataSyncPropertyWatcher::NewL
//
// Two phased construction
///////////////////////////////////
CNssDataSyncPropertyWatcher* CNssDataSyncPropertyWatcher::NewL( MNssDataSyncStateObserver& aHost )
    {
    CNssDataSyncPropertyWatcher* self = new (ELeave) CNssDataSyncPropertyWatcher( aHost );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
///////////////////////////////////
// CNssDataSyncPropertyWatcher::CNssDataSyncPropertyWatcher
//
// Constructor
///////////////////////////////////
CNssDataSyncPropertyWatcher::CNssDataSyncPropertyWatcher( MNssDataSyncStateObserver& aHost ) :
    CActive( EPriorityStandard ),
    iHost( &aHost ),
    iDataSyncStatus( EDataSyncNotRunning )
    {
    // Nothing
    }
    
///////////////////////////////////
// CNssDataSyncPropertyWatcher::ConstructL
//
// Second-pase constructor
///////////////////////////////////
void CNssDataSyncPropertyWatcher::ConstructL() 
    {
    RUBY_DEBUG_BLOCK( "CNssDataSyncPropertyWatcher::ConstructL" );
    User::LeaveIfError( iProperty.Attach( KPSUidDataSynchronizationInternalKeys, KDataSyncStatus ) );
    CActiveScheduler::Add( this );
    // initial subscription and process current property value
    RunL();
    }
    
///////////////////////////////////
// CNssDataSyncPropertyWatcher::~CNssDataSyncPropertyWatcher
//
// Destructor
///////////////////////////////////
CNssDataSyncPropertyWatcher::~CNssDataSyncPropertyWatcher()
    {
    if( IsAdded() ) 
        {
        Deque();
        }
    iProperty.Close();
    }
    
///////////////////////////////////
// CNssDataSyncPropertyWatcher::DoCancel
//
// Cancel listening now
///////////////////////////////////
void CNssDataSyncPropertyWatcher::DoCancel() 
    {
    iProperty.Cancel();
    }


///////////////////////////////////
// CNssDataSyncPropertyWatcher::runL
//
// Is called, when property changed
///////////////////////////////////
void CNssDataSyncPropertyWatcher::RunL()
    {
    RUBY_DEBUG_BLOCK( "CNssDataSyncPropertyWatcher::RunL" );
    if( iStatus != KErrNone ) 
        {
        // At least report the error
        RUBY_ERROR1( "CNssDataSyncPropertyWatcher::RunL iStatus is [%d]", iStatus.Int() );
        }
        
    // resubscribe before processing new value to prevent missing updates
    // even if some error happened
    iProperty.Subscribe( iStatus );
    SetActive();
    // property updated, get new value
    TInt dataSyncStatus;
    if ( iProperty.Get( dataSyncStatus) == KErrNotFound )
        {
        // not defined is the same as sync is not running
        dataSyncStatus = EDataSyncNotRunning;
        }
    iDataSyncStatus = static_cast<TDataSyncStatus>( dataSyncStatus );
    iHost->DataSyncStateChanged( InProgress() );

    }
    
///////////////////////////////////
// CNssDataSyncPropertyWatcher::InProgress
//
// Tells if data sync is active right now
///////////////////////////////////
TBool CNssDataSyncPropertyWatcher::InProgress()
	{
	// There can be several different "running" statuses, but only one "not running"
    // So check for not running
	return iDataSyncStatus != EDataSyncNotRunning;
	}

//  End of File
