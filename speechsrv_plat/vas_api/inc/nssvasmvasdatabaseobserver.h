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
* Description:  MNssVASDatabaseObserver is the interface class for VAS DB event 
*				 notification to observers. An observer must derive from 
*				 MNssVASDatabaseObserver and implement its HandleVASDBEvent
*				 (TVASDBEVent aEvent) method to handle the event. 
*
*/


#ifndef MNSSVASDATABASEOBSERVER_H
#define MNSSVASDATABASEOBSERVER_H

//  INCLUDES
#include <nssvascvasdbevent.h>

// CLASS DECLARATION

/**
* MNssVASDatabaseObserver is the interface class for VAS DB event notification. 
* An observer must derive from MNssVASDatabaseObserver and implement 
* its HandleVASDBEvent(CNssVASDBEvent aEvent) method to handle the event.
* The clint is responsible for deleting the event object. 
*  @lib NssVASApi.lib
*  @since 2.8
*/
class MNssVASDatabaseObserver 
    {
    public: 
      /**
      * Callback for event notification
      * @since 2.0
      * @param aEvent 
      * @return 
      */
	  virtual void HandleVASDBEvent(CNssVASDBEvent *aEvent) = 0;
    };

#endif      // MNSSVASDATABASEOBSERVER_H
            
// End of File
