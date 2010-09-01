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
* Description:  Utility class for info view dialog.
*
*/


#ifndef PBKINFOVIEWDIALOGUTIL_H
#define PBKINFOVIEWDIALOGUTIL_H


//  INCLUDES
#include <e32base.h>
#include <akntabgrp.h>      // for AknTextUtils
#include <aknlistquerycontrol.h>
#include <AknQueryDialog.h>
#include <AknsUtils.h>      // for AknsUtils

// CONSTANTS
const TInt KBufSize = 128;


/**
* TDialogUtil
* 
* TDialogUtil contains general dialog utilities.
*/
NONSHARABLE_CLASS ( TDialogUtil )
	{
    public:
    
        /**
		* Constructs a list-box item for CAknDoubleLargeStyleListBox.
		* @param aFirstLine First line of the dialog item
        * @param aSecondLine Second line of the dialog item
        * @param aIconIndex Icon index
        * @return HBufC* Pointer to the newly created list box item.
		*/
        static HBufC* ConstructListBoxItemLC( const TDesC& aFirstLine,
                                              const TDesC& aSecondLine,
                                              TInt aIconIndex );
        
        /**
		* Constructs an independent CGulIcon object with fallback support.
		* If no matching item (aId) is found in the currently active skin,
		* attempts to construct the item using the given file (last 3 parameters).
		* @param aID Item ID of the masked bitmap to be created.
        * @param aFilename Filename to be used to construct the item, 
        * @param aFileBitmapId Id (for bitmap) in the file. 
        * @param aFileMaskId Id (for mask) in the file.
        * @return CGulIcon* Pointer to the newly created CGulIcon object.
        *         Ownership of the object is transferred to the caller.
		*/
		static CGulIcon* CreateIconL( TAknsItemID aId, const TDesC& aFileName,
		                              TInt aFileBitmapId, TInt aFileMaskId);
	};

/**
* CStatusPaneHandler
* 
* This class is used for changing dialog title and image.
*/
NONSHARABLE_CLASS (CStatusPaneHandler) : public CBase
	{
    public:
        /**
        * Two-phased constructor.
        */
		static CStatusPaneHandler* NewL(CAknAppUi* aAppUi);

        /**
        * Destructor.
        */
		virtual ~CStatusPaneHandler();

    private:
        /**
        * By default Symbian OS constructor is private.
        */
		void ConstructL();

        /**
        * C++ default constructor.
        */
		CStatusPaneHandler(CAknAppUi* aAppUi);

	public:
	    /**
		* Sets the member variable for previous title.
		* @param  None.
		* @return None.
		*/
        void StoreOriginalTitleL();
        
        /**
		* Sets the dialog title.
		* @param aResourceId Resource id from resource file.
		* @return None.
		*/
		void SetTitleL( TInt aResourceId );

		/**
		* Restores previous title when returning to previous view.
		* @param  None.
		* @return None.
		*/
		void RestorePreviousTitleL();

     private:	
		/**
		* Gets the dialog title.
		* @param aAppUi Pointer to UI class.
		* @param aText Title will be put to this variable.
		* @return TBool ETrue: success / EFalse: failure.
		*/
		static TBool GetPreviousTitleL( CAknAppUi* aAppUi, TDes& aText );
		
		/**
		* Restores previous title image when returning to previous view.
		* @param  None.
		* @return None.
		*/
		void RestoreOriginalImageL();

		/**
		* Sets dialog image.
		* @param aAppUi Pointer to UI class.
		* @param aIcon New image .
		* @return CEikImage* previous image.
		*/
		static CEikImage* SetImageL( CAknAppUi* aAppUi, CEikImage* aIcon );
		
		/**
		* Sets the dialog title.
		* @param aAppUi Pointer to the UI class.
		* @param aText TItle text.
		*/
		void static SetTitleL( CAknAppUi* aAppUi, const TDesC& aText );
		

	private:
		// Access to app ui
		CAknAppUi* iAppUi;

		// Original status pane title
		TBuf<KBufSize> iOriginalTitle;
		
		// Is original status pane title stored
		TBool iOriginalTitleStored;

		// Original context pane image
		CEikImage* iOriginalImage;
	};

#endif      // PBKINFOVIEWDIALOGUTIL_H
            
// End of File
