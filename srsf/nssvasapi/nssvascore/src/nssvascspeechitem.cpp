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
* Description:  The CNssSpeechItem provides speech mechanism to request services 
*               from SRS.
*
*/


// INCLUDE FILES
#include <e32std.h>

#include "srsfbldvariant.hrh"
#include <nssvascoreconstant.h>
#include "nssvascspeechitem.h"
#include "nssvascspeechitemtrainer.h"
#include "nssvasctag.h"
#include "nssvasctrainingparameters.h"
#include "nssvasdbkonsts.h"
#include "rubydebug.h"

// CONSTANTS
// This value is used for maximum retry times
const TInt KVASSpeechItemMaxRetry = 2;   
// This value is used to reset the retry number 
const TInt KVASSpeechItemResetRetry = 0;

// Character which is used instead of separator marker
_LIT( KEmptySpace, " " );

// Panic category
_LIT( KSpeechItemPanic, "CNssSpeechItem" );

// -----------------------------------------------------------------------------
// CNssSpeechItem::CNssSpeechItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssSpeechItem::CNssSpeechItem( CNssContext& aContext, 
                                CNssSpeechItemSrsPortal& aPortal, 
                                CNssSpeechItemTrainer& aTrainer )
                              : iRuleID( KInvalidRuleID ),
                                iTrainedType( TNssVasCoreConstant::EVasNotTrained ),
                                iState( TNssSpeechItemConstant::EIdle ),
                                iContext( aContext ),
                                iPortal( aPortal ),
                                iTrainer( aTrainer ),
                                iTagId( KNssVASDbDefaultValue ),
                                iTrained( EFalse ),
                                iRecover( EFalse )
    {
    //Register to Portal
    iPortal.Register();
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::~CNssSpeechItem
// Destructor
// -----------------------------------------------------------------------------
//
CNssSpeechItem::~CNssSpeechItem()
    {
    delete iText;
    delete iTrimmedText;
    
    if ( iTtsData )
        {
        CleanPlayback();
        }
    
    iPortal.Deregister(); // Deregister to Portal
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::NewL
// Two-phased constructor.
// This is for new SpeechItem
// -----------------------------------------------------------------------------
//
CNssSpeechItem* CNssSpeechItem::NewL( CNssContext& aContext, 
                                      CNssSpeechItemSrsPortal& aPortal, 
                                      CNssSpeechItemTrainer& aTrainer )
    {
    CNssSpeechItem* self = new (ELeave) CNssSpeechItem( aContext, aPortal, aTrainer );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::ConstructL
// Symbian 2nd phase constructor can leave.
// This is overloaded function for SpeechItem from database
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::ConstructL( TUint32 aRuleID, const TDesC& aText )
    {
    SetTextL( aText );
    iRuleID = aRuleID;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::NewLC
// Two-phased constructor.
// This is overloaded function for SpeechItem from database
// -----------------------------------------------------------------------------
//
CNssSpeechItem* CNssSpeechItem::NewLC( CNssContext& aContext,
                                       CNssSpeechItemSrsPortal& aPortal,
                                       CNssSpeechItemTrainer& aTrainer,
                                       TUint32 aRuleID, const TDesC& aText )
    {
    CNssSpeechItem* self = new (ELeave) CNssSpeechItem( aContext, aPortal, aTrainer );
    CleanupStack::PushL( self );
    self->ConstructL( aRuleID, aText );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::NewL
// Two-phased constructor.
// This is overloaded function for SpeechItem from database
// -----------------------------------------------------------------------------
//
CNssSpeechItem* CNssSpeechItem::NewL( CNssContext& aContext,
                                      CNssSpeechItemSrsPortal& aPortal,
                                      CNssSpeechItemTrainer& aTrainer,
                                      TUint32 aRuleID, 
                                      const TDesC& aText )
    {
    CNssSpeechItem* self = NewLC( aContext, aPortal, aTrainer, aRuleID, 
                                  aText );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::PlayL
// Method to do play operation. In SI, this means synthesizing the name.
// -----------------------------------------------------------------------------
//
MNssSpeechItem::TNssSpeechItemResult
CNssSpeechItem::PlayL( MNssPlayEventHandler* aPlayEventHandler, TLanguage aLanguage )
    {
    if ( !iTtsData )
        {
        iTtsData = new (ELeave) TTtsData;
        }
    
    iTtsData->iStyle.iLanguage = aLanguage;
    
    RUBY_DEBUG1( "CNssSpeechItem::PlayL lang id: %d", iTtsData->iStyle.iLanguage );
    
    // Verify for valid state
    if (!(iState == TNssSpeechItemConstant::EIdle ||
        iState == TNssSpeechItemConstant::ETrainComplete))
        {
        RUBY_DEBUG0( "CNssSpeechItem::PlayL - wrong state" );

        return MNssSpeechItem::EVasUnexpectedRequest;
        }
    
    // Verify for valid event handler
    if ( aPlayEventHandler )
        {
        iPlayEventHandler = aPlayEventHandler;
        }
    else
        {
        return MNssSpeechItem::EVasInvalidParameter;
        }
    
    TRAPD( err, DoPlayL() );
    if ( err != KErrNone )
        {
        CleanPlayback();
        return MNssSpeechItem::EVasPlayFailed;
        }
    
    return MNssSpeechItem::EVasErrorNone;
    
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::PlayL
// Method to do play operation. In SI, this means synthesizing the name.
// -----------------------------------------------------------------------------
//
MNssSpeechItem::TNssSpeechItemResult 
CNssSpeechItem::PlayL( MNssPlayEventHandler* aPlayEventHandler )
    {
    // Use UI language by default
    // If synthesis engine does not support UI language for some reason,
    // then language identification will be used instead
    return PlayL( aPlayEventHandler, User::Language() );
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::DoPlayL
// Starts playing. Assumes that all checks have been done earlier.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::DoPlayL()
    {
    TTtsData* d = iTtsData;

    d->iTts = CNssTtsUtilityWrapper::NewL( *this );
    d->iStyleID = d->iTts->AddStyleL( d->iStyle );

    d->iParsedText = CTtsParsedText::NewL();
    d->iParsedText->SetTextL( Text() );

    d->iSegment.SetTextPtr( d->iParsedText->Text() );
    d->iSegment.SetStyleID( d->iStyleID );

    d->iParsedText->AddSegmentL( d->iSegment );

    d->iTts->OpenAndPlayParsedTextL( *d->iParsedText );

    iOldState = iState;
    iState = TNssSpeechItemConstant::EPlayStart;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::CleanPlayback
// Cleans playback and releases all memory.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::CleanPlayback()
    {
    TTtsData* d = iTtsData;

    if ( d->iTts )
        {
        d->iTts->Stop();
        d->iTts->Close();
        }

    delete d->iParsedText;
    d->iParsedText = NULL;

    delete d->iTts;
    d->iTts = NULL;

    delete iTtsData;
    iTtsData = NULL;

    iState = iOldState;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::MakePlayCallback
// Makes callback to VAS client. Uses error status from TTS Utility to decide
// callback and parameters.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::MakePlayCallback( TInt aResult )
    {
    MNssPlayEventHandler::TNssPlayResult res;

    if ( aResult == KErrNone )
        {
        iPlayEventHandler->HandlePlayComplete( MNssPlayEventHandler::EVasErrorNone );
        }
    else{
        switch( aResult )
            {
            case KErrNoMemory:
                res = MNssPlayEventHandler::EVasPlayNoMemory;
                break;

            case KErrAccessDenied:
                res = MNssPlayEventHandler::EVasPlayAccessDeny;
                break;

            default:
                res = MNssPlayEventHandler::EVasPlayFailed;
                break;
            }
        iPlayEventHandler->HandlePlayComplete( res );
        }
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::MapcCustomCommandEvent
// TTS may call this method if we had used custom interfaces,
// But we don't use custom interfaces.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::MapcCustomCommandEvent( TInt /*aEvent*/, TInt /*aError*/ )
    {
    // We don't expect to arrive here
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::MapcInitComplete
// TTS calls this method to announce that it has initialized playing.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration )
    {
    if ( aError != KErrNone )
        {
        CleanPlayback();
        MakePlayCallback( aError );
        }
    else
        {
        if ((iState == TNssSpeechItemConstant::EPlayStart) || 
            (iState == TNssSpeechItemConstant::EPlayMemoryStart)
            )
            {
            iPlayDuration = I64LOW( aDuration.Int64() );
            iPlayEventHandler->HandlePlayStarted(iPlayDuration);
            }
        else
            {
            NotifyHandlerForUnexpectedEvent();
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::MapcPlayComplete
// TTS calls this method to announce that playing has finished.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::MapcPlayComplete( TInt aError )
    {
    CleanPlayback();
    MakePlayCallback( aError );
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::RecordL
// Method to do record operation
// -----------------------------------------------------------------------------
//
MNssSpeechItem::TNssSpeechItemResult 
CNssSpeechItem::RecordL( MNssTrainVoiceEventHandler* aTrainVoiceEventHandler )
    {
    RUBY_DEBUG1( "CNssSpeechItem::RecordL tagid [%d]", iTagId );
    
    // Verify for valid state
    if (iState == TNssSpeechItemConstant::EIdle)
        {
        RUBY_DEBUG0( "CNssSpeechItem::RecordL - wrong state" );

        return MNssSpeechItem::EVasUnexpectedRequest;
        }
    
    // Verify for valid event handler
    if (aTrainVoiceEventHandler)
        {
        iTrainVoiceEventHandler = aTrainVoiceEventHandler;
        }
    else
        {
        return MNssSpeechItem::EVasInvalidParameter;
        }
    
    return MNssSpeechItem::EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::TrainVoiceL
// Method to do train operation
// For retraining, the previous utterance data need to be preserved for the 
// delete operation later.
// -----------------------------------------------------------------------------
//
MNssSpeechItem::TNssSpeechItemResult 
CNssSpeechItem::TrainVoiceL(MNssTrainVoiceEventHandler* /*aTrainVoiceEventHandler*/)
    {
    // This is SI, don't even try to train by voice.
    return MNssSpeechItem::EVasUnexpectedRequest;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::CancelL
// Method to do cancel operation by calling the SpeechRecognitionUtility Cancel
// method, and cleanup the SRS from the current transaction.
// -----------------------------------------------------------------------------
//
MNssSpeechItem::TNssSpeechItemResult CNssSpeechItem::CancelL()
    {
    RUBY_DEBUG1( "CNssSpeechItem::CancelL tagid [%d]", iTagId );

    //Verify a transaction is in progress
    if (iState == TNssSpeechItemConstant::EIdle)
        {   
        return MNssSpeechItem::EVasUnexpectedRequest;
        }
    
    // Do the Clean up
    CleanUp();
    
    return MNssSpeechItem::EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::TrainingCapabilities
// Method to get the Training capabilities
// -----------------------------------------------------------------------------
//
CArrayFixFlat<TNssVasCoreConstant::TNssTrainingCapability>* 
CNssSpeechItem::TrainingCapabilities()
    { 
    return NULL;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::RuleID
// Method to get the rule ID
// -----------------------------------------------------------------------------
//
TUint32 CNssSpeechItem::RuleID()
    {
    return iRuleID;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::SetRuleID
// Method to set the rule ID
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::SetRuleID(TUint32 aRuleID)
    {
    iRuleID = aRuleID;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::TrainedType
// Method to get the Trained Type
// -----------------------------------------------------------------------------
//
TNssVasCoreConstant::TNssTrainedType CNssSpeechItem::TrainedType()
    {
    return iTrainedType;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::SetTrainedType
// Method to set the Trained Type
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::SetTrainedType( TNssVasCoreConstant::TNssTrainedType aTrainType )
    {
    iTrainedType = aTrainType;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::Text
// Method to get the Text
// -----------------------------------------------------------------------------
//
TDesC& CNssSpeechItem::Text()
    {
    if ( iTrimmedText )
        {
        return *iTrimmedText;
        }

    if ( iText )
        {
        return *iText;
        }
    else
        {
        return (TDesC&)KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::RawText
// Method to get the raw text
// -----------------------------------------------------------------------------
//
TDesC& CNssSpeechItem::RawText()
    {
    if ( !iText )
        {
        return (TDesC&)KNullDesC;
        }
    
    return *iText;
    }
    
// -----------------------------------------------------------------------------
// CNssSpeechItem::PartialTextL
// Returns the part of the text which is indentified with the given ID
// -----------------------------------------------------------------------------
//
HBufC* CNssSpeechItem::PartialTextL( const TDesC& aIdentifier )
    {
    if ( !iText )
        {
        User::Leave( KErrNotFound  );
        }
        
    // Copy text for manipulation
    HBufC* partialText = HBufC::NewL( iText->Length() );
    CleanupStack::PushL( partialText );
    *partialText = *iText;
    TPtr partialTextPtr = partialText->Des();

    // Find all separator characters from text
    TInt markerIndex = partialTextPtr.Find( KNameSeparator );
    TBool found( EFalse );
    
    while ( markerIndex != KErrNotFound )
        {
        // Separator marker should never be the last character of string
        __ASSERT_ALWAYS( markerIndex <= partialTextPtr.Length(), User::Leave( KErrCorrupt ) );
        
        TBool removeThisPart( ETrue );
        
        if ( partialTextPtr.Mid( markerIndex + 1, 1 ) == aIdentifier )
            {
            found = ETrue;
            removeThisPart = EFalse;
            }
        
        if ( markerIndex == 0 )
            {
            // Remove separator marker & the next char (which is the identifier)
            partialTextPtr.Delete( markerIndex, KTrainingIndexSize );
            }
        else
            {
            // Replace separator marker & the next char
            partialTextPtr.Replace( markerIndex, KTrainingIndexSize, KEmptySpace );
            }         
        
        if ( removeThisPart )
            {
            // Find next separator
            TInt tempIndex = partialTextPtr.Find( KNameSeparator );
            if ( tempIndex != KErrNotFound )
                {
                partialTextPtr.Delete( markerIndex, tempIndex - markerIndex );
                }
            else
                {
                partialTextPtr.Delete( markerIndex, partialTextPtr.Length() );
                }
            }
        
        // Find the next marker
        markerIndex = partialTextPtr.Find( KNameSeparator );
        }
    
    if ( found )
        {
        CleanupStack::Pop( partialText );
        }
    else
        {
        CleanupStack::PopAndDestroy( partialText );
        User::Leave( KErrNotFound );
        }
    
    // Ownership transferred to client 
    return partialText;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::SetTextL
// Method to set the Text
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::SetTextL( const TDesC& aText )
    {
    if ( iText )
        {
        delete iText;
        iText = NULL;
        }
    iText = aText.Left( KNssVasDbTagName ).AllocL();
    
    if ( iTrimmedText )
        {
        delete iTrimmedText;
        iTrimmedText = NULL;
        }
    iTrimmedText = iText->AllocL();
    
    // Remove escape characters
    for ( TInt iCounter = 0; iCounter < iTrimmedText->Length(); iCounter ++ )
        {
        if ( (*iTrimmedText)[iCounter] == KNameSeparator()[0] )
            {
            TPtr ptr( iTrimmedText->Des() );
            if ( iCounter == iTrimmedText->Length() - 1 )
                {
                // Special handling for last char
                ptr.Delete( iCounter, 1 );
                }
            else
                {
                ptr.Replace( iCounter, KTrainingIndexSize, KEmptySpace );
                }
            }
        
        }
    TPtr ptr( iTrimmedText->Des() );
    ptr.Trim();
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::IsPlayingSupported
// Method to query playing support
// -----------------------------------------------------------------------------
//
TBool CNssSpeechItem::IsPlayingSupported()
    {
    if ( Text().Length() == 0 )
        {
        return EFalse;
        }

    return( ETrue );
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::TrainTextL
// Method to train a speech model from text
// -----------------------------------------------------------------------------
//
CNssSpeechItem::TNssSpeechItemResult CNssSpeechItem::TrainTextL(
                           MNssTrainTextEventHandler *aEventHandler,
                           CNssTrainingParameters    *aTrainingParams )
    {
    
    if ( /*aEventHandler == 0 ||*/ Text().Length() == 0 )
        {
        return CNssSpeechItem::EVasInvalidParameter;
        }
    
    RUBY_DEBUG1( "CNssSpeechItem::TrainTextL tagig [%d]", iTagId );
    
    // Verify for the valid state
    if (iState != TNssSpeechItemConstant::EIdle && 
        iState != TNssSpeechItemConstant::ETrainComplete )
        {
        RUBY_DEBUG0( "CNssSpeechItem::TrainTextL - wrong state" );
       
        return MNssSpeechItem::EVasUnexpectedRequest;
        }
    
    // Verify for the valid event handler
    if ( !aEventHandler )
        {
        return MNssSpeechItem::EVasInvalidParameter;
        }
    
    
    iState = TNssSpeechItemConstant::ETrainStart;
    
    // To diffrentiate from voice trained tags
    iTrainedType = TNssVasCoreConstant::EVasTrainText;
    
    // Training assigns rule ID from KInvalidRuleID to some correct value.
    if ( iRuleID == KInvalidRuleID )
        {
        return( iTrainer.TrainTextDelayed( aEventHandler, aTrainingParams, *this, iContext ));
        }
    else
        {
        return( iTrainer.RetrainTextDelayed( aEventHandler, aTrainingParams, *this, iContext ));
        }
    
    // Use delayed training to train the new phrase.
    //return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::NSSBeginSaveToSrsL
// Method to begin the transaction for saving the utterance data to SRS 
// -----------------------------------------------------------------------------
//
MNssCoreSrsDBEventHandler::TNssSrsDBResult 
CNssSpeechItem::NSSBeginSaveToSrs( MNssSaveTagClient* aSaveTagClient, CNssTag* aTag )
    {
    RUBY_DEBUG1( "CNssSpeechItem::BeginSaveToSrsL tagid: [%d]", aTag->TagId() );
    
    // The state should be in the Training Complete
    if (iState != TNssSpeechItemConstant::ETrainComplete)
        {
        RUBY_DEBUG0( "CNssSpeechItem::BeginSaveToSrsL - wrong state" );

        return MNssCoreSrsDBEventHandler::EVasUnexpectedRequest;
        }
    
    TInt ret = iTrainer.SaveTagDelayed( aSaveTagClient, *aTag );
    
    if ( ret != KErrNone )
        {
        return MNssCoreSrsDBEventHandler::EVasUnexpectedRequest;
        }
    
    return MNssCoreSrsDBEventHandler::EVasSuccess;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::NSSBeginDeleteFromSrsL
// Method to begin the transaction for saving the utterance data to SRS 
// -----------------------------------------------------------------------------
//
MNssCoreSrsDBEventHandler::TNssSrsDBResult 
CNssSpeechItem::NSSBeginDeleteFromSrs( MNssDeleteTagClient* aDeleteTagClient, CNssTag* aTag )
    {
    RUBY_DEBUG1( "CNssSpeechItem::BeginDeleteFromSrsL tagid: [%d]", aTag->TagId() );
    
    // The state should be in the Training Complete
    if (iState != TNssSpeechItemConstant::ETrainComplete &&
        iState != TNssSpeechItemConstant::EIdle )
        {
        RUBY_DEBUG0( "CNssSpeechItem::BeginSaveToSrsL - wrong state" );
        
        return MNssCoreSrsDBEventHandler::EVasUnexpectedRequest;
        }
    
    TInt ret = iTrainer.DeleteTagDelayed( aDeleteTagClient, *aTag );
    
    if ( ret != KErrNone )
        {
        return MNssCoreSrsDBEventHandler::EVasUnexpectedRequest;
        }
    
    return MNssCoreSrsDBEventHandler::EVasSuccess;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::CommitSrsChanges
// Method to to commit the transaction to the SRS.
// This is a SYNCHRONOUS method
// -----------------------------------------------------------------------------
//
MNssCoreSrsDBEventHandler::TNssSrsDBResult CNssSpeechItem::CommitSrsChanges()
    {
    // Not used
    User::Panic( KSpeechItemPanic, __LINE__ );
    return MNssCoreSrsDBEventHandler::EVasSuccess;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::CommitSrsChanges
// Method to to roll back the transaction to the SRS.
// This is a SYNCHRONOUS method
// -----------------------------------------------------------------------------
//
MNssCoreSrsDBEventHandler::TNssSrsDBResult CNssSpeechItem::RollbackSrsChanges()
    {
    // Not used
    User::Panic( KSpeechItemPanic, __LINE__ );
    return MNssCoreSrsDBEventHandler::EVasSuccess;
    }


// -----------------------------------------------------------------------------
// CNssSpeechItem::CopyUtteranceData
// Method to copy utterance data from current data to transient data
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::CopyUtteranceData()
    {
    iTransientRuleID = iRuleID;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::MsruoEvent
// Method to process the events from SRS utility object
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::MsruoEvent( TUid aEvent, TInt aResult )
    {
    RUBY_DEBUG2( "CNssSpeechItem::MsruoEvent, Event=0x%x, Result=%d", aEvent.iUid, aResult  );
    
    switch (aEvent.iUid)
        {
        
        
        //----------------------------------------------------------------------
        //  Pre-Train Events 
        //----------------------------------------------------------------------
        
        
        case KUidAsrEventGetModelCountVal:
            RUBY_ERROR0( "CNssSpeechItem::MsruoEvent KUidAsrEventAddRuleVal not expected" );
            break;
                        
        case KUidAsrEventGetAvailableStorageVal:
            if (aResult == KErrNone)
                {
                DoGetAvailableStorageCompleteEvent();
                }
            else
                {
                DoTrainFailEvent(aResult);
                }
            break;
            
            
            //----------------------------------------------------------------------
            //  Train Events 
            //----------------------------------------------------------------------
            
        case KUidAsrEventTrainReadyVal:
            RUBY_DEBUG0( "CNssSpeechItem::MsruoEvent - TrainReadyEvent" );
            
            // Always success, no need to verify aResult
            if (iState == TNssSpeechItemConstant::ETrainStart)
                {
                iTrainVoiceEventHandler->HandleReadyToRecord();
                }
            else
                {
                NotifyHandlerForUnexpectedEvent();
                }
            break;
            
        case KUidAsrEventRecordStartedVal:
            RUBY_DEBUG0( "CNssSpeechItem::MsruoEvent - RecordStartedEvent" );
            
            // Always success, no need to verify aResult
            if (iState == TNssSpeechItemConstant::ETrainStart)
                {
                iTrainVoiceEventHandler->HandleRecordStarted();
                }
            else
                {
                NotifyHandlerForUnexpectedEvent();
                }
            break;
        case KUidAsrEventEouDetectedVal:
            RUBY_DEBUG0( "CNssSpeechItem::MsruoEvent - EouDetectedEvent" );
            
            // Always success, no need to verify aResult
            if (iState == TNssSpeechItemConstant::ETrainStart)
                {
                iTrainVoiceEventHandler->HandleEouDetected();
                }
            else
                {
                NotifyHandlerForUnexpectedEvent();
                }
            break;
            
        case KUidAsrEventTrainVal:
            if (aResult == KErrNone)
                {
                DoTrainCompleteEvent();
                }
            else
                {
                DoTrainFailEvent(aResult);
                }
            break;
            
            // currently this is only for fail cases
        case KUidAsrEventRecordVal: 
            RUBY_DEBUG0( "CNssSpeechItem::MsruoEvent - ERecordFailed" );

            // If the result is success, do nothing
            if (aResult != KErrNone)
                {
                DoTrainFailEvent(aResult);
                }
            break;
            
            
            // End of Train Events 
            
            
            //----------------------------------------------------------------------
            //  Save Events 
            //----------------------------------------------------------------------
            
        case KUidAsrEventAddRuleVal:
            RUBY_ERROR0( "CNssSpeechItem::MsruoEvent KUidAsrEventAddRuleVal not expected" );
            break;
            
            // End of Save to Srs Events  
            
            
            //----------------------------------------------------------------------
            //  Delete Events 
            //----------------------------------------------------------------------
            
            
        case KUidAsrEventRemoveRuleVal:
            RUBY_ERROR0( "CNssSpeechItem::MsruoEvent KUidAsrEventRemoveRuleVal not expected" );
            break;
            
            // End of Deleting from Srs Events  
            
            
        default:
            RUBY_DEBUG0( "CNssSpeechItem::MsruoEvent ERROR:default in switch" );
            
            NotifyHandlerForUnexpectedEvent();
            break;
    }
}



//  -------------------------       Play       -----------------------------  //

//  -------------------------       Train       ----------------------------  //


// -----------------------------------------------------------------------------
// CNssSpeechItem::DoGetAvailableStorageCompleteEvent
// Method to handle the "GetAvailableStorage" Event.
// Verify the size of available storage, if it is not zero, continue with
// the next operation to train.
// If the size is zero, send the error message.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::DoGetAvailableStorageCompleteEvent()
    {
    // Obsolete
    User::Panic( KSpeechItemPanic, __LINE__ );
    }


// -----------------------------------------------------------------------------
// CNssSpeechItem::DoTrainCompleteEvent
// Method to process the Train event.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::DoTrainCompleteEvent()
    {
    RUBY_DEBUG0( "CNssSpeechItem::DoTrainCompleteEvent" );

    if (iState == TNssSpeechItemConstant::ETrainStart)
        {
        iState = TNssSpeechItemConstant::ETrainComplete;
        //iTrainVoiceEventHandler->HandleTrainComplete();
        iTrainVoiceEventHandler->HandleTrainComplete( KErrNone );
        }
    else
        {
        NotifyHandlerForUnexpectedEvent();
        }
    }



// -----------------------------------------------------------------------------
// CNssSpeechItem::DoTrainFailEvent
// Method to process the train failed events
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::DoTrainFailEvent( TInt aResult )
    {
    RUBY_DEBUG1( "CNssSpeechItem::DoTrainFailEvent [%d]", aResult );
    
    if (iState != TNssSpeechItemConstant::ETrainStart)
        {
        NotifyHandlerForUnexpectedEvent();
        return;
        }
    
    CleanUp();
    MNssTrainVoiceEventHandler::TNssTrainResult errorCode = DoTrainResult( aResult );
    
    iTrainVoiceEventHandler->HandleTrainComplete( errorCode );
    }



// -----------------------------------------------------------------------------
// CNssSpeechItem::DoTrainResult
// Method to process the Train result.
// -----------------------------------------------------------------------------
//
MNssTrainVoiceEventHandler::TNssTrainResult 
CNssSpeechItem::DoTrainResult( TInt aResult )
    {
    
    MNssTrainVoiceEventHandler::TNssTrainResult errorCode;
    
    switch (aResult)
        {
        case KErrAsrNoSpeech:
            RUBY_DEBUG0( "CNssSpeechItem::DoTrainResult - EVasTrainFailedNoSpeech" );
           
            errorCode = MNssTrainVoiceEventHandler::EVasTrainFailedNoSpeech;
            break;
        case KErrAsrSpeechTooEarly:
            RUBY_DEBUG0( "CNssSpeechItem::DoTrainResult - EVasTrainFailedTooEarly" );

            errorCode = MNssTrainVoiceEventHandler::EVasTrainFailedTooEarly;
            break;
        case KErrAsrSpeechTooLong:
            RUBY_DEBUG0( "CNssSpeechItem::DoTrainResult - EVasTrainFailedTooLong" );
            
            errorCode = MNssTrainVoiceEventHandler::EVasTrainFailedTooLong;
            break;
        case KErrAsrSpeechTooShort:
            RUBY_DEBUG0( "CNssSpeechItem::DoTrainResult - KErrAsrSpeechTooShort" );

            errorCode = MNssTrainVoiceEventHandler::EVasTrainFailedTooShort;
            break;
        case KErrAccessDenied:      // this is temporary .....
        case KErrDied:
            RUBY_DEBUG0( "CNssSpeechItem::DoTrainResult - EVasTrainAccessDeny" );

            errorCode = MNssTrainVoiceEventHandler::EVasTrainAccessDeny;
            break;
        case KErrNoMemory:
            RUBY_DEBUG0( "CNssSpeechItem::DoTrainResult - EVasTrainNoMemory" );

            errorCode = MNssTrainVoiceEventHandler::EVasTrainNoMemory;
            break;
        default:
            RUBY_DEBUG0( "CNssSpeechItem::DoTrainResult - default" );

            errorCode = MNssTrainVoiceEventHandler::EVasTrainFailed;
            break;
        }
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::DoSrsDBResult
// Method to process the SRS DB result.
// -----------------------------------------------------------------------------
//
MNssCoreSrsDBEventHandler::TNssSrsDBResult 
CNssSpeechItem::DoSrsDBResult( TInt aResult )
    {
    
    MNssCoreSrsDBEventHandler::TNssSrsDBResult errorCode;
    
    switch (aResult)
        {
        case KErrDiskFull:
            RUBY_DEBUG0( "CNssSpeechItem::DoSrsDBResult - KErrDiskFull" );
            
            errorCode = MNssCoreSrsDBEventHandler::EVasDiskFull;
            break;
        case KErrNoMemory:
            RUBY_DEBUG0( "CNssSpeechItem::DoSrsDBResult - EVasNoMemory" );
            
            errorCode = MNssCoreSrsDBEventHandler::EVasNoMemory;
            break;
        default:
            {
            RUBY_DEBUG0( "CNssSpeechItem::DoSrsDBResult - default" );

            switch ( iState )
                {
                case TNssSpeechItemConstant::ESaveToSrsStart:
                    errorCode = MNssCoreSrsDBEventHandler::EVasAddPronunciationFailed;
                    break;
                case TNssSpeechItemConstant::EAddPronounciationComplete:
                    errorCode = MNssCoreSrsDBEventHandler::EVasAddRuleFailed;
                    break;
                case TNssSpeechItemConstant::EDeleteToSrsStart:
                    errorCode = MNssCoreSrsDBEventHandler::EVasRemoveRuleFailed;
                    break;
                case TNssSpeechItemConstant::ERemoveRuleComplete:
                    errorCode = MNssCoreSrsDBEventHandler::EVasRemovePronunciationFailed;
                    break;
                case TNssSpeechItemConstant::ERemovePronounciationComplete:
                    errorCode = MNssCoreSrsDBEventHandler::EVasRemoveModelFailed;
                    break;
                default:
                    errorCode = MNssCoreSrsDBEventHandler::EVasDBOperationError;
                    break;
                }
            }
        }
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::NotifyHandlerForUnexpectedEvent
// Method to notify the event handler for receiving the Unexpected Event
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::NotifyHandlerForUnexpectedEvent()
    {
    RUBY_DEBUG1( "CNssSpeechItem::NotifyHandlerForUnexpectedEvent tagid: %d", iTagId );
    
    switch( iState )
        {
        case TNssSpeechItemConstant::EAddPronounciationComplete:
        case TNssSpeechItemConstant::EAddRuleComplete:
        case TNssSpeechItemConstant::ERemoveRuleComplete:
        case TNssSpeechItemConstant::ERemovePronounciationComplete:
        case TNssSpeechItemConstant::ERemoveModelCompelete:
        case TNssSpeechItemConstant::EWaitForCommitChanges:
            // Shouldn't happen, since all calls to SRS
            // are done at CNssSpeechItemTrainer.
            User::Panic( KSpeechItemPanic, __LINE__ );
            
            break;
            
        case TNssSpeechItemConstant::EPlayMemoryStart:
        case TNssSpeechItemConstant::EPlayStart:
            if (iPlayEventHandler)
                {
                CleanUp();
                iPlayEventHandler->HandlePlayComplete(
                    MNssPlayEventHandler::EVasPlayFailed);
                }
            break;
            
        case TNssSpeechItemConstant::ETrainStart:
            if (iTrainVoiceEventHandler)
                {
                CleanUp();
                iTrainVoiceEventHandler->HandleTrainComplete(
                    MNssTrainVoiceEventHandler::EVasTrainFailed);
                }
            break;
            
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::SetTagId
// Method to set the Tag ID
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::SetTagId( TInt aTagId )
    {
    iTagId = aTagId;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::TagId
// Method to return the Tag ID
// -----------------------------------------------------------------------------
//
TInt CNssSpeechItem::TagId()
    {
    return iTagId;
    }		

// -----------------------------------------------------------------------------
// CNssSpeechItem::GrammarId
// Method to return the Grammar ID
// -----------------------------------------------------------------------------
//
TUint32 CNssSpeechItem::GrammarId()
    {
    return iContext.GrammarId();
    }		

// -----------------------------------------------------------------------------
// CNssSpeechItem::SetTrained
// Method to set the Trained flag
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::SetTrained( TBool aTrained )
    {
    iTrained = aTrained;
    }		

// -----------------------------------------------------------------------------
// CNssSpeechItem::Trained
// Method to return the Trained flag
// -----------------------------------------------------------------------------
//
TBool CNssSpeechItem::Trained()
    {
    return iTrained;
    }		

// ---------------------------------------------------------
// CNssSpeechItem::CopyL
// creates a copy of this SpeechItem
// ---------------------------------------------------------
//
CNssSpeechItem* CNssSpeechItem::CopyL( CNssContext* aContext )
    {
    RUBY_DEBUG1( "CNssSpeechItem::CopyL tagid: %d", iTagId );
    
    CNssSpeechItem* copy = new (ELeave) CNssSpeechItem(*aContext, iPortal, iTrainer);
    
    if ( iText )
        {
        copy->iText = iText->Alloc();
        }
    if ( iTrimmedText )
        {
        copy->iTrimmedText = iTrimmedText->Alloc();
        }
    copy->iRuleID = iRuleID;
    copy->iTransientRuleID = iTransientRuleID;
    
    copy->iTrainedType = iTrainedType;
    copy->iState = iState;
    copy->iContext = iContext;
    copy->iPortal = iPortal;
    copy->iPlayEventHandler = iPlayEventHandler;
    //copy->iSrsDBEventHandler = iSrsDBEventHandler;
    copy->iTrainVoiceEventHandler = iTrainVoiceEventHandler;
    
    copy->iTagId = iTagId;
    copy->iTrained = iTrained;
    copy->iPlayDuration = iPlayDuration;
    
    
    // set the iSrsApi to NULL
    // set the iRetry to NULL
    
    // set the state to Idle
    copy->iState = TNssSpeechItemConstant::EIdle;
    // set iRecover to False
    copy->iRecover = EFalse;
    
    return copy;
    }

// ---------------------------------------------------------
// CNssSpeechItem::CleanUp
// Method to do the clean up
// ---------------------------------------------------------
//
void CNssSpeechItem::CleanUp()
    {
    RUBY_DEBUG1( "CNssSpeechItem::CleanUp tagid: %d", iTagId );

    if ( iTtsData )
        {
        CleanPlayback();
        }
    }

// ---------------------------------------------------------
// CNssSpeechItem::DoRetry
// Method to verify whether retry is already max out,
// if not return ETrue, else return EFalse
// ---------------------------------------------------------
//
TBool CNssSpeechItem::DoRetry()
    {
    if ( iRetry < KVASSpeechItemMaxRetry )
        {
        iRetry++;
        return ETrue;
        }
    else
        {
        // Reset the retry count
        iRetry = KVASSpeechItemResetRetry;
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CNssSpeechItem::DelayedTrainingComplete
// Updates the speech item, after SI training has been carried out.
// -----------------------------------------------------------------------------
//
void CNssSpeechItem::DelayedTrainingComplete( TUint32 aRuleID )
    {
    SetRuleID( aRuleID );
    
    if ( aRuleID != KInvalidRuleID )
        {
        this->iTrainedType = TNssVasCoreConstant::EVasTrainText;
        }
    else
        {
        // Training failed; mark all invalid
        SetRuleID( KInvalidRuleID );
        
        this->iTrainedType = TNssVasCoreConstant::EVasNotTrained;
        }
    
    this->iTrained = ETrue;
    this->iState = TNssSpeechItemConstant::ETrainComplete;
    }

// -----------------------------------------------------------------------------
// TTtsData::TTtsData
// Initializes everything to zero.
// -----------------------------------------------------------------------------
//
CNssSpeechItem::TTtsData::TTtsData() : iStyle(),
                                       iStyleID( 0 ),
                                       iSegment( 0 ),
                                       iTts( NULL ),
                                       iParsedText( NULL )
    {
    // Nothing
    }

// End of file
