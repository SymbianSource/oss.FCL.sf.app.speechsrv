/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUISECONDARYDISPLAYAPI_H
#define VUISECONDARYDISPLAYAPI_H

// INCLUDES
#include <e32base.h>

#include "vuivoicerecogdefs.h"

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
const TUid KCatVoiceUi = {0x101F8543};

/*
* ==============================================================================
* Dialogs shown by VoiceUi subsystem. These messages are handled using the
* Secondary Display support in Avkon.
* ==============================================================================
*/
enum TVUISecondaryDisplayDialogs
    {
    /**
    * No note. Error condition if this comes to CoverUI
    */
    ECmdVoiceNoNote = 0,
    
    /**
    * A command for showing the "Speak now" note on secondary display.
    */
    ECmdShowVoiceSpeakNowNote,
    
    /**
    * A command for showing the "No match found" note on secondary display.
    */
    ECmdShowVoiceNoMatchesNote,

    /**
    * A command for showing the "Voice system error" note on secondary display.
    */
    ECmdShowVoiceSystemErrorNote,

    /**
    * A command for showing the "Call in progress" note on secondary display.
    */
    ECmdShowCallInProgressNote,

    /**
    * A command for showing the "Not recognized" note on secondary display.
    */
    ECmdShowVoiceNotRecognizedNote,

    /**
    * A command for showing the "No voice tags saved" note on secondary display.
    */
    ECmdShowVoiceNoTagNote,

    /**
    * A command for showing the matched item note on secondary display.
    */
    ECmdShowMatchedItemNote,

    /**
    * A command for showing the N-best list query on secondary display.
    */
    ECmdShowNBestListQuery,

    /**
    * A command for showing the voice verification query on secondary display.
    */
    ECmdShowVoiceVerificationQuery,
    
    /**
    * A command for showing the first time activation query on secondary display.
    */
    ECmdShowFirstTimeActivationQuery
    };

/*
* ==============================================================================
* Parameter definitions for the messages in this file.
* ==============================================================================
*/

// Package definition for Matched Item
struct TMatchData
    {
    TUid iType;
    TName iName;
    
    TBuf<KMaxPhoneNumberLength> iNumber;
    };    

typedef TPckgBuf<TMatchData> TMatchDataPckg;

// Package definition for Verification Item
struct TVerificationData
    {
    TName iName;
    };    

typedef TPckgBuf<TVerificationData> TVerificationDataPckg;

} // namespace SecondaryDisplay

#endif      // VUISECONDARYDISPLAYAPI_H
            
// End of File
