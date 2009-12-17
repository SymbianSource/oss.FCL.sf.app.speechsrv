/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssVASDBEvent encapsulates events occuring on the VAS DB.
*
*/


#ifndef TNSSVASDBEVENT_H
#define TNSSVASDBEVENT_H

//  INCLUDES
#include <nssvasmtag.h>

// CLASS DECLARATION

/**
*  CNssVASDBEvent class encapsulates a VAS DB event.
*  A VAS DB evnt consists of the type of event and the tag object on which the event occured. 
*  The enumeration above shows the event types. Symbian DBMS event types do not have tag
*  tag with them (set to NULL). The client is responsible for deleting the event object,
*  otherwise a memory leak would occur.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssVASDBEvent: public CBase
    {
	public:

	// ENUMS
	enum TVASDBEventType
	{
		// standard Symbian DBMS event types (from RDBNotifier)
		// event notification across threads possible,
		EVASDatabaseEventNull, 
		EVASDatabaseEventDatabaseClose,
		EVASDatabaseEventDatabaseUnlock,
		EVASDatabaseEventDatabaseCommit,
		EVASDatabaseEventDatabaseRollback,
		EVASDatabaseEventDatabaseRecover,
		// In addition, following event types are also supported by VAS.
		// These events are notifieds only within the same thread. The
		// observers and event originator must be on the same thread.
		// future releases would allow event noification across threads
		EVASDatabaseEventTagAdded, 
		EVASDatabaseEventTagChanged, 
		EVASDatabaseEventTagDeleted
	};
    
	public:  // Constructors and destructor
     
        /**
        * 2Phase Constructor
		* @param aType - event type, 
		* @param aTag - tag on which the event occured
		* @return 
        */
        static CNssVASDBEvent* NewL(TVASDBEventType& aType, MNssTag *aTag);

		 /**
        * 2Phase Constructor
		* @param aType - event type, 
		* @param aTag - tag on which the event occured
		* @return 
        */
        static CNssVASDBEvent* NewLC(TVASDBEventType& aType, MNssTag *aTag);

		/**
        * C++ destructor
		* @param 
		* @return 
        */
        ~CNssVASDBEvent();

        /**
        * Get function to determine the type of event
		* @since 2.0
        * @param void
        * @return Type of event
        */
        IMPORT_C TVASDBEventType Type();

		/**
        * Function to get the tag object on which the event took place.
		* Deleting the event, deletes the tag associated with it.
		* @since 2.0
        * @param void
        * @return Pointer to a tag object on which event occured
        */
        IMPORT_C MNssTag* Tag();
		

	private:
		
		/**
        * C++ constructor.
        */
        CNssVASDBEvent();

		/**
        * EPOC Constructor
		* @param aType - event type, 
		* @param aTag - tag on which the event occured
		* @return 
        */
		void ConstructL(TVASDBEventType& aType, MNssTag *aTag);

		// Data
        TVASDBEventType		iType;
		MNssTag*				iTag;	// NULL for standard Symbian DBMS events.
    };

#endif      // TVASDBEVENT_H  
            
// End of File
