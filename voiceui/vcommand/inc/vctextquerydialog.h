/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Text query dialog for editing commands
*
*/


#ifndef CVCTEXTQUERYDIALOG_H
#define CVCTEXTQUERYDIALOG_H

// INCLUDES
#include <AknQueryDialog.h>

// CLASS DECLARATION
/**
* 
*
*/
NONSHARABLE_CLASS( CVCTextQueryDialog ) : public CAknTextQueryDialog
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CVCTextQueryDialog* NewL( TDes& aDataText, const TTone& aTone = ENoTone );

        /**
        * Destructor.
        */
        virtual ~CVCTextQueryDialog();
                                
    public:   // Functions from base classes
    
        /**
         * From CAknTextQueryDialog
         * @see CAknTextQueryDialog for more information
         */
        virtual TBool CheckIfEntryTextOk() const;
        
        /**
         * From CAknTextQueryDialog
         * @see CAknTextQueryDialog for more information
         */
        virtual void UpdateLeftSoftKeyL();
                             
    private:  // New functions
        
        /**
        * C++ default constructor.
        */
        CVCTextQueryDialog( TDes& aDataText, const TTone& aTone );
};

#endif  // CVCTEXTQUERYDIALOG_H

// End of File
