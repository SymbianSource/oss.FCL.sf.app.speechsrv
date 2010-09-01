/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssSelectNotificationBuilder class performs initialization 
*                for the CNssTagSelectNotification class. 
*
*/


#ifndef CNSSSELECTNOTIFICATIONBUILDER_H
#define CNSSSELECTNOTIFICATIONBUILDER_H

//  INCLUDES

#include <e32base.h>
#include <e32def.h>
#include "nssvasmtagselectnotification.h"

// FORWARD DECLARATIONS

class CNssTagSelectNotification; 

// CLASS DECLARATION

/**
*
*  The CNssSelectNotificationBuilder class performs initialization for the 
*  CNssTagSelectNotification class.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/

class CNssSelectNotificationBuilder :public CBase
    {
    public:  // Constructors and destructor
			 
        /**
        * Two-phased constructor.
        */		
        IMPORT_C static CNssSelectNotificationBuilder* NewL();

        /**
        * Two-phased constructor.
        */	
        IMPORT_C static CNssSelectNotificationBuilder* NewLC();

        /**
        * Destructor.
        */	
        ~CNssSelectNotificationBuilder();

    public: // New functions

        /**
        * Initialize Tag Select Notification object 
        * @since 2.0
        * @return no return value.
        */ 
        IMPORT_C void InitializeL();

        /**
        * Get a Pointer to Tag Select Notification object.
        * @since 2.0
        * @return MNssTagSelectNotification* Pointer to the MNssTagSelectNotification
		*                                 object.
        */ 
        IMPORT_C MNssTagSelectNotification* GetTagSelectNotification();

    private:

        /**
        * Constructor. 
        */ 
        CNssSelectNotificationBuilder();

    private:    // Data

		// Pointer to the Tag Select Notification object.
        CNssTagSelectNotification* iTagSelectNotification;

    };

#endif      // CNSSSELECTNOTIFICATIONBUILDER_H

// End of File
