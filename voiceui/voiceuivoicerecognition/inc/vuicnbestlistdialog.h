/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class that shows the N-Best List
*
*/

#ifndef VUICNBESTLISTDIALOG_H
#define VUICNBESTLISTDIALOG_H

//  INCLUDES
#include <aknlists.h>
#include <aknlistquerydialog.h>
#include <AknMediatorObserver.h>

// FORWARD DECLARATIONS
class CAknIconArray;
class CListQueryDialog;
class MKeyCallback;

// CONSTANTS

// CLASS DECLARATION

/**
* The N-Best List Dialog
* @lib VoiceUiRecognition.lib
*/
NONSHARABLE_CLASS( CNBestListDialog ) : public CCoeControl,
                                        public MEikCommandObserver,
                                        public MAknDialogMediatorObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CNBestListDialog* NewL();

        /**
        * Destructor.
        */
        virtual ~CNBestListDialog();

    public: // New functions

        /**
        * Registers given object as a key listener
        */
        void RegisterForKeyCallback( MKeyCallback* aKeyCallback );
        
        /**
        * Creates N-Best List
        * @param aItems Items that will be shown in the list. Takes ownership of the list.
        * @param aHeader Optional header text for the list
        */
        void CreateNBestListPopupLC( CDesC16ArrayFlat* aItems,
                                     const TDesC& aHeader = KNullDesC );

        /**
        * Shows N-Best List
        * @return Key value that caused the dialog to be dismissed
        */
        TInt ShowNBestListPopupL();
        
        /**
        * Sets n-best list softkeys
        * @param aResourceId Softkey resource id
        */
        void SetSoftkeysL( TInt aResourceId );
        
        /**
        * Sets optional commands hidder or visible
        * @param aHidden ETrue if commands should be hidden else EFalse
        */
        void SetCommandsHidden( TBool aHidden );

        /**
        * Takes care of the headset short key presses. Replaces the list with new one
        * or moves down one item in the list.
        * @param aItems New items that will be shown in the list. Takes ownership of the list.
        * @return EFalse if the end of the list is passed else ETrue
        */
        TBool HandleShortKeypressL( CDesC16ArrayFlat* aItems );
        
        /**
        * Returns currently selected index
        * @return Index of current n-best list selection
        */
        TInt SelectedIndex();
        
    public:     // Functions from base classes
    
        /**
        * From CCoeControl Called when a key is pressed
        * @see CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * From MEikCommandObserver
        * @see MEikCommandObserver for more information.
        */
        void ProcessCommandL( TInt aCommandId );
        
        /**
        * From MAknDialogMediatorObserver
        * @see MAknDialogMediatorObserver for more information.
        */
        TAknDialogMediatorObserverCommand MediatorCommandL( TUid aDomain, TUid aCategory, 
                                                            TInt aCommandId,
                                                            const TDesC8& aData ); 

        /**
        * From MAknDialogMediatorObserver
        * @see MAknDialogMediatorObserver for more information.
        */
        void NotifyMediatorExit(); 

    private:    // New functions

        /**
        * Converts key event to one of keypress enumeration values
        * @param aKeyEvent Original key event
        * @return One of keypress enumeration values
        */
        TInt ConvertKeyEventL( const TKeyEvent& aKeyEvent );

        /**
        * Creates new icon array. Ownership is transferred to caller.
        * @return Icon array
        */
        CArrayPtr<CGulIcon>* CreateIconArrayL();

        /**
        * Load a possibly skinned icon (with mask) and append it to an 
        * icon array.
        * @param aArray    Icon array to add the icon to
        * @param aID       Skin ID for the icon
        * @param aBitmapId Id for the default icon in the MBM file
        * @param aMaskId   Id for the default icon mask in the MBM file
        */
        static void AppendIconToArrayL( CAknIconArray* aArray,
                                        const TAknsItemID& aID,
                                        TInt aBitmapId,
                                        TInt aMaskId );
        
        /**
        * Creates and sets n-best list data to secondary display
        * @param aItems New items that will be shown in the list. Takes ownership of the list.
        */                                
        void CreateAndSetSecondaryDisplayDataL( CDesC16ArrayFlat* aItems );
                
    private:    // Constructors

        /**
        * C++ default constructor.
        */
        CNBestListDialog();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //data
    
        // Dialog
        CListQueryDialog*                   iDialog;
        
        // Key observer
        MKeyCallback*                       iKeyCallback;
        
        // Index of currently selected item
        TInt                                iCurrentIndex;
        
        // Number of tags in the list
        TInt                                iTagCount;
                
        // Current CBA resource id
        TInt                                iCba;
    };

#endif  // VUICNBESTLISTDIALOG_H

// End of File