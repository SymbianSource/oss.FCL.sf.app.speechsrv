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
#include "vuicabortstate.h"
#include "vuicexitstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuictoneplayer.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CAbortState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAbortState* CAbortState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CAbortState* self = new (ELeave) CAbortState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CAbortState::~CAbortState()
    {
    RUBY_DEBUG0( "CAbortState::~CAbortState START" );
    
    RUBY_DEBUG0( "CAbortState::~CAbortState EXIT" );
    }
    
// ---------------------------------------------------------
// CAbortState::HandleEventL
// ---------------------------------------------------------
//       
void CAbortState::HandleEventL( TInt /*aEvent*/ )
    {
    RUBY_DEBUG_BLOCK( "CAbortState::HandleEventL" );

    CState* nextState = CExitState::NewL( DataStorage(), UiModel() );

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CAbortState::ExecuteL
// ---------------------------------------------------------
//       
void CAbortState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CAbortState::ExecuteL" );

    DataStorage().TonePlayer()->PlayTone( EAvkonSIDNameDiallerAbortTone );
    }

// ---------------------------------------------------------
// CAbortState::CAbortState
// ---------------------------------------------------------
//              
CAbortState::CAbortState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel )
    {
    }
    
// End of File

