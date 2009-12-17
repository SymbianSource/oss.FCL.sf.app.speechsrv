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
* Description:  The CSeechItem provides speech mechanism to request services from SRS.
*
*/


#ifndef NSSVASCSPEECHITEM_H
#define NSSVASCSPEECHITEM_H

// INCLUDE FILES
#include <nsssispeechrecognitiondatacommon.h>
#include <nsssispeechrecognitionutilityobserver.h>
#include <nssttscommon.h>

#include "srsfbldvariant.hrh"
#include "nssvascoreconstant.h"
#include "nssvasmspeechitem.h"
#include "nssvasmtrainvoiceeventhandler.h"
#include "nssvasmtraintexteventhandler.h"
#include "nssvasmplayeventhandler.h"
#include "nssvasmcoresrsdbeventhandler.h"
#include "nssvasmsavetagclient.h"
#include "nssvasmdeletetagclient.h"
#include "nssvasccontext.h"
#include "nssvascspeechitemsrsportal.h"
#include "nssvastspeechitemconstant.h"
#include "nssvasttsutilitywrapper.h"

// FORWARD DECLARATIONS
class CNssSpeechItemTrainer;
class CNssTag;

/**
* The CNssSpeechItem class provides a realization of the MNssSpeechItem client
* interface. The CNssSpeechItem methods use the CSpeechRecognitionUtility Api 
* to implement client speech requests.
*
* @lib nssvasapi.lib
* @since 2.8
*/
class CNssSpeechItem : public CBase, 
                       public MNssSpeechItem, 
                       public MSISpeechRecognitionUtilityObserver,
                       public MTtsClientUtilityObserver
{

  public:   // Constructors and destructor

    /**
    * Destructor.
    */ 
    ~CNssSpeechItem();

    /**
    * Two-phased CNssSpeechItem constructor
    *
    * @since 2.0
    * @param aContext A reference to Context object
    * @param aPortal A reference to SpeechItem Portal object
    * @return a pointer to the new SpeechItem object
    */   
    static CNssSpeechItem* NewL( CNssContext& aContext, 
                                 CNssSpeechItemSrsPortal& aPortal, 
                                 CNssSpeechItemTrainer& aTrainer );


    /**
    * Two-phased CNssSpeechItem constructor
    *
    * @since 2.0
    * @param aContext A reference to Context object
    * @param aPortal A reference to SpeechItem Portal object
    * @param aRuleID A rule ID value
    * @param aText A reference to the TDesc text 
    * @return a pointer to the new SpeechItem object
    */   
    static CNssSpeechItem* NewL( CNssContext& aContext,
                                 CNssSpeechItemSrsPortal& aPortal,
                                 CNssSpeechItemTrainer& aTrainer,
                                 TUint32 aRuleID,
                                 const TDesC& aText );

    /**
    * Two-phased CNssSpeechItem constructor
    *
    * @since 2.0
    * @param aContext A reference to Context object
    * @param aPortal A reference to SpeechItem Portal object
    * @param aRuleID A rule ID value
    * @param aText A reference to the TDesc text 
    * @return a pointer to the new SpeechItem object
    */   
    static CNssSpeechItem* NewLC( CNssContext& aContext,
                                  CNssSpeechItemSrsPortal& aPortal, 
                                  CNssSpeechItemTrainer& aTrainer,
                                  TUint32 aRuleID,
                                  const TDesC& aText );


  public: // New functions

    /**
    * Method to return the rule ID
    *
    * @since 2.0
    * @param 
    * @return the rule ID.
    */   
    TUint32 RuleID();

    /**
    * Method to set the rule ID
    *
    * @since 2.0
    * @param aRuleID Rule ID 
    */   
    void SetRuleID( TUint32 aRuleID );

    /**
    * Method to set the Training Type
    *
    * @since 2.0
    * @param aTrainType Training Type
    */   
    void SetTrainedType( TNssVasCoreConstant::TNssTrainedType aTrainType );

	/**
    * Create a copy of the SpeechItem object. 
	* This cunction creates a new SpeechItem by allocating memory for
	* a new SpeechItem, and deep copying the members of the SpeechItem 
	* on which this fucntion is called. Old SpeechItem and new SpeechItem
    * exist. Client need to deallocate memory for both.
    *
    * @param aContext a pointer to a Context
    * @return pointer the newly created SpeechItem.
    */
	CNssSpeechItem* CopyL( CNssContext* aContext );



  public: // Functions from base classes


    /**
    * Function inherited from MSISpeechRecognitionUtilityObserver to
    * handle the events from SRS Utility object
    *
    * @since 2.0
    * @param aEvent an event from the SRS utility object
    * @param aResult the result from aEvent operation
    */   
    void MsruoEvent( TUid aEvent, TInt aResult );

 
    /**
    * The Record method is used by the client in the train voice sequence
    * for SRS to record input speech.
    *
    * @since 2.0
    * @param aTrainVoiceEventHandler a reference to TrainVoiceEventHandler
    * @return a fail or success result from processing this method.
    */   
    TNssSpeechItemResult RecordL( MNssTrainVoiceEventHandler* aTrainVoiceEventHandler );


    /**
    * Method called by the client to indicate to SRS that a playback is
    * needed.
    *
    * @since 2.0
    * @param aPlayEventHandler a reference to PlayEventHandler
    * @return a fail or success result from processing this method.
    */   
    TNssSpeechItemResult PlayL( MNssPlayEventHandler* aPlayEventHandler );

    /**
    * Method called by the client to indicate to SRS that a playback is
    * needed with a certain language.
    *
    * @since 2.8
    * @param aPlayEventHandler a call back object to handle the play events
    * @param aLanguage language which will be used in playback
    * @return TNssSpeechItemResult synchronous return value to identify whether the
    *         synchronous process is success or fail
    */    
    TNssSpeechItemResult PlayL( MNssPlayEventHandler* aPlayEventHandler,
                                TLanguage aLanguage );

    /**
    * Method called by the client to indicate to SRS that training is
    * needed.
    *
    * @since 2.0
    * @param aTrainVoiceEventHandler a reference to TrainVoiceEventHandler
    * @return a fail or success result from processing this method.
    */   
    TNssSpeechItemResult TrainVoiceL(MNssTrainVoiceEventHandler* aTrainVoiceEventHandler);

    /**
    * Method called by the client to  cancel the previous operation.
    *
    * @since 2.0
    * @return a fail or success result from processing this method.
    */   
    TNssSpeechItemResult CancelL();
 
    /**
    * The TrainingCapabilities member fucntion is used by the client 
    * to get training capabilities to beused in Speaker Independent voice 
    * recognition.
    *
    * @since 2.0
    * @return a pointer to the Training Capability array object
    */   
    CArrayFixFlat<TNssVasCoreConstant::TNssTrainingCapability>* TrainingCapabilities();
 
    /**
    * Method to return the Training Type
    *
    * @since 2.0
    * @return the Training Type
    */   
    TNssVasCoreConstant::TNssTrainedType TrainedType();

    /**
    * Method to return the Text
    *
    * @since 2.0
    * @return the Text
    */   
    TDesC& Text();

    /**
    * Returns the raw training text from where escape characters are not removed.
    * Should not be used by clients who want to display the string.
    *
    * @since 3.2
    * @return Reference to the text buffer
    */    
    TDesC& RawText();
    
    /**
    * Returns the part of text which identified with the given id.
    * If RawText() is for example "_1Amy_1Harris_2mobile" then 
    * PartialTextL( _L( "1" ) ) call will return "Amy Harris".
    *
    * @since 3.2
    * @param aIdentifier ID of the text part to return
    * @reuturn Text or KNullDesC if identifier is not found
    */
    HBufC* PartialTextL( const TDesC& aIdentifier );
    
    /**
    * Method to set the training text
    *
    * @since 2.0
    * @param aText Text which will be trained
    * @return
    */   
    void SetTextL( const TDesC& aText );

    /**
    * Method to query playing support.
    *
    * @since 2.8
    * @return TBool True if playing is supported.
    */
    TBool IsPlayingSupported();

    /**
    * Method to train a speech model from text.
    *
    * @since 2.8
    * @param aEventHandler Event handler
    * @param aTrainingParams Training options, NULL uses default ones.
    * @return TNssSpeechItemResult Success status
    */
    TNssSpeechItemResult TrainTextL( MNssTrainTextEventHandler* aEventHandler,
                                     CNssTrainingParameters* aTrainingParams );

    /**
    * Unused method for future extensions of MTtsClientUtilityObserver.
    *
    * @from MTtsClientUtilityObserver
    * @since 2.8
    * @param aEvent event ID
    * @param aError result
    */
	void MapcCustomCommandEvent( TInt aEvent, TInt aError );

    /**
    * Method to signal that playing has been initialized.
    *
    * @from MTtsClientUtilityObserver
    * @since 2.8
    * @param aError Successful or not
    * @param aDuration Length of the TTS waveform
    */
	void MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration);

    /**
    * Method to signal that playing has been completed.
    *
    * @from MTtsClientUtilityObserver
    * @since 2.8
    * @param aError Successful or not
    */
	void MapcPlayComplete( TInt aError );

  public: // New Functions 

    /**
    * Method to start transaction for saving the data into the SRS.
    *
    * @since 2.0
    * @param aSaveTagClient Callback after we made it or died trying
    * @param aTag Tag to be deleted
    * @return a fail or success result from processing this method.
    */   
    MNssCoreSrsDBEventHandler::TNssSrsDBResult 
        NSSBeginSaveToSrs( MNssSaveTagClient* aSaveTagClient, CNssTag* aTag );

    /**
    * Method to start transaction for deleting tags from SRS and VAS DB.
    *
    * @since 2.0
    * @param aDeleteTagClient Callback after we made it or died trying
    * @param aTag Tag to be deleted
    * @return a fail or success result from processing this method.
    */   
    MNssCoreSrsDBEventHandler::TNssSrsDBResult 
        NSSBeginDeleteFromSrs( MNssDeleteTagClient* aDeleteTagClient, CNssTag* aTag );

    /**
    * Method to commit the transaction to the SRS.
    * This is a SYNCHRONOUS method.
    * @since 2.0
    * @return a fail or success result from processing this method.
    */   
    MNssCoreSrsDBEventHandler::TNssSrsDBResult CommitSrsChanges();


    /**
    * Method to end the transaction without committing..
    * This is a SYNCHRONOUS method.
    * @since 2.8
    * @return a fail or success result from processing this method.
    */   
    MNssCoreSrsDBEventHandler::TNssSrsDBResult RollbackSrsChanges();

    /**
    * Method to set the Tag ID
    * @since 2.0
    * @param aTagId a Tag ID 
    */   
	void SetTagId( TInt aTagId );

    /**
    * Method to return the Tag ID
    *
    * @since 2.0
    * @return the Tag ID.
    */   
    TInt TagId();

    /**
    * Method to return the Grammar ID from the context of the speech item.
    *
    * @since 2.8
    * @return the Grammar ID.
    */
    TUint32 GrammarId();

    /**
    * Method to set the Trained flag for True or False
    *
    * @since 2.0
    * @param aTrained Trained flag
    */   
	void SetTrained( TBool aTrained );

    /**
    * Method to return the Trained flag
    *
    * @since 2.0
    * @return the Trained flag
    */   
    TBool Trained();

    /**
    * Updates the speech item after training.
    *
    * @since 2.8
    * @param aRuleID Rule ID.
    */
    void DelayedTrainingComplete( TUint32 aRuleID );


  private: //function

    /**
    * C++ constructor
    *
    * @param aContext reference to Context object
    * @param aPortal reference to SpeechItem Portal object
    * @param aTrainer reference to speech item trainer
    */   
    CNssSpeechItem( CNssContext& aContext, 
                    CNssSpeechItemSrsPortal& aPortal, 
                    CNssSpeechItemTrainer& aTrainer );

    /**
    * Symbian 2nd phase constructor 
    *
    * @param aRuleID Rule ID value
    * @param aText Reference to the TDesc text 
    */   
    void ConstructL( TUint32 aRuleID, const TDesC& aText );

    /**
    * Method to copy utterance data from current to transient values
    */   
    void CopyUtteranceData();

    /**
    * Method to handle the "GetUtteranceDurationEvent" complete from SRS
    */   
    void DoGetUtteranceDurationCompleteEvent();

    /**
    * Method to handle the "PlayEvent" complete from SRS
    */   
    void DoPlayCompleteEvent();

    /**
    * Method to handle the "PlayEvent" fail from SRS
    *
    * @param aResult - a Result value from the Play operation
    */   
    void DoPlayFailEvent( TInt aResult );

    /**
    * Method to process the SRS play error 
    *
    * @param aResult the result value from SRS which indicates an error
    * @return a converted error code to identify the VAS play error
    */   
    MNssPlayEventHandler::TNssPlayResult DoPlayResult( TInt aResult );
    
    /**
    * Method to handle the "GetAvailableStorageEvent" complete from SRS
    */   
    void DoGetAvailableStorageCompleteEvent();

    /**
    * Method to handle the "TrainEvent" complete from SRS
    */   
    void DoTrainCompleteEvent();

    /**
    * Method to process the various train failed events from SRS'
    *
    * @param aResult the result value to indicate an error
    */   
    void DoTrainFailEvent( TInt aResult );

    /**
    * Method to process the SRS train error
    *
    * @param aResult the result value from SRS which indicates an error
    * @return a converted error code to identify the VAS train error
    */   
    MNssTrainVoiceEventHandler::TNssTrainResult DoTrainResult( TInt aResult );
    
    /**
    * Method to handle the "AddPronunciation" complete from SRS
    */   
    void DoAddPronunciationCompleteEvent();

    /**
    * Method to handle the "AddPronunciation" fail from SRS
    *
    * @param aResult a Result value from the event
    */   
    void DoAddPronounciationFailEvent( TInt aResult );
    
    /**
    * Method to handle the "RemoveModel" complete from SRS
    */   
    void DoRemoveModelCompleteEvent();

    /**
    * Method to handle the "RemoveModel" fail from SRS
    *
    * @param aResult a Result value from the event
    */   
    void DoRemoveModelFailEvent( TInt aResult );

    /**
    * Method to process the SRS DB error 
    *
    * @param aResult the result value from SRS to indicate an error
    * @return an error code to identify the DB error
    */   
    MNssCoreSrsDBEventHandler::TNssSrsDBResult DoSrsDBResult( TInt aResult );

    /**
    * Method to notify the event handler for receiving the Unexpected Event
    */  
    void NotifyHandlerForUnexpectedEvent();

    /**
    * Method to do clean up
    */   
    void CleanUp();
    
    /**
    * Method to verify whether it is ok to retry
    *
    * @return ETrue is ok to retry, EFalse stops retry 
    */   
    TBool DoRetry();
 
    /**
    * This method starts playback with CTtsUtility.
    */
    void DoPlayL();

    /**
    * This method releases resources, which were reserved for playing,
    * and updated the state from PlayingStrated to Idle.
    */
    void CleanPlayback();

    /**
    * Makes the callback after playing has completed or failed.
    *
    * @param Symbian standard error code
    */
    void MakePlayCallback( TInt aResult );

  private: //data

    // The rule ID.
    TUint32 iRuleID;

    // The transient rule ID.
    TUint32 iTransientRuleID;

    // The text data.
    HBufC* iText;

    // The trained type.
    TNssVasCoreConstant::TNssTrainedType iTrainedType;

    // Need to keep track the training state.
    TNssSpeechItemConstant::TNssState iState;
    TNssSpeechItemConstant::TNssState iOldState;

    // Reference to Context object
    CNssContext& iContext;

    // Reference to SRS Portal object.
    CNssSpeechItemSrsPortal& iPortal;

    // Reference to SRS Trainer object. Used for delayed text training.
    CNssSpeechItemTrainer& iTrainer;

    // Pointer to Play Event Handler object.
    MNssPlayEventHandler* iPlayEventHandler; 

    // Pointer to Train Event Handler object
	MNssTrainVoiceEventHandler* iTrainVoiceEventHandler;

    // The Tag ID for internal use by Vas Database,
	TInt iTagId; 

    // The Trained flag for re-training operation
    TBool iTrained;

    // The duration for the play utterance
    TTimeIntervalMicroSeconds32 iPlayDuration;

    // The flag to identify recovery is in progress
    TBool iRecover;

    // Retry counter
    TInt iRetry;

    // Memory is allocated for TTS data only when plaing the tag.
    // This is because CNssSpeechItem may have thousands of copies.
    struct TTtsData
        {
        // TTS speech style
        TTtsStyle iStyle;

        // TTS speech style handle
        TTtsStyleID iStyleID;

        // TTS speech segment
        TTtsSegment iSegment;

        // TTS player
        CNssTtsUtilityWrapper* iTts;

        // Parsed text
        CTtsParsedText* iParsedText;

        // Initializes everyting to zero.
        TTtsData();
        };

    // TTS Data
    TTtsData* iTtsData;
    
    // Cleaned text data where escape characters have been removed
    HBufC* iTrimmedText;
};

#endif // NSSVASCSPEECHITEM_H

// End of file
