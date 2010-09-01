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


#ifndef VUICKEYGRABBER_H
#define VUICKEYGRABBER_H

// INCLUDES
#include <e32base.h>
#include <w32std.h>

// FORWARD DECLARATIONS
class MKeyCallback;

// CLASS DECLARATION
/**
* 
*
*/
NONSHARABLE_CLASS( CKeyGrabber ) : public CActive
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CKeyGrabber* NewL( MKeyCallback* aKeyObserver );

        /**
        * Destructor.
        */
        virtual ~CKeyGrabber();
                                
    public:     // New functions    
    
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
        CKeyGrabber( MKeyCallback* aKeyObserver );
                
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
        
    private:    // Data

        // Key callback
        MKeyCallback*                   iKeyObserver;

        // Window group
        RWindowGroup                    iWindowGroup;
        
        // Window server session
        RWsSession                      iWsSession;
        
        // Camera key handle
        TInt                            iCameraHandle;
        
        // Poc key handle
        TInt                            iPocHandle;
        
        // Power key handle
        TInt                            iPowerHandle;
        
        // Application key handle
        TInt                            iApplicationHandle;

};

#endif  // VUICKEYGRABBER_H

// End of File
