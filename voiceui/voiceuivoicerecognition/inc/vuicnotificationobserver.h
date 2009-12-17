/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/

#ifndef VUICNOTIFICATIONOBSERVER_H
#define VUICNOTIFICATIONOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*
*
*/
NONSHARABLE_CLASS( CNotificationObserver ) : public CActive
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CNotificationObserver* NewL();

        /**
        * Destructor.
        */
        virtual ~CNotificationObserver();
                                
    public:     // From base classes
    
        /**
        * From CActive 
        * @see CActive for more information
        */
        void DoCancel();
    
        /**
        * From CActive 
        * @see CActive for more information
        */
        void RunL();
         
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CNotificationObserver();
                
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();

    private:    // Data

        // Fold event
        RProperty                           iNotificationState;
        
        // Current fold status
        TInt                                iNotificationStatus;
};

#endif  // VUICNOTIFICATIONOBSERVER_H

// End of File
