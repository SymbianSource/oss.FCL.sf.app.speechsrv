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
* Description:  
*
*/


#ifndef VUICLISTQUERYDIALOG_H
#define VUICLISTQUERYDIALOG_H

// INCLUDES
#include <aknlistquerydialog.h>

// CLASS DECLARATION
/**
* 
*
*/
NONSHARABLE_CLASS( CListQueryDialog ) : public CAknListQueryDialog,
										public MEikScrollBarObserver
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CListQueryDialog* NewL( TInt *aIndex,
                                       CListQueryDialog** aSelf,
                                       MEikCommandObserver* aObserver = NULL,
                                       TInt aMenuTitleResourceId = NULL );

        /**
        * Destructor.
        */
        virtual ~CListQueryDialog();
                                
    public:  // New functions
    
        /**
        * Checks if menu is currently visible
        * @return ETrue if menu is visible
        */
        TBool IsMenuVisible();
        
        /**
        * Sets optional menu commands hidden or visible
        * @param aHidden ETrue if commands should be hidden else EFalse
        */
        void SetCommandVisibility( TBool aHidden );
    
    protected:  // Functions from base classes
    
        /**
        * From CAknListQueryDialog
        * @see CAknListQueryDialog for more information.
        */
        void PostLayoutDynInitL();
        
        /**
        * From CAknListQueryDialog
        * @see CAknListQueryDialog for more information.
        */
        TBool OkToExitL( TInt aButtonId );
        
        /**
        * From CAknListQueryDialog
        * @see CAknListQueryDialog for more information.
        */
        void ProcessCommandL( TInt aCommandId );
        
        /**
        * From CAknListQueryDialog
        * @see CAknListQueryDialog for more information.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * From CAknListQueryDialog
        * @see CAknListQueryDialog for more information.
        */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
        
        /**
        * From MEikScrollBarObserver
        * @see MEikScrollBarObserver for more information.
        */
        void HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );
                         
    private:  // New functions
        
        /**
        * C++ default constructor.
        */
        CListQueryDialog( MEikCommandObserver* aObserver,
                          TInt aMenuTitleResourceId,
                          TInt *aIndex,
                          CListQueryDialog** aSelf );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
    private:    // Data
        
        // State of optionally visible menu commands
        TBool                   iCommandsHidden;
        
        // Menu command observer
        MEikCommandObserver*    iObserver;
        
        // Options menu resource id
        TInt                    iMenuResourceId;
        
        // Address of self pointer
        CListQueryDialog**      iSelf;
};

#endif  // VUICLISTQUERYDIALOG_H

// End of File
