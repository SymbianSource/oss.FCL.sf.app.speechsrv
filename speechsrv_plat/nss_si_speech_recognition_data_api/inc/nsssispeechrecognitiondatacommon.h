/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     This file contains common data shared between SISpeechRecognitionDataClient
*     %version: 10 %
*               and SISpeechRecognitionDataDevASR.
*
*  Copyright © 2002-2006 Nokia Corporation.
*/





#ifndef NSSSISPEECHRECOGNITIONDATACOMMON_H
#define NSSSISPEECHRECOGNITIONDATACOMMON_H

//  INCLUDES
#include <e32base.h>

// For externalize/internalize 
#include  <s32strm.h>
// For 
#include <bamdesca.h> 

// TYPEDEFS
typedef TUint16 TSIModelBankID;
typedef TUint16 TSIModelID;

typedef TUint16 TSILexiconID;
typedef TUint32 TSIPronunciationID;

typedef TUint16 TSIGrammarID;
typedef TUint32 TSIRuleID;
typedef TUint8  TSIRuleVariantID;

typedef RArray<TSIPronunciationID> TSIPronunciationIDSequence;

// Language array which is used as an element of another RArray
typedef RArray<TLanguage> RLanguageArray;

// CONSTANTS
const TSIPronunciationID KUndefinedPronounciationID = 0x0;

const TSIModelBankID     KInvalidModelBankID = 0x7fff;
const TSIModelID         KInvalidModelID     = 0x7fff;
const TSILexiconID       KInvalidLexiconID     = 0x7fff;
const TSIPronunciationID KInvalidPronunciationID=0x7fffffff;
const TSIGrammarID       KInvalidGrammarID     = 0x7fff;
const TSIRuleID          KInvalidRuleID        = 0x7fffffff;
const TSIRuleVariantID   KInvalidRuleVariantID = 0x7f;

// ASCII phonemes can't be longer than this.
const TInt   KMaxPhonemeLength = 7;

const TUint8 KPhonemeSeparator = '-';



// CONSTANTS
const TInt KUidAsrEventTrainReadyVal 				= 0x101FBF9E;
const TInt KUidAsrEventTrainVal 					= 0x101FBF9F;
const TInt KUidAsrEventAddPronunciationVal 			= 0x101FBFA0;
const TInt KUidAsrEventAddRuleVal 					= 0x101FBFA1;
const TInt KUidAsrEventLoadModelsVal 				= 0x101FBFA2;
const TInt KUidAsrEventLoadLexiconVal 				= 0x101FBFA3;
const TInt KUidAsrEventLoadGrammarVal 				= 0x101FBFA4;
const TInt KUidAsrEventRecognitionReadyVal 			= 0x101FBFA5;
const TInt KUidAsrEventRecognitionVal 				= 0x101FBFA6;
const TInt KUidAsrEventUnloadRuleVal 				= 0x101FBFA7;
const TInt KUidAsrEventRecordStartedVal 			= 0x101FBFA8;
const TInt KUidAsrEventRecordVal 					= 0x101FBFA9;
const TInt KUidAsrEventEouDetectedVal 				= 0x101FBFAA;
const TInt KUidAsrEventPlayStartedVal	 			= 0x101FBFAB;
const TInt KUidAsrEventPlayVal			 			= 0x101FBFAC;
const TInt KUidAsrEventCreateGrammarVal 			= 0x101FBFAD;
const TInt KUidAsrEventCreateLexiconVal 			= 0x101FBFAE;
const TInt KUidAsrEventCreateModelBankVal 			= 0x101FBFAF;
const TInt KUidAsrEventRemoveGrammarVal 			= 0x101FBFB0;
const TInt KUidAsrEventRemoveLexiconVal 			= 0x101FBFB1;
const TInt KUidAsrEventRemoveModelBankVal 			= 0x101FBFB2;
const TInt KUidAsrEventRemoveRuleVal 				= 0x101FBFB3;
const TInt KUidAsrEventRemovePronunciationVal 		= 0x101FBFB4;
const TInt KUidAsrEventRemoveModelVal 				= 0x101FBFB5;
const TInt KUidAsrEventGetAvailableStorageVal 		= 0x101FBFB6;
const TInt KUidAsrEventGetRuleValidityVal 			= 0x101FBFB7;
const TInt KUidAsrEventGetUtteranceDurationVal 		= 0x101FBFB8;
const TInt KUidAsrEventGetModelCountVal 			= 0x101FBFB9;
const TInt KUidAsrEventGetAllRuleIDsVal 			= 0x101FBFBA;
const TInt KUidAsrEventGetAllPronunciationIDsVal 	= 0x101FBFBB;
const TInt KUidAsrEventGetAllModelIDsVal 			= 0x101FBFBC;
const TInt KUidAsrEventGetAllClientGrammarIDsVal 	= 0x101FBFBD;
const TInt KUidAsrEventGetAllClientLexiconIDsVal 	= 0x101FBFBE;
const TInt KUidAsrEventGetAllClientModelBankIDsVal 	= 0x101FBFBF;
const TInt KUidAsrEventGetAllGrammarIDsVal 			= 0x101FBFC0;
const TInt KUidAsrEventGetAllLexiconIDsVal 			= 0x101FBFC1;
const TInt KUidAsrEventGetAllModelBankIDsVal 		= 0x101FBFC2;

