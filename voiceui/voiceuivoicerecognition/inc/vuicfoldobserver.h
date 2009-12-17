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


#ifndef VUICFOLDOBSERVER_H
#define VUICFOLDOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class MVoiceRecognitionDialogCallback;

// CLASS DECLARATION
/**
* Fold state observer
*
*/
NONSHARABLE_CLASS( CFoldObserver ) : public CActive
{
    public:     // Constructors and destructor

/**
        * Symbian two-phased constructor.
        */
        static CFoldObserver* NewL( MVoiceRecognitionDialogCallback* aCallback );

        /**
        * Destructor.
        */
        virtual ~CFoldObserver();
                                
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
        CFoldObserver( MVoiceRecognitionDialogCallback* aCallback );
                
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();

    private:    // Data

        // Callback observer
        MVoiceRecognitionDialogCallback*    iCallback;

        // Fold event
        RProperty                           iFoldEvent;
        
        // Current fold status
        TInt                                iFoldStatus;
};

#endif  // VUICFOLDOBSERVER_H

// End of File
