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
* Description:  Base class for voice ui states
*
*/


#ifndef VUICSTATE_H
#define VUICSTATE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CDataStorage;
class CUiModel;

// CLASS DECLARATION
/**
* Abstract base class for different states
*
*/
NONSHARABLE_CLASS( CState ) : public CBase
{
    public:     // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CState();
                                
    public:     // New functions    
    
        /**
        * Handles received event
        * @param Event to be handled
        */
        virtual void HandleEventL( TInt aEvent ) = 0;
        
        /**
        * Executes the state functionality
        */
        virtual void ExecuteL() = 0;
         
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CState( CDataStorage& aDataStorage, CUiModel& aUiModel );
                
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
        /**
        * Returns the data storage
        * @return Reference to DataStorage
        */
        CDataStorage& DataStorage() const;
         
        /**
        * Returns the ui model
        * @return Reference to UiModel
        */     
        CUiModel& UiModel() const;
        
        /**
        * Starts progress timer
        * @param aEventHandler Object which will handle timed event
        * @param aDelay Delay before first timed event
        * @param aInterval Interval between timed events
        */
        void StartTimerL( CState& aEventHandler, TInt aDelay, TInt aInterval );
        
        /**
        * Deletes progress timer
        */
        void StopTimer();
       
        /**
        * Callback for timed event
        * @param aObject Contains this pointer
        * @return KErrNone
        */
        static TInt OnTimedEvent( TAny* aObject );
        
        /**
        * Handles timed event
        */
        virtual void DoTimedEventL();
        
        /**
        * Brings the application to foreground
        */
        void BringToForeground();
        
    private:    // Data

        // Data storage
        CDataStorage*     iDataStorage;
        
        // Data model
        CUiModel*         iUiModel;
        
        // Timer for timed events
        CPeriodic*        iTimer;
};

#endif  // VUICSTATE_H

// End of File
