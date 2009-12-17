/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Voice key observer
*
*/


#ifndef VCMANAGERVOICEKEYLAUNCHER_H
#define VCMANAGERVOICEKEYLAUNCHER_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h> 
#include <eikenv.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS


/**
* Class for starting Voice UI
* @since 5.0
*/
NONSHARABLE_CLASS( CVCommandManagerVoiceKeyLauncher ) : public CActive
    {
    public:
        /** 
        *  Factory construction
        *  @return Constructed and already started observer
        */
        static CVCommandManagerVoiceKeyLauncher* NewL();
        
        /** 
        * Destructor 
        */
        ~CVCommandManagerVoiceKeyLauncher();

    public:     // From base classes
    
        /**
        * From CActive 
        * @see CActive for more information
        */
        void DoCancel();
    
        /**
        * From CActive 
        * @see CActive for more information
        */
        void RunL();
        
    private:
        /**
        * C++ constructor 
        */
        CVCommandManagerVoiceKeyLauncher();
        
        /**
        * Symbian second phase constructor 
        */
        void ConstructL();
        
        /**
        * Start voice ui 
        */        
        void LaunchVoiceUiL();
        
        
    private:
        
        // is key capture enabled
        TBool iKeyEnabled;
        
        // key handle for window group
        TInt32 iKeyCaptureHandle;
        TInt32 iLongKeyCaptureHandle;
        
        // window group
        RWindowGroup iWindowGroup;
        
        RWsSession iWsSession;
    };

#endif // VCMANAGERVOICEKEYLAUNCHER_H
            
// End of File
