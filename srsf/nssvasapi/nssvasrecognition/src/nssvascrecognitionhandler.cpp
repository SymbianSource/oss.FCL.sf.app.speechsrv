/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssRecognitionHandler class performs the recognition function.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32property.h>
#include "nssvascoreconstant.h"

#include "nssvascrecognitionhandler.h"
#include "nssvasctagmgr.h"
#include "nssvascadaptationitem.h"
#include <nsssispeechrecognitionutilityobserver.h>

#ifndef __WINS__
	// for hardware
    #include <AudioPreference.h>
#else
  // use default values for WINS
  const TUint KAudioPriorityVoiceDial = 73;

  const TUint KAudioPrefVocosPlayback = 0x00030001;
  const TUint KAudioPrefVocosTrain = 0x00020001;
  const TUint KAudioPrefVocosRecog = 0x00010001;
#endif
#include "nssvascdbtagselectnotifier.h"
#include "nssvascoreconstant.h"
#include "nssvasmrecognizeinitcompletehandler.h"
#include "nssvascvasdbbuilder.h"
#include "rubydebug.h"

// maximal number of results that can be handled
const TInt KMaxResults = 100;

#ifdef _DEBUG
// Used under UDEB only
_LIT( KRecognitionHandlerPanic, "VasCNssRecognitionHandler.cpp" );
#endif  // _DEBUG


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssRecognitionHandler* CNssRecognitionHandler::NewL()
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::NewL" );

    // version of NewLC which leaves nothing on the cleanup stack
    CNssRecognitionHandler* self=NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssRecognitionHandler* CNssRecognitionHandler::NewLC()
    {
    CNssRecognitionHandler* self = new (ELeave)  CNssRecognitionHandler();
    CleanupStack::PushL(self);
    // push onto cleanup stack 
    // (in case self->ConstructL leaves)
    self->ConstructL();      // use two-stage construct
    return self;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::~CNssRecognitionHandler
// Destructor
// -----------------------------------------------------------------------------
//
CNssRecognitionHandler::~CNssRecognitionHandler()
    {
    RUBY_DEBUG0( "CNssRecognitionHandler::~CNssRecognitionHandler" );
    
    Cleanup();
    
    for ( TInt k( 0 ); k < iAdaptItemArray.Count(); k++ )
        {
        iAdaptItemArray[k]->Disable();
        }
    iAdaptItemArray.Close();
    
    CNssVASDBBuilder::RemoveInstance();  // remove iVasDBBuilder instance
    
    TInt state( ERecognitionFail );
    RProperty::Get( KSINDUID, ERecognitionState, state );
    
    if ( state == ERecognitionStarted || state == ERecognitionSpeechEnd )
        {
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail );
        }
    // Delete P&S property
    RProperty::Delete( KSINDUID, ERecognitionState ); 
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::RecognizeInitL
// Method to perform initialization for the recognize funtion.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult CNssRecognitionHandler::RecognizeInitL(
    MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
    const CArrayPtrFlat<MNssContext>& aRecognitionVocabulary,
	TInt aMaxResults)
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::RecognizeInitL" );

    MNssRecognitionHandler::TNssRecognitionResult res = EVasErrorNone;

    // Vocabulary independent initialization:
    // CSISpeechRecognitionUtility and CSIClientResultSet.
    res = CommonRecognizeInitL( aInitCompleteHandler, aMaxResults );

    if ( res != EVasErrorNone )
        {
        return( res );
        }

    // Agenda for vocab dependent init:
    // 1. Get the list of global contexts from VAS DB.
    //    The information we need is:
    //     * Model Bank ID, which is the same for all contexts
    //     * Grammar ID for each context
    // 2. Load a model bank from SRS DB.
    // 3. Load the grammars from SRS DB. Each context has one grammar.
	if ( !iContextList )
	    {
		iContextList = new (ELeave) TMNssContextList(1);
        }
    else
        {
        iContextList->Reset();
        }


    for ( TInt k( 0 ); k < aRecognitionVocabulary.Count(); k++ )
        {
        CNssContext* context = ((CNssContext*)aRecognitionVocabulary[k])->CopyL();
        CleanupStack::PushL( context );

        iContextList->AppendL( context );

        CleanupStack::Pop( context );
        }
    iContextCount = iContextList->Count();

    // Check if there are tags in the contexts.
    // If there are, load a model bank. After that, further initializations
    // follow: Loading lexicons and grammars.
	// Check for Tags in contexts and continue recognition
	TRAPD( err, HandleTagCheckL() );

	if ( err != KErrNone )
	    {
        RUBY_DEBUG0( "CNssRecognitionHandler: TRAPD HandleTagCheckL() failed" );
		// Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
		Cleanup();
        return EVasRecognitionInitFailed;
	    }

    return( EVasErrorNone );
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::RecognizeInitL
// Method to perform initialization for the recognize funtion.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult CNssRecognitionHandler::RecognizeInitL(
		                   MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
						   TInt aMaxResults)
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::RecognizeInitL" );

    MNssRecognitionHandler::TNssRecognitionResult res = EVasErrorNone;

    // Vocabulary independent initialization:
    // CSISpeechRecognitionUtility and CSIClientResultSet.
    res = CommonRecognizeInitL( aInitCompleteHandler, aMaxResults );

    if ( res != EVasErrorNone )
        {
        return( res );
        }

    // Agenda for vocab dependent init:
    // 1. Get the list of global contexts from VAS DB.
    //    The information we need is:
    //     * Model Bank ID, which is the same for all contexts
    //     * Grammar ID for each context
    // 2. Load a model bank from SRS DB.
    // 3. Load the grammars from SRS DB. Each context has one grammar.

    if ( iContextList )
        {
        iContextList->Reset();
        delete iContextList;
        }

	// Get Global Context List from Vas Db
    iContextList = iContextMgr->GetGlobalContexts();
    
    if ( !iContextList )
        {
        return EVasRecognitionInitFailed;
        }

    iContextCount = iContextList->Count();


	// Check for Tags in contexts and continue recognition
	TRAPD( err, HandleTagCheckL() );

	if ( err != KErrNone )
	    {
        RUBY_DEBUG0( "CNssRecognitionHandler: TRAPD HandleTagCheckL() failed" );

		// Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
		Cleanup();
        return EVasRecognitionInitFailed;
	    }

	return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::Recognize
