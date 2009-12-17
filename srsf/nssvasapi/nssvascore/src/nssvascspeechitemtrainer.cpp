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
* Description:  The CNssSpeechItemTrainer provides methods to train, save
*               and delete CNssSpeechItem objects in large quantities.
*
*/


/* Here is a state machine for training:
*
* Summary:
*
* Idle state
*     |
* Waiting state     <-------------------
*     |                                | (loop back to waiting state)
* ###################################  |
* # IF NEEDED: Create lexicon state #  |
* #            |                    #  |
* # IF NEEDED: Create grammar state #---
* #            |                    #
* #       Training state            #
* ###################################
*     |
* Idle state
*
*
*
* State: Idle state
* Event: A client calls TrainTextL, 
* Action:This induces a call to CNssSpeechItemTrainer::TrainTextDealyed().
*
*        The basic idea of _delayed_training_ is that several training calls
*        are piled up before taking action. When a large number of names is
*        trained at a time, the economies of scale are considerable.
*
*        So, TrainTextDelayed() adds the name to the to-be-trained list,
*        and starts a _delay_timer_. When this delay timer has finished, the
*        actual training starts.
* Transition: Idle state -> Waiting state
*
*
* State: Waiting state
* Event: A client calls TrainTextL.
* Action:The function adds the name to the to-be-trained list, and
*        resets the timer
*        (background: In contact synchronization via PC Suite, names are added
*         every 1/2 seconds. We want to keep piling the names  as long as
*         new names keep coming. Since we can't wait 50 seconds
*         before training, the only reasonable solution is to reset the timer
*         every time a new name arrives)
* Transition: Waiting state -> Waiting state
*
*
* State: Waiting state
* Event: Timer finishes doing time.
* Action:Check the preparations for training:
*         * Has the grammar for the context been created?
*         * Has the lexicon for the context been created?
* CASE: Preconditions OK
*       Action    : Send async AddVoiceTags call
*       Transition: Waiting state -> Training state
*
* State: Training state
* Event: Training finished
* Action: Save the newly created Rule IDs to the speech items.
*         Commit the database.
*         Make the callbacks (success or failure) to the client.
*
* Transition: Check if new names were piled during traning.
*         YES: Training state -> Waiting state
*         NO : Training state -> Idle state
*
*/

#include "srsfbldvariant.hrh"
#include "nssvascspeechitemtrainer.h"
#include "nssvasctrainingparameters.h"
#include "nssvascvasdatabase.h"
#include "nssvasmsavetagclient.h"
#include "nssvasmdeletetagclient.h"

#include "rubydebug.h"

#include <badesca.h>

#ifdef _DEBUG
    // Used in UDEB only
    _LIT( KSpeechItemTrainerPanic, "VasCSpeechItemTrainer" );
#endif  // _DEBUG

#define DEBUGPANIC User::Panic( KSpeechItemTrainerPanic, __LINE__ )
#define REACT( a, b ) if ( a != KErrNone ) { b; }
#define CHECK_NOT_NULL( a ) \
    if ( !(a) ) \
        { \
        return MNssSpeechItem::EVasTrainFailed; \
        }
#define ALLOWED_STATES( a, b, c ) \
    if ( iState != a && iState != b && iState != c ) \
        { \
        return MNssSpeechItem::EVasTrainFailed; \
        } 

// tags are buffered in the buffer that grows with this granularity
const TInt KTagBufferGranularity = 100;

// rule ids are buffered in the buffer that grows with this granularity
const TInt KRuleIdBufferGranularity = 50;

// tag ids are buffered in the buffer that grows with this granularity
const TInt KRemoveTagIdBufferGranularity = 50;

// Granularity for the array of words per sinde phrase
const TInt KSindeWordArrayGranularity = 2;


// Local function declarations
TInt EnforceRuleIdCountInvariant( RArray<TUint32>& aArray, TInt aCount );

// ---------------------------------------------------------
// CNssSpeechItemTrainer::CNssSpeechItemTrainer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CNssSpeechItemTrainer::CNssSpeechItemTrainer( CNssVASDatabase* aDatabase )
: iDatabase( aDatabase ),
  iSpeechItemBuffer       ( NULL ),
  iSpeechItemTrainingBuffer( NULL ),
  iGrammarIdBuffer        ( NULL ),
  iGrammarIdDeletingBuffer( NULL ),
  iTagBuffer(NULL)
    {
    // empty
    }


// -----------------------------------------------------------------------------
// CNssSpeechItemTrainer::ConstructL
// Symbian 2nd phase constructor can leave.
// This is overloaded function for SpeechItemTrainer from database
// -----------------------------------------------------------------------------
//
void CNssSpeechItemTrainer::ConstructL()
    {
    RUBY_DEBUG_BLOCK("CNssSpeechItemTrainer::ConstructL");
    iActionTracker = CNssTrainingActionTracker::NewL( *this );

    // Create buffer for grouping phrases in training
    iSpeechItemBuffer = new(ELeave)RPointerArray<CNssSpeechItem>;
    iSpeechItemTrainingBuffer = 0; // No name being currently trained.
    }


