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
* Description:  Definition of class for dialing a phone number.
*
*/


#ifndef VCMANAGERDIALER_H
#define VCMANAGERDIALER_H

//  INCLUDES
#include <e32base.h>    // CBase

enum TCallType
    {
    EVoice,
    EVideo,
    EVoIP
    };
    
// FORWARD DECLARATIONS
class CAiwServiceHandler;
    
// CLASS DEFINITION

/**
* Does the dialing based on recognized voice tag.
*
*/
NONSHARABLE_CLASS ( CVcManagerDialer ) : public CBase
    {
    public: 

        /**
        * Two-phased constructor.
        */
        static CVcManagerDialer* NewL();

        /**
        * Destructor.
        */
        ~CVcManagerDialer();

    public: 
	
        /**
        * Dials to given number
        * @param aName name of the contact
        * @param aNumber number to be called
        * @param aCallType Type of calling (voice, video, VoIP)
        */
        void ExecuteL( TPtrC aName, TPtrC aNumber, TCallType aCallType );
        
    private:
    
        /**
        * Default constructor.
        */
        CVcManagerDialer();
        
        /**
        * Second phase constructor.
        */
        void ConstructL();
        
    private:
    
        // Aiw service handler
        CAiwServiceHandler* iServiceHandler;

    };

#endif // VCMANAGERDIALER_H
