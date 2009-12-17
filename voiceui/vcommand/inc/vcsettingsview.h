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
* Description:  Settings view class
*
*/


#ifndef CVCSETTINGSVIEW_H
#define CVCSETTINGSVIEW_H

// INCLUDES
#include <aknview.h>
#include <eiklbo.h>

// FORWARD DECLARATIONS
class CVCSettingsContainer;


// CLASS DECLARATION

/**
* Settings view class.
*/
class CVCSettingsView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CVCSettingsView();

    public: // Functions from base classes

        /**
        * From CAknView Return UID of view
        * @return TUid uid of the view
        */
        TUid Id() const;

        /**
        * From CAknView, handle commands
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From MEikMenuObserver Dynamically customize menu items
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CAknView
        * @see CAknView for more information
        */
        void HandleClientRectChange();
        
    private: // Functions from base classes

        /**
        * From CAknView Activate this view
        * @param aPrevViewId 
        * @param aCustomMessageId 
        * @param aCustomMessage 
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView Deactivate this view
        */
        void DoDeactivate();

    private: // Data
    
        CVCSettingsContainer*	iContainer; // owned
        TInt                    iListBoxSelectedItemIndex;
        TVwsViewId              iPreviousView;
        TBool                   iOperationInProgress;
        
    };

#endif	// CVCSETTINGSVIEW_H

// End of File
