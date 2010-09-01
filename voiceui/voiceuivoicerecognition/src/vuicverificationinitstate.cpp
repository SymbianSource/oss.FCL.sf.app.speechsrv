/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "vuicverificationinitstate.h"
#include "vuicverificationstate.h"
#include "vuicabortstate.h"
#include "vuicerrorstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuictoneplayer.h"

#include "rubydebug.h"

// CONSTANTS

    
// -----------------------------------------------------------------------------
// CVerificationInitState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVerificationInitState* CVerificationInitState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CVerificationInitState* self = new (ELeave) CVerificationInitState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CVerificationInitState::~CVerificationInitState()
    {
    RUBY_DEBUG0( "CVerificationInitState::~CVerificationInitState START" );
    
    if ( iInternalState != EReady )
        {
        DataStorage().VerificationRecognitionHandler()->Cancel();
        }
    
    RUBY_DEBUG0( "CVerificationInitState::~CVerificationInitState EXIT" );
    }
    
// ---------------------------------------------------------
// CVerificationInitState::HandleEventL
// ---------------------------------------------------------
//       
void CVerificationInitState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CVerificationInitState::HandleEventL" );

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
            else
                {
                nextState = CVerificationState::NewL( DataStorage(), UiModel() );
                iInternalState = EReady;
                }
            break;
        
        case KErrInit:
        
            nextState = CErrorState::NewL( DataStorage(), UiModel(), aEvent );
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
// CVerificationInitState::ExecuteL
// ---------------------------------------------------------
//       
void CVerificationInitState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationInitState::ExecuteL" );
    
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

        // Initialize start tone
        DataStorage().TonePlayer()->InitToneL( EAvkonSIDNameDiallerStartTone );

        // Initialize recognition
        RecognizeInitL();
        }
    }

// ---------------------------------------------------------
// CVerificationInitState::CVerificationInitState
// ---------------------------------------------------------
//              
CVerificationInitState::CVerificationInitState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iInternalState( ENotStarted )
    {
    }
    
// ---------------------------------------------------------
// CVerificationInitState::InitializeL
// ---------------------------------------------------------
//
void CVerificationInitState::InitializeL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationInitState::InitializeL" );
        
    CNssRecognitionHandlerBuilder* builder = CNssRecognitionHandlerBuilder::NewL();
    DataStorage().SetVerificationRecognitionHandlerBuilder( builder );    
    builder->InitializeL();
   
    DataStorage().SetVerificationRecognitionHandler( builder->GetRecognitionHandler() );
    }

#ifndef __WINS__
#ifdef __FULLDUPLEX_CHANGE
// ---------------------------------------------------------
// CVerificationInitState::StartSamplingL
// ---------------------------------------------------------
//
void CVerificationInitState::StartSamplingL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationInitState::StartSamplingL" );
    
    MNssRecognitionHandler* handler = DataStorage().VerificationRecognitionHandler(); 
    User::LeaveIfError(
        handler->PreStartSampling( DataStorage().VoiceRecognitionImpl() ) );
    }
#endif // __FULLDUPLEX_CHANGE
#endif // __WINS__ 

// ---------------------------------------------------------
// CVerificationInitState::RecognizeInitL
// ---------------------------------------------------------
//
void CVerificationInitState::RecognizeInitL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationInitState::RecognizeInitL" );
    
    // Content of this array may not be destroyed here. In case of a Leave content
    // will be destroyed in DataStorage.
    CArrayPtrFlat<MNssContext>* array = new (ELeave) CArrayPtrFlat<MNssContext>( 1 );
    CleanupDeletePushL( array );
        
    array->AppendL( DataStorage().Context() );

    MNssRecognitionHandler* handler = DataStorage().VerificationRecognitionHandler();
    User::LeaveIfError(
        handler->RecognizeInitL( DataStorage().VoiceRecognitionImpl(), *array, 1 ) );
                                 
    CleanupStack::PopAndDestroy( array );
    }
    
// End of File

