/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Watcher for the PC-Suite data sync state
*
*/


#ifndef NSSVASDATASYNCWATCHER_H
#define NSSVASDATASYNCWATCHER_H

//  INCLUDES

#include <e32base.h>

// Publish & Subscribe of the contact handler activity
#include <e32property.h>  

#include <DataSyncInternalPSKeys.h>

// CONSTANTS


// MACROS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface for classes that want to be notified about the PC Suite data
*  sync status: is it running or not
*/
NONSHARABLE_CLASS( MNssDataSyncStateObserver ) 
    {
    public:
        /** 
        *  Is called when data sync status is changed and when data sync state
        *  observation was just started
        *  @param aRunning ETrue if from now on data sync process is running
        *                  EFalse otherwise
        */
        virtual void DataSyncStateChanged( TBool aRunning ) = 0;
    };


/** 
*  Watches for the changes of the data sync state (is it running) or not
*  and notifies the host observer about it
*/
NONSHARABLE_CLASS( MNssDataSyncWatcher )
    {
    public:
        virtual ~MNssDataSyncWatcher();
        /**
        * @return ETrue if data sync is in progress, EFalse otherwise
        */
        virtual TBool InProgress() = 0;
    };
    
/** 
*  Creates and returns 
*/
NONSHARABLE_CLASS ( CNssDataSyncWatcherFactory ) : public CBase
    {
    public:
        /**
        *  Constructs and returns the data sync watcher according to what's
        *  available in the system. I.e RProperty based watcher or
        *  shared data client based watcher
        *  @param aHost Observer to be notified about data sync state changes
        *
        *  @return Constructed and already started watcher
        */
        static MNssDataSyncWatcher* ConstructDataSyncWatcherL( MNssDataSyncStateObserver& aHost );
    };

/**
* RProperty based data sync watcher implementation
*/
NONSHARABLE_CLASS ( CNssDataSyncPropertyWatcher ) : public CActive, public MNssDataSyncWatcher
    {
    public:
        /** 
        *  Factory construction
        *  @param aHost Observer to be notified about data sync state changes
        *  @return Constructed and already started CNssDataSyncWatcher
        */
        static CNssDataSyncPropertyWatcher* NewL( MNssDataSyncStateObserver& aHost );
        
        /** 
        * Destructor 
        * Doesn't need to be declared as virtual. Is virtual since CBase 
        * implementation
        */
        ~CNssDataSyncPropertyWatcher();
        
        /**
        * @return ETrue if data sync is in progress, EFalse otherwise
        */
        virtual TBool InProgress();
    private:
        /** C++ constructor */
        CNssDataSyncPropertyWatcher( MNssDataSyncStateObserver& aHost );
        
        /** Symbian second phase constructor */
        void ConstructL();
        
        /** is called when data sync property changed */
        void RunL();
        
        /** Is called when system wants immediate cancelling of the watching */
        void DoCancel();
    private:
        // Publish & Sunscribe property about the data sync state
        RProperty   iProperty;
        
        // Host to be notified about the state changes
        MNssDataSyncStateObserver* iHost;
        
        // current data sync status
        TDataSyncStatus iDataSyncStatus;
    };

#endif // NSSVASDATASYNCWATCHER_H
            
// End of File
