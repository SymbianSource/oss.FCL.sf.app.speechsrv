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
* Description:  Contains the UIDs for applications.
*
*/



#ifndef APPCONTROLLERCONSTANTS_H
#define APPCONTROLLERCONSTANTS_H

// INCLUDES
#include    <e32base.h>

// CONSTANTS

// Maximum length of user defined bluetooth name length
const TInt KBTMaxNameLength = 30;

// Profile ids start from 100 so they're separated from the apps
const TUint KProfileIdsStart    = 100;
// Value for training an original profile names
const TInt  KProfileValue = 100;

// These values are used to train SIND voice tags
// The first row application name has integer value of 0 - 100.
// The second row application name has integer value of 100 - 200
const TUint KSecondApplicationValue = 100;
// Profile ids start from 200
// The first row profile name has integer value of 200-300
const TUint	KFirstProfileValue = 200;
// The second row profile name has integer value of 300 - 400
const TUint	KSecondProfileValue = 300;

/**
* APPLICATION UIDS
*/

// Bluetooth ui UID
const TUid KUidAppBtUi = { 0x10005951 };

#endif      // APPCONTROLLERCONSTANTS_H
            
// End of File
