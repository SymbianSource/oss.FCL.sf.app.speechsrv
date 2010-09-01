/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssSpeechItemTrainer trains SpeechItem objects and later
*               saves them to the database. Both in training and saving,
*               SpeechItemTrainrt allows grouping several requests together
*               and executing them in one run. This is crucial if we want
*               training and saving to scale up.
*
*/



#ifndef NSSVASCSPEECHITEMTRAINER_H
#define NSSVASCSPEECHITEMTRAINER_H

#include "nssvasccontext.h"
#include "nssvascspeechitem.h"

#include "nssvasmcoresyncrecoveryhandler.h"
#include "nsssispeechrecognitionutilityobserver.h"
#include "nssvasctrainingactiontracker.h"

#include <badesca.h>

// FORWARD DECLARATIONS
class CNssSpeechItem;
class MNssSaveTagClient;
class MNssDeleteTagClient;
class CNssVASDatabase;


typedef enum
    {
    EStateIdle,
    ETrainStateWaiting,

    // Training states
    ETrainStateSaveContext,
    ETrainStateTraining,

    // Saving states
    ESaveStateWaiting,
    ESaveStateSaving,

    // Deleting states
    EDeleteStateWaiting,
    EDeleteStateDeleting,

    // Retraining states
    ERetrainStateWaiting,
    ERetrainStateRetraining

    } TTrainState;

