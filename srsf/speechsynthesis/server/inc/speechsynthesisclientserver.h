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
* Description:  Commom definitions for client and server side
*
*/


#ifndef SPEECHSYNTHESISCLIENTSERVER_H
#define SPEECHSYNTHESISCLIENTSERVER_H

#include <e32std.h>


_LIT( KSpeechSynthesisServerName, "!speechsynthesisserver" );
_LIT( KSpeechSynthesisServerImg, "speechsynthesisserver" );  // DLL/EXE name


// Definition of messages send from client to server via session
// Do NOT change order, add new messagwe ids to the end!
enum TSynthesisMessages
    {
    EPrime = 0,
    EPrimeFileOutput,
    EPrimeStreamOutput,
    ETransferFileHandle,
    ESynthesise,
    EStopSynthesis,     // 5
    EPauseSynthesis,
    EGetDuration, 
    EGetLanguages,
    EGetLanguageCount,
    EGetVoices,         // 10
    EGetVoiceCount,
    EVoice,
    ESetVoice,
    EMaxSpeakingRate,
    ESpeakingRate,      // 15
    ESetSpeakingRate,
    EMaxVolume,
    EVolume,
    ESetVolume,
    ESetAudioPriority,  // 20
    ESetAudioOutput,
    ECustomCommand
    };
    
const TInt KFsHandleIndex = 0;
const TInt KFileHandleIndex = 1;

#endif // SPEECHSYNTHESISCLIENT_H
