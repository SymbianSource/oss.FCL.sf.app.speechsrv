/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Declares UI class for application.
*
*/


#ifndef TESTAPPAPPUI_H
#define TESTAPPAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CTestAppContainer;
class CTestAppEngine;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/
class CTestAppAppUi : public CAknAppUi
    {
    public: // // Constructors and destructor

        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CTestAppAppUi();
        
        void ChangeTextL( TInt aIndex,  TPtrC aText, TInt aNumber = -1 );
        
        void ShowNoteL( TPtrC aText, TInt aNumber = -1 );
        
    private:
    
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
    
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);       
        
    private: //Data
        
        CTestAppContainer*  iAppContainer; 
        
        CTestAppEngine*     iAppEngine;
        
        TBool               iServerConnected;
        
    };

#endif

// End of File
