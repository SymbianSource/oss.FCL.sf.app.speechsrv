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
* Description:  Application class
*
*/


#ifndef VCEXECUTORAPP_H
#define VCEXECUTORAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS

// UID of the application
const TUid KUidVcExecutorApp = { 0x10281D15 };


// CLASS DECLARATION

/**
* CVCExecutorApp application class.
* Provides factory to create concrete document object.
* 
*/
class CVCExecutorApp : public CAknApplication
    {
    
    private: // Functions from base classes

        /**
        * From CApaApplication
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID.
        * @return The value of KUidVcManager.
        */
        TUid AppDllUid() const;
    };

#endif // VCEXECUTORAPP_H

// End of file
