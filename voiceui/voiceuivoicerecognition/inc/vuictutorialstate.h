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


#ifndef VUICTUTORIALSTATE_H
#define VUICTUTORIALSTATE_H

// INCLUDES
#include "vuicstate.h"
#include "vuictutorial.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CTutorialState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CTutorialState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CTutorialState();
                                
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
        CTutorialState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();

    private:    // Functions
                
    private:    // Data

        enum TInternalState
            {
            ENotStarted,
            EStarted,
            EInitialized,
            ECompleted
            };

        // For checking current tutorial state
        CTutorial::TTutorialState       iCurrentTutorialState;
        
        // For handling internal state logics
        TInternalState                  iInternalState;
};

#endif  // VUICTUTORIALSTATE_H

// End of File
