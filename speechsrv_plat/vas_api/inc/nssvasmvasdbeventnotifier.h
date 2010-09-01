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
* Description:  MNssVASDBEventNotifier is the interface class for clients to add 
*				 or remove observers to the VAS database. 
*
*/


#ifndef NSSMVASDBEVENTNOTIFIER_H
#define NSSMVASDBEVENTNOTIFIER_H

//  INCLUDES
#include <nssvascoreconstant.h>
#include <nssvasmvasdatabaseobserver.h>


// CLASS DECLARATION

/**
*  MNssVASDBEventNotifier is the interface class for clients to add/remove observers.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class MNssVASDBEventNotifier 
    {
    public:  
      /**
      * Adds an observer to VAS DB
      * @since 2.0
      * @param call back address
      * @return none
      */
	  virtual void AddObserverL(MNssVASDatabaseObserver* aObserver) = 0;

	  /**
      * Removes an observer from VAS DB
      * @param call back address
      * @leave KErrNotFound if given observer has never been added
      */
	  virtual void RemoveObserverL(MNssVASDatabaseObserver* aObserver) = 0;

    };

#endif      // MVASDBEVENTNOTIFIER_H
            
// End of File
