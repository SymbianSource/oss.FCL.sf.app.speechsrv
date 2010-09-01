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

#include "vuicstate.h"
#include "vuicexecutestate.h"
#include "vuicexitstate.h"
#include "vuicabortstate.h"

#include "vuicdatastorage.h"
#include "vuicuimodel.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuicglobalnote.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CExecuteState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExecuteState* CExecuteState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CExecuteState* self = new (ELeave) CExecuteState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CExecuteState::~CExecuteState()
    {
    RUBY_DEBUG0( "CExecuteState::~CExecuteState START" );
    
    RUBY_DEBUG0( "CExecuteState::~CExecuteState EXIT" );
    }
    
// ---------------------------------------------------------
// CExecuteState::HandleEventL
// ---------------------------------------------------------
//       
void CExecuteState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CExecuteState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case EShortKeypress:
        case ELongKeypress:
        case EEndCallKeypress:
        
            // Do nothing
            break;
            
        default:
        
            nextState = CExitState::NewL( DataStorage(), UiModel() );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CExecuteState::ExecuteL
// ---------------------------------------------------------
//       
void CExecuteState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CExecuteState::ExecuteL" );
            
    // Activate the selected voice tag
    MNssRecognitionHandler::TNssRecognitionResult result =
        DataStorage().RecognitionHandler()->SelectTagL( DataStorage().Tag() );

    if ( result != MNssRecognitionHandler::EVasErrorNone )
        {
        if ( !DataStorage().DeviceLockMode() )
            {
            UiModel().Note().DisplayGlobalNoteL( CGlobalNote::ESystemError );
            }
        }

    HandleEventL( KErrNone );
    }

// ---------------------------------------------------------
// CExecuteState::CExecuteState
// ---------------------------------------------------------
//              
CExecuteState::CExecuteState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel )
    {
    }
    
// End of File

