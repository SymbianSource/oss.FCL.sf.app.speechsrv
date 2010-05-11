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
* Description:  Array of tags and corresponding spoken texts
*
*/


#include "rubydebug.h"
#include "tagnameset.h"
#include <nssvasmspeechitem.h>
#include <nssvasmtag.h>
#include <nssvasmtagmgr.h>

_LIT( KTagNameSetPanic, "TNS" );

CTagNameSet* CTagNameSet::NewLC()
    {
    CTagNameSet* self = new (ELeave) CTagNameSet;
    CleanupStack::PushL( self );
    return self;
    }
    
CTagNameSet::~CTagNameSet()
    {
    iTags.ResetAndDestroy();
    iTexts.ResetAndDestroy();
    }
    
        
/**
* Adds a tag-text pair to be trained
* @param aTag tag to be trained and saved. This objects takes ownership
*        on the aTag
* @param aTrainingText text to be recognized. This object doesn't
*        take ownership on aTrainingText, but makes an own copy
*/
void CTagNameSet::AppendL( const MNssTag* aTag, const TDesC& aTrainingText )
    {
    HBufC* text = aTrainingText.AllocL();
    iTexts.Append( text );
    iTags.Append( aTag );    
    }

/**
* Train and save to VAS all the stored tags. It is not specified what happens
* to VAS if this method leaves
* @param aParams
* @param aTagMgr
* @param aIgnoreErrors If ETrue, doesn't fail the whole operation if some
*        individual trainings/savings failed. Trains and saves as much as
*        possible
*/
void CTagNameSet::TrainAndSaveL( const CNssTrainingParameters& aParams, 
                                 MNssTagMgr& aTagMgr, TBool aIgnoreErrors )
    {
    RUBY_DEBUG_BLOCK( "CTrainingPack::TrainAndSaveL" );
    iIgnoreErrors = aIgnoreErrors;
    RUBY_DEBUG1( "CTrainingPack::TrainAndSaveL Processing [%d] pairs", iTags.Count() );
    
    TrainTagsL( aParams );
    SaveTagsL( aTagMgr );
    }
    
/**
* Remove all the stored tags from VAS. It is not specified what happens
* to VAS if this method leaves
* @param aTagMgr
* @param aIgnoreErrors If ETrue, doesn't fail the whole operation if some
*        individual removals failed. Removes as many tags as
*        possible
*/                            
void CTagNameSet::UntrainL( MNssTagMgr& aTagMgr, TBool aIgnoreErrors )
    {
    RUBY_DEBUG_BLOCK("");
    iIgnoreErrors = aIgnoreErrors;
    RUBY_DEBUG1( "Removing [%d] tags from VAS", iTags.Count() );
    SetCallbacksExpected( iTags.Count() );
    
    if( iCallbacksExpected > 0 )
        {
       	for( TInt i = 0; i < iTags.Count(); i++ )
        	{
            DeleteTagL( aTagMgr, *iTags[i] );
    	    }  // for
        WaitUntilOperationsAreCompletedL();
        }
    }

/**
* @return Number of tag-name pairs stored
*/
TInt CTagNameSet::Count() const
    {
    return iTags.Count();
    }

/**
* @param aIndex must be >= 0 and < Count()
* @return Voice tag at the given position
* @panic KErrArgument if aIndex is out of bounds
*/    
const MNssTag& CTagNameSet::TagAt( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iTags.Count(), 
                    User::Panic( KTagNameSetPanic, KErrArgument ));
    return *iTags[aIndex];
    }

/**
* @param aIndex must be >= 0 and < Count()
* @return Recognition text at the given position
* @panic KErrArgument if aIndex is out of bounds
*/        
const TDesC& CTagNameSet::TextAt( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iTexts.Count(), 
                    User::Panic( KTagNameSetPanic, KErrArgument ));
    return *iTexts[aIndex];
    }

/**
* Train all the stored tag-name pairs. Synchronous
*/
void CTagNameSet::TrainTagsL( const CNssTrainingParameters& aParams )
    {
    __ASSERT_ALWAYS( iCallbacksExpected == 0, User::Leave( KErrInUse) );
    SetCallbacksExpected( Count() );
    if( iCallbacksExpected > 0 )
        {
        for( TInt i = 0; i < Count(); i++ )
            {
            TrainTagL( TagAt( i ), aParams, TextAt( i ) );
            }  // for
        WaitUntilOperationsAreCompletedL();
        }  // if
    }


/**
* Trains the voice tag. Asynchronous
* @leave VAS error code
* @todo Is it ok to mix system-wide codes with the TNssSpeechItemResult codes
*/
void CTagNameSet::TrainTagL( const MNssTag& aTag, 
		const CNssTrainingParameters& aParams, const TDesC& aSpokenText )
	{
	RUBY_DEBUG_BLOCKL( "CTagNameSet::TrainTagL" );
    RUBY_DEBUG1( "Training for spoken text [%S]", &aSpokenText )

    MNssSpeechItem* speechItem = const_cast<MNssTag&>(aTag).SpeechItem();
    speechItem->SetTextL( aSpokenText );
    MNssSpeechItem::TNssSpeechItemResult nssErr = 
		speechItem->TrainTextL( this, const_cast<CNssTrainingParameters*>( &aParams ) );
    
    if ( nssErr != MNssSpeechItem::EVasErrorNone )
        {
        RUBY_ERROR1( "CTagNameSet::TrainTagL training start failed with the nss code [%d]", 
                    nssErr );
        if( iIgnoreErrors )
            {
            RUBY_DEBUG0( "CTagNameSet::TrainTagL ignoring error" );
            DecreaseCallbacksExpected();
            }
        else 
            {
            // Wait for started operations before leaving
            DecreaseCallbacksExpected();
            WaitUntilOperationsAreCompletedL();
            
            switch( nssErr )
                {
                case MNssSpeechItem::EVasInvalidParameter:
                    User::Leave( KErrArgument );
                case MNssSpeechItem::EVasTrainFailed:
                    User::Leave( KErrGeneral );
                default:
                    User::Leave( nssErr );
                } 
            }        
        }
    RUBY_DEBUG0( "CTagNameSet::TrainTagL Tag training has been initiated" );
	}			
			

/**
* Train Complete Event - Voice tag training completed
* @param aErrorCode KErrNone if training was successfull
*/       
void CTagNameSet::HandleTrainComplete( TInt aErrorCode )
	{
	//RUBY_DEBUG0( "Start" );
	ProcessCallback( aErrorCode );
	//RUBY_DEBUG0( "End" );
	}

/**
* Saves the trained voice tag to VAS. Synchronous
*/
void CTagNameSet::SaveTagsL( MNssTagMgr& aTagManager )
	{
	RUBY_DEBUG_BLOCK( "CTagSaver::SaveTagsL" );
	__ASSERT_ALWAYS( iCallbacksExpected == 0, User::Leave( KErrInUse) );
    SetCallbacksExpected( Count() );
    if( iCallbacksExpected > 0 )
        {
        for( TInt i = 0; i < Count(); i++ )
            {
            SaveTagL( aTagManager, TagAt( i ) );
            }  // for
            
        WaitUntilOperationsAreCompletedL();
        }  // if
	}
	

