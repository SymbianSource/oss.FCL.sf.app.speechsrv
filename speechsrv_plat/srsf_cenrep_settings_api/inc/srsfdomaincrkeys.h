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
* Description:  Domain CenRep keys for SRSF
*
*/


#ifndef SRSFDOMAINCRKEYS_H
#define SRSFDOMAINCRKEYS_H

// SRSF Settings API
// Used for SRSF/SIND settings/status storage
const TUid KCRUidSRSFSettings = {0x10281C7D};

// TTS playback volume setting
const TUint32 KSRSFPlaybackVolume = 0x00000002;

// Rejection sensitivity setting
const TUint32 KSRSFRejection = 0x00000003;

// Language which was used in voice verification training
const TUint32 KSRSFVoiceUiLanguage = 0x00000004;

#endif // SRSFDOMAINCRKEYS_H
