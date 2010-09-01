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
* Description:  Active object to handle global list query results.
*
*/

#ifndef CAPPCONTROLLERACTIVEOBJECT_H
#define CAPPCONTROLLERACTIVEOBJECT_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CVCApplicationControllerImpl;

/**
*  Active object to handle note results.
*/
class CAppControllerActiveObject : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        * 
        */
        CAppControllerActiveObject(CVCApplicationControllerImpl& aController);
     
        /**
        * Destructor.
        */
        virtual ~CAppControllerActiveObject();


    public: // Functions from base classes

        /**
        * @see CActive::DoCancel.
        */
        void DoCancel();

        /**
        * @see CActive::RunL.
        */
        void RunL();

        /**
        * @see CActive::RunError.
        */
        TInt RunError( TInt aError );

        /**
        * @see CActive::SetActive.
        */
        void SetActive();

	private: // Data
		CVCApplicationControllerImpl& iController;
    };

#endif      // CAPPCONTROLLERACTIVEOBJECT_H 
            
// End of File