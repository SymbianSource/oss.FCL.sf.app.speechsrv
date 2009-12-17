/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Declares main application class.
*
*/


#ifndef TESTAPPAPP_H
#define TESTAPPAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidTestApp = { 0x043ECF25 };

// CLASS DECLARATION

/**
* CTestAppApp application class.
* Provides factory to create concrete document object.
* 
*/
class CTestAppApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CTestAppDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidTestApp).
        * @return The value of KUidTestApp.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

