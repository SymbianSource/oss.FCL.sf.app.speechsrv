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


#ifndef VUICVERIFICATIONSTATE_H
#define VUICVERIFICATIONSTATE_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS

class CVerificationDialog;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CVerificationState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CVerificationState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CVerificationState();
                                
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
        CVerificationState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
    private:    // Functions        
        
        /**
        * Handles keypress event
        * @return ETrue if event was handled else EFalse
        */
        TBool HandleKeypressL();
        
        /**
        * Plays prompt with name from recognition results with TTS
        */
        void PlayPromptL();      

        /**
        * Selects first tag from recognition results
        */
        void SelectTag();
        
        /**
        * Selects command by position id
        * @return Command id or KErrGeneral
        */
        TInt SelectCommandById();
   
    private:    // Data
    
        enum TInternalState
            {
            ENotStarted,
            EStarted,
            ETtsInitialized,
            ETtsCompleted,
            EStartCompleted,
            ERecordStarted,
            ERecordEnded,
            ECompleted
            };
    
        // Verification dialog
        CVerificationDialog*        iVerificationDialog;
        
        // Stores a keypress event from HandleEventL
        TInt                        iKeypress;
        
        // For handling internal state logics
        TInternalState              iInternalState;
        
        // Tag index
        TInt                        iTagIndex;
    };

#endif  // VUICVERIFICATIONSTATE_H

// End of File
