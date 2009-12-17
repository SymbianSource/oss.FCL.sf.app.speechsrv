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

#include <vasmbasepbkhandler.h>

#include "vuicstate.h"
#include "vuicinitstate.h"
#include "vuicrecordstate.h"
#include "vuicabortstate.h"
#include "vuicerrorstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuictoneplayer.h"
#include "vuicpropertyhandler.h"

#include "rubydebug.h"

// CONSTANTS

    
// -----------------------------------------------------------------------------
// CInitState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CInitState* CInitState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CInitState* self = new (ELeave) CInitState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CInitState::~CInitState()
    {
    RUBY_DEBUG0( "CInitState::~CInitState START" );
    
    if ( iInternalState != EReady )
        {
        DataStorage().RecognitionHandler()->Cancel();
        }
    
    RUBY_DEBUG0( "CInitState::~CInitState EXIT" );
    }
    
// ---------------------------------------------------------
// CInitState::HandleEventL
// ---------------------------------------------------------
//       
void CInitState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CInitState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == EStarted )
                {
#ifndef __WINS__                  
#ifdef __FULLDUPLEX_CHANGE
                iInternalState = ESamplingStarted;
                nextState = this;
                }
            else if ( iInternalState == ESamplingStarted )
                {
#endif // __FULLDUPLEX_CHANGE
#endif // __WINS__  
                iInternalState = EInitialized;
                }
            else if ( iInternalState == EInitialized )
                {
                iInternalState = EReady;
                nextState = this;
                }
            else
                {
                nextState = CRecordState::NewL( DataStorage(), UiModel() );
                }
            break;
        
        case KErrInit:
        case KErrNoContacts:
        
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
            break;
            
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
// CInitState::ExecuteL
// ---------------------------------------------------------
//       
void CInitState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CInitState::ExecuteL" );
    
    if ( iInternalState == ENotStarted )
        {
        // Initialize recognition system
        InitializeL();
        iInternalState = EStarted;

#ifndef __WINS__          
#ifdef __FULLDUPLEX_CHANGE
        // Initialize sampling
        StartSamplingL();
        }
    else if ( iInternalState == ESamplingStarted )
        {
#endif // __FULLDUPLEX_CHANGE
#endif // __WINS__  
        // Initialize phonebook handler
        TRAPD( error, DataStorage().PbkHandler()->InitializeL() );
        if ( error != KErrAlreadyExists )
            {
            User::LeaveIfError( error );
            }
        
        // Initialize start tone
        DataStorage().TonePlayer()->InitToneL( EAvkonSIDNameDiallerStartTone );

        // Initialize recognition
        RecognizeInitL();
        }
    else
        {
        // Play the starting tone
        DataStorage().TonePlayer()->PlayTone( EAvkonSIDNameDiallerStartTone ); 
        }
    }

// ---------------------------------------------------------
// CInitState::CInitState
// ---------------------------------------------------------
//              
CInitState::CInitState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iInternalState( ENotStarted )
    {
    }
    
// ---------------------------------------------------------
// CInitState::InitializeL
// ---------------------------------------------------------
//
void CInitState::InitializeL()
    {
    RUBY_DEBUG_BLOCK( "CInitState::InitializeL" );

    DataStorage().AccessoryButtonMonitor()->Register( DataStorage().VoiceRecognitionImpl() );

    CNssRecognitionHandlerBuilder* builder = CNssRecognitionHandlerBuilder::NewL();
    DataStorage().SetRecognitionHandlerBuilder( builder );    
    builder->InitializeL();
      
    DataStorage().SetRecognitionHandler( builder->GetRecognitionHandler() );
    }

#ifndef __WINS__      
#ifdef __FULLDUPLEX_CHANGE
// ---------------------------------------------------------
// CInitState::StartSamplingL
// ---------------------------------------------------------
//
void CInitState::StartSamplingL()
    {
    RUBY_DEBUG_BLOCK( "CInitState::StartSamplingL" );
    
    MNssRecognitionHandler* handler = DataStorage().RecognitionHandler(); 
    User::LeaveIfError(
        handler->PreStartSampling( DataStorage().VoiceRecognitionImpl() ) );
    }
#endif // __FULLDUPLEX_CHANGE
#endif // __WINS__  
        
// ---------------------------------------------------------
// CInitState::RecognizeInitL
// ---------------------------------------------------------
//
void CInitState::RecognizeInitL()
    {
    RUBY_DEBUG_BLOCK( "CInitState::RecognizeInitL" );
    
    TInt numResults = KSindMaxResults * 3;

    if ( DataStorage().VerificationMode() == EVoice )
        {
        numResults = KVerificationResults;
        }

    MNssRecognitionHandler* handler = DataStorage().RecognitionHandler();
    User::LeaveIfError(
        handler->RecognizeInitL( DataStorage().VoiceRecognitionImpl(), numResults ) );
    }
    
// End of File

