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
#include <avkon.hrh>

#include "vuicstate.h"
#include "vuictutorialstate.h"
#include "vuicinitstate.h"
#include "vuicresultsstate.h"
#include "vuicverificationinitstate.h"
#include "vuicadaptstate.h"
#include "vuicexitstate.h"
#include "vuicabortstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuictoneplayer.h"
#include "vuictutorial.h"

#include "rubydebug.h"

// Constants
    
// -----------------------------------------------------------------------------
// CTutorialState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTutorialState* CTutorialState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CTutorialState* self = new (ELeave) CTutorialState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CTutorialState::~CTutorialState()
    {
    RUBY_DEBUG0( "CTutorialState::~CTutorialState START" );
    
    DataStorage().Tutorial()->DestroyMessage();
    
    RUBY_DEBUG0( "CTutorialState::~CTutorialState EXIT" );
    }
    
// ---------------------------------------------------------
// CTutorialState::HandleEventL
// ---------------------------------------------------------
//       
void CTutorialState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CTutorialState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            if ( iCurrentTutorialState == CTutorial::ETryState )
                {
                nextState = CInitState::NewL( DataStorage(), UiModel() );
                }
            else if ( iCurrentTutorialState == CTutorial::EBestState )
                {
                if ( DataStorage().VerificationMode() != EVoice )
                    {
                    nextState = CResultsState::NewL( DataStorage(), UiModel() );
                    }
                else
                    {
                    nextState = CVerificationInitState::NewL( DataStorage(), UiModel() );
                    }
                }
            else if ( iCurrentTutorialState == CTutorial::EActivationState )
                {
                nextState = CAdaptState::NewL( DataStorage(), UiModel() );
                }
            else if ( iInternalState == EStarted )
                {
                iInternalState = EInitialized;
                nextState = this;            
                }
            else if ( iInternalState == EInitialized )
                {
                iInternalState = ECompleted;
                }
            else
                {
                nextState = this;
                }
            break;
            
        case KErrCancel:
        
            nextState = CExitState::NewL( DataStorage(), UiModel() );
            break;
            
        case EShortKeypress:
        case ELongKeypress:
        case EEndCallKeypress:
        
            // Do nothing
            break;
            
        default:
        
            nextState = CAbortState::NewL( DataStorage(), UiModel() );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CTutorialState::ExecuteL
// ---------------------------------------------------------
//
void CTutorialState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CTutorialState::ExecuteL" );
    
    iCurrentTutorialState = DataStorage().Tutorial()->GetTutorialState();
    
    if ( iCurrentTutorialState == CTutorial::ECommandState )
        {
        if ( iInternalState == ENotStarted )
            {
            // Initialize start tone
            DataStorage().TonePlayer()->InitToneL( EAvkonSIDNameDiallerStartTone );
            iInternalState = EStarted;
            }
        else
            {
            // Play the starting tone
            DataStorage().TonePlayer()->PlayTone( EAvkonSIDNameDiallerStartTone );
            }                
        }
        
    if ( iInternalState != EStarted )
        {
        BringToForeground();

        TInt returnValue = DataStorage().Tutorial()->ShowMessageL( DataStorage().DeviceLockMode() );
        
        if ( returnValue != KErrGeneral )
            {
            HandleEventL( returnValue );
            }
        }
    }

// ---------------------------------------------------------
// CTutorialState::CTutorialState
// ---------------------------------------------------------
//              
CTutorialState::CTutorialState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iCurrentTutorialState( CTutorial::EIntroState ),
   iInternalState( ENotStarted )
    {
    }

// ---------------------------------------------------------
// CTutorialState::ConstructL
// ---------------------------------------------------------
//           
void CTutorialState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CTutorialState::ConstructL" );
    
    CState::ConstructL();
    
    DataStorage().Tutorial()->CreateMessageL();
    }
    
// End of File

