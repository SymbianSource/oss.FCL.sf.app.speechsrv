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
* Description:  Provides access to the VAS context
*
*/


#include "rubydebug.h"
#include "contextprovider.h"

#include <nssvasmcontextmgr.h>
#include <nssvasmcontext.h>
#include "vcommandinternalapi.h"

_LIT( KContextProviderPanic, "VCP" );

CContextProvider* CContextProvider::NewL( CNssVASDBMgr& aVasDbManager )
	{
	CContextProvider* self = CContextProvider::NewLC( aVasDbManager );
	CleanupStack::Pop( self );
	return self;
	}
	
CContextProvider* CContextProvider::NewLC( CNssVASDBMgr& aVasDbManager )
	{
	CContextProvider* self = new (ELeave) CContextProvider;
	CleanupStack::PushL( self );
	// No part of initialization can leave -> no ConstructL()
	self->iContextManager = aVasDbManager.GetContextMgr();
	return self;
	}
	
CContextProvider::~CContextProvider()
	{
	delete iSaveContextWaiter; // if any
	delete iVasContext;  // if any
	}
	
/**
* Returns the VCommand VAS context. If it doesn't exists,
* it is being created. client is responsible for the context deletion
* 
* @todo If support for different contexts is needed, 
* 1) Rename the method into GetContextLC;
* 2) add the optional
* argument "const TDesC& aContextName = KVoiceCommandContext"
*/
MNssContext* CContextProvider::GetVCommandContextLC()
	{
	RUBY_DEBUG0( "CContextProvider::GetVCommandContextL start" );
    __ASSERT_ALWAYS( !iVasContext, User::Leave( KErrNotReady ) );
    if( iContextManager->GetContextList( this ) != KErrNone )
        {
        RUBY_DEBUG0( "CContextProvider::GetVCommandContextL No context in the VAS yet. Creating one" );
        return CreateVCommandContextLC();
        }
    else
        {
        RUBY_DEBUG0( "CContextProvider::GetVCommandContextL List of contexts requested" );
        }
        TRAPD ( err, WaitForAsyncCallbackL(); );
        if ( err )
            {
            iContextManager->CancelGetContext();
            User::Leave( err );
            }
        MNssContext* result = iVasContext;
        iVasContext = NULL;        
        CleanupDeletePushL( result );
        
        RUBY_DEBUG0( "CContextProvider::GetVCommandContextL end" );
        return result;
	}

/**
* Creates VCommand context. To be called during the GetVCommandContextLC
* sequence
*/	
MNssContext* CContextProvider::CreateVCommandContextLC()
	{
	RUBY_DEBUG0( "CContextProvider::CreateVCommandContextLC start" );
	__ASSERT_ALWAYS( !iVasContext, User::Leave( KErrNotReady ) );
    iVasContext = iContextManager->CreateContextL();
    iVasContext->SetNameL( KVoiceCommandContext );
    iVasContext->SetGlobal( ETrue );
    
    if ( iContextManager->SaveContext( this, iVasContext ) != 
                            KErrNone )
        {
        RUBY_ERROR0( "CContextProvider::CreateVCommandContextLC SaveContext call failed" );
        User::Leave( KErrAbort );
        }
    else 
        {
        RUBY_DEBUG0( "CContextProvider::CreateVCommandContextLC SaveContext requested" );
        }
        
    // We cannot use the inherited WaitForAsyncCallback. 
    // It might already be waiting from GetVCommandContextLC
    /** 
     * @todo Consider transforming WaitForAsyncCallback from wait/released into 
     *        wait_counter
     */
    iSaveContextWaiter = CAsyncWorker::NewL();
    iSaveContextWaiter->WaitForAsyncCallbackL();
    delete iSaveContextWaiter;
    iSaveContextWaiter = NULL;
    MNssContext* result = iVasContext;
    iVasContext = NULL;
    CleanupDeletePushL( result );

    RUBY_DEBUG0( "CContextProvider::CreateVCommandContextLC end" );
    return result;
	}	
				
// From MNssGetContextClient
/**
* Callback to indicate GetContext successed.
* client has to delete the context after using the context.
* @since 2.0
* @param aContext
* @param aErrorCode KErrNone if getting of context was successfull
* @return None
*/
void CContextProvider::GetContextCompleted( MNssContext* /*aContext*/,
                                            TInt /*aErrorCode*/ )
	{
	RUBY_ERROR0( "CContextProvider::GetContextCompleted Should never be called" );
	__ASSERT_ALWAYS( EFalse, User::Panic( KContextProviderPanic, KErrNotSupported ) );
	}

