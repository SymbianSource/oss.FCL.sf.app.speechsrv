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
* Description:  This file contains the implementation of Recognition Algorithm
*               Manager, a submodule of DevASR.
*
*/


// INCLUDE FILES
#ifdef _DEBUG
#include <e32svr.h>
#endif
#include "devasrrecognitionalgmgr.h"
#include "rubydebug.h"

// CONSTANTS
const TInt KInitFrontEnd        = 1;
//const TInt KInitTrainBE         = 2;
const TInt KInitRecognizerBE    = 3;
const TInt KLoadGrammar         = 4;
const TInt KLoadLexicon         = 5;
const TInt KLoadModels          = 6;
//const TInt KUtteranceDataRcvd   = 7;
const TInt KActivateGrammar     = 8;
const TInt KDeActivateGrammar   = 9;
const TInt KUnloadRule          = 10;
const TInt KUnloadGrammar       = 11;

//_LIT( KPanicDescriptor, "CRecognitionAlgMgr Panic" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::CRecognitionAlgMgr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRecognitionAlgMgr::CRecognitionAlgMgr( MRecAlgMgrObserver& aObserver )
                    : CActive( CActive::EPriorityStandard ),
                      iRecAlgMgrObserver( &aObserver ),
                      iCombinedGrammar( NULL ),
                      iCombineNeeded( ETrue ),
                      iInitialized( EFalse ),
                      iRecoHw( NULL ),
                      iAdaptHw( NULL ),
                      iSDModelBank( NULL ),
                      iSIModelBank( NULL ),
                      iSDGrammar( NULL ),
                      iSIGrammar( NULL ),
                      iSICompGrammar( NULL ),
                      iSDCompGrammar( NULL ),
                      iSDLexicon( NULL ),
                      iSILexicon( NULL ),
                      iFEState( EIdle ),
                      iBEState( EIdle ),
                      iFeatures( EFalse ),
                      iAdaptation( ETrue ),
                      iAdaptationData( NULL )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::CRecognitionAlgMgr()" );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::ConstructL()" );

    iRecoHw = CASRSRecognitionHwDevice::NewL( *this );
    iRecoHw->InitializeL();

    iAdaptHw = CASRSAdaptHwDevice::NewL( *this );
    iAdaptHw->InitializeL();

    CActiveScheduler::Add(this);
    
    RUBY_DEBUG1( "CRecognitionAlgMgr::ConstructL(): RecoHwDevice instance [%x]", iRecoHw );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRecognitionAlgMgr* CRecognitionAlgMgr::NewL( MRecAlgMgrObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::ConstructL()" );
    
    CRecognitionAlgMgr* self = new ( ELeave ) CRecognitionAlgMgr( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::~CRecognitionAlgMgr
// Destructor
// -----------------------------------------------------------------------------
//
CRecognitionAlgMgr::~CRecognitionAlgMgr()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::~CRecognitionAlgMgr()" );
    
    iSIActiveGrammars.Reset();
    iSIDeActivatedGrammars.Reset();
    iNBestList.Reset();
    iScores.Reset();

    if ( iRecoHw != NULL )
        {
        iRecoHw->Clear();
        delete iRecoHw;
        }

    if ( iAdaptHw != NULL )
        {
        iAdaptHw->Clear();
        delete iAdaptHw;
        }

    delete iCombinedGrammar;

    iBlackList.ResetAndDestroy();
    iBlackList.Close();

    delete iAdaptationData;
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::AdaptModelsL
// Adapts models.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::AdaptModelsL( const CSIResultSet& aResultSet, 
                                       TInt aResultIndex,
                                       TLanguage aLanguage )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::AdaptModelsL()" );

    RUBY_DEBUG1( "CRecognitionAlgMgr::AdaptModelsL aResultIndex: %x", aResultIndex );

    // TODO: Not too nice to remove consts like this
    CSIResult& iCorrectResult = ( CSIResult& ) aResultSet.AtL( aResultIndex );

    // Check that we have a modelbank to adapt
    if ( iSIModelBank == NULL )
        {
        User::Leave( KErrNotReady );
        }

    CSIResultSet& iResultSet = ( CSIResultSet& ) aResultSet;
    iAdaptHw->StartAdaptationL( iResultSet.AdaptationData(), 
                                *iSIModelBank, 
                                iCorrectResult.Pronunciation(),
                                aLanguage );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::InitFrontEnd
// Receives the initialize frontend request and starts an asynchronous handler
// for the request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::InitFrontEnd( TRecognizerMode aMode )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::InitFrontEnd()" );

    // Save arguments and set code
    iMode = aMode;
    iRequestFunction = KInitFrontEnd;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleInitFrontEnd
// Asynchronously handles the initialize frontend request. The front-end module
// is started as a result.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleInitFrontEnd()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleInitFrontEnd()" );
    
    if ( !iInitialized )
        {
        iRecAlgMgrObserver->InitFEComplete( KErrNotReady );
        return;
        }

    if ( iFEState != EIdle )
        {
        iRecAlgMgrObserver->InitFEComplete( KErrInUse );
        return;
        }
    
    if ( iMode != ESiRecognition && iMode != ESiRecognitionSpeechInput )
        {
        iRecAlgMgrObserver->InitFEComplete( KErrNotSupported );
        return;
        }

    TInt error = KErrNone;

    if ( iFeatures || iAdaptation )
        {
        TRAP( error, iRecoHw->InitRecognizerFEL( ETrue ) );
        }
    else
        {
        TRAP( error, iRecoHw->InitRecognizerFEL( EFalse ) );
        }

    iRecAlgMgrObserver->InitFEComplete( error );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::InitRecognizerBE
// Receives the initialize recognition backend request and starts an asynchronous
// handler for the request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::InitRecognizerBE( CSIResultSet& aResult )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::InitRecognizerBE()" );
    
    // Save arguments and set code
    iSDResult = NULL;
    iSIResult = &aResult;
    iRequestFunction = KInitRecognizerBE;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleInitRecognizerBE
// Asynchronously handles the initialize recognition backend request. The backend
// module for recognition function is started as a result.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleInitRecognizerBE()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleInitRecognizerBE()" );

    if ( !iInitialized )
        {
        iRecAlgMgrObserver->InitRecognizerBEComplete( KErrNotReady );
        return;
        }

    if ( iBEState != EIdle )
        {
        iRecAlgMgrObserver->InitRecognizerBEComplete( KErrInUse );
        return;
        }
    TInt error( KErrNone );
    if ( iSDResult )
        {
        iRecAlgMgrObserver->InitRecognizerBEComplete( KErrNotSupported );
        return;
        }
    else if ( iSIResult )
        {
        // Call RecoHwDevice
        if ( ( iSIActiveGrammars.Count() == 0 )  || ( iSIModelBank == NULL ) )
            {
            // Guess it is ok to leave with KErrNotReady if all necessary 
            iRecAlgMgrObserver->InitRecognizerBEComplete( KErrNotReady );
            return;
            }
        else
            {
            if ( iCombineNeeded )
                {
                delete iCombinedGrammar;
                iCombinedGrammar = NULL;

                TRAPD( error, iRecAlgMgrObserver->CombineGrammarL( iSIActiveGrammars, iBlackList ) );  
                if ( error != KErrNone )
                    {
                    iRecAlgMgrObserver->InitRecognizerBEComplete( error );
                    }
                // Wait for the callback from grammar compilation!
                return;
                }
            // Call reco HW device
            TRAP( error, iRecoHw->InitRecognizerBEL( iCombinedGrammar->Des(), *iSIModelBank ) );
            if ( error != KErrNone )
                {
                iRecAlgMgrObserver->InitRecognizerBEComplete( error );
                }
            }
        }
    else
        {
        error = KErrArgument;
        }
    
    if ( error == KErrNone )
        {
        iBEState = EProcessing;
        }
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::LoadGrammarL
// Receives the load grammar request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::LoadGrammarL( const CSIGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::LoadGrammarL()" );

    if ( IsActive() )
        {
        // Callback before the request function returns
        User::Leave( KErrServerBusy );
        return;
        }
    
    // Save arguments and set code
    iSIGrammar = &aGrammar;
    iRequestFunction = KLoadGrammar;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::LoadGrammarL
// Receives the load grammar request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::LoadGrammarL( const CSICompiledGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::LoadGrammarL()" );
    
    if ( IsActive() )
        {
        // Callback before the request function returns
        User::Leave( KErrServerBusy );
        return;
        }
    
    // Save arguments and set code
    iSICompGrammar = &aGrammar; 
/*    __ASSERT_DEBUG( iSDGrammar == NULL, User::Panic( KPanicDescriptor, KErrCorrupt ) );
    __ASSERT_DEBUG( iSIGrammar == NULL, User::Panic( KPanicDescriptor, KErrCorrupt ) );*/
    iRequestFunction = KLoadGrammar;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleLoadGrammar
// Asynchronous handler for Load Grammar request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleLoadGrammar()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleLoadGrammar()" );

    TInt error( KErrNone );

    if ( !iInitialized )
        {
        iRecAlgMgrObserver->LoadGrammarComplete( KErrNotReady );
        return;
        }

    if ( iSICompGrammar )
        {

        if ( iSICompGrammar->Count() < 1 )
            {
            iRecAlgMgrObserver->LoadGrammarComplete( KErrArgument );
            return;
            }

        error = iSIActiveGrammars.Append( iSICompGrammar );

        iSICompGrammar = NULL;

        // Mark that there is need to recombine grammars
        iCombineNeeded = ETrue;

        iRecAlgMgrObserver->LoadGrammarComplete( error );
 
        }
    else
        {
        iRecAlgMgrObserver->LoadGrammarComplete( KErrNotSupported );
        }
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::UnloadGrammarL
// Receives the unload grammar request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::UnloadGrammarL( const CSIGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::UnloadGrammarL()" );

    if ( IsActive() )
        {
        // Callback before the request function returns
        User::Leave( KErrServerBusy );
        return;
        }
    
    // Save arguments and set code
    iSIGrammar = &aGrammar;
    iRequestFunction = KUnloadGrammar;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::UnloadGrammarL
// Receives the unload grammar request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::UnloadGrammarL( const CSICompiledGrammar& aGrammar )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::UnloadGrammarL()" );
    
    if ( IsActive() )
        {
        // Callback before the request function returns
        User::Leave( KErrServerBusy );
        return;
        }
    
    // Save arguments and set code
    iSICompGrammar = &aGrammar; 
    iRequestFunction = KUnloadGrammar;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleUnloadGrammar
// Asynchronous handler for Load Grammar request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleUnloadGrammar()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleUnloadGrammar()" );

    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;

    if ( !iInitialized )
        {
        iRecAlgMgrObserver->UnloadGrammarComplete( KErrNotReady );
        return;
        }

    if ( iSICompGrammar )
        {
        
        TRAPD( error, GetGrammarL( iSICompGrammar->GrammarID(), &siActiveGrammar, &siNonActiveGrammar ) ); // Leaves if not found
        if ( error != KErrNone )
            {
            iRecAlgMgrObserver->UnloadGrammarComplete( error ); 
            return;
            }
        
        if ( siActiveGrammar )
            {
            // Remove grammar from active grammars
            iSIActiveGrammars.Remove( iSIActiveGrammars.Find( siActiveGrammar ) );
            // Mark that there is need to recombine grammars
            iCombineNeeded = ETrue;
            }
        else if ( siNonActiveGrammar )
            {
            // Remove grammar from deactive
            iSIActiveGrammars.Remove( iSIActiveGrammars.Find( siActiveGrammar ) );
            // No need to set recombination flag
            }
        else
            {
            iRecAlgMgrObserver->UnloadGrammarComplete( KErrNotFound ); 
            return;
            }
        }
    else
        {
        iRecAlgMgrObserver->UnloadGrammarComplete( KErrNotSupported );
        return;
        }

    iRecAlgMgrObserver->UnloadGrammarComplete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::GetGrammarL
// Find grammar based on identifier
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::GetGrammarL( const TSIGrammarID aGrammarID, 
                                      CSICompiledGrammar** aSIActiveGrammar,
                                      CSICompiledGrammar** aSIDeActivatedGrammar )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::GetGrammarL()" );

    // Find the grammar.
    // One of the arrays should contain the specified grammar and
    // others are empty.
    TInt i( 0 );
    
    // SI active grammars.
    for ( i = 0; i < iSIActiveGrammars.Count(); i++ )
        {
        if ( aGrammarID == iSIActiveGrammars[i]->GrammarID() )
            {
            // Grammar found.
            *aSIActiveGrammar = iSIActiveGrammars[i];
            *aSIDeActivatedGrammar = NULL;
            return;
            }
        }
    
    // SI deactived grammars.
    for ( i = 0; i < iSIDeActivatedGrammars.Count(); i++ )
        {
        if ( aGrammarID == iSIDeActivatedGrammars[i]->GrammarID() )
            {
            // Grammar found.
            *aSIDeActivatedGrammar = iSIDeActivatedGrammars[i];
            *aSIActiveGrammar = NULL;
            return;
            }
        }

    User::Leave( KErrNotFound );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::ActivateGrammarL
// Receives the load grammar request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::ActivateGrammarL( const TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::ActivateGrammarL()" );

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    
    iRequestFunction = KActivateGrammar;
    iGrammarID = aGrammarID;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleActivateGrammar
// Asynchronous handler for activate grammar request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleActivateGrammar()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleActivateGrammar()" );

    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;

    if ( !iInitialized )
        {
        iRecAlgMgrObserver->ActivateGrammarComplete( KErrNotReady );
        }

    // Find the grammar
    TRAPD( error, GetGrammarL( iGrammarID, &siActiveGrammar, &siNonActiveGrammar ) ); // Leaves if not found
    if ( error != KErrNone )
        {
        iRecAlgMgrObserver->ActivateGrammarComplete( error ); 
        }

    if ( siActiveGrammar )
        {
        // Do nothing since grammar is already active
        }
    else if ( siNonActiveGrammar )
        {
        // Move item from non-active grammar list to active list
        iSIActiveGrammars.Append( siNonActiveGrammar );
        iSIDeActivatedGrammars.Remove( iSIDeActivatedGrammars.Find( siNonActiveGrammar ) );
        // Mark that there is need to recombine grammars
        iCombineNeeded = ETrue;
        }
    else
        {
        iRecAlgMgrObserver->ActivateGrammarComplete( KErrNotFound );
        return;
        }

    iRecAlgMgrObserver->ActivateGrammarComplete( KErrNone ); 
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::DeactivateGrammarL
// Receives the load grammar request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::DeactivateGrammarL( const TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::DeactivateGrammarL()" );

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    
    iRequestFunction = KDeActivateGrammar;
    iGrammarID = aGrammarID;
    Ready( KErrNone );    
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleDeActivateGrammar
// Asynchronous handler for deactivate grammar request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleDeActivateGrammar()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleDeActivateGrammar()" );

    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;

    if ( !iInitialized )
        {
        iRecAlgMgrObserver->DeActivateGrammarComplete( KErrNotReady );
        }

    // Find the grammar
    TRAPD( error, GetGrammarL( iGrammarID, &siActiveGrammar, &siNonActiveGrammar ) ); // Leaves if not found
    if ( error != KErrNone )
        {
        iRecAlgMgrObserver->DeActivateGrammarComplete( error ); 
        }

    if ( siActiveGrammar )
        {
        // Move item from active grammar list to non-active list
        iSIDeActivatedGrammars.Append( siActiveGrammar );
        iSIActiveGrammars.Remove( iSIActiveGrammars.Find( siActiveGrammar ) );
        // Mark that there is need to recombine grammars
        iCombineNeeded = ETrue;
        }
    else if ( siNonActiveGrammar )
        {
        // Do nothing since grammar is already inactive
        }
    else
        {
        iRecAlgMgrObserver->DeActivateGrammarComplete( KErrNotFound ); 
        return;
        }

    iRecAlgMgrObserver->DeActivateGrammarComplete( KErrNone ); 
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::LoadLexiconL
// Receives the load lexicon request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::LoadLexiconL( const CSILexicon& aLexicon )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::LoadLexiconL()" );
    
    // Save arguments and set code
    if ( IsActive() )
        {
        // Callback before the request function returns
        User::Leave( KErrServerBusy );
        return;
        }
    
    iSILexicon = &aLexicon;
    
    iRequestFunction = KLoadLexicon;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleLoadLexicon
// Asynchronous handler for Load Lexicon request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleLoadLexicon()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleLoadLexicon()" );

    TInt error( KErrNone );

    if ( !iInitialized )
        {
        iRecAlgMgrObserver->LoadLexiconComplete( KErrNotReady );
        return;
        }

    if ( iSDLexicon )
        {
        iSDLexicon = NULL;
        iRecAlgMgrObserver->LoadLexiconComplete( KErrNotSupported );
        return;
        }
    else if ( iSILexicon )
        {
        }
    else
        {
        error = KErrArgument;
        }
    
    // Do not load empty lexicon
    if ( iSILexicon && iSILexicon->Count() == 0 )
        {
        iSILexicon = NULL;
        iRecAlgMgrObserver->LoadLexiconComplete( KErrArgument );
        return;
        }

/*    __ASSERT_DEBUG( iSDLexicon == NULL, User::Panic( KPanicDescriptor, KErrCorrupt ) );
    __ASSERT_DEBUG( iSILexicon == NULL, User::Panic( KPanicDescriptor, KErrCorrupt ) );*/
    
    iRecAlgMgrObserver->LoadLexiconComplete( error );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::LoadModelsL
// Receives the load models request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::LoadModelsL( const CSDModelBank& aModels )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::LoadModelsL()" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        return;
        }

    // Save arguments and set code
    iSDModelBank = ( CSDModelBank* ) &aModels;
    iRequestFunction = KLoadModels;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::LoadModelsL
