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
* Description:  Document class
*
*/


#ifndef VCEXECUTORAPPDOCUMENT_H
#define VCEXECUTORAPPDOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CAiNameDialerDocument application class.
*/
class CVCExecutorAppDocument : public CAknDocument
    {
    public: // Constructors and destructor
    
        /**
        * Two-phased constructor.
        *
        * @param "CEikApplication& aApp" Application object
        */
        static CVCExecutorAppDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CVCExecutorAppDocument();

    public: // From CAknDocument
    
        /**
        * Updates window group task name
        *
        * @param "CApaWindowGroupName* aWgName" Window group name
        */
        void UpdateTaskNameL( CApaWindowGroupName* aWgName );

    private:

        /**
        * Default constructor.
        *
        * @param "CEikApplication& aApp" Application object
        */
        CVCExecutorAppDocument( CEikApplication& aApp );
        
        /**
        * Second phase constructor
        */        
        void ConstructL();

    private:

        /**
        * From CEikDocument, creates CAiNameDialerAppUi "App UI" object.
        *
        * @return Created AppUi instance
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // VCEXECUTORAPPDOCUMENT_H

// End of File
