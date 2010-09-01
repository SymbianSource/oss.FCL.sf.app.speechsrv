/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains general data definitions for pbkinfoview.
*
*/


#ifndef PBKINFOVIEWDEFINES_H
#define PBKINFOVIEWDEFINES_H

//  INCLUDES
#include <e32base.h>
#include <AiwGenericParam.hrh>

// CONSTANTS
_LIT( KLocFileString, "%U" );
_LIT( KFormatString, "%S" );
_LIT( KEmpty, "" );
_LIT( KSpace, " " );
_LIT( KUnderscore, "_" );
_LIT( KTagNameSeparator, "_1" );
_LIT( KTagExtensionSeparator, "_2" );
_LIT( KMatchNameSeparator, "_1*" );

const TInt KTagSeparatorLength = 2;
const TInt32 KVoiceTaglessContactId = -2;

// resource files
_LIT( KResFileName, "z:pbkinfoview.rsc" );
_LIT( KBitmapFileName, "z:avkon2.mbm" );

// string sizes
const TInt KBufSize16 = 16;

const TInt KDefaultArraySize = 10;

#endif  // PBKINFOVIEWDEFINES_H

// End of File
