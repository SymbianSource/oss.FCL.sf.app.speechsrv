/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CVASDBMonitor class is an active object that is a wrapper around 
*               the RDBNotifier class. Events from RDBNotifier are handled by 
*               encapsulating them in a CNssVASDBEvent object and passing it to all 
*               its observers. In addition to the events provided by RDBNotifier, 
*               CVASDBMonitor also provides additional events when a entry is added,
*               changed or deleted from the VAS DB. CVASDBMonitor owns the list of 
*               observers of the VAS DB.
*
*/


#ifndef CNSSVASDBEVENTMONITOR_H
#define CNSSVASDBEVENTMONITOR_H

//  INCLUDES
#include <d32dbms.h>

#include "nssvascoreconstant.h"
#include "nssvascvasdbevent.h"
#include "nssvasmvasdatabaseobserver.h"
#include "nssvasctag.h"


// CLASS DECLARATION

/**
*  CNssVASDBEventMonitor is an active object responsible for observing VAS database changes
*  and notifying all registerted observers of the change.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssVASDBEventMonitor :public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNssVASDBEventMonitor* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CNssVASDBEventMonitor();

    public: // New functions
        
        /**
        * Request to start monitoring the VAS DB 
		* @since 2.0
        * @param void
        * @return void
        */
        void StartMonitoring();

		/**
        * Method that adds an observer to the observer list 
		* @since 2.0
        * @param aObserver to be added
        * @return none
        */
        void AddObserverL(MNssVASDatabaseObserver* aObserver);

		/**
        * Method that removes an observer from the observer list.
		* @since 2.0
        * @param aObserver to be removed
        * @return error code
        */
        TInt RemoveObserver(MNssVASDatabaseObserver* aObserver);


    public: // Functions from base classes

        /**
        * From CActive, called by RunL of RDBNotifier
		* @since 2.0
        * @param void
		* @return void
        */
        void RunL();

		/**
        * From CActive, cancel outstanding requests
		* @since 2.0
        * @param void
		* @return void
        */
        void DoCancel();

		/**
        * handling of addional events ( tag added, tag deleted, tag changed)
		* These events can be reported only when the events occur on the same 
		* thread as that of the observer (client monitoring the event).
		* @since 2.0
        * @param aEvent - VAS DB event type
		* @param aTag - tag on which the event occured
        * @return void
        */
		void HandleDatabaseEventL(CNssVASDBEvent::TVASDBEventType aEvent, CNssTag *aTag);

	private:

        /**
        * C++ constructor.
        */
        CNssVASDBEventMonitor();

        /**
        * EPOC constructor
        */
        void ConstructL();

		/**
        * Called by RunL() to create an event object and 
		* call HandleDatabaseEvent() for all observers
		* Handles Standard Symbian DBMS events
		* @since 2.0
        * @param aEvent - Symbian DBMS event
        * @return void
        */
        void HandleDatabaseEventL(RDbNotifier::TEvent aEvent);


    private:    // Data

		RDbs					iDbSession;
		RDbNamedDatabase		iDatabase;
        RDbNotifier				iNotifier;
		CArrayPtrFlat<MNssVASDatabaseObserver>* iObservers;

    };

#endif      // CVASDBEVENTMONITOR_H  
            
// End of File