// Receives the load models request and start the asynchronous handler.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::LoadModelsL( const CSIModelBank& aModels )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::LoadModelsL()" );

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    // Save arguments and set code
    iSIModelBank = ( CSIModelBank* ) &aModels;
    iRequestFunction = KLoadModels;
    Ready( KErrNone );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleLoadModels
// Asynchronous handler for Load Models request.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleLoadModels()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleLoadModels()" );

    TInt error( KErrNone );

    if ( !iInitialized )
        {
        iSDModelBank = NULL;
        iSIModelBank = NULL;
        iRecAlgMgrObserver->LoadModelsComplete( KErrNotReady );
        return;
        }

    if ( iSDModelBank )
        {
        // SD modelbank not supported
        iSDModelBank = NULL;
        iRecAlgMgrObserver->LoadModelsComplete( KErrNotSupported );
        return;
        }
    else if ( iSIModelBank )
        {
        }
    else
        {
        error = KErrArgument;
        }

    if ( iSIModelBank && iSIModelBank->Count() < 1 )
        {
        iSIModelBank = NULL;
        iRecAlgMgrObserver->LoadModelsComplete( KErrArgument );
        return;
        }

/*    __ASSERT_DEBUG( iSDModelBank == NULL, User::Panic( KPanicDescriptor, KErrCorrupt ) );
    __ASSERT_DEBUG( iSIModelBank == NULL, User::Panic( KPanicDescriptor, KErrCorrupt ) );*/
    
    iRecAlgMgrObserver->LoadModelsComplete( error );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::GetEnginePropertiesL
