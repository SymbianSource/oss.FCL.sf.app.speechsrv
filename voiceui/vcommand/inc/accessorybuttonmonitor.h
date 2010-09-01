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
* Description:  Definition of CVCAccessoryButtonMonitor
*
*/



#ifndef ACCESSORYBUTTONMONITOR_H
#define ACCESSORYBUTTONMONITOR_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  @app VCommand.exe
*/
class CVCAccessoryButtonMonitor : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVCAccessoryButtonMonitor* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CVCAccessoryButtonMonitor();

    public: // New functions
        
     
    public: // Functions from base classes

        
    private:

        /**
        * C++ default constructor.
        */
        CVCAccessoryButtonMonitor();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

    };

#endif      // ACCESSORYBUTTONMONITOR_H
            
// End of File
