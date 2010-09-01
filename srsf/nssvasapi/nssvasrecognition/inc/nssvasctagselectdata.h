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
* Description:  The CNssTagSelectData Class contains the data for tag select 
*                notification.
*
*/


#ifndef CNSSTAGSELECTDATA_H
#define CNSSTATSELECTDATA_H

// INCLUDES

#include "nssvasccontext.h"
#include "nssvasmtagselecthandler.h"


// CLASS DEFINITIONS

/**
*
*  The CNssTagSelectData class contains the data for tag select notification. 
*
*  @lib NssVASApi.lib
*  @since 2.8
*/

class CNssTagSelectData : public CBase
{

    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */		
        static CNssTagSelectData* NewL(CNssContext* aContext, 
								    MNssTagSelectHandler* aTagSelectHandler);

        /**
        * Two-phased constructor.
        */	
		static CNssTagSelectData* NewLC(CNssContext* aContext, 
								     MNssTagSelectHandler* aTagSelectHandler);

        /**
        * Destructor.
        */	
		~CNssTagSelectData();


    public: // New functions
        
        /**
        * Get Context
        * @since 2.0
        * @return A pointer to the context.
        */
        CNssContext* Context();

        /**
        * Get TagSelectHandler
        * @since 2.0
        * @return A pointer to the TagSelectHandler event handler.
        */
        MNssTagSelectHandler* TagSelectHandler();

    private:

        /**
        * Constructor.
        */	
		CNssTagSelectData();

       /**
        * By default Symbian 2nd phase constructor is private.
        */		
        void ConstructL(CNssContext* aContext, 
					    MNssTagSelectHandler* aTagSelectHandler);

    private:

		// Context - The context the selected voice tag appears in
		CNssContext* iContext;

		// Event Callback - The event handler to be notified when a voice tag is 
		// selected in the context.
		MNssTagSelectHandler* iTagSelectHandler;

};

#endif // CNssTagSELECTDATA_H

// End of File
