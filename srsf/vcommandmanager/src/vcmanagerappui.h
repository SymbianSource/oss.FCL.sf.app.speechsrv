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
* Description:  AppUi class of VCommandManager
*
*/


#ifndef VCMANAGERAPPUI_H
#define VCMANAGERAPPUI_H

//  INCLUDES
#include <aknappui.h>
#include <nssvasmtagselecthandler.h>
#include <nssvasmcontext.h>
#include <nssvascselectnotificationbuilder.h>
#include <nssvasmtagselectnotification.h>
#include <nssvasmcontextmgr.h>

// FORWARD DECLARATIONS
class CNssVoiceUIPluginHandler;
class CVCRecognitionManager;
class CNssContactHandler;
class CVcManagerContactSelectHandler;
class CNssVASDBMgr;
class CVCommandManagerBackupObserver;
class CVCommandManagerVoiceKeyLauncher;
class CVCommandManagerVoiceHeadSetLauncher;

// CLASS DECLARATION

/**
* This class is a part of the standard application framework.
*/
class CVCommandManagerAppUi : public CAknAppUi 
    {
    public: // Constructors and destructor
            
        /**
        * 2nd phase constructor.
        */
        void ConstructL();
	
        /**
        * Destructor.
        */
       virtual ~CVCommandManagerAppUi();   
	    
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
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );
                
        /**
        * From CEikAppUi, takes care of screen resolution change
        */
        void HandleScreenDeviceChangedL();
        
    private: // New functions
    
        /**
        * Creates SIND services
        */
        void StartServicesL();
        
    private:
    
        // Resource id of the current status pane
        TInt iInitialStatuspaneResourceId;
        
        // VUIPF plugin handler
        CNssVoiceUIPluginHandler* iPluginHandler;
        
        // Recognition result handler
        CVCRecognitionManager* iRecogManager;
        
        // VAS contact handler
        CNssContactHandler* iContactHandler;
        
        // Backup/restore observer
        CVCommandManagerBackupObserver* iBackupObserver;
        
        // Starting voice ui
        CVCommandManagerVoiceKeyLauncher* iVoiceKeyLauncher;
        CVCommandManagerVoiceHeadSetLauncher* iVoiceHeadSetLauncher;
        
        // For dialing
        MNssTagSelectNotification* iTagSelectNotification;
        CNssSelectNotificationBuilder* iSelectNotificationBuilder;
        CVcManagerContactSelectHandler* iContactSelectHandler;
        CNssVASDBMgr* iVasDbManager;
        MNssContextMgr* iContextManager;
    };

#endif // VCMANAGERAPPUI_H 
            
// End of File
