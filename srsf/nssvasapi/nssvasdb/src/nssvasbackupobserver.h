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


#ifndef NSSBACKUPOBSERVER_H
#define NSSBACKUPOBSERVER_H

//  INCLUDES

#include <e32base.h>

// Publish & Subscribe of the contact handler activity
#include <e32property.h>  
#include "nssvascvasdb.h"

// CONSTANTS


// MACROS

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
* RProperty based data sync watcher implementation
*/
NONSHARABLE_CLASS( CNssVasBackupObserver ) : public CActive
    {
    public:
        /** 
        *  Factory construction
        *  @param aDb Database 
        *  @return Constructed and already started observer
        */
        static CNssVasBackupObserver* NewL( CNssVasDb& aDb );
        
        /** 
        * Destructor 
        */
        ~CNssVasBackupObserver();
        
    private:
        /**
        * C++ constructor 
        */
        CNssVasBackupObserver( CNssVasDb& aDb );
        
        /**
        * Symbian second phase constructor 
        */
        void ConstructL();
        
        /**
        * is called when data sync property changed 
        */
        void RunL();
        
        /**
        * Is called when system wants immediate cancelling of the watching 
        */
        void DoCancel();
        
    private:
        // Publish & Sunscribe property about backup/restore 
        RProperty   iProperty;
        
        // Database to be notified about the backup/restore
        CNssVasDb& iDb;
    };

#endif // NSSBACKUPOBSERVER_H
            
// End of File
