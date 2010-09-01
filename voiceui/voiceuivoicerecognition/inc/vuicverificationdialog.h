/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the voice verification dialog
*
*/

#ifndef VUICVERIFICATIONDIALOG_H
#define VUICVERIFICATIONDIALOG_H

//  INCLUDES
#include <aknlists.h>
#include <aknlistquerydialog.h>
#include <AknMediatorObserver.h>

#include <secondarydisplay/vuisecondarydisplayapi.h>

// FORWARD DECLARATIONS
class CListQueryDialog;
class MKeyCallback;

// CONSTANTS

// CLASS DECLARATION

/**
* Voice verification dialog
*/
NONSHARABLE_CLASS( CVerificationDialog ) : public CCoeControl,
                                           public MAknDialogMediatorObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVerificationDialog* NewL();

        /**
        * Destructor.
        */
        virtual ~CVerificationDialog();

    public: // New functions

        /**
        * Registers given object as a key listener
        */
        void RegisterForKeyCallback( MKeyCallback* aKeyCallback );
        
        /**
        * Creates verification query
        * @param aHeader Optional header text for the list
        */
        void CreateVerificationPopupLC( const TDesC& aHeader );

        /**
        * Shows verification query
        * @return Key value that caused the dialog to be dismissed
        */
        TInt ShowVerificationPopupL();
                       
        /**
        * Returns selected command based on the selection id.
        * @param aId Selection id
        * @return Command id or KErrGeneral
        */
        TInt SelectedCommand( TInt aId );
        
    public:     // Functions from base classes
    
        /**
        * From CCoeControl Called when a key is pressed
        * @see CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
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
        * Sets data for secondary display
        * @param aData Data to be send to secondary display
        */
        void SetSecondaryDisplayDataL( SecondaryDisplay::TVerificationData& aData );
                
    private:    // Constructors

        /**
        * C++ default constructor.
        */
        CVerificationDialog();

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
    };

#endif  // VUICVERIFICATIONDIALOG_H

// End of File