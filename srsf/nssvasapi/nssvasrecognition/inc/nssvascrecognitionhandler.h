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


#ifndef NSSVASCRECOGNITIONHANDLER_H
#define NSSVASCRECOGNITIONHANDLER_H

//  INCLUDES
#include <nsssispeechrecognitionutilityobserver.h>
#include "nssvasmrecognitionhandler.h"
#include "nssvasccontextmgr.h"
#include "nssvasmadaptationeventhandler.h"

// FORWARD DECLARATIONS

class CNssSiUtilityWrapper;
class CNssTagMgr;
class CNssVASDBBuilder;
class CNssAdaptationItem;

// CLASS DECLARATION

/**
*
*  The CNssRecognitionHandler class performs the recognition function.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/

class CNssRecognitionHandler :public CBase, 
                           public MNssRecognitionHandler, 
                           public MSISpeechRecognitionUtilityObserver
{

   public:  //enum

        enum TNssRState
        {
        EVasIdle,
        EVasWaitingForLoadModelsComplete,
        EVasWaitingForLoadLexiconComplete,
        EVasWaitingForLoadGrammarComplete,
        EVasWaitingForRecognitionReadyRecognitionInit,
        EVasWaitingForRecognize,
        EVasWaitingForRecordStarted,
        EVasWaitingForEouDetected,
        EVasWaitingForRecognitionComplete,
        EVasWaitingForSelectTag,
        EVasWaitingForUnloadRuleComplete,
        EVasWaitingForRecognitionReadyRejectTag,
        EVasDirectAdaptation,
        EVasIdleAdaptation,
        EVasCorrectingPluginDatabase
        };


    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */        
        static CNssRecognitionHandler* NewL();

        /**
        * Two-phased constructor.
        */    
        static CNssRecognitionHandler* NewLC();
        
        /**
        * Destructor.
        */                
        virtual ~CNssRecognitionHandler();

    public: // Functions from base classes

         /**
        * Perform initialization for the recognize funtion.
        * @since 2.0
        * @param aInitCompleteHandler The event call back to receive recognize 
        *                             init events.
        * @param aRecognitionVocabulary List of context, which are active in recognition.
        * @param aMaxResults The maximum number of voice tags to be recognized.
        * @return TNssRecognitionResult Return indicating request is valid.
        */               
        TNssRecognitionResult RecognizeInitL(
                            MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
                            const CArrayPtrFlat<MNssContext>& aRecognitionVocabulary,
                            TInt aMaxResults);
         /**
        * Perform initialization for the recognize funtion. Automatically loads
        * the commands and the names as recognition vocabulary.
        * @since 2.0
        * @param aInitCompleteHandler The event call back to receive recognize 
        *                             init events.
        * @param aMaxResults The maximum number of voice tags to be recognized.
        * @return TNssRecognitionResult Return indicating request is valid.
        */               
        TNssRecognitionResult RecognizeInitL(
                            MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
                            TInt aMaxResults );
        /**
        * From MNssRecognitionHandler Interface 
        * Perform the recognize function.
        * @since 2.0
        * @param aEventHandler The event call back to receive recognize events.
        * @return TNssRecognitionResult Return indicating request is valid.
        */        
        TNssRecognitionResult Recognize( MNssRecognizeEventHandler* aEventHandler );

        /**
        * From MNssRecognitionHandler
        * Starts sampling already before RecognizeInitL and Recognize calls.
        * This is used to ensure that there is no delay between user notification
        * and sampling start.
        *
        * @since 3.2
        * @param aInitCompleteHandler Callback handler
        * @return Return indicating request is valid.
        */
        TNssRecognitionResult PreStartSampling( MNssRecognizeInitCompleteHandler* aInitCompleteHandler );

        /**
        * From MNssRecognitionHandler Interface 
        * Initiate the Select Tag event.
        * @since 2.0
        * @param aClientTag The voice tag which was selected. The tag is owned
        *                   by the client.
        * @return TNssRecognitionResult Return indicating request is valid.
        */        
        TNssRecognitionResult SelectTagL( MNssTag* aClientTag );

        /**
        * From MNssRecognitionHandler Interface 
        * Initiate the Select Tag event.
        * @since 2.0
        * @param aClientTag The voice tag which was selected. The tag is owned
        *                   by the client.
        * @param aNotifyHandlers If false, the global context handlers won't be notified.
        * @return TNssRecognitionResult Return indicating request is valid.
        */        
        TNssRecognitionResult SelectTagL( MNssTag* aClientTag, TBool aNotifyHandlers );

        /**
        * Request Reject Tag, i.e. blacklisting.
        * @since 2.0 
        * @param aClientTagList The list of voice tags which is to be rejected, i.e. 
        *                       blacklisted. The tag list is owned by the client.
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        TNssRecognitionResult RejectTagL(
                             MNssRejectTagCompleteHandler* aRejectTagCompleteHandler,
                             CArrayPtrFlat<MNssTag>* aClientTagList );

        /**
        * From MNssRecognitionHandler Interface 
        * Cancel the last recognition function.
        * @since 2.0
        * @param -
        * @return TNssRecognitionResult Return indicating request is valid.
        */                
        TNssRecognitionResult Cancel();
 
        /**
        * From MNssRecognitionHandler Interface
        * Get the adaptation item to adapt, when you know whether
        * the result was correct or not.
        * @since 2.0
        * @param -
        * @return MNssAdaptationItem The adaptation item.
        */
        MNssAdaptationItem* GetAdaptationItemL();

        /**
        * From MSpeechRecognitionEvent Interface
        * Handle Speech Recognition Utility event
        * @since 2.0
        * @param aEvent The event callback to receive Speech Recognition Utility 
        *               events.
        * @param aResult The result value associated with aEvent.
        * @return -
        */
        void MsruoEvent( TUid aEvent, TInt aResult );

    public: // New functions

        /**
        * AdaptL - called by adaptation item. Adapts the acoustic models.
        * @since 2.0
        * @return None
        */
        void AdaptL( MNssAdaptationEventHandler* aCallback,
                     CSIClientResultSet* aResultSet,
                     TInt aCorrect );

        /**
        * RemoveAdaptationItem - When an adaptation item is destroyed,
        * it signals to its "host" recognition handler. This way, the host
        * knows to remove it from the list of active adaptation items.
        */
        void RemoveAdaptationItem( CNssAdaptationItem* anItem );

    private:

        /**
        * Constructor.
        */        
        CNssRecognitionHandler();

       /**
        * By default Symbian 2nd phase constructor is private.
        */        
        void ConstructL();

        /**
        * Perform required cleanup to get back to the EIdle state and issue the
        * HandleRecognizeInitError event with RecognizeInitFailed error code
        * @return void
        */ 
        void CleanupRecognizeInitFailed();

        /**
        * Perform required cleanup to get back to the EIdle state and issue the
        * HandleRecognizeError event with RecognizeFailed error code
        * @return void
        */ 
        void CleanupRecognizeFailed();

        /**
        * Perform required cleanup to get back to the EIdle state and issue the
        * HandleRejectTagError event with RejectTagFailed error code
        * @return void
        */ 
        void CleanupRejectTagFailed();

        /**
        * Perform required cleanup to get back to the EIdle state
        * @return void
        */ 
        void Cleanup();

        /**
        * Gracefully terminate and delete the SRF object, iSrsApi.
        * @return void
        */ 
        void DeleteSrf();

        /**
        * Delete context objects from iContextList
        * @return void
        */ 
        void ClearContextList();

        /**
        * Delete the tag objects from iTagList
        * @return void
        */ 
        void ClearTagList();

        /**
        * Delete the TRuleEntry objects from iRuleList
        * @return void
        */ 
        void ClearRuleList();

        /**
        * Delete entries from iContextStatus
        * @return void
        */ 
        void ClearContextStatus();

        /**
        * Cleanup and determine the correct event handler
        * @return void
        */
        void UnexpectedEvent();

        /**
        * Handle LoadModelsComplete SRF event
        * @return void
        */
        void DoLoadModelsComplete();

        /**
        * Handle LoadModelsFailed SRF event
        * @return void
        */
        void DoLoadModelsFailed();

        /**
        * Handle LoadLexiconComplete SRF event
        * @return void
        */
        void DoLoadLexiconComplete();

        /**
        * Handle LoadLexiconFailed SRF event
        * @return void
        */
        void DoLoadLexiconFailed();

        /**
        * Handle LoadGrammarComplete SRF event
        * @return void
        */
        void DoLoadGrammarComplete();

        /**
        * Handle LoadGrammarFailed SRF event
        * @return void
        */
        void DoLoadGrammarFailed();

        /**
        * Handle RecognitionReady SRF event
        * @return void
        */
        void DoRecognitionReady();

        /**
        * Handle RecognitionComplete SRF event
        * @return void
        */
        void DoRecognitionComplete();
        
        /**
        * Handle RecognitionFailedNoSpeech, RecognitionFailedTooEarly, 
        * RecognitionFailedTooLong, and RecognitionFailedTooShort SRF events
        * @param aResult The Msruo event Result code.
        * @return void
        */        
        void DoRecognitionFailedNoSpeech( TInt aResult );

        /**
        * Handle RecognitionFailedNoMatch SRF event
        * @return void
        */
        void DoRecognitionFailedNoMatch();

        /**
        * Handle RecognitionFailed SRF event
        * @return void
        */
        void DoRecognitionFailed();

        /**
        * Handle RecordStarted SRF event
        * @return void
        */
        void DoRecordStarted();

        /**
        * Handle RecordFailed SRF event
        * @param aResult The Msruo event Result code.
        * @return void
        */
        void DoRecordFailed( TInt aResult );

        /**
        * Handle EouDetected SRF event
        * @return void
        */
        void DoEouDetected();

        /**
        * Handle UnloadRuleComplete SRF event
        * @return void
        */
        void DoUnloadRuleComplete();

        /**
        * Handle UnloadRuleFailed SRF event
        * @return void
        */
        void DoUnloadRuleFailed();
        
        /**
        * Handle PreStartSampling SRSF event
        */
        void DoPreStartSamplingComplete( TInt aError );

        /**
        * Handle Tag Check for contexts
        * @return void
        */
        void HandleTagCheckL();

        /**
        * Create the SRF object, iSrsApi.
        * @return void
        */ 
        void CreateSrfL();

        /**
        * Perform those RecognizeInitL tasks,
        * which do not depend on vocabulary.
        */
        MNssRecognitionHandler::TNssRecognitionResult CommonRecognizeInitL(
            MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
            TInt aMaxResults );


    struct TRuleEntry 
    {
        TUint32 ruleId;
        TUint32 grammarId;
    };


    private:    // Data

        // Pointer to SRS Utility object.
        CNssSiUtilityWrapper*     iSrsApi;

        // Pointer to SRS to ClientResultSet object, i.e. returned SRS data.
        CSIClientResultSet*            iSIClientResultSet;

        // Pointer to RecognizeInit Complete Event Handler object.
        MNssRecognizeInitCompleteHandler* iInitCompleteHandler;

        // Pointer to Recognize Complete Event Handler object.
        MNssRecognizeEventHandler*        iEventHandler;

        // Pointer to RejectTag Complete Event Handler object.
        MNssRejectTagCompleteHandler*       iRejectTagCompleteHandler;

        // Pointer to VAS Database Builder object.
        CNssVASDBBuilder*                 iVasDBBuilder;

        // Pointer to Context Manager object.
        CNssContextMgr*                   iContextMgr;

        // Pointer to Context List object.
        TMNssContextList*                 iContextList;

        // Pointer to Tag Manager object.
        CNssTagMgr*                       iTagMgr;

        // Pointer to Tag List object.
        CArrayPtrFlat<CNssTag>*           iTagList;

        // Pointer to Rule List object - used during RejectTag 
        CArrayFixFlat<TRuleEntry>*     iRuleList;

        // Context Status Array - True indicates tags for context exist
        // False indicates there are not tags in the context
        CArrayFixFlat<TBool>*           iContextStatus;

        // Current Loop Counter used across asynch Context logic
        // during RecognizeInit
        TInt                           iContextCurrentLoopCount;

        // Number of contexts - used across asynch Context logic
        // during RecognizeInit
        TInt                           iContextCount;

        // Current Loop Counter used across asynch Result (tag) logic
        // during Recognize
        TInt                           iTagCurrentLoopCount;

        // Number of returned Results (tags) -  used across asynch tag logic
        // during Recognize
        TInt                           iTagCount;

        // Current Loop Counter used across asynch RejectTag logic
        TInt                           iRejectTagCurrentLoopCount;

        // Number of rejected tags - used across asynch RejectTag logic
        TInt                           iRejectTagCount;

        // Maximum number of voice tags to be recognized, requested by client 
        TInt                           iMaxResults;

        // The current recognition state.
        TNssRState                        iRState;

        // Array of adaptation items. Each MNssAdaptationItem is
        // internally linked to some "host" CNssRecognitionHandler.
        RPointerArray<CNssAdaptationItem> iAdaptItemArray;

        // Callback after adaptation
        MNssAdaptationEventHandler*       iAdaptationHandler;
        
        // Wait loop
        CActiveSchedulerWait iWait;
};

#endif // NSSVASCRECOGNITIONHANDLER_H   
            
// End of File
