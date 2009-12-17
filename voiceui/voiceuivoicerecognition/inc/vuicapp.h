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


#ifndef VUICAPP_H
#define VUICAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidVoiceUi = { 0x101F8543 };

// CLASS DECLARATION

/**
* CApp application class.
* Provides factory to create concrete document object.
*/
NONSHARABLE_CLASS( CApp ) : public CAknApplication
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

#endif  // VUICAPP_H

// End of File

