/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active object for subscribing to Publish & Suscribe properties
*
*/


#ifndef CAPPCONTROLLERPROPERTYHANDLER_H
#define CAPPCONTROLLERPROPERTYHANDLER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARDS
class CVCApplicationControllerImpl;

// CLASS DECLARATION

/**
* Active object for subscribing to Publish & Suscribe properties
* @since 2.8
*/
class CAppControllerPropertyHandler : public CActive
    {
    public:  // Constructors and destructor
        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aCategory Category of the property to watch
        * @param aKey      Key UID of the property to watch
        * @param aCallback Callback to get notifications about changes
        */
        static CAppControllerPropertyHandler* NewL(
            TUid aCategory,
            TUint aKey,
            CVCApplicationControllerImpl* aController );

        // Destructor
        virtual ~CAppControllerPropertyHandler();

    public:  // New functions
        
		/**
        * Get current property value.
        * @since 2.8
        * @return current (integer) value of this property
        */
        TInt Value();


    private: // Functions from base classes

        /**
        * From CActive.
        * Handle a change event.
        */
        void RunL();

        /**
        * From CActive.
        * Cancel the request to receive events.
        */
        void DoCancel();

    private: // New functions

        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL(TUid aCategory,
						TUint aKey );

        /**
        * C++ constructor.
        */
        CAppControllerPropertyHandler(CVCApplicationControllerImpl* aController);

    private:	// Data

        RProperty					  iProperty;
        TCallBack					  iCallback;
        TBool						  iRequestIssued;
		CVCApplicationControllerImpl* iController;
    };

#endif

// End of File
