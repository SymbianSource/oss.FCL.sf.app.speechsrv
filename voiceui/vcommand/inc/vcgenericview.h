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
* Description:  View class for vcommand app
*
*/


#ifndef CVCGENERICVIEW_H
#define CVCGENERICVIEW_H

// INCLUDES
#include <aknview.h>
#include <vcommandapi.h>

#include "vcmodel.h"

// FORWARD DECLARATIONS
class CVCAppUi;
class CVCGenericContainer;
class CAknProgressDialog;
class CEikProgressInfo;
class CCoeEnv;


// CLASS DECLARATION

/**
 * CVCGenericView view class. 
 */
class CVCGenericView : public CAknView
    {

    public: // Constructors and destructor

        /**
         * Constructor.
         *
         * @param aService reference to VC API
         */
        CVCGenericView( CVCModel& aModel );

        /**
         * Symbian C++ default constructor.
         * @param aResource view resource id from the compiled resource file
         */
        void ConstructL( TInt aResourceId );

        /**
         * Destructor.
         */
        ~CVCGenericView();


    public: // Function from base classes
    
        /**
         * From CAknView
         *
         * @see CAknView for more information
         */
        void HandleCommandL( TInt aCommand );

        /** 
         * Handle screen size change.
         */
        void HandleSizeChange();
        
        /**
         * From CAknView
         * @see CAknView for more information
         */
        TUid Id() const;      


    public: // New functions
    
         /**
         * Handles basic commands like editing and playing items and opening views.
         * To make sure only one event is handled each time, HandleCommandL should be 
         * called instead.
         * 
         * @param aCommand command to handle
         */
        void DoHandleCommandL( TInt aCommand );
        
        /**
         * Returns the title of the folder where this view is active.
         *
         * @return TPtrC name of the item folder in listbox
         */
        const TDesC& FolderTitle() const;
        
         /**
         * Sets the middle softkey label. 
         * @param aIndex index in listbox
         */
        void SetMiddleSoftkeyLabelL( TInt aIndex ) const;
        
        /**
        * Refreshes the view to the current voice commands status
        */
        void RefreshViewL();
    
    
    private: // Functions from base classes

        /**
         * From CAknView
         * @see CAknView for more information
         */
        void HandleClientRectChange();

        /**
         * From MEikMenuObserver
         * This function defines how to show menupanel.
         *
         * @see MEikMenuObserver for more information
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * From CAknView
         *
         * @see CAknView for more information
         */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId, 
                                  const TDesC8& aCustomMessage );

        /**
         * From CAknView
         * @see CAknView for more information
         */
        void DoDeactivate();


    private: // New functions

        /**
		 * Displays cannot delete error note.
		 */
        void CannotDeleteNoteL();
        
        /**
         * Shows playback voice command progress dialog.
         */
        void PlaybackVoiceCommandL();
        
        /**
         * Launches demo VoiceUI.
         */
        void LaunchDemoVoiceUiL() const;

        /**
         * Sets the current folders title
         * 
         * @see CVCFolderInfo::Title
         * @param aNewName new name for folder
         */
        void SetFolderTitleL( const TDesC& aNewName );
        
        /**
         * Sets the middle softkey label.
         * @param aResourceId Label text from resource file
         * @param aCommandId Menu c
         */                          
        void DoSetMiddleSoftKeyLabelL( const TInt aResourceId,
                                       const TInt aCommandId  ) const;

        /**
         * Remove MSK command mappings.
         */                          
        void RemoveCommandFromMSK() const;
     
    private: // data        
    
        HBufC*                          iFolderName;
        CVCModel&                       iModel;
        CVCGenericContainer*            iContainer;
        // Array of voice commands
        CVCommandArray*                 iCommands;
        // The focused index in listbox. Used to set the correct focus when
        // returning from folder or settings view
        TInt                            iFocusedIndex;
            
    };

#endif  // CVCGENERICVIEW_H

// End of File

