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
#include <apgtask.h>
#include <coemain.h>

#include "vuicstate.h"

#include "rubydebug.h"

// CONSTANTS
const TUid KVoiceUiUid = { 0x101F8543 };
    
// Destructor       
CState::~CState()
    {
    RUBY_DEBUG0( "CState::~CState START" );
    
    StopTimer();
    
    RUBY_DEBUG0( "CState::~CState EXIT" );
    }

// ---------------------------------------------------------
// CState::CState
// ---------------------------------------------------------
//              
CState::CState(  CDataStorage& aDataStorage, CUiModel& aUiModel )
 : iDataStorage( &aDataStorage ), iUiModel( &aUiModel )    
    {
    }
    
// ---------------------------------------------------------
// CState::ConstructL
// ---------------------------------------------------------
//           
void CState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CState::ConstructL" );
    }    

// ---------------------------------------------------------
// CState::DataStorage
// ---------------------------------------------------------
//     
CDataStorage& CState::DataStorage() const
    {
    return *iDataStorage;
    }

// ---------------------------------------------------------
// CState::UiModel
// ---------------------------------------------------------
//       
CUiModel& CState::UiModel() const
    {
    return *iUiModel;
    }
    
// ---------------------------------------------------------
// CState::StartTimerL
// Starts timer
// ---------------------------------------------------------
//
void CState::StartTimerL( CState& aEventHandler, TInt aDelay, TInt aInterval )
    {
    RUBY_DEBUG0( "CState::StartTimerL START" );

    StopTimer();

    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    TTimeIntervalMicroSeconds32 delay( aDelay );
    TTimeIntervalMicroSeconds32 interval( aInterval );
    iTimer->Start( delay, interval, TCallBack( OnTimedEvent, &aEventHandler ) );
    
    RUBY_DEBUG0( "CState::StartTimerL EXIT" );
    }
    
// ---------------------------------------------------------
// CState::StopTimer
// Stops timer
// ---------------------------------------------------------
//
void CState::StopTimer()
    {
    RUBY_DEBUG0( "CState::StopTimer START" );

    if ( iTimer )
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        delete iTimer;
        iTimer = NULL;
        }
        
    RUBY_DEBUG0( "CState::StopTimer EXIT" );
    }
    
// ---------------------------------------------------------
// CState::OnTimedEvent
// ---------------------------------------------------------
//
TInt CState::OnTimedEvent( TAny* aObject )
   {
   TRAP_IGNORE( ( (CState*) aObject )->DoTimedEventL() );
   return KErrNone;
   }
   
// ---------------------------------------------------------
// CState::DoTimedEventL
// ---------------------------------------------------------
//
void CState::DoTimedEventL()
   {   
   }

// ---------------------------------------------------------
// CState::BringToForeground
// ---------------------------------------------------------
//   
void CState::BringToForeground()
    {
    TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
    TApaTask apaTask = apaTaskList.FindApp( KVoiceUiUid );
    
    if ( apaTask.Exists() )
        {
        apaTask.BringToForeground();
        }
    }
    
// End of File

