/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The MTagSelectNotification interface class allows the client to
*                register and deregister for a voice tag select event.
*                See MTagSelectHandler class for event handler. 
*
*/


#ifndef MNSSTAGSELECTNOTIFICATION_H
#define MNSSTAGSELECTNOTIFICATION_H

// INCLUDES
#include <e32base.h>
#include <e32def.h>
#include "nssvasmcontext.h"
#include "nssvasmtagselecthandler.h"

// CLASS DEFINITIONS

/**
*  Voice Tag Select Notification class.
*  Allows the client to register and deregister for a voice tag select event.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/

class MNssTagSelectNotification
{

public:


   public:  //enum

        enum TNssSelectResult
        {
        EVasErrorNone,
        EVasSelectFailed  
        };

        /**
        * Register for voice tag select notification
        * @since 2.0
        * @param aContext The voice tag context which determines notification.
        * @param aTagSelectHandler The event handler for tag select notification.
        * @return TSelectError Return code to indicate request was valid.
        */
        virtual TNssSelectResult RegisterL(
            MNssContext* aContext,
			MNssTagSelectHandler* aTagSelectHandler) = 0;

       /**
        * Deegister from voice tag select notification
        * @since 2.0
        * @param aContext The voice tag context which determines notification.
        * @param aTagSelectHandler The event handler for tag select notification,
		*                          used to deregister the correct client.
        * @return TSelectError Return code to indicate request was valid.
        */
        virtual TNssSelectResult Deregister(
            MNssContext* aContext,
			MNssTagSelectHandler * aTagSelectHandler) = 0;

};

#endif // MNSSTAGSELECTNOTIFICATION_H

// End of File
