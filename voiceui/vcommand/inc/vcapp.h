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
* Description:  Application class for vcommand app
*
*/


#ifndef CVCDAPP_H
#define CVCDAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidVCommand = { 0x101F8555 };

// CLASS DECLARATION

/**
* CVCApp application class.
* Provides factory to create concrete document object.
*/
class CVCApp : public CAknApplication
    {
    
    private:

        /**
        * From CApaApplication
        * @see CApaApplication for more information
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication
        * @see CApaApplication for more information
        */
        TUid AppDllUid() const;
  
    };

#endif  // CVCAPP_H

// End of File

