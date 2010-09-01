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


 
#ifndef TAGNAMESET_H
#define TAGNAMESET_H

#include "asyncworker.h"
#include <nssvasmtraintexteventhandler.h>
#include <nssvasmsavetagclient.h>
#include <nssvasmdeletetagclient.h>

class MNssTagMgr;
class MNssTag;
class CNssTrainingParameters;

/** 
* Array of tags and corresponding spoken texts
*/        
class CTagNameSet : public CAsyncWorker, public MNssTrainTextEventHandler, 
                    public MNssSaveTagClient, public MNssDeleteTagClient
    {
    public:
        static CTagNameSet* NewLC();
        
        ~CTagNameSet();

        /**
        * Adds a tag-text pair to be trained
        * @param aTag tag to be trained and saved. This objects takes ownership
        *        on the aTag
        * @param aTrainingText text to be recognized. This object doesn't
        *        take ownership on aTrainingText, but makes an own copy
        */
        void AppendL( const MNssTag* aTag, const TDesC& aTrainingText = KNullDesC );
        
        /**
        * Train and save to VAS all the stored tags. It is not specified what happens
        * to VAS if this method leaves
        * @param aParams
        * @param aTagMgr
        * @param aIgnoreErrors If ETrue, doesn't fail the whole operation if some
        *        individual trainings/savings failed. Trains and saves as much as
        *        possible
        */
        void TrainAndSaveL( const CNssTrainingParameters& aParams, 
                            MNssTagMgr& aTagMgr, TBool aIgnoreErrors = EFalse);

        /**
        * Remove all the stored tags from VAS. It is not specified what happens
        * to VAS if this method leaves
        * @param aTagMgr
        * @param aIgnoreErrors If ETrue, doesn't fail the whole operation if some
        *        individual removals failed. Removes as many tags as
        *        possible
        */                            
        void UntrainL( MNssTagMgr& aTagMgr, TBool aIgnoreErrors = EFalse );
        
        /**
        * @return Number of tag-name pairs stored
        */
        TInt Count() const;
        
        /**
        * @param aIndex must be >= 0 and < Count()
        * @return Voice tag at the given position
        * @panic KErrArgument if aIndex is out of bounds
        */
        const MNssTag& TagAt( TInt aIndex ) const;
        
        /**
        * @param aIndex must be >= 0 and < Count()
        * @return Recognition text at the given position
        * @panic KErrArgument if aIndex is out of bounds
        */
        const TDesC& TextAt( TInt aIndex ) const;
        
    public:
      
        // From MNssTrainTextEventHandler
        
        /**
	    * Train Complete Event - Voice tag training completed
	    * @param aErrorCode KErrNone if training was successfull
	    */       
        void HandleTrainComplete( TInt aErrorCode );
  
  public:
        
        // From MNssSaveTagClient
       
        /**
        * Callback to indicate that SaveTag sequence has been completed
        * @param aErrorCode KErrNone if saving of tag was successfull
        */
        void SaveTagCompleted( TInt aErrorCode );
       
        // From MNssDeleteTagClient
	       
	    /**
	    * Callback to indicate a tag was deleted successfully.
	    * @param aErrorCode KErrNone if deletion of tag was successfull
	    */
	    virtual void DeleteTagCompleted( TInt aErrorCode );


  private:
        
        /**
        * Train all the stored tag-name pairs. Synchronous
        */
        void TrainTagsL( const CNssTrainingParameters& aParams );

  		/**
        * Trains the voice tag. Asynchronous
        * @leave VAS error codes
        */
        void TrainTagL( const MNssTag& aTag, 
        		const CNssTrainingParameters& aParams, const TDesC& aSpokenText );

        /**
        * Saves to VAS all the stored voice tags. Synchronous
        * All the voice tags must be previously trained
        */        		
        void SaveTagsL( MNssTagMgr& aTagManager );

        /**
        * Saves the trained voice tag to VAS. 
        */
        void SaveTagL( MNssTagMgr& aTagManager, const MNssTag& aTag );
        
  		/**
        * Deletes the voice tag from VAS. Asynchronous
        * @leave VAS error codes
        */
        void DeleteTagL( MNssTagMgr& aTagManager, MNssTag& aTag );
        
        /**
        * Handles the common part of the callback processing
        * Decrements the number of still expected callbacks and
        * handles the errors according to aIgnoreErrors from the 
        * public methods
        */
        void ProcessCallback( TInt aErrorCode );
        
        /**
        * Sets the number of callbacks to expect
        */
        void SetCallbacksExpected ( TUint aCallbacksExpected );
        
        /**
        * Decreases the number of callbacks to expect by one
        * @return Number of callbacks expected after decreasing
        */
        TUint DecreaseCallbacksExpected();
        
        /**
        * Waits until all the expected callbacks are received
        * If no callbacks are expected returns immediately
        * @leave @see CAsyncWorker::WaitForAsyncCallbackL
        */
        void WaitUntilOperationsAreCompletedL();

    private:
        RPointerArray<MNssTag> iTags;
        RPointerArray<HBufC> iTexts;
        
        // Counter to count how many 
        // callbacks are still expected
        TUint iCallbacksExpected;
        
        // Ignore individual train/save/remove errors
        TBool iIgnoreErrors;
    };

#endif // TAGNAMESET_H
