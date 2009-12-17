/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the implementation of DevASR.
*
*/


// INCLUDE FILES
#include <nssdevasr.h>
#include "devasrsrsalgorithmmanager.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDevASR::CDevASR
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevASR::CDevASR() : iSRSAlgorithmManager( NULL ),
                     iReserved1( NULL ),
                     iReserved2( NULL )
    {
    // Nothing
    }


// -----------------------------------------------------------------------------
// CDevASR::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevASR::ConstructL( MDevASRObserver& aObserver )
    {
    iSRSAlgorithmManager = CSRSAlgorithmManager::NewL( aObserver );
    }


// -----------------------------------------------------------------------------
// CDevASR::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDevASR* CDevASR::NewL( MDevASRObserver& aObserver )
    {
    CDevASR* self = NewLC( aObserver );
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CDevASR::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDevASR* CDevASR::NewLC( MDevASRObserver& aObserver )
    {
    CDevASR* self = new ( ELeave ) CDevASR;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    return self;
    }


// -----------------------------------------------------------------------------
// CDevASR::~CDevASR
// The destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDevASR::~CDevASR()
    {
    delete iSRSAlgorithmManager;
    }


// -----------------------------------------------------------------------------
// CDevASR::ActivateGrammarL
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::ActivateGrammarL( TSIGrammarID aGrammarID )
    {
    iSRSAlgorithmManager->ActivateGrammarL( aGrammarID );
    }


// -----------------------------------------------------------------------------
// CDevASR::AdaptL
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::AdaptL( const CSIResultSet& aResultSet, TInt aResultIndex )
    {
    // Adaptation
    iSRSAlgorithmManager->AdaptL( aResultSet, aResultIndex );
    }


// -----------------------------------------------------------------------------
// CDevASR::Cancel
// Cancels the current and any on going requests/tasks. Forwards call to
// CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::Cancel()
    {
    iSRSAlgorithmManager->Cancel();
    }


// -----------------------------------------------------------------------------
// CDevASR::Cancel
// Compiles SI grammar. Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::CompileGrammarL( CSICompiledGrammar& aGrammar/*, 
                                        CSIModelBank& aModelBank*/ )
    {
    iSRSAlgorithmManager->CompileGrammarL( aGrammar/*, aModelBank*/ );
    }


// -----------------------------------------------------------------------------
// CDevASR::ComputeNewGrammarSizeL
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::ComputeNewGrammarSizeL( const CSIGrammar& aGrammar, 
                                               const TUint32 aTargetNRuleVariants, 
                                               const TUint32 aMaxNRuleVariants, 
                                               const RArray<TUint>& aNewRuleScores, 
                                               RArray<TUint>& aNewRuleVariants, 
                                               TUint32& aNPrune )
    {
    iSRSAlgorithmManager->ComputeNewGrammarSizeL( aGrammar, 
                                                  aTargetNRuleVariants, 
                                                  aMaxNRuleVariants, 
                                                  aNewRuleScores, 
                                                  aNewRuleVariants, 
                                                  aNPrune );
    }


// -----------------------------------------------------------------------------
// CDevASR::CustomInterface
// Send a command to the implementation that the DevASR API doesn't currently
// supports. Now returns NULL every time, no custom operations.
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CDevASR::CustomInterface( TUid /*aCustomInterfaceId*/ )
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CDevASR::DeactivateGrammarL
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::DeactivateGrammarL( TSIGrammarID aGrammarID )
    {
    iSRSAlgorithmManager->DeactivateGrammarL( aGrammarID );
    }


// -----------------------------------------------------------------------------
// CDevASR::EndRecord
// Requests to end recording and a result will be resolved. Passes the function 
// call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::EndRecord()
    {
    iSRSAlgorithmManager->StopRecognition();
    }


// -----------------------------------------------------------------------------
// CDevASR::EndRecSession
// Signal the end of a recognition session. Passes the function call to 
// CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::EndRecSession()
    {
    iSRSAlgorithmManager->EndRecSession();
    }


// -----------------------------------------------------------------------------
// CDevASR::GetEnginePropertiesL
// Retreive the properties of the underlying speech recognition engine.
// An array of values corresponding to the querried identifiers will be populated.
// The function may leave with KErrArgument. Passes the function call to 
// CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::GetEnginePropertiesL( const RArray<TInt>& aPropertyId,
                                             RArray<TInt>& aPropertyValue)
    {
    iSRSAlgorithmManager->GetEnginePropertiesL( aPropertyId, aPropertyValue );
    }


// -----------------------------------------------------------------------------
// CDevASR::GetUtteranceData
// Get the recorded audio data accumulated during training. Passes the function 
// call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::GetUtteranceData( TDes8* aBuffer,
                                         TTimeIntervalMicroSeconds32& aDuration )
    {
    iSRSAlgorithmManager->GetUtteranceData( aBuffer, aDuration );
    }


// -----------------------------------------------------------------------------
// CDevASR::InitFrontEnd
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::InitFrontEnd( TRecognizerMode aMode )
    {
    iSRSAlgorithmManager->InitFrontEnd( aMode );
    }


// -----------------------------------------------------------------------------
// CDevASR::InitRecognizerBE
// Initializes the recognition back-end. The module responsible for recognition
// function is started as a result. Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::InitRecognizerBE( CSIResultSet& aResult )
    {
    iSRSAlgorithmManager->InitRecognizerBE( aResult );
    }