/**
* Saves the trained voice tag to VAS. 
*/
void CTagNameSet::SaveTagL( MNssTagMgr& aTagManager, const MNssTag& aTag )
	{
	RUBY_DEBUG_BLOCKL( "CTagNameSet::SaveTagL" );
    TInt nssErr = 
            aTagManager.SaveTag( this, const_cast<MNssTag*>( &aTag ) );
    if( nssErr != KErrNone ) 
        {
        RUBY_ERROR1( "CTagNameSet::SaveTagL Savetag start failed with the nss code [%d]", nssErr );
        if( iIgnoreErrors )
            {
            RUBY_DEBUG0( "CTagNameSet::SaveTagL ignoring error" );
            DecreaseCallbacksExpected();
            }
        else 
            {
            // Wait for started operations before leaving
            DecreaseCallbacksExpected();
            WaitUntilOperationsAreCompletedL();
            
            User::Leave( KErrGeneral );
            }        
        }
    RUBY_DEBUG0( "CTagNameSet::SaveTagL Savetag sequence started" );

	}

/**
* Callback to indicate that SaveTag sequence has been completed
* @param aErrorCode KErrNone if saving of tag was successfull
*/
void CTagNameSet::SaveTagCompleted( TInt aErrorCode )
	{
	RUBY_DEBUG0( "Start" );
    ProcessCallback( aErrorCode );
    RUBY_DEBUG0( "End" );
    }

/**
* Deletes the voice tag from VAS. Asynchronous
* @leave VAS error codes
*/
void CTagNameSet::DeleteTagL( MNssTagMgr& aTagManager, MNssTag& aTag )
    {
    User::LeaveIfError( aTagManager.DeleteTag( this, &aTag ) );
    }

/**
* Callback to indicate a tag was deleted successfully.
* @param aErrorCode KErrNone if deletion of tag was successfull
*/
void CTagNameSet::DeleteTagCompleted( TInt aErrorCode )
    {
    RUBY_DEBUG0( "Start" );
    ProcessCallback( aErrorCode );
    RUBY_DEBUG0( "End" );
    }

/**
* Handles the common part of the callback processing
* Decrements the number of still expected callbacks and
* handles the errors according to aIgnoreErrors from the 
* public methods
*/
void CTagNameSet::ProcessCallback( TInt aErrorCode )
    {
    //RUBY_DEBUG0( "Start" );
    if( iIgnoreErrors || aErrorCode == KErrNone )
        {
        if( DecreaseCallbacksExpected() == 0 )
            {
            StopAsyncWaitingOrPanic( KErrNone );
            }  
        }
    else
        {
        RUBY_ERROR1( "Callback reported error [%d] ", aErrorCode );
        if( DecreaseCallbacksExpected() == 0 )
            {
            StopAsyncWaitingOrPanic( aErrorCode );
            }
        }
    //RUBY_DEBUG0( "End" );    
    }

/**
* Sets the number of callbacks to expect
*/
void CTagNameSet::SetCallbacksExpected (TUint aCallbacksExpected )
    {
    RUBY_DEBUG2( "Changing from [%d] to [%d]", iCallbacksExpected, aCallbacksExpected );
    iCallbacksExpected = aCallbacksExpected;
    }

/**
* Decreases the number of callbacks to expect by one
* @return Number of callbacks expected after decreasing        
*/
TUint CTagNameSet::DecreaseCallbacksExpected()
    {
    __ASSERT_ALWAYS( iCallbacksExpected > 0, User::Panic( KTagNameSetPanic, KErrNotReady ) );
    RUBY_DEBUG2( "Changing from [%d] to [%d]", iCallbacksExpected, iCallbacksExpected - 1 );
    return --iCallbacksExpected;
    }

/**
* Waits until all the expected callbacks are received
* If no callbacks are expected returns immediately
* @leave @see CAsyncWorker::WaitForAsyncCallbackL
*/
void CTagNameSet::WaitUntilOperationsAreCompletedL()
    {
    RUBY_DEBUG_BLOCK( "" );
    if( iCallbacksExpected > 0 )
        {
        RUBY_DEBUG1( "[%d] callbacks expected. Waiting for them", iCallbacksExpected );
        WaitForAsyncCallbackL();
        }
    else 
        {
        RUBY_DEBUG0( "no callbacks expected. Returning immediately" );
        }
    }

//End of file
