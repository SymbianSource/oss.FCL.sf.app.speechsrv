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
* Description:  VAS contact handler implementation
*
*/


#ifndef NSSCONTACTHANDLERIMPL_H
#define NSSCONTACTHANDLERIMPL_H

//  INCLUDES
#include <cenrepnotifyhandler.h>

#include <vasmcontactobserver.h>

#include <nssvasmgetcontextclient.h>
#include <nssvasmgettagclient.h>
#include <nssvasmtraintexteventhandler.h>
#include <nssvasmdeletecontextclient.h>
#include <nssvasmdeletetagclient.h>
#include <nssvasmsavetagclient.h>
#include <nssvasmsavecontextclient.h>
#include "nssvasdatasyncwatcher.h"
#include <nssvascoreconstant.h>
#include "srsfbldvariant.hrh"

// Publish & Subscribe of the contact handler activity
#include <e32property.h>  

#include <PbkFields.hrh>

// CONSTANTS

// Number of restart timer calls checked during periodic calls check
const TInt KEventMomentsSize = 3;

// MACROS
// Deprecated
#ifdef TEST_MANUAL_SYNC
#define CNssContactHandler CManualContactHandler
#endif // TEST_MANUAL_SYNC
    
// FORWARD DECLARATIONS
class CNssTrainingParameters;
class MVasBasePbkHandler;
class CNssVASDBMgr;
class MNssTagMgr;
class MNssContextMgr;
class CSuccessfulDeletionSimulator;
class CRepository;
class CNssChBackupObserver;

// CLASS DECLARATION

/**
*  TNssContextClientData contains last last sync time and UI language
*  which was used when training voice tags.
*
*  @lib NssVASContactHdlr.dll
*  @since 2.8
*/
struct TNssContextClientData
    {
    TTime iLastSyncTime;
    TLanguage iLanguage;
    TContactSyncId iContactSyncId;

    void InternalizeL(const TDesC8& aBuffer)
        {
        iLastSyncTime = 0;
    	iLanguage = ELangOther;
    	iContactSyncId = KErrNotFound;

        if ( (TUint)aBuffer.Size() > sizeof( TNssContextClientData ) )        
            {
            User::Leave( KErrCorrupt );
            }

        Mem::Copy( this, aBuffer.Ptr(), sizeof( TNssContextClientData ) );
        }

    void ExternalizeL(TDes8& aBuffer)
        {
        if ( (TUint)aBuffer.MaxSize() < sizeof( TNssContextClientData ) )
            {
            User::Leave( KErrTooBig );
            }

        aBuffer.SetLength( 0 );
        aBuffer.Copy( (TUint8*)this, sizeof( TNssContextClientData ) );
        }
    };
    
/**
*  Contact handler creates voice tags out of phonebook contacts and keeps
*  SIND data in sync with the phonebook
*
*  @lib NssVASContactHdlr.dll
*  @since 2.8
*/
NONSHARABLE_CLASS( CNssContactHandlerImplementation ): public CBase,
                                                       public MVasContactObserver,
                                                       public MNssGetTagClient,
                                                       public MNssDeleteTagClient,
                                                       public MNssGetContextClient,
                                                       public MNssSaveTagClient,
                                                       public MNssSaveContextClient,
                                                       public MNssTrainTextEventHandler,
                                                       public MNssDeleteContextClient,
                                                       public MNssDataSyncStateObserver,
                                                       public MCenRepNotifyHandlerCallback
    {
   // DATA TYPES
    enum TContactHandlerState
		{
		ECHIdle,
    	ECHInitializing,
    	// contact addition, removal or change
    	ECHHandlingNormalChanges,
    	ECHFullResync
		};
		
		
    struct TExtension
        {
        TPbkFieldId iId;
        HBufC* iText;
        TVasExtensionAction iAction;
        TVasExtensionCommand iCommand;
        };	
        
    struct TContactData
    	{
    	// Can be 1.Nickname, 2.firstname-lastname or vise versa, 3.Company name
    	HBufC*                iTitle;
    	TContactItemId        iID;
    	TTime                 iSyncTime;
    	TVasTagType           iType;
    	};

   public:  // Constructors and destructor

      /**
      * Two-phased constructor.
      */
      static CNssContactHandlerImplementation* NewL( );
        
      /**
      * Destructor.
      */
      virtual ~CNssContactHandlerImplementation();
   public: // New functions
      
      /**
      * Disables the contact handler until EnableEventHandling is called
      * Typically this means 'until reboot'
      * Is used in case of unrecoverable error.
      * 
      * @todo Move to private?
      * @return void
      */
      void DisableEventHandling();

      /**
      * Enables the contact handler. 
      * 
      * @deprecated Move to private?
      * @return void
      */
      void EnableEventHandling();

    public: // Functions from base classes
    
        /**
        * Is called by phonebook handler when contact change has occurred
        *
        * @param aEvent Event to be handled
        */
        void HandleEventL( const TPhonebookEvent& aEvent );
        
        /**
        * Is called by tag manager when getting of a tag list is completed 
        * successfully
        * From MNssGetTagClient Interface 
        *
        * @param aTagList list of tags returned
        * @param aErrorCode KErrNone if getting of tags has been successfull
        * @return void
        */
        void GetTagListCompleted( MNssTagListArray* aTagList, TInt aErrorCode );
        
        /**
        * Called when the deletion of a tag requested from tag manager
        * is completed successfully
        * From MNssDeleteTagClient Interface 
        * @param aErrorCode KErrNone if tag deletion has been successfull
        * @return void
        */
        void DeleteTagCompleted( TInt aErrorCode );
        
        /**
        * Called by tag manager, when GetContext() completes successfully
        *
        * Depending on the contact handler state, continues full resync sequence or
		* sets the idle state
        * @param aContext
        * @param aErrorCode KErrNone if getting of context list was successfull
        * @return void
        */
        void GetContextCompleted(MNssContext* aContext, TInt aErrorCode);
        
        /**
        * Callback to indicate GetContext successed.
        * @param aContextList - A list of contexts. 
        *        user has to ResetAndDestroy() after using the list.
        * @param aErrorCode KErrNone if getting of context list was successfull
        * @return None
        */
        void GetContextListCompleted( MNssContextListArray *aContextList,
                                      TInt aErrorCode );
        
        /**
        * Is called when SaveContext() completes
        * @param aErrorCode KErrNone if saving of context was successfull
        */
        void SaveContextCompleted( TInt aErrorCode );
        
        /**
        * Callback to indicate taht SaveTag completed successfully
        * @param aErrorCode KErrNone if saving of tag has was successfull
        * @return None
        */
        void SaveTagCompleted( TInt aErrorCode );
        
        /** 
        *  Is called when data sync status is changed and when data sync state
        *  observation was just started
        *  @param aRunning ETrue if from now on data sync process is running
        *                  EFalse otherwise
        *  @see MNssDataSyncStateObserver
        */
        void DataSyncStateChanged( TBool aRunning );
        
        /** 
        *  Is called when backup/restore status is changed and when backup/restore
        *  state observation was just started
        *  @param aRunning ETrue if some backup/restore action is in progress
        *                  EFalse otherwise
        *  @param aRestoreType ETrue if the event is restore related
        *                      EFalse if the event is backup related
        *  @see CNssChBackupObserver
        */
        void BackupRestoreStateChanged( TBool aRestoreType, TBool aRunning );
        
        /** 
        *  From MCenRepNotifyHandlerCallback
        *  @see MCenRepNotifyHandlerCallback for more information
        */
        void HandleNotifyInt( TUint32 aId, TInt aNewValue );
        
    private:
        
      	/**
      	* C++ default constructor.
      	*/
      	CNssContactHandlerImplementation( );

        /**
        * Second-phase constructor
        */
        void ConstructL();
        
        // Prohibit copy constructor
        CNssContactHandlerImplementation( const CNssContactHandlerImplementation& );
        
        // Prohibit assigment operator
        CNssContactHandlerImplementation& operator= ( const CNssContactHandlerImplementation& );
        
      	/** 
      	* Register itself for phonebook database events
      	*/  
      	void RegisterForDatabaseEventsL(  );

        /**
        * Check if the event was caused by voice tag field adding
        * @param aEvent event from contact db
        * @return ETrue if voice tag field was added
        */
        TBool CausedByVoiceTagAddition( const TPhonebookEvent& aEvent );
        
        /**
        * Get the current contact's title in a buffer
        * @param aContactId Id of the contact to return title for
        * @param aUseDefaultOrder If ETrue created title uses preferred 
        * 		 lastname-firstname order of the current UI language. If 
        *		 EFalse, uses reverse order
        * @param aBothFirstAndLastFound Is set to ETrue if returned title 
        *		 contains both first name and last name. To EFalse otherwise
        * @param aTagType Set based on found tag type (name or company name)
        * @return 'Title' of the contact, i.e. value that is pronounced when
        *		 this contact tag is recognized by SIND
        */
        HBufC* GetContactTitleL( TContactItemId aContactId, TBool aUseDefaultOrder, 
                                 TBool& aBothFirstAndLastFound, TVasTagType& aTagType );
        
        /** 
        * Return the nickname for a given contact
        *
        * @exception KErrNotFound if nickname field is not present
        * @param aContactId Phonebook id of the contact in question
        * @return NULL if nickname is present, but empty. 
        *		  Otherwise newly allocated descriptor to the nickname
        */
        HBufC* GetContactNicknameL( TContactItemId aContactId );
        
        /*
         * Returns a flag which tells if name order should be changed.
         * @return ETrue if preferred order of the current UI language is 
         * lastname-firstname. EFalse otherwise
         */
        TBool SwapNameOrder();
        
        // Process the Contact Database Events
        void DoHandleEventsL();
        
        /**
         * Routine to handle failure during processing the event at the lower
         * layers
         */ 
        void DoHandleEventFailedL();
        
        /**
         * Single tag training completed successfully
         */       
        void HandleTrainComplete( TInt aErrorCode );
        
        /**************** The functions to handle miscellanious changes ***************/
        
        // Handling miscellaneous changes has 3 phases:
        // 1. Fuse add & remove & change events into a list
        //    of tags to be added and tags to be removed.
        // 2. Remove tags to be removed.
        // 3. Add tags to be added
        
        
        /** 
         * Fuse add & remove & change events into a list of tags to be added
         * and tags to be removed.
         */
        void CollectAddRemoveListsL();
        
        
        /**
         * Starts adding names: Creates the tags and sends TrainTextL calls.
         */
        void DoAddNamesL();
        
        /** 
         * Is called after all the tags scheduled for addition have been 
         * trained successfully
         */
        void DoAddNamesTrainingFinished();
        
        /**
         * Initiates saving tags after they have been trained.
         * After saving tags started is recursively called for every next
         * tag to be saved
         */
        void DoAddNamesSaveTag();
        
        /**
         * Is called when single tag saving has been completed
         * @param aError KErrNone if saving was successful, 
         *				 KErrGeneral otherwise
         */ 
        void DoAddNamesSaveTagCompleted( TInt aError );
        
        /** 
         * Cleanup addition-related buffers after new tags have been created, 
         * trained and saved.
         * Add voice tag icons to the phonebook
         */
        void DoAddNamesCompletedL();
        
        
        /**
         * Start removing names by requesting list of tags for the first 
         * contact scheduled for deletion
         */
        void DoRemoveNamesL();
        
        /**
         * Callback for handling list of tags, that should be deleted
         * The first list is requested from DoRemoveNamesL, other - recursively
         * from DoRemoveNamesAfterGetTagList
         * @param aTagListArray List of tags to be deleted. Can be empty
         */ 
        void DoRemoveNamesAfterGetTagList( MNssTagListArray* aTagListArray );
        
        /** 
         * Send single DeleteTag call to remove the first tag of those that 
         * are in the to-be-deleted list from VAS
         * Expects callbacks to DeleteTagCompleted or to DeleteTagFailed
         */
        void DoRemoveNamesCallDeleteTag();
        
        /**
         * DeleteTag() callbacks call this funciton. It counts
		 * the number of callbacks, and when all callbacks have arrived
		 * goes to the next phase: RemovingNamesCompletedL or
		 * DoHandleEventFailedL
         */
        void DoRemoveNamesAfterDeleteTagCompleted();
        
        /**
         * Tag removal for a pack on names completed successfully. Clean the
         * deletion tag buffers
         */ 
        void RemovingNamesCompletedL();
        
        /**
         *  TLinearOrder comparison function for sorting phonebook events
         */
        static TInt PhonebookOrder( const TPhonebookEvent& a1, const TPhonebookEvent& a2 );
        
        /*************** The functions to handle miscellanious changes end ************/
        /**
        * Compares client data from VAS context to the state of the phonebook
        * to find out if full synchronization is needed.
        * Compares client data (saved in VAS context)
		* to state of the phonebook to find out if
		* full synchronization is needed.
		*
		* If full resync is needed initiates it by saving into the event queue
		* special 'full resync event'
        * @since 2.0
        */       
        void CheckErrorSigns();
        
        /**
         * Handle errors which happend when updating normal changes
         */ 
        void NormalChangesHandleError();
        
        /**
         * Start full resynchronisation
         */
        void FullResyncStart();
        
        /**
         * Delete existing context if any, create a new one.
         * Is a part of full resynchronization sequence
         */
        void FullResyncCreateContextL();
        
        /**
         * Simulate phonebook addition events for all the phonebook contacts.
         * Is a part of full resynchronization sequence
         */
        void FullResyncCreateChangesL();

		/**
		 * Checks if the event queue contains an event, which
		 * launches full resynchronization
		 * @return ETrue if full resync request is present in the queue
		 *		   EFalse otherwise
		 */
        TBool IsFullResyncNeeded();
        
        /**
         * Is called when conext deletion has completed successfully.
         * Is a part of the full synchronization sequence
         * @param aErrorCode KErrNone if deletion of context was successfull
         */
        void DeleteContextCompleted( TInt aErrorCode );
        
        /**
         * Is called when conext deletion fails.
         * Is a part of the full synchronization sequence
         */
        //void DeleteContextFailed( enum MNssDeleteContextClient::TNssDeleteContextClientFailCode  );
        
        /**
         * Static callback required by Symbian Timer services.
         * Real action happens in the DoPeriodicCallback 
         * @param pX Pointer to the active instance of CNssContactHandlerImplementation
         * @return KErrNone. Has no meaning in this case
         * @see CNssContactHandlerImplementation::DoPeriodicCallback()
         */
        static TInt PeriodicCallback( TAny* pX );
        
        /**
         * Timer callback used to check if periodic action completed,
         * there are events to process and contact handler can do it now 
         * (is idle). If this is true, initiates event processing
         */
        void DoPeriodicCallback();
        
        /**
    	* Checks if it events look like they are periodically sent 
    	* from some lengthy operation
    	* @return ETrue if periodic behavior detected, EFalse otherwise
    	*/
    	TBool PeriodicAction();
    	
    	/**
    	* Updates variables used for periodic action checks.
    	* Adds current time to the array of phonebook event moments
    	*/
    	void UpdatePeriodicActionVars();
    	
    	
    	/**
    	* Returns maximal interval between time moments recorded in the  array.
    	* Moments are stored in a cyclic way, i.e. moment 0 is stored at index 0,
    	* moment1 at index 2,... moment n at index 0 again
    	* @param aMoments Array of time moments
    	* @param aMomensLength Length of the aMoments array. Must be > 1
    	* @param aLastMomentIndex Index of the last recorded moment within aMoments
    	* @return Maximal interval. If Maximal interval is too big to fit into
    	*		  TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32(KMaxTInt)
    	*		  is returned
    	*/
    	TTimeIntervalMicroSeconds32 MaximalInterval( const TTime aMoments[], const TInt aMomentsLength, const TInt aLastMomentIndex );
    	
    	/**
    	* Set the contact handler state. It is the place, where state-related 
    	* invariants are checked in debug builds. In debug builds panics if
    	* intended change is illegal
    	*/
    	void SetState( TContactHandlerState aState );
    	
    	/**
    	* Returns immediately if neither data sync nor backup is in progress
    	* Otherwise waits indefinetely until data sync run is over
    	*/
    	void WaitUntilNoSyncBackupIsRunning();
        
        /**
         * Sets the central repository full resync flag
         * @leave CRepository error codes
         */
        void SetFullResyncCrFlagL( TBool aResyncNeeded );

#ifdef __SIND_EXTENSIONS	    	
    	/**
    	* Read name dialling extension from resource file
    	*/
    	void ReadExtensionsL();
    	
    	/**
    	* Finds extension field based on priority lists
    	*
    	* @param aAction Extension action
    	* @return KErrNotFound if contact item field was not found
    	*/
    	TInt FindExtensionField( TVasExtensionAction aAction,
    	                         TPbkFieldId aDefaultFieldId );
    	                                          
    	                                          
        /**
        * Loops through list of fields
        *
        * @param aArray Array field IDs
        * @param aCount Number of elements in aArray
        * @param aItem Contact item
        *
        * @return KErrNotFound if contact item field was not found
        */    	                                          
        TInt ExtensionFieldFindLoop( const TPbkFieldId* const aArray, 
                                     TInt aCount );
#endif // __SIND_EXTENSIONS	

    	/**
         * Reads a contact and returns a phonebook contact item. If contact is 
         * already locked by something, function keeps trying for a long time.
         * If still fails, contact handler is disabled and full resync will
         * be attempted after the reboot
         *
         * @param aContactId Contact item id to be read.
         */           
        void ReadContactL( TContactItemId aContactId );
            
        /**
         * Opens a contact. If contact is already locked by something,
         * function keeps trying for a long time.
         * If still fails, contact handler is disabled and full resync will
         * be attempted after the reboot
         *
         * @param aContactId Contact item id to be opened.
         */
        void OpenContactL( TContactItemId aContactId );            
        
        /** 
        * Reads contact from phonebook
        * 
        * If contact is already locked by something, function keeps trying for a long time.
        * If still fails, contact handler is disabled and full resync will
        * be attempted after the reboot
        *
        * @param aContactId Contact of interest
        * @param aReadContact ETrue for ReadContactL and EFalse for OpenContactL
        */
        void GetContactL( TContactItemId aContactId, TBool aReadContact );
        
        /**
         * Static callback required by Symbian Timer services.
         * Real action happens in the DoContactWaitCallback 
         * @param pX Pointer to the active instance of CNssContactHandlerImplementation
         * @return KErrNone. Has no meaning in this case
         * @see CNssContactHandlerImplementation::DoPeriodicCallback()
         */
        static TInt ContactWaitCallback( TAny* pX );
        
        /**
         * Timer callback used to check if phonebook contact is unlocked
         * Unlocks the waiter
         */
        void DoContactWaitCallback();
        
        /**
        * Utility function to check if there isn't enough disk space to perform
        * the operation.
        *
        * @return ETrue if there is not enough disk space, EFalse otherwise
        */
        TBool NoDiskSpace();
        
        /**
        * Utility function to substitute all separator characters as white spaces.
        *
        * @param aBuf Descriptor which is manipulated
        */
        void TrimName( HBufC* aBuf );
        
        /**
        * Utility function to append name to a buffer.
        *
        * @param aBuf Descriptor which is appended
        * @param aName Name
        */
        void AppendName( HBufC* aBuf, HBufC* aName );

        /**
        * Utility function to append extension marker ("_2") to buffer.
        *
        * @param aDes Buffer where to do the modification
        */        
        void AppendExtensionMarker( TDes& aDes );

		/**
		* Establishes the connection to the contact engine and 
		* starts listening to it
		*/ 
		void ConnectToPhonebookL();
		
		/** 
		* Clears the connection to the contact engine
		*/
		void DisconnectFromPhonebook();
    	
   private:    // Data
       // Phonebook engine
       MVasBasePbkHandler* iPbkHandler;
       CNssVASDBMgr* iVasDbManager;
       MNssTagMgr* iTagManager;
       MNssContextMgr* iContextManager;
       TContactHandlerState iState;

       // Id of the contact that is currently deleted
       /** @todo Why is it not used for additions? Or why it is used at all? */
       TContactItemId iContactId;

       // Notifier object for the default contact database. 
       // iContactChangeNotifier attaches contact handler to the phonebook
       // engine as long as the object exists.
 //      CPbkContactChangeNotifier* iContactChangeNotifier;

       // Tells if ocntact handler is active
       TBool iHandlerEnabled;

       // Context used for name dialing
       MNssContext* iContext;

       // List of raw database events. Also can store special 'full resync' event
       RArray<TPhonebookEvent> iEventArray;

       // Number of retrials to save tags
       TInt iRetry;

       // Delimeter between first an last name in the contact title
       TChar iSeparator;
       
       // Array of relevant data from contacts to add
       // Just id and title
       RArray<TContactData> iContactQueue;

       // Language specific settings
       CNssTrainingParameters* iTrainingParams;
       
       // List of contacts to add to VAS
       RArray<TPhonebookEvent> iAddList;

       // List of contacts to remove tags for
       RArray<TPhonebookEvent> iDelList;
       
       // List of contacts, for which voice tag icon has been added OR REMOVED
       // Is used to ignore events related to the change of voice dial icon
       RArray<TContactItemId> iVoiceTagAddedList;
       
       // List of contacts for which voice tag icon should be removed if present
       // The need for it occurs if training voice tag fails and before training
       // icon was present
       RArray<TContactItemId> iRemoveIconList;
       
       // List of tags that are to be trained and saved
       RPointerArray<MNssTag> iTagArray;
       
       // Counts the number of callbacks after TrainText() calls
       TInt iTrainCallbackCounter;
       
       // Counts the number of callbacks after SaveTag() calls
       TInt iSaveCallbackCounter;
       
       // Counts the number of callbacks after DeleteTag() calls
       TInt iDeleteCallbackCounter;
       
       // General information about the context used for name dialing:
       // language and last sync timestamp
       TNssContextClientData iClientData;
       
       // Tells if full resync happened at least once
       // I.e. if name dialing context exists
       TBool iHaveWeEverSynced;
       
       // Indicates if error happened during normal event handling
       // Can be only KErrNone of KErrGeneral
       TInt iNormalChangesError;
       
       // List of tags to be deleted for one contact
       // (main name + possible nickname )
       MNssTagListArray* iDeleteTagListArray;
       
       // Languages which use lastname-firstname order in names
       RArray<TLanguage> iSwappedLanguages;
       
       // Timer used to ensure, that events queue is checked at least every 30 secs
       CPeriodic* iPeriodicHandleEventTimer;
       
       // Array of event receivement times
       TTime iEventMoments[KEventMomentsSize];
    
       // Index of the last value in iRestartTimerCalls
       TInt iEventMomentIndex;
    
       
	   // Time interval used to detect END of periodic action. In microseconds
	   // Note, that it is NOT the value used to command 
	   // iPeriodicHandleEventTimer
	   TTimeIntervalMicroSeconds iEndOfPeriodicActionInterval;
	   
	   // Counter for consecutive full resyncs forced by error during
	   // handling normal changes (or also resync changes)
	   // Is used to prevent endless resyncing in case of some unexpected
	   // consistent lower level failure
	   TInt iConsecutiveErrorFullResync;
	   
	   // Publish & Subscribe property, that tells external entities if contact
	   // handler training is ongoing. Note, that it is different from
	   // the contact handler state
	   RProperty iBusyProperty;
	   
	   // Publish & Subscribe property that watches for the PC-Suite Sync activity
	   RProperty iSyncProperty;

	   // Temporary container for a tag that is created and used in the
	   // DoAddNamesL. Member variable is used only because we cannot use
	   // CleanupStack with MNssTag
	   MNssTag* iNewTag;
	   // Informs that VoIP feature is supported.
	   TBool iVoIPFeatureSupported;
	   
	   // Informs that video telephony feature is supported
	   TBool iVideoCallFeatureSupported;
	   
#ifdef __SIND_EXTENSIONS	   
       // extensions
	   RArray<TExtension> iExtensionList;
#endif //__SIND_EXTENSIONS

	   // ETrue if PC-suite data sync is in progress. I.e. contact hanlder should
	   // delay its actions until iDataSyncRunning is EFalse
	   TBool iDataSyncRunning;
	   
	   // Waits until iDataSyncRunning is EFalse
	   CActiveSchedulerWait* iSyncBackupWaiter;
	   
	   // Waits until contact of interest is unlocked
	   CActiveSchedulerWait* iContactWaiter;
	   
	   // Periodic timer that ticks ones per socond if we are waiting for the
	   // contact unlock
	   CPeriodic* iContactWaitTimer;
	   
	   // Watches for the data sync state changes
	   MNssDataSyncWatcher* iDataSyncWatcher;
	   
	   // Watches the backup/restore state
	   CNssChBackupObserver* iBackupObserver;
	   
	   // Watches central repository changes
	   CCenRepNotifyHandler* iRepositoryObserver;
	   
	   // Is used to *asynchronously* simulate successful tag deletion
	   CSuccessfulDeletionSimulator* iDeletionSimulator;
	   
	   // To simplify access for the pure utility class
	   friend class CSuccessfulDeletionSimulator;
	   
	   // File server session to find out the available disk space
	   RFs iFSession;
	   
	   // central repository for setting resync needed flag
	   CRepository* iRepository;

	   // flag for phonebook restore
	   TBool iRestoreRunning;
	   
	   // Current resync flag state
	   TBool iResyncAtBoot;
    };

#endif // CONTACTHANDLERIMPL_H
            
// End of File