// -----------------------------------------------------------------------------
// CDevASR::LoadEngineParametersL
// Load the specified recognizer parameter(s).
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::LoadEngineParametersL( const RArray<TInt>& aParameterId,
                                              const RArray<TInt>& aParameterValue )
    {
    iSRSAlgorithmManager->LoadEnginePropertiesL( aParameterId, aParameterValue );
    }


// -----------------------------------------------------------------------------
// CDevASR::LoadGrammar
// Load the specified SI grammar into the recognizer.
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::LoadGrammar( const CSIGrammar& aGrammar )
    {
    TRAPD( error, iSRSAlgorithmManager->LoadGrammarL( aGrammar ) );
    // No way to process or send the error upwards.
    error = error;
    }


// -----------------------------------------------------------------------------
// CDevASR::LoadGrammar
// Load the specified compiled SI grammar into the recognizer.
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::LoadGrammar( const CSICompiledGrammar& aGrammar )
    {	
    TRAPD( error, iSRSAlgorithmManager->LoadGrammarL( aGrammar ) );
    // No way to process or send the error upwards.
    error = error;
    }


// -----------------------------------------------------------------------------
// CDevASR::UnloadGrammar
// Unloads the specified SI grammar from the recognizer.
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::UnloadGrammar( const CSIGrammar& aGrammar )
    {
    TRAPD( error, iSRSAlgorithmManager->UnloadGrammarL( aGrammar ) );
    // No way to process or send the error upwards.
    error = error;
    }


// -----------------------------------------------------------------------------
// CDevASR::LoadGrammar
// Unloads the specified compiled SI grammar from the recognizer.
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::UnloadGrammar( const CSICompiledGrammar& aGrammar )
    {	
    TRAPD( error, iSRSAlgorithmManager->UnloadGrammarL( aGrammar ) );
    // No way to process or send the error upwards.
    error = error;
    }


// -----------------------------------------------------------------------------
// CDevASR::LoadLexicon
// Load the specified SI lexicion into the recognizer.
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::LoadLexicon( const CSILexicon& aLexicon )
    {
    TRAPD( error, iSRSAlgorithmManager->LoadLexiconL( aLexicon ) );
    // No way to process or send the error upwards.
    error = error;
    }


// -----------------------------------------------------------------------------
// CDevASR::LoadModels
// Load the specified SI models into the recognizer.
// Forwards call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::LoadModels( const CSIModelBank& aModels )
    {
    TRAPD( error, iSRSAlgorithmManager->LoadModelsL( aModels ) );
    // No way to process or send the error upwards.
    error = error;
    }


// -----------------------------------------------------------------------------
// CDevASR::PruneGrammar
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDevASR::PruneGrammar( const CSIGrammar& aGrammar, 
                                      const TUint32 aMinNumber, 
                                      RArray<TSIRuleVariantInfo>& aPrunedRuleVariants )
    {
    return iSRSAlgorithmManager->PruneGrammar( aGrammar, 
                                               aMinNumber, 
                                               aPrunedRuleVariants );
    }


// -----------------------------------------------------------------------------
// CDevASR::Record
// Request to begin recording. Passes the function call to CVMAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::Record( TTimeIntervalMicroSeconds32 aRecordDuration )
    {
    iSRSAlgorithmManager->StartRecognition( aRecordDuration );
    }


// -----------------------------------------------------------------------------
// CDevASR::PreStartSamplingL
// Begins sampling before Record() call
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::PreStartSamplingL()
    {
    iSRSAlgorithmManager->PreStartSamplingL();
    }
    

// -----------------------------------------------------------------------------
// CDevASR::SendSpeechData
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::SendSpeechData( TPtrC8& aBuffer, TBool aEnd )
    {
    iSRSAlgorithmManager->SendSpeechData( aBuffer, aEnd );
    }


// -----------------------------------------------------------------------------
// CDevASR::SetPrioritySettings
// Set the priority of the sound device
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings )
    {
    iSRSAlgorithmManager->SetPrioritySettings( aPrioritySettings );
    }


// -----------------------------------------------------------------------------
// CDevASR::StartRecSession
// Signal the start of a recognition session.
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDevASR::StartRecSession( TRecognizerMode aMode )
    {
    return iSRSAlgorithmManager->StartRecSession( aMode );
    }


// -----------------------------------------------------------------------------
// CDevASR::StartTrainFromText
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::StartTrainingFromTextL( CSITtpWordList& aWordList, 
                                               const RArray<TLanguage>& aDefaultLanguage, 
                                               const RArray<TUint32>& aMaxNPronunsForWord )

    {
    iSRSAlgorithmManager->StartTrainingFromTextL( aWordList, aDefaultLanguage, 
                                                  aMaxNPronunsForWord );
    }


// -----------------------------------------------------------------------------
// CDevASR::UnloadRule
// Unload the specified rule from the grammar.
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevASR::UnloadRule( TSIGrammarID aGrammarID,
                                   TSIRuleID aRuleID )
    {
    iSRSAlgorithmManager->UnloadRule( aGrammarID, aRuleID );
    }


// -----------------------------------------------------------------------------
// CDevASR::IsGrammarLoaded
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDevASR::IsGrammarLoaded( TSIGrammarID aGrammarID )
    {
    return iSRSAlgorithmManager->IsGrammarLoaded( aGrammarID );
    }


// -----------------------------------------------------------------------------
// CDevASR::IsGrammarActive
// Passes the function call to CSRSAlgorithmManager.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDevASR::IsGrammarActive( TSIGrammarID aGrammarID )
    {
    return iSRSAlgorithmManager->IsGrammarActive( aGrammarID );
    }

// End of File
