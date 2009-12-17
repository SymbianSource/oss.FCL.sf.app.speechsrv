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
* Description:  Gets the voice tags from VAS
*
*/


#include "rubydebug.h"
#include "taggetter.h"

#include <nssvascoreconstant.h>

// For CleanupResetAndDestroy
#include <mmfcontrollerpluginresolver.h>  

// For getting tags basing on the convertion details
// KCommandIdRrdIntIndex has to be known
#include "tagcommandconverter.h"

CTagGetter* CTagGetter::NewLC()
	{
	CTagGetter* self = new (ELeave) CTagGetter;
	CleanupStack::PushL( self );
	return self;
	}
	
CTagGetter* CTagGetter::NewL()
	{
	CTagGetter* self = CTagGetter::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
/**
* Retrieves the list of tags for a given context. Synchronous.
* Leaves two objects on the cleanup stack!
* First PopAndDestroy will ResetAndDestroy content
* Second one will destroy the MNsstagListArray itself
*/
MNssTagListArray* CTagGetter::GetTagListLC2( MNssTagMgr& aTagManager, const MNssContext& aContext )
	{
	RUBY_DEBUG0( "CTagGetter::GetTagListLC2 start" );
    __ASSERT_ALWAYS( !iTagList, User::Leave( KErrNotReady ) );
    TInt nssErr = 
            aTagManager.GetTagList( this, const_cast<MNssContext*>( &aContext) );
    MNssTagListArray* result = WaitAndFormTagListLC2( nssErr );
    RUBY_DEBUG0( "CTagGetter::GetTagListLC2 end" );
    return result;
	}

/**
* Retrieves the list of tags for a given context and vcommand id. Synchronous.
* Leaves two objects on the cleanup stack!
* First PopAndDestroy will ResetAndDestroy content
* Second one will destroy the MNsstagListArray itself
*/
MNssTagListArray* CTagGetter::GetTagListLC2( MNssTagMgr& aTagManager, const MNssContext& aContext, 
                                 TInt aCommandId )
    {
    RUBY_DEBUG0( "CTagGetter::GetTagListLC2 start" );
    __ASSERT_ALWAYS( !iTagList, User::Leave( KErrNotReady ) );
    TInt nssErr = 
            aTagManager.GetTagList( this, const_cast<MNssContext*>( &aContext), 
                                    aCommandId, KCommandIdRrdIntIndex );
    MNssTagListArray* result = WaitAndFormTagListLC2( nssErr );

    RUBY_DEBUG0( "CTagGetter::GetTagListLC2 end" );
    return result;
    }
    
/** 
* After one of the GetTagListLC2 functions requested a list of 
* tags from VAS, they call this function to wait for the query
* result and process it 
* Leaves two objects on the cleanup stack!
* First PopAndDestroy will ResetAndDestroy content
* Second one will destroy the MNssTagListArray itself
* @param aNssErr Error code from the GetTagList request
*/
MNssTagListArray* CTagGetter::WaitAndFormTagListLC2( TInt aNssErr )
    {
    RUBY_DEBUG0( "CTagGetter::WaitAndFormTagListLC2 start" );
    MNssTagListArray* result = NULL;
    if( aNssErr != KErrNone ) 
        {
        RUBY_ERROR1( "CTagGetter::GetTagListLC2 GetTagList failed with nss code [%d]",
                    aNssErr );
        // Unfortunately VAS does not report if it was a real failure or just no tags
        // Return empty list in both cases
        // 1 stands for the random granularity, not for a number of elements
        result = new (ELeave) MNssTagListArray( 1 );
        }
    else 
    	{
    	// Tag list requested
    	WaitForAsyncCallbackL();
    	result = iTagList;
	    iTagList = NULL;
    	}
    CleanupDeletePushL( result );	
    CleanupResetAndDestroyPushL( *result );

    RUBY_DEBUG0( "CTagGetter::WaitAndFormTagListLC2 end" );
    return result;
    }
    
// From  MNssGetTagClient

/**
* Callback to indicate GetTag successed.
* client has to delete the tag list
* @param aTagList
* @param aErrorCode KErrNone if getting of tag list was successfull
* @return None
*/
void CTagGetter::GetTagListCompleted( MNssTagListArray* aTagList, 
                                      TInt aErrorCode )
	{
	RUBY_DEBUG0( "CTagGetter::GetTagListCompleted start" );
    
    if( aErrorCode == KErrNone )
        {
        TInt err = KErrNone;
        if( !aTagList )
            {
            RUBY_ERROR0( "CTagGetter::GetTagListCompleted NULL received" );
            err = KErrArgument;
            }
        else if( iTagList ) 
            {
            // Whenever error is returned, post-WaitForAsynchCallbackL code has no chance
            // to clean the iVasContext;
            delete iTagList;
            iTagList = NULL;
            RUBY_ERROR0( "CTagGetter::GetTagListCompleted iTagList is not empty" );
            err = KErrAlreadyExists;
            }
        else 
            {
            iTagList = aTagList;
            }
        StopAsyncWaitingOrPanic( err );        
        }
    
    else // aErrorCode is not KErrNone
        {
        RUBY_ERROR1( "CTagGetter::GetTagCompleted error. Error code[%d] ", aErrorCode );
        StopAsyncWaitingOrPanic( aErrorCode );
        }
    
    RUBY_DEBUG0( "CTagGetter::GetTagListCompleted end" );   
	}
			
//End of file
