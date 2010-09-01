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
* Description:  Implements the buffer timeout logic specific to the speech item 
*               trainer. Is notified about the speech item trainer action requests,
*               and decides when to actually fire the buffered actions
*
*/


#include "nssvasctrainingactiontracker.h"
#include "rubydebug.h"

const TInt KDelayInMicroseconds = 500000;


CNssTrainingActionTracker* CNssTrainingActionTracker::NewL( MDelayedNotifiable& aNotifiable )
    {
    CNssTrainingActionTracker* self = new (ELeave) CNssTrainingActionTracker( aNotifiable );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNssTrainingActionTracker::CNssTrainingActionTracker( MDelayedNotifiable& aNotifiable ) :
    CTimer( EPriorityStandard ), iHost( aNotifiable )
    {
    // empty
    }
    
CNssTrainingActionTracker::~CNssTrainingActionTracker()
    {
    delete iAsyncCallback;
    }    
        
/**
 * Is called by the client to notify tracker about the request to
 * perform a potentially bufferable action. It is up to tracker
 * when to stop recording the action requests and fire the 
 * delayed actions via MDelayedNotifiable::DoBufferedActionsL
 * 
 * DoBufferedActionsL is always called asynchronously. Even if tracker
 * decides to fire the delayable actions immediately
 */
void CNssTrainingActionTracker::ActionRequestedL()
    {
    RUBY_DEBUG_BLOCK("");
    iUnfiredActions = ETrue;
    if ( !IsActive() )
        {
        RUBY_DEBUG0( "The first event in a sequence" );
        iAsyncCallback->CallBack();
        }
    RestartTimer();
    }
    
void CNssTrainingActionTracker::RestartTimer()
    {
    if( IsActive() )
        {
        Cancel();
        }
    After( TTimeIntervalMicroSeconds32( KDelayInMicroseconds ) );
    }
    
    
void CNssTrainingActionTracker::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    iAsyncCallback = new (ELeave) CAsyncCallBack( TCallBack(FireActionsImmediately, this ), 
                                                  CActive::EPriorityStandard );
    }

void CNssTrainingActionTracker::RunL()
    {
    // timeout happened
    if( iUnfiredActions )
        {
        RUBY_DEBUG0("Unfired actions in the buffeer");
        DoFireActionsL();
        }
    }
    
TInt CNssTrainingActionTracker::FireActionsImmediately( TAny* pX )
    {
    TRAPD( err, static_cast<CNssTrainingActionTracker*>(pX)->DoFireActionsL() );
    return err;
    }
    
    
void CNssTrainingActionTracker::DoFireActionsL()
    {
    RUBY_DEBUG_BLOCK("");
    // Whatever is requested after RunBufferedActionsL or *during* RunBufferedActionsL
    // has to be fired later
    iUnfiredActions = EFalse;
    iHost.RunBufferedActionsL();
    }
    
// End of file
