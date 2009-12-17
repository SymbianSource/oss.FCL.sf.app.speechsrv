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
* Description:  Common definitions used by the DevASR module.
*
*/


#ifndef NSSDEVASRCOMMON_H
#define NSSDEVASRCOMMON_H

// TYPEDEFS
typedef TInt TDevASRError;

// CONSTANTS
// Pre-defined Engine Parameter Identifiers starts from index 0x01
// User-define Engine Parameter Identifiers starts from index 0x10000 and end at index KMaxTInt.

// Property identifiers
// Flag to tell if DevASR should call FeatureVectorDataRcvd callback or not
const TInt KDevASRSendFeatures = 0x1F000;
// Flag to tell if result set should contain adaptation data or not
const TInt KDevASRAdaptation = 0x1F001;

// DATA TYPES

/**
* DevASR Event codes
*/
enum TDevASREvent
	{
    EDevASREouDetected = 1,
    EDevASRInitFrontend,
    EDevASRInitRecognitionBackend,
    EDevASRInitTrainBackend,
    EDevASRLoadGrammar,
    EDevASRLoadLexicon,
    EDevASRLoadModels,
    EDevASRUnloadRule,
    EDevASRPlay,
    EDevASRPlayStarted,
    EDevASRRecognize,
    EDevASRRecord,
    EDevASRRecordStarted,
    EDevASRTrain,
    EDevASRTrainFromText,
    EDevASRActivateGrammar,
    EDevASRDeactivateGrammar,
    EDevASRAdapt,
    EDevASRGrammarCompile,
    EDevASRUnloadGrammar
	};

/**
* Mode used to initialize the recognizer
*/
enum TRecognizerMode
	{
	ESiRecognition,
	ESdRecognition,
	ESdTraining,
	ESiRecognitionTest,
	ESdRecognitionTest,
	ESdTrainingTest,
	ESiRecognitionSpeechInput,
    ESdRecognitionSpeechInput,
    ESdTrainingSpeechInput,
    ESiRecognitionTestSpeechInput,
    ESdRecognitionTestSpeechInput,
    ESdTrainingTestSpeechInput,
	};

#endif // NSSDEVASRCOMMON_H
