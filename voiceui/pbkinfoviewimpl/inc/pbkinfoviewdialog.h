/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Pbk info view dialog.
*
*/


#ifndef PBKINFOVIEWDIALOG_H
#define PBKINFOVIEWDIALOG_H

//  INCLUDES
#include <e32base.h>
#include <eikdialg.h>
#include <eiklbo.h>
#include <aknlists.h>
#include <AknDialog.h>
#include <aknnavi.h>
#include <AknInfoPopupNoteController.h>

#include "pbkinfoviewdialogutil.h"
#include "pbkinfoviewreshandler.h"
#include "pbkinfoviewsindhandler.h"

// FORWARD DECLARATIONS
class TXspIconHelper;

// CLASS DECLARATION

/**
*  Pbk info view dialog.
*/
NONSHARABLE_CLASS (CPbkInfoViewDialog) : public CAknDialog, public MEikListBoxObserver
	{
    enum TInfoViewDialogIconIndex
		{
		EIconIndexPhone = 0,
		EIconIndexMobile,
	    EIconIndexVideo,
	    EIconIndexVoip,
	    EIconIndexEmail,
	    EIconIndexEmpty
		};	

    public:// Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPbkInfoViewDialog* NewL( TInt aContactId );
        
        /**
        * Destructor.
        */
        virtual ~CPbkInfoViewDialog();
        
    public:
    
        /**
        * Overrides CAknDialog::ExecuteLD. Checks whether the contact has any
        * voice tags. If not an info note is displayed. Otherwise calls
        * CAknDialog::ExecuteLD
        * @param aResourceId The resource ID of the dialog to be loaded
        * @return see CAknDialog::ExecuteLD
        */
        TInt ExecuteLD( TInt aResourceId );
        
        /** 
         * Responds to a change in focus.
         *
         * @param aDrawNow Contains the value that was passed to it by 
         *        @c SetFocus(). 
         */         
        void FocusChanged(TDrawNow aDrawNow); 
        
    private:

        /**
        * C++ default constructor.
        */
        CPbkInfoViewDialog( TInt aContactId );
	
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

	private:
	
	    /**
        * Called by framework before the dialog is shown.
        * @param None
		* @return None
        */
		void PostLayoutDynInitL();

        /**
        * Called by framework after the dialog is shown.
        * @param None
		* @return None
        */
		void PreLayoutDynInitL();

		/**
		* Handles menu events.
		* @param  aCommandId Command id.
		* @return None.
		*/
		void ProcessCommandL( TInt aCommandId );

        /**
        * From the base class.
		* Called by framework for key event handling.
        * @param aKeyEvent.
		* @param TEventCode.
		* @return Return code.
        */
		TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

        /**
        * From the base class.
		* Called by framework before exiting the dialog.
        * @param Button id.
		* @return ETrue to exit\ EFalse to not to exit.
        */
		TBool OkToExitL(TInt aButtonId);

		/**
        * From the base class.
		* Called by framework before menu is shown.
        * @param aResourceId Menu resource id.
		* @param aMenuPane Pointer to the menu.
        * @return None.
        */
		void DynInitMenuPaneL( TInt aResourceID,	CEikMenuPane* aMenuPane );

	   /**
		* From the base class.
        * Called when dialog is activated.
        * @return None.
        */
		void ActivateL();
		
		/**
		* Method to get context sensitive help topic.
        * @param aContext Help topic to open.
		* @return None.
        */
		void GetHelpContext( TCoeHelpContext& aContext ) const;
		
		/**
		* Called when display resolution changes.
        * @param aType.
		* @return None.
        */
		void HandleResourceChange( TInt aType );
		
		/**
		* Updates dialog cba.
		* @param aResourceId Resource id.
		* @return None.
		*/
		void UpdateCbaL( TInt aResourceId );
		
	    /**
        * Returns TInfoViewDialogIconIndex enumeration icon index.
		* @param aIconId enumeration TPbkIconId from PbkIconId.hrh.
		*        aTagIndex voice tag index
        * @return Icon index.
        */
		TInt IconIndex( TInt aIconId,TInt aTagIndex );
		
		/**
        * Creates listbox items.
        * @return None.
        */
		void CreateListBoxItemsL();
		
		/**
        * Shows popup for a voice command in the list.
        * @return None.
        */
		void ShowPopupL();

		/**
		* Gets current listbox item.
		* @param None.
		* @return Current listbox item index.
		*/
		TInt CurrentItemIndex();
		
		/**
	    * Displays an information note
	    * @param aResourceId Id for the resource text to be shown
	    */
        void ShowInformationNoteL( TInt aResourceId );

		/**
	    * Check if call is ongoing
	    */		
		TBool IsCallActive();
		
		/**
	    * Show "Call in progress" note
	    */		
		void CallInProgressNoteL();
		
		/**
	    * Create field icons
	    */			
		void CreateFieldIconsL(CArrayPtr<CGulIcon>* aIconArray);
		
	private: // from MEikListBoxObserver
        /**
        * From MEikListBoxObserver, called by framework.
        * @param aListBox.
        * @param aEventType.
		* @return None
        */
		void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
		
    private:

		// Id of the contact in contacts db
		TInt iContactId;
		
		// Contact is on sim or mmc card and has no voice tags
		TBool iVoiceTaglessContact;

		// Id of the dialog mode
		TInt iDialogMode;

		// List of voice commands, does not take ownership
		CAknDoubleGraphicStyleListBox* iListBox;
		
		// For title and icon handling
		CStatusPaneHandler* iStatusPaneHandler;
		
		// For hiding tabs, does not take ownership
		CAknNavigationControlContainer* iNaviPane;
		
		// Popup controller
		CAknInfoPopupNoteController* iPopupController;
		
		// Resource handler
		CPbkInfoViewResHandler* iResHandler;

		// Currently selected listbox item
		TInt iCurrentListBoxIndex;
		
		// SIND handler
		CPbkInfoViewSindHandler* iSindHandler;
		
		//XSp icon helper
		RArray<TXspIconHelper>   iXspIconHelper;
    };

#endif    // PBKINFOVIEWDIALOG_H
            
// End of File
