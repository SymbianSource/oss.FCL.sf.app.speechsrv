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
* Description:  Application class of VCommandManager
*
*/


#ifndef VCMANAGERAPP_H
#define VCMANAGERAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS

// UID of the application
const TUid KUidVcManager = { 0x102818E7 };


// CLASS DECLARATION

/**
* CVCommandManagerApp application class.
* Provides factory to create concrete document object.
* 
*/
class CVCommandManagerApp : public CAknApplication
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

#endif // VCMANAGERAPP_H

// End of file
