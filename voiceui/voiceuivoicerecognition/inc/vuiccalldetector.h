/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUICCALLDETECTOR_H
#define VUICCALLDETECTOR_H

#include <e32base.h>
#include <e32property.h>

class MCallDetectorObserver;

/**
 *  Detector for incoming call
 */
NONSHARABLE_CLASS( CCallDetector ) : public CActive
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCallDetector* NewL( MCallDetectorObserver* aObserver );

        /**
        * Destructor.
        */
        ~CCallDetector();
        
    public: // New functions

        /**
        * Request call notifications
        *
        */
        void RequestNotification();

    private:

        /**
        * C++ default constructor.
        */
        CCallDetector( MCallDetectorObserver* aObserver );

        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();
    
        /**
        * From CActive
        * @see CActive for more information
        */
        void RunL();
        
        /**
        * From CActive
        * @see CActive for more information
        */
        void DoCancel();

    private:

        // Call observer
        MCallDetectorObserver*          iObserver;
        
        RProperty                       iTsyProperty;
    };

#endif // VUICCALLDETECTOR_H
