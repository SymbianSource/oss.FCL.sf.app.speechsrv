/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles the dialog used in tutorial mode
*
*/


#ifndef VUICTUTORIALMESSAGEDIALOG_H
#define VUICTUTORIALMESSAGEDIALOG_H

// INCLUDES
#include <coecntrl.h>

#include <secondarydisplay/vuisecondarydisplayapi.h>

// FORWARD DECLARATIONS
class CMessageQueryDialog;

// CLASS DECLARATION
/**
* Handles the dialog used in tutorial mode
*/
NONSHARABLE_CLASS( CTutorialMessageDialog ) : public CCoeControl
{
    public:     // Constructors and destructor
        
        /**
        * Symbian two-phased constructor.
        */
        static CTutorialMessageDialog* NewL();
        
        /**
        * Symbian two-phased constructor.
        */
        static CTutorialMessageDialog* NewLC();

        /**
        * Destructor.
        */
        virtual ~CTutorialMessageDialog();
        
        
    public:     // New functions
    
        /**
        * Shows a message dialog
        * @param aDialogResourceId  Resource id of the dialog
        * @param aHeaderId          Resource id of header text
        * @param aMessagePart1Id    Resource id of message text part 1
        * @param aMessagePart2Id    Resource id of message text part 2
        * @param aCommandResourceId Resource id of the softkeys
        * @param aDialog            Secondary display dialog id
        * @return Keycode of the pressed key
        */
        TInt ShowMessageDialogL( TInt aDialogResourceId,
                                 TInt aHeaderId,
                                 TInt aMessagePart1Id,
                                 TInt aMessagePart2Id,
                                 TInt aCommandResourceId,
                                 SecondaryDisplay::TVUISecondaryDisplayDialogs aDialog =
                                    SecondaryDisplay::ECmdVoiceNoNote );
                                
    public:     // Functions from base classes                               
        
    private:

        /**
        * C++ default constructor.
        */
        CTutorialMessageDialog();
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Concatenates given strings and inserts a space between the strings
        * @param aString1 First string
        * @param aString2 Second string
        * @return Concatenated string
        */
        HBufC* ConcatenateLC( const TDesC& aString1, const TDesC& aString2 );
        
    private:    // Data
        
        // Message dialog
        CMessageQueryDialog*        iDlg;
        
        // Current command set
        TInt                        iCommands;
};

#endif  // VUICTUTORIALMESSAGEDIALOG_H

// End of File