// Method to perform the recognize function.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult CNssRecognitionHandler::Recognize(
                                        MNssRecognizeEventHandler* aEventHandler )
    {
    RUBY_DEBUG0( "CNssRecognitionHandler::Recognize" );
    
    if ( iRState != EVasWaitingForRecognize )
        {
        RUBY_DEBUG0( "CNssRecognitionHandler::Recognize - Unexpected Request" );
        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        return EVasUnexpectedRequest;
        }
    
    iEventHandler = aEventHandler;
	   
    iRState = EVasWaitingForRecordStarted;
    
    RUBY_DEBUG0( "CNssRecognitionHandler: SRSF Record()" );
    
    // Asynchronous call: next expected event: ERecordStarted
    
    TInt errorCode = iSrsApi->Record( KNssVASRecordTime );
    
    if ( errorCode != KErrNone )
        {
        RUBY_DEBUG1( "CNssRecognitionHandler: Record Err = %d", errorCode );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        return EVasRecognitionFailed;
        }
    
    return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::PreStartSampling
// Method to start pre-sampling before the actual recognition start.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult 
CNssRecognitionHandler::PreStartSampling( MNssRecognizeInitCompleteHandler* aInitCompleteHandler )
    {
    RUBY_DEBUG0( "" ); 
    
    // State check
    if ( iRState != EVasIdle )
        {
        RUBY_DEBUG0( "Unexpected Request" );
        return EVasUnexpectedRequest;
        }
    
    // Check callback
    if ( !aInitCompleteHandler )
        {
        return EVasInvalidParameter;
        }    
    
    iInitCompleteHandler = aInitCompleteHandler;
    
    if ( !iSrsApi )
        {		
        TRAPD( err, CreateSrfL() );
        if ( err != KErrNone )
            {
            RUBY_DEBUG0( "SRS creation fails" );
            iSrsApi = NULL;
            Cleanup();
            return EVasRecognitionInitFailed;
            }    
        }
      
    iSrsApi->SetAudioPriority( KAudioPriorityVoiceDial, 
                               KAudioPrefVocosTrain, 
                               KAudioPrefVocosPlayback, 
                               KAudioPrefVocosRecog );               
    
    TInt errorCode = iSrsApi->PreStartSampling();
    
    if ( errorCode != KErrNone )
        {
        RUBY_DEBUG1( "PreStartSampling Err = %d", errorCode );
        Cleanup();
        return EVasRecognitionInitFailed;
        }    
    
    return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::SelectTagL
// Method to initiate the Select Tag event.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult CNssRecognitionHandler::SelectTagL(
															   MNssTag *aClientTag)
    {
    return SelectTagL( aClientTag, ETrue );
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::SelectTagL
// Method to initiate the Select Tag event.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult CNssRecognitionHandler::SelectTagL( 
                                                            MNssTag* aClientTag, 
                                                            TBool aNotifyHandlers )
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::SelectTagL" );
    
    if ( iRState != EVasWaitingForSelectTag )
        {
        RUBY_DEBUG0( "CNssRecognitionHandler::SelectTagL - Unexpected Request" );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        
        Cleanup();
        return EVasUnexpectedRequest;
        }
    
    if ( !aClientTag )
        {
        RUBY_DEBUG0( "CNssRecognitionHandler: Invalid Parameter" );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        
        Cleanup();
        return EVasInvalidParameter;
        }
    
    // Check if we want to notify GlobalContext handlers
    if ( !aNotifyHandlers )
        {
        // No; just cleanup
        Cleanup();
        }
    else
        { // Yes
        // make a copy of the client tag, aClientTag, so the client can delete it
        CNssTag* internalTag = ((CNssTag*)aClientTag)->CopyL();
        
        Cleanup();
        
        CNssDBTagSelectNotifier *selectNotifier = iVasDBBuilder->GetTagSelectNotifier();
        
        selectNotifier->HandleSelection(internalTag);
        
        delete internalTag;
        }
    
    return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::RejectTagL
// Method to perform Reject Tag, i.e. blacklisting.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult CNssRecognitionHandler::RejectTagL(
						   MNssRejectTagCompleteHandler* aRejectTagCompleteHandler,
		                   CArrayPtrFlat<MNssTag>* aClientTagList)
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::RejectTagL" );
    
    if ( iRState != EVasWaitingForSelectTag )
        {
        RUBY_DEBUG0( "CNssRecognitionHandler::RejectTagL - Unexpected Request" );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 	
        Cleanup();
        return EVasUnexpectedRequest;
        }
    
    // Begin a Loop to unload rules. A rule is contained in a tag.
    // The unload rule requests are Asynchronous. The 
    // loop is continued and completed in the event handler logic, 
    // MsruoEvent(aEvent=UnloadRuleComplete).
    // Loop through the input client tag list to build a Rule List, one rule 	
    // for each tag. Unload the rule corresponding to each tag.
    // iRejectTagCurrentLoopCount and iRejectTagCount control the loop.
    
    iRejectTagCompleteHandler = aRejectTagCompleteHandler;
    
    if (aClientTagList == 0)
        {
        // client tag list is NULL error
        RUBY_DEBUG0( "CNssRecognitionHandler: Invalid Parm: Tag List Null" );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        return EVasInvalidParameter;
        }
    iRejectTagCount = aClientTagList->Count(); 
    
    if (iRejectTagCount == 0)
        {
        // tag list is empty error
        RUBY_DEBUG0( "NssRecognitionHandler: Invalid Parm: Tag List Empty" );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        return EVasInvalidParameter;
        }
    
    iRejectTagCurrentLoopCount = 0; 
    
    if (!iRuleList)
        {
        iRuleList = new (ELeave) CArrayFixFlat<TRuleEntry>(iRejectTagCount);
        }
    
    TRuleEntry ruleEntry;
    
    for ( TInt index=0; index < iRejectTagCount; index++ )
        {
        CNssTag* tempTag = (CNssTag*)aClientTagList->At(index);
        CNssSpeechItem* speechItem = (CNssSpeechItem*)tempTag->SpeechItem();
        TUint32 ruleID = speechItem->RuleID();
        
        CNssContext* context = (CNssContext*)tempTag->Context();
        TUint32 grammarID = context->GrammarId();
        
        ruleEntry.ruleId = ruleID; 
        ruleEntry.grammarId = grammarID;
        
        iRuleList->AppendL(ruleEntry);
        }
    
    // get the first iRuleList entry = 0. The rest of the entries are
    // processed in the event handler, MsruoEvent(aEvent=UnloadRuleComplete)
    ruleEntry = iRuleList->At(iRejectTagCurrentLoopCount);
    TUint32 ruleID = ruleEntry.ruleId;
    TUint32 grammarID = ruleEntry.grammarId;
    
    iRState = EVasWaitingForUnloadRuleComplete;
    
    RUBY_DEBUG0( "CNssRecognitionHandler: SRSF UnloadRule()" );
    
    TInt errorCode = iSrsApi->UnloadRule( (TSIGrammarID)grammarID, ruleID );
    if ( errorCode )
        {
        RUBY_DEBUG1( "CNssRecognitionHandler: UnloadRule Err = %d", errorCode );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        return EVasRejectTagFailed;
        }
    
    return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::Cancel
