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


#ifndef VUICPLAYSTATE_H
#define VUICPLAYSTATE_H

// INCLUDES
#include <AknsItemID.h>

#include <cntdef.h> 

#include <nssttsutility.h>

#include <vuivoicerecogdefs.h>

#include "vuicstate.h"

// FORWARD DECLARATIONS
class CGlobalProgressDialog;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CPlayState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CPlayState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CPlayState();
                                
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
        CPlayState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
        /**
        * Handles timed event
        */
        void DoTimedEventL();

    private:    // Functions
    
        /**
        * Formats given name to fit one line
        * @param aName Name to be formatted
        */
        void FormatName( TDes& aName );
    
        /**
        * Plays voice tag
        */
        void PlayVoiceTagL();
        
        /**
        * Fetches and formats data that will be shown while playing
        * @param aName Name to be used
        * @param aPhoneNumber Phone number to be used
        * @param aFieldType Field type to be used
        */
        void FormatDataL( TDes& aName, TDes& aPhoneNumber, TFieldType& aFieldType );
        
        /**
        * Shows the play dialog
        * @param aName Name that will be shown
        * @param aPhoneNumber Phone number that will be shown
        * @param aFieldType Type of the shown field
        */
        void ShowPlaydialogL( const TDesC& aName, const TDesC& aPhoneNumber,
                              const TFieldType aFieldType );
    
        /**
        * Determines icon information according to the given type
        * @param aType Type to be used
        * @param aIcon Selected icon
        * @param aIconMask Selected icon mask
        * @param aIconId Selected icon id
        */
        void GetDialogIcon( const TFieldType& aType, TInt& aIcon,
                            TInt& aIconMask, TAknsItemID& aIconId );
                 
    private:    // Data

        enum TInternalState
            {
            ENotStarted,
            EStarted,
            EInitialized,
            ECompleted
            };
        
        // Dialog
        CGlobalProgressDialog*          iProgressDialog;
        
        // For handling internal state logics
        TInternalState                  iInternalState;
        
        // Used to count timer ticks
        TInt                            iTickCount;
        
        // Timer data model
        VTimerModel                     iTModel;
        
        // Playback length
        TInt                            iPlaybackDelay;
};

#endif  // VUICPLAYSTATE_H

// End of File
