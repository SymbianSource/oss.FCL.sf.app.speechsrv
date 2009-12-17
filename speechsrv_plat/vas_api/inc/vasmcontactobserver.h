/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contact observer interface
*
*/


#ifndef VASMCONTACTOBSERVER_H
#define VASMCONTACTOBSERVER_H

// INCLUDES
#include <e32std.h>

#include <cntdef.h>

// CONSTANTS

// DATA TYPES
   
// Contact event
enum TVasContactEvent
    {
    ENullEvent,                     // No event occured.  
    EContactAdded,                  // A contact has been added to the contact store.  
    EContactDeleted,                // A contact has been deleted from the contact store.  
    EContactChanged,                // An existing contact in the contact store has been changed.  
    EGroupAdded,                    // A contact group has been added to the contact store.  
    EGroupDeleted,                  // A contact group has been deleted from the contact store.  
    EGroupChanged,                  // An existing contact group in the contact store has been changed.  
    EStoreBackupBeginning,          // Contact store backup is beginning.  
    EStoreRestoreBeginning,         // Contact store restore is beginning.  
    EStoreBackupRestoreCompleted,   // Contact store backup or restore is completed.  
    EUnknownChanges,                // There has been some unknown changes in the contact store.  
    ENotSupported                   // Other kind of changes that are not supported
    };
    
struct TPhonebookEvent
    {
    TTime iTime;
    TVasContactEvent iType;
    TContactItemId iContactId;
     
    TPhonebookEvent()
        {
        iType = EContactChanged;
        iContactId = 0;
        iTime.UniversalTime();
        }
    };

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Contact observer interface
*
* @since 3.2
*/
class MVasContactObserver
    {
    public:  // Constructors and destructor
   
    public: // New functions

        /**
        * Called when a contact change event occurs
        *
        * @since 3.2
        */
        virtual void HandleEventL( const TPhonebookEvent& aEvent ) = 0;
        
    public: // Functions from base classes
        
    private:  // New functions
               
    private:    // Data
    
        };

#endif      // VASMCONTACTOBSERVER_H
            
// End of File
