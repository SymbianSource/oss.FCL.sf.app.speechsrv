/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the global progress dialog class
*
*/


#ifndef VUICGLOBALPROGRESSDIALOG_H
#define VUICGLOBALPROGRESSDIALOG_H

//  INCLUDES
#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <AknsItemID.h>

#include <secondarydisplay/vuisecondarydisplayapi.h>

// FORWARD DECLARATIONS
class MKeyCallback;

// CLASS DECLARATION

/**
*  Displays progress dialog
*/
NONSHARABLE_CLASS( CGlobalProgressDialog ) : public CCoeControl,
                                             public MProgressDialogCallback
{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CGlobalProgressDialog* NewLC();
        
        /**
        * Two-phased constructor.
        */
        static CGlobalProgressDialog* NewL();

        /**
        * Destructor.
        */
        virtual ~CGlobalProgressDialog();

    public: // New functions
        
        /**
        * Sets the icon for this dialog
        * since 2.0
        * @param aIconFile File containing icon
        * @param aIconId Icon ID
        * @param aIconMaskId  Icon mask ID
        */
        void SetIconL( const TDesC& aIconFile, TInt aIconId = 0,
                       TInt aIconMaskId = -1 );
        /**
        * Sets the image for the dialog
        * since 2.0
        * @param aImageFile Image filename
        * @param aImageId Image ID
        * @param aImageMaskId Image mask ID
        */
        void SetImageL( const TDesC& aImageFile, TInt aImageId = 0,
                        TInt aImageMaskId = -1 );

        /**
        * Sets additional information to be sent to secondary display.
        * Must be called before sending data to notifier to have effect.
        *
        * @param aCategory Category Uid
        * @param aDialogIndex One of dialog indices
        * @param aData Data to be sent to cover UI
        */
        void SetSecondaryDisplayDataL( TUid aCategory, TInt aDialogIndex,
                                       const SecondaryDisplay::TMatchData* aData );

        /**
        * Shows the progress dialog
        * since 2.0
        * @param aPrompt Dialog prompt
        * @param aIconText Text of the icon
        * @param aSoftkeys Dialog soft keys
        */
        void ShowProgressDialogL( const TDesC& aPrompt, const TDesC& aIconText, TInt aSoftkeys = 0 );

        /**
        * Updates the progress dialog
        * since 2.0
        * @param aValue progress bar value
        * @param aFinalValue progress bar final value
        */
        void UpdateProgressDialogL( TInt aValue, TInt aFinalValue );

        /**
        * Register for softkey callback
        * since 2.0
        * @param aKeyCallback Pointer to callback client
        */
        void RegisterForKeyCallback( MKeyCallback* aKeyCallback );

        /**
        * Sets skin ids for image and icon
        * @param aImageId Image id
        * @param aImageBitmapId Fallback bitmap id for the image
        * @param aImageMaskId Fallback mask id for the image
        * @param aIconId Icon id
        */
        void SetSkinIds( TAknsItemID aImageId, TInt aImageBitmapId,
                         TInt aImageMaskId, TAknsItemID aIconId );
      
    public:     // Functions from base classes                                

        /**
        * From MProgressDialogCallback Handles keypresses and closing of the dialog 
        * @see MProgressDialogCallback
        */
        void DialogDismissedL( TInt aButtonId );

    private:

        /**
        * By default Symbian constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CGlobalProgressDialog();
        
        /**
        * Called to complete the global progress dialog
        * since 2.0
        */
        void ProcessFinished();

    private:    // Data

        enum TInternalState
            {
            EStarted,
            ERunning,
            EFinished
            };
            
        // Progress dialog
        CAknProgressDialog*         iProgressDialog;
        
        // Key observer
        MKeyCallback*               iKeyCallback;
        
        // For handling internal logics
        TInternalState              iInternalState;
};

#endif  // VUICGLOBALPROGRESSDIALOG_H

// End of File
