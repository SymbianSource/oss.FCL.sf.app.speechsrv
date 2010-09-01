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


#include "rubydebug.h"
#include "asyncworker.h"

// async worker panic code
_LIT( KAsyncWorkerPanic, "AWRK" );

CAsyncWorker* CAsyncWorker::NewLC()
	{
	CAsyncWorker* self = new (ELeave) CAsyncWorker;
	CleanupStack::PushL( self );
	return self;
	}
	
CAsyncWorker* CAsyncWorker::NewL()
	{
	CAsyncWorker* self = CAsyncWorker::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
CAsyncWorker::~CAsyncWorker()
	{
	delete iWaiter;  // if any
	}

/**
* Pauses the execution until StopAsyncWaitingL is called
* Leaves if StopAsyncWaitingL is called with the negative error code
* @leave KErrNotReady if another waiting is still in progress
* @leave Leaves with the error code passed to StopAsyncWaitingL
*/
void CAsyncWorker::WaitForAsyncCallbackL()
	{
	/** @todo Change to RUBY_ASSERT_ALWAYS */
	RUBY_DEBUG_BLOCKL( "CAsyncWorker::WaitForAsyncCallbackL" );
	if( iWaiter )
		{
		RUBY_ERROR0( "CAsyncWorker::WaitForAsyncCallbackL Already waiting" );
		}
	
	__ASSERT_ALWAYS( !iWaiter, User::Leave( KErrNotReady ) );
	iErr = KErrNone;
    iWaiter = new (ELeave) CActiveSchedulerWait;
    iWaiter->Start();
    delete iWaiter;
    iWaiter = NULL;
    User::LeaveIfError( iErr );
	}

/** 
* Resumes the process paused by WaitForAsyncCallbackL
* @param aErr Result code to pass to the 
*        WaitForAsyncCallbackL error handling. Note, that 
*        WaitForAsyncCallbackL leaves only if aErr is negative
* @return KErrNotReady if no waiting is in progress. Otherwise KErrNone
*/
TInt CAsyncWorker::StopAsyncWaiting( TInt aErr )
	{
    RUBY_DEBUG1( "CAsyncWorker::StopAsyncWaitingL aErr [%d] ", aErr ); 
    iErr = aErr;   
    /** @todo Change to RUBY_ASSERT_ALWAYS */
	if( !iWaiter )
		{
		RUBY_ERROR0( "CAsyncWorker::WaitForAsyncCallbackL Not waiting" );
		}
	if( !iWaiter)
		{
		return KErrNotReady;
		}
	
    iWaiter->AsyncStop();
    return KErrNone;
	}
	
/**
* Same as StopAsyncWaiting, but panics with AWRK <error code> instead 
* of returning the error code
* @see StopAsyncWaiting
*/
void CAsyncWorker::StopAsyncWaitingOrPanic( TInt aErr )
	{
	TInt err = StopAsyncWaiting( aErr );
	if( err != KErrNone ) 
		{
		User::Panic( KAsyncWorkerPanic, err );
		}
	}
	
//End of file
