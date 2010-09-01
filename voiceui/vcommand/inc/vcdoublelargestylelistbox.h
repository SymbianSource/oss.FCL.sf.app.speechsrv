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
* Description:  Listbox for commands
*
*/


#ifndef CVCDOUBLELARGESTYLELISTBOX_H
#define CVCDOUBLELARGESTYLELISTBOX_H

// INCLUDES
#include <aknlists.h>

class MEikScrollBarObserver;

// CLASS DECLARATION
/**
* 
*
*/
NONSHARABLE_CLASS( CVCDoubleLargeStyleListBox ) : public CAknDoubleLargeStyleListBox
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CVCDoubleLargeStyleListBox* NewL();

        /**
        * Destructor.
        */
        virtual ~CVCDoubleLargeStyleListBox();
        
        /**
         * Adds scroll bar observer
         *
         * @param aObserver Observer class
         */
        void AddScrollBarObserver( MEikScrollBarObserver* aObserver );
                                
    public:   // Functions from base classes
    
        /**
         * From CAknDoubleLargeStyleListBox
         * @see CAknDoubleLargeStyleListBox for more information
         */
        void HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );
                             
    private:  // New functions
        
        /**
        * C++ default constructor.
        */
        CVCDoubleLargeStyleListBox();
        
    private:  // Data
    
        MEikScrollBarObserver* iObserver;
};

#endif  // CVCDOUBLELARGESTYLELISTBOX_H

// End of File
