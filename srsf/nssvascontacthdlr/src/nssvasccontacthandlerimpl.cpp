/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Responsible for maintaining synchronization between the contact DB
*               and the VAS DB
*
*/


/** @todo Read the whole contact in one go.

  Currently Contact handler usually opens the same contact (ReadContactL) two to 
  three times in order to read its data (mostly often three times).
  One is in GetContactNickname
  Second in GetContactTitle
  Third in DoAddNamesL when deciding what number field should the voice tag 
        be trained for
  If we could unite these reads, "reading contacts" phase will be 2-3 times faster.
  It would make about 10-20% improvement for the training of some 300 contacts */

// INCLUDE FILES
#include "nssvasccontacthandlerimpl.h"
#include "nssvasctrainingparameters.h"
#include <nssvascvasdbmgr.h>
#include <centralrepository.h>
#include <f32file.h>
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include <sysutil.h>
#include "srsfprivatecrkeys.h"

// Publish & Subscribe of the contact handler activity
#include <nssvascoreconstant.h>

#include <vascvpbkhandler.h>
#include <featmgr.h>
#include "nssvasdatasyncwatcher.h"
#include "nsschbackupobserver.h"
#include <StringLoader.h> 
#include <nssvascontacthandler.rsg>
#include <data_caging_path_literals.hrh>
#include "rubydebug.h"

// CONSTANTS
// Name of the context in the SIND DB. Voice tags for contacts are saved there
_LIT( KNssCHNameDialContext, "NAMEDIAL" );

// Languages which use lastname-firstname order
const TLanguage KSwappedLanguages[] = 
    {
    ELangHungarian,       // 17
    ELangTaiwanChinese,   // 29
    ELangHongKongChinese, // 30
    ELangJapanese,        // 32
    ELangPrcChinese,      // 31
    ELangKorean,          // 65
    ELangVietnamese       // 96
    };

// Number of languages that use lastname-firstname order
const TInt KNumberOfSwappedLanguages = sizeof( KSwappedLanguages ) / sizeof ( TLanguage );


// Maximal number of retrials to save tags
const TInt KVASCHMaxRetry = 2;

// Size of the event 'pack'. Tags are added and deleted in groups.
// KMaxNamesAtOnce is the maximal size of a group
const TInt KMaxNamesAtOnce = 100;

// Initial value of the time interval used to detect periodic action. In 
// microseconds. If KEventMomentsSize events happened  within the 
// KInitialPeriodicActionInterval, we assume, that periodic action has started
const TInt KInitialPeriodicActionInterval = 15000000;

// Time interval used to periodically check if there are pending 
// events to process. In microseconds
const TInt KPeriodicTimerInterval = 1000000;

// If there are this many consecutive full resyncs initiated by errors during
// handling normal changes (or during previous full resync), there is probably
// something consistently wrong at the lower layers. 
// Full resync trials (and the whole contact handler) should be disabled until
// reboot after KMaxConsecutiveErrorFullResync consecutive unsuccessful trials
const TInt KMaxConsecutiveErrorFullResync = 2;

// List of phonebook fields, that are supported by contact handler.
// I.e. fields, that can be dialed and voice tagged
/** @todo Consider using information from phonebook about which fields contain numbers
and using the following lists for priorities only */
const TPbkFieldId KTaggableFields[] = 
    { 
    EPbkFieldIdPhoneNumberMobile, 
    EPbkFieldIdPhoneNumberGeneral,
    EPbkFieldIdPhoneNumberStandard, 
    EPbkFieldIdPhoneNumberHome,
    EPbkFieldIdPhoneNumberWork, 
    EPbkFieldIdPhoneNumberVideo,
    EPbkFieldIdVOIP,
    EPbkFieldIdXsp
    };
const TInt KTaggableFieldsCount = sizeof( KTaggableFields ) / sizeof ( TPbkFieldId );

// List of phonebook fields that are supported by video extension
const TPbkFieldId KVideoExtensionFields[] = 
    {
    EPbkFieldIdPhoneNumberVideo,
    EPbkFieldIdPhoneNumberMobile,
    EPbkFieldIdPhoneNumberGeneral,
    EPbkFieldIdPhoneNumberHome,
    EPbkFieldIdPhoneNumberWork
    };
const TInt KVideoExtensionFieldsCount = sizeof( KVideoExtensionFields ) / sizeof ( TPbkFieldId );

// List of phonebook fields that are supported by message extension
const TPbkFieldId KMessageExtensionFields[] = 
    {
    EPbkFieldIdPhoneNumberMobile,
    EPbkFieldIdPhoneNumberGeneral,
    EPbkFieldIdPhoneNumberHome,
    EPbkFieldIdPhoneNumberWork,
    EPbkFieldIdPhoneNumberVideo
    };
const TInt KMessageExtensionFieldsCount = sizeof( KMessageExtensionFields ) / sizeof ( TPbkFieldId );   

// List of phonebook fields that are supported by VOIP extension
const TPbkFieldId KVOIPExtensionFields[] = 
    {
    EPbkFieldIdVOIP,
    EPbkFieldIdXsp
    };
const TInt KVOIPExtensionFieldsCount = sizeof( KVOIPExtensionFields ) / sizeof ( TPbkFieldId ); 

// Size of the client data per context. Is used to store the context language
// In bytes
const TInt KClientDataSize = sizeof(TNssContextClientData);


// Number of retries to try to lock the same contact
// 600 times should stand for 10 mins
// -1 means trying forever
const TInt KMaxContactLockRetrials = 600;

// Interval between the ticks of the contact wait timer
// in microseconds
const TInt32 KContactWaitInterval = 1000000;

// Bytes that should be free in the disk drive in addition to critical level 
// before operation is started (100kB)
const TInt KIncreaseOfDatabaseSizes = 100000;

// Define this if name free-order is enabled
//#define NSSVAS_CONTACTHANDLER_FREEORDER
// LOCAL FUNCTION DECLARATIONS


// ================= MEMBER FUNCTIONS =======================


#ifdef _DEBUG
_LIT( KFile, "VasCContactHandlerImpl.cpp" );
#endif

/**
* Is used in DoHandleEventFailedL to *asynchronously* simulate successful tag
*  deletion
*/
NONSHARABLE_CLASS( CSuccessfulDeletionSimulator ) : public CAsyncOneShot 
	{
	public:
		CSuccessfulDeletionSimulator( CNssContactHandlerImplementation& aHost );
		~CSuccessfulDeletionSimulator();
	protected:
	    /** Is called by the system after we asked it to Call() closer
	    */
		virtual void RunL();
    private:
        CNssContactHandlerImplementation& iHost;
	};

// -----------------------------------------------------------------------------
// CSuccessfulDeletionSimulator::~CSuccessfulDeletionSimulator
// Destructor
// -----------------------------------------------------------------------------
//	
CSuccessfulDeletionSimulator::~CSuccessfulDeletionSimulator() 
    {
    RUBY_DEBUG0( "~CSuccessfulDeletionSimulator" );
    if( IsAdded() ) 
        {
        RUBY_DEBUG0( "~CSuccessfulDeletionSimulator Request is still in progress, deque it" );
        Deque();
        }
    }

// -----------------------------------------------------------------------------
// CSuccessfulDeletionSimulator::CSuccessfulDeletionSimulator
// Constructor
// -----------------------------------------------------------------------------
//
CSuccessfulDeletionSimulator::CSuccessfulDeletionSimulator( CNssContactHandlerImplementation& aHost ) :
	CAsyncOneShot( CActive::EPriorityStandard ),
	iHost( aHost )
	{
	//nothing
	}
	
// -----------------------------------------------------------------------------
//  CAppCloser::RunL
//  Will be called by the system and exits the application
// -----------------------------------------------------------------------------
//
void CSuccessfulDeletionSimulator::RunL() 
	{
	RUBY_DEBUG0( "CSuccessfulDeletionSimulator::RunL Start" );
	if( iStatus == KErrNone ) 
		{
		CArrayPtrFlat<MNssTag>* empty = new (ELeave) CArrayPtrFlat<MNssTag>( 1 );
    	iHost.DoRemoveNamesAfterGetTagList( empty );
    	// empty will be destroyed in the DoRemoveNamesAfterGetTagList
		}
	else 
		{
		RUBY_ERROR1( "CSuccessfulDeletionSimulator::RunL Unexpected iStatus [%d]", iStatus.Int() );
		}
	RUBY_DEBUG0( "CSuccessfulDeletionSimulator::RunL End" );
	}
	


// -----------------------------------------------------------------------------
// CNssContactHandlerImplementation::CNssContactHandlerImplementation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssContactHandlerImplementation::CNssContactHandlerImplementation( )
: iState( ECHInitializing ),
  iSeparator( KNameSeparator()[0] ),
  iEndOfPeriodicActionInterval( KInitialPeriodicActionInterval )
    {
    iClientData.iLanguage = ELangNone;
    
    // initialize last RestartTimer calls to be long ago in 
    // the past (January 1st, 0 AD nominal Gregorian)
    for( TInt i = 0; i < KEventMomentsSize; i++)
    	{
    	iEventMoments[i] = 0;
    	}
    }

// -----------------------------------------------------------------------------
// CNssContactHandlerImplementation::ConstructL
// EPOC second phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNssContactHandlerImplementation::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::ConstructL" );

    User::LeaveIfError( iFSession.Connect() );

    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();
    iVoIPFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    iVideoCallFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdCsVideoTelephony );
    // Frees the TLS! Must be done after FeatureManager is used.
    FeatureManager::UnInitializeLib();

    // Before enabling event, start monitoring the PC-suite state
    iSyncBackupWaiter = new (ELeave) CActiveSchedulerWait;
    
    iDataSyncWatcher = CNssDataSyncWatcherFactory::ConstructDataSyncWatcherL( *this );
    
    iBackupObserver = CNssChBackupObserver::NewL( *this );
    
    iContactWaiter = new (ELeave) CActiveSchedulerWait;
    
    iDeletionSimulator = new (ELeave) CSuccessfulDeletionSimulator( *this );

    EnableEventHandling();

    iEventArray.Reset();    // Ready to be used

    TInt errProp = iBusyProperty.Define( KSINDUID, ETrainingContactsState, RProperty::EInt );
    if( ( errProp != KErrNone ) && ( errProp != KErrAlreadyExists ) ) 
        {
        RUBY_ERROR1("CNssContactHandlerImplementation::ConstructL() Failed to define the property. Error [%d]", 
                    errProp);
        User::Leave( errProp );
        }
        
    // Publish contact handler as busy before the DB operations
    errProp = iBusyProperty.Attach( KSINDUID, ETrainingContactsState );
    if( errProp != KErrNone ) 
        {
        // KErrNotFound is also an error as we just defined the property
        RUBY_ERROR1("CNssContactHandlerImplementation::ConstructL() Failed to attach to property. Error [%d]", 
                    errProp);
        User::Leave( errProp );
        }
        
    errProp = iBusyProperty.Set( EContactsTraining );
    if ( errProp != KErrNone )
        {
        RUBY_ERROR1("CNssContactHandlerImplementation::ConstructL() Failed to set property. Error [%d]", 
                     errProp );
        User::Leave( errProp )  ;
        }

    
    // central repository
    iRepository = CRepository::NewL( KCRUidSRSFSettings );
    iRepositoryObserver = CCenRepNotifyHandler::NewL( *this, *iRepository,
                                                      CCenRepNotifyHandler::EIntKey,
                                                      KSRSFFullResyncNeeded );
    iRepositoryObserver->StartListeningL();
    
	//Get VAS DB manager
	iVasDbManager = CNssVASDBMgr::NewL();
	iVasDbManager->InitializeL();
	RUBY_DEBUG0( "CNssContactHandlerImplementation::ConstructL iVasDbManager initialized" );

	//Get Tag Manager and Context Manager interfaces
	iTagManager = iVasDbManager->GetTagMgr();
	iContextManager = iVasDbManager->GetContextMgr();

    // Prepare training parameters
    iTrainingParams = CNssTrainingParameters::NewL();
    
    RUBY_DEBUG0( "CNssContactHandlerImplementation::ConstructL iTrainingParams prepared" );

    // Set languages which have lastname-firstname order
    for ( TInt iCounter = 0; iCounter < KNumberOfSwappedLanguages; iCounter++ )
        {
        User::LeaveIfError( iSwappedLanguages.Append( KSwappedLanguages[iCounter] ) );
        }

#ifdef __SINDE_TRAINING
    // Main array for languages
    RArray<RTrainingLanguageArray> languages;
    
    // Languages for names
    RArray<TLanguage> nameLanguages;
    User::LeaveIfError( nameLanguages.Append( User::Language() ) );
    User::LeaveIfError( nameLanguages.Append( ELangOther ) );
    User::LeaveIfError( languages.Append( nameLanguages ) );
    
    // Languages for extensions
    RArray<TLanguage> extLanguages;
    User::LeaveIfError( extLanguages.Append( User::Language() ) );
    User::LeaveIfError( languages.Append( extLanguages ) );
    
    // Store languages to training parameters
    iTrainingParams->SetSindeLanguages( languages );

    // Close arrays, iTrainingParams has done a copy of these
    extLanguages.Close();
    nameLanguages.Close();
    languages.Close();
    
#else
    // Set the languages
    RArray<TLanguage> *languages = new (ELeave) RArray<TLanguage>;
    CleanupStack::PushL( languages );
    // Always generate a pronunciation in UI language
    User::LeaveIfError( languages->Append( User::Language() ) );
    // give some extra languages, which can be selected in engine
    User::LeaveIfError( languages->Append( ELangOther ) );
    iTrainingParams->SetLanguages( languages );
    CleanupStack::Pop( languages );
#endif
    
    // The separator between the first name and the last name
    iTrainingParams->SetSeparator( iSeparator );
    
#ifdef __SIND_EXTENSIONS

    ReadExtensionsL();
