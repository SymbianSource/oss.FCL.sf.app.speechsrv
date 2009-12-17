/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <StringLoader.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuicstate.h"
#include "vuicerrorstate.h"
#include "vuictutorialstate.h"
#include "vuicexitstate.h"

#include "vuicdatastorage.h"
#include "vuicuimodel.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuictoneplayer.h"
#include "vuicttsplayer.h"
#include "vuicglobalnote.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CErrorState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CErrorState* CErrorState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel, TInt aError )
    {
    CErrorState* self = new (ELeave) CErrorState( aDataStorage, aUiModel, aError );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CErrorState::~CErrorState()
    {
    RUBY_DEBUG0( "CErrorState::~CErrorState START" );
    
    DataStorage().TtsPlayer()->Stop();
    
    RUBY_DEBUG0( "CErrorState::~CErrorState EXIT" );
    }
    
// ---------------------------------------------------------
// CErrorState::HandleEventL
// ---------------------------------------------------------
//       
void CErrorState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CErrorState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
            
            if ( iInternalState == EStarted )
                {
                iInternalState = EInitialized;
                nextState = this;
                }
            break;
        
        case KErrNoMatch:
        
            nextState = CTutorialState::NewL( DataStorage(), UiModel() );
            break;
            
        default:
        
            nextState = CExitState::NewL( DataStorage(), UiModel() );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CErrorState::ExecuteL
// ---------------------------------------------------------
//       
void CErrorState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CErrorState::ExecuteL" );
    
    if ( iInternalState == ENotStarted )
        {
        if ( !DataStorage().DeviceLockMode() )
            {
            UiModel().Note().DisplayGlobalNoteL( NoteType() );
            }
        
        iInternalState = EStarted;
        
        TInt toneType = ToneType();
    
        if ( toneType == EVoiceNoMatchFound )
            {
            HBufC* ttsText = StringLoader::LoadLC( R_TEXT_VOICE_NO_MATCHES );

            DataStorage().TtsPlayer()->PlayL( *ttsText );
            
            CleanupStack::PopAndDestroy( ttsText );
            }
        else if ( toneType != KErrNone )
            {
            DataStorage().SetPlayDuration( 0 );
            DataStorage().TonePlayer()->PlayTone( toneType );
            }
        else
            {
            DataStorage().SetPlayDuration( 0 );
            HandleEventL( toneType );
            }
        }
    else if ( iInternalState == EInitialized )
        {
        TInt timeoutValue = DataStorage().PlayDuration().Int64();
        if ( timeoutValue < KErrorMessageTimeoutMicroseconds )
            {
            timeoutValue = KErrorMessageTimeoutMicroseconds;
            }
        
        StartTimerL( *this, timeoutValue, timeoutValue );
        iInternalState = EReady;
        }    
    }

// ---------------------------------------------------------
// CErrorState::CErrorState
// ---------------------------------------------------------
//              
CErrorState::CErrorState( CDataStorage& aDataStorage, CUiModel& aUiModel, TInt aError )
 : CState( aDataStorage, aUiModel ), iInternalState( ENotStarted ), iError( aError )
    {
    }
    
// ---------------------------------------------------------
// CErrorState::DoTimedEventL
// ---------------------------------------------------------
//      
void CErrorState::DoTimedEventL()
    {
    RUBY_DEBUG_BLOCK( "CErrorState::DoTimedEventL" );
    
    // Dismiss error message dialog
    UiModel().Note().DismissGlobalNoteL();
    
    DataStorage().Tutorial()->ErrorState();
    
    HandleEventL( iError );
    }
    
// ---------------------------------------------------------
// CErrorState::NoteType
// ---------------------------------------------------------
//      
CGlobalNote::TGlobalNoteState CErrorState::NoteType()
    {
    RUBY_DEBUG0( "CErrorState::NoteType START" );
    
    CGlobalNote::TGlobalNoteState type;
    
    switch( iError )
        {
        case KErrNoMatch:
        case KErrNoResults:
            {
            type = CGlobalNote::ENoMatchPermanent;
            break;
            }
        case KErrCallInProgress:
            {
            type = CGlobalNote::ECallInProgress;
            break;
            }
        case KErrNoContacts:
            {
            type = CGlobalNote::ENoTag;
            break;
            }
        case KErrInit:
            {
            type = CGlobalNote::ESystemError;
            break;
            }
        default:
            {
            type = CGlobalNote::ENotRecognizedPermanent;
            break;
            }
        }
        
    RUBY_DEBUG0( "CErrorState::NoteType EXIT" );
    
    return type;
    }
    
// ---------------------------------------------------------
// CErrorState::ToneType
// ---------------------------------------------------------
//      
TInt CErrorState::ToneType()
    {
    RUBY_DEBUG0( "CErrorState::ToneType START" );
    
    TInt type;
    
    switch( iError )
        {
        case KErrNoMatch:
        case KErrNoResults:
            {
            // if TTS is used
            if ( DataStorage().SynthesizerMode() != KErrNone ||
                 DataStorage().DeviceLockMode() )
                {
                type = EVoiceNoMatchFound;
                }
            else
                {
                type = EAvkonSIDNameDiallerAbortTone;
                }
            break;
            }
        case KErrCallInProgress:
            {
            // No tone should be played
            type = KErrNone;
            break;
            }
        default:
            {
            type = EAvkonSIDNameDiallerErrorTone;
            break;
            }
        }  
    
    RUBY_DEBUG0( "CErrorState::ToneType EXIT" );
    
    return type;
    }
    
// End of File

