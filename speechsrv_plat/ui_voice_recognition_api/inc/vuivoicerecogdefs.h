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
* Description:  Constant definitions 
*
*/


#ifndef VUIVOICERECOGDEFS_H
#define VUIVOICERECOGDEFS_H

const TInt KRecogFinal = 32;
const TInt KRecogInterval = 1;
const TInt KRecogIncrement = 1;
const TInt KRecogDelay = 1;
const TInt KKRecogInterval = 125000;

const TInt KMaxPhoneNumberLength = 48;  // taken from Phonebook
const TInt KMaxNameLength = 110;    // taken from Phonebook
const TInt KMaxFieldLength = 150;    // taken from Phonebook, EMail address length

const TInt KSindMaxResults = 6;    // The number of voice tags needed for the N-Best List.

const TInt KTimeoutMicroseconds = 4*1000000; // 4 seconds
const TInt KErrorMessageTimeoutMicroseconds = 3*1000000; // 3 seconds
const TInt KPlaybackTimeout = 45;    // ~2.5s depending on HW

const TInt KVerificationCommands = 3;
const TInt KVerificationResults = 3;
const TInt KVerificationTries = 3;

_LIT( KVoiceDialContext, "NAMEDIAL" );    // context for phonebook contact voice tags
_LIT( KVoiceCommandContext, "COMMAND" );    // context for voice commands application tags
_LIT( KVoiceVerificationContext, "VERIFICATION" );    // context for voice verification commands tags

_LIT( KDirAndFile,"z:voiceui.mbm" );

_LIT8( KVoiceUiMode, "DEVICELOCKMODE" );    // Startup mode for device lock functionality

const TUid KVCommandAppUid = { 0x101F8555 };
const TUid KVoiceUiUID = { 0x101F8543 };

const TInt KProfileValue = 200;

// Voice Ui error codes
const TInt KErrInit                     = -1000;
const TInt KErrNoMatch                  = -1001;
const TInt KErrNoResults                = -1002;
const TInt KErrCallInProgress           = -1003;
const TInt KErrNoContacts               = -1004;
const TInt KErrRecogFailed              = -1005;
const TInt KErrNoSpeech                 = -1006;
const TInt KErrTooEarly                 = -1007;
const TInt KErrTooLong                  = -1008;
const TInt KErrTooShort                 = -1009;
const TInt KErrNoVerificationTrained    = -1009;

// Verification modes
enum TVuiVerificationMode
    {
    EAutomatic = 0,
    EManual,
    EVoice
    };

// Voice Ui keypress codes
enum TVuiKeypress
    {
    ENoKeypress = 1000,
    EShortKeypress,
    ELongKeypress,
    EEndCallKeypress,
    EUpKeypress,
    EDownKeypress,
    ESelectKeypress,
    EDirectSelectKeypress,
    EOpenKeypress,
    EMoreKeypress,
    EOptionsKeypress,
    EScrollKeypress,
    EDragKeypress
    };

struct VTimerModel
    {
    TInt iFinalValue;
    TInt iHundreths;
    TInt iIncrement;
    TBool iRunning;
    };
    
// Read capability: ReadDeviceData.
_LIT_SECURITY_POLICY_C1( KVoiceUiReadPolicy, ECapabilityReadDeviceData );
    
// Write capability: WriteDeviceData.
_LIT_SECURITY_POLICY_C1( KVoiceUiWritePolicy, ECapabilityWriteDeviceData );

#endif    // VUIVOICERECOGDEFS_H

// End of File