// SI event values 
// !!!! There are not unique! Just successors of SD numbers.
const TInt KUidAsrEventAdaptVal         = 0x101FBFC3;
const TInt KUidAsrEventAddVoiceTagVal   = 0x101FBFC4;
const TInt KUidAsrEventAddVoiceTagsVal  = 0x101FBFC5;
const TInt KUidAsrEventSIRecognitionVal = 0x101FBFC6;
const TInt KUidAsrEventAddRuleVariantVal= 0x101FBFC7;
const TInt KUidAsrEventCreateRuleVal    = 0x101FBFC8;
const TInt KUidAsrEventEndRecordVal     = 0x101FBFC9;
const TInt KUidAsrEventUnloadGrammarVal = 0x101FBFCA;


const TInt KUidAsrEventUpdateGrammarAndLexiconVal = 0x101FBFCB;
const TInt KUidAsrEventActivateGrammarVal         = 0x101FBFCC;
const TInt KUidAsrEventDeactivateGrammarVal       = 0x101FBFCD;
const TInt KUidAsrEventRemoveRulesVal             = 0x101FBFCE;

const TInt KUidAsrEventGetPronunciationCountVal 	= 0x101FBFCF;
const TInt KUidAsrEventGetRuleCountVal 				= 0x101FBFD0;

const TInt KUidAsrEventPreStartSamplingVal          = 0x101FBFD1;

const TUid KUidAsrEventTrainReady 					= {KUidAsrEventTrainReadyVal};
const TUid KUidAsrEventTrain 						= {KUidAsrEventTrainVal};
const TUid KUidAsrEventAddPronunciation 			= {KUidAsrEventAddPronunciationVal};
const TUid KUidAsrEventAddRule 						= {KUidAsrEventAddRuleVal};
const TUid KUidAsrEventLoadModels	 				= {KUidAsrEventLoadModelsVal};
const TUid KUidAsrEventLoadLexicon	 				= {KUidAsrEventLoadLexiconVal};
const TUid KUidAsrEventLoadGrammar	 				= {KUidAsrEventLoadGrammarVal};
const TUid KUidAsrEventRecognitionReady	 			= {KUidAsrEventRecognitionReadyVal};
const TUid KUidAsrEventRecognition	 				= {KUidAsrEventRecognitionVal};
const TUid KUidAsrEventUnloadRule	 				= {KUidAsrEventUnloadRuleVal};
const TUid KUidAsrEventRecordStarted	 			= {KUidAsrEventRecordStartedVal};
const TUid KUidAsrEventRecord	 					= {KUidAsrEventRecordVal};
const TUid KUidAsrEventEouDetected	 				= {KUidAsrEventEouDetectedVal};
const TUid KUidAsrEventPlayStarted	 				= {KUidAsrEventPlayStartedVal};
const TUid KUidAsrEventPlay			 				= {KUidAsrEventPlayVal};
const TUid KUidAsrEventCreateGrammar	 			= {KUidAsrEventCreateGrammarVal};
const TUid KUidAsrEventCreateLexicon	 			= {KUidAsrEventCreateLexiconVal};
const TUid KUidAsrEventCreateModelBank				= {KUidAsrEventCreateModelBankVal};
const TUid KUidAsrEventRemoveGrammar				= {KUidAsrEventRemoveGrammarVal};
const TUid KUidAsrEventRemoveLexicon				= {KUidAsrEventRemoveLexiconVal};
const TUid KUidAsrEventRemoveModelBank				= {KUidAsrEventRemoveModelBankVal};
const TUid KUidAsrEventRemoveRule					= {KUidAsrEventRemoveRuleVal};
const TUid KUidAsrEventRemovePronunciation			= {KUidAsrEventRemovePronunciationVal};
const TUid KUidAsrEventRemoveModel					= {KUidAsrEventRemoveModelVal};
const TUid KUidAsrEventGetAvailableStorage			= {KUidAsrEventGetAvailableStorageVal};
const TUid KUidAsrEventGetRuleValidity				= {KUidAsrEventGetRuleValidityVal};
const TUid KUidAsrEventGetUtteranceDuration			= {KUidAsrEventGetUtteranceDurationVal};