/**
* The CNssSpeechItemTrainer class builds SpeechItem instances when requested.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssSpeechItemTrainer
: public CBase,
  public MSISpeechRecognitionUtilityObserver,
  private MDelayedNotifiable
{
    public:

    /**
    * Destructor.
    * The CNssSpeechItemTrainer destructor
    * Destroys the speech item trainer.
    */   
    ~CNssSpeechItemTrainer();

    /**
    * Creates CNssSpeechItemTrainer.
    * @since 2.8
    * @param aDatabase VAS database object
    * @return Speech item trainer
    */       
    static CNssSpeechItemTrainer* NewL( CNssVASDatabase* aDatabase );

    /**
    * Creates CNssSpeechItemTrainer. Pushes it to the cleanup stack.
    * @since 2.8
    * @param aDatabase VAS database object
    * @return Speech item trainer
    */       
    static CNssSpeechItemTrainer* NewLC( CNssVASDatabase* aDatabase );
    
    /**
    * Adds a names to the queue of names to be trained. These names are
    * actually trained after a delay. Calling this function resets
    * the delay timer.
    * @since 2.8
    * @param aEventHandler Callback after the names have been trained
    * @param aTrainingParams Parameters for training
    * @param aSpeechItem Speech item to be trained
    * @param aContext Context, to which the speech item belongs.
    * @return Success status
    */       
    MNssSpeechItem::TNssSpeechItemResult TrainTextDelayed(
        MNssTrainTextEventHandler* aEventHandler,
	    CNssTrainingParameters*    aTrainingParams,
        CNssSpeechItem& aSpeechItem,
        CNssContext& aContext
        );

    /**
    * Adds a names to the queue of names to be trained. These names are
    * actually trained after a delay. Calling this function resets
    * the delay timer.
    * @since 2.8
    * @param aEventHandler Callback after the names have been trained
    * @param aTrainingParams Parameters for training
    * @param aSpeechItem Speech item to be trained
    * @param aContext Context, to which the speech item belongs.
    * @return Success status
    */       
    MNssSpeechItem::TNssSpeechItemResult RetrainTextDelayed(
        MNssTrainTextEventHandler* aEventHandler,
	    CNssTrainingParameters*    aTrainingParams,
        CNssSpeechItem& aSpeechItem,
        CNssContext& aContext
        );

    /**
    * Queue a tag for saving.
    * @since 2.8
    * @param aSaveTagClient callback to call after saving / failing to save.
    * @param aTag           tag to be saved
    * @return Symbian-wide error codes
    */
    TInt SaveTagDelayed(
        MNssSaveTagClient* aSaveTagClient,
        CNssTag& aTag
        );

    /*
    * Saves the tags, which the client has piled up
    * by calling SaveTagDelayed() repeatedly
    * @since 2.8
    */
    void DoSaveTags();

    /**
    * Queue a tag for removal.
    * @param aDeleteTagClient callback to call after deleting / failing to delete.
    * @param aTag           tag to be removed
    * @return Symbian-wide error codes
    */
    TInt DeleteTagDelayed(
        MNssDeleteTagClient* aDeleteTagClient,
        CNssTag& aTag
        );

    /*
    * Deprecated, to be removed, when no usage for this method is confirmed
    * This method is substituted with DoDeleteGroupedTags
    *
    * Deletes the tags, which the client has piled up
    * by calling DeleteTagDelayed() repeatedly
    * @since 2.8
    */
    void DoDeleteTags();
    
    /*
    * Deletes the tags, which the client has piled up
    * by calling DeleteTagDelayed() repeatedly
    * @since 2.8
    */
    void DoDeleteGroupedTags();

    /*
    * Sends the next RemoveRule call to SRS
    * @since 2.8
    */
    void DeleteNextTag();

    /*
    * Handles successful KAsrEventRemoveRule event after RemoveRule call to SRS.
    *
    * Can be obsolete method.
    * @todo Check if it used in retraining tags. If not, remove it
    *
    * @since 2.8
    */
    void HandleRemoveTagComplete();
    
    /*
    * Handles successful KAsrEventRemoveRules event after RemoveRule call to SRS.
    * @since 2.8
    */
    void HandleRemoveTagsComplete();

    /*
    * Handles failed KAsrEventRemoveRule event after RemoveRule call to SRS.
    * @since 2.8
    * @param aError Symbian-wide error code
    */
    void HandleRemoveTagFailed( TInt aError );

    /*
    * Removes tags from VAS and commits SRS removals if successful.
    * @since 2.8
    */
    void RemoveTagsFromVAS();

    /*
    * Sends callbacks related to deleting tags. Also works as an error handler.
    * @since 2.8
    */
    void FinishDeleteTags( TInt aSuccess );

    // Functions from base classes

    /*
    * This function is called after the action tracker decides that it is the time for
    * executing the buffered actions
    * @from MDelayedNotifiable
    */
    void RunBufferedActionsL();

    /*
    * Starts the timer if it isn't running, and resets the time if it is running.
    * @since 2.8
    */
    void RestartTimer();

    /*
    * Trains names after timeout has happened.
    * @since 2.8
    */
    void DoTrainTextDelayed();

    /*
    * Checks whether training parameters are equal to previous ones.
    * In queued training, all names must have the same training parameters.
    * @since 2.8
    * @param aParams Training parameters
    * @return ETrue, if the training parameters are the same as with other tags.
    */
    TBool CheckTrainingParametersL( const CNssTrainingParameters *aParams );

    /*
    * Checks whether the context is equal to the previous one.
    * In queued training, all names must belong to the same context.
    * @since 2.8
    * @param aContext Context of the tag
    * @return ETrue, if the context is the same as for the other tags.
    */
    TBool CheckContext(CNssContext& aContext);

    /*
    * Cleans up, if an error happens during training.
    * @since 2.8
    * @param anItemArray Array of speech items
    */
    void HandleTrainError( RPointerArray<CNssSpeechItem>*& anItemArray );

    /*
    * Splits a descriptor into an array of descriptors.
    * @since 2.8
    * @param aPhrase Phrase to be split.
    * @param aSeparator Separator character between the parts of the string.
    * @return Array, where the descriptor has been split.
    */
    CDesC16ArrayFlat* SplitPhraseL( const TDesC& aPhrase, TChar aSeparator );

    /*
    * Callback to mediate speech recognition events
    * @since 2.8
    * @from MSpeechRecognitionUtilityObserver
    * @param aEvent ID of the speech event
    * @param aResult Success status
    */
    virtual void MsruoEvent( TUid aEvent, TInt aResult );

    /*
    * Cleans memory allocated for training.
    * @since 2.8
    */
    void CleanUpTraining();

    /*
    * Called when training is complete.
    * @since 2.8
    * @param aResult Success status
    */
    void HandleTrainComplete( TInt aResult );

    /*
    * Called when lexicon has been created.
    * @since 2.8
    * @param aError Success status
    */
    void HandleLexiconCreated( TInt aError );

    /*
    * Called when grammar has been created.
    * @since 2.8
    * @param aError Success status
    */
    void HandleGrammarCreated( TInt aError );

    /**
    * Initializes iSrsApi or returns an error.
    * @since 2.8
    * @return KErrNone or some symbian-wide error code.
    */
    TInt CreateSrsApi();

    /*
    * Commits training to SRS database..
    * @since 2.8
    * @return success status
    */
    TInt CommitSIUpdate();

    /*
    * Commits training to SRS database..
    * @since 2.8
    * @return success status
    */
    TInt UncommitSIUpdate();

    /*
    * Trys to allocate a speech item buffer
    * @since 2.8
    * @retutn ETrue if success, EFalse if failure
    */
    TBool SpeechItemBufferNeeded();

    /*
    * Trys to allocate a grammar id buffer
    * @since 2.8
    * @retutn ETrue if success, EFalse if failure
    */
    TBool GrammarIdBufferNeeded();

    /*
    * Phase 1 of retraining: Removes old rules
    * @since 2.8
    */
    void DoRetrainTextDelayed();

    /*
    * Phase 2 of retraining: Adds new rules
    * @since 2.8
    */
    void DoRetrainAddVoiceTags();

    /*
    * Phase 3 of retraining: Updates VAS DB and makes the callbacks.
    * @since 2.8
    * @param aResult Success status of the AddVoiceTags call
    */
    void HandleRetrainComplete( TInt aResult );

    /*
    * Converts an array of Speech Items to a format,
    * where they can be sent to Utility for training.
    * @since 2.8
    * @param aSpeechItems List of speech items
    * @param aPhrases Storage array for splitted phrases
    * @return Success status
    */
    TInt SpeechItems2Phrases(
        RPointerArray<CNssSpeechItem>& aSpeechItems,
        RPointerArray<MDesCArray>& aPhrases );

    /**
    * Cleans up training/retraining and sends the callbacks to the client.
    * @since 2.8
    * @param aResult Was training successful or not.
    */
    void SendTrainingCallbacks( TInt aResult );

    /**
    * Cleanup/error handler function for Delayed Remove operation.
    * @since 2.8
    * @param aSuccess Success status of remove tags
    */
    void RemoveTagsFinished( TInt aSuccess );

  private: //function


   /**
    * C++ constructor
    */ 
    CNssSpeechItemTrainer( CNssVASDatabase* aDatabase );


    /**
    * Symbian 2nd phase constructor 
    */      
    void ConstructL();
    
    /**
    * Set the train state.
    * This is the place to check any state-related invariants and/or
    * print state switching related info in UDEB
    */
    void SetState(TTrainState aState);

    /*
    * Splits a descriptor into an array of descriptors.
    * @since 3.1
    * @param aPhrase Phrase to be split. (for example "_1Adam_1Russell_2mobile")
    * @param aSeparator Separator character between the parts of the string.
    * @return Array, where the descriptor has been split.
    */
    CDesC16ArrayFlat* SplitPhraseSindeL( const TDesC& aPhrase, TChar aSeparator );

  private: //data

    // Pointer to SRS Portal object.
    // The SpeechItemTrainer own the SpeechItemPortal
    CNssVASDatabase* iDatabase;

    // Buffer for grouping the MNssSpeechItems for efficient training.
    RPointerArray<CNssSpeechItem>* iSpeechItemBuffer;

    // Names, which are being trained by CSpeechRecognitionUtility.
    RPointerArray<CNssSpeechItem>* iSpeechItemTrainingBuffer;

    // Stores grammar IDs, so we don't need to store the whole tag when deleting.
    RArray<TUint32>* iGrammarIdBuffer;

    // The grammar IDs for speech items, which we are currently deleting.
    RArray<TUint32>* iGrammarIdDeletingBuffer;

    // This buffer is sent to SRS Utility. Contains: The recognition phrase split into subwords.
    RPointerArray<MDesCArray> iPhraseArray;

    // State: Idle, waiting or training.
    TTrainState iState;

    // Training parameters
    CNssTrainingParameters* iTrainingParams;

    // Context
    CNssContext* iContext;

    // Rule ID array. Return values from AddVoiceTags() are placed here.
    RArray<TUint32>* iRuleIDArray;
    
    // Rule ID array. Rule IDs for tags scheduled for deletion are placed here
    RArray<TUint32>* iDeleteRuleIDArray;

    // Handle to speech services
    CNssSiUtilityWrapper *iSrsApi;

    MNssTrainTextEventHandler *iTrainEventHandler;

    // Lexicon ID for training
    TUint32 iLexiconId;

    // Grammar ID for training
    TUint32 iGrammarId;

    /*************** Saving variables ****************/
    // The tags to be saved
    RPointerArray<CNssTag> *iTagBuffer;

    // Callback function after saving
    MNssSaveTagClient* iSaveEventHandler;

    /*************** Deleting variables **************/
    // Tags, which are currently being deleted
    RPointerArray<CNssTag> *iTagDeleteBuffer;

    // Callback function after removal
    MNssDeleteTagClient* iDeleteEventHandler;

    // Counter for asynchronous delete calls
    TInt iTagDeleteCounter;
    
    // decides when to execute the buffered (delayed) actions
    CNssTrainingActionTracker* iActionTracker;

};



#endif // NSSVASCSPEECHITEMTRAINER_H

// End of file
