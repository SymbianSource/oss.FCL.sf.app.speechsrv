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
* Description:  
*
*/


#ifndef VUICABORTSTATE_H
#define VUICABORTSTATE_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CAbortState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CAbortState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CAbortState();
                                
    public:     // New functions
    
        /**
        * Handles received event
        * @param Event to be handled
        */
        virtual void HandleEventL( TInt aEvent );
        
        /**
        * Executes the state functionality
        */
        virtual void ExecuteL();
         
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CAbortState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
    private:    // Functions
                
    private:    // Data
};

#endif  // VUICABORTSTATE_H

// End of File
