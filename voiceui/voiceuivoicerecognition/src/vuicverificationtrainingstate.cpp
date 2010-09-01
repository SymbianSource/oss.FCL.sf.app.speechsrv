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
* Description:  
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <centralrepository.h>

#include <nssvasctrainingparameters.h>

#include <srsfdomaincrkeys.h>

#include <vuivoicerecognition.rsg>

#include "vuicstate.h"
#include "vuicverificationtrainingstate.h"
#include "vuictutorialstate.h"
#include "vuicerrorstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"

#include "rubydebug.h"

// Constants

static const TInt KCommandIds[] = 
    {
    R_QAN_VC_VERIFICATION_SELECT,
    R_QAN_VC_VERIFICATION_OTHER,
    R_QAN_VC_VERIFICATION_CANCEL
    };
    
// -----------------------------------------------------------------------------
// CVerificationTrainingState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVerificationTrainingState* CVerificationTrainingState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CVerificationTrainingState* self = new (ELeave) CVerificationTrainingState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CVerificationTrainingState::~CVerificationTrainingState()
    {
    RUBY_DEBUG0( "CVerificationTrainingState::~CVerificationTrainingState START" );
    
    delete iVasDbManager;
    iContextManager = NULL;
    iTagManager = NULL;
    
    if ( iTags )
        {
        iTags->ResetAndDestroy();
        }
    delete iTags;
    
    delete iTrainingParameters;
    
    SaveLanguage();
    
    RUBY_DEBUG0( "CVerificationTrainingState::~CVerificationTrainingState EXIT" );
    }
    
// ---------------------------------------------------------
// CVerificationTrainingState::HandleEventL
// ---------------------------------------------------------
//       
void CVerificationTrainingState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CVerificationTrainingState::HandleEventL" );

    CState* nextState = NULL;
    
    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == EStarted )
                {
                nextState = this;
                iInternalState = EContextDeleted;
                }
            else if ( iInternalState == EContextDeleted )
                {
                nextState = this;
                iInternalState = EContextReady;
                }
            else if ( iInternalState == EContextReady )
                {
                --iExpectedCallbacks;
                
                if ( iExpectedCallbacks == KErrNone )
                    {
                    nextState = this;
                    iInternalState = ECommandsTrained;
                    }
                }
            else
                {
                --iExpectedCallbacks;
                
                if ( iExpectedCallbacks == KErrNone )
                    {
                    nextState = CTutorialState::NewL( DataStorage(), UiModel() );
                    iInternalState = ECommandsSaved;
                    }
                }
            break;
            
        default:
            
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CVerificationTrainingState::ExecuteL
// ---------------------------------------------------------
//       
void CVerificationTrainingState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationTrainingState::ExecuteL" );
    
    if ( iInternalState == ENotStarted )
        {
        iInternalState = EStarted;
        
        // Delete context if already exists
        if ( DataStorage().Context()->ContextName() == KVoiceVerificationContext )
            {
            if ( iContextManager->DeleteContext( DataStorage().VoiceRecognitionImpl(),
                                                 DataStorage().Context() ) != KErrNone )
                {
                RUBY_ERROR0( "CVerificationTrainingState::ExecuteL - DeleteContext call failed" );
                HandleEventL( KErrGeneral );
                }
            }
        else 
            {
            HandleEventL( KErrNone );
            }
        }
    else if ( iInternalState == EContextDeleted )
        {
        // Create new context
        MNssContext* vasContext = iContextManager->CreateContextL();
        DataStorage().SetContext( vasContext );
            
        vasContext->SetNameL( KVoiceVerificationContext );
        vasContext->SetGlobal( EFalse );
    
        if ( iContextManager->SaveContext( DataStorage().VoiceRecognitionImpl(),
                                           vasContext ) != KErrNone )
            {
            RUBY_ERROR0( "CVerificationTrainingState::ExecuteL - SaveContext call failed" );
            HandleEventL( KErrGeneral );
            }
        }
    else if ( iInternalState == EContextReady )
        { 
        TrainTagsL();        
        }
     else if ( iInternalState == ECommandsTrained )
        {
        SaveTagsL();
        }
    }

// ---------------------------------------------------------
// CVerificationTrainingState::CVerificationTrainingState
// ---------------------------------------------------------
//              
CVerificationTrainingState::CVerificationTrainingState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iInternalState( ENotStarted ), iTags(),
   iExpectedCallbacks( 0 )
    {
    }