// Retreive the properties of the underlying speech recognition engine.
// An array of values corresponding to the querried identifiers will be populated.
// The function may leave with KErrArgument.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::GetEnginePropertiesL( const RArray<TInt>& aPropertyId,
                                              RArray<TInt>& aPropertyValue )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::GetEnginePropertiesL()" );
    
    for ( TInt index = 0; index < aPropertyId.Count(); index++ )
        {
        switch( aPropertyId[index] )
            {
            case KDevASRSendFeatures:
                if ( iFeatures )
                    {
                    aPropertyValue[index] = 1;
                    }
                else
                    {
                    aPropertyValue[index] = 0;
                    }
                break;
                
            case KDevASRAdaptation:
                if ( iAdaptation )
                    {
                    aPropertyValue[index] = 1;
                    }
                else
                    {
                    aPropertyValue[index] = 0;
                    }
                break;
                
            default:
                break;
                
            }
        }
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::LoadEnginePropertiesL
// Loads properties to engine, invalid IDs are neglected.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::LoadEnginePropertiesL( const RArray<TInt>& aParameterId,
                                               const RArray<TInt>& aParameterValue )
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::LoadEnginePropertiesL()" );
    
    for ( TInt index = 0; index < aParameterId.Count(); index++ )
        {
        switch( aParameterId[index] )
            {
            case KDevASRSendFeatures:
                if ( aParameterValue[index] == 0 )
                    {
                    iFeatures = EFalse;
                    }
                else
                    {
                    iFeatures = ETrue;
                    }
                break;
                
            case KDevASRAdaptation:
                if ( aParameterValue[index] == 0 )
                    {
                    iAdaptation = EFalse;
                    }
                else
                    {
                    iAdaptation = ETrue;
                    }
                break;
                
            default:
                break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::SendSpeechData
// Sends utterance data to recognizer.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::SendSpeechData( TPtrC8& aBuffer, TBool aEnd )
    {
    iRecoHw->SendSpeechData( aBuffer, aEnd );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::StartRecSession
// Signal the start of a recognition sesion.
// -----------------------------------------------------------------------------
//
TInt CRecognitionAlgMgr::StartRecSession( TRecognizerMode aMode )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::StartRecSession()" );

    TInt error( KErrNone );

    if ( !iInitialized )
        {
        return KErrNotReady; 
        }

    if ( aMode == ESiRecognition )
        {
        iMode = aMode;
        }
    else if ( aMode == ESiRecognitionSpeechInput )
        {
        iMode = aMode;
        }
    else
        {
        return KErrNotSupported;
        }

    return error;
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::StartRecognitionL
// Start recognition.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::StartRecognitionL()
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::StartRecognitionL()" );

    if ( !iInitialized )
        {
        return;
        }

    // Reset the result IDs
    iNBestList.Reset();
    iScores.Reset();

    iRecoHw->StartRecognitionL( iNBestList, iScores );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::EndRecSession
// Signal the end of a recognition sesion.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::EndRecSession()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::EndRecSession()" );
  
    // Reset grammars etc.
    iSIActiveGrammars.Reset();
	iSIDeActivatedGrammars.Reset();
    iNBestList.Reset();
    iScores.Reset();
    iBlackList.ResetAndDestroy();

    delete iCombinedGrammar;
    iCombinedGrammar = NULL;

    iCombineNeeded = ETrue;
    iSDModelBank = NULL;
    iSIModelBank = NULL;
    iSDGrammar = NULL;
    iSIGrammar = NULL;
    iSICompGrammar = NULL;
    iSDCompGrammar = NULL;
    iSDLexicon = NULL;
    iSILexicon = NULL;
    iFEState = EIdle;
    iBEState = EIdle;
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::Cancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::Cancel()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::Cancel()" );

    iRecoHw->StopRecognition();

    iFEState = EIdle;
    iBEState = EIdle;

    iAdaptHw->CancelAdaptation();
    
    CActive::Cancel();
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::DoCancel
// Cancels the current and any on going requests/tasks.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::DoCancel()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::DoCancel()" );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::GetUtteranceDuration
// Get start and end point of utterance.
// -----------------------------------------------------------------------------
//
TBool CRecognitionAlgMgr::GetUtteranceDuration( TUint32& /*aStartFrame*/,
                                                TUint32& /*aEndFrame*/,
                                                TReal& /*aFrameLength*/ )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::GetUtteranceDuration()" );

    return EFalse;
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::UnloadRule
// Unloads one rule from specified grammar.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::UnloadRule()" );

    if ( !IsActive() )
        {
        iGrammarID = aGrammarID;
        iRuleID = aRuleID;
        iRequestFunction = KUnloadRule;
        Ready( KErrNone );
        }
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::HandleUnloadRule
// Asynchrounous handler for rule unload.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::HandleUnloadRule()
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::HandleUnloadRule()" );

    TInt error = KErrNone;
    CSICompiledGrammar* siActiveGrammar = NULL;
    CSICompiledGrammar* siNonActiveGrammar = NULL;
    CSICompiledGrammar* siGrammar = NULL;

    if ( !iInitialized )
        {
        iRecAlgMgrObserver->UnloadRuleComplete( KErrNotReady );
        }
    
    TRAP( error, GetGrammarL( iGrammarID, &siActiveGrammar, &siNonActiveGrammar ) );
    if ( error != KErrNone )
        {
        iRecAlgMgrObserver->UnloadRuleComplete( error );
        return;
        }

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
        iRecAlgMgrObserver->UnloadRuleComplete( KErrNotFound );
        return;
        }

    // Check that given rule id can be found from the given grammar
    if ( siGrammar->Find( iRuleID ) < 0 )
        {
        iRecAlgMgrObserver->UnloadRuleComplete( KErrNotFound );
        return;
        }

    // Fetch the actual rule object based on rule id
    TRAP_IGNORE( 
    
        CSIRule& blackRule = siGrammar->AtL( siGrammar->Find( iRuleID ) );
     
        // Loop through the rule variants in blacklisted rule
        for ( TInt i = 0; i < blackRule.Count(); i++ )
            {
            // AtL should not leave in any case since i is every time
            // within range 0..Count()-1
            CSIRuleVariant& blackVariant = blackRule.AtL( i );
    
            // Create rule variant info
            TSIRuleVariantInfo* info = new TSIRuleVariantInfo( iGrammarID, iRuleID, blackVariant.RuleVariantID() );
            if ( !info )
                {
                iRecAlgMgrObserver->UnloadRuleComplete( KErrNoMemory );
                return;
                }
    
            iBlackList.Append( info );
            }
        ); // TRAP_IGNORE

    // Mark that new combination is needed
    // iCombineNeeded = ETrue;

    iRecAlgMgrObserver->UnloadRuleComplete( error );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::CombineComplete
// Notifies that grammar combination has been done.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::CombineComplete( HBufC8* aResult, TInt aError )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::CombineComplete()" );

    if ( aError != KErrNone )
        {
        iRecAlgMgrObserver->InitRecognizerBEComplete( aError );
        return;
        }

    delete iCombinedGrammar;
    iCombinedGrammar = NULL;
    iCombinedGrammar = aResult;

    // Do the rest of BE initialization
    TRAPD( error, iRecoHw->InitRecognizerBEL( iCombinedGrammar->Des(), *iSIModelBank ) );
    if ( error != KErrNone )
        {
        iRecAlgMgrObserver->InitRecognizerBEComplete( error );
        }
    }


// -----------------------------------------------------------------------------
// MASRAdaptationHwDeviceObserver MIXIN impl begins
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MaahdAdaptationComplete
// Model adaptation has been completed.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MaahdAdaptationComplete( TInt aError )
    {
//    iAdaptHw->ClearAdaptation();
    iRecAlgMgrObserver->AdaptComplete( aError );
    }


// -----------------------------------------------------------------------------
// MASRAdaptationHwDeviceObserver MIXIN impl begins
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// MASRSRecogHwDeviceObserver MIXIN impl begins
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MarhdoRequestSpeechData
// Forwards request to upper layers.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MarhdoRequestSpeechData()
    {
    iRecAlgMgrObserver->RequestSpeechData();
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MarhdoInitializationComplete
// Initialization of Hw Device has been completed.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MarhdoInitializationComplete( TInt aError )
    {
    if ( aError == KErrNone )
        {
        iInitialized = ETrue;
        }
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MarhdoRecognizerFEComplete
// FE has been initialized.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MarhdoInitRecognizerFEComplete( TInt aError )
    {
    RUBY_DEBUG1( "CRecognitionAlgMgr::MarhdoInitRecognizerFEComplete(), Status: %d", aError );

    if ( aError == KErrNone )
        {
        iFEState = EIdle;
        }
    iRecAlgMgrObserver->InitFEComplete( aError );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MarhdoRecognizerBEComplete
// BE has been initialized.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MarhdoInitRecognizerBEComplete( TInt aError )
    {
    RUBY_DEBUG1( "CRecognitionAlgMgr::MarhdoInitRecognizerBEComplete(), Status: %d", aError );

    if ( aError == KErrNone )
        {
        iBEState = EIdle;
        }
    iRecAlgMgrObserver->InitRecognizerBEComplete( aError );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MarhdoRecognitionComplete
// Recognition has been completed
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MarhdoRecognitionComplete( TInt aError )
    {
    RUBY_DEBUG1( "CRecognitionAlgMgr::MarhdoRecognitionComplete, Status: %d", aError );

    if ( iCombinedGrammar == NULL )
        {
        RUBY_DEBUG0( "CRecognitionAlgMgr::HandleMarhdoRecognitionComplete, Cancelled" );
        delete iAdaptationData;
        iAdaptationData = NULL;
        iRecAlgMgrObserver->RecognitionComplete( KErrCancel );
        return;
        }

     // Resolve result
    iRecAlgMgrObserver->ResolveResult( iNBestList, *iSIResult, iSIActiveGrammars, 
                                       iCombinedGrammar->Des()/*, *iSIModelBank*/ );


    RUBY_DEBUG1( "CRecognitionAlgMgr::MarhdoRecognitionComplete, result resolved, result count: %d", iSIResult->Count() );

    TRAP_IGNORE(

        // Put scores in place
        for ( TInt i = 0; i < iSIResult->Count(); i++ )
            {
            // Should not leave since we are every time within range
            CSIResult& result = iSIResult->AtL( i );

            if ( i < iScores.Count() )
                {
                result.SetScore( iScores[i] );
                }
            else
                {
                result.SetScore( 0 );
                }
            }
            
        ); // TRAP_IGNORE
    
    iScores.Reset();

    RUBY_DEBUG0( "CRecognitionAlgMgr::MarhdoRecognitionComplete, scores in place" );

    // Put adaptation data in place if needed
    if ( iAdaptation )
        {
        iSIResult->SetAdaptationData( iAdaptationData );
        // Result set gains ownership, no need to delete
        iAdaptationData = NULL;
        RUBY_DEBUG0( "CRecognitionAlgMgr::MarhdoRecognitionComplete, adaptation data in place" );
        }

    iBEState = EIdle;
    iFEState = EIdle;
    iRecAlgMgrObserver->RecognitionComplete( aError );

    RUBY_DEBUG0( "CRecognitionAlgMgr::MarhdoRecognitionComplete, ok, event sent" );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MarhdoEouDetected
// Called when EOU has been detected, no need to send samples anymore.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MarhdoEouDetected( TInt aError )
    {
    RUBY_DEBUG1( "CRecognitionAlgMgr::MarhdoEouDetected(), Status: %d", aError );

    iRecAlgMgrObserver->EouDetected( aError );
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::MarhdoFeatureVector
// Feature vector sent by AsrRecognitionHwDevice
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::MarhdoFeatureVector( const TDesC8& aFV, 
                                              TInt32 aSNR, 
                                              TInt32 aPosition )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::FeatureVectorDataRcvd()" );

    // Collect data to one buffer
    if ( iAdaptation )
        {
        if ( aPosition == 0 )
            {
            // Clear previous data if exists
            if ( iAdaptationData )
                {
                delete iAdaptationData;
                iAdaptationData = NULL;
                }
            // Copy data to new buffer
            iAdaptationData = HBufC8::New( aFV.Length() );
            if ( !iAdaptationData )
                {
                return;
                }
            TPtr8 bufferPtr( iAdaptationData->Des() );
            bufferPtr.Copy( aFV.Ptr(), aFV.Length() );
            }
        else if ( aPosition > 0 )
            {
            if ( iAdaptationData != NULL )
                {
                // Append data to existing buffer
                iAdaptationData = iAdaptationData->ReAlloc( iAdaptationData->Length() + aFV.Length() );
                // Check that we got enough memory for new buffer
                if ( iAdaptationData == NULL )
                    {
                    return;
                    }
                TPtr8 bufferPtr( iAdaptationData->Des() );
                bufferPtr.Append( aFV );
                }
            else
                {
                // Do a new buffer since there is no existing one
                iAdaptationData = HBufC8::New( aFV.Length() );
                if ( !iAdaptationData )
                    {
                    return;
                    }
                TPtr8 bufferPtr( iAdaptationData->Des() );
                bufferPtr.Copy( aFV.Ptr(), aFV.Length() );
                }
            }
        }
    // Same callback to client if needed
    if ( iFeatures )
        {
        iRecAlgMgrObserver->FeatureVectorDataRcvd( aFV, aSNR, aPosition );
        }
    }


// -----------------------------------------------------------------------------
// MASRSRecogHwDeviceObserver MIXIN impl ends
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Active object implementation begins
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::RunL
// Invoke by the active scheduler when a request completes.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::RunL()
    {
    RUBY_DEBUG_BLOCK( "CRecognitionAlgMgr::RunL()" );

    switch(iRequestFunction)
        {
        case KInitFrontEnd:
            HandleInitFrontEnd();
            break;
            
        case KInitRecognizerBE:
            HandleInitRecognizerBE();
            break;
            
        case KLoadGrammar:
            HandleLoadGrammar();
            break;
            
        case KLoadLexicon:
            HandleLoadLexicon();
            break;
            
        case KLoadModels:
            HandleLoadModels();
            break;
            
        case KActivateGrammar:
            HandleActivateGrammar();
            break;

        case KDeActivateGrammar:
            HandleDeActivateGrammar();
            break;

        case KUnloadRule:
            HandleUnloadRule();
            break;

        case KUnloadGrammar:
            HandleUnloadGrammar();
            break;
        
        default:
            break;
        };
    
    }


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::Ready
// Utility function to post a request complete
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::Ready( const TInt aStatus )
    {
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, aStatus );
    SetActive();
    }


// -----------------------------------------------------------------------------
// Active object impl ends
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::AlgorithmState
// Utility function to print out the state of the algorithms.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::AlgorithmState()
    {
#ifdef _DEBUG
    switch ( iFEState )
        {
        case EIdle:
            RUBY_DEBUG1( "Recognition algorithm is: [%d] - EIdle", iFEState );
            break;
            
        case EProcessing:
            RUBY_DEBUG1( "Recognition algorithm is: [%d] - EProcessing", iFEState );
            break;
            
        case ECancel:
            RUBY_DEBUG1( "Recognition algorithm is: [%d] - ECancel", iFEState );
            break;
            
        default:
            break;
        };
#endif
    }
    
// -----------------------------------------------------------------------------
// CRecognitionAlgMgr::SetRejection
// Set rejection threshold.
// -----------------------------------------------------------------------------
//
void CRecognitionAlgMgr::SetRejection( TUint32 aRejection )
    {
    RUBY_DEBUG0( "CRecognitionAlgMgr::SetRejection()" );

    iRecoHw->SetRejection( aRejection );
    }
