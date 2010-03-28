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
* Description:  Container class for vcommand app
*
*/



#ifndef CVCGENERICCONTAINER_H
#define CVCGENERICCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <coedef.h> 
#include <bamdesca.h> 
#include <barsread.h>
#include <badesca.h>
#include <aknlists.h>
#include <AknIconArray.h>
#include <eiksbobs.h> 
#include <AknWaitDialog.h>
#include <AknInfoPopupNoteController.h>
#include <data_caging_path_literals.hrh>
#include <csxhelp/vc.hlp.hrh>

#include "vcgenericview.h"
#include "vcommandconstants.h"
#include "vcmodel.h"


// CONSTANTS

// FORWARD DECLARATIONS
class CEikTextListBox;
class CEikFormattedCellListBox;

// CLASS DECLARATION

/**
* CVCommandGenericContainer container control class.
*/
class CVCGenericContainer : public CCoeControl,
                            public MCoeControlObserver,
                            public MEikListBoxObserver,
                            public MEikScrollBarObserver
    {

    public: // Constructors and destructor
        
        /**
         * C++ constructor.
         *
         * @param aMenubar a menubar
         * @param aView a pointer to view class
         */
        CVCGenericContainer( CEikMenuBar* aMenuBar,
                             CVCModel& aModel, 
                             CVCGenericView& aView );

        /**
         * Symbian default constructor.
         *
         * @param aRect Frame rectangle for container.
         */
        void ConstructL( const TRect& aRect );

        /**
         * Destructor.
         */
        ~CVCGenericContainer();
        
    public: // Functions from base classes
    
        /**
         * From CoeControl
         *
         * @see CCoeControl for more information
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;


    public: // New functions

        /**
         * Returns the focused item on listbox
         *
         * @return TInt Id of the focused item
         */
        TInt CurrentSelection() const;
        
        /**
         * Sets the focused item on listbox
         *
         * @return void
         */
        void SetCurrentSelection( TInt aIndex );
    
        /**
         *
         * @return TInt the number of items in the list box
         */
        TInt ListBoxCount() const;
    
        /**
         * Returns the first line name.
         *
         * @param aIndex list item index
         * @return const TDesC& first row name
         */
        TDesC* GetFirstRowNameLC( TInt aIndex ) const;
        
        /**
         * Returns the second line name.
         *
         * @param aIndex list item index
         * @return const TDesC& second row name
         */
        TDesC* GetSecondRowNameLC( TInt aIndex ) const;
        
        /**
         * Returns a title that is to be shown in folder view.
         *
         * @param aIndex list item index
         * @return TPtrC folder title name
         */
        TPtrC GetFolderTitle( TInt aIndex );
        
        /**
         * Returns true if pointed listbox item is a folder.
         *
         * @param aListBoxIndex index of listbox item
         * @return ETrue if item is a folder
         */
        TBool IsItemFolderL( TInt aListBoxIndex ) const;
        
        /**
         * Shows the Edit command text query.
         *
         * @return void
         */
        void ShowEditTextL();
        
        /**
         * Shows delete application query
         * and handles the voice tag deletion
         *
         * @return void
         */
        void DeleteVCommandL();
        
        /**
         * Returns ETrue if event is under processing at the moment.
         *
         * @return ETrue if event handling process is running
         */
        TBool IsHandlingKeyPressInProgress();
        
        /**
         * Sets event handling state.
         *
         * @param aValue new status for event handling process
         * @return void
         */
        void SetHandlingKeyPressInProgress( TBool aValue );
        
        void RefreshListboxL( CVCommandUiEntryArray& oldCommands,
                              CVCommandUiEntryArray& newCommands );
                              
        /**
         * Shows tooltip text for item.
         *
         * @param aItemIndex index of the listbox item
         */
        void ShowTooltipL( TInt aItemIndex );


    protected: // Functions from base classes
    
        /**
         * From MEikListBoxObserver
         * @see MEikListBoxObserver for more information
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
        
        /**
         * From MEikScrollBarObserver
         * @see MEikScrollBarObserver for more information
         */
        void HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );

        /**
       	 * From CCoeControl.
       	 *
       	 * @see CCoeControl for more information
       	 */
        void FocusChanged( TDrawNow aDrawNow );
        
        /**
         * From CCoeControl.
         *
         * @see CCoeControl for more information
         */
        void SizeChanged();
     
        /**
         * From CoeControl.
         *
         * @see CCoeControl for more information
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl.
         *
         * @see CCoeControl for more information
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * From MCoeControlObserver.
         *
         * @see MCoeControlObserver for more information
         */
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
        
        /**
         * From CCoeControl
         *
         * @see CCoeControl for more information
         */
        void HandleResourceChangeL( TInt aType );      
        
        /**        
         * From CCoeControl
         *
         * @see CCoeControl for more information
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
    private: // New functions

        /**
         * Constructs status pane
         *
         * @param aResourceId
         */
        void SetTitlePaneL( TInt aResourceId ) const;
        
        /**
         * Constructs the listbox.
         */
        void ConstructListBoxL();

        /**
         * Initializes the listbox. 
         */
        void PopulateListBoxL();
        
        /**
         * Checks if the user edited name is valid.
         * 
         * @param aName name to check
         * @return TBool ETrue if name exists or is too short.
         */
        TBool CheckNameValidityL( const TDesC& aName );
        
        /**
         * Sets the new text to the list.
         * 
         * @param aIndex The index of an application
         * @param aNewText the text that replaces the old one
         * @param aOldText the old text
         */
        void UpdateTextL( TInt aIndex, const TDesC& aNewText,
                          const TDesC& aOldText );
        
        /**
         * Sorts the contents of the iItemNameArray to an alphabetical order.
         */
        void SortItemListL();
        
        /**
        * Finds the commands that should be added to the listbox
        * @param oldCommands A list of commands before an update
        * @param newCommands A list of commands after an update
        * @return RArray<CVCommand> Commands that need to be removed from the
        *         listbox
        */
        RArray<CVCommandUiEntry> FindCommandsToBeAddedL(
            CVCommandUiEntryArray& oldCommands, CVCommandUiEntryArray& newCommands );
                        
        /**
        * Adds items to the listbox.
        * @param aCommands Commands to be added
        */
        void AddCommandsL( RArray<CVCommandUiEntry>& aCommands );
        
        /**
        * Finds the commands that should be removed from the listbox
        * @param oldCommands A list of commands before an update
        * @param newCommands A list of commands after an update
        * @return RArray<CVCommand> Commands that need to be added to the
        *         listbox
        */
        RArray<CVCommandUiEntry> FindCommandsToBeRemovedL(
            CVCommandUiEntryArray& oldCommands, CVCommandUiEntryArray& newCommands );
            
        /**
        * Removes items from listbox
        * @param aCommands Commands to be removed
        */
        void RemoveCommands( RArray<CVCommandUiEntry>& aCommands );
        
        /**
        * Searches for a string in listbox item array.
        * @param aText The search string
        * @return TInt Index in the array where the string was found.
        *         KErrNotFound if the string was not found.
        */
        TInt SearchItemArrayL( const TDesC& aText );
        
        /**
        * Utility function for listbox item creation
        * @param aIconArrayIndex Icon array index for this item
        * @param aFirstRow First row for this item
        * @param aSecondRow Second row for this item
        * @return TDesC* A new listbox item
        */
        TDesC* CreateListboxItemLC( TInt aIconArrayIndex,
                                    const TDesC& aFirstRow,
                                    const TDesC& aSecondRow );

        /**
        * Checks for critical level flash and shows "no memory" note if needed
        *
        * @return ETrue if CL would be reached after this operation.
        */
        TBool CheckCriticalLevelL();

    private: // data
    
        CEikMenuBar*                    iMenuBar;           // Not owned
        CVCGenericView&                 iView;
        
        CVCModel&                       iModel;
        
        //CEikTextListBox*                iListBox;           // Owned
        CEikFormattedCellListBox*       iListBox;           // Owned
        
        // Displayed names for the items in listbox
        CDesC16ArrayFlat*               iItemArray;     // Not owned
        
        // Titles for different folders in folder view
        CDesC16ArrayFlat*               iFolderTitles;
        
        // Corresponds index by index with the items in iItemNameArray.
        // Stores information on whether an item is a folder
        RArray<TBool>                   iItemIsFolder;
        
        CAknInfoPopupNoteController*    iPopupController;   // Owned
        
        TBool                           iHandlingKeyPressInProgress;
        
    };

#endif // CVCGENERICCONTAINER_H

// End of File

