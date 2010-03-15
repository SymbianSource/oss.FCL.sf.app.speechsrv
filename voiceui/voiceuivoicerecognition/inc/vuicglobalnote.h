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
* Description:  Handle showing of global notes
*
*/


#ifndef VUICGLOBALNOTE_H
#define VUICGLOBALNOTE_H

// INCLUDES
#include <e32base.h>
#include <AknGlobalNote.h>

#include <secondarydisplay/vuisecondarydisplayapi.h>

#include "vuivoiceicondefs.h"

// CLASS DECLARATION
/**
* Handle showing of global notes
*/
NONSHARABLE_CLASS( CGlobalNote ) : public CBase
{
    public:     // Constructors and destructor
       
        /**
        * Symbian two-phased constructor.
        */
        static CGlobalNote* NewL();

        /**
        * Destructor.
        */
        virtual ~CGlobalNote();
        
    public:     // Enumerations
    
        enum TGlobalNoteState
            {
            ECallInProgress,
            ESystemError,
            ENoTag,
            ENoMatchPermanent,
            ENotRecognizedPermanent,
            ENoMemory
            };
                                
    public:     // New functions
    
        /**
        * Shows a message dialog
        * @param
        */
        void DisplayGlobalNoteL( TGlobalNoteState aState );
        
        /**
        * Shows a message dialog
        */
        void DismissGlobalNoteL();
        
    private:

        /**
        * C++ default constructor.
        */
        CGlobalNote();
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();
                
        /**
        * Displays global note
        * @param aNoteType Type of note
        * @param aMsgId Message ID
        * @param aDialog Cover ui dialog type
        * @param aIcon Icon id
        * @param aIconMask Icon mask id
        */
        void DisplayGlobalNoteL( TAknGlobalNoteType aNoteType, TInt aMsgId,
                                 SecondaryDisplay::TVUISecondaryDisplayDialogs aDialog,
                                 TMifAvkon aIcon = (TMifAvkon) 0, TMifAvkon aIconMask = (TMifAvkon) 0 );
        /**
        * Displays global note
        * @param aNoteType Type of note
        * @param aNoteText Text of the note
        * @param aDialog Cover ui dialog type
        * @param aIcon Icon id
        * @param aIconMask Icon mask id
        */
        void DisplayGlobalNoteL( TAknGlobalNoteType aNoteType, HBufC* aNoteText,
                                 SecondaryDisplay::TVUISecondaryDisplayDialogs aDialog,
                                 TMifAvkon aIcon, TMifAvkon aIconMask );
        
    private:    // Data

        // Note
        CAknGlobalNote*         iNote;
        
        // Id of the currently shown note
        TInt                    iNoteId;
};

#endif  // VUICGLOBALNOTE_H

// End of File
