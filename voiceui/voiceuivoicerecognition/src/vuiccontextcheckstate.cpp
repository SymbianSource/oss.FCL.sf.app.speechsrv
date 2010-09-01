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
#include <centralrepository.h>

#include <srsfdomaincrkeys.h>

#include "vuicstate.h"
#include "vuiccontextcheckstate.h"
#include "vuicverificationtrainingstate.h"
#include "vuictutorialstate.h"
#include "vuicexitstate.h"
#include "vuicerrorstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"

#include "rubydebug.h"

static const TInt KUndefinedLanguage = -1;
    
// -----------------------------------------------------------------------------
// CContextCheckState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContextCheckState* CContextCheckState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CContextCheckState* self = new (ELeave) CContextCheckState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CContextCheckState::~CContextCheckState()
    {
    RUBY_DEBUG0( "CContextCheckState::~CContextCheckState START" );
    
    delete iVasDbManager;
    iContextManager = NULL;
    iTagManager = NULL;
    
    RUBY_DEBUG0( "CContextCheckState::~CContextCheckState EXIT" );
    }
    
// ---------------------------------------------------------
// CContextCheckState::HandleEventL
// ---------------------------------------------------------
//       
void CContextCheckState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CContextCheckState::HandleEventL" );

    CState* nextState = NULL;
    
    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == EStarted )
                {
                iInternalState = ECommandsReady;
                nextState = this;
                }
            else if ( iInternalState == ECommandsReady )
                {
                iInternalState = EDialReady;
                nextState = this;
                }
            else if ( iInternalState == EDialReady )
                {
                iInternalState = EVerificationReady;
                nextState = this;
                }
            else
                {
                nextState = CTutorialState::NewL( DataStorage(), UiModel() );
                }
            break;
            
        case KErrNoVerificationTrained:
        
            nextState = CVerificationTrainingState::NewL( DataStorage(), UiModel() );
            break;
            
        case KErrNotReady:
        
            nextState = CExitState::NewL( DataStorage(), UiModel() );
            break;
            
        default:
            
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CContextCheckState::ExecuteL
// ---------------------------------------------------------
//       
void CContextCheckState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CContextCheckState::ExecuteL" );

    if ( iInternalState == ENotStarted )
        {
        iInternalState = EStarted;
        
        // Get voice command context
        if ( iContextManager->GetContext( DataStorage().VoiceRecognitionImpl(),
                                          KVoiceCommandContext ) != KErrNone )
            {
            HandleEventL( KErrNotReady );
            }
        }
    else if ( iInternalState == ECommandsReady )
        {
        // Get voice command context tag count
        iTagCount += iTagManager->TagCount( DataStorage().Context() );
        
        // Get voice dial context
        if ( iContextManager->GetContext( DataStorage().VoiceRecognitionImpl(),
                                          KVoiceDialContext ) != KErrNone )
            {
            HandleEventL( KErrNotReady );
            }
        }
    else if ( iInternalState == EDialReady )
        {
        // Get voice dial context tag count
        iTagCount += iTagManager->TagCount( DataStorage().Context() );
        
        // Check if any tags were found
        if ( !iTagCount )
            {
            HandleEventL( KErrNoContacts );
            }
            
        // Get voice verification context
        if ( iContextManager->GetContext( DataStorage().VoiceRecognitionImpl(),
                                          KVoiceVerificationContext ) != KErrNone )
            {
            HandleEventL( KErrNoVerificationTrained );
            }
        }
    else if ( iInternalState == EVerificationReady )
        {
        // Get voice verification context tag count
        iTagCount = iTagManager->TagCount( DataStorage().Context() );
        
        // Check if any tags were found or has UI language changed
        if ( !iTagCount || LanguageChangedL() )
            {
            HandleEventL( KErrNoVerificationTrained );
            }
        else
            {
            HandleEventL( KErrNone );
            }
        }
    }

// ---------------------------------------------------------
// CContextCheckState::CContextCheckState
// ---------------------------------------------------------
//              
CContextCheckState::CContextCheckState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iInternalState( ENotStarted ),
   iTagCount( 0 )
    {
    }

// ---------------------------------------------------------
// CContextCheckState::ConstructL
// ---------------------------------------------------------
//           
void CContextCheckState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CContextCheckState::ConstructL" );
    
    CState::ConstructL();
    
    iVasDbManager = CNssVASDBMgr::NewL();
    iVasDbManager->InitializeL();
    iTagManager = iVasDbManager->GetTagMgr();
    iContextManager = iVasDbManager->GetContextMgr();   
    }
    
// ---------------------------------------------------------
// CContextCheckState::LanguageChangedL
// ---------------------------------------------------------
//           
TBool CContextCheckState::LanguageChangedL()
    {
    RUBY_DEBUG_BLOCK( "CContextCheckState::LanguageChangedL" );
    
    TBool changed = EFalse;
    TInt language = KUndefinedLanguage;
    
    CRepository* client = CRepository::NewLC( KCRUidSRSFSettings );
    client->Get( KSRSFVoiceUiLanguage, language );
    
    if ( language != User::Language() )
        {
        client->Set( KSRSFVoiceUiLanguage, KUndefinedLanguage );
        changed = ETrue;
        }
        
    CleanupStack::PopAndDestroy( client );
    
    return changed;
    }
    
// End of File