// -----------------------------------------------------------------------------
// CNssSpeechItemTrainer::NewL
// Two-phased constructor.
// This is for new SpeechItemTrainer
// -----------------------------------------------------------------------------
//
CNssSpeechItemTrainer* CNssSpeechItemTrainer::NewL( CNssVASDatabase* aDatabase )
    {
    RUBY_DEBUG_BLOCK( "CNssSpeechItemTrainer::NewL" );

    CNssSpeechItemTrainer* self = NewLC( aDatabase );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CNssSpeechItemTrainer::NewLC
// Two-phased constructor.
// This is overloaded function for SpeechItemTrainer from database
// -----------------------------------------------------------------------------
//
CNssSpeechItemTrainer* CNssSpeechItemTrainer::NewLC( CNssVASDatabase* aDatabase )
    {
    CNssSpeechItemTrainer* self
        = new (ELeave) CNssSpeechItemTrainer( aDatabase );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::~CNssSpeechItemTrainer
// Delete the SpeechItemTrainer object and set the Portal's 
// state to Terminate.
// The Trainer does not delete the Portal, the Portal  
// deletes itself.
// ---------------------------------------------------------
//
CNssSpeechItemTrainer::~CNssSpeechItemTrainer()
    {
    RUBY_DEBUG0( "CNssSpeechItemTrainer::~CNssSpeechItemTrainer" );

    delete iActionTracker;


    // Buffer for grouping the MNssSpeechItems for efficient training.
    if ( iSpeechItemBuffer )
        {
        iSpeechItemBuffer->Reset();
        delete iSpeechItemBuffer;
        }

    // Names, which are being trained by CSISpeechRecognitionUtility.
    if ( iSpeechItemTrainingBuffer )
        {
        iSpeechItemTrainingBuffer->Reset();
        delete iSpeechItemTrainingBuffer;
        }

    // This buffer is sent to SRS Utility. Contains: The recognition phrase split into subwords.
    iPhraseArray.ResetAndDestroy();

    // Training parameters
    delete iTrainingParams;

    // Context
    delete iContext;

    // Rule ID array. Return values from AddVoiceTags() are placed here.
    if ( iRuleIDArray )
        {
        iRuleIDArray->Close();
        delete iRuleIDArray;
        }
      
    if ( iDeleteRuleIDArray )
    	{
        RUBY_DEBUG0( "CNssSpeechItemTrainer::~CNssSpeechItemTrainer Deleting iDeleteRuleIDArray" );
    	iDeleteRuleIDArray->Close();
    	delete iDeleteRuleIDArray;
    	}

    // Handle to speech services
    if ( iSrsApi )
        {
        iSrsApi->CancelUtility();
        delete iSrsApi;
        iSrsApi = NULL;
        }

    // The tags to be saved: Owned by client
    // RPointerArray<CNssTag> *iTagBuffer;
    if ( iTagBuffer )
        {
        iTagBuffer->Reset();
        delete iTagBuffer;
        iTagBuffer = 0;
        }

    if ( iGrammarIdBuffer )
        {
        iGrammarIdBuffer->Reset();
        delete iGrammarIdBuffer;
        iGrammarIdBuffer = 0;
        }

    if ( iGrammarIdDeletingBuffer )
        {
        iGrammarIdDeletingBuffer->Reset();
        delete iGrammarIdDeletingBuffer;
        iGrammarIdDeletingBuffer = 0;
        }
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::RetrainTextDelayed
// Does delayed retraining.
// ---------------------------------------------------------
//
MNssSpeechItem::TNssSpeechItemResult CNssSpeechItemTrainer::RetrainTextDelayed(
    MNssTrainTextEventHandler* aEventHandler,
	CNssTrainingParameters*    aTrainingParams,
    CNssSpeechItem&            aSpeechItem,
    CNssContext&               aContext)
    {
    RUBY_DEBUG0( "CNssSpeechItemTrainer::RetrainTextDelayed" );

    // Check parameters: 
    // callback 
    CHECK_NOT_NULL( aEventHandler );

    // phrase to be trained
    CHECK_NOT_NULL( aSpeechItem.Text().Length() );

    // Check state
    ALLOWED_STATES( EStateIdle, ERetrainStateWaiting, ERetrainStateRetraining );

    // Check event handler
    if ( !iTrainEventHandler )
        {
        iTrainEventHandler = aEventHandler;
        }
    else if ( iTrainEventHandler != aEventHandler )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    // The training parameters and the context 
    // must be the same for all in the buffer.
    TRAPD( error,
        if ( !CheckTrainingParametersL( aTrainingParams ) ||
             !CheckContext( aContext ) )
            {
            return( MNssSpeechItem::EVasTrainFailed );
            }
         );
    if ( error != KErrNone )
        {
        return MNssSpeechItem::EVasTrainFailed;
        }

    // Buffer the training request

    // Allocate buffer, if not done earlier
    if ( !SpeechItemBufferNeeded() )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    // If this is the first tag, save Lexicon ID and Grammar ID.
    if ( iSpeechItemBuffer->Count() == 0 )
        {
        iLexiconId = aContext.LexiconId();
        iGrammarId = aContext.GrammarId();
        }
    else
        {
        // The tags after the first one must have the same Gramamr ID & Lexicon ID.
        if ( iLexiconId != aContext.LexiconId() ||
            iGrammarId != aContext.GrammarId() )
            {
            return( MNssSpeechItem::EVasTrainFailed );
            }
        }

    // Add request to buffer
    TInt ret = iSpeechItemBuffer->Append( &aSpeechItem );

    if ( ret != KErrNone )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    // Restart timer. When the timer has waited for a defined period, the names
    // get retrained. Timer is reseted every time RetrainTextDelayed is called.
    //
    // Retraining in groups is more efficient. The delay ensures this grouping.
    switch( iState )
        {
        case ERetrainStateRetraining:
            // Wait for the retraining to finish before acting.
            break;

        case EStateIdle:
            SetState( ERetrainStateWaiting );
            RestartTimer();
            break;

        case ERetrainStateWaiting:
            RestartTimer();
            break;

        default:
            return( MNssSpeechItem::EVasTrainFailed );
        }

    return( MNssSpeechItem::EVasErrorNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::SaveTagDelayed
// Does delayed saving.
// ---------------------------------------------------------
//
TInt CNssSpeechItemTrainer::SaveTagDelayed(
    MNssSaveTagClient* aSaveTagClient,
    CNssTag& aTag )
    {
    RUBY_DEBUG0( "CNssSpeechItemTrainer::SaveTagDelayed" );

    // Check internal state: Must not be training
    if ( this->iState != EStateIdle &&
         this->iState != ESaveStateWaiting &&
         this->iState != ESaveStateSaving )
        {
        return( KErrNotReady );
        }

    // Check arguments
    if ( aSaveTagClient == 0  )
        {
        return KErrArgument;
        }

    if ( !((CNssSpeechItem*)aTag.SpeechItem())->Trained() )
        {
        return KErrNotReady;
        }

    // The event handler must be the same for all group-saved tags
    if ( !iSaveEventHandler )
        {
        iSaveEventHandler = aSaveTagClient;
        }
    else if ( aSaveTagClient != iSaveEventHandler )
        {
        return KErrArgument;
        }

    // Buffer the saving request

    // Allocate buffer, if not done earlier
    if ( !iTagBuffer )
        {
        iTagBuffer = new RPointerArray<CNssTag>( KTagBufferGranularity );

        if ( !iTagBuffer )
            {
            return KErrNoMemory;
            }
        }

    // Add request to buffer
    TInt ret = iTagBuffer->Append( &aTag );

    if ( ret != KErrNone )
        {
        return( ret );
        }

    // Restart timer. When the timer has waited for a defined period, the names
    // get saved. Timer is reseted every time SaveTagDelayed is called.
    //
    // Saving in groups is more efficient. The delay ensures this grouping.
    switch( iState )
        {
        case ESaveStateSaving:
            // Wait for the training to finish before acting.
            break;

        case EStateIdle:
            SetState( ESaveStateWaiting );
            RestartTimer();
            break;

        case ESaveStateWaiting:
            RestartTimer();
            break;

        default:
            return( KErrCorrupt );
        }

    return( KErrNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DeleteTagDelayed
// Does delayed deleting.
// (other items were commented in a header)
// ---------------------------------------------------------
//
TInt CNssSpeechItemTrainer::DeleteTagDelayed(
    MNssDeleteTagClient* aDeleteTagClient,
    CNssTag& aTag )
    {
    RUBY_DEBUG0( "CNssSpeechItemTrainer::DeleteTagDelayed" );

    // Check internal state: Must not be doing something else
    ALLOWED_STATES( EStateIdle, EDeleteStateWaiting, EDeleteStateDeleting );

    // Check arguments
    CHECK_NOT_NULL( aDeleteTagClient );

    // The event handler must be the same for all group-deleted tags
    if ( !iDeleteEventHandler )
        {
        iDeleteEventHandler = aDeleteTagClient;
        }
    else if ( aDeleteTagClient != iDeleteEventHandler )
        {
        return KErrArgument;
        }

    // Buffer the deleting request

    // Allocate buffer, if not done earlier
    if ( !SpeechItemBufferNeeded() ||
         !GrammarIdBufferNeeded()  )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    // If this is the first tag, save the grammar id.
    if ( iSpeechItemBuffer->Count() == 0 )
        {
        iGrammarId = ((CNssContext*)aTag.Context())->GrammarId();
        if ( !iDeleteRuleIDArray ) 
        	{
        	iDeleteRuleIDArray = new RArray<TUint32>;
        	if( !iDeleteRuleIDArray ) 
        		{
        		return KErrNoMemory;
	        	}
        	// no deletion. iDeleteRuleIDArray might still be used
        	}
//        delete iDeleteRuleIDArray; // if any
//        iDeleteRuleIDArray = new RArray<TUint32>;
        }
    else
        {
        // subsequent tags must have the same grammar id.
        if ( iGrammarId != ((CNssContext*)aTag.Context())->GrammarId() )
            {
            return( MNssSpeechItem::EVasTrainFailed );
            }
        }

    // Add request to buffer
    // iSpeechItemBuffer will be copied into iSpeechItemTrainingBuffer later and
    // iSpeechItemTrainingBuffer will be used to delete tags from the VAS DB
    TInt ret = iSpeechItemBuffer->Append( (CNssSpeechItem*)aTag.SpeechItem() );
    if ( ret != KErrNone )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    RUBY_DEBUG1( "CNssSpeechItemTrainer::DeleteTagDelayed Adding [%d] to iDeleteRuleIDArray", 
        ( ( CNssSpeechItem* )aTag.SpeechItem() )->RuleID() );

    ret = iDeleteRuleIDArray->Append( ( ( CNssSpeechItem* )aTag.SpeechItem() )->RuleID() );

    if ( ret != KErrNone )
        {
        RUBY_DEBUG1( "CNssSpeechItemTrainer::DeleteTagDelayed Adding to iDeleteRuleIDArray failed with error [%d]", ret );
        return( MNssSpeechItem::EVasTrainFailed );
        }

    // Restart timer. When the timer has waited for a defined period, the names
    // get deleted. Timer is reseted every time DeleteDagDelayed is called.
    //
    // Deleting in groups is more efficient. The delay ensures this grouping.
    switch( iState )
        {
        case EDeleteStateDeleting:
            // Wait for the training to finish before acting.
            break;

        case EStateIdle:
            SetState( EDeleteStateWaiting );
            RestartTimer();
            break;

        case EDeleteStateWaiting:
            RestartTimer();
            break;

        default:
            return( KErrCorrupt );
        }

    return( KErrNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::TrainTextDelayed
// Does delayed training.
// ---------------------------------------------------------
//
MNssSpeechItem::TNssSpeechItemResult CNssSpeechItemTrainer::TrainTextDelayed(
    MNssTrainTextEventHandler* aEventHandler,
	CNssTrainingParameters*    aTrainingParams,
    CNssSpeechItem&            aSpeechItem,
    CNssContext&               aContext)
    {
    RUBY_DEBUG0( "CNssSpeechItemTrainer::TrainTextDelayed" );

    // Check parameters: 
    // callback
    CHECK_NOT_NULL( aEventHandler );

    // Text is not null
    CHECK_NOT_NULL( aSpeechItem.Text().Length() );

    // Check state
    ALLOWED_STATES( EStateIdle, ETrainStateWaiting, ETrainStateTraining );

    // Check event handler
    if ( !iTrainEventHandler )
        {
        iTrainEventHandler = aEventHandler;
        }
    else if ( iTrainEventHandler != aEventHandler )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    // The training parameters and the context 
    // must be the same for all in the buffer.
    TRAPD( error,
        if ( !CheckTrainingParametersL( aTrainingParams ) ||
             !CheckContext( aContext ) )
            {
            return( MNssSpeechItem::EVasTrainFailed );
            }
        );
    if ( error != KErrNone )
        {
        return MNssSpeechItem::EVasTrainFailed;
        }

    // Buffer the training request
    if ( !SpeechItemBufferNeeded() )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    TInt ret = iSpeechItemBuffer->Append( &aSpeechItem );

    if ( ret == KErrNoMemory )
        {
        return( MNssSpeechItem::EVasTrainFailed );
        }

    // Restart timer. When the timer has waited for a defined period, the names
    // get trained. Timer is reseted every time TrainTextDelayed is called.
    //
    // Training in groups is more efficient. The delay ensures this grouping.
    switch( iState )
        {
        case ETrainStateTraining:
            // Wait for the training to finish before acting.
            break;

        case EStateIdle:
            SetState( ETrainStateWaiting );
            RestartTimer();
            break;

        case ETrainStateWaiting:
            RestartTimer();
            break;

        default:
            return( MNssSpeechItem::EVasTrainFailed );
        }

    return( MNssSpeechItem::EVasErrorNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::HandleTrainError
// Called if AddVoiceTags() call failed.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::HandleTrainError( RPointerArray<CNssSpeechItem>*& anItemArray )
    {
    RUBY_DEBUG0( "CNssSpeechItemTrainer::HandleTrainError" );

    TInt count = anItemArray->Count();

    CleanUpTraining();

    // Announce the error to the client
    for( TInt k=0; k<count; k++ )
        {
        iTrainEventHandler->HandleTrainComplete( KErrGeneral );
        
        }
    iTrainEventHandler = NULL;
    // All the errors reported, everything was rolled back
    // Back tothe idle state
    SetState( EStateIdle );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::CleanUpTrianing
// Cleans memory allocated by training.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::CleanUpTraining(void)
    {
    // Delete buffered names
    if ( iSpeechItemTrainingBuffer )
        {
        iSpeechItemTrainingBuffer->Close();
        }
    delete iSpeechItemTrainingBuffer;
    iSpeechItemTrainingBuffer = 0;

    // Delete parameters common to all names
    delete iTrainingParams;
    iTrainingParams = 0;

    delete iContext;
    iContext = 0;
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DoTrainTextDelayed
// Starts training tags. Sends the AddVoiceTags() call.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::DoTrainTextDelayed()
    {
    SetState( ETrainStateTraining );

    // Move from speech item buffers from filling state to processing state.

    // names to be trained + names to be retrained
    iSpeechItemTrainingBuffer = iSpeechItemBuffer;
    iSpeechItemBuffer = 0;

    // Initialize iSrsApi (speech recognition utility)
    TInt error = CreateSrsApi();
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CNssSpeechItemTrainer::DoTrainTextDelayed Creating SrsApi failed. Error [%d]", error );
        HandleTrainError( iSpeechItemTrainingBuffer );
        }
    else 
       	{
        	
	    __ASSERT_DEBUG( iContext->LexiconId() != KInvalidLexiconID, 
	                    User::Panic( KSpeechItemTrainerPanic, KErrCorrupt ) );
	    __ASSERT_DEBUG( iContext->GrammarId() != KInvalidGrammarID, 
	                    User::Panic( KSpeechItemTrainerPanic, KErrCorrupt ) );

	    // Extract texts from speech items to phrase array.

	    // iPhraseArray is an array of arrays: 
	    // 
	    //        |-- firstname1
	    //  tag1 -|-- lastname1
	    //
	    //  tag2 -|-- firstname2
	    //        |-- lastname2
	    iPhraseArray.ResetAndDestroy();

	    // Convert speech item array to the format understood by the Utility.
	    TInt ret = SpeechItems2Phrases( *iSpeechItemTrainingBuffer, iPhraseArray );
	    if ( ret != KErrNone )
	        {
	        SendTrainingCallbacks( ret );
	        /** @todo reengineer into signle return path
	            @todo check that everything is cleaned up before the return */
	        return;
	        }

	    // Initialize Rule ID array. The Utility puts the IDs
	    // of newly created rules here.
	    // Not using (ELeave) since this is non-leaving function (trapping of (ELeave) is not allowed).
	    iRuleIDArray = new RArray<TUint32>;

	    if ( iRuleIDArray == 0 )
	        {
            RUBY_DEBUG0( "CNssSpeechItemTrainer::DoTrainTextDelayed iRuleIDArray == 0" );

	        HandleTrainError( iSpeechItemTrainingBuffer );
	        }
	    else
	       	{
            RUBY_DEBUG1( "CNssSpeechItemTrainer::Calling AddVoiceTags (%d names)", 
                         iPhraseArray.Count() ); 
	       	// allocation succeeded

            TRAPD( errSinde, const RArray<RTrainingLanguageArray>& languages = 
                                        iTrainingParams->SindeLanguagesL() );
            
            if ( errSinde == KErrNone )
                {
#ifdef __SINDE_TRAINING
                TRAP_IGNORE(
                    const RArray<RTrainingLanguageArray>& languages = 
                                        iTrainingParams->SindeLanguagesL();
    		        ret = iSrsApi->AddVoiceTags( iPhraseArray,
                                                 languages,
                                                 (TSILexiconID)iContext->LexiconId(),
                                                 (TSIGrammarID)iContext->GrammarId(),
                                                 *iRuleIDArray );
                ); // TRAP_IGNORE
#endif // __SINDE_TRAINING
                }
            else
                {
                // Use the old style of training if SINDE languages are not available
		        ret = iSrsApi->AddVoiceTags( iPhraseArray,
                                             iTrainingParams->Languages(),
                                             (TSILexiconID)iContext->LexiconId(),
                                             (TSIGrammarID)iContext->GrammarId(),
                                             *iRuleIDArray );
                }

		    // error -> cleanup
		    if ( ret != KErrNone )
		        {
                RUBY_DEBUG1( "CNssSpeechItemTrainer::AddVoiceTags failed(%d)", ret );

		        HandleTrainComplete( ret );
		        }  // if ret is not KErrNone
	       	}  // if iRuleIDArray allocation succeeded
       	}  // if SRS API was created successfully
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DoRetrainTextDelayed
// Starts the 1st phase of retraining,
// which is removing old rules.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::DoRetrainTextDelayed()
    {
    __ASSERT_DEBUG( iSpeechItemBuffer != 0, DEBUGPANIC );

    SetState( ERetrainStateRetraining );

    // Move speech item buffer from filling state to processing state.
    iSpeechItemTrainingBuffer = iSpeechItemBuffer;
    iSpeechItemBuffer = 0;

    // Initialize iSrsApi (speech recognition utility)
    TInt error = CreateSrsApi();
    if ( error != KErrNone )
        {
        RUBY_DEBUG1( "CNssSpeechItemTrainer::DoRetrainTextDelayed Creating SrsApi failed. Error [%d]", error );
        HandleTrainError( iSpeechItemTrainingBuffer );
        return;
        }

    // Start deleting tags
    iTagDeleteCounter = 0;
    DeleteNextTag();
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DoRetrainAddVoiceTags
// Phase 2 of retraining: Add voice tags
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::DoRetrainAddVoiceTags()
    {
    SetState( ERetrainStateRetraining );

    // Move from speech item buffers from filling state to processing state.

    __ASSERT_DEBUG( iContext->LexiconId() != KInvalidLexiconID, 
                    User::Panic( KSpeechItemTrainerPanic, KErrCorrupt ) );
    __ASSERT_DEBUG( iContext->GrammarId() != KInvalidGrammarID, 
                    User::Panic( KSpeechItemTrainerPanic, KErrCorrupt ) );

    // Extract texts from speech items to phrase array.

    iPhraseArray.ResetAndDestroy();

    // Convert speech item array to the format understood by the Utility.
    TInt ret = SpeechItems2Phrases( *iSpeechItemTrainingBuffer, iPhraseArray );
    if ( ret != KErrNone )
        {
        SendTrainingCallbacks( ret );
        return;
        }

    // Allocate Rule ID array. The Utility fills this array.
    // When the utility trans a name, it attaches a Rule ID to the name.
    iRuleIDArray = new RArray<TUint32>;
    if ( iRuleIDArray == 0 )
        {
        SendTrainingCallbacks( KErrNoMemory );
        return;
        }

    RUBY_DEBUG1( "CNssSpeechItemTrainer::Retrain Calling AddVoiceTags (%d names)", 
                 iPhraseArray.Count() );

    TRAPD( errSinde, const RArray<RTrainingLanguageArray>& languages = 
                                        iTrainingParams->SindeLanguagesL() );
            
    if ( errSinde == KErrNone )
        {
#ifdef __SINDE_TRAINING        	
        TRAP_IGNORE(
            const RArray<RTrainingLanguageArray>& languages = 
                                iTrainingParams->SindeLanguagesL();
	        ret = iSrsApi->AddVoiceTags( iPhraseArray,
                                         languages,
                                         (TSILexiconID)iContext->LexiconId(),
                                         (TSIGrammarID)iContext->GrammarId(),
                                         *iRuleIDArray );
        ); // TRAP_IGNORE
#endif // __SINDE_TRAINING
        }
    else
        {
        // Use the old style of training if SINDE languages are not available
        ret = iSrsApi->AddVoiceTags( iPhraseArray,
                                     iTrainingParams->Languages(),
                                     (TSILexiconID)iLexiconId,
                                     (TSIGrammarID)iGrammarId,
                                     *iRuleIDArray );
        }

    // error -> cleanup
    if ( ret != KErrNone )
        {
        RUBY_DEBUG1( "CNssSpeechItemTrainer::Retrain ERROR: AddVoiceTags(%d)", ret );

        SendTrainingCallbacks( ret );
        }

    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DoRetrainAddVoiceTags
// After the new tags have been trained:
// Announce the new rule IDs for speech items,
// and update the rule IDs to the VAS database.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::HandleRetrainComplete( TInt aResult )
    {
    RUBY_DEBUG1( "CNssSpeechItemTrainer::HandleTrainComplete(%d)", aResult );

    // iRuleIdArray contains the rule IDs of the newly trained tags.
    // The following call makes sure that
    // iRuleIdArray.Count() == iSpeechItemTrainingBuffer.Count().
    if ( EnforceRuleIdCountInvariant( *iRuleIDArray, 
                        iSpeechItemTrainingBuffer->Count() ) != KErrNone )
        {
        SendTrainingCallbacks( KErrNoMemory );
        }
    else
        {
        TInt count = iSpeechItemTrainingBuffer->Count();
        RArray<TNssSpeechItem> ruleIdUpdateArray( KRuleIdBufferGranularity );

        for( TInt itemIdx = 0; itemIdx < count; itemIdx++ )
            {
            CNssSpeechItem* item = (*iSpeechItemTrainingBuffer)[itemIdx];
            TUint32    ruleID = (*iRuleIDArray)[itemIdx];
            if ( ruleID != KInvalidRuleID )
                {
                // tag was trained successfully

                item->DelayedTrainingComplete( ruleID );
                
                TNssSpeechItem flatItem;

                flatItem.iRuleId = item->RuleID();
                flatItem.iTagId  = item->TagId();

                TInt ret = ruleIdUpdateArray.Append( flatItem );

                if ( ret != KErrNone )
                    {
                    ruleIdUpdateArray.Close();
                    SendTrainingCallbacks( KErrNoMemory );
                    return;
                    }
                }
            else
                {
                // training failed -> remove tag
                iDatabase->DeleteTag( item->TagId() );
                }   
            }

        aResult = iDatabase->UpdateTagRuleIDs( ruleIdUpdateArray );
        ruleIdUpdateArray.Close();
        SendTrainingCallbacks( aResult );
        }

    
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DoSaveTags
// Saves tags.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::DoSaveTags()
    {

    __ASSERT_DEBUG( iState == ESaveStateWaiting, DEBUGPANIC );

    // Save the tags en masse
    TInt ret = iDatabase->SaveTags( iTagBuffer );

    // Next, we're going to make callbacks to the client.
    // Before that, we have to return the object to idle state,
    // This way, the last callback can trigger new train/save action.

    // Destroy buffered tags
    TInt count = iTagBuffer->Count();
    iTagBuffer->Close();
    delete iTagBuffer;
    iTagBuffer = 0;

    // Detach save tag client
    MNssSaveTagClient* client = iSaveEventHandler;
    iSaveEventHandler = 0;

    // Make SpeechItemTrainer ready for new challenges
    SetState( EStateIdle );

    // Make callbacks
    for ( TInt k = 0; k < count; k++ )
        {
        if ( ret == KErrNone )
            {
#ifdef _DEBUG
            TRAPD( err, client->SaveTagCompleted( KErrNone ) );
            __ASSERT_DEBUG( err == KErrNone, DEBUGPANIC );
#else
            client->SaveTagCompleted( KErrNone );
#endif // _DEBUG
            }
        else
            {
#ifdef _DEBUG
            TRAPD( err, client->SaveTagCompleted( KErrGeneral ) );
            __ASSERT_DEBUG( err == KErrNone, DEBUGPANIC );
#else
            client->SaveTagCompleted( KErrGeneral );
#endif // _DEBUG
            }
        }
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DoDeleteTags
// Starts tag removal.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::DoDeleteTags()
    {
    // NOTE: This method is never called from anywhere!
    
    SetState( EDeleteStateDeleting );

    TInt err = CreateSrsApi();
    if ( err != KErrNone )
        {
        RemoveTagsFinished( err );
        return;
        }

    // Move Tag Buffer from filling state to processing state.
    iSpeechItemTrainingBuffer = iSpeechItemBuffer;
    iSpeechItemBuffer = 0;
    iSpeechItemBuffer = new RPointerArray<CNssSpeechItem>; // New filling buffer
    if( !iSpeechItemBuffer )
        {
        SendTrainingCallbacks( KErrNoMemory );
        return;
        }

    iTagDeleteCounter = 0;
    DeleteNextTag();
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DoDeleteGroupedTags
// Starts grouped tag removal.
// Used when deleting tags without retraining.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::DoDeleteGroupedTags()
    {
  	RUBY_DEBUG1("CNssSpeechItemTrainer::DoDeleteGroupedTags iDeleteRuleIDArray->Count() = [%d]", iDeleteRuleIDArray->Count() );
	SetState( EDeleteStateDeleting );

    // Move Tag Buffer from filling state to processing state.
    // We want to have this buffer != NULL so that we are able to do callbacks (with appropriate error code )
    // in RemoveTagsFinished method even if e.g. CreateSrsApi fails. 
    iSpeechItemTrainingBuffer = iSpeechItemBuffer;
    iSpeechItemBuffer = 0;

    TInt err = CreateSrsApi();
    if ( err != KErrNone )
        {
        RemoveTagsFinished( err );
        return;
        }

    iSpeechItemBuffer = new RPointerArray<CNssSpeechItem>; // New filling buffer
    
    if( !iSpeechItemBuffer )
        {
        // As this method is not used when retraining, no need 
        // to send other callbacks than those related to deleting tags.
        RemoveTagsFinished( KErrNoMemory );
        return;
        }

	TInt ret = iSrsApi->RemoveRules( ( TSIGrammarID )iGrammarId,
                                     *iDeleteRuleIDArray );

    if ( ret != KErrNone && ret != KErrNotFound )
        {
        RUBY_DEBUG1( "CNssSpeechItemTrainer::DoDeleteGroupedTags RemoveRules failed. \
                      Error [%d]", ret );
        RemoveTagsFinished( ret );
        return;
        }

    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::DeleteNextTag
// Sends a RemoveRule() call for the next rule in the removal queue.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::DeleteNextTag()
    {
    RUBY_DEBUG2( "CNssSpeechItemTrainer::DeleteNextTag iTagDeleteCounter [%d], iSpeechItemTrainingBuffer->Count() [%d]", iTagDeleteCounter, iSpeechItemTrainingBuffer->Count() );

    __ASSERT_DEBUG( iTagDeleteCounter <= iSpeechItemTrainingBuffer->Count(), 
                    User::Panic( KSpeechItemTrainerPanic, __LINE__ ) );

    CNssSpeechItem* speechItem = (*iSpeechItemTrainingBuffer)[iTagDeleteCounter];

    TInt ret = iSrsApi->RemoveRule(
        (TSIGrammarID)speechItem->GrammarId(),
        (TSIRuleID)speechItem->RuleID()
        );

    if ( ret != KErrNone && ret != KErrNotFound )
        {
        RemoveTagsFinished( ret );
        return;
        }
    }


// ---------------------------------------------------------
// CNssSpeechItemTrainer::HandleRemoveTagComplete
// Called after SRS has successfully removed a tag.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::HandleRemoveTagComplete()
    {
    iTagDeleteCounter++;

    if ( iTagDeleteCounter < iSpeechItemTrainingBuffer->Count() )
        {
        DeleteNextTag();
        }
    else
        {
        RemoveTagsFinished( KErrNone );
        return;
        }
    }
    
// ---------------------------------------------------------
// CNssSpeechItemTrainer::HandleRemoveTagsComplete
// Called after SRS has successfully removed a group of tags.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::HandleRemoveTagsComplete()
    {
    RemoveTagsFinished( KErrNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::HandleRemoveTagFailed
// Called if SRS fails to remove a tag.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::HandleRemoveTagFailed( TInt aError )
    {
    RemoveTagsFinished( aError );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::RemoveTagsFinished
// All required tags have been removed.
// Go to next phase.
//    functions a bit defferently depending on whether we were retraining or
//    deleting tags
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::RemoveTagsFinished( TInt aSuccess )
    {
    // The continuation depends on what we are doing:

    // If we are deleting tags
    if ( iState == EDeleteStateDeleting )
        {
        if ( aSuccess == KErrNone )
            {
            RemoveTagsFromVAS();
            }
        else{
            FinishDeleteTags( aSuccess );
            }
        }

    // If we are retraining 
    // (removing the old rules before adding the new ones)
    else if ( iState == ERetrainStateRetraining )
        {
        if  ( aSuccess == KErrNone )
            {
            DoRetrainAddVoiceTags();
            }
        else{
            SendTrainingCallbacks( aSuccess );
            }
        }
    else
        {
        RUBY_DEBUG1( "CNssSpeechItemTrainer::RemoveTagsFinished - ERROR: Wrong state(%d)", iState );
        }
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::RemoveTagsFromVAS
// Deletes tags from VAS.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::RemoveTagsFromVAS()
    {
    RArray<TUint32> tagIdArray( KRemoveTagIdBufferGranularity );
    TInt ret = KErrNone;

    ////////////// Remove tags from VAS DB ///////////////////

    // Take the Tag IDs to an array.
    for ( TInt k( 0 ); k < iSpeechItemTrainingBuffer->Count(); k++ )
        {
        ret |= tagIdArray.Append( (*iSpeechItemTrainingBuffer)[k]->TagId() );
        }

    // Delete tags from VAS DB
    if ( ret == KErrNone )
        {
        ret = iDatabase->DeleteTags( tagIdArray );
        }

    // Close tag ID array.
    tagIdArray.Close();

    ////////////// Remove tags from VAS DB done //////////////

    FinishDeleteTags( KErrNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::FinishDeleteTags
// Cleans up and sends calbacks after
// deleting has finished or completed.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::FinishDeleteTags( TInt aSuccess )
    {
    RUBY_DEBUG1("CNssSpeechItemTrainer::FinishDeleteTags aSuccess [%d]", aSuccess);
    // Commit or uncommit according to success
    if ( iSrsApi )
        {
        if ( aSuccess == KErrNone )
            {
            CommitSIUpdate();
            }
        else{
            UncommitSIUpdate();
            }
        }
	
    // Free memory
    TInt count( 0 );
    if ( iSpeechItemTrainingBuffer )
        {        
        count = iSpeechItemTrainingBuffer->Count();
        RUBY_DEBUG1("CNssSpeechItemTrainer::FinishDeleteTags aSuccess Freeing [%d] items from the training buffer", count);
        iSpeechItemTrainingBuffer->Close();
        }
    delete iSpeechItemTrainingBuffer;
    iSpeechItemTrainingBuffer = 0;

    // Send callbacks
    MNssDeleteTagClient* client = iDeleteEventHandler;
    iDeleteEventHandler = 0;
    iTagDeleteCounter = 0;

    
    for ( TInt k( 0 ); k < count; k++ )
        {
        RUBY_DEBUG1("CNssSpeechItemTrainer::FinishDeleteTags sending callback for item [%d]", k);
        // Failed or succeedeed, one rule is processed
        iDeleteRuleIDArray->Remove(0);
       
        client->DeleteTagCompleted( aSuccess );
        }
    RUBY_DEBUG2("CNssSpeechItemTrainer::FinishDeleteTags Reported [%d] deletions. [%d] remaining. Setting idle state", count, iDeleteRuleIDArray->Count());
    SetState( EStateIdle );

    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::RunBufferedActionsL
// Called when it is time to execute the buffered actions
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::RunBufferedActionsL()
    {
    RUBY_DEBUG_BLOCK( "CNssSpeechItemTrainer::RunL" );
    
    if ( iState == ETrainStateWaiting )
        {
        DoTrainTextDelayed();
        }
    else if ( iState == ESaveStateWaiting )
        {
        DoSaveTags();
        }
    else if ( iState == EDeleteStateWaiting )
        {
        DoDeleteGroupedTags();
        }
    else if ( iState == ERetrainStateWaiting )
        {
        DoRetrainTextDelayed();
        }
    else
        {
        RUBY_DEBUG1( "CNssSpeechItemTrainer::RunL - ERROR: Wrong state (%d)", iState );
        }
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::RestartTimer
// Restarts the timer.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::RestartTimer()
    {
    TRAP_IGNORE( iActionTracker->ActionRequestedL() );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::CheckTrainingParametersL
// All tags in the queue should have the same training parameters. This
// function check that a new context has similar training parameters
// as the previous ones.
// ---------------------------------------------------------
//
TBool CNssSpeechItemTrainer::CheckTrainingParametersL( const CNssTrainingParameters* aParams )
    {
    if ( aParams )
        {
        if ( !iTrainingParams )
            {
            // Make a local copy of training parameters
            iTrainingParams = CNssTrainingParameters::NewL();

            const RArray<TLanguage>& paramLangArray = aParams->Languages();

            RArray<TLanguage>* langArray = new ( ELeave ) RArray<TLanguage>;
            CleanupStack::PushL( langArray );
                
            for ( TInt k = 0; k < paramLangArray.Count(); k++ )
                {
                TInt ret = langArray->Append( paramLangArray[ k ] );
                if ( ret != KErrNone )
                    {
                    langArray->Close();
                    delete langArray;
                    return EFalse;
                    }
                }
                
            // Copy training languages
            iTrainingParams->SetLanguages( langArray );
            CleanupStack::Pop( langArray );

            // Copy SINDE languages
            TRAP_IGNORE( iTrainingParams->SetSindeLanguages( aParams->SindeLanguagesL() ) );
            
            // Copy separator
            iTrainingParams->SetSeparator( aParams->Separator() );
            }
        else
            {
            // Compare with previous training parameters
            
            // Separator
            if ( aParams->Separator() != iTrainingParams->Separator() )
                {
                return EFalse;
                }

            // Training languages
            for ( TInt k = 0; k < aParams->Languages().Count(); k++ )
                {
                if ( aParams->Languages()[ k ] != iTrainingParams->Languages()[ k ] )
                    {
                    return EFalse;
                    }
                }
            
            // @todo SINDE languages should be checked also!            
            }
       }
       
    return ETrue;
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::CheckContext
// All tags in the queue should have the same contexts. This
// function check that a new context is similar to that of previous tags.
// ---------------------------------------------------------
//
TBool CNssSpeechItemTrainer::CheckContext(CNssContext& aContext)
    {
    if ( &aContext == 0 ) // Default settings - anything goes.
        {
        return( EFalse );
        }

    if ( aContext.RecognitionMode() != ENSSSdSiMode &&
         aContext.RecognitionMode() != ENSSSiMode )
        {
        return( EFalse );
        }

    if ( iContext )
        {
        // Check that identifiers (name & ID) are identical.
        if ( aContext.ContextId() != iContext->ContextId() )
            {
            return( EFalse );
            }

        if ( aContext.ContextName().Compare( iContext->ContextName() ) != 0 )
            {
            return( EFalse );
            }
        }
    else
        {
        if ( aContext.RecognitionMode()  == ENSSSdSiMode )
            {
            aContext.SetRecognitionMode( ENSSSiMode );
            }

        TRAPD( error, iContext = aContext.CopyL() );
        if ( error != KErrNone )
            {
            return EFalse;
            }
        }

    return( ETrue );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::HandleTrainComplete
// Called after speech recognition utility has trained all tags.
// Announces the newly assigned Rule IDs for the speech items.
// Signals the VAS client that training has been finished.
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::HandleTrainComplete( TInt aResult )
    {
    RUBY_DEBUG1( "CNssSpeechItemTrainer::HandleTrainComplete(%d)", aResult );

    // iRuleIdArray contains the rule IDs of the newly trained tags.
    // The following call makes sure that
    // iRuleIdArray.Count() == iSpeechItemTrainingBuffer.Count().
    if ( EnforceRuleIdCountInvariant( *iRuleIDArray, 
              iSpeechItemTrainingBuffer->Count() ) != KErrNone )
        {
        SendTrainingCallbacks( KErrNoMemory );
        }

    // Save rule IDs. Some rule IDs may have "KInvalidRuleID". This is a sign
    // that training failed. It's good to save also that info to speech items.
    for( TInt itemIdx = 0; itemIdx < iSpeechItemTrainingBuffer->Count(); itemIdx++ )
        {
        CNssSpeechItem* item = (*iSpeechItemTrainingBuffer)[ itemIdx ];
        TUint32    ruleID = (*iRuleIDArray)[ itemIdx ];

        item->DelayedTrainingComplete( ruleID );
        }

    SendTrainingCallbacks( aResult );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::SendTrainingCallbacks
// 
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::SendTrainingCallbacks( TInt aResult )
    {
    // If the changes were successful, commit them.
    // If they werent, roll back the changes.
    if ( aResult == KErrNone )
        {
        CommitSIUpdate();
        }
    else{
        UncommitSIUpdate();
        }

    // Save count (callback count)...
    TInt count = iSpeechItemTrainingBuffer->Count();
    MNssTrainTextEventHandler* callback = iTrainEventHandler;
    RArray<TUint32>* ruleIdArray = iRuleIDArray;
    iRuleIDArray = 0;

    // Delete buffered names that were just trained
    if ( iSpeechItemTrainingBuffer )
        {
        iSpeechItemTrainingBuffer->Close();
        }
    delete iSpeechItemTrainingBuffer;
    iSpeechItemTrainingBuffer = 0;


    // If there are names on the training queue,
    // restart the waiting of delayed training.
    if ( iSpeechItemBuffer && iSpeechItemBuffer->Count() > 0 )
        {
        // don't delete context etc as training will be continued
        SetState( ETrainStateWaiting );
        RestartTimer();
        }
    else
        {
        SetState( EStateIdle );
            
        // Cleanup
        delete iTrainingParams;
        iTrainingParams = 0;

        delete iContext;
        iContext = 0;
        
        iTrainEventHandler = 0;        
        }

    // Make the callbacks to the client according to the recognition result.
    for( TInt k( 0 ); k < count; k++ )
        {
        if ( ruleIdArray->Count() <= k )
            {
            RUBY_DEBUG2( "CNssSpeechItemTrainer::SendTrainingCallbacks ruleIdArray->Count() [%d] <= k [%d]", ruleIdArray->Count(), k );
            callback->HandleTrainComplete( KErrNoMemory );
            }
        if ( (*ruleIdArray)[ k ] == KInvalidRuleID )
            {
            RUBY_DEBUG0( "CNssSpeechItemTrainer::SendTrainingCallbacks (*ruleIdArray)[ k ] == KInvalidRuleID" );
            if ( aResult == KErrNone )
                {
                callback->HandleTrainComplete( KErrGeneral );
                }
            else
                {
                callback->HandleTrainComplete( aResult );
                }
            
            }
        else
            {
            callback->HandleTrainComplete( KErrNone );
            }
        }

    ruleIdArray->Close();
    delete ruleIdArray;
    ruleIdArray = 0;
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::MsruoEvent
// SRS Utility callback function 
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::MsruoEvent( TUid aEvent, TInt aResult )
    {
    switch( aEvent.iUid )
        {
        case KUidAsrEventAddVoiceTagsVal:
            RUBY_DEBUG0( "CNssSpeechItemTrainer::MsruoEvent - state is KUidAsrEventAddVoiceTagsVal" );

            if ( iState == ETrainStateTraining )
                {
                HandleTrainComplete( aResult );
                }
            else if ( iState == ERetrainStateRetraining )
                {
                HandleRetrainComplete( aResult );
                }
            break;

        case KUidAsrEventRemoveRuleVal:
            RUBY_DEBUG0( "CNssSpeechItemTrainer::MsruoEvent - state is KUidAsrEventRemoveRuleVal" );

            if ( aResult == KErrNone || aResult == KErrNotFound )
                {
                HandleRemoveTagComplete();
                }
            else{
                HandleRemoveTagFailed( aResult );
                }
            break;
            
        case KUidAsrEventRemoveRulesVal:
            RUBY_DEBUG0( "CNssSpeechItemTrainer::MsruoEvent - state is KUidAsrEventRemoveRulesVal" );

        	if( aResult == KErrNone || aResult == KErrNotFound )
        		{
        		HandleRemoveTagsComplete();
        		}
        	else 
        		{
        		HandleRemoveTagFailed( aResult );
        		}
        	break;

        default:
            RUBY_DEBUG2( "CNssSpeechItemTrainer::MsruoEvent - ERROR: Uknown state (%d,%d)", aEvent.iUid, aResult );
            break;
        }
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::SplitPhraseSindeL
// Splits a phrase to parts. Returns the parts in array.
// ---------------------------------------------------------
//
CDesC16ArrayFlat* CNssSpeechItemTrainer::SplitPhraseSindeL( const TDesC& aPhrase, 
                                                            TChar aSeparator )
    {
    CDesC16ArrayFlat* wordArray = new ( ELeave ) CDesC16ArrayFlat( KSindeWordArrayGranularity );
    CleanupStack::PushL( wordArray );

    // Separate the words in the phrase.
    TPtrC text = aPhrase; // Cursor to the phrase

    do
        {
        TInt index = text.Locate( aSeparator );

        if ( index != 0 )
            {
            // no more separators -> this is the last word -> end of word == end of string
            if ( index == KErrNotFound ) 
                {
                index = text.Length();
                }

                // Add word to array
                wordArray->AppendL( text.Left( index ) );
                }

        // Discard the processed word
        TInt charactersLeft = text.Length() - index; 
        // Forget the separator
        if ( charactersLeft > 0 )                    
            {
            charactersLeft--;                        
            }

        // Update text cursor
        text.Set( text.Right( charactersLeft ) );

        } 
    while( text.Length() > 0 );

    CleanupStack::Pop( wordArray );
    return wordArray;
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::SplitPhrase
// Splits a phrase to parts. Returns the parts in array.
// ---------------------------------------------------------
//
CDesC16ArrayFlat* CNssSpeechItemTrainer::SplitPhraseL( const TDesC& aPhrase, TChar aSeparator )
    {
    CDesC16ArrayFlat *wordArray = new (ELeave) CDesC16ArrayFlat( KSindeWordArrayGranularity );

    // Separate the words in the phrase. Especially firstname / lastname.
    TPtrC text = aPhrase; // Cursor to the phrase

    do
        {
        TInt index = text.Locate( aSeparator );

        // no more separators -> this is the last word -> end of word == end of string
        if ( index == -1 ) 
            {
            index = text.Length();
            }

        // Add word to array
        wordArray->AppendL( text.Left( index ) );

        // Discard the processed word
        TInt charactersLeft = text.Length() - index; 
        // Forget the separator
        // (the last word doesn't have one)
        if ( charactersLeft > 0 )                    
            {
            charactersLeft--;                        
            }

        // Update text cursor
        text.Set( text.Right( charactersLeft ) );

        } 
    while( text.Length() > 0 );

    return wordArray;
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::CommitSIUpdate
// Commits changes to SRS and destroys SRS utility.
// ---------------------------------------------------------
//
TInt CNssSpeechItemTrainer::CommitSIUpdate()
    {
    if ( !iSrsApi )
        {
        return( KErrNotReady );
        }

    iSrsApi->CommitChanges();

    delete iSrsApi;
    iSrsApi = NULL;

    return( KErrNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::UncommitSIUpdate
// Destroys SRS utility without committing.
// ---------------------------------------------------------
//
TInt CNssSpeechItemTrainer::UncommitSIUpdate()
    {
    if ( !iSrsApi )
        {
        return( KErrNotReady );
        }

    delete iSrsApi;
    iSrsApi = NULL;

    return( KErrNone );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::SpeechItemBufferNeeded
// Trys to allocate a speech item buffer
// ---------------------------------------------------------
//
TBool CNssSpeechItemTrainer::SpeechItemBufferNeeded()
    {
    if ( !iSpeechItemBuffer )
        {
        iSpeechItemBuffer = new RPointerArray<CNssSpeechItem>;
        if ( !iSpeechItemBuffer )
            {
            return( EFalse );
            }
        }

    return( ETrue );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::SpeechItemBufferNeeded
// Trys to allocate a grammar id buffer
// ---------------------------------------------------------
//
TBool CNssSpeechItemTrainer::GrammarIdBufferNeeded()
    {
    if ( !iGrammarIdBuffer )
        {
        iGrammarIdBuffer = new RArray<TUint32>;
        if ( !iGrammarIdBuffer )
            {
            return( EFalse );
            }
        }

    return( ETrue );
    }

// ---------------------------------------------------------
// CNssSpeechItemTrainer::SpeechItemBufferNeeded
// Trys to create SRS api. Returns success status.
// ---------------------------------------------------------
//
TInt CNssSpeechItemTrainer::CreateSrsApi()
    {
    if ( !iSrsApi )
        {
        TRAPD( err, iSrsApi = CNssSiUtilityWrapper::NewL( *this, KNssVASApiUid ) );
        REACT( err, return( err ) );

        iSrsApi->SetEventHandler( this );
        }

    return KErrNone;
    }


// ---------------------------------------------------------
// CNssSpeechItemTrainer::SpeechItems2Phrases
// Converts an array of speech items
// into Utility's AddVoiceTags array format.
// ---------------------------------------------------------
//
TInt CNssSpeechItemTrainer::SpeechItems2Phrases(
    RPointerArray<CNssSpeechItem>& aSpeechItems,
    RPointerArray<MDesCArray>& aPhrases )
    {
    // iPhraseArray is an array of arrays: 
    // 
    //        |-- firstname1
    //  tag1 -|-- lastname1
    //
    //  tag2 -|-- firstname2
    //        |-- lastname2

    // Create training parameters, if none were given
    if ( iTrainingParams == 0 )
        {
        RArray<TLanguage>* languages = new RArray<TLanguage>();

        if ( !languages )
            {
            return( KErrNoMemory );
            }

        // Use UI language + 1 other as default if languages are not given
        // in training parameters
        TInt err = languages->Append( User::Language() );

        if ( err == KErrNone )
            {
            err = languages->Append( ELangOther );
            }

        if ( err != KErrNone )
            {
            languages->Close();
            delete languages;
            return( err );
            }

        TRAP( err, iTrainingParams = CNssTrainingParameters::NewL() ); 
        if ( err != KErrNone )
            {
            languages->Close();
            delete languages;
            return( err );
            }

        iTrainingParams->SetLanguages( languages );
        }

    // Split the phrases to subwords (names -> first name + last name)
    for ( TInt k( 0 ); k < aSpeechItems.Count(); k++ )
        {
        // Split names into the first name and the last name
        CDesC16ArrayFlat *wordArray = 0;

        // Check if SINDE type of training should be used
        TRAPD( errSinde, const RArray<RTrainingLanguageArray>& temp = 
                                        iTrainingParams->SindeLanguagesL() );
        if ( errSinde == KErrNone )
            {
            TRAP_IGNORE( wordArray = SplitPhraseSindeL( aSpeechItems[ k ]->RawText(),
                                                        iTrainingParams->Separator() ) );
            }
        else
            {
            TRAP_IGNORE( wordArray = SplitPhraseL( aSpeechItems[ k ]->Text(),
                                                   iTrainingParams->Separator() ) );
            }

        if ( wordArray == 0 )
            {
            return KErrNoMemory;
            }

        TInt ret = aPhrases.Append( wordArray );

        if ( ret != KErrNone )
            {
            return ret;
            }
        }

    return( KErrNone );
    }
    
// ---------------------------------------------------------
// CNssSpeechItemTrainer::SetState
// Changes state of the trainer
// ---------------------------------------------------------
//
void CNssSpeechItemTrainer::SetState(TTrainState aState) 
	{
	RUBY_DEBUG2("CNssSpeechItemTrainer::SetState Switching state from [%d] to [%d]", iState, aState);
#ifdef _DEBUG
	if( iState == EDeleteStateWaiting ) 
		{
		RUBY_DEBUG0("CNssSpeechItemTrainer::SetState From EDeleteStateWaiting");
		}
	if( aState == EDeleteStateWaiting ) 
		{
		RUBY_DEBUG0("CNssSpeechItemTrainer::SetState To EDeleteStateWaiting");
		}
		
#endif	
	iState = aState;
	}

// ---------------------------------------------------------
// CNssSpeechItemTrainer::EnforceRuleIdCountInvariant
// Adds KInvalidRuleId to the Rule ID Array, until
// the number of rules is equal to count.
// ---------------------------------------------------------
//
TInt EnforceRuleIdCountInvariant( RArray<TUint32>& aArray, TInt aCount )
    {
    while( aArray.Count() < aCount )
        {
        TInt err = aArray.Append( KInvalidRuleID );
        if ( err != KErrNone )
            {
            return( err );
            }
        }

    return KErrNone;
    }
