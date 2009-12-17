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
* Description:  Internal CR keys for SRSF
*
*/


#ifndef SRSFPRIVATECRKEYS_H
#define SRSFPRIVATECRKEYS_H

// Internal CR keys
#include "srsfinternalcrkeys.h"

// If value equals to one, indicates a full resync is needed on the next boot.
// If value equals to KImmediateResync, indicates an immediate full resync
const TUint32 KSRSFFullResyncNeeded = 0x00000000;

const TInt KImmediateResync = 2;

// Language which was used in VCommandManager training
const TUint32 KSRSFUiLanguage = 0x00000001;

#endif // SRSFPRIVATECRKEYS_H
