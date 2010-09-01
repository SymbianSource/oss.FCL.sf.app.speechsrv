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
* Description:  Definition of voice event interfaces
*
*/


#ifndef NSSVOICEEVENT_H
#define NSSVOICEEVENT_H


// CLASS DECLARATION

/**
*  MVoiceEventExecutor for notifies VoiceUI Plug-in Handler about voice event
*
*  @since 3.1
*/
class MVoiceEventExecutor
    {
  
    public: 
        
        /**
        * Notifies observer that a voice event needs to be handled.
        * @since 3.1
        * @param aEventUID UID of voice event
        * @return One of the system wide error codes, KErrNone if successful.
        */
        virtual TInt VoiceEvent( TUid aEventUid ) = 0;
    };

/**
*  MVoiceEventObserverPlugin determines VoiceUI Plug-in interface.
*
*  @since 3.1
*/
class CVoiceEventObserverPluginBase : public CBase
    {
  
    public: 

        /**
        * Initializes a plug-in
        * @since 3.1
        * @param aEventHandler reference to a voice event handler.
        * @return none
        */
        virtual void InitializeL( MVoiceEventExecutor& aEventHandler ) = 0;
        
    };

#endif // NSSVOICEEVENT_H   

// End of File
