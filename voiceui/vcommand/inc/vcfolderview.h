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
* Description:  View class for folders
*
*/


#ifndef CVCFOLDERVIEW_H
#define CVCFOLDERVIEW_H

// INCLUDES
#include "vcgenericview.h"
#include "vcmodel.h"

// CLASS DECLARATION

/**
 * CVCFolderView view class. Inherited from a base class CVCGenericView.
 */
class CVCFolderView : public CVCGenericView 
    {
    public: // Constructors and destructor

        /**
         * Constructor.
         *
         * @param aService reference to VC API
         */
        CVCFolderView( /*CVCommandHandler& aService,*/
                       CVCModel& aModel );

    public: // Functions from base classes

        /**
         * From CAknView
         * Returns folder view's id.
         * @see CAknView for more information
         */
        TUid Id() const;
    };

#endif  // CVCFOLDERVIEW_H

// End of File

