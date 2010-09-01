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


#ifndef VUICVERIFICATIONINITSTATE_H
#define VUICVERIFICATIONINITSTATE_H

// INCLUDES
#include "vuicstate.h"

#include "voiceuibldvariant.hrh"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CVerificationInitState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CVerificationInitState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CVerificationInitState();
                                
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
        CVerificationInitState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
    private:    // Functions
    
        /**
        * Initializes recognition system
        * @since 3.2
        */
        void InitializeL();
        
#ifdef __FULLDUPLEX_CHANGE
        /**
        * Starts sampling
        * @since 3.2
        */
        void StartSamplingL();
#endif // __FULLDUPLEX_CHANGE
        
        /**
        * Starts recognition initialization
        * @since 3.2
        */
        void RecognizeInitL();
                
    private:    // Data

        enum TInternalState
            {
            ENotStarted,
            EStarted,
#ifdef __FULLDUPLEX_CHANGE
            ESamplingStarted,
#endif // __FULLDUPLEX_CHANGE
            EInitialized,
            EReady
            };

        // For handling internal state logics
        TInternalState      iInternalState;
};

#endif  // VUICVERIFICATIONINITSTATE_H

// End of File
