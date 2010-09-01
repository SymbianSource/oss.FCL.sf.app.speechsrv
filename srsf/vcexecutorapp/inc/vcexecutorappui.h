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
* Description:  vcexecutorapp application UI class
*
*/


#ifndef VCEXECUTORAPPUI_H
#define VCEXECUTORAPPUI_H

//  INCLUDES
#include <aknappui.h>

// CLASS DECLARATION

/**
* This class is a part of the standard application framework.
*/
class CVCExecutorAppUi : public CAknAppUi 
    {
    public: // Constructors and destructor
            
        /**
        * 2nd phase constructor.
        */
        void ConstructL();
	
        /**
        * Destructor.
        */
       virtual ~CVCExecutorAppUi();   
	    
    public: // New functions
    
        /**
        * Method to get the CEikonEnv pointer
        *
        * @return CEikonEnv
        */
        CEikonEnv* EikonEnv() const;
	    
        /**
        * Updates status pane to the idle pane
        */
        void UpdateStatusPaneL();

    private: // From CEikAppUi

        /**
        * Takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * Handles command line parameters.
        * @param aCommandLine CApaCommandLine object
        */
        TBool ProcessCommandParametersL( CApaCommandLine& aCommandLine );
        
        /**
        * From CEikAppUi, takes care of screen resolution change
        */
        void HandleScreenDeviceChangedL();

    private:
    
        // Resource id of the current status pane
        TInt iInitialStatuspaneResourceId;
    };

#endif // VCEXECUTORAPPUI_H
            
// End of File
