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
* Description:  Constants for the Voice commands UI
*
*/


#ifndef VCOMMANDCONSTANTS_H
#define VCOMMANDCONSTANTS_H

const TUid KUidVCMainView = { 1 };
const TUid KUidVCSettingsView = { 3 } ;
const TUid KUidVCFolderView = { 4 };

const TInt KVCMaxNameSize = 128;

// Middle Softkey control ID.
const TInt KVcMskControlID = 3;

// Setting items default values
//
const TInt KSettingsRejection( 5 );
const TInt KSettingsVolume( 5 );
const TInt KSettingsVerification( 0 );

// UID of the Help
const TUid KUidHelp = { 0x101F8555 };
const TUid KVoiceUiUid = { 0x101F8543 };

#endif      // VCOMMANDCONSTANTS_H   
   
// End of File

