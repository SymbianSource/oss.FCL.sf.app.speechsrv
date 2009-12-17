/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUICAPPUI_H
#define VUICAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

#include "vuimvoicerecogdialogcallback.h"
#include "vuimcalldetectorobserver.h"
#include "vuimkeycallback.h"

// FORWARD DECLARATIONS
class CVoiceRecognitionDialogImpl;
class CCallDetector;
class CKeyGrabber;
class CFoldObserver;
class CNotificationObserver;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
*/
NONSHARABLE_CLASS( CAppUi ) : public CAknAppUi,
                              public MVoiceRecognitionDialogCallback,
                              public MCallDetectorObserver,
                              public MKeyCallback
    {
    public: // Constructors and destructor

        /**
        * Symbian C++ default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CAppUi();
        
    public: // Functions from base classes

        /**
        * From MVoiceRecognitionDialogCallback
        *
        * @see MVoiceRecognitionDialogCallback for more information
        */
        void DialogDismissed();
        
        /**
        * From MCallDetectorObserver
        *
        * @see MCallDetectorObserver for more information
        */
        void CallDetectedL();
        
        /**
        * From MKeyCallback
        *
        * @see MKeyCallback for more information
        */
        void HandleKeypressL( TInt aSoftkey );

    public: // New Functions

    private: // Functions from base classes

        /**
        * From CEikAppUi
        *
        * @see CEikAppUi for more information
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * From CEikAppUi
        *
        * @see CEikAppUi for more information
        */
        void PrepareToExit( );
        
    private: // New functions
        
        /**
         * Handles command line parameters.
         * @param aCommandLine CApaCommandLine object
         */
        TBool ProcessCommandParametersL( CApaCommandLine& aCommandLine );    
    
    private: // data
        
        // Recognition dialog
        CVoiceRecognitionDialogImpl*    iRecognitionDialog;
        
        // Call detector
        CCallDetector*                  iCallDetector;
        
        // Key grabber
        CKeyGrabber*                    iKeyGrabber;
        
        // Fold observer
        CFoldObserver*                  iFoldObserver;
        
        // Soft notification observer
        CNotificationObserver*          iNotificationObserver;
    };

#endif  // VUICAPPUI_H

// End of File