const TUid KUidAsrEventGetModelCount				= {KUidAsrEventGetModelCountVal};
const TUid KUidAsrEventGetPronunciationCount		= {KUidAsrEventGetPronunciationCountVal};
const TUid KUidAsrEventGetRuleCount					= {KUidAsrEventGetRuleCountVal};

const TUid KUidAsrEventGetAllRuleIDs				= {KUidAsrEventGetAllRuleIDsVal};
const TUid KUidAsrEventGetAllPronunciationIDs		= {KUidAsrEventGetAllPronunciationIDsVal};
const TUid KUidAsrEventGetAllModelIDs				= {KUidAsrEventGetAllModelIDsVal};
const TUid KUidAsrEventGetAllClientGrammarIDs		= {KUidAsrEventGetAllClientGrammarIDsVal};
const TUid KUidAsrEventGetAllClientLexiconIDs		= {KUidAsrEventGetAllClientLexiconIDsVal};
const TUid KUidAsrEventGetAllClientModelBankIDs		= {KUidAsrEventGetAllClientModelBankIDsVal};
const TUid KUidAsrEventGetAllGrammarIDs				= {KUidAsrEventGetAllGrammarIDsVal};
const TUid KUidAsrEventGetAllLexiconIDs				= {KUidAsrEventGetAllLexiconIDsVal};
const TUid KUidAsrEventGetAllModelBankIDs			= {KUidAsrEventGetAllModelBankIDsVal};

// SI event UIDs
const TUid KUidAsrEventAdapt         = {KUidAsrEventAdaptVal};
const TUid KUidAsrEventAddVoiceTag   = {KUidAsrEventAddVoiceTagVal};
const TUid KUidAsrEventAddVoiceTags  = {KUidAsrEventAddVoiceTagsVal};
const TUid KUidAsrEventSIRecognition = {KUidAsrEventSIRecognitionVal};
const TUid KUidAsrEventAddRuleVariant= {KUidAsrEventAddRuleVariantVal};
const TUid KUidAsrEventCreateRule    = {KUidAsrEventCreateRuleVal};
const TUid KUidAsrEventEndRecord     = {KUidAsrEventEndRecordVal};
const TUid KUidAsrEventUnloadGrammar = {KUidAsrEventUnloadGrammarVal};
const TUid KUidAsrEventUpdateGrammarAndLexicon = {KUidAsrEventUpdateGrammarAndLexiconVal};
                                                       
const TUid KUidAsrEventActivateGrammar = {KUidAsrEventActivateGrammarVal};
const TUid KUidAsrEventDeactivateGrammar = {KUidAsrEventDeactivateGrammarVal};
const TUid KUidAsrEventRemoveRules = {KUidAsrEventRemoveRulesVal};

const TInt KUidMediaTypeSIVal = 0x101FF932;
const TUid KUidMediaTypeSI    = {KUidMediaTypeSIVal};

const TUid KUidAsrEventPreStartSampling = {KUidAsrEventPreStartSamplingVal};

// Speech Recognition Errors
const TInt KErrAsrNoSpeech 				= -12050;
const TInt KErrAsrSpeechTooEarly 		= -12051;
const TInt KErrAsrSpeechTooLong 		= -12052;
const TInt KErrAsrSpeechTooShort		= -12053;
const TInt KErrAsrNoMatch				= -12054;
const TInt KErrAsrDataRightViolation	= -12055;
const TInt KErrAsrNotRegisted			= -12056;
const TInt KErrAsrInvalidState			= -12057;
const TInt KErrAsrInitializationFailure	= -12058;

// Pre-defined Engine Parameter Identifiers starts from index 0x01
// User-define Engine Parameter Identifiers starts from index 0x10000 and end at index KMaxTInt.
const TInt KRecognizerMode = 0x01;
const TInt KGrammarSupport = 0x02;
const TInt KLoadMultipleGrammars = 0x03;
const TInt KLoadMultipleLexicons = 0x04;
const TInt KLoadMultipleModelBanks = 0x05;
const TInt KModelStorageCapacity = 0x06;
const TInt KMaxLoadableModels = 0x07;
const TInt KMaxFrontEndBackEndDelay 		= 0x08;
const TInt KSamplingRate 					= 0x09;
const TInt KBitsPerSample 					= 0x0A;

// Lexicon reference counter identifier
const TInt KLexiconReferenceCounter = 0x0B;


enum TNSSRecognitionMode
{
	ENSSSdMode,
	ENSSSiMode,
	ENSSSdSiMode
};

#endif // NSSSISPEECHRECOGNITIONDATACOMMON_H
