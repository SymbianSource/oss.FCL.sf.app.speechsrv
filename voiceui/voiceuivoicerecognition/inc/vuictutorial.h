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
* Description:  Handles tutorial mode resources and dialog showing
*
*/


#ifndef VUICTUTORIAL_H
#define VUICTUTORIAL_H

// INCLUDES
#include "vuictutorialmessagedialog.h"


// CLASS DECLARATION
/**
* Handles tutorial mode resources and dialog showing
*/
NONSHARABLE_CLASS( CTutorial ) : public CBase
{
    public:     // Constructors and destructor
       
        /**
        * Symbian two-phased constructor.
        */
        static CTutorial* NewL();

        /**
        * Destructor.
        */
        virtual ~CTutorial();
        
    public:     // Enumerations
    
        enum TTutorialState
            {
            EIntroState,
            ECommandState,
            EDialState,
            EOtherState,
            ETryState,
            EBestState,
            EActivationState,
            EErrorState,
            EFinishedState            
            };
                                
    public:     // New functions
    
        /**
        * Creates a message dialog
        */
        void CreateMessageL();
        
        /**
        * Creates and shows a message dialog
        * @return Keycode of the pressed key
        */
        TInt CreateAndShowMessageL();
        
        /**
        * Shows a message dialog
        * @param aDeviceLockMode ETrue to set tutorial in device lock mode
        * @return Keycode of the pressed key
        */
        TInt ShowMessageL( TBool aDeviceLockMode = EFalse );
        
        /**
        * Destroys created message dialog
        */
        void DestroyMessage();
        
        /**
        * Returns the current state of the tutorial
        * @return One of the TTutorialState types
        */
        TTutorialState GetTutorialState();
        
        /**
        * Sets the next shown message to be an error message
        */
        void ErrorState();
                                
    private:

        /**
        * Check's if the tutorial mode is enabled
        * @return ETrue if tutorial mode is enabled else EFalse
        */
        TBool IsTutorialModeEnabled();
        
        /**
        * Sets the first time mode off
        */
        void DisableFirstTimeMode();

        /**
        * C++ default constructor.
        */
        CTutorial();
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();
                       
        /**
        * Loads tutorial and first time mode settings from CenRep
        */
        void LoadSettingsL();

        /**
        * Saves tutorial and first time mode settings to CenRep
        */
        void SaveSettingsL();
        
    private:    // Data
    
        // Actual tutorial mode
        TBool                       iTutorialMode;
        
        // First time tutorial mode
        TBool                       iFirstTimeMode;
        
        // For handling internal logics
        TTutorialState              iState;
        
        // Message dialog
        CTutorialMessageDialog*     iDlg;
};

#endif  // VUICTUTORIAL_H

// End of File
