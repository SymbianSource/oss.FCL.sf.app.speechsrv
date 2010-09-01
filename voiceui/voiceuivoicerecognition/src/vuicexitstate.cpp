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
#include "vuicexitstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CExitState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExitState* CExitState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CExitState* self = new (ELeave) CExitState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CExitState::~CExitState()
    {
    RUBY_DEBUG0( "CExitState::~CExitState START" );
        
    RUBY_DEBUG0( "CExitState::~CExitState EXIT" );
    }
    
// ---------------------------------------------------------
// CExitState::HandleEventL
// ---------------------------------------------------------
//       
void CExitState::HandleEventL( TInt /*aEvent*/ )
    {
    RUBY_DEBUG_BLOCK( "CExitState::HandleEventL" );
    }

// ---------------------------------------------------------
// CExitState::ExecuteL
// ---------------------------------------------------------
//       
void CExitState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CExitState::ExecuteL" );

    DataStorage().VoiceRecognitionImpl()->Exit();
    }

// ---------------------------------------------------------
// CExitState::CExitState
// ---------------------------------------------------------
//              
CExitState::CExitState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel )
    {
    }
    
// End of File