/**
* Callback to indicate GetContext successed.
* client has to ResetAndDestroy() after using the list.
* @since 2.0
* @param aContextList A list of contexts. 
* @param aErrorCode KErrNone if getting of context list was successfull
* @return None
*/
void CContextProvider::GetContextListCompleted(
    MNssContextListArray *aContextList, TInt aErrorCode )
	{
	// No single L function called => TRAPD( DoGetContextListCompletedL() ) not needed
    RUBY_DEBUG0( "CContextProvider::GetContextListCompleted start" );
    
    if( aErrorCode == KErrNone )
        {
        TInt err = KErrNone;

        if ( iVasContext )
            {
            RUBY_ERROR0( "CContextProvider::GetContextListCompleted iVasContext is not NULL" );
            err = KErrAlreadyExists;
            }
       
        if( !aContextList )
            {
            RUBY_ERROR0( "CContextProvider::GetContextListCompleted empty context list" );
            err = KErrArgument;
            }
        
        if( err == KErrNone )  // guarding conditions are ok
            {
            RUBY_DEBUG1( "CContextProvider::GetContextListCompleted Iterating over [%d] contexts",
                        aContextList->Count() );
            for( TInt i ( 0 ); i < aContextList->Count(); i++ ) 
                {
                if ( aContextList->At(i)->ContextName() == KVoiceCommandContext )
                    {
                    iVasContext = aContextList->At( i );
                    }
                else
                    {
                    delete aContextList->At( i );
                    }
                }  // for
            aContextList->Reset();
            delete aContextList;
        
        	if( !iVasContext ) 
    	        {
    	        RUBY_DEBUG0( "CContextProvider::GetContextListCompleted COMMAND context not found" );
    	        TRAP( err,
    	                iVasContext = CreateVCommandContextLC();
    	        		CleanupStack::Pop( iVasContext );
    	        		);
    	        if( err != KErrNone )
    	        	{
    	        	RUBY_ERROR1( "CContextProvider::GetContextListCompleted \
    	        				 Failed to create the VCommandContext", err );
    	        	}
    	        }
            }  // if guarding conditions are ok
            
        if( err != KErrNone )
        	{
        	RUBY_ERROR1( "CContextProvider::GetContextListCompleted failed with [%d]", err );
        	// Whenever error is returned, post-WaitForAsynchCallbackL code has no chance
            // to clean the iVasContext;
            delete iVasContext;
            iVasContext = NULL;
        	}
            
        StopAsyncWaitingOrPanic( err );
        }
        
    else // aErrorCode is not KErrNone
        {
        RUBY_ERROR1( "CContextProvider::GetContextCompleted error. Error code [%d] ", aErrorCode );
        
        // Whenever error is returned, post-WaitForAsynchCallbackL code has no chance
        // to clean the iVasContext;
        delete iVasContext;
        iVasContext = NULL;
        StopAsyncWaitingOrPanic( aErrorCode );       
        }
    
    RUBY_DEBUG0( "CContextProvider::GetContextListCompleted end" );     
   	}

// From MNssSaveContextClient

/**
* Callback to indicate SaveContext successed.
* @since 2.0
* @param aErrorCode KErrNone if saving of context was successfull
* @return None
*/
void CContextProvider::SaveContextCompleted( TInt aErrorCode )
	{
	RUBY_DEBUG0( "CContextProvider::SaveContextCompleted start" );
	
	if( aErrorCode == KErrNone )
	    {
	    iSaveContextWaiter->StopAsyncWaitingOrPanic( KErrNone );    
	    }
    else
        {
        RUBY_ERROR1( "CContextProvider::SaveContextCompleted error. Error code [%d] ", aErrorCode );
        
        // Whenever error is returned, post-WaitForAsynchCallbackL code has no chance
        // to clean the iVasContext;
        delete iVasContext;
        iVasContext = NULL;
        iSaveContextWaiter->StopAsyncWaitingOrPanic( aErrorCode );        
        }
    
    RUBY_DEBUG0( "CContextProvider::SaveContextCompleted end" );
	}

//End of file
