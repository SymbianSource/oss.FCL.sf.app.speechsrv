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

#include <nssvasmadaptationitem.h>

#include "vuicstate.h"
#include "vuicadaptstate.h"
#include "vuicexecutestate.h"
#include "vuicabortstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CAdaptState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAdaptState* CAdaptState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CAdaptState* self = new (ELeave) CAdaptState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CAdaptState::~CAdaptState()
    {
    RUBY_DEBUG0( "CAdaptState::~CAdaptState START" );
    
    delete iAdaptationItem;
        
    RUBY_DEBUG0( "CAdaptState::~CAdaptState EXIT" );
    }
    
// ---------------------------------------------------------
// CAdaptState::HandleEventL
// ---------------------------------------------------------
//       
void CAdaptState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CAdaptState::HandleEventL" );

    CState* nextState = NULL;
    
    switch( aEvent )
        {
        case EShortKeypress:
        case ELongKeypress:
        case EEndCallKeypress:
        
            // Do nothing
            break;
            
        default:
            
            nextState = CExecuteState::NewL( DataStorage(), UiModel() );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CAdaptState::ExecuteL
// ---------------------------------------------------------
//       
void CAdaptState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CAdaptState::ExecuteL" );
    
    if ( DataStorage().AdaptationEnabled() )
        {
        iAdaptationItem = DataStorage().RecognitionHandler()->GetAdaptationItemL();

        iAdaptationItem->AdaptL( DataStorage().VoiceRecognitionImpl(),
                                 *DataStorage().Tag() );
        }
    else
        {
        HandleEventL( KErrNone );
        }
    }

// ---------------------------------------------------------
// CAdaptState::CAdaptState
// ---------------------------------------------------------
//              
CAdaptState::CAdaptState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel )
    {
    }
    
// End of File

