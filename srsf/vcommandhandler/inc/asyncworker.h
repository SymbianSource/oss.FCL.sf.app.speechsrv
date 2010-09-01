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
* Description:  Class that is able to pause the execution until the 
*                asynchronous call resumes it. Knows how to leave the
*                resumed function with the error code supplied by the
*                resuming asynchronous call
*
*/


 
#ifndef ASYNCWORKER_H
#define ASYNCWORKER_H

class CActiveSchedulerWait;

/**
 * Is able to pause the execution until the 
 * asynchronous call resumes it. Knows how to leave the
 * resumed function with the error code supplied by the
 * resuming asynchronous call
 */
class CAsyncWorker : public CBase 
	{
	public:
		CAsyncWorker* NewLC();
		CAsyncWorker* NewL();
		virtual ~CAsyncWorker();
		
	public:
		/**
        * Pauses the execution until StopAsyncWaitingL is called
        * Leaves if StopAsyncWaitingL is called with the negative error code
        * @leave KErrNotReady if another waiting is still in progress
        * @leave Leaves with the error code passed to StopAsyncWaitingL
        */
        void WaitForAsyncCallbackL();
        
        /** 
        * Resumes the process paused by WaitForAsyncCallbackL
        * @param aErr Result code to pass to the 
        *        WaitForAsyncCallbackL error handling. Note, that 
        *        WaitForAsyncCallbackL leaves only if aErr is negative
        * @return KErrNotReady if no waiting is in progress. Otherwise KErrNone
        */
        TInt StopAsyncWaiting( TInt aErr );
        
        /**
		* Same as StopAsyncWaiting, but panics with AWRK <error code> instead 
		* of returning the error code
		* @see StopAsyncWaiting
		*/
        void StopAsyncWaitingOrPanic( TInt aErr );
        
	private:
		// To pass the error code from the async callback
		TInt	iErr;
		
		// Performs the waiting
		CActiveSchedulerWait* iWaiter;
	};

#endif // ASYNCWORKER_H