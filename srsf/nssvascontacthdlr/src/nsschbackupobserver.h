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


#ifndef NSSCHBACKUPOBSERVER_H
#define NSSCHBACKUPOBSERVER_H

//  INCLUDES

#include <e32base.h>
#include "nssvasccontacthandlerimpl.h"

// Publish & Subscribe of the contact handler activity
#include <e32property.h>  

// CONSTANTS


// MACROS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 @todo generalize the common parts of this class and CNssVasBackupObserver from VAS API
       At the moment the code is copy-pasted and duplicated
*/



/**
* RProperty based data sync watcher implementation
*/
NONSHARABLE_CLASS( CNssChBackupObserver ) : public CActive
    {
    public:
        /** 
        *  Factory construction
        *  @param aDb Database 
        *  @return Constructed and already started observer
        */
        static CNssChBackupObserver* NewL( CNssContactHandlerImplementation& aHost );
        
        /** 
        * Destructor 
        */
        ~CNssChBackupObserver();
        
        /**
        * @return ETrue if some backup/restore action is in progress
        *         EFalse otherwise
        */
        TBool InProgress();

    private:
        /**
        * C++ constructor 
        */
        CNssChBackupObserver( CNssContactHandlerImplementation& aHost );
        
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
        // Publish & Subscribe property about backup/restore 
        RProperty   iProperty;
        
        // Object to be notified about the backup/restore
        CNssContactHandlerImplementation& iHost;
        
        // ETrue if some backup/restore action is in progress
        TBool iInProgress;
    };

#endif // NSSCHBACKUPOBSERVER_H
            
// End of File
