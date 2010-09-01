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
* Description:  CNssVASDBEventNotifier is the implementation class for interface 
*				 MNssVASDBEventNotifier.
*
*/


#ifndef CNSSVASDBEVENTNOTIFIER_H
#define CNSSVASDBEVENTNOTIFIER_H

//  INCLUDES
#include "nssvasmvasdbeventnotifier.h"
#include "nssvascvasdatabase.h"

// CLASS DECLARATION

/**
*  CNssVASDBEventNotifier is the implementation class for interface MNssVASDBEventNotifier.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssVASDBEventNotifier : public CBase, public MNssVASDBEventNotifier
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CNssVASDBEventNotifier(CNssVASDatabase* aDatabase);

        /**
        * Destructor.
        */
        virtual ~CNssVASDBEventNotifier();

    public: // New functions
        
        /**
        * Add an observer to the VAS DB
		* @since 2.0
        * @param aObserver, Observer to be added
        * @return none
        */
        void AddObserverL(MNssVASDatabaseObserver* aObserver);

    public: // Functions from base classes

        /**
        * Remove an observer from VAS DB
        * @param aObserver, Observer to be removed
        * @leave KErrNotFound if given observer has never been added
        */
        void RemoveObserverL(MNssVASDatabaseObserver* aObserver);
        
  
    private:

        /**
        * EPOC constructor 
        */
        void ConstructL();
	
	private:
		// pointer to VAS Database
		CNssVASDatabase* iDatabase;
    };

#endif      // CVASDBEVENTNOTIFIER_H  
            
// End of File