// Method to cancel the last recognition function.
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult CNssRecognitionHandler::Cancel()
    {
    RUBY_DEBUG1( "CNssRecognitionHandler::Cancel() iRState = %d", iRState );

	// a Cancel request for the Idle state requires no action.	
    if ( iRState == EVasIdle )
        {
		return EVasErrorNone;    
        }
 
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 

	// for all states except Idle call Cleanup which calls SRF cancel and 
	// transitions to Idle state.
    Cleanup();
    return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::GetAdaptationItemL
// Method to get the adaptation data.
// -----------------------------------------------------------------------------
//
MNssAdaptationItem* CNssRecognitionHandler::GetAdaptationItemL(void)
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::GetAdaptationItemL" );
	
    // Check state
	if ( iRState != EVasWaitingForSelectTag )
    	{
        User::Leave( KErrNotReady );
    	}

    // Client Result Set should be there since the state is what it is.
    if ( !iSIClientResultSet )
        {
        User::Leave( KErrCorrupt );
        }

    // Construct adaptation item
    CNssAdaptationItem* adaptationItem = CNssAdaptationItem::NewL( this, iSIClientResultSet );
    // Adaptation item now owns the previous Client Result Set.
    iSIClientResultSet = 0; 

    // Add the item to our catalog of adaptation items.
    CleanupStack::PushL( adaptationItem );
    User::LeaveIfError( iAdaptItemArray.Append( adaptationItem ) );
    CleanupStack::Pop( adaptationItem );

    return( adaptationItem );
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::GetAdaptationItemL
// Method to get the adaptation data.
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::RemoveAdaptationItem(CNssAdaptationItem* anItem)
    {
    for ( TInt k( 0 ); k < iAdaptItemArray.Count(); k++ )
        {
        if ( iAdaptItemArray[k] == anItem )
            {
            iAdaptItemArray.Remove( k );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::MsruoEvent
// Method to process the events from SRS utility object
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::MsruoEvent( TUid aEvent, TInt aResult )
    {
    RUBY_DEBUG2( "CNssRecognitionHandler::MsruoEvent Event = 0x%x, Result = %d", aEvent.iUid, aResult );

    switch ( aEvent.iUid )
        {
        case KUidAsrEventLoadModelsVal:
            {
            if (aResult == KErrNone)
                {
                DoLoadModelsComplete();
                }
            else
                {
                DoLoadModelsFailed();
                }
            break;
            }
            
        case KUidAsrEventLoadGrammarVal:
            {
            if (aResult == KErrNone)
                {
                DoLoadGrammarComplete();
                }
            else
                {
                DoLoadGrammarFailed();
                }
            break;
            }	
        case KUidAsrEventRecognitionReadyVal:
            {
            if (aResult == KErrNone)
                {
                DoRecognitionReady();                
                }
            else
                {
                DoRecognitionFailed();
                }
            break;
            }	
        case KUidAsrEventRecognitionVal:
            {
            
            if (aResult == KErrNone)
                {
                DoRecognitionComplete();
                }
            else if ((aResult == KErrAsrNoSpeech) ||
                (aResult == KErrAsrSpeechTooEarly) ||
                (aResult == KErrAsrSpeechTooLong) ||
                (aResult == KErrAsrSpeechTooShort))
                {
                DoRecognitionFailedNoSpeech(aResult);
                }
            else if (aResult == KErrAsrNoMatch)
                {
                DoRecognitionFailedNoMatch();
                }
            // Handle Cancelled the same as Failed
            else
                {
                DoRecognitionFailed();
                }
            break;
            }	
        case KUidAsrEventRecordStartedVal:
            {
            DoRecordStarted();
            break;
            }
        case KUidAsrEventRecordVal:
            {
            if (aResult != KErrNone)
                {
                DoRecordFailed(aResult);
                }
            // ignore Record KErrNone
            break;
            }
        case KUidAsrEventEouDetectedVal:
            {
            DoEouDetected();
            break;
            }
            
        case KUidAsrEventUnloadRuleVal:
            {
            if (aResult == KErrNone)
                {
                DoUnloadRuleComplete();
                }
            else
                {
                DoUnloadRuleFailed();
                }
            break;
            }	
            
        case KUidAsrEventAdaptVal:
            {
            if ( iRState != EVasDirectAdaptation && iRState != EVasIdleAdaptation )
                {
#ifdef _DEBUG
                User::Panic( KRecognitionHandlerPanic, __LINE__ );
#endif
                return;
                }
            
            if ( iRState == EVasIdleAdaptation )
                {
                Cleanup();
                }
            
            if ( aResult == KErrNone )
                {
                iAdaptationHandler->HandleAdaptComplete( KErrNone );
                }
            else{
                iAdaptationHandler->HandleAdaptComplete( KErrGeneral );
                }
            iRState = EVasWaitingForSelectTag;
            break;
            }
            
        case KUidAsrEventRemoveRuleVal:
            {
            if ( iRState == EVasCorrectingPluginDatabase )
                {
                if ( iWait.IsStarted() )
                    {
                    iWait.AsyncStop();
                    }
                }
            else
                {
                Cleanup();
                }
            break;
            }
            
        case KUidAsrEventPreStartSamplingVal:
            {
            DoPreStartSamplingComplete( aResult );
            break;
            }     
               
        default:
            {
            UnexpectedEvent();
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::CNssRecognitionHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssRecognitionHandler::CNssRecognitionHandler()
    {
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::ConstructL" );
    iVasDBBuilder = CNssVASDBBuilder::InstanceL();
    iVasDBBuilder->InitializeL();
    
    iContextMgr = iVasDBBuilder->GetContextMgr();
    
    iTagMgr = iVasDBBuilder->GetTagMgr();
    
    iRState = EVasIdle;
    
    // Define P&S property
    TInt err = RProperty::Define( KSINDUID, ERecognitionState, RProperty::EInt); 
    if ( err != KErrNotFound && err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::CleanupRecognizeInitFailed
// Perform required cleanup to get back to the Idle state and issue the
// HandleRecognizeInitError event with RecognizeInitFailed error code
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::CleanupRecognizeInitFailed()
    {
    RUBY_DEBUG0( "CNssRecognitionHandler::CleanupRecognizeInitFailed" );
    
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
    
    Cleanup();
    iInitCompleteHandler->HandleRecognizeInitComplete(
        MNssRecognizeInitCompleteHandler::EVasRecognizeInitFailed );
    }	

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::CleanupRecognizeFailed
// Perform required cleanup to get back to the Idle state and issue the
// HandleRecognizeError event with RecognizeFailed error code
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::CleanupRecognizeFailed()
    {
    RUBY_DEBUG0( "CNssRecognitionHandler::CleanupRecognizeFailed" );
    
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
    
    Cleanup();
    iEventHandler->HandleRecognizeComplete( NULL,
        MNssRecognizeEventHandler::EVasRecognizeFailed);
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::CleanupRejectTagFailed
// Perform required cleanup to get back to the Idle state and issue the
// HandleRejectTagError event with RejectTagFailed error code
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::CleanupRejectTagFailed()
    {
    RUBY_DEBUG0( "CNssRecognitionHandler::CleanupRejectTagFailed" );
    
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 	
    
    Cleanup();
    iRejectTagCompleteHandler->HandleRejectTagComplete(
        MNssRejectTagCompleteHandler::EVasRejectTagFailed);
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::Cleanup
// Perform required cleanup to get back to the Idle state
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::Cleanup()
    {
    RUBY_DEBUG0( "CNssRecognitionHandler::Cleanup" );
    
    if ( iTagMgr )
        {
        iTagMgr->CancelGetTag();
        }
    if ( iContextMgr )
        {
        iContextMgr->CancelGetContext();
        }
    DeleteSrf();
    ClearContextList();
    ClearTagList();
    ClearRuleList();
    ClearContextStatus();
    
    iRState = EVasIdle;
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler:: DeleteSrf
// Gracefully terminate and delete the SRF object, iSrsApi.
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DeleteSrf()
    {
    if ( iSrsApi )
        {
        RUBY_DEBUG0( "CNssRecognitionHandler::DeleteSrf" );
        
        iSrsApi->CancelUtility();
        
        iSrsApi->EndRecSession();  // no need to check return code
        
        iSrsApi->SetEventHandler(NULL);
        
        delete iSrsApi;
        
        iSrsApi=NULL;
        }

    if ( iSIClientResultSet )
        {
        delete iSIClientResultSet;
        iSIClientResultSet = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::ClearContextList
// Delete the context objects from iContextList
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::ClearContextList()
    {
    if (iContextList)
        {
        iContextList->ResetAndDestroy();
        delete iContextList;
        iContextList = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::ClearTagList
// Delete the tag objects and tag list, iTagList
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::ClearTagList()
    {
    // The client deletes iTagList, its pointers,  and its tags
    // There is a window in RecognitionComplete while looping
    // through the ClientResultData and getting corresponding tags from
    // VasDb (GetTag). If a VasDb error occurs during this loop, 
    // Recognition still owns the iTagList tags and must delete them.
    // Otherwise at the end of the GetTag loop the list is given to the
    // client, and the client deletes the tags.
    
    // if iTagList is not NULL, delete iTagList and its tags.
    
    if ( iTagList )
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::ClearRuleList
// Delete the TRuleEntry objects from iRuleList
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::ClearRuleList()
    {
    if ( iRuleList )
        {
        iRuleList->Reset();
        delete iRuleList;
        iRuleList = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::ClearContextStatus
// Delete the entries from iContextStatus
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::ClearContextStatus()
    {
    if ( iContextStatus )
        {
        iContextStatus->Reset();
        delete iContextStatus;
        iContextStatus = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::UnexpectedEvent
// Unexpected event occurred. . 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::UnexpectedEvent()
    {
    RUBY_DEBUG1( "CNssRecognitionHandler - UnexpectedEvent() iRState = %d", iRState );
    
    switch ( iRState )
        {
        case EVasIdle:
            {
            RUBY_DEBUG0( "CNssRecognitionHandler - Idle, Unexpected Srs Event" );
            break;
            }
            
            // RecognitionInit
        case EVasWaitingForLoadModelsComplete:
        case EVasWaitingForLoadLexiconComplete:
        case EVasWaitingForLoadGrammarComplete:
        case EVasWaitingForRecognitionReadyRecognitionInit:
            {
            CleanupRecognizeInitFailed();
            break;
            }
            
            // Recognition
        case EVasWaitingForRecordStarted:
        case EVasWaitingForEouDetected:
        case EVasWaitingForRecognitionComplete:
            {
            CleanupRecognizeFailed();
            break;
            }
            
            // RejectTag
        case EVasWaitingForUnloadRuleComplete:
        case EVasWaitingForRecognitionReadyRejectTag:
            {
            CleanupRejectTagFailed();
            break;
            }
        case EVasWaitingForRecognize:
        case EVasWaitingForSelectTag:
            {
            // There is no event handler to return to.
            // Notify P&S
            RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 			
            Cleanup();
            RUBY_DEBUG1( "CNssRecognitionHandler - Invalid State = %d", iRState );
            break;
            }
        default:
            {
            // unknown state - do not know event handler to return to
            // Notify P&S
            RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
            Cleanup();
            RUBY_DEBUG1( "CNssRecognitionHandler - Unknown State = %d", iRState );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoPreStartSamplingComplete
// Handle PreStartSampling SRSF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoPreStartSamplingComplete( TInt aError )
    {
    iInitCompleteHandler->HandlePreSamplingStarted( aError );
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoLoadModelsComplete
// Handle LoadModelsComplete SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoLoadModelsComplete()
    {
    if ( iRState != EVasWaitingForLoadModelsComplete )
        {
        UnexpectedEvent();
        return;
        }
    /*	iRState = EVasWaitingForLoadLexiconComplete;
    TUint32 lexiconId = iContextList->
    At(iContextCurrentLoopCount)->LexiconId();
      
    TInt errorCode = iSrsApi->LoadLexicon( (TSILexiconID)lexiconId );
    if (errorCode)
        {
        CleanupRecognizeInitFailed();	
        }
    */
    iRState = EVasWaitingForLoadGrammarComplete;
    
    // iContextCurrentLoopCount is 0
    // set loop count to -1 so Load Grammar Complete converts it back to 0
    iContextCurrentLoopCount = -1;
    DoLoadGrammarComplete();
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoLoadModelsFailed
// Handle LoadModelsFailed SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoLoadModelsFailed()
    {
    if ( iRState != EVasWaitingForLoadModelsComplete )
        {
        UnexpectedEvent();
        return;
        }
    CleanupRecognizeInitFailed();	
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoLoadLexiconComplete
// Handle LoadLexiconComplete SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoLoadLexiconComplete()
    {
    if ( iRState != EVasWaitingForLoadLexiconComplete )
        {
        UnexpectedEvent();
        return;
        }
    iRState = EVasWaitingForLoadGrammarComplete;
    
    // iContextCurrentLoopCount is 0
    // set loop count to -1 so Load Grammar Complete converts it back to 0
    iContextCurrentLoopCount--;
    DoLoadGrammarComplete();
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoLoadLexiconFailed
// Handle LoadLexiconFailed SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoLoadLexiconFailed()
    {
    if ( iRState != EVasWaitingForLoadLexiconComplete )
        {
        UnexpectedEvent();
        return;
        }
    CleanupRecognizeInitFailed();			
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoLoadGrammarComplete
// Handle LoadGrammarComplete SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoLoadGrammarComplete()
    {
    if ( iRState != EVasWaitingForLoadGrammarComplete )
        {
        UnexpectedEvent();
        return;
        }
    // if there are more contexts, 
    // continue loading Grammars, into the SRS
    iContextCurrentLoopCount++;
    
    while ( iContextCurrentLoopCount < iContextCount )
        {
        TBool status = iContextStatus->At( iContextCurrentLoopCount );
        // if True, there are tags in the context load the grammar
        if ( status )
            {
            TUint32 grammarId = iContextList->
                At( iContextCurrentLoopCount )->GrammarId();
            TInt errorCode = iSrsApi->LoadGrammar( (TSIGrammarID) grammarId ); 
            if ( errorCode )
                {
                RUBY_DEBUG1( "CNssRecognitionHandler: LoadGrammar Err = %d", errorCode );
                CleanupRecognizeInitFailed();			
                }
            return;
            }
        
        // else False, skip the context and go to the next
        else
            {
            iContextCurrentLoopCount++;	
            }
        }
    
    // are done loading contexts;				
    iRState = EVasWaitingForRecognitionReadyRecognitionInit;
    
    RUBY_DEBUG0( "CNssRecognitionHandler: SRF Recognize" );
    
    // Asynchronous call: next expected event:  ERecognitionReady 
    // dereference iClientResult ptr to make it a reference 
    TInt errorCode = iSrsApi->Recognize( *iSIClientResultSet );
    
    if ( errorCode ) // Check if errorCode is zero (KErrNone) or non-zero.
        {
        RUBY_DEBUG1( "CNssRecognitionHandler: Recognize Err = %d", errorCode );
        CleanupRecognizeInitFailed();
        }							
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoLoadGrammarFailed
// Handle LoadGrammarFailed SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoLoadGrammarFailed()
    {
    if ( iRState != EVasWaitingForLoadGrammarComplete )
        {
        UnexpectedEvent();
        return;
        }
    CleanupRecognizeInitFailed();
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoRecognitionReady
// Handle RecognitionReady SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoRecognitionReady()
    {
    if ( iRState == EVasWaitingForRecognitionReadyRecognitionInit )
        {
        ClearContextStatus();
        iRState = EVasWaitingForRecognize;
        
        RUBY_DEBUG0( "CNssRecognitionHandler: HandleRecognizeInitComplete" );
       
        iInitCompleteHandler->HandleRecognizeInitComplete( 
            MNssRecognizeInitCompleteHandler::EVasErrorNone );
        }
    else if ( iRState == EVasWaitingForRecognitionReadyRejectTag )
        {
        iRState = EVasWaitingForRecognize;
        
        RUBY_DEBUG0( "CNssRecognitionHandler: HandleRejectTagComplete" );
        
        iRejectTagCompleteHandler->HandleRejectTagComplete(
            MNssRejectTagCompleteHandler::EVasErrorNone );
        }
    else
        {
        UnexpectedEvent();
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoRecognitionComplete
// Handle RecognitionComplete SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoRecognitionComplete()
    {
    if ( iRState != EVasWaitingForRecognitionComplete )
        {
        UnexpectedEvent();
        return;
        }
    
    TInt resultSetCount = iSIClientResultSet->ResultCount();
    
    // allocate the Tag List - the client is responsible for deleting this
    if (!iTagList)
        {
        iTagList = new CArrayPtrFlat<CNssTag>( 1 );
        if ( !iTagList )
            {
            RUBY_DEBUG0( "CNssRecognitionHandler: TRAPD new CTAG Array failed" );
            CleanupRecognizeFailed();
            return;
            }		
        }
    
    // Loop to convert result set data to tags and put the tags 
    // into a Tag List
    // Since getTag is asynchronous the loop continues at
    // member function GetTagCompleted(). Use loop control variables:
    // iTagCurrentLoopCount and iTagCount
    
    iTagCurrentLoopCount = 0;
    iTagCount = resultSetCount;
    /*
    const CSDClientResult& clientResult = iClientResultSet->At(0);
    */
    TInt count( iSIClientResultSet->ResultCount() );
    TInt k( 0 );
    TInt err( KErrNone );
    
    for ( k = 0; k < count; k++ )
        {
        const CSIClientResult* clientResult( NULL );
        TRAP( err, clientResult = &iSIClientResultSet->AtL( k ) );
        if ( err != KErrNone )
            {
            break;
            }
        
        // Get the tag from DB
        CNssTag* tag = iTagMgr->GetTag( 
            clientResult->GrammarID(),
            clientResult->RuleID() );
        
        if ( !tag )
            {
            RUBY_DEBUG0( "CNssRecognitionHandler::DoRecognitionComplete found a tag which is NOT in VAS database" );
            TNssRState oldState = iRState;
            iRState = EVasCorrectingPluginDatabase;

            TInt error = iSrsApi->RemoveRule( clientResult->GrammarID(), clientResult->RuleID() );
            if ( error == KErrNone && !iWait.IsStarted() )
                {
                iWait.Start();
                }
            iRState = oldState;
            }
        else
            {
            // Append it to the "our" tag list (which will be copied
            // to produce client tag list)
            TRAP( err, iTagList->AppendL( tag ) );
        
            if ( err != KErrNone )
                {
                break;
                }
            }
        }
    
    // If no tags could be found
    if ( iTagList->Count() == 0 )
        {
        RUBY_DEBUG0( "CNssRecognitionHandler::DoRecognitionComplete no tags found at all" );
        // Cleans and makes the "RecognizeFailed" callback.
        CleanupRecognizeFailed();
        return;
        }
    
    // Copy the tag list. The copy is sent to the client.
    CArrayPtrFlat<MNssTag>* clientTagList = NULL;
    
    clientTagList = new CArrayPtrFlat<MNssTag>(1);
    
    if ( clientTagList == 0 )
        {
        // Cleans and makes the "RecognizeFailed" callback.
        CleanupRecognizeFailed();
        return;
        }
    
    for ( k = 0; k < iTagList->Count(); k++ )
        {
        MNssTag* tag = STATIC_CAST( MNssTag*, (*iTagList)[k] );
        
        TRAP( err, clientTagList->AppendL( tag ) );
        
        if ( err != KErrNone )
            {
            RUBY_DEBUG0( "CRecognitionHandler: TRAPD DestList AppendL failed" );
            
            // Don't do the "ResetAndDestroy()" : iTagList still owns the tags.
            clientTagList->Reset();
            delete clientTagList;
            
            // Cleans and makes the "RecognizeFailed" callback.
            CleanupRecognizeFailed();
            return;
            }			
        }
    
    iRState = EVasWaitingForSelectTag;
    
    // After this, clientTagList owns the tags:
    iTagList->Reset();
    delete iTagList;
    iTagList = 0;
    
    RUBY_DEBUG0( "CRecognitionHandler: HandleRecognizeComplete" );
    
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionSuccess ); 
    
    iEventHandler->HandleRecognizeComplete(
        clientTagList, MNssRecognizeEventHandler::EVasErrorNone );
    
    //    const CSIClientResult& clientResult = iSIClientResultSet->AtL(0);
    
    /******************** Debug recognized tags *********************/
    /*
    RFs session;
    RFile file;
    _LIT( aFileName, "c:\\documents\\recotag.txt" );
    
      session.Connect();
      if ( file.Open( session, aFileName, EFileWrite ) != KErrNone )
      {
      if ( file.Create( session, aFileName, EFileWrite ) != KErrNone )
      {
      User::Panic( _L("SDContrlDebug"), __LINE__ );
      }
      }
      
        for ( TInt k( 0 ); k < iSIClientResultSet->ResultCount(); k++ )
        {
        const CSIClientResult& clientResult = iSIClientResultSet->AtL(k);
        
          TBuf8<100> debugBuf;
          debugBuf.AppendFormat( _L8("Recognized tag with RID:%d, GID:%d\n"), clientResult.RuleID(), clientResult.GrammarID() );
          file.Write( debugBuf );
          }
          
            
              file.Close();
              session.Close();
    */
    /****************************************************************/
    
    // get next tag
    /*	TUint32 grammarId = clientResult.GrammarID();
    TUint32 ruleId = clientResult.RuleID();
    
      TInt dbErrorCode;
      dbErrorCode = iTagMgr->GetTag( this, 
      grammarId, 
      ruleId);
      if (dbErrorCode!=KErrNone)
      {
        CleanupRecognizeFailed();
        }
    */
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoRecognitionFailedNoSpeech
// Handle RecognitionFailedNoSpeech, RecognitionFailedTooEarly, 
// RecognitionFailedTooLong, and RecognitionFailedTooShort SRF events 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoRecognitionFailedNoSpeech( TInt aResult )
    {
    if ( iRState != EVasWaitingForRecognitionComplete )
        {
        UnexpectedEvent();
        return;
        }
    
    MNssRecognizeEventHandler::TNssRecognizeError errCode;
    errCode = MNssRecognizeEventHandler::EVasRecognizeFailed;
    if ( aResult == KErrAsrNoSpeech )
        {
        errCode = MNssRecognizeEventHandler::EVasRecognitionFailedNoSpeech;
        }
    else if ( aResult == KErrAsrSpeechTooEarly )
        {
        errCode = MNssRecognizeEventHandler::EVasRecognitionFailedTooEarly;
        }
    else if ( aResult == KErrAsrSpeechTooLong )
        {
        errCode = MNssRecognizeEventHandler::EVasRecognitionFailedTooLong;
        }
    else if ( aResult == KErrAsrSpeechTooShort )
        {
        errCode = MNssRecognizeEventHandler::EVasRecognitionFailedTooShort;
        }
    
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
    
    Cleanup();
    iEventHandler->HandleRecognizeComplete( NULL, errCode );
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoRecognitionFailedNoMatch
// Handle RecognitionFailedNoMatch SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoRecognitionFailedNoMatch()
    {
    if ( iRState == EVasWaitingForRecognitionReadyRecognitionInit )
        {
        CleanupRecognizeInitFailed();
        }
    else if ( iRState == EVasWaitingForRecognitionComplete )
        {
        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        iEventHandler->HandleRecognizeComplete( NULL,
            MNssRecognizeEventHandler::EVasRecognitionFailedNoMatch );	
        }
    else if ( iRState == EVasWaitingForRecognitionReadyRejectTag )
        {
        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        iRejectTagCompleteHandler->HandleRejectTagComplete(
            MNssRejectTagCompleteHandler::EVasNoTagInContexts);
        }
    else
        {
        UnexpectedEvent();
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoRecognitionFailed
// Handle RecognitionFailed SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoRecognitionFailed()
    {
    switch ( iRState )
        {
        case EVasWaitingForRecognitionReadyRecognitionInit:
            {
            CleanupRecognizeInitFailed();
            break;
            }
        case EVasWaitingForRecordStarted:
        case EVasWaitingForEouDetected:
        case EVasWaitingForRecognitionComplete:
            {
            CleanupRecognizeFailed();
            break;
            }
        case EVasWaitingForRecognitionReadyRejectTag:
            {
            CleanupRejectTagFailed();
            break;
            }
        default:
            {
            UnexpectedEvent();
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoRecordStarted
// Handle RecordStarted SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoRecordStarted()
    {
    if ( iRState != EVasWaitingForRecordStarted )
        {
        UnexpectedEvent();
        return;
        }
    iRState = EVasWaitingForEouDetected;
    
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionStarted ); 
    
    iEventHandler->HandleRecordStarted(); 
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoRecordFailed
// Handle RecordFailed SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoRecordFailed( TInt aResult )
    {
    if ( iRState != EVasWaitingForEouDetected )
        {
        UnexpectedEvent();
        return;
        }
    if ( ( aResult == KErrDied ) ||
        ( aResult == KErrAccessDenied ) )
        {
        
        Cleanup();
        iEventHandler->HandleRecognizeComplete( NULL,
            MNssRecognizeEventHandler::EVasRecognizeAccessDeny );
        }
    else
        {
        CleanupRecognizeFailed();
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoEouDetected
// Handle EouDetected SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoEouDetected()
    {
    if ( iRState != EVasWaitingForEouDetected )
        {
        UnexpectedEvent();
        return;
        }
    iRState = EVasWaitingForRecognitionComplete;
    // Notify P&S
    RProperty::Set( KSINDUID, ERecognitionState, ERecognitionSpeechEnd ); 
    
    iEventHandler->HandleEouDetected();
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoUnloadRuleComplete
// Handle UnloadRuleComplete SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoUnloadRuleComplete()
    {
    if ( iRState != EVasWaitingForUnloadRuleComplete )
        {
        UnexpectedEvent();
        return;
        }
    // if there are more tags
    // continue unloading rules, from the SRS
    iRejectTagCurrentLoopCount++;
    if ( iRejectTagCurrentLoopCount < iRejectTagCount )
        {
        TRuleEntry ruleEntry = iRuleList->At( iRejectTagCurrentLoopCount );
        TUint32 ruleID = ruleEntry.ruleId;
        TUint32 grammarID = ruleEntry.grammarId;
        
        RUBY_DEBUG0( "CNssRecognitionHandler: SRF UnloadRule" );
        
        TInt errorCode = iSrsApi->UnloadRule( (TSIGrammarID)grammarID, ruleID );
        if ( errorCode )
            {
            RUBY_DEBUG1( "CNssRecognitionHandler: UnloadRule Err = %d", errorCode );

            CleanupRejectTagFailed();
            }
        }
    else
        {
        ClearRuleList();
        iRState = EVasWaitingForRecognitionReadyRejectTag;
        
        RUBY_DEBUG0( "CNssRecognitionHandler: SRF Recognize" );
        
        // dereference iClientResult ptr to make it a reference 
        //TInt errorCode = iSrsApi->Recognize(*iClientResultSet);
        TInt errorCode = iSrsApi->Recognize( *iSIClientResultSet );
        if ( errorCode )
            {
            RUBY_DEBUG1( "CNssRecognitionHandler: Recognize Err = %d", errorCode );
           
            CleanupRejectTagFailed();
            }	
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::DoUnloadRuleFailed
// Handle UnloadRuleFailed SRF event 
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::DoUnloadRuleFailed()
    {
    ClearRuleList();
    if ( iRState != EVasWaitingForUnloadRuleComplete )
        {
        UnexpectedEvent();
        return;
        }
    CleanupRejectTagFailed();
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::HandleTagCheckL
// Loop through the contexts to build the ContextStatus array.
// If there are any tags in any context,
// Then proceed on with recognition and load models,
// Else return with EVasNoTagInContexts.
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::HandleTagCheckL()
    {
    TBool anyTag = EFalse;
    CNssContext* context;
    
    if ( !iContextStatus )
        {
        iContextStatus = new (ELeave) CArrayFixFlat<TBool>( iContextCount );
        }
    
    __ASSERT_DEBUG( iContextCount == iContextList->Count(), User::Panic( KRecognitionHandlerPanic, __LINE__ ) );
    
    for ( TInt index=0; index < iContextCount; index++ )
        {
        context = iContextList->At( index );
        // if a tag exists in the requested context, status is set to true
        // if there are no tags in the requested context, status is set to false
        TBool status = iContextMgr->TagExist( context );
        
        iContextStatus->AppendL( status );
        if ( status )
            {
            // if a tag is found, set to True
            anyTag = ETrue;
            }
        }
    
    if ( anyTag )
        {
        // True: there are tags, continue recognition
        iRState = EVasWaitingForLoadModelsComplete;
        
        TUint32 modelBankID = iContextList->At( 0 )->ModelBankId();
        TInt errorCode = iSrsApi->LoadModels( (TSIModelBankID)modelBankID ); 
        if ( errorCode )
            {
            RUBY_DEBUG1( "CNssRecognitionHandler: LoadModels Err = %d", errorCode );
            
            CleanupRecognizeInitFailed();
            }
        }
    else
        {
        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        // False: There are no tags in any context.
        Cleanup();
        iInitCompleteHandler->HandleRecognizeInitComplete(
            MNssRecognizeInitCompleteHandler::EVasNoTagInContexts );
        }
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::CreateSrfL
// Create the SRF object, iSrsApi.
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::CreateSrfL()
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::CreateSrfL" );
    // passing the CNssRecognitionHandler object as an active event handler
    // and passing the VAS Api's UID
    iSrsApi	= CNssSiUtilityWrapper::NewL(*this, KNssVASApiUid);
    }

// -----------------------------------------------------------------------------
// CNssRecognitionHandler::AdaptL
// Starts adaptation. Called by MNssAdaptationItem.
// -----------------------------------------------------------------------------
//
void CNssRecognitionHandler::AdaptL( MNssAdaptationEventHandler* aHandler,
                                     CSIClientResultSet* aResult,
                                     TInt aCorrect )
    {
    RUBY_DEBUG_BLOCK( "CNssRecognitionHandler::AdaptL" );
    
    if ( !aHandler || !aResult )
        {
        User::Leave( KErrArgument );
        }
    
    // We can do an adaptation either
    //  * directly after a recognition (direct adaptation), or
    //  * when the state is idle (idle adaptation).
    // In IDLE   Adaptation: We need to create and destroy SRS Utility object.
    // In DIRECT Adaptation: We need to return to the same state where we left.
    
    if ( iRState == EVasIdle )
        {
        iRState = EVasIdleAdaptation;
        
        CreateSrfL();
        
        User::LeaveIfError( iSrsApi->Adapt( *aResult, aCorrect ) );
        }
    else if ( iRState == EVasWaitingForSelectTag )
        {
        iRState = EVasDirectAdaptation;
        
        if ( !iSrsApi )
            {
            User::Leave( KErrNotReady );
            }
        
        User::LeaveIfError( iSrsApi->Adapt( *aResult, aCorrect ) );
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    
    iAdaptationHandler = aHandler;
    }

// -----------------------------------------------------------------------
// CNssRecognitionHandler::CommonRecognizeInitL
//
// Called by RecognizeInitL -variants
// to perform those initializations,
// which do not depend on vocabulary.
// -----------------------------------------------------------------------
//
MNssRecognitionHandler::TNssRecognitionResult
CNssRecognitionHandler::CommonRecognizeInitL( 
                    MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
                    TInt aMaxResults )
    {
    // Agenda:
    // 1. Create CSISpeechRecognitoinUtility object
    //
    // 2. Call SetPrioritySettings & StartRecSession
    //    - nothing interesting happens there
    //
    // 4. Initialize CSIClientResultSet for storing recongition results
    //
    if ( iRState != EVasIdle )
        {
        RUBY_DEBUG0( "CNssRecognitionHandler::RecognizeInitL - Unexpected Request" );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        Cleanup();
        return EVasUnexpectedRequest;
        }
    
    // Check callback
    if ( !aInitCompleteHandler )
        {
        return EVasInvalidParameter;
        }
    iInitCompleteHandler = aInitCompleteHandler;
    
    // Check that aMaxResults is reasonable (100 is an arbitrary limit)
    if ( aMaxResults < 0 || aMaxResults > KMaxResults )
        {
        return EVasInvalidParameter; 
        }
    iMaxResults = aMaxResults;
    
    if ( !iSrsApi )
        {		
        // Put a trap in case instantiation of SRF leaves
        TRAPD( err, CreateSrfL() );
        if ( err != KErrNone )
            {
            RUBY_DEBUG0( "CNssRecognitionHandler::RecognizeInitL - create SRS fail" );
            
            iSrsApi = NULL;     // For clean up purpose
            
            // Notify P&S
            RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
            
            Cleanup();
            return EVasRecognitionInitFailed;
            }
        }
    
    // Set audio priority. If some other application wants to play/record
    // while we are recognizing, these priorities determine who gets the access.
    iSrsApi->SetAudioPriority( KAudioPriorityVoiceDial, 
                               KAudioPrefVocosTrain, 
                               KAudioPrefVocosPlayback, 
                               KAudioPrefVocosRecog );
    
    // speaker independent mode recognition session
    TInt errorCode = iSrsApi->StartRecSession( ENSSSiMode );
    
    if ( errorCode != KErrNone )
        {
        RUBY_DEBUG1( "CNssRecognitionHandler: StartRecSession Err = %d", errorCode );

        // Notify P&S
        RProperty::Set( KSINDUID, ERecognitionState, ERecognitionFail ); 
        
        Cleanup();
        return EVasRecognitionInitFailed;
        }
    
    // Create the structure for recognition results
    if ( !iSIClientResultSet )
        {
        iSIClientResultSet = CSIClientResultSet::NewL();
        }
    iSIClientResultSet->SetMaxResultsL( iMaxResults );
    
    return EVasErrorNone;
    }

// End of file