// ---------------------------------------------------------
// CVerificationTrainingState::ConstructL
// ---------------------------------------------------------
//           
void CVerificationTrainingState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationTrainingState::ConstructL" );
    
    CState::ConstructL();
    
    iVasDbManager = CNssVASDBMgr::NewL();
    iVasDbManager->InitializeL();
    iTagManager = iVasDbManager->GetTagMgr();
    iContextManager = iVasDbManager->GetContextMgr();   
    }
    
// ---------------------------------------------------------
// CVerificationTrainingState::CreateTagsL
// ---------------------------------------------------------
//  
void CVerificationTrainingState::CreateTagsL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationTrainingState::CreateTagsL" );
    
    iTags = new ( ELeave ) RPointerArray<MNssTag>;
 
    for ( int i = 0; i < KVerificationCommands; ++i )
        {
        iTags->Append( iTagManager->CreateTagL( DataStorage().Context() ) );
        }
    }
    
// ---------------------------------------------------------
// CVerificationTrainingState::ConstructTrainingParametersL
// ---------------------------------------------------------
//  
void CVerificationTrainingState::ConstructTrainingParametersL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationTrainingState::ConstructTrainingParametersL" );
    
    RArray<TLanguage>* languageArray = new ( ELeave ) RArray<TLanguage>;
    CleanupDeletePushL( languageArray ); // Protect [allocated on the heap] array itself
    CleanupClosePushL( *languageArray );  // Protect the array elements
    
    // Always generate a pronunciation in UI language
    User::LeaveIfError( languageArray->Append( User::Language() ) );
    
    iTrainingParameters = CNssTrainingParameters::NewL();
    
    iTrainingParameters->SetLanguages( languageArray );
    
    // For array elements pushed via CleanupClosePushL
    CleanupStack::Pop( languageArray );
    CleanupStack::Pop( languageArray ); 
    }
    
// ---------------------------------------------------------
// CVerificationTrainingState::TrainTagsL
// ---------------------------------------------------------
//  
void CVerificationTrainingState::TrainTagsL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationTrainingState::TrainTagsL" );
    
    CreateTagsL();
    
    ConstructTrainingParametersL();
    
    iExpectedCallbacks = KVerificationCommands;
    
    for ( int i = 0; i < KVerificationCommands; ++i )
        {
        HBufC* text = StringLoader::LoadLC( KCommandIds[ i ] );    
        
        MNssSpeechItem* speechItem = static_cast<MNssTag*>( (*iTags)[ i ] )->SpeechItem();
        speechItem->SetTextL( *text );
        
        CleanupStack::PopAndDestroy( text );
        
        MNssRRD* rrd = static_cast<MNssTag*>( (*iTags)[ i ] )->RRD();
        
        rrd->IntArray()->AppendL( i );
            
        MNssSpeechItem::TNssSpeechItemResult result = 
		    speechItem->TrainTextL( DataStorage().VoiceRecognitionImpl(),
		                            iTrainingParameters );
    
        if ( result != MNssSpeechItem::EVasErrorNone )
            {
            RUBY_ERROR1( "CVerificationTrainingState::TrainTagsL - Training start failed with the nss code [%d]", result );

            HandleEventL( KErrGeneral );
            }
        }
    }
    
// ---------------------------------------------------------
// CVerificationTrainingState::SaveTagsL
// ---------------------------------------------------------
//  
void CVerificationTrainingState::SaveTagsL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationTrainingState::SaveTagsL" );
    
    iExpectedCallbacks = KVerificationCommands;
    
    for ( int i = 0; i < KVerificationCommands; ++i )
        {
        TInt result = 
            iTagManager->SaveTag( DataStorage().VoiceRecognitionImpl(),
                                  static_cast<MNssTag*>( (*iTags)[ i ] ) );
        if ( result != KErrNone ) 
            {
            RUBY_ERROR1( "CVerificationTrainingState::SaveTagsL - Savetag start failed with the nss code [%d]", result );

            HandleEventL( KErrGeneral );
            }
        }
    }
        
// ---------------------------------------------------------
// CVerificationTrainingState::SaveLanguage
// ---------------------------------------------------------
//           
void CVerificationTrainingState::SaveLanguage()
    {
    RUBY_DEBUG0( "CVerificationTrainingState::SaveLanguage START" );
    
    TRAP_IGNORE(
        CRepository* client = CRepository::NewLC( KCRUidSRSFSettings );    
        client->Set( KSRSFVoiceUiLanguage, User::Language() );        
        CleanupStack::PopAndDestroy( client ) );
    
    RUBY_DEBUG0( "CVerificationTrainingState::SaveLanguage EXIT" );
    }
    
// End of File

