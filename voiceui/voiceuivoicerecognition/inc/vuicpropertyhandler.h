/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUICPROPERTYHANDLER_H
#define VUICPROPERTYHANDLER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARDS
class CVoiceRecognitionDialogImpl;

// CLASS DECLARATION

/**
* Active object for subscribing to Publish & Suscribe properties
*/
NONSHARABLE_CLASS( CPropertyHandler ) : public CActive
    {
    public:  // Constructors and destructor
        /**
        * Symbian two-phased constructor.
        * @param aCategory Category of the property to watch
        * @param aKey      Key UID of the property to watch
        * @param aCallback Callback to get notifications about changes
        */
        static CPropertyHandler* NewL( TUid aFirstCategory,
                                       TUint aFirstKey,
                                       TUid aSecondCategory,
                                       TUint aSecondkey);

        // Destructor
        virtual ~CPropertyHandler();

    public:  // New functions
        
        /**
        * Method to handle the deregistration of monitoring accessories
        * @param 
        */  
        void Register( CVoiceRecognitionDialogImpl* aRecogObserver );

        /**
        * Method to handle the deregistration of monitoring accessories
        * @param 
        */  
        void DeRegister( CVoiceRecognitionDialogImpl* aRecogObserver );

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
        void ConstructL( TUid aFirstCategory,
                         TUint aFirstKey,
                         TUid aSecondCategory,
                         TUint aSecondKey );

        /**
        * C++ constructor.
        */
        CPropertyHandler();

    private:    // Data

        RProperty                       iProperty;
        TCallBack                       iCallback;
        TBool                           iRequestIssued;
        CVoiceRecognitionDialogImpl*    iRecogObserver;
        TUid                            iFirstCategory;
        TUint                           iFirstKey;
    };

#endif    // VUICPROPERTYHANDLER_H

// End of File
