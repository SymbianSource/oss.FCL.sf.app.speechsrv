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
* Description:  Voice UI Accessory Monitor API
*
*/



#ifndef VOICEUIDOMAINPSKEYS_H
#define VOICEUIDOMAINPSKEYS_H

// Voice UI Accessory Monitor API
// VCommandmanager uses this API for notifying
// Voice UI about certain Symbian Remote Control events.

const TUid KPSUidVoiceUiAccMonitor = {0x101F8543};

// Value: Accessory event id
const TUint32 KVoiceUiAccessoryEvent = 0x00000001;

const TInt KVoiceUiNoEvent = 0;
const TInt KVoiceUiShortPressEvent = 1;
const TInt KVoiceUiLongPressEvent = 2;

const TUint32 KVoiceUiOpenKey = 0x00000002;

const TInt KVoiceUiIsClose = 0;
const TInt KVoiceUiIsOpen = 1;

#endif // VOICEUIDOMAINPSKEYS_H
