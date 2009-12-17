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


#ifndef VUICCONTEXTCHECKSTATE_H
#define VUICCONTEXTCHECKSTATE_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS
class CNssVASDBMgr;
class MNssTagMgr;
class MNssContextMgr;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CContextCheckState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CContextCheckState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CContextCheckState();
                                
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
        CContextCheckState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();

    private:    // Functions
    
        /**
        * Checks if UI language has changed and sets the language to undefined if changed
        * @return ETrue if language has changed else EFalse
        */
        TBool LanguageChangedL();
                
    private:    // Data
    
        enum TInternalState
            {
            ENotStarted,
            EStarted,
            ECommandsReady,
            EDialReady,
            EVerificationReady
            };
    
        // Vas stuff for handling context operations
        CNssVASDBMgr*           iVasDbManager;
        MNssTagMgr*             iTagManager;
        MNssContextMgr*         iContextManager;
        
        // For handling internal state logics
        TInternalState          iInternalState;
        
        // Number of found voice tags
        TInt                    iTagCount;
};

#endif  // VUICCONTEXTCHECKSTATE_H

// End of File
