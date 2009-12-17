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
* Description:  
*
*/


#ifndef VUICDOCUMENT_H
#define VUICDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
* CDocument application class.
*/
NONSHARABLE_CLASS( CDocument ) : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CDocument();
        
    private: // New Functions

        /**
        * C++ default constructor.
        * @param aApp reference to application object.
        */
        CDocument( CEikApplication& aApp );
        
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * From CEikDocument
        * £see CEikDocument for more information
        */
        CEikAppUi* CreateAppUiL();
    };

#endif  // VUICDOCUMENT_H

// End of File

