/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for Voice UI plug-in Handler
*
*/


#ifndef NSSVOICEUIPLUGINHANDLER_H
#define NSSVOICEUIPLUGINHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <nssvoiceevent.h>
#include <nssvoiceuiplugin.h>

// CLASS DECLARATION

/**
*  Class for handling voiceUI plug-ins
*
*  @lib NssVoiceUIPluginHandler.lib
*  @since 3.1
*/
class CNssVoiceUIPluginHandler : public CBase, MVoiceEventExecutor
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CNssVoiceUIPluginHandler* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNssVoiceUIPluginHandler();
        
    public: // Functions from base classes
        
        /**
        * From MVoiceEventExecutor
        * @since 3.1
        * @param aEventUID UID of voice event
        * @return One of the system wide error codes, KErrNone if successful.
        */
        TInt VoiceEvent( TUid aEventUID ); 
        
    private:
        
        /**
        * C++ default constructor.
        */
        CNssVoiceUIPluginHandler();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Plug-in loading
        */
        void LoadPluginsL();

        /**
        * Plug-in unloading
        */
        void UnloadPlugins();
        
    private: // Data
        
        // Array for event plug-ins
        RPointerArray<CVoiceEventObserverPluginBase> iEventPlugins;

        // IDs in Ecom
        RArray<TUid> iEventPluginDtorIds;

        // Array for action plug-ins
        RPointerArray<CVoiceUiPluginBase> iActionPlugins;

        // IDs in Ecom
        RArray<TUid> iActionPluginDtorIds;
        
    };

#endif // NSSVOICEUIPLUGINHANDLER_H   

// End of File
