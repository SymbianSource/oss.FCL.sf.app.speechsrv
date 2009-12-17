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


#ifndef VUICADAPTSTATE_H
#define VUICADAPTSTATE_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS

class MNssAdaptationItem;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CAdaptState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CAdaptState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CAdaptState();
                                
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
        CAdaptState( CDataStorage& aDataStorage, CUiModel& aUiModel );

    private:    // Functions
                
    private:    // Data
    
        // Adaptation item
        MNssAdaptationItem*                 iAdaptationItem;
};

#endif  // VUICADAPTSTATE_H

// End of File
