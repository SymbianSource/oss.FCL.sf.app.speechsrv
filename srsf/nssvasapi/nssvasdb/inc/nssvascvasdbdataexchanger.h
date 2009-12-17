/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines serialized data types, which were used to transfer
*                data between the clinet/server interface of VAS.
*
*/


#ifndef __CNSSVASDBDATAEXCHANGER_H
#define __CNSSVASDBDATAEXCHANGER_H

// Data Exchange Interface Definition
#include "nssvasdbkonsts.h"
#include "nssvasccontext.h"

//typedef TBuf<KNssVasDbRRDText> NssVasDbRRDText;

class TNssSpeechItem
    {
public:
    TUint32 iRuleId;
    TUint32 iTagId;
    TBufC<KNssVasDbTagName> iName;
    };

class TNssGrammarIdRuleId
    {
public:
    TUint32 iGrammarId;
    TUint32 iRuleId;
    };
    
typedef CArrayFixFlat<TNssGrammarIdRuleId> TNssGrammarIdRuleIdListArray;    

class TNssTagReference
    {
public:
    TInt      iTagId;
    TBufC<40> iTagName;
    };

typedef CArrayFixFlat<TNssTagReference> TNssTagReferenceListArray;

typedef TPckgC<TNssGrammarIdRuleId>     TPckgCTNssGrammarIdRuleId;

#endif // __CNSSVASDBDATAEXCHANGER_H
