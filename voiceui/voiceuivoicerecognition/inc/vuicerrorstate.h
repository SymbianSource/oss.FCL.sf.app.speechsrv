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


#ifndef VUICERRORSTATE_H
#define VUICERRORSTATE_H

// INCLUDES
#include "vuicstate.h"

#include "vuicglobalnote.h"

// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CErrorState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CErrorState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel, TInt aError );

        /**
        * Destructor.
        */
        virtual ~CErrorState();
                                
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
        CErrorState( CDataStorage& aDataStorage, CUiModel& aUiModel, TInt aError );

        /**
        * Handles timed event
        */
        void DoTimedEventL();

    private:    // Functions
        
        /**
        * Returns which note should be shown
        * @return One of enumeration values
        */
        CGlobalNote::TGlobalNoteState NoteType();
        
        /**
        * Returns which tone should be played
        * @return KErrNone if no tone should be player, otherwise one of tone enumerations
        */
        TInt ToneType();
                
    private:    // Data
    
        enum TInternalState
            {
            ENotStarted,
            EStarted,
            EInitialized,
            EReady
            };
            
        // For handling internal state logics
        TInternalState      iInternalState;
    
        // Error value received in HandleEventL
        TInt                iError;
};

#endif  // VUICERRORSTATE_H

// End of File
