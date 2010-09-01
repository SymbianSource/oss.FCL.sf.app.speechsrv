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


#ifndef VUICVERIFICATIONTRAININGSTATE_H
#define VUICVERIFICATIONTRAININGSTATE_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS
class CNssVASDBMgr;
class MNssTagMgr;
class MNssContextMgr;
class MNssTag;
class CNssTrainingParameters;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CVerificationTrainingState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CVerificationTrainingState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CVerificationTrainingState();
                                
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
        CVerificationTrainingState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();

    private:    // Functions
    
        /**
        * Creates voice tags for verification commands
        */
        void CreateTagsL();
        
        /**
        * Constructs command training parameters
        */
        void ConstructTrainingParametersL();
        
        /**
        * Trains verification commands voice tags
        */
        void TrainTagsL();
    
        /**
        * Saves verification commands voice tags
        */
        void SaveTagsL();
        
        /**
        * Saves current UI language
        */
        void SaveLanguage();
                
    private:    // Data
    
        enum TInternalState
            {
            ENotStarted,
            EStarted,
            EContextDeleted,
            EContextReady,
            ECommandsTrained,
            ECommandsSaved
            };
    
        // Vas stuff for handling context operations
        CNssVASDBMgr*           iVasDbManager;
        MNssTagMgr*             iTagManager;
        MNssContextMgr*         iContextManager;
        
        // Training parameters
        CNssTrainingParameters* iTrainingParameters;
        
        // For handling internal state logics
        TInternalState          iInternalState;
        
        // Created voice tags
        RPointerArray<MNssTag>* iTags;
        
        // Number of expected callbacks
        TInt                    iExpectedCallbacks;
};

#endif  // VUICVERIFICATIONTRAININGSTATE_H

// End of File