#endif // __SIND_EXTENSIONS    	

	ConnectToPhonebookL();
	
    TLitC<9> name = KNssCHNameDialContext;

    // Get context.

    // (1) if we have synced earlier -> we have created a context, and saved the language
    // (2) if we haven't synced      -> we need to create context
    //
    // In case (2), GetContext fails.
    // We'll assume that GetContext is successful until it fails.
    iHaveWeEverSynced = ETrue;

    TInt err = iContextManager->GetContext( this, name );
    RUBY_DEBUG1( "CNssContactHandlerImplementation::ConstructL Requested GetContext. err [%d]", err);

    if ( err != KErrNone )
        {
        // The same function is called, whether GetContext fails synchronously
        // or asynchronously.
        GetContextCompleted( NULL, KErrNotFound );
        }
   	iDeleteTagListArray = new (ELeave) CArrayPtrFlat<MNssTag>(50);
   	
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::NewL
// Two-phased constructor.
// ---------------------------------------------------------
// 
CNssContactHandlerImplementation* CNssContactHandlerImplementation::NewL( )
    {
    RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::NewL" );
    CNssContactHandlerImplementation* self = new (ELeave) CNssContactHandlerImplementation();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::~CNssContactHandlerImplementation
// Destructor
// ---------------------------------------------------------
// 
CNssContactHandlerImplementation::~CNssContactHandlerImplementation()
   {
   RUBY_DEBUG0( "CNssContactHandlerImplementation::~CNssContactHandlerImplementation FIXED" );
   
   iFSession.Close();

   iEventArray.Close();     // Close the RArray

   delete iDataSyncWatcher;
   delete iBackupObserver;
   
   if ( iSyncBackupWaiter != NULL )
       {
       if( iSyncBackupWaiter->IsStarted() ) 
           {
           iSyncBackupWaiter->AsyncStop();
           }
       }
   delete iSyncBackupWaiter;
   
   if ( iContactWaiter != NULL )
       {
       if( iContactWaiter->IsStarted() ) 
           {
           iContactWaiter->AsyncStop();
           }
       }
   

   delete iContactWaiter;
   delete iDeletionSimulator;
   delete iPeriodicHandleEventTimer;

   delete iContext;

   delete iVasDbManager;
   RProperty::Delete( KSINDUID, ETrainingContactsState );
   iBusyProperty.Close();
   
   delete iRepositoryObserver;
   delete iRepository;
   
   delete iTrainingParams;
   iTagArray.ResetAndDestroy();

   for( TInt k( 0 ); k < iContactQueue.Count(); k++ )
       {
       delete iContactQueue[k].iTitle;
       }
   iContactQueue.Close();

   iDelList.Close();
   iAddList.Close();
   iRemoveIconList.Close();
   iVoiceTagAddedList.Close();
   iSwappedLanguages.Close();
   
   DisconnectFromPhonebook();
   
#ifdef __SIND_EXTENSIONS
    
    for ( TInt i(0); i < iExtensionList.Count(); i++ )
       {
       delete iExtensionList[i].iText;
       }

    iExtensionList.Close();
#endif // __SIND_EXTENSIONS    
   }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::HandleEventL
// Called by phonebook handler when a contact database event
// occurs
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::HandleEventL( const TPhonebookEvent& aEvent )
    {
    RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::HandleEventL" );
    RUBY_DEBUG2( "CNssContactHandlerImplementation::HandleEventL(). Contact id [%d], event type [%d]", aEvent.iContactId, aEvent.iType );
    
    if ( aEvent.iType == EContactChanged ||
         aEvent.iType == EContactDeleted ||
         aEvent.iType == EContactAdded   ||
         aEvent.iType == EUnknownChanges )
        {
        if( !CausedByVoiceTagAddition( aEvent ) )
        	{
        	// Check if operations can be finished
        	// There should be enough disk space
        	if ( NoDiskSpace() )
        	    {
        	    RUBY_DEBUG0( "CNssContactHandlerImplementation::HandleEventL not enough disk space, disabling event handling" );
        	    DisableEventHandling();
        	    }
        	else
        	    {
                TInt errProp = iBusyProperty.Set( EContactsTraining );
                if ( errProp != KErrNone )
                    {
                    RUBY_ERROR1("CNssContactHandlerImplementation::ConstructL() Failed to set property. Error [%d]", 
                                 errProp );
                    User::Leave( errProp )  ;
                    }
        	    UpdatePeriodicActionVars();
	            iEventArray.Append( aEvent );

    	        if ( iState == ECHIdle )
        	     	{
            	    RUBY_DEBUG0( "CNssContactHandlerImplementation::HandleEventL() iState == ECHIdle => DoHandleEvent" );

            	    // Process the first new event from the queue
            	    TRAPD( err, DoHandleEventsL() );
            	    RUBY_DEBUG1( "CNssContactHandlerImplementation::HandleEventL. Assert would fail and Panic IF err != KErrNone. err [%d]", err );
                    RUBY_ASSERT_DEBUG( err == KErrNone, User::Panic( KFile, __LINE__ ) );
                
                    if ( err != KErrNone )
                        {
                        RUBY_ERROR1( "CNssContactHandlerImplementation::HandleEventL. Error [%d] in DoHandleEventsL", err );
                        }
                    }
         	    else 
	        	    {
	        	    RUBY_DEBUG1( "CNssContactHandlerImplementation::HandleEventL() iState == [%d] => Do not handle the event right now", iState );
	        	    } // if iState else
        	    }
            }  // if caused be voice tag addition
	    else
        	{
			RUBY_DEBUG2( "CNssContactHandlerImplementation::HandleEventL(). Event for Contact id [%d] was caused by a voice icon addition and will be ignored, eventArray size [%d]", aEvent.iContactId, iEventArray.Count() );
			if ( ( iVoiceTagAddedList.Count() == 0 ) && ( iState == ECHIdle ) && ( iEventArray.Count() == 0 ) )  
			    {
			    RUBY_DEBUG0( "HandleEventL No more voice icon events expected, nothing pending." );
			    }
			else 
			    {
			    RUBY_DEBUG1( "HandleEventL [%d] voice icon events are still expected", iVoiceTagAddedList.Count() );
			    }
        	}
        }
    else if ( aEvent.iType == EStoreRestoreBeginning )
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::HandleEventL restore begin" );
        iRestoreRunning = ETrue; // set to differentiate ending of restore or backup
        }
    else if ( aEvent.iType == EStoreBackupRestoreCompleted && iRestoreRunning )
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::HandleEventL restore end" );
        // Set value to make full resync
        SetFullResyncCrFlagL( ETrue );
        iRestoreRunning = EFalse;
        } // if event type
    }



// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoHandleEventL
// Process the Event from phonebook
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoHandleEventsL()
	{
	RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::DoHandleEventsL" );
	RUBY_DEBUG1( "CNssContactHandlerImplementation::DoHandleEventsL, eventArray size [%d]", iEventArray.Count() );

	// If there is no lengthy higher level routine executed
	if( !PeriodicAction() ) 
		{
		RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventsL !PeriodicAction()" );
		if ( iHandlerEnabled )
        	{
   	    	if ( IsFullResyncNeeded() )
       		   	{
       			RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventsL() Full resync if needed. Setting iState to ECHFullResync" );
				SetState( ECHFullResync );
           		
            	iEventArray.Close();
	
   	        	FullResyncStart();
       	    	}
   			else
       	   		{
       			RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventsL() Full resync if NOT needed. Setting iState to ECHHandlingNormalChanges" );
       			SetState( ECHHandlingNormalChanges );

       			// There are 3 phases in handling events:
	          	// (1) Make two lists:
   	     	 	//  * Names to be added
       	  		//  * Names to be deleted
          		CollectAddRemoveListsL();

   	      		// (2) Delete the names to be deleted. This operation is asynchronous.
       	  		DoRemoveNamesL();

       			// (3) After removing names, add the names to be added.
                // DoRemoveNamesL() starts the step (3)
         		}
    		}
    	else 
    		{
    		// In UREL, empty "else" branch will be eliminated by the compiler
			RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventsL() Handler was disabled" );
    		}
		
   		RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventsL() completed" );
		
		}// if !PeriodicAction
		
	else 
		{
		// In UREL, empty "else" branch will be eliminated by the compiler
		RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventsL PeriodicAction detected, DoHandleEventsL omitted" );
		}
		
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DeleteTagCompleted
// Called when the deletion of a tag is completed successfully
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DeleteTagCompleted( TInt aErrorCode )
	{
	if( aErrorCode == KErrNone )
	    {
    	RUBY_DEBUG1( "CNssContactHandlerImplementation::DeleteTagCompleted. Assert iState == ECHHandlingNormalChanges. iState [%d]", iState);
        RUBY_ASSERT_DEBUG( iState == ECHHandlingNormalChanges, User::Panic( KFile, __LINE__ ) );	    
	    }
    else
        {
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DeleteTagCompleted: tag deletion failed" );
    	
    	iNormalChangesError = aErrorCode;
        }	
    
    DoRemoveNamesAfterDeleteTagCompleted();
	}

// -----------------------------------------------------------------
// CNssContactHandlerImplementation::GetTagCompleted
// Called by tag manager when getting of a tag list completes
// -----------------------------------------------------------------
//
void CNssContactHandlerImplementation::GetTagListCompleted(
    MNssTagListArray* aTagList, TInt aErrorCode )
    {
    if( aErrorCode == KErrNone )
        {
        if ( iState == ECHHandlingNormalChanges )
            {
            DoRemoveNamesAfterGetTagList( aTagList );
            }
        else
            {
            RUBY_DEBUG1( "CNssContactHandlerImplementation::GetTagListCompleted : Unknown state(%d)\n", iState );
    		}
        }
    else
        {
        RUBY_DEBUG1( "CNssContactHandlerImplementation::GetTagListCompleted. Error [%d]", aErrorCode );
        
        TRAPD( err, DoHandleEventFailedL() );    // Process the event fail
        if ( err != KErrNone )
            {
            RUBY_ERROR1( "CNssContactHandlerImplementation::GetTagFailed. Error [%d] in DoHandleEventFailedL", err );
            }
        }
	}


// ---------------------------------------------------------
// CNssContactHandlerImplementation::DisableEventHandling
// Disables the event handling. Is used in case of unrecoverable error
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DisableEventHandling()
   {
   RUBY_DEBUG0( "CNssContactHandlerImplementation::DisableEventHandling" );
   iHandlerEnabled = EFalse;
   }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::EnableEventHandling
// Enables event handling
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::EnableEventHandling()
   {
   RUBY_DEBUG0( "CNssContactHandlerImplementation::EnableEventHandling" );
   iHandlerEnabled = ETrue;
   }


// ---------------------------------------------------------
// CNssContactHandlerImplementation::GetContextListCompleted
// Called by tag manager when GetContextList() completes successfully
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::GetContextListCompleted(
    MNssContextListArray*  /*aContextList*/, TInt /*aErrorCode*/ )
    {
    RUBY_ERROR0( "CNssContactHandlerImplementation::GetContextListCompleted - this function should never be called." );
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::CheckErrorSigns
// Compares client data (saved in VAS context)
// to state of the phonebook to find out if
// full synchronization is needed.
//
// If full resync is needed initiates it by saving into the event queue
// special 'full resync event'
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::CheckErrorSigns()
    {
    
    RUBY_DEBUG0( "CNssContactHandlerImplementation::CheckErrorSigns" );
    TBool errorFound( EFalse );
    // Full resync, if this is the first time we launch.
    //              if the UI language has changed.
    //              if VAS Db and Contact Db time stamps do not match
    if  ( iClientData.iLanguage == ELangNone ||
        iClientData.iLanguage != User::Language() )
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::CheckErrorSigns language changed." );
        errorFound = ETrue;
        }
    else
        {
        // check that is full resyncronization needed
        iRepository->Get( KSRSFFullResyncNeeded, errorFound );
        }
        
    if ( errorFound )
        {
        TPhonebookEvent event;
       	event.iType = ENullEvent;
       	event.iContactId = 0;
        
        if ( iEventArray.Append( event ) != KErrNone )
            {
            RUBY_ERROR0( "CNssContactHandlerImplementation::CheckErrorSigns Failed to append to iEventArray" );
            }        
        }
    
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::GetContextCompleted
// Called by tag manager, when GetContext() completes successfully
//
// Depending on the contact handler state, continues full resync sequence or
// sets the idle state
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::GetContextCompleted(
    MNssContext* aContext, TInt aErrorCode )
	{
	if( aErrorCode == KErrNone )
	    {
        RUBY_ASSERT_DEBUG( iState == ECHInitializing || iState == ECHFullResync,
                        User::Panic( KFile, __LINE__ ) );

    	RUBY_DEBUG0( "CNssContactHandlerImplementation::GetContextCompleted" );
        iContext = aContext;
        TRAPD(err,

            iClientData.InternalizeL( iContext->ClientData() );

    	    if ( iState == ECHInitializing )
    	        {
    	        // If there are warning sings of error,
    	        // do full resynchronization.
    	        CheckErrorSigns();

    	        SetState( ECHIdle );
    	        RUBY_DEBUG0( "CNssContactHandlerImplementation::GetContextCompleted. Setting iState to ECHIdle" );
    		
    	        if ( iEventArray.Count() ) // Verify whether more events in the queue
    	            {
    	            
    	    		RUBY_DEBUG1( "CNssContactHandlerImplementation::GetContextCompletedhere are still [%d] events in the queue. Call DoHandleEvents for them", iEventArray.Count() );
    				
    	            DoHandleEventsL(); // Process the 1st new event from the queue
    	            }
    	        }
    	    else if ( iState == ECHFullResync )
    	        {
    	        TRAPD( error,  FullResyncCreateChangesL() );
    	        if( error != KErrNone ) 
    	            {
    	            RUBY_ERROR1( "FullResyncCreateChangesL failed with [%d]. Panic, nothing can be done", error );
    	            RUBY_ASSERT_DEBUG( EFalse, User::Panic( KFile, __LINE__ ) );
    	            }
    	        }
    	    else 
    	    	{
    	    	RUBY_ERROR0( "CNssContactHandlerImplementation::GetContextCompleted Unexpected state" );
    	    	RUBY_ASSERT_DEBUG( EFalse, User::Panic( KFile, __LINE__ ) );
    	    	}
    	    );  // TRAPD
    	if( err != KErrNone ) 
    		{
    		RUBY_ERROR1( "CNssContactHandlerImplementation::GetContextCompleted Leaves with [%d] inside", err );
    		}
        RUBY_DEBUG0( "CNssContactHandlerImplementation::GetContextCompleted completed" );	    
	    }
	    
	else // aErrorCode != KErrNone
	    {
        RUBY_ASSERT_DEBUG( iState == ECHInitializing || iState == ECHFullResync,
                        User::Panic( KFile, __LINE__ ) );

        if ( iState == ECHInitializing )
            {
            iHaveWeEverSynced = EFalse;
            SetState ( ECHFullResync );
            FullResyncStart();
            }	    
	    }

	}

// ---------------------------------------------------------
// CNssContactHandlerImplementation::SaveContextCompleted
// Called when SaveContext() completes successfully
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::SaveContextCompleted( TInt aErrorCode )
    {
    if( aErrorCode == KErrNone )
        {
        RUBY_ASSERT_DEBUG( iState == ECHInitializing || 
                        iState == ECHFullResync,
                        User::Panic( KFile, __LINE__ ) );

        iHaveWeEverSynced = ETrue;

        if ( iState == ECHInitializing )
            {
            // Retrive the Context to get the new ContextId from VasDB
            iContextManager->GetContext( this, KNssCHNameDialContext );
            }
        else if ( iState == ECHFullResync )
            {
            TRAPD( error, FullResyncCreateChangesL() );
            if ( error != KErrNone ) 
                {
                RUBY_ERROR1( "SaveContextCompleted Failed to create full resync changes. Critical error [%d]", 
                            error );
                DisableEventHandling();
                }  // if error != KErrNone
            }  // else if ( iState == ECHFullResync )
        else 
        	{
        	// we should never be here, because the condition is checked 
        	// by ASSERT_DEBUG at the start of the function
        	RUBY_ERROR0( "CNssContactHandlerImplementation::SaveContextCompleted Unexpected state!" );
        	}        
        }
        
    else // aErrorCode != KErrNone
        {
        // Failed to initialize will collect the events forever and will never process them
        // Will try to initialize again after reboot
        RUBY_ERROR0( "CNssContactHandlerImplementation::SaveContextCompleted failed. Staying forever in ECHFullResync" );
        RUBY_ERROR0( "CNssContactHandlerImplementation::SaveContextCompleted failed. Reboot to try initializing again" );
        DisableEventHandling();        
        }
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::SaveTagCompleted
// Called when SaveTag() completes
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::SaveTagCompleted( TInt aErrorCode )
    {
    RUBY_ASSERT_DEBUG( iState == ECHHandlingNormalChanges, User::Panic( KFile, __LINE__ ) );
    
    DoAddNamesSaveTagCompleted( aErrorCode );
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::GetContactTitleL
// Populates aTitle with either "FirstName LastName" or "CompanyName".
// "CompanyName" is given only if both First and Last names are missing.
//---------------------------------------------------------
//
HBufC* CNssContactHandlerImplementation::GetContactTitleL( TContactItemId aContactId, 
                                                           TBool aUseDefaultOrder,
                                                           TBool& aBothFirstAndLastFound,
                                                           TVasTagType& aTagType )
	{
	RUBY_DEBUG_BLOCKL( "CNssContactHandlerImplementation::GetContactTitleL" );
    // Find out if lastname-firstname should be used
    TBool useLastFirstOrder = EFalse;
    
    // By default, this function returns firstname and/or lastname
    aTagType = ETagTypeName;

    if ( aUseDefaultOrder )
        {
        useLastFirstOrder = SwapNameOrder();
        }
    else
        {
        useLastFirstOrder = !SwapNameOrder();
        }
    WaitUntilNoSyncBackupIsRunning();
    ReadContactL( aContactId );
    
    HBufC* firstNameText = NULL;
    HBufC* lastNameText = NULL;
    HBufC* companyNameText = NULL;
    
    TInt firstLength( 0 );
    TInt lastLength( 0 );
    TInt companyLength( 0 );    
    
    TInt error = KErrNone;
    
    // name reading fields are used in Japanese
	TRAP( error, iPbkHandler->FindFieldL( EPbkFieldIdFirstNameReading ) );
    if ( error == KErrNotFound ) 
        {
        // not found
        TRAP( error, iPbkHandler->FindFieldL( EPbkFieldIdFirstName ) );
        }
        
    if ( error == KErrNone )
        {
        firstNameText = iPbkHandler->TextL().AllocLC();
        TrimName( firstNameText );
        firstLength = firstNameText->Length();
        if ( firstLength == 0 )
            {
            CleanupStack::PopAndDestroy( firstNameText );
            firstNameText = NULL;
            }
        }

    TRAP( error, iPbkHandler->FindFieldL( EPbkFieldIdLastNameReading ) );
    if ( error == KErrNotFound ) 
        {
        // not found
        TRAP( error, iPbkHandler->FindFieldL( EPbkFieldIdLastName ) );
        }
        
    if ( error == KErrNone )
        {
        lastNameText = iPbkHandler->TextL().AllocLC();
        TrimName( lastNameText );
        lastLength = lastNameText->Length();
        if ( lastLength == 0 )
            {
            CleanupStack::PopAndDestroy( lastNameText );
            lastNameText = NULL;
            }        
        }

    TRAP( error, iPbkHandler->FindFieldL( EPbkFieldIdCompanyName ) );
    
    if ( error == KErrNone )
        {
        companyNameText = iPbkHandler->TextL().AllocLC();
        TrimName( companyNameText );
        companyLength = companyNameText->Length();
        if ( companyLength == 0 )
            {
            CleanupStack::PopAndDestroy( companyNameText );
            companyNameText = NULL;
            }         
        }

    // Use third parameter to return a flag if both lastname and firstname are found
    if ( ( firstLength > 0 ) && ( lastLength > 0 ) )
        {
        aBothFirstAndLastFound = ETrue;
        }
    else
        {
        aBothFirstAndLastFound = EFalse;
        }

    HBufC* title( NULL );

#ifdef __SIND_EXTENSIONS
    if ( firstLength + lastLength > 0 )
        {
        TInt spaceForIndexes( 0 );
        // Find out how much more space is needed for "training indexes"
        if ( aBothFirstAndLastFound )
            {
            spaceForIndexes = 2 * KTrainingIndexSize;
            }
        else
            {
            spaceForIndexes = KTrainingIndexSize;
            }
            
        title = HBufC::NewL( firstLength + lastLength + spaceForIndexes );
        
        // Check the order 
        if ( useLastFirstOrder )
            {
            // Lastname-firstname
            if ( lastNameText ) 
                {
                AppendName( title, lastNameText );
                }
            
            if ( firstNameText )
                {
                AppendName( title, firstNameText );
                }
            }
        else
            {
            // Firstname-lastname
            if ( firstNameText )
                {
                AppendName( title, firstNameText );
                }
                
            if ( lastNameText ) 
                {
                AppendName( title, lastNameText );
                }
            }
        }
    // Try company name, if the personal name fails.
    else if ( companyLength > 0 )
        {
        aTagType = ETagTypeCompanyName;
        title = HBufC::NewL( companyLength + KTrainingIndexSize );
        AppendName( title, companyNameText );
        }
#else
    // Use personal name, if there is one.
    if ( firstLength + lastLength > 0 )
        {
        title = HBufC::NewL( firstLength + 1 + lastLength );

        // Check the order 
        if ( useLastFirstOrder )
            {
            // Lastname-firstname
            if ( lastNameText ) 
                {
                title->Des().Append( lastNameText->Des() );

                if ( firstNameText )
                    {
                    // Separator marker
                    title->Des().Append( iSeparator );
                    }
                }
            
            if ( firstNameText )
                {
                title->Des().Append( firstNameText->Des() );
                }


            }
        else
            {
            // Firstname-lastname
            if ( firstNameText ) 
                {
                title->Des().Append( firstNameText->Des() );

                if ( lastNameText )
                    {
                    // Separator marker
                    title->Des().Append( iSeparator );
                    }
                }
                
            if ( lastNameText )
                {
                title->Des().Append( lastNameText->Des() );
                }

            }
        }
    // Try company name, if the personal name fails.
    else if ( companyLength > 0 )
        {
        aTagType = ETagTypeCompanyName;
        title = HBufC::NewL( companyLength );
        title->Des().Append( companyNameText->Des() );
        }
#endif // __SIND_EXTENSIONS
    // It is possible to make a contact without a name.
    else
        {
        RUBY_DEBUG1( "CNssContactHandlerImplementation::ContactTitle: No name, ContactId=[%d]" , aContactId );
        }

    if ( companyNameText )
        {
        CleanupStack::PopAndDestroy( companyNameText );
        }
        
    if ( lastNameText )
        {
        CleanupStack::PopAndDestroy( lastNameText );
        }
        
    if ( firstNameText )
        {
        CleanupStack::PopAndDestroy( firstNameText );
        }
    
    return title;
	}


// ---------------------------------------------------------
// CNssContactHandlerImplementation::SwapNameOrder
// Returns ETrue if name order for current UI language should
// be lastname-firstname, EFalse otherwise
//---------------------------------------------------------
//
TBool CNssContactHandlerImplementation::SwapNameOrder()
    {
    // Find UI language
    TLanguage uiLanguage = User::Language();

    if ( iSwappedLanguages.Find( uiLanguage ) == KErrNotFound )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }


// ---------------------------------------------------------
// CNssContactHandlerImplementation::GetContactNicknameL
// Returns nickname for a given contact
//---------------------------------------------------------
//
HBufC* CNssContactHandlerImplementation::GetContactNicknameL( TContactItemId aContactId )
	{
	RUBY_DEBUG_BLOCKL( "CNssContactHandlerImplementation::GetContactNicknameL" );
	WaitUntilNoSyncBackupIsRunning();
    ReadContactL( aContactId );

	// Get the nick name
	iPbkHandler->FindFieldL( EPbkFieldIdSecondName );

    // Get the length of the additional name.
    TInt nickLength = iPbkHandler->TextL().Length();

    HBufC* title = NULL;

#ifdef __SIND_EXTENSIONS
    // Use personal name, if there is one.
    if ( nickLength > 0 )
        {
        title = HBufC::NewL( nickLength + KTrainingIndexSize );
        
        HBufC* nickName = iPbkHandler->TextL().AllocL();
        TrimName( nickName );
        AppendName( title, nickName );
        delete nickName;
        }
#else
    // Use personal name, if there is one.
    if ( nickLength > 0 )
        {
        title = HBufC::NewL( nickLength );
        title->Des().Append( iPbkHandler->TextL() );
        }
#endif // __SIND_EXTENSIONS

    return title;
	}

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoHandleEventFailed
// Routine to process when handling the contact event failed
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoHandleEventFailedL()
    {
    RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventFailedL start" );
    
    // Errors are normal if user deletes contact without a voice tag
    // E.g. because this contact had no number
    if( ( iDeleteTagListArray != NULL ) && ( iDeleteTagListArray->Count() > 0 ) ) 
    	{
    	// simulate successful deletion, just return no tags for further deletions
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventFailedL Deletion failed. Simulating successful empty deletion" );
    	iDeletionSimulator->Call();
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventFailedL Called deletion simulator" );
    	}
    else 
    	{
    	
	    // Try full resync couple of times. If still fails, stop contact handler. 
	    // When the phone is booted for the next time, try resyncing again
    	if( iConsecutiveErrorFullResync < KMaxConsecutiveErrorFullResync ) 
    		{
    		iConsecutiveErrorFullResync++;
    		RUBY_DEBUG1( "CNssContactHandlerImplementation::DoHandleEventFailedL Trying full resync for the [%d] time", iConsecutiveErrorFullResync);
    		// clean structures that might be already full
    		iTagArray.ResetAndDestroy();

		    for( TInt k( 0 ); k < iContactQueue.Count(); k++ )
		    	{
		       	delete iContactQueue[k].iTitle;
		       	}
		   	iContactQueue.Close();

		   	iDelList.Close();
		   	iAddList.Close();
		   	iVoiceTagAddedList.Close();
    		// generate full resync event
    		TPhonebookEvent event;
        	event.iType = ENullEvent;
        	event.iContactId = 0;
        	iEventArray.Insert( event, 0 );
        	// will start full resync
        	iTagArray.ResetAndDestroy();
        	iSaveCallbackCounter = 0; // simulate completed action
        	RUBY_DEBUG0( "DoHandleEventFailedL No leaving in the FullResync branch, retrying" );
        	DoHandleEventsL(); // No leaving in the FullResync branch
    		} 
    	else
	    	{
	    	RUBY_ERROR1( "CNssContactHandlerImplementation::DoHandleEventFailedL Tried full resync for %d times. Disabling contact handler until reboot", KMaxConsecutiveErrorFullResync );
    		// Tried several consecutive resyncs, didn't help
    		// Disable itself until reboot
    		DisableEventHandling();
    		}  
    	}  // if deletion else
    RUBY_DEBUG0( "CNssContactHandlerImplementation::DoHandleEventFailedL end" );
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::PhonebookOrder
// TLinearOrder comparison function for sorting phonebook events
// according to their Phonebook ID.
//---------------------------------------------------------
//
TInt CNssContactHandlerImplementation::PhonebookOrder( const TPhonebookEvent& a1, const TPhonebookEvent& a2 )
    {
    if ( a1.iContactId != a2.iContactId )
        {
        return( a1.iContactId - a2.iContactId );
        }

    if ( a1.iTime < a2.iTime )
        {
        return( -1 );
        }
    else if ( a1.iTime > a2.iTime )
        {
        return( 1 );
        }
    else{
        return( 0 );
        }
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::IsFullResyncNeeded
// Checks if the event queue contains an event, which
// launches full resynchronization. If it does, there is
// no point in processing the other events.
// ---------------------------------------------------------
//
TBool CNssContactHandlerImplementation::IsFullResyncNeeded()
    {
    for( TInt k( 0 ); k < iEventArray.Count(); k++ )
        {
        // Null event is used inside contact handler to signal full resync.
        if ( iEventArray[k].iType == ENullEvent
             || iEventArray[k].iType == EStoreBackupRestoreCompleted 
             || iEventArray[k].iType == EUnknownChanges )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::CollectAddRemoveListsL
// Given a list of Contact DB Events, this function compiles
// a list of ids to-be-deleted and ids to-be-added.
// A Contact DB Event contains:
//  * Contact ID
//  * Event type (added, modified, removed)
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::CollectAddRemoveListsL()
    {
    RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::CollectAddRemoveListsL" );
    RUBY_DEBUG1( "CNssContactHandlerImplementation::CollectAddRemoveListsL(). iEventArray.Count [%d]", iEventArray.Count() );
    TInt k( 0 );
	
	RUBY_ASSERT_DEBUG( iAddList.Count() == 0, User::Panic( KFile, __LINE__ ) );
    RUBY_ASSERT_DEBUG( iDelList.Count() == 0, User::Panic( KFile, __LINE__ ) );

    // We collect two lists:
    //  * Contacts to be deleted.
    //  * Contacts to be added.

    TLinearOrder<TPhonebookEvent> order( PhonebookOrder );

    // Sort events according to the phonebook ID. This way, the events
    // modifying the same name are sequentially.
    iEventArray.Sort( order );
	

    // If several events happen for the same contact, fuse them into one event.
    for ( k = 0; k < iEventArray.Count() - 1; k++ )
        {
        if ( iEventArray[k].iContactId == iEventArray[k+1].iContactId )
            {
            // Sorting should have preserved the order of the events
            RUBY_ASSERT_DEBUG(iEventArray[k+1].iTime >= iEventArray[k].iTime, User::Panic( KFile, __LINE__ ) );
            RUBY_DEBUG1( "CNssContactHandlerImplementation::CollectAddRemoveListsL(). Fusing events for ContactId [%d]", iEventArray[k].iContactId );
            iEventArray[k+1].iType = EContactChanged;
            iEventArray.Remove( k );

            // We have removed an element
            k--;
            }
        }

	RUBY_DEBUG1( "CNssContactHandlerImplementation::CollectAddRemoveListsL(). Events sorted and fused iEventArray.Count [%d]", iEventArray.Count() );

    for( k = 0; k < iEventArray.Count() && k < KMaxNamesAtOnce; k++ )
        {
        TPhonebookEvent* event = &iEventArray[k];

        switch ( event->iType )
            {
            case EContactChanged:
                {
                // events caused by saving voice tag field are skipped 
                // earlier, in HandleDatabaseEventL

                // We need to delete the old name
                User::LeaveIfError( iDelList.Append( *event ) ); 
                // and after that, add the new name.
                User::LeaveIfError( iAddList.Append( *event ) ); 
                break;
                }
            case EContactDeleted:
                {
                User::LeaveIfError( iDelList.Append( *event ) );
                break;
                }
            case EContactAdded:
                {
                User::LeaveIfError( iAddList.Append( *event ) );
                break;
                }
            default:
                RUBY_ERROR1( "CNssContactHandlerImplementation::CollectAddRemoveListsL() Unkown event type: %d", event->iType );
                break;
            }
        }

    if ( k < KMaxNamesAtOnce )
        {
        iEventArray.Close();
        }
    else
        {
    	RUBY_DEBUG1( "CNssContactHandlerImplementation::CollectAddRemoveListsL(). k >= KMaxNamesAtOnce [%d]. Deleting first KMaxNamesAtOnce events, they've been sorted into corresponding lists already",
    	            KMaxNamesAtOnce);
        for ( k = 0; k < KMaxNamesAtOnce; k++ )
            {
            iEventArray.Remove(0);
            }
        }
    RUBY_DEBUG1( "CNssContactHandlerImplementation::CollectAddRemoveListsL() iEventArray.Count [%d]",
                iEventArray.Count());
	RUBY_DEBUG2( "CNssContactHandlerImplementation::CollectAddRemoveListsL iDelList.Count() [%d], iAddList.Count() [%d]", iDelList.Count(), iAddList.Count());
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoRemoveNamesL
// Starts name removal sequence
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoRemoveNamesL()
    {
    RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::DoRemoveNamesL" );
    if ( iDelList.Count() == 0 )
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesL Nothing to delete - skipping" );

        RemovingNamesCompletedL();
        RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesL completed early <= nothing to remove" );
        return;
        }

    RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesL - removing" );

    // we have: a list of tag IDs, which should be removed
    // we need: a list of tags, which should be removed.

    // First, we get the tags which correspond to the tag ids.
    // We need k async calls to VAS,
    // where k is the number of tag ids in iDelList.
    // We get a list of k' tags, where k' >= k.
    // If some tag has both proper name and a nickname, then k' > k.
    
    iContactId = iDelList[0].iContactId;

    if ( iDeleteTagListArray )
        {
        iDeleteTagListArray->ResetAndDestroy();
        }
    else
        {
        iDeleteTagListArray = new (ELeave) CArrayPtrFlat<MNssTag>(50);
        }

    TInt retCode = iTagManager->GetTagList( this, iContext, iContactId, 0 );

    if ( retCode != KErrNone )
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesL failed to GetTagList" );
        // In the case of change event, try to add names even though they are not found
        // Might happen in scenario like this:
        // 1. User adds a name without any phone number -> no voice tag is created
        // 2. Contact is modified by adding a phone number
        iDelList.Close();
        DoAddNamesL();
        }
    RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesL completed" );
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList
// If contact database announces that n contacts need to be
// deleted, we do n calls to GetTagList (the first is from DoRemoveNamesL). 
// Each callback returns 1 or more tags (name + nickname = 2 tags). 
// This function processes the callbacks by adding the tags to the 
// to-be-deleted list.
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList( MNssTagListArray* aTagList )
    {
    RUBY_DEBUG1( "CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList. iState [%d]", iState );

    if ( iDelList.Count() == 0 ) 
		{
		RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList iDelList.Count() == 0" );
		}

    // Sieve out "false" tags: Those which have a wrong contact ID.
    /** Why? Maybe user was fast enough to delete these contacts from the phonebook 
    *   Anyway, why should we keep "false" tags in our db?
    */
    for ( TInt i=0; i < aTagList->Count(); i++ )
        {
        CArrayFixFlat<TInt>* intArray = aTagList->At(i)->RRD()->IntArray();
	
        RUBY_ASSERT_DEBUG( intArray->Count() > 0, User::Panic( KFile, __LINE__ ) );

        if ( intArray->At(0) != iContactId )
            {
            RUBY_ERROR2( "DoRemoveNamesAfterGetTagList expected contact id %d, but got contact id: %d", iContactId, intArray->At(0) );
            delete aTagList->At( i );
            aTagList->Delete( i );
            }
        }
        
//    TRAPD( err, // trapping is a fix to make the function non-leaving

	    // Add the tags to the to-be-deleted list.
	    // (there are 1 or 2 tags in aTagList depending on
	    //  whether nickname is also there or not)
	    TRAPD(err,
		for ( TInt k( 0 ); k < aTagList->Count(); k++ )
	        {
	        iDeleteTagListArray->AppendL( (*aTagList)[k] );
	        }
			);
		if ( err != KErrNone )
			{
			// Destroy tag list
	    	aTagList->Reset();
	    	delete aTagList;
	    	aTagList = 0;
	        return;
			}
	    // We have processed this ID
	    RUBY_DEBUG1( "CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList Before removal. iDelList.Count [%d]", iDelList.Count() );
	    if( iDelList.Count() > 0 ) 
	    	{
	    	iDelList.Remove( 0 );
	    	} 
	    else 
	    	{
	    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList Empty iDelList!" );
	    	}
	    RUBY_DEBUG1( "CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList After removal. iDelList.Count [%d]", iDelList.Count() );

	    // If there are still more IDs, convert the next ID into a tag.
	    if ( iDelList.Count() > 0 )
	        {
	        RUBY_DEBUG1( "CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList There are [%d] contacts to delete. Calling GetTagList for them", iDelList.Count() );
	        iContactId = iDelList[0].iContactId;

	        TInt retCode 
	            = iTagManager->GetTagList( this, iContext, iContactId, 0 );

	        if ( retCode != KErrNone )
	            {
	            RUBY_DEBUG0( "DoRemoveNamesAfterGetTagList Failed to GetTagList, going to DoHandleEventFailedL" );
	            TRAP_IGNORE( DoHandleEventFailedL() );      // to handle the process event failed
	            /** @todo reengineer into single return path */
	            // Destroy tag list
	    		aTagList->Reset();
	    		delete aTagList;
	    		aTagList = 0;
	            return;
	            }
	        }
	        
	    // If we just finished converting the last ID, move to the next phase.
	    else{
	        RUBY_DEBUG0( "DoRemoveNamesAfterGetTagList Moving to the next phase" );
	        DoRemoveNamesCallDeleteTag();
	        }
//        );  // TRAPD
//        if( err != KErrNone ) 
//        	{
//        	RUBY_ERROR1( "CNssContactHandlerImplementation::DoRemoveNamesAfterGetTagList Leaves with [%d] inside", err );
//        	}
		// Destroy tag list
	    aTagList->Reset();
	    delete aTagList;
	    aTagList = 0;
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoRemoveNamesCallDeleteTag
// Sends the DeleteTag() calls to lower layers when removing names.
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoRemoveNamesCallDeleteTag()
    {
    RUBY_DEBUG1( "CNssContactHandlerImplementation::DoRemoveNamesCallDeleteTag. iDeleteTagListArray->Count() [%d] Setting iDeleteCallbackCounter = 0", 
                iDeleteTagListArray->Count() );
    
    if( iDeleteTagListArray->Count() > 0 ) 
    	{
    	
	    iDeleteCallbackCounter = 0;

	    for ( TInt k( 0 ); k < iDeleteTagListArray->Count(); k++ )
	        {
	        TInt ret = 
	            iTagManager->DeleteTag( this, (*iDeleteTagListArray)[k] );

	        if ( ret != KErrNone )
	            {
	            TRAPD( err, DoHandleEventFailedL(); );
	            if( err != KErrNone ) 
	            	{
	            	RUBY_ERROR1( "CNssContactHandlerImplementation::DoRemoveNamesCallDeleteTag Error handling Left with [%d]", err );
	            	}
	            return;  /** @todo reengineer into single return path */
	            }
	        }
    	} // if there are tags to delete
    else
    	{
    	// if there were no tags to delete (attempt to delete non-existing tags)
    	// simulating callback from successful deletion of.. 0 tags
    	//iDeleteCallbackCounter = -1;
    	//DeleteTagCompleted();
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesCallDeleteTag Here!" );
    	}
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoRemoveNamesAfterDeleteTagCompleted
// DeleteTag() callback calls this funciton. It counts
// the number of callbacks, and when all callbacks have arrived,
// goes to the next phase.
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoRemoveNamesAfterDeleteTagCompleted()
	{
    iDeleteCallbackCounter++;
    
    RUBY_DEBUG2( "CNssContactHandlerImplementation::DoRemoveNamesAfterDeleteTagCompleted. iDeleteCallbackCounter [%d], Count() [%d]", iDeleteCallbackCounter, iDeleteTagListArray->Count() );
    
    if ( iDeleteCallbackCounter == iDeleteTagListArray->Count() )
        {
        if( iNormalChangesError == KErrNone ) 
        	{
        	TRAPD( err, RemovingNamesCompletedL() );
    		if ( err != KErrNone )
        		{
        		RUBY_ERROR1( "CNssContactHandlerImplementation::DoRemoveNamesAfterDeleteTagCompleted. Error [%d] in RemovingNamesCompletedL", err );
        		}
        	}
        else 
        	{
        	// There is not much we can do about the deletion error
        	// Retrying deletion can cause endless cycle
        	// Sometimes failed deletions are ok (if we tried to delete non-existing tag)
        	/** @todo reengineer SpeechItemTrainer (and/or SRS plugin) to report deletion 
        	    of non-existing tag as success. After all they are not in the DB */
        	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoRemoveNamesAfterDeleteTagCompleted Deletion failed. Still simulating a successful one" );
        	TRAPD( err, RemovingNamesCompletedL() );
    		if ( err != KErrNone )
        		{
        		RUBY_ERROR1( "CNssContactHandlerImplementation::DoRemoveNamesAfterDeleteTagCompleted. Error [%d] in RemovingNamesCompletedL", err );
        		}
        	}  // if (iNormalChangesError == KErrNone) else
        }  // if count
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::RemovingNamesCompletedL
// Cleans up after some names have been removed
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::RemovingNamesCompletedL()
    {
	RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::RemovingNamesCompletedL" );
    iDelList.Close();

    if ( iDeleteTagListArray )
        {
        iDeleteTagListArray->ResetAndDestroy();
        iDeleteCallbackCounter = 0;
        }

    DoAddNamesL();
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoAddNamesL
// Starts name addition sequence by fetching contacts data and ordering
// voice tags training
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoAddNamesL()
    {
    RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::DoAddNamesL" );
    TInt k( 0 );

    if ( iAddList.Count() == 0 )
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesL Nothing to add. Setting iState = ECHIdle. Return" );
    	SetState ( ECHIdle );
    	// If there are events on the list, DoAddNamesCompleted will fire them
        DoAddNamesCompletedL();  
        }
    else 
        {
        // Create new tags and train them.
        for ( k = 0; k < iAddList.Count(); k++ )
            {
            // 3 names may be created from one contact:
            //  * Main name (firstname + lastname XOR companyname)
            //  * Reversed order name (usually lastname + firstname, depends on the UI language)
            //      -> only if NSSVAS_CONTACTHANDLER_FREEORDER is defined
            //  * Nickname

            TContactData contactData;

            contactData.iID = iAddList[k].iContactId;
            contactData.iSyncTime.UniversalTime();

            // Nickname
            contactData.iTitle = NULL;
            TRAPD( err, contactData.iTitle = GetContactNicknameL( iAddList[k].iContactId ) );

            if ( contactData.iTitle != NULL )
                {
                CleanupStack::PushL( contactData.iTitle );
                contactData.iType = ETagTypeNickName;
                User::LeaveIfError( iContactQueue.Append( contactData ) );
                CleanupStack::Pop( contactData.iTitle );
                }

            // Main name ( Using GetContactTitleL() )
            contactData.iTitle = NULL;
            TBool bothFound = EFalse;
            TVasTagType tagType( ETagTypeUnknown );
            TRAP( err, contactData.iTitle = GetContactTitleL( iAddList[k].iContactId, ETrue, bothFound, tagType ) );

            // If all the name fields are empty, nothing
            // can be done for the tag. We're finished.
            if ( contactData.iTitle != NULL )
                {
                RUBY_DEBUG2( "CNssContactHandlerImplementation::DoAddNamesL For Contact id [%d] title is: [%S]", iAddList[k].iContactId, contactData.iTitle );
                CleanupStack::PushL( contactData.iTitle );
                contactData.iType = tagType;
                User::LeaveIfError( iContactQueue.Append( contactData ) );
                CleanupStack::Pop( contactData.iTitle );
                }

    #ifdef NSSVAS_CONTACTHANDLER_FREEORDER
            // Default order added previously, now adding
            // secondary order if both first and lastnames are found
            if ( bothFound )
                {
                contactData.iTitle = NULL;
                TRAPD( err, contactData.iTitle = GetContactTitleL( iAddList[k].iContactId, EFalse, bothFound, tagType ) );
                if ( contactData.iTitle != NULL )
                    {
                    CleanupStack::PushL( contactData.iTitle );
                    contactData.iType = tagType;
                    User::LeaveIfError( iContactQueue.Append( contactData ) );
                    CleanupStack::Pop( contactData.iTitle );
                    }
                }
    #endif // NSSVAS_CONTACTHANDLER_FREEORDER

            }

        // No tag had any text.
        if ( iContactQueue.Count() == 0 )
            {
            RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesL. No tag had any text, return" );
            DoAddNamesCompletedL();
            }
        else 
            {
            // At least some tag has some text
            RUBY_ASSERT_DEBUG( iTagArray.Count() == 0, User::Panic( KFile, __LINE__ ) );

            for ( k = 0; k < iContactQueue.Count(); k++ )
                {
                // (1) Create a new tag
                if ( iNewTag != NULL ) 
                    {
                    // If previous call to this function leaves, iNewTag can 
                    // be not deleted
                    delete iNewTag;
                    iNewTag = NULL;
                    }
                iNewTag = iTagManager->CreateTagL( iContext );

                MNssRRD* rrd = iNewTag->RRD();
                MNssSpeechItem* speechItem = iNewTag->SpeechItem();

                // (2) Fill RRD. RRD contains client-chosen data about the tag.
                //     In case of name dialing, RRD contains the contact ID,
                //     which is used later to get the phone number.
                CArrayFixFlat<TInt> *id = new (ELeave) CArrayFixFlat<TInt>( 1 );

                CleanupStack::PushL( id );

                id->AppendL( iContactQueue[k].iID ); // ID
                // select a number field for voice tag
                // TInt fieldIndex( 0 );

        		WaitUntilNoSyncBackupIsRunning();
                TRAPD( err, ReadContactL( iContactQueue[k].iID ); );
                if ( err == KErrNone )
                    {
                    // 1. Default voice call number set by the user
                    TInt found = KErrNotFound;
                    TRAP( found, iPbkHandler->FindDefaultContactFieldL( EDefaultCommand ) );
                    
                    if ( found == KErrNotFound )
                        {
                        for( TInt i = 0; i < KTaggableFieldsCount; i++) 
                            {
                            TRAP( found, iPbkHandler->FindFieldL( KTaggableFields[i] ) );
                            if( found == KErrNone && ( !iPbkHandler->IsFieldEmptyL() ) )
                                {
                                break;  // found field
                                }
                            else 
                                {
                                // When field->IsEmptyOrAllSpaces(), it is the same for us as no field present
                                found = KErrNotFound;
                                }
                            }  // for i < KTaggableFieldsCount
                        }

                    if ( found == KErrNone ) // found and is not empty
                        {
                        id->AppendL( iPbkHandler->FieldIdL() ); // field ID to array
                        id->AppendL( EDial );
                        id->AppendL( iContactQueue[k].iType );
                        id->AppendL( EDefaultCommand );
                        rrd->SetIntArrayL ( id );
                                                
                        // (3) Set the training text.
                        speechItem = iNewTag->SpeechItem();
                        speechItem->SetTextL( *iContactQueue[k].iTitle );
                 
                        User::LeaveIfError( iTagArray.Append( iNewTag ) );
                        // Leave protection for iNewTag completed
                        // Ownership and deletion responsibility moved
                        // to iTagArray
                        iNewTag = NULL;
                        }
                    else
                        {
                        // field not found or is empty
                        RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesL Did NOT identify a suitable voice dial field for contact id [%d]", iContactQueue[k].iID);
                        
                        
                        delete iNewTag;
                        iNewTag = NULL;
                        }  // if field else
#ifdef __SIND_EXTENSIONS
                    
                    // this part creates separate voice tags for extensions
                    for ( TInt i(0); i < iExtensionList.Count(); i++ )
                        {
                        found = KErrNotFound;
                        TVasExtensionAction action = iExtensionList[i].iAction;
                        TPbkFieldId fieldId = iExtensionList[i].iId;
                        
                        // default message number
                        // default video
                        // default e-mail
                        // default VOIP  
                        if ( action == ENewMessage ||
                             iExtensionList[i].iId == EPbkFieldIdPhoneNumberVideo ||
                             iExtensionList[i].iId == EPbkFieldIdEmailAddress ||
                             iExtensionList[i].iId == EPbkFieldIdVOIP )
                            {
                            // @todo Should use DefaultMessageField or something similar!!
                            TRAP( found,
                                iPbkHandler->FindDefaultContactFieldL( iExtensionList[i].iCommand ) );
                            }
                            
                        /*if ( extensionField == NULL )
                            {
                            extensionField = item->FindField( iExtensionList[i].iId );
                            }
                            
                        if ( extensionField == NULL && field != NULL 
                             && ( action == ENewMessage ) )
                            {
                            // if default neither default field or mobile field was not found
                            // message use normal SIND field
                            extensionField = field;
                            fieldId = field->FieldInfo().FieldId();
                            }*/
                            
                        // If field was not found, try to find it based on the priority lists
                        if ( found == KErrNotFound )
                            {
                            found = FindExtensionField( action, iExtensionList[i].iId );
                            // If field was found, check the field ID
                            if ( found == KErrNone )
                                {
                                fieldId = iPbkHandler->FieldIdL();
                                }
                            }
                            
                        if ( found == KErrNone )
                            {
                            // create new tag
                            iNewTag = iTagManager->CreateTagL( iContext );

                            rrd = iNewTag->RRD();
                            speechItem = iNewTag->SpeechItem();
                            
                            // set RRD data
                            CArrayFixFlat<TInt> *extIds = new (ELeave) CArrayFixFlat<TInt>( 1 );
                            CleanupStack::PushL( extIds );

                            extIds->AppendL( iContactQueue[k].iID ); 
                            if ( iExtensionList[i].iCommand == EMessageCommand ||
                                  iExtensionList[i].iCommand == EVideoCommand )
                                {
                                TInt id = iPbkHandler->FieldIdL();
                                if ( id )
                                    {
                                    extIds->AppendL( id );
                                    }
                                else
                                    {
                                    extIds->AppendL( fieldId );
                                    }
                                }
                            else
                                {
                                extIds->AppendL( fieldId );
                                }
                            extIds->AppendL( action );
                            extIds->AppendL( iContactQueue[k].iType );
                            extIds->AppendL( iExtensionList[i].iCommand );

                            rrd->SetIntArrayL ( extIds );
                                                
                            // set the training text.
                            TPtr namePtr( iContactQueue[k].iTitle->Des() );
                            TPtr extensionPtr( iExtensionList[i].iText->Des() );
                            HBufC* tempTitle = HBufC::NewLC( namePtr.Length() + extensionPtr.Length() + KTrainingIndexSize );
                            TPtr titlePtr( tempTitle->Des() );                                                           
                            
                            //not work: titlePtr.AppendFormat( extensionPtr, namePtr ); 
                            
                            titlePtr = extensionPtr;
                            AppendExtensionMarker( titlePtr );
                            TInt index = titlePtr.Find( KExtensionFormatString );
                            if ( index >= 0 )
                                {
                                // replace %U with name
                                titlePtr.Delete( index, 2 ); // delete %U
                                titlePtr.Insert( index, namePtr ); 
                                }
                            speechItem->SetTextL( titlePtr );
                        
                            User::LeaveIfError( iTagArray.Append( iNewTag ) );
                            iNewTag = NULL; //ownership changed
                            CleanupStack::PopAndDestroy( tempTitle );  
                            CleanupStack::PopAndDestroy( extIds );  
                            }
                        }

#endif // __SIND_EXTENSIONS                       
                    // delete id
                    CleanupStack::PopAndDestroy( id );
                    }
                else 
                    {
                    // Opening contact failed. E.g. if it was just deleted by user	
                    RUBY_DEBUG2( "CNssContactHandlerImplementation::DoAddNamesL Failed to open contact id [%d]. Leave [%d]", iContactQueue[k].iID, err);
                    CleanupStack::PopAndDestroy( id );
                    delete iNewTag;
                    iNewTag = NULL;
                    /** @todo Try reengineering duplicated cleanups into single piece 
                    of the code */
                    }
                }  // for k .. iContactQueue

            if ( iTagArray.Count() == 0 )
                {
                // If there are events on the list, DoAddNamesCompleted will fire them
                DoAddNamesCompletedL();  
                RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesL. Returned from DoAddNamesCompleted. Setting iState = ECHIdle" );
        		SetState ( ECHIdle );
                }
            else 
                {
                RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesL Before training tags. TagCount = [%d]",
        	            iTagArray.Count() );

                iTrainCallbackCounter = 0;
                for ( k = 0; k < iTagArray.Count(); k++ )
                    {
                    MNssSpeechItem* speechItem = iTagArray[k]->SpeechItem();

                    MNssSpeechItem::TNssSpeechItemResult trainRet 
                    = speechItem->TrainTextL( this, iTrainingParams );

                    if ( trainRet != MNssSpeechItem::EVasErrorNone )
                        {
                        RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesL : TrainTextL call failed." );
                        // Not much we can do except for timed retry.
                        // This stops the contact handler.
                        DoHandleEventFailedL();
                        break;  // no processing anymore. Quit function
                        }
                    }  // for k < iTagArray.Count()
                }  // if iTagArrayCount != 0
            }
        }  // if ( iAddList.Count() != 0 )
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoAddNamesTrainingFinished
// Called after training of a current pack of tags has finished. 
// Starts saving tags.
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoAddNamesTrainingFinished()
    {
    iSaveCallbackCounter = 0;
    iRetry = 0; // retry, if save tag fails
    iNormalChangesError = 0; // to record failures in save tag

    DoAddNamesSaveTag();
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoAddNamesSaveTag
// This function has two branches:
// (1) Initiate saving tags
// (1) After all tags have been saved, go to the next phase.
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoAddNamesSaveTag()
    {
    RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesSaveTag" );
    TInt retCode;

    // If all tags have been saved, go to the next phase.
    if ( iSaveCallbackCounter == iTagArray.Count() )
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesSaveTag All tags have been saved, go to the next phase" );
        // Tags saved - go to the next phase
        if ( iNormalChangesError == KErrNone )
            {
            // This function is called once per 100 contacts -> It is ok to use TRAP
            TRAPD( error, DoAddNamesCompletedL() );
            if( error != KErrNone ) 
                {
                /** @todo Test this branch */
                // Some fatal error occured
                RUBY_ERROR1( "DoAddNamesSaveTag DoAddNamesCompletedL failed with [%d]. Disabling SIND",
                            error );
                DisableEventHandling();
                }
            }
        else
            {
            RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesSaveTag Saving tags failed. Try again a few times" );
            // Saving tags failed. Try again a few times, and if it still
            // doesn't work, go to fatal error handler.
            if ( iRetry < KVASCHMaxRetry )
                {
                iRetry++;
                RUBY_DEBUG2( "CNssContactHandlerImplementation::DoAddNamesSaveTag Retried saving [%d] times. [%d] retries left",
                            iRetry, KVASCHMaxRetry - iRetry);
                iSaveCallbackCounter = 0;
                iNormalChangesError = KErrNone;
                
                DoAddNamesSaveTag();
                }
            else
                {
                RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesSaveTag Retried saving [%d] times. Still saving fails. Handling error", iRetry );
                NormalChangesHandleError();
                }
            }
        }  // if ( iSaveCallbackCounter == iTagArray.Count() )
    
    // If this is the first call to this function,
    // make "save tag" calls. After that, wait for the callbacks.
    else if ( iSaveCallbackCounter == 0 )
        {
        RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesSaveTag iSaveCallbackCounter == 0. Run SaveTag for [%d] contacts",
                    iTagArray.Count());
        for ( TInt k( 0 ); k < iTagArray.Count(); k++ )
            {
            retCode = iTagManager->SaveTag( this, iTagArray[k] );

            if ( retCode != KErrNone )
                {
                RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesSaveTag - SaveTag failed." );
				TRAPD( err, DoHandleEventFailedL(); );
				if( err != KErrNone ) 
					{
					RUBY_ERROR1( "CNssContactHandlerImplementation::DoAddNamesSaveTag handling error Left with [%d]", err );
					}
                return;
                }  // if retCode
            }  // for iTagArray
        }  // else if iSaveCallbackCounter == 0
    else 
    	{
    	// iSaveCallbackCounter != 0, but different from iTagArray.Count()
    	// This means, that SaveTag callbacks are still coming. We will wait for them
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesSaveTag. iSaveCallbackCounter != 0, but different from iTagArray.Count()" );
        }
    } 

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoAddNamesSaveTagCompleted
// Saving single tag has completed. 
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoAddNamesSaveTagCompleted( TInt aError )
    {
    iSaveCallbackCounter++;
    RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesSaveTagCompleted iSaveCallbackCounter became [%d]",
                iSaveCallbackCounter);
	// |= to let single KErrGeneral fail any number of KErrNone
    iNormalChangesError |= aError;

    // Check if all tags have been saved.
    DoAddNamesSaveTag();
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoAddNamesCompleted
// When adding names has successfully finished, clean up.
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DoAddNamesCompletedL()
    {
    RUBY_DEBUG2( "CNssContactHandlerImplementation::DoAddNamesCompleted. iDeleteCallbackCounter [%d], Count() [%d]", iDeleteCallbackCounter, iDeleteTagListArray->Count() );
	TInt initialDeleteTagListCount = iDeleteTagListArray->Count();    
    
    iSaveCallbackCounter = 0;
    iTrainCallbackCounter = 0;
    
    // DoAddNamesCompleted if often called after voice tags have been added =>
    // Nothing has to be done at all
    TBool changedDatabase = EFalse;  

#ifndef __SIND_EXTENSIONS
// Set VAS_SETS_VOICETAG on if old behaviour is needed.
// That is, VAS sets on the voice tag field to Phonebook engine and Phonebook UI checks the contacts DB for voice tags.
// Off means that VAS does not set voice tag field but instead Phonebook asks from VAS.
#ifdef VAS_SETS_VOICETAG
    // add voice tag fields
    // iTagArray contains only successfully trained tags.
    // Failed items are removed.
    for ( TInt i( 0 ); i < iTagArray.Count(); i++ )
        {
        MNssRRD* rrd = iTagArray[i]->RRD();
        CArrayFixFlat<TInt>* intArray = rrd->IntArray();
        RUBY_ASSERT_DEBUG( intArray->Count() >= 2, User::Panic( KFile, __LINE__ ) );
        if ( intArray->Count() >= 2 )
            {
            TBool committed( EFalse ); // ETrue if contact is committed already
            TInt contactId( intArray->At( 0 ) );
            RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesCompleted. Working on contact id [%d]",
                        contactId);
            TInt fieldId( intArray->At( 1 ) );

            WaitUntilNoSyncBackupIsRunning();
            // Open contact item based on id
            TRAPD( errFor,
                OpenContactL( contactId );
                
                RUBY_DEBUG1( "DoAddNamesCompleted Opened contact [%d]", contactId );
    			TInt found == KErrNotFound;
    			TRAP( found, iPbkHandler->FindDefaultContactFieldL( EDefaultCommand ) );
                if ( found == KErrNotFound )
                    {
                    TRAP( found, iPbkHandler->FindFieldL( fieldId ) );
                    }
    					// field - field, that SHOULD have voice tag attached
                        	
                if ( found == KErrNone )
                    {
                    TRAP( error, iPbkHandler->ChangeVoiceTagFieldL( ETrue ) );
                    if ( error == KErrNone )
                        {
                        RUBY_DEBUG1("DoAddNamesCompleted  Successfully added icon to the contact [%d]", 
                                    contactId);
                        iPbkHandler->CloseContactL( ETrue );
                        RUBY_DEBUG1("DoAddNamesCompleted  Successfully committed contact [%d]", 
                                    contactId);
                        iVoiceTagAddedList.Append( contactId );
                        changedDatabase = ETrue;
                        committed = ETrue;
                        }
                    }
                else
                    {
                    // field not found anymore
                    RUBY_DEBUG2("DoAddNamesCompleted Field [%d] not found in (already deleted from?) contact [%d]",
                                fieldId, contactId );
                    iDeleteTagListArray->AppendL( NULL ); // One DeleteTag will generate one callback
                    iTagManager->DeleteTag( this, iTagArray[i] );
                    }
                if ( !committed )
                    {
                    RUBY_DEBUG1("DoAddNamesCompleted Did not change contact [%d]. Closing without committing", 
                       	            contactId);
                    iPbkHandler->CloseContactL( EFalse );
                    }
            );  // TRAPD errFor
            if ( errFor != KErrNone ) 
                {
                RUBY_DEBUG2("CNssContactHandlerImplementation::DoAddNamesCompleted Failed to add icon for contact [%d]. Error [%d]",
                            contactId, errFor);
                    // voice tag was not in the contacts database, delete it from VAS
                    // One DeleteTag will generate one callback
                iDeleteTagListArray->AppendL( NULL );
                iTagManager->DeleteTag( this, iTagArray[i] );
                }
            }  // if intArray->Count() >= 2
        }  // for  iTagArray
#endif // VAS_SETS_VOICETAG
#endif //__SIND_EXTENSIONS        

    iTagArray.ResetAndDestroy();

    iAddList.Close();
    
#ifndef __SIND_EXTENSIONS
#ifdef VAS_SETS_VOICETAG
    // Remove voice tag icons for contacts, that failed to be trained or saved
    for( TInt l = 0 ; l < iRemoveIconList.Count(); l++ ) 
        {
        WaitUntilNoSyncBackupIsRunning();
        TRAPD( error, OpenContactL( iRemoveIconList[l] ) );
        if ( error != KErrNone ) 
            {
            RUBY_DEBUG2( "CNssContactHandlerImplementation::DoAddNamesCompleted Failed to open contact [%d]. Error [%d]", iRemoveIconList[l], error);
            }
        else 
            {           
            TRAP( error, iPbkHandler->ChangeVoiceTagFieldL( EFalse ) );
            if ( error == KErrNotFound )
                {
                // Nothing to remove. Already no voice tag icon
                TRAP( error, iPbkHandler->CloseContactL( EFalse ) );
                if ( error != KErrNone ) 
                    {
                    RUBY_ERROR2( "CNssContactHandlerImplementation::DoAddNamesCompleted Failed to close contact [%d]. Error [%d]", iRemoveIconList[l], error);
                    }
                }
            else if ( error != KErrNone )
                {
                RUBY_ERROR2( "CNssContactHandlerImplementation::DoAddNamesCompleted  Failed to remove voice tag icon for contact [%d]. Error [%d]", iRemoveIconList[l], error);
                TRAP( error, iPbkHandler->CloseContactL( EFalse ) );
                if ( error != KErrNone ) 
                    {
                    RUBY_ERROR2( "CNssContactHandlerImplementation::DoAddNamesCompleted Failed to close contact [%d]. Error [%d]", iRemoveIconList[l], error);
                    }
                }
            else
                {
                iVoiceTagAddedList.Append( iRemoveIconList[l] );
                TRAP( error, iPbkHandler->CloseContactL( ETrue ) );
                if ( error != KErrNone ) 
                    {
                    RUBY_ERROR2( "CNssContactHandlerImplementation::DoAddNamesCompleted Failed to commit contact [%d]. Error [%d]", iRemoveIconList[l], error);
                    }
                }            
            
            }  // if opened contact successfully
        
        }
#endif // VAS_SETS_VOICETAG
#endif //__SIND_EXTENSIONS
        
    iRemoveIconList.Close();

    for( TInt k( 0 ); k < iContactQueue.Count(); k++ )
        {
        delete iContactQueue[k].iTitle;
        iContactQueue[k].iTitle = NULL;
        }
    iContactQueue.Close();
    
    if( changedDatabase ) 
    	{
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesCompleted changed DBx" );
    	WaitUntilNoSyncBackupIsRunning();
	    TRAPD(err, iPbkHandler->CompressL() );
	    if ( err )
    		{
    		RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesCompleted Contacts DB compression failed. Error [%d]", 
    		            err);
    		}
    	}
    else 
    	{
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesCompleted did NOT change DB" );
    	}
    	
    	// full resyncs are always tag additions
    if( initialDeleteTagListCount == iDeleteTagListArray->Count() ) 
    	{
    	SetState( ECHIdle );
    	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesCompleted Syncing with phonebook generated NO deletions" );
    	// then full resync will not be initiated
    	iConsecutiveErrorFullResync = 0;
	    if ( iEventArray.Count() > 0 )
        	{
    		RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesCompleted There are still [%d] events in the queue. Call DoHandleEvents for them",
    		            iEventArray.Count() );
        	TRAPD( err, DoHandleEventsL() );
        	if ( err != KErrNone )
	            {
	            RUBY_ERROR1( "CNssContactHandlerImplementation::DoAddNamesCompleted. Error [%d] in DoHandleEventsL", err );
            	}
        	}
    	else 
       		{
       		// No self-generated events, and no pending events to handle
       		// This is the only place in the code, where everything is completed 
       		// and nothing is pending
       		RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesCompleted All the updates completed. No DB event is pending" );
       		
       		}
    	} 
    else 
    	{
    	RUBY_DEBUG1( "CNssContactHandlerImplementation::DoAddNamesCompleted Generated [%d] deletions. Starting to wait for callbacks", 
    		iDeleteTagListArray->Count() - initialDeleteTagListCount);
    	}
       	
    RUBY_DEBUG0( "CNssContactHandlerImplementation::DoAddNamesCompleted completed" );
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::NormalChangesHandleError
// Handle errors which happend when updating normal changes
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::NormalChangesHandleError()
    {
    // We can't know, what went wrong.
    // Stop contact handler, so that the next time the phone is booted
    // full synchronization happens.
    RUBY_DEBUG0( "CNssContactHandlerImplementation::NormalChangesHandleError " );
    TRAPD( err, DoHandleEventFailedL(); );
    if( err != KErrNone ) 
        {
        RUBY_ERROR1( "CNssContactHandlerImplementation::NormalChangesHandleError DoHandleEventFailedL Left with [%d]", err );
        }
    }

//---------------------------------------------------------
// CNssContactHandlerImplementation::HandleTrainComplete
// Saves the tag after successful training.
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::HandleTrainComplete( TInt aErrorCode )
    {
    if( aErrorCode == KErrNone )
        {
        RUBY_DEBUG1( "Training OK for %d:th name.", iTrainCallbackCounter );

        RUBY_DEBUG1( "CNssContactHandlerImplementation::HandleTrainComplete. iState [%d]", iState);
        RUBY_ASSERT_DEBUG( iState == ECHInitializing || 
                        iState == ECHHandlingNormalChanges,
                        User::Panic( KFile, __LINE__ ) );

        iTrainCallbackCounter++;

        if ( iTrainCallbackCounter == iTagArray.Count() )
            {
            if ( iState != ECHInitializing )
                {
                // assert at the beginning of the function guarantees, 
                // that iState == ECHHandlingNormalChanges 
                DoAddNamesTrainingFinished();
                }
            }        
        }
    
    else // aErrorCode != KErrNone
        {
        RUBY_DEBUG1( "CNssContactHandlerImplementation::HandleTrainComplete. Training failed for %d:th name.\n",
                    iTrainCallbackCounter );

        RUBY_ASSERT_DEBUG( iState == ECHInitializing || 
                        iState == ECHHandlingNormalChanges,
                        User::Panic( KFile, __LINE__ ) );

    	// Order removal of voice tag icon if present
    	MNssRRD* rrd = iTagArray[iTrainCallbackCounter]->RRD();
        CArrayFixFlat<TInt>* intArray = rrd->IntArray();
        RUBY_ASSERT_DEBUG( intArray->Count() >= 2, User::Panic( KFile, __LINE__ ) );
        if ( intArray->Count() >= 2 )
        	{
        	TInt error = iRemoveIconList.Append( intArray->At(0) );
        	if( error != KErrNone ) 
        		{
        		RUBY_ERROR2( "CNssContactHandlerImplementation::HandleTrainError Failed to insert [%d] into iRemoveIconList. Error [%d]",
        					intArray->At(0), error);
        		}
            }
      	else
            {
            RUBY_ERROR2( "CNssContactHandlerImplementation::HandleTrainComplete rrd->IntArray()->Count [%d] for iTrainCallbackCounter [%d]", 
            			intArray->Count(), iTrainCallbackCounter);   	
            }
                

        delete iTagArray[ iTrainCallbackCounter ];
        iTagArray.Remove( iTrainCallbackCounter );

        if ( iTrainCallbackCounter == iTagArray.Count() )
            {
            if ( iState == ECHHandlingNormalChanges )
                {
                DoAddNamesTrainingFinished();
                }
            else 
            	{
            	RUBY_ERROR1( "CNssContactHandlerImplementation::HandleTrainComplete Unexpected state [%d]", iState);
            	}
            }        
        }
    }

//---------------------------------------------------------
// CNssContactHandlerImplementation::HandleTrainError
// Handles single tag training failure
//---------------------------------------------------------
//
/*void CNssContactHandlerImplementation::HandleTrainError( enum MNssTrainTextEventHandler::TNssTrainResult )
    {
    RUBY_DEBUG1( "CNssContactHandlerImplementation::HandleTrainError Training failed for %d:th name.\n",
                iTrainCallbackCounter );

    RUBY_ASSERT_DEBUG( iState == ECHInitializing || 
                    iState == ECHHandlingNormalChanges,
                    User::Panic( KFile, __LINE__ ) );

	// Order removal of voice tag icon if present
	MNssRRD* rrd = iTagArray[iTrainCallbackCounter]->RRD();
    CArrayFixFlat<TInt>* intArray = rrd->IntArray();
    RUBY_ASSERT_DEBUG( intArray->Count() >= 2, User::Panic( KFile, __LINE__ ) );
    if ( intArray->Count() >= 2 )
    	{
    	TInt error = iRemoveIconList.Append( intArray->At(0) );
    	if( error != KErrNone ) 
    		{
    		RUBY_ERROR2( "CNssContactHandlerImplementation::HandleTrainError Failed to insert [%d] into iRemoveIconList. Error [%d]",
    					intArray->At(0), error);
    		}
        }
  	else
        {
        RUBY_ERROR2( "CNssContactHandlerImplementation::HandleTrainError rrd->IntArray()->Count [%d] for iTrainCallbackCounter [%d]", 
        			intArray->Count(), iTrainCallbackCounter);   	
        }
            

    delete iTagArray[ iTrainCallbackCounter ];
    iTagArray.Remove( iTrainCallbackCounter );

    if ( iTrainCallbackCounter == iTagArray.Count() )
        {
        if ( iState == ECHHandlingNormalChanges )
            {
            DoAddNamesTrainingFinished();
            }
        else 
        	{
        	RUBY_ERROR1( "CNssContactHandlerImplementation::HandleTrainError Unexpected state [%d]", iState);
        	}
        }
    }*/

//---------------------------------------------------------
// CNssContactHandlerImplementation::FullResyncStart
// Starts a full synchronization
//---------------------------------------------------------
//
void CNssContactHandlerImplementation::FullResyncStart()
    {
    // If we have synchronized earlier, we must delete the old context.
    if ( iHaveWeEverSynced )
        {
        iContextManager->DeleteContext( this, iContext );
        }
    else
        {
        TRAPD( err, FullResyncCreateContextL() );
        if ( err != KErrNone )
            {
            RUBY_ERROR1( "CNssContactHandlerImplementation::FullResyncStart. Error [%d] in FullResyncCreateContextL", err );
            }
        }
    }

// ------------------------------------------------------------
// CNssContactHandlerImplementation::FullResyncCreateContextL
// Full resync: Create a new context
// (prev: Destroy old context, next:populate the context with tags)
// ------------------------------------------------------------
//
void CNssContactHandlerImplementation::FullResyncCreateContextL()
    {
    RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::FullResyncCreateContextL" );
    if ( iContext != 0 )
        {
        delete iContext;
        iContext = 0;
        }

    iContext = iContextManager->CreateContextL();
    iContext->SetNameL( KNssCHNameDialContext );
    iContext->SetGlobal( ETrue );

    TBuf8<KClientDataSize> clientDataBuf;
    iClientData.iLanguage = User::Language();
    iClientData.iContactSyncId = KErrNotFound;
    	
    TRAPD( error, iClientData.ExternalizeL( clientDataBuf ) );
    if ( !error )
        {
        iContext->SetClientData( clientDataBuf );
        iContextManager->SaveContext( this, iContext );
        }
    }

// ------------------------------------------------------------
// CNssContactHandlerImplementation::FullResyncCreateChanges
// Create tags and send training requests to VAS.
// VAS trains them and calls back when it's ready
// ------------------------------------------------------------
//
void CNssContactHandlerImplementation::FullResyncCreateChangesL()
    {
	RUBY_DEBUG_BLOCK( "CNssContactHandlerImplementation::FullResyncCreateChangesL" );

    WaitUntilNoSyncBackupIsRunning();

    CContactIdArray* ids = iPbkHandler->ContactIdArrayLC();
	
	RUBY_DEBUG0( "CNssContactHandlerImplementation::FullResyncCreateChanges. iContactQueue.Count() == 0" );
    RUBY_ASSERT_DEBUG( iContactQueue.Count() == 0, User::Panic( KFile, __LINE__ ) );
    // Create change events for training
    for ( TInt k( 0 ); k < ids->Count(); k++ )
        {
        TPhonebookEvent event;
        event.iContactId = (*ids)[k];
        event.iType = EContactAdded;
        iEventArray.Append( event );
        }

    CleanupStack::PopAndDestroy( ids ); // Cleanup stack: empty

    // go to processing events
    SetState ( ECHIdle );
    TRAPD( err, DoHandleEventsL() );
    if ( err != KErrNone )
        {
        RUBY_ERROR1( "CNssContactHandlerImplementation::FullResyncCreateChanges. Error [%d] in DoHandleEventsL", err );
        }
    }

// ------------------------------------------------------------    
// CNssContactHandlerImplementation::DeleteContextCompleted
// DeleteContext callback. Deleting a context is part of
// full synchronization.
// ------------------------------------------------------------
//
void CNssContactHandlerImplementation::DeleteContextCompleted( TInt aErrorCode )
    {
    if( aErrorCode == KErrNone )
        {
        // Go to the next phase of full synchronization
        RUBY_DEBUG1( "CNssContactHandlerImplementation::DeleteContextCompleted. iState == ECHFullResync, iState [%d]", iState );
        RUBY_ASSERT_DEBUG( iState == ECHFullResync, User::Panic( KFile, __LINE__ ) );
        TRAPD( err, FullResyncCreateContextL() );
        if ( err != KErrNone )
            {
            RUBY_ERROR1( "CNssContactHandlerImplementation::DeleteContextCompleted. Error [%d] in FullResyncCreateContextL", err );
            }        
        }
    else
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::DeleteContextCompleted failed" );
        RUBY_ASSERT_DEBUG( iState == ECHFullResync, User::Panic( KFile, __LINE__ ) );

        RUBY_DEBUG0( "CNssContactHandlerImplementation::DeleteContextCompleted - can't handle, stopping." );
    	TRAPD( err, DoHandleEventFailedL(); );
    	if( err != KErrNone ) 
    		{
    		RUBY_ERROR1( "CNssContactHandlerImplementation::DeleteContextCompleted Left with [%d]", err);
    		}        
        }
    }

// ------------------------------------------------------------
// CNssContactHandlerImplementation::CausedByVoiceTagAddition
// Check if the event was caused by voice tag field adding
// ------------------------------------------------------------
//
TBool CNssContactHandlerImplementation::CausedByVoiceTagAddition( const TPhonebookEvent& aEvent )
	{
	TBool isVoiceTagAddedEvent (EFalse);
	if( EContactChanged == aEvent.iType )
		{
		for ( TInt i = 0; i < iVoiceTagAddedList.Count(); i++ )
        	{
        	if ( aEvent.iContactId == iVoiceTagAddedList[i] )
        		{
            	isVoiceTagAddedEvent = ETrue;
            	iVoiceTagAddedList.Remove( i-- );  // "i--" because of removal
            	// Continue iterations. We are not sure if voice tag for the given 
            	// contact could be changed twice
            	}
        	}  // for
		}  // if event type
	return isVoiceTagAddedEvent;
	}

// ------------------------------------------------------------
// CNssContactHandlerImplementation::PeriodicCallback
// Static callback required by Symbian Timer services.
// ------------------------------------------------------------
//
TInt CNssContactHandlerImplementation::PeriodicCallback( TAny* pX )
	{
	((CNssContactHandlerImplementation*)pX)->DoPeriodicCallback();
	return KErrNone;  // useless for CPeriodic
	}
    
// ------------------------------------------------------------
// CNssContactHandlerImplementation::DoPeriodicCallback
// Periodically check if there are some events to process
// ------------------------------------------------------------
//
void CNssContactHandlerImplementation::DoPeriodicCallback()
	{
	RUBY_DEBUG0( "CNssContactHandlerImplementation::DoPeriodicCallback" );
	// Check is needed to eliminate unnesessary TRAP and 64 bit time operations
	// Note, that we are not checking for PeriodicAction(), but for iPeriodicHandleEventTimer == NULL
	// PeriodicAction() is a method for detection of start of the action,
	// while iPeriodicHandleEventTimer == NULL says if periodic action is being executed
	// right *now*
	
	if ( ( iState == ECHIdle ) && ( iEventArray.Count() > 0 ) && ( !PeriodicAction() ) ) 
		{
		RUBY_DEBUG1( "CNssContactHandlerImplementation::DoPeriodicCallback. iState is ECHIdle, there are [%d] events to handle and no periodic action in progress", iEventArray.Count());
		TRAPD( err, DoHandleEventsL());
        if ( err != KErrNone )
            {
            RUBY_ERROR1( "CNssContactHandlerImplementation::DoPeriodicCallback. Error [%d] in DoHandleEventsL", err );
            }
        }  // if there are some events
	else
		{
		RUBY_DEBUG0( "CNssContactHandlerImplementation::DoPeriodicCallback No events to handle"  );
		}
	}
	
// ---------------------------------------------------------
// CNssContactHandlerImplementation::PeriodicAction
// Detects periodic action.
// Checks if many events have been received "lately"
// ---------------------------------------------------------
//
TBool CNssContactHandlerImplementation::PeriodicAction()
    {
    // index of the first recorded call
    TInt checkedIndex = iEventMomentIndex + 1;
    checkedIndex = checkedIndex < KEventMomentsSize ? checkedIndex : 0;
    TTime& checkedTime = iEventMoments[checkedIndex];
    
    // Check if the first recorded call happened "lately" = within last 
    // KPeriodicActionInterval seconds
    TTime currentTime;
    currentTime.UniversalTime();
    TTimeIntervalMicroSeconds diff = currentTime.MicroSecondsFrom( checkedTime );
    TBool result = diff < iEndOfPeriodicActionInterval;
    if ( result == EFalse ) 
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::PeriodicAction. Periodic action completed." );
        RUBY_DEBUG2( "CNssContactHandlerImplementation::PeriodicAction. Periodic action completed. iEventMomentIndex [%d], checkedIndex [%d]", iEventMomentIndex, checkedIndex );
        RUBY_DEBUG1( "currentTime [%d]", I64INT(currentTime.Int64()));
        RUBY_DEBUG2( "CNssContactHandlerImplementation::PeriodicAction. diff [%d], iEndOfPeriodicActionInterval [%d]", I64INT(diff.Int64()), I64INT(iEndOfPeriodicActionInterval.Int64() ) );
        
        for( TInt i = 0; i < KEventMomentsSize; i++ ) 
            {
            RUBY_DEBUG2( "checkedIndex[%d] = [%d]", i, I64INT(iEventMoments[i].Int64() ) );
            }
        
        // Periodic action completed. Or hasn't even been started
        // Restore timeout values
        // iEndOfPeriodicActionInterval = KInitialPeriodicActionInterval;
        if ( iPeriodicHandleEventTimer != NULL ) 
            {
            // stop the timer, it was started, when periodic action was started
            RUBY_DEBUG0( "CNssContactHandlerImplementation::PeriodicAction Stopping the periodic action timer" );
            iPeriodicHandleEventTimer->Cancel();
            delete iPeriodicHandleEventTimer;
            iPeriodicHandleEventTimer = NULL;
            }
        }
    else 
        {
        // Periodic action detected
        // start the timer if it is not running yet
        if( iPeriodicHandleEventTimer == NULL ) 
            {
            TRAPD( err, iPeriodicHandleEventTimer = CPeriodic::NewL( EPriorityNormal ) );
            if( err != KErrNone ) 
                {
                // This is a bad situation, but not the fatal one.
                // If we cannot use timer to detect end of periodic action,
                // Let's disable periodic action detection at all
                RUBY_DEBUG1( "CNssContactHandlerImplementation::PeriodicAction. PeriodicTimer creation failed. Error [%d]. Disabling periodic actions", err);
                result = EFalse;
                }
            else 
                {  // Timer was created successfully
                RUBY_DEBUG0( "CNssContactHandlerImplementation::PeriodicAction Starting the periodic action timer" );
                iPeriodicHandleEventTimer->Start( KPeriodicTimerInterval, KPeriodicTimerInterval, TCallBack( PeriodicCallback, this ) );
                }
            }
        }
    return result;
    }
	
// ------------------------------------------------------------
// CNssContactHandlerImplementation::UpdatePeriodicActionVars
// Update variables, used to detect if periodic action is happening
// ------------------------------------------------------------
//
void CNssContactHandlerImplementation::UpdatePeriodicActionVars()
    {
    if ( ++iEventMomentIndex >= KEventMomentsSize )
        {
        iEventMomentIndex = 0;
        }
    iEventMoments[iEventMomentIndex].UniversalTime();
    
    // Update values used to detect end of periodic action
    TTimeIntervalMicroSeconds32 currInterval = MaximalInterval( iEventMoments, KEventMomentsSize, iEventMomentIndex );
    // 8 times. At the end of action, this would typically be no longer, than 0.5secs
    // Note, that this is the interval fo KEventMomentsSize events
    if ( ( currInterval.Int() << 3 ) >= ( KInitialPeriodicActionInterval ) ) 
        {
        RUBY_DEBUG1( "CNssContactHandlerImplementation::UpdatePeriodicActionVars. Calculated interval is too big. Reverting periodic interval to the max [%d] mcs", KInitialPeriodicActionInterval);
        iEndOfPeriodicActionInterval = KInitialPeriodicActionInterval;
        }
    else 
        {
        currInterval = currInterval.Int() << 3;  
        RUBY_DEBUG1( "CNssContactHandlerImplementation::UpdatePeriodicActionVars. Adjusting periodic interval to [%d] mcs", currInterval.Int() );
        iEndOfPeriodicActionInterval = currInterval.Int();
        }
    }

// ------------------------------------------------------------
// CNssContactHandlerImplementation::MaximalInterval
// Return maximal interval from the array of time moments
// ------------------------------------------------------------
//
TTimeIntervalMicroSeconds32 CNssContactHandlerImplementation::MaximalInterval( const TTime aMoments[], const TInt aMomentsLength, const TInt aLastMomentIndex )
    {
    RUBY_ASSERT_DEBUG( aMomentsLength > 1, User::Panic( KFile, __LINE__ ) );
    TInt currIndex( aLastMomentIndex );
    TInt prevIndex = currIndex - 1 < 0 ? aMomentsLength - 1 : currIndex - 1;
    TTimeIntervalMicroSeconds32 result( 0 );
    TTimeIntervalMicroSeconds currInterval;
    
    for ( TInt i = 0; i < aMomentsLength - 1; i++ ) 
        {
        currInterval = aMoments[currIndex].MicroSecondsFrom( aMoments[prevIndex] );
        const TInt64& currIntVal = currInterval.Int64();
        TBool tooBigValue = currIntVal > KMaxTInt;
        
        // if high != 0, aMoments were filled in the incorrect way
        // but we should not bug the user about it
        if ( tooBigValue ) 
            {
            // this cannot be a negative value.. if moments are recorded in the correct order.
            // Then non-zero high means, that interval is too big to fit into
            // TTimeIntervalMicroSeconds32
            RUBY_DEBUG0( "CNssContactHandlerImplementation::MaximalInterval. Maximal interval is out of bounds" );
            result = TTimeIntervalMicroSeconds32( KMaxTInt );
            break;
            }
        else 
            {
            // normal size value
            // casting is safe - we checked, that it fits into TInt
            TTimeIntervalMicroSeconds32 currInterval32 ( (TInt)currIntVal );
            
            if ( currInterval32 > result ) 
                {
                result = currInterval32;
                }
            }
        currIndex = currIndex - 1 < 0 ? aMomentsLength - 1 : currIndex - 1;
        prevIndex = currIndex - 1 < 0 ? aMomentsLength - 1 : currIndex - 1;
        }  // for
    return result;
    }

// ------------------------------------------------------------
// CNssContactHandlerImplementation::SetState
// Sets the contact handler state. 
// In debug, panics (via asserts) if state change is not legal
// ------------------------------------------------------------
//
void CNssContactHandlerImplementation::SetState( TContactHandlerState aState )
	{
    RUBY_DEBUG2( "CNssContactHandlerImplementation::SetState from [%d] to [%d]", iState, aState);
    switch( aState ) 
        {
        case ECHFullResync:
            // no pending events
            RUBY_ASSERT_DEBUG( ( iDeleteTagListArray == NULL ) || ( iDeleteCallbackCounter == iDeleteTagListArray->Count() ), User::Panic( KFile, __LINE__ ) );
            RUBY_ASSERT_DEBUG( iSaveCallbackCounter == iTagArray.Count(), User::Panic( KFile, __LINE__ ) );
            
            // full resyncronization needed again if this full resyncronization fails
            RUBY_TRAP_IGNORE( SetFullResyncCrFlagL( ETrue ) );
            break;
        case ECHHandlingNormalChanges:
            RUBY_ASSERT_DEBUG( iState == ECHIdle, User::Panic( KFile, __LINE__ ) );
            RUBY_TRAP_IGNORE( SetFullResyncCrFlagL( ETrue ) );
            break;
        case ECHIdle:
            if ( iDeleteTagListArray != NULL ) 
                {
                RUBY_ASSERT_DEBUG( (iSaveCallbackCounter == iTagArray.Count() ) || ( iDeleteCallbackCounter == iDeleteTagListArray->Count() ), User::Panic( KFile, __LINE__ ) );
                }
            else 
                {
                RUBY_ASSERT_DEBUG( ( iSaveCallbackCounter == iTagArray.Count() ) , User::Panic( KFile, __LINE__ ) );
                }
            if( iEventArray.Count() == 0 ) 
                {
                    // Idle state and no pending events mean, that contact handler
                    // completed its operations for now
                    TInt errProp = iBusyProperty.Set( EContactsNoTraining );
                    if ( errProp != KErrNone )
                        {
                        RUBY_ERROR1("CNssContactHandlerImplementation::ConstructL() Failed to set property. Error [%d]", 
                                    errProp );
                        // @todo Not a leaving function -> leave commented out
                        //User::Leave( errProp )  ;
                        }
                }
                
            // all changes done, no need for resyncronization
            RUBY_TRAP_IGNORE( SetFullResyncCrFlagL( EFalse ) );
            
            break;
        default:
            RUBY_ERROR0( "CNssContactHandlerImplementation::SetState Unexpected or unknown state" );
            RUBY_ASSERT_DEBUG( EFalse, User::Panic( KFile, __LINE__ ) );
            
            // failure, full resyncronization needed
            RUBY_TRAP_IGNORE( SetFullResyncCrFlagL( ETrue ) );
            break;
        }
    // actually switch state
    iState = aState;
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DataSyncStateChanged
//
// ---------------------------------------------------------
//
void CNssContactHandlerImplementation::DataSyncStateChanged( TBool aRunning )
    {
    if( !aRunning && ( iSyncBackupWaiter->IsStarted() ) )
        {
        RUBY_DEBUG0( "DataSyncStateChanged Sync became inactive and CH was waiting for it" );
        iSyncBackupWaiter->AsyncStop();
        }
    }
    
// ---------------------------------------------------------
//  Is called when backup/restore status is changed and when backup/restore
//  state observation was just started
//  @param aRunning ETrue if some backup/restore action is in progress
//                  EFalse otherwise
//  @param aRestoreType ETrue if the event is restore related
//                      EFalse if the event is backup related
//  @see CNssChBackupObserver
// ---------------------------------------------------------
void CNssContactHandlerImplementation::BackupRestoreStateChanged( TBool aRestoreType, 
                                                                  TBool aRunning )
	{
    RUBY_DEBUG2( "BackupRestoreStateChanged aRestoreType [%d], aRunning [%d]", 
                  aRestoreType, aRunning );
    
    if( aRestoreType && aRunning )
        {
        RUBY_DEBUG0( "Setting the Full Resync CR flag" );
        RUBY_TRAP_IGNORE( SetFullResyncCrFlagL( ETrue ) );
        }
    
    if( !aRunning && ( iSyncBackupWaiter->IsStarted() ) )
        {
        RUBY_DEBUG0( "BackupRestoreStateChanged b/r became inactive and CH was waiting for it" );
        iSyncBackupWaiter->AsyncStop();
        }	
	}

// ---------------------------------------------------------
// CNssContactHandlerImplementation::HandleNotifyInt
//
// ---------------------------------------------------------
// 	
void CNssContactHandlerImplementation::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    RUBY_DEBUG0( "" );
    
    if ( aId == KSRSFFullResyncNeeded )
        {
        if ( aNewValue == KImmediateResync )
            {
            RUBY_DEBUG0( "Immediate full resync requested" );
            
            // Write the previous value back to the cenrep
            TRAP_IGNORE( SetFullResyncCrFlagL( iResyncAtBoot ) );
            
            // Create an event that will launch full resync
            TPhonebookEvent event;
            event.iType = ENullEvent;
            event.iContactId = 0;
        
            iEventArray.Append( event );
            
            // Start resync if nothing is ongoing
            if ( iState == ECHIdle )
                {
                TRAP_IGNORE( DoHandleEventsL() );
                }
            }
        }
    }
    
// ---------------------------------------------------------
// Sets the central repository full resync flag
// ---------------------------------------------------------
void CNssContactHandlerImplementation::SetFullResyncCrFlagL( TBool aResyncNeeded )
    {
    RUBY_DEBUG_BLOCKL("");
    User::LeaveIfError( iRepository->Set( KSRSFFullResyncNeeded, aResyncNeeded ) );
    iResyncAtBoot = aResyncNeeded;
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::WaitUntilNoSyncBackupIsRunning
//
// ---------------------------------------------------------
//    
void CNssContactHandlerImplementation::WaitUntilNoSyncBackupIsRunning()
    {
    RUBY_DEBUG0( "WaitUntilNoSyncBackupIsRunning start" );
    RUBY_ASSERT_DEBUG( !iSyncBackupWaiter->IsStarted(), User::Panic( KFile, __LINE__ ) );
    
    // Disconnection from phonebook should happen only once and only if b/r is running
    TBool disconnectedFromPb = EFalse;
    
    // Check the values until both data sync and backup/restore are not active
    while( iDataSyncWatcher->InProgress() || iBackupObserver->InProgress() )
    	{
    	RUBY_DEBUG2( "WaitUntilNoSyncBackupIsRunning Have to wait. data sync [%d], backup/restore [%d]",
    				 iDataSyncWatcher->InProgress(), iBackupObserver->InProgress() );
    	if( !iSyncBackupWaiter->IsStarted() ) 
            {
            if( (!disconnectedFromPb) && iBackupObserver->InProgress() )
            	{
            	RUBY_DEBUG0( "WaitUntilNoSyncBackupIsRunning Disconnecting from phonebook" );
            	DisconnectFromPhonebook();
            	disconnectedFromPb = ETrue;
            	}
            iSyncBackupWaiter->Start();
            }
        else 
            {
            RUBY_ERROR0( "WaitUntilNoSyncBackupIsRunning Attempt to start already active iSyncBackupWaiter. Huge error!" );
            }
    	}
    	
    if( disconnectedFromPb )
    	{
    	RUBY_DEBUG0( "WaitUntilNoSyncBackupIsRunning Reconnecting to phonebook" );
    	TRAP_IGNORE( ConnectToPhonebookL() );
    	}
            
    RUBY_DEBUG0( "WaitUntilNoSyncBackupIsRunning end" );
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::GetContactL
//
// ---------------------------------------------------------
//        
void CNssContactHandlerImplementation::GetContactL( TContactItemId aContactId, TBool aReadContact ) 
    {
    RUBY_DEBUG1( "CNssContactHandlerImplementation::GetContactL. contact id [%d]", aContactId );
    TInt err = KErrNone;
    // Number of attempts tries
    TInt attemptsTried( 0 );
    do 
        {
        if( err == KErrInUse ) // i.e. not the first cycle iteration
            {
            RUBY_DEBUG2( "GetContactL Contact [%d] is locked. Waiting one more second. Retrial [%d]", 
                         aContactId, attemptsTried);
            // start periodic timer
            if( attemptsTried == 1)   // i.e exactly the second cycle iteration
                {
                RUBY_DEBUG0( "GetContactL Creating periodic timer" );
                iContactWaitTimer = CPeriodic::NewL( EPriorityNormal );
                iContactWaitTimer->Start( KContactWaitInterval, KContactWaitInterval, TCallBack( ContactWaitCallback, this ) );
                }
            RUBY_DEBUG0( "GetContactL Starting the inner cycle waiting" );
            iContactWaiter->Start( );
            RUBY_DEBUG0( "GetContactL Waiting completed, trying to open contact again" );
            }
            
        TRAP( err, iPbkHandler->FindContactL( aContactId, aReadContact ) );

        attemptsTried++;
        // -1 means forever
        if( ( KMaxContactLockRetrials != -1 ) && ( attemptsTried >= KMaxContactLockRetrials ) ) 
            {
            RUBY_DEBUG1( "GetContactL Tried for [%d] times, still no success", attemptsTried );
            break;
            }
        }
    while( err == KErrInUse );
    if( iContactWaitTimer != NULL ) 
        {
        iContactWaitTimer->Cancel();
        delete iContactWaitTimer;
        iContactWaitTimer = NULL;
        }
    // Propagate all the leaves, BUT KErrInUse
    if( ( err != KErrInUse ) && ( err != KErrNone ) )
        {
        User::LeaveIfError( err );
        }
    if( err == KErrInUse ) 
        {
        RUBY_DEBUG1( "GetContactL Failed to lock contact [%d]. Disabling CH, will resync on reboot", aContactId );
        DisableEventHandling();
        User::Leave( KErrInUse );
        }
    RUBY_DEBUG0( "CNssContactHandlerImplementation::GetContactL completed" );
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::ReadContactL
//
// ---------------------------------------------------------
//     
void CNssContactHandlerImplementation::ReadContactL( TContactItemId aContactId )
    {
    GetContactL( aContactId, ETrue );
    }
    
// ---------------------------------------------------------
// CNssContactHandlerImplementation::OpenContactL
//
// ---------------------------------------------------------
//     
void CNssContactHandlerImplementation::OpenContactL( TContactItemId aContactId )
    {
    GetContactL( aContactId, EFalse );
    }
    
// ---------------------------------------------------------
// CNssContactHandlerImplementation::ContactWaitCallback
//
// ---------------------------------------------------------
//     
TInt CNssContactHandlerImplementation::ContactWaitCallback( TAny* pX )
    {
	((CNssContactHandlerImplementation*)pX)->DoContactWaitCallback();
	return KErrNone;  // useless for CPeriodic    
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::DoContactWaitCallback
//
// ---------------------------------------------------------
//             
void CNssContactHandlerImplementation::DoContactWaitCallback()
    {
    iContactWaiter->AsyncStop();
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::NoDiskSpace
// Checks if there is not enough disk space to finish the
// operation. Returns ETrue if there is no space.
// ---------------------------------------------------------
//    
TBool CNssContactHandlerImplementation::NoDiskSpace()
    {
    RUBY_DEBUG0( "CNssContactHandlerImplementation::NoDiskSpace" );
    TBool diskSpace( EFalse );
    TRAP_IGNORE( diskSpace = SysUtil::FFSSpaceBelowCriticalLevelL( &iFSession, KIncreaseOfDatabaseSizes ) );
    
    if ( diskSpace )
        {
        RUBY_TRAP_IGNORE( SetFullResyncCrFlagL( ETrue ) );
        RUBY_DEBUG0( "CNssContactHandlerImplementation::NoDiskSpace NOT enough disk space available" );
        return ETrue;
        }
    else
        {
        RUBY_DEBUG0( "CNssContactHandlerImplementation::NoDiskSpace There is enough disk space" );
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::TrimName
// Substitutes separator characters in descriptor with
// white spaces. Removes extra withspaces. 
// ---------------------------------------------------------
//    
void CNssContactHandlerImplementation::TrimName( HBufC* aBuf )
    {
    TPtr ptr = aBuf->Des();
   
    for ( TInt findIndex = 0; findIndex < aBuf->Des().Length(); findIndex++ )
        {
        if ( ptr[findIndex] == iSeparator )
            {
            ptr[findIndex] = ' ';
            }
        }
       
    ptr.TrimAll();
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::AppendName
// Appends name to given buffer
// ---------------------------------------------------------
//    
void CNssContactHandlerImplementation::AppendName( HBufC* aBuf, HBufC* aName )
    {
    if ( aName->Length() != 0 )
        {     
        aBuf->Des().Append( iSeparator );
        aBuf->Des().Append( KNameTrainingIndex );
        aBuf->Des().Append( aName->Des() );
        }
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::AppendExtensionMarker
// Appends marker for the extensions
// Parameters should be in format "%U text message"
// In which case output is "%U_2text message"
// ---------------------------------------------------------
//    
void CNssContactHandlerImplementation::AppendExtensionMarker( TDes& aDes )
    {
    if ( aDes.Length() > 0 )
        {
        TInt insertPosition( 0 );
        if ( aDes.Find( KExtensionFormatString ) == 0 )
            {
            // Find the first space and insert marker to that position
            TInt firstSpace( 0 );
            firstSpace = aDes.Find( _L(" ") );
            if ( firstSpace == 2 )         // Next character after KExtensionFormatString
                {
                aDes.Delete( firstSpace, 1 );
                }
            insertPosition = 2;
            }
        // Insert marker
        aDes.Insert( insertPosition, KExtensionTrainingIndex );
        aDes.Insert( insertPosition, KNameSeparator );
        }
    }

/**
* Establishes the connection to the contact engine and 
* starts listening to it
*/ 
void CNssContactHandlerImplementation::ConnectToPhonebookL()
	{
	RUBY_ASSERT_DEBUG( (!iPbkHandler), User::Leave( KErrNotReady ) );
	iPbkHandler = CVasVPbkHandler::NewL();
	iPbkHandler->InitializeL();
	iPbkHandler->CreateContactObserverL( this );
	}

/** 
* Clears the connection to the contact engine
*/
void CNssContactHandlerImplementation::DisconnectFromPhonebook()
	{
	delete iPbkHandler;
	iPbkHandler = NULL;
	}
    
#ifdef __SIND_EXTENSIONS	

// ---------------------------------------------------------
// CNssContactHandlerImplementation::ReadExtensionsL
// Reads the extension resource file
// ---------------------------------------------------------
//           
void CNssContactHandlerImplementation::ReadExtensionsL()
    {
    // using StringLoader should be used, but there is no 
    // access to CCoeEnv
    

    // letters for drives in search order
    const TBuf<1> KResourceDrivers = _L("z"); 
    _LIT( KResourceFileName, "nssvascontacthandler.rsc" );
    
    // number of extension commands in the resource file
    const TInt KExtensionCommandCount = 8;
   
    // load resources
    RResourceFile resourceFile;
    RFs fs;
    CleanupClosePushL( fs );
    TBool found( EFalse );
    User::LeaveIfError( fs.Connect() );
   
        
    TFileName name;
    TInt i( 0 );
    // use the first drive
    name.Append( KResourceDrivers[i] );
    name.Append(':');
    name.Append( KDC_RESOURCE_FILES_DIR );
    name.Append( KResourceFileName );

    while ( !found && i < KResourceDrivers.Length() )
        {
        name[0] = KResourceDrivers[i++];

        BaflUtils::NearestLanguageFile( fs, name );
       
        if ( BaflUtils::FileExists(fs, name) )
            {
            // open resource
            resourceFile.OpenL( fs, name );
            CleanupClosePushL( resourceFile );
            resourceFile.ConfirmSignatureL();
            found = ETrue;
            }
        }

    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    
    for ( TInt extensionIndex( 0 ); extensionIndex < KExtensionCommandCount; extensionIndex++ )
        {
        TExtension extension;
        TResourceReader reader;
        TBool extentionSupported( ETrue );
        
        switch ( extensionIndex )
            {
            
            case 0:
            reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_MOBILE ) );
            extension.iId = EPbkFieldIdPhoneNumberMobile;     
            extension.iAction = EDial;
            extension.iCommand = EMobileCommand;
            break;
            
            case 1:
            reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_MESSAGE ) );
            extension.iId = EPbkFieldIdPhoneNumberMobile;     
            extension.iAction = ENewMessage;     
            extension.iCommand = EMessageCommand;
            break;
            
            case 2:
            reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_GENERAL ) );
            extension.iId = EPbkFieldIdPhoneNumberGeneral;     
            extension.iAction = EDial;
            extension.iCommand = EGeneralCommand;          
            break;
            
            case 3:
            reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_WORK ) );
            extension.iId = EPbkFieldIdPhoneNumberWork;     
            extension.iAction = EDial;
            extension.iCommand = EWorkCommand; 
            break;
            
            case 4:
            reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_HOME ) );
            extension.iId = EPbkFieldIdPhoneNumberHome;     
            extension.iAction = EDial;
            extension.iCommand = EHomeCommand;          
            break;
            
            case 5:
            if ( iVideoCallFeatureSupported )
                {  
                reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_VIDEO ) );
                extension.iId = EPbkFieldIdPhoneNumberVideo;     
                extension.iAction = EDial;
                extension.iCommand = EVideoCommand;                
                }
            else
                { 
                extentionSupported = EFalse;
                }
            break; 
                                                                       
            case 6:
            reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_EMAIL ) );
            extension.iId =EPbkFieldIdEmailAddress ;     
            extension.iAction = ENewEmail;  
            extension.iCommand = EEmailCommand;
            break;
            
            case 7:
            if ( iVoIPFeatureSupported )
                {                
                reader.SetBuffer( resourceFile.AllocReadLC( R_SIND_EXTENSION_VOIP ) );
                extension.iId = EPbkFieldIdVOIP;     
                extension.iAction = EDial;
                extension.iCommand = EVoipCommand; 
                }
            else
                {
                extentionSupported = EFalse;
                }
            break; 
            
            default:
            User::Leave( KErrGeneral );  // !!! @todo: PANIC                               
            }
        
        if ( extentionSupported )
            {           
            extension.iText = reader.ReadHBufCL();
            CleanupStack::PushL( extension.iText );    
            User::LeaveIfError( iExtensionList.Append( extension ) );
            CleanupStack::Pop( extension.iText );
            CleanupStack::PopAndDestroy(); // AllocReadLC
            }
        }

    CleanupStack::PopAndDestroy( &resourceFile );    
    CleanupStack::PopAndDestroy( &fs );

    }
	
// ---------------------------------------------------------
// CNssContactHandlerImplementation::FindExtensionField
// Finds the extension field based on priority lists
// ---------------------------------------------------------
//        	
TInt CNssContactHandlerImplementation::FindExtensionField( TVasExtensionAction aAction, 
                                                           TPbkFieldId aDefaultFieldId )
    {  
    TInt found = KErrNotFound;
    
    switch ( aDefaultFieldId )
        {
        case EPbkFieldIdPhoneNumberVideo:
            {
            // Use defaults for video extension
            found = ExtensionFieldFindLoop( KVideoExtensionFields, KVideoExtensionFieldsCount );
            break;
            }
        
        case EPbkFieldIdPhoneNumberMobile:
            {
            if ( aAction == ENewMessage )
                {
                // Use defaults for message extension
                found = ExtensionFieldFindLoop( KMessageExtensionFields, KMessageExtensionFieldsCount );  
                }
            else
                {
                TRAP( found, iPbkHandler->FindFieldL( aDefaultFieldId ) );
                }
                
            break;
            }
            
        case EPbkFieldIdVOIP:
            {
            // Use defaults for VOIP extension
            found = ExtensionFieldFindLoop( KVOIPExtensionFields, KVOIPExtensionFieldsCount );  
                
            break;
            }
        
        default:
            {
            TRAP( found, iPbkHandler->FindFieldL( aDefaultFieldId ) );
            break;
            }
        }
    return found;
    }

// ---------------------------------------------------------
// CNssContactHandlerImplementation::ExtensionFieldFindLoop
// Finds the extension from the prioroty list arrays
// ---------------------------------------------------------
//        	
TInt CNssContactHandlerImplementation::ExtensionFieldFindLoop( const TPbkFieldId* const aArray, 
                                                               TInt aCount )
    {
    TInt found = KErrNotFound;
       
    for ( TInt iCounter = 0; iCounter < aCount; iCounter++ )
        {
        TRAP( found, iPbkHandler->FindFieldL( aArray[iCounter] ) );
        if ( found == KErrNone )
            {
            break;
            }
        }
    return found;
    }

#endif // __SIND_EXTENSIONS
    	
//  End of File
