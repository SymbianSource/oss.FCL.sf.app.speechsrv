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
* Description:  Algorithm manager for grammar handling algorihtms.
*
*/


// INCLUDE FILES
#include "devasrvmalgorithmmanager.h"
//#include "DevASRSrsAlgorithmManager.h"
#include <nssdevasr.h>
#include <nsssispeechrecognitiondatadevasr.h>
#include "rubydebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::CVMAlgorithmManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVMAlgorithmManager::CVMAlgorithmManager( MDevASRObserver& aObserver,
                                          MVmAlgMgrObserver& aVmObserver )
                                        : iGrCompiler( NULL ), 
                                          iVocMan( NULL ), 
                                          iTtp( NULL ), 
                                          iObserver( aObserver ),
                                          iTtpState( ETtpStateUninitialized ),
                                          iGrState( EGrStateUninitialized ),
                                          iVmAlgObserver( aVmObserver )
    {
    // Nothing
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVMAlgorithmManager::ConstructL()" );

    // TtpHwDevice
    iTtp = CASRSTtpHwDevice::NewL( *this );
    iTtp->InitializeL();
    iTtp->SetPhonemeNotationL( _L("NIPA8" ) );
    iTtpState = ETtpStateIdle;
    // Grammar compiler HW device
    iGrCompiler = CASRSGrCompilerHwDevice::NewL( *this );
    iGrCompiler->InitializeL();
    iGrState = EGrStateIdle;
    // VocManHwDevice
    iVocMan = CASRSVocManHwDevice::NewL();
    iVocMan->InitializeL();
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVMAlgorithmManager* CVMAlgorithmManager::NewL( MDevASRObserver& aObserver, 
                                                MVmAlgMgrObserver& aVmObserver )
    {
    CVMAlgorithmManager* self = new( ELeave ) CVMAlgorithmManager( aObserver, aVmObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::~CVMAlgorithmManager
// Destructor
// -----------------------------------------------------------------------------
//
CVMAlgorithmManager::~CVMAlgorithmManager()
    {
    if ( iGrCompiler != NULL )
        {
        iGrCompiler->Clear();
        }
    delete iGrCompiler;
    iGrCompiler = NULL;
    if ( iVocMan != NULL )
        {
        iVocMan->Clear();
        }
    delete iVocMan;
    iVocMan = NULL;
    if ( iTtp != NULL )
        {
        iTtp->Clear();
        }
    delete iTtp;
    iTtp = NULL;
//    iDataArray.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::AdaptVocabL
// Use CASRVocManHwDevice to adapt rule.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::AdaptVocabL( const CSIResultSet& aResultSet, 
                                       TInt aResultIndex )
    {
    RUBY_DEBUG_BLOCK( "CVMAlgorithmManager::AdaptVocabL()" );

    const CSIResult& iCorrectResult = aResultSet.AtL( aResultIndex );

    TSIGrammarID grammarID( iCorrectResult.GrammarID() );
    TSIRuleID ruleID( iCorrectResult.RuleID() );
    TSIRuleVariantID ruleVariantID( iCorrectResult.RuleVariantID() );
    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;
    CSICompiledGrammar* siGrammar = NULL;
  
    TRAPD( error, iVmAlgObserver.GetGrammarL( grammarID, &siActiveGrammar, &siNonActiveGrammar ) );
    User::LeaveIfError( error );

    TInt index = 0;

    if ( siActiveGrammar != NULL )
        {
        siGrammar = siActiveGrammar;
        }
    else if ( siNonActiveGrammar != NULL )
        {
        siGrammar = siNonActiveGrammar;
        }
    else
        {
        User::Leave( KErrNotFound );	
        }
    
    index = siGrammar->Find( ruleID );
    User::LeaveIfError( index );
    CSIRule& rule = siGrammar->AtL( index );
    
    // Do the actual vocabulary adaptation using VocManHwDevice
    TRAP( error, iVocMan->AdaptL( ruleVariantID, rule ) );
    // Neglect possible error since it just might be due to the reason
    // that there are no proper parameters in place within the RuleVariants..
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::ComputeNewGrammarSizeL
// Use CASRVocManHwDevice to compute size.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::ComputeNewGrammarSizeL( const CSIGrammar& aGrammar, 
                                                  const TUint32 aTargetNRuleVariants, 
                                                  const TUint32 aMaxNRuleVariants, 
                                                  const RArray<TUint>& aNewRuleScores, 
                                                  RArray<TUint>& aNNeNRuleVariants, 
                                                  TUint32& aNPrune )
    {
    RUBY_DEBUG_BLOCK( "CVMAlgorithmManager::ComputeNewGrammarSizeL()" );

    iVocMan->ComputeNRuleVariantsL( aGrammar, aTargetNRuleVariants, 
                                    aMaxNRuleVariants, aNewRuleScores, 
                                    aNNeNRuleVariants, aNPrune );
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::CompileGrammarL
// Compiles SI grammar.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::CompileGrammarL( CSICompiledGrammar& aGrammar/*, 
                                           CSIModelBank& aModelBank*/ )
    {
    RUBY_DEBUG_BLOCK( "CVMAlgorithmManager::CompileGrammarL()" );

    if ( iGrState == EGrStateUninitialized )
        {
        User::Leave( KErrNotReady );
        }
    if ( iGrState == EGrStateBusyCombining || iGrState == EGrStateBusyCompiling )
        {
        User::Leave( KErrInUse );
        }

    iGrState = EGrStateBusyCompiling;

    iGrCompiler->CompileGrammarL( aGrammar/*, aModelBank*/ );
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::CombineGrammarL
// Combines a list of grammars.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::CombineGrammarL( const RPointerArray<CSICompiledGrammar>& aCompiledGrammars,
                                           const RPointerArray<TSIRuleVariantInfo>& aExcludedRules )
    {
    RUBY_DEBUG_BLOCK( "CVMAlgorithmManager::CombineGrammarL()" );

    if ( iGrState == EGrStateUninitialized )
        {
        User::Leave( KErrNotReady );
        }
    if ( iGrState == EGrStateBusyCombining || iGrState == EGrStateBusyCompiling )
        {
        User::Leave( KErrInUse );
        }

    iGrState = EGrStateBusyCombining;

    iGrCompiler->CombineGrammarL( aCompiledGrammars, aExcludedRules );
    }

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::PruneGrammar
// Use CASRVocManHwDevice to prune a grammar.
// -----------------------------------------------------------------------------
//
TBool CVMAlgorithmManager::PruneGrammar( const CSIGrammar& aGrammar, 
                                         const TUint32 aMinNumber, 
                                         RArray<TSIRuleVariantInfo>& aPrunedRuleVariants )
    {
    return iVocMan->Prune( aGrammar, aMinNumber, aPrunedRuleVariants );
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::ResolveResult
// Use CASRVocManHwDevice to resolve a result.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::ResolveResult( const RArray<TUint>& aNBestIDs,
                                         CSIResultSet& aSIResultSet,
                                         const RPointerArray<CSICompiledGrammar>& aSICompiledGrammar,
                                         const TDesC8& aCombinedData/*,
                                         CSIModelBank& iSIModelBank*/ )
    {
    iGrCompiler->ResolveResult( aNBestIDs, aSIResultSet,
                                aSICompiledGrammar, aCombinedData );
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::TrainFromText
// Use CVMAlgorithmManager to train from text.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::TrainFromTextL( CSITtpWordList& aWordList, 
                                          const RArray<TLanguage>& aDefaultLanguage, 
                                          const RArray<TUint32>& aMaxNPronunsForWord )
    {
    RUBY_DEBUG_BLOCK( "CVMAlgorithmManager::TrainFromTextL()" );

    if ( iTtpState == ETtpStateUninitialized )
        {
        User::Leave( KErrNotReady );
        }
    if ( iTtpState == ETtpStateBusy )
        {
        User::Leave( KErrInUse );
        }

    iTtpState = ETtpStateBusy;
    
    TRAPD( error, iTtp->ConvertWordListL( aWordList, aDefaultLanguage, aMaxNPronunsForWord ) );
    if ( error != KErrNone )
        {
        iTtpState = ETtpStateIdle;
        User::Leave( error );
        }
    }


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::Cancel
// Use CVMAlgorithmManager to cancel training.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::Cancel()
    {
    if ( iTtpState == ETtpStateBusy )
        {
        iTtp->CancelWordListConversion();
        }
    if ( iGrState == EGrStateBusyCompiling )
        {
        iGrCompiler->CancelCompilation();
        }
    if ( iGrState == EGrStateBusyCombining )
        {
        iGrCompiler->CancelCombination();
        }
    }


// -----------------------------------------------------------------------------
// GetEnginePropertiesL
// Returns properties of the underlying engines.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::GetEnginePropertiesL( const RArray<TInt>& /*aPropertyId*/, 
                                                RArray<TInt>& /*aPropertyValue*/ )
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::LoadEnginePropertiesL
// Loads properties to engine, invalid IDs are neglected.
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::LoadEnginePropertiesL( const RArray<TInt>& /*aParameterId*/,
                                                 const RArray<TInt>& /*aParameterValue*/ )
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// MASRTtpHwDeviceObserver mixin begins
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CVMAlgorithmManager::MathdoConfigurationData
// 
// -----------------------------------------------------------------------------
//
HBufC8* CVMAlgorithmManager::MathdoConfigurationData( TUint32 aPackageType,
                                                      TUint32 aPackageID,
                                                      TUint32 aStartPosition,
                                                      TUint32 aEndPosition )
    {
    // Call DevASR observer
    return iObserver.ConfigurationData( aPackageType, aPackageID, 
                                        aStartPosition, aEndPosition );
    }
    

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::MathdoWordListReady
// 
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::MathdoWordListReady( const TInt aError )
    {
    RUBY_DEBUG0( "CVMAlgorithmManager::MathdoWordListReady()" );
    
    iTtpState = ETtpStateIdle;

    // Call DevASR observer
    iObserver.DevASREvent( EDevASRTrainFromText, aError );
    }
    

// -----------------------------------------------------------------------------
// MASRTtpHwDeviceObserver mixin ends
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MASRGrCompilerObserver mixin begins
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::MghdoGrammarCompilerComplete
// 
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::MghdoGrammarCompilerComplete( TInt aError )
    {
    iGrState = EGrStateIdle;
    // Forward callback to DevASR observer
    iObserver.DevASREvent( EDevASRGrammarCompile, aError );
    }

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::MghdoGrammarCombinerComplete
// 
// -----------------------------------------------------------------------------
//
void CVMAlgorithmManager::MghdoGrammarCombinerComplete( HBufC8* aResult, 
                                                        TInt aError )
    {
    iGrState = EGrStateIdle;
    iVmAlgObserver.CombineComplete( aResult, aError );
    }

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::MghdoSILexiconL
// 
// -----------------------------------------------------------------------------
//
CSILexicon* CVMAlgorithmManager::MghdoSILexiconL( TSILexiconID anID )
    {
    RUBY_DEBUG_BLOCK( "CVMAlgorithmManager::MghdoSILexiconL()" );

    return iObserver.SILexiconL( anID );
    }

// -----------------------------------------------------------------------------
// CVMAlgorithmManager::MghdoConfigurationData
// 
// -----------------------------------------------------------------------------
//
HBufC8* CVMAlgorithmManager::MghdoConfigurationData( TUint32 aPackageType,
                                                     TUint32 aPackageID,
                                                     TUint32 aStartPosition,
                                                     TUint32 aEndPosition )
    {
    // Call DevASR observer
    return iObserver.ConfigurationData( aPackageType, aPackageID, 
                                        aStartPosition, aEndPosition );
    }


// -----------------------------------------------------------------------------
// MASRGrCompilerObserver mixin ends
// -----------------------------------------------------------------------------

// End of File
