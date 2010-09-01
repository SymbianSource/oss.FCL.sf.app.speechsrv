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


#ifndef VCMANAGERBACKUPOBSERVER_H
#define VCMANAGERBACKUPOBSERVER_H

//  INCLUDES

#include <e32base.h>

// Publish & Subscribe of the contact handler activity
#include <e32property.h>  

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 @todo generalize the common parts of this class, CNssChBackupObserver
       and CNssVasBackupObserver from VAS API
       At the moment the code is copy-pasted and duplicated
*/



/**
* RProperty based data sync watcher implementation
*/
NONSHARABLE_CLASS( CVCommandManagerBackupObserver ) : public CActive
    {
    public:
        /** 
        *  Factory construction
        *  @return Constructed and already started observer
        */
        static CVCommandManagerBackupObserver* NewL();
        
        /** 
        * Destructor 
        */
        ~CVCommandManagerBackupObserver();
 
    private:
        /**
        * C++ constructor 
        */
        CVCommandManagerBackupObserver();
        
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
        
        /**
        * Enables voice command retraining
        */
        void SetVoiceCommandRetrainingL();
        
    private:
        // Publish & Subscribe property about backup/restore 
        RProperty   iProperty;
        
        // Has restore started
        TBool       iRestoreStarted;
    };

#endif // VCMANAGERBACKUPOBSERVER_H
            
// End of File
