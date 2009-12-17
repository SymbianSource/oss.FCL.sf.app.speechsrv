/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUICRECORDSTATE_H
#define VUICRECORDSTATE_H

// INCLUDES
#include "vuicstate.h"

#include <vuivoicerecogdefs.h>

// FORWARD DECLARATIONS
class CGlobalProgressDialog;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CRecordState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CRecordState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CRecordState();
                                
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
        CRecordState( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Handles timed event
        */
        void DoTimedEventL();

    private:    // Functions
                
    private:    // Data
    
        enum TInternalState
            {
            ENotStarted,
            EStarted,
            ERecording,
            EStopped,
            EFinished
            };

        // Record progressdialog
        CGlobalProgressDialog*          iProgressDialog;
        
        // For handling internal state logics
        TInternalState                  iInternalState;
        
        // Used to count timer ticks
        TInt                            iTickCount;
        
        // Timer data model
        VTimerModel                     iTModel;
};

#endif  // VUICRECORDSTATE_H

// End of File
