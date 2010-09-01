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
* Description:  The set of messages provided to Secondary Display software by
*              : Application controller.
*
*/

#ifndef APPCONTROLLERSECONDARYDISPLAYAPI_H
#define APPCONTROLLERSECONDARYDISPLAYAPI_H

// INCLUDES
#include <e32base.h>

/*
* ==============================================================================
* This file contains the following sections:
*   - Dialog definitions
*   - Command definitions
*   - Event definitions
*   - Parameter definitions
* ==============================================================================
*/

namespace SecondaryDisplay
{
	
// The category UID for the messages in this header file.
//
const TUid KCatAppController = {0x10281D15};

/*
* ==============================================================================
* Dialogs shown by VCExecutorApp subsystem. These messages are handled using the
* Secondary Display support in Avkon.
* ==============================================================================
*/
enum TAppControllerSecondaryDisplayDialogs
    {
    /**
    * No note. Error condition if this comes to CoverUI
    */
    ECmdAppControllerNoNote = 0,
    
    /**
    * A command for showing BT activation note on secondary display.
    */
    ECmdShowOfflineBtActivationQuery,

    /**
    * A command for showing BT disabled in offline on secondary display.
    */
    ECmdShowBtDisabledOfflineNote,

    /**
    * A command for showing BT general error on secondary display.
    */
    ECmdShowBtGeneralErrorNote,
    
    /**
    * A command for showing leave offline profile note on secondary display.
    */
    ECmdShowLeaveOfflineProfileQuery
    };
}

#endif      // APPCONTROLLERSECONDARYDISPLAYAPI_H
            
// End of File
