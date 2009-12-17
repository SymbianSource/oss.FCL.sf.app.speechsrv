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


#ifndef NSSVASCTRAININGACTIONTRACKER_H
#define NSSVASCTRAININGACTIONTRACKER_H

#include <e32base.h>

/**
 * An interface for the objects to be notifiable by the CNssTrainingActionTracker
 */
class MDelayedNotifiable
    {
    public:
        virtual void RunBufferedActionsL() = 0;
    };

/**
 * Implements the buffer timeout logic specific to the speech item 
 * trainer. Is notified about the speech item trainer action requests,
 * and decides when to actually fire the buffered actions
 */
NONSHARABLE_CLASS( CNssTrainingActionTracker ) : public CTimer
    {
    public:
        static CNssTrainingActionTracker* NewL( MDelayedNotifiable& aNotifiable );
        
        /**
         * Is called by the client to notify tracker about the request to
         * perform a potentially bufferable action. It is up to tracker
         * when to stop recording the action requests and fire the 
         * delayed actions via MDelayedNotifiable::DoBufferedActionsL
         * 
         * DoBufferedActionsL is always called asynchronously. Even if tracker
         * decides to fire the delayable actions immediately
         */
        void ActionRequestedL();
        
        ~CNssTrainingActionTracker();
    
    protected:
        
        // Fired when timer is expired
        void RunL();
    
    private:
        CNssTrainingActionTracker( MDelayedNotifiable& aNotifiable );
    
        void ConstructL();
        
        /**
         * static callback required by CAsyncCallBack* iAsyncCallback
         * @param pX A pointer to this object
         */        
        static TInt FireActionsImmediately( TAny* pX );
        
        /**
         * Call back the host
         */
        void DoFireActionsL();
        
        /**
         * Another action requested. Start waiting [again]
         */
        void RestartTimer();
        
    private:
        // Observer
        MDelayedNotifiable& iHost;
        
        // Utility for firing the action immediately, but asynchronously
        CAsyncCallBack* iAsyncCallback;
        
        // ETrue when tracker has some actions to fire in the queue
        TBool iUnfiredActions;
    };

#endif // NSSVASCTRAININGACTIONTRACKER_H

// End of file
