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
* Description:  Application UI class for vcommand app
*
*/


#ifndef CVCAPPUI_H
#define CVCAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <AknWaitDialog.h>
#include <vcommandapi.h>
#include "vcmodelappuiinterface.h"

class CVCModel;
class CAppCloser;
class CVCGenericView;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
*/
class CVCAppUi : public CAknViewAppUi, public MVcModelAppUiInterface
    {
    public: // Constructors and destructor

        /**
        * Symbian C++ default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CVCAppUi();
        
    public: // Functions from base classes
    
        /**
        * From MVcModelAppUiInterface
        * Notifies the the command set has changed and the view may need to be
        * updated
        */
        virtual void CommandSetChanged();
        
        /**
        * Called before starting VCommandHandler operations that should be
        * executed fully before application can be exited. Leave safe operation.
        * A subsequent call to CleanupStack::PopAndDestroy should be done
        * after such operations have completed
        */
        virtual void StartAtomicOperationLC();
        
        /**
        * From CAknViewAppUi
        */
        TBool IsDisplayingMenuOrDialog();

    public: // New Functions

        /**
        * Sets titlepane text
        * @param aResourceId id for resource
        * @return void
        */
        void SetTitleL( TInt aResourceId );

        /**
        * Displays command too short note
        * @return void
        */
        void ShowErrorNoteL( TInt aResourceId );

        /**
        * Displays name already in use note
        * @return void
        */
        void NameAlreadyInUseNoteL( const TDesC& aName );
        
        /**
        * Displays wait note
        * @param aText text to be displayed
        * @return void
        */
        void ShowWaitNoteL( const TDesC& aText );
        
        /**
        * Deletes wait note
        * @return void
        */
        void DeleteWaitNoteL();

		/**
        * Checks if there's call in progress.
        * @return TInt ESACallNone if there's no active call
        */
        TInt CheckCallStateL();

	    /**
        * Displays call in progress note
        * @return void
        */
        void CallInProgressNoteL();
        
        /**
        *
        * @return TBool ETrue if there is a video call
        */
        TBool IsVideoCallL();
        
        /**
        * Exits the application correctly, taking into account whether a call
        * has been made to NotifyStartOfAtomicOperation
        * @return void
        */        
        void ExitProgram();
        
        /**
        * Exits the application
        */
        void DoExitProgram();
        
        /**
        * Calls CAppCloser::Call. CAppCloser::RunL will eventually exit the
        * application.
        */
        void ExitProgramDeferred();


    private: // Functions from base classes

        /**
        * From MEikMenuObserver
        * @see MEikMenuObserver for more information
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CEikAppUi
        * @see CEikAppUi for more information
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * From CEikAppUi
        * Handle skin change event.
        */
        void HandleResourceChangeL( TInt aType );
        
    private: // New functions
    
        /**
        * Runs the EndAtomicOperation when calling CleanupStack::PopAndDestroy after
        * a call to StartAtomicOperationLC
        */
        static void CleanupEndAtomicOperation( TAny* aService );
        
        /**
        * Creates and pushes to cleanup stack a TCleanupItem that calls
        * EndAtomicOperation when CleanupStack::PopAndDestroy is called
        */
        void CreateAndPushEndAtomicCleanupItemL();
        
        /**
        * Called after VCommandHandler operations that need to be operated
        * fully before application can be exited have been completed.
        */
        void EndAtomicOperation();

    
    private: // data
        
        // Wait dialog for command editing
        CAknWaitDialog*                 iWaitNote;
        
        // Indicates that an operation has to be finished before the
        // application can be exited
        TBool                           iOperationInProgress;
        
        // Indicates that an request for exiting the application has been made
        TBool                           iExitRequested;
      
        // Active object used to close the application
        CAppCloser*				        iCloser;
        
        friend class CAppCloser; 
        
        CVCGenericView*                 iMainView;
        
        CVCGenericView*                 iFolderView;
        
        CVCModel*                       iModel;
    };

#endif  // CVCAPPUI_H

// End of File


