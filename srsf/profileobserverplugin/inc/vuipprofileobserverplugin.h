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
*
*/


#include <e32base.h>
#include <nssvoiceevent.h>


// Forward declaration
//
class CVCommandProfileObserver;


/**
 * Class which uses VUIP-framework to start profile observing when 
 * phone is booted.
 *
 */
class CVUIPprofileobserverplugin : public CVoiceEventObserverPluginBase
    {
    
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVUIPprofileobserverplugin* NewL();
      
        /**
        * Destructor.
        */
        ~CVUIPprofileobserverplugin();


    public: // Functions from base classes

        /**
        * From CVoiceEventObserverPluginBase
        * @param aEventHandler reference to a voice event handler.
        */        
        void InitializeL( MVoiceEventExecutor& aEventHandler );


    private:
        
        /**
        * C++ default constructor.
        */
        CVUIPprofileobserverplugin();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

   
    private: // Data
    
        CVCommandProfileObserver*   iProfileObserver; // owned
    };
    
   