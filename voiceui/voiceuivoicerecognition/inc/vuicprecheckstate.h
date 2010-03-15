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


#ifndef VUICPRECHECKSTATE_H
#define VUICPRECHECKSTATE_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CPrecheckState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CPrecheckState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CPrecheckState();
                                
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
        CPrecheckState( CDataStorage& aDataStorage, CUiModel& aUiModel );

    private:    // Functions
            
        /**
        * Checks auto lock value
        * @return Auto lock value or KErrNotFound
        */
        TInt GetAutoLockValue();
        
        /**
        * Checks if call is active
        * @return Call state or KErrNotFound
        */
        TInt CheckCallState();
        
        /**
        * Checks if video call is active
        * @return ETrue if video call else EFalse
        */
        TBool IsVideoCall();
        
        /**
        * Checks if "lockphone" dialog is active
        * @return ETrue if lockphone dialog else EFalse
        */
        TBool IsLockPhoneDialogL();
                
    private:    // Data

};

#endif  // VUICPRECHECKSTATE_H

// End of File
