/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  VAS database constants
*
*/


#ifndef NSSVASDBKONSTS_H
#define NSSVASDBKONSTS_H

// CONSTANTS

_LIT( KNameCol, "name" );
_LIT( KGlobalCol, "global" );
_LIT( KGrammarIdCol, "grammarid" );
_LIT( KLexiconIdCol, "lexiconid" );
_LIT( KModelBankIdCol, "modelbankid" );
_LIT( KContextIdCol, "contextid" );

_LIT( KRRDTextCol, "rrdtext" );
_LIT( KRRDIntCol, "rrdint" );
_LIT( KRRDPositionCol, "rrdposition" );

_LIT( KModelIdCol,"modelid" );
_LIT( KPronunciationIdCol, "pronunciationid" );
_LIT( KTrainTypeCol, "traintype" );
_LIT( KClientDataCol, "clientdata" );
_LIT( KRuleIdCol, "ruleid" );

_LIT( KTagIdCol, "tagid" );

const TInt KNssVasDbContextName          =50;
const TInt KNssVasDbRRDText              =64;
const TInt KNssVasDbSpeechItemName       =128;
const TInt KNssVasDbTagName              =128;
const TInt KNssStatementLength           =300; // can be really long, but 300 can be enough
const TInt KNssMaxSQLLength              =KNssVasDbContextName + KNssStatementLength;
const TInt KNssMaxNumLength              =10;
const TInt KNssAppDynamicArrayGranularity=10;

// Vas Db specific error codes
const TInt KErrDbFailed	         =-1000;
const TInt KErrDbNotSet          =-1001;
const TInt KErrDbNoData	         =-1002;
const TInt KErrDbNoSuchContext   =-1003;
const TInt KErrDbNoSuchEvent     =-1004;
const TInt KErrItemAlreadyExist  =-1005;
const TInt KErrItemRefusedGeneral=-1006;
const TInt KErrTransferFailed    =-1007;
const TInt KErrDbAlreadyOpen     =-1008;
const TInt KErrDbBusy            =-1009;
const TInt KErrAlreadyConnected  =-1010;

#endif // NSSVASDBKONSTS_H

// End of file
