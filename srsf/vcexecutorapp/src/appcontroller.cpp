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
* Description:  This is an implementation class for checking which 
*               application or profile needs to be activated and then
*               starts it.
*
*/


// INCLUDE FILES
#include "appcontroller.h"
#include <vcexecutorapp.rsg>
#include "appcontroller.hrh"
#include "appcontrollerconstants.h"
#include <aknlists.h>
#include <aknPopup.h>
#include <apgtask.h>     //TApaTask, TApaTaskList
#include <apacmdln.h>    //CApaCommandLine
#include <w32std.h>      // RWsSession
#include <e32svr.h>
#include <StringLoader.h>
#include <f32file.h>
#include <badesca.h> 
#include <bautils.h>
#include <barsc.h>
#include <e32def.h>

#include <PSVariables.h>
#include "appcontrollerpropertyhandler.h"
#include <ctsydomainpskeys.h>

#include <startupdomainpskeys.h>

#include <centralrepository.h>


#include <CoreApplicationUIsSDKCRKeys.h>

#include <aknnotewrappers.h>
#include <featmgr.h>
#include <bldvariant.hrh>
#include <AknQueryDialog.h>

#include <sendui.h>     //CreateAndSendMessageL()
#include <TSendingCapabilities.h>
#include <SendUiConsts.h>

#include <MsgArrays.h>

#include <coemain.h>

// Profile Engine API
#include <Profile.hrh>

#include <AknNotifierController.h>
#include <AknGlobalConfirmationQuery.h> // Confirmation query.
#include "appcontrolleractiveobject.h"  // CAppControllerActiveObject.
#include <AknGlobalNote.h>    // Information note
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR

#include "rubydebug.h"


#include <aiwdialdata.h>
#include <AiwDialDataTypes.h>
#include <AiwServiceHandler.h>
#include <GSLauncher.h>


// Cover UI start
#include "secondarydisplay/appcontrollersecondarydisplayapi.h"
#include "SecondaryDisplay/SecondaryDisplaySysApAPI.h"
// Cover UI end

const TUid KAppManagerUid = { 0x10283321 }; // Control Panel Application Manager UID
const TUid KVoiceCommandUid = { 0x10282C40 }; // Control Panel Voice Command UID

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CVCApplicationControllerImpl::CVCApplicationControllerImpl() : iResourceFileOffset( 0 )
    {
    }

// Destructor
CVCApplicationControllerImpl::~CVCApplicationControllerImpl()
    {
    if( iApaLsSession )
        {
        iApaLsSession->Close();
        delete iApaLsSession;
        }
 

    delete iPropertyHandler;
    delete iMsgTypes;
    
    if (iSendUi)
        {
        delete iSendUi;    
        }
        
    // Delete active object.
    if ( iActiveObject )
        {
        iActiveObject->Cancel();
        delete iActiveObject;
        }

    if (iProfileEngine)
        {
        iProfileEngine->Release();
        iProfileEngine = NULL;
        }

    delete iGlobalConfQuery;
    
    delete iQueryDialog;
    
    if ( iCoeEnv && iEikonEnv->EikAppUi() )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack( this );
        }
    
    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::ConstructL
// Symbian 2nd phase constructor
// ---------------------------------------------------------
//
void CVCApplicationControllerImpl::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::ConstructL" );
    
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();

    iPropertyHandler = CAppControllerPropertyHandler::NewL(
        KPSUidCtsyCallInformation, KCTsyCallState, this);

    // Create active object to handle note request statuses.
    iActiveObject = new (ELeave) CAppControllerActiveObject( *this );

    iGlobalConfQuery = CAknGlobalConfirmationQuery::NewL();
    }

// ----------------------------------------------------------------
// CVCApplicationControllerImpl* CVCApplicationControllerImpl::NewL
// Two-phased constructor.
// ----------------------------------------------------------------
//
CVCApplicationControllerImpl* CVCApplicationControllerImpl::NewL()                                                           
    {
    CVCApplicationControllerImpl* self= new (ELeave) CVCApplicationControllerImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::ExecuteCommandL
// Checks the Id and diverts the application or profile
// to the right launching engine.
// ---------------------------------------------------------
//
TInt CVCApplicationControllerImpl::ExecuteCommandL( TUint aCommandId,
                                                    const TDesC& /*aParameters*/ )
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::ExecuteCommandL" );
    
    TInt err = KErrNone;

    // The method we're about to launch is an application, recognized by it's first name.
    if( aCommandId < KFirstProfileValue )
        {
        err = DoExecuteCommandL( aCommandId, KNullDesC() );
        }
    // The method we're about to launch is a profile, recognized by it's first name
    else if( ( aCommandId >= KFirstProfileValue) && ( aCommandId < KSecondProfileValue) )
        {
        aCommandId -= KProfileValue;
        ProfileEngL( aCommandId );
        }
    // The method we're about to launch is a profile, recognized by it's second name
    else if ( ( aCommandId >= KSecondProfileValue ) )
        {
        aCommandId -= KFirstProfileValue;
        ProfileEngL( aCommandId );
        }
    else
        {
        // Id does not match.
        RUBY_DEBUG1( "CVCApplicationControllerImpl::ExecuteCommandL: command id %d not found", aCommandId );
        User::Leave( KErrArgument );
        }

    return err;
    }
    
// ---------------------------------------------------------
// CVCApplicationControllerImpl::DoExecuteCommandL
// Checks the Id and diverts the application or profile
// to the right launching engine.
// ---------------------------------------------------------
//
TInt CVCApplicationControllerImpl::DoExecuteCommandL( TUint aCommandId, const TDesC& /*aParameters*/ )
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::DoExecuteCommandL" );
     
    RUBY_DEBUG1( "CVCApplicationControllerImpl::DoExecuteCommandL. executing command with id %d", aCommandId );

    iCallStatus = EFalse;
    TInt err = KErrNone;
    CGSLauncher* gsLauncher( NULL );
    TUid zeroUid = {0};
    // Check which app needs to be activated
    switch( aCommandId )
        {
        case EAppVoiceMail:

            err = VmbxEngL();
            break;
#ifdef __BT
        case EAppBluetooth:
            
            err = BTEngL();
            break;
#endif
        case EAppWriteMessage:
    
            ActivateMessageL( EEditorTypeNewMessage );
            break;
            
        case EAppWriteEmail:

            ActivateMessageL( EEditorTypeNewEmail );
            break;
            
        case EAppWritePostcard:
       
            ActivateMessageL( EEditorTypePostcard );
            break;
        
        case EAppApplicationManagerCPView:
            gsLauncher = CGSLauncher::NewLC();
            gsLauncher->LaunchGSViewL( KAppManagerUid, zeroUid, KNullDesC8 );
            CleanupStack::PopAndDestroy( gsLauncher );
            break;
            
        case EAppVoiceCommandCPView:
            gsLauncher = CGSLauncher::NewLC();
            gsLauncher->LaunchGSViewL( KVoiceCommandUid, zeroUid, KNullDesC8 );
            CleanupStack::PopAndDestroy( gsLauncher );
            break;
            
        default:
            err = KErrNotSupported;
            break;
        }

        // Check if it's profile
    if ( err == KErrNotSupported && aCommandId >= KProfileIdsStart )
        {
        ProfileEngL( aCommandId );
        }

    return err;
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::ProfileEngL
// Check which profile user wants to activate and do it.
// ---------------------------------------------------------
//
void CVCApplicationControllerImpl::ProfileEngL( TUint aCommandId )
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::ProfileEngL" );

    // cut the VAS DB value from aCommandId so we can activate the profile.
    iProfileId = 0;
    iProfileId = aCommandId -= KProfileValue;
        
    RUBY_DEBUG1( "CVCApplicationControllerImpl::ProfileEngL: Trying to activate profile id: %d", iProfileId );
    
    // Create profile engine
    iProfileEngine = CreateProfileEngineL();
    
        // If Off-line profile is supported
    if( ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) ) &&
        // and active profile is Off-line
        ( iProfileEngine->ActiveProfileId() == EProfileOffLineId ) &&
        // and currently focused profile is not Off-line
        ( iProfileId != EProfileOffLineId ) )
        {
        // We are changing from Off-line to a normal profile.
        // Check if SIM card exists or not
        TInt simCStatus( ESimNotPresent );

        RProperty simStatus;
        User::LeaveIfError(
        simStatus.Attach( KPSUidStartup, KPSSimStatus ) );
        User::LeaveIfError( simStatus.Get( simCStatus ) );
        simStatus.Close();

        if( simCStatus == ESimNotPresent )
            {
            // SIM card does not exist.
            // Show an information note and exit from method.
            HBufC* infoText = StringLoader::LoadLC( R_TEXT_INSERT_SIM );
            CAknInformationNote* note =
                new( ELeave ) CAknInformationNote( ETrue );
// Cover UI start
            if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
                {
                // Initializes cover support
                note->PublishDialogL( SecondaryDisplay::ECmdShowInsertSimNote,
                                      SecondaryDisplay::KCatSysAp );
                }
// Cover UI end

            note->ExecuteLD( *infoText );
            CleanupStack::PopAndDestroy( infoText );
            return;
            }
        else
            {
            iState = EQueryDialog;

            if ( iCoeEnv && iEikonEnv->EikAppUi() )
                {
                iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityAlert );
                }
                        
            iQueryDialog = CAknQueryDialog::NewL();
// Cover UI start
            if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
                {
                // Initializes cover support
                iQueryDialog->PublishDialogL(
                    SecondaryDisplay::ECmdShowLeaveOfflineProfileQuery,
                    SecondaryDisplay::KCatAppController );
                }
// Cover UI end
            
            TInt result = iQueryDialog->ExecuteLD( R_LEAVE_OFFLINE_PROFILE_CONFIRMATION_QUERY );
            
            iQueryDialog = NULL;            
            if ( iCoeEnv && iEikonEnv->EikAppUi() )
                {
                iEikonEnv->EikAppUi()->RemoveFromStack( this );
                }

            if ( !result )
                {
                // SIM card exists.
                // User replied that he/she doesn't want to activate RF.
                // Exit from method, do not activate the selected profile.
        
                return;                
                } 
            }
        }
    iProfileEngine->SetActiveProfileL( iProfileId );
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::VmbxEngL()
// Get the Voice mailbox number and make the call.
// ---------------------------------------------------------
//
TInt CVCApplicationControllerImpl::VmbxEngL()
    {
    RUBY_DEBUG_BLOCK( "CVCApplicationControllerImpl::VmbxEngL" );
    
    iState = EVoiceMail;

    TBuf<KVmbxMaxNumberLength> iNumber;


    // existing ETel connection should be used in Open() where available
    User::LeaveIfError( iVmbx.Open() );  

    // Get vmbx number, active ALS line used as default
    TInt err = iVmbx.GetVmbxNumber( iNumber );
    

    if( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }

    // If not found, query the user to enter it
    if (err == KErrNotFound)
        {
        RUBY_DEBUG0( "Querying voice mailbox number" );
        if ( iVmbx.QueryNumberL(EVmbxNotDefinedQuery, iNumber) )
            {
            if( iNumber.Length() )
                {
                RUBY_DEBUG0( "CVCApplicationControllerImpl::VmbxEngL: calling voice mailbox number" );
                DialVmbxL( iNumber );
                }
            else
                {
                // User entered incorrect number of nothing
                // This is handled by the vmbx engine.
                }   
            }
        else
            {
            // User pressed cancel
            }
        }
    // vmbx number was found so just make the call.
    else if (!err && iNumber.Length())
        {
        RUBY_DEBUG0( "CVCApplicationControllerImpl::VmbxEngL: calling voice mailbox number" );
        DialVmbxL( iNumber );
        }
        
    iVmbx.Close();

    return err;
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::DialVmbxL
// Does the call to voice mailbox number
// ---------------------------------------------------------
//
void CVCApplicationControllerImpl::DialVmbxL( const TDesC& aNumber )
    {

    CAiwServiceHandler* serviceHandler = CAiwServiceHandler::NewLC();

    // Create AIW interest
    RCriteriaArray interest;
    CleanupClosePushL( interest );      // cleanup stack 2
    CAiwCriteriaItem* criteria = CAiwCriteriaItem::NewLC( 
                                        KAiwCmdCall, KAiwCmdCall,
                                        _L8( "*" ) );  // cleanup stack 3
    TUid base;
    base.iUid = KAiwClassBase;
    criteria->SetServiceClass( base );
    User::LeaveIfError( interest.Append( criteria ) );
     
    // attach to AIW interest
    serviceHandler->AttachL( interest );
        
    CAiwDialData* dialData = CAiwDialData::NewLC();   
    dialData->SetPhoneNumberL( aNumber );
           
    dialData->SetCallType( CAiwDialData::EAIWForcedCS );
    
    CAiwGenericParamList& paramList = serviceHandler->InParamListL();
    dialData->FillInParamListL(paramList);

    serviceHandler->ExecuteServiceCmdL( KAiwCmdCall,
                                        paramList,
                                        serviceHandler->OutParamListL(),
                                        0,
                                        this );
                                        
    iWait.Start();
                                        
    CleanupStack::PopAndDestroy( dialData );
    CleanupStack::PopAndDestroy( criteria ); 
    CleanupStack::PopAndDestroy( &interest );
    CleanupStack::PopAndDestroy( serviceHandler );

    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::BTEngL()
// Check if BT is on/off and change it.
// ---------------------------------------------------------
//
#ifdef __BT
TInt CVCApplicationControllerImpl::BTEngL()
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::BTEngL" );
    
    TInt err = KErrNone;

    TBTPowerStateValue powerState = EBTPowerOff;
    CBTEngSettings* mcm = CBTEngSettings::NewLC(NULL); // No callbacks from this environment
    
    // Getter for BT MCM power mode
    // returns power mode via powerState reference
    err = mcm->GetPowerState(  powerState );

    if( ( powerState == EBTPowerOff ) && ( !err ) ) // Power is OFF
        {
        TInt offlineModeOff( 0 );    // possible values are 0 and 1
        TInt activationEnabled( 0 ); // possible values are 0 and 1

        // Check whether local Bluetooth name is defined
        TBuf<KBTMaxNameLength> btName;
        mcm->GetLocalName( btName );
        if ( btName.Length() <= 0 )
            {
            // Activate Bluetooth UI
            ActivateAppL( KUidAppBtUi );
            }
        else
            {
            // Checks whether off-line mode is on and whether Bluetooth can be
            // activated in off-line mode
            GetOfflineModeFlagsL( offlineModeOff, activationEnabled );

            if ( !offlineModeOff )
                {
                // Choose user interaction
                if ( activationEnabled )
                    {
                    iState = EQueryDialog;
    
                    if ( iCoeEnv && iEikonEnv->EikAppUi() )
                        {
                        iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityAlert );
                        }

                    // Create and show query
                    //
                    iQueryDialog = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
// Cover UI start
                    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
            		    {
                        // Initializes cover support
                        iQueryDialog->PublishDialogL( 
                            SecondaryDisplay::ECmdShowOfflineBtActivationQuery,
                	    	SecondaryDisplay::KCatAppController );
                        }
// Cover UI end
                    TInt result = iQueryDialog->ExecuteLD(R_BT_ACTIVATE_IN_OFFLINE_QUERY);
                    
                    iQueryDialog = NULL;                    
                    if ( iCoeEnv && iEikonEnv->EikAppUi() )
                        {
                        iEikonEnv->EikAppUi()->RemoveFromStack( this );
                        }
                    
                    if ( result )
                        {
                        err = mcm->SetPowerState( EBTPowerOn );  // Set power on
                        }
                    }
                else
                    {
                    HBufC* stringHolder = StringLoader::LoadLC( R_BT_OFFLINE_DISABLED );
                    CAknInformationNote* note = new (ELeave) CAknInformationNote();
// Cover UI start
                    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
                        {
                        // Initializes cover support
                        note->PublishDialogL( SecondaryDisplay::ECmdShowBtDisabledOfflineNote,
                                              SecondaryDisplay::KCatAppController );
                        }
// Cover UI end

                    note->ExecuteLD( *stringHolder );
                    CleanupStack::PopAndDestroy(); //stringHolder
                    }
                }
            else
                {
                // Offline mode is off
                //
                if ( activationEnabled )
                    {
                    err = mcm->SetPowerState( EBTPowerOn );  // Set power on
                    }
                }
            }
        }
    else if(( powerState == EBTPowerOn ) && ( !err ))     // Power is ON
        {
        err = mcm->SetPowerState( EBTPowerOff ); // Set power off
        }
          
    CleanupStack::PopAndDestroy( mcm );

    if ( err != KErrNone )
        {
        HBufC* text = StringLoader::LoadLC(R_BT_GENERAL_ERROR);
        CAknErrorNote* dlg = new (ELeave) CAknErrorNote(ETrue);
// Cover UI start
        if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
            {
            // Initializes cover support
            dlg->PublishDialogL( SecondaryDisplay::ECmdShowBtGeneralErrorNote,
                                 SecondaryDisplay::KCatAppController );
            }
// Cover UI end

        dlg->ExecuteLD(*text);
        CleanupStack::PopAndDestroy(); // text
        }
    return err;
    }

#endif // __BT

// ----------------------------------------------------------
// CVCApplicationControllerImpl::GetOfflineModeFlagsL
// ----------------------------------------------------------
//
void CVCApplicationControllerImpl::GetOfflineModeFlagsL( TInt& aOfflineModeOff, 
                                                    TInt& aBTActivationEnabled ) const
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::GetOfflineModeFlagsL" );
    
    // Connecting and initialization:
    CRepository* repository = CRepository::NewL(KCRUidCoreApplicationUIs);
    
    // Offline mode on?
    repository->Get( KCoreAppUIsNetworkConnectionAllowed, aOfflineModeOff );
    
    RUBY_DEBUG1( "CVCApplicationControllerImpl::GetOfflineModeFlagsL: offline status %d", aOfflineModeOff );
    
    // Closing connection:
    delete repository;
    
    // BT Activation enabled?
    
    CRepository* repository2 = CRepository::NewL( KCRUidBluetoothEngine );
  
    repository2->Get(KBTEnabledInOffline, aBTActivationEnabled);

    RUBY_DEBUG1( "CVCApplicationControllerImpl::GetOfflineModeFlagsL: activation allowed %d", aBTActivationEnabled );
    
    // Closing connection:
    delete repository2;
    }

// ----------------------------------------------------------
// CVCApplicationControllerImpl::GetSimcardMode
// ----------------------------------------------------------
//
void CVCApplicationControllerImpl::GetSIMCardMode( TInt& aSIMCardMode ) const                               
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::GetSIMCardMode" );

        // We are changing from Off-line to a normal profile.
        // Check if SIM card exists or not
        aSIMCardMode = ESimNotPresent;

        RProperty simStatus;
        simStatus.Attach( KPSUidStartup, KPSSimStatus );
        simStatus.Get( aSIMCardMode );
        simStatus.Close();
        }

// ---------------------------------------------------------------------------
// CVCApplicationControllerImpl::ActivateMessageL
// ---------------------------------------------------------------------------
TInt CVCApplicationControllerImpl::ActivateMessageL(TEditorType aType)
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::ActivateMessageL" );
    
    TInt err = KErrNone;
    TUid mtmType = KNullUid;

    switch( aType )
        {
        case EEditorTypeNewMessage:            
            mtmType.iUid = KSenduiMtmUniMessageUidValue ;
            break;
        case EEditorTypeNewEmail:            
            mtmType.iUid = KSenduiMtmSmtpUidValue;
            break;
        case EEditorTypePostcard:
            mtmType.iUid = KSenduiMtmPostcardUidValue;
            break;
        default:
            err = KErrNotFound;
            break;
        }
    if (!err)
        {
        MHandlesSendAppUi().CreateAndSendMessageL( mtmType, 
                                                   NULL,
                                                   KNullUid,
                                                   EFalse );
        }
    return err;
    }
    
// ---------------------------------------------------------------------------
// CVCApplicationControllerImpl::MHandlesSendAppUi()
// Returns send ui instance.
// ---------------------------------------------------------------------------
CSendUi& CVCApplicationControllerImpl::MHandlesSendAppUi()
    {
    if ( !iSendUi )
        {
        TRAPD( error, iSendUi = CSendUi::NewL() );
        if ( error != KErrNone )
            {
            iSendUi = NULL;    
            }
        }
    return *iSendUi;
    }

// ----------------------------------------------------
// CVCApplicationControllerImpl::HandleCurrentCallStatusChange
// Handle call status change event.
// ----------------------------------------------------
//
void CVCApplicationControllerImpl::HandleCurrentCallStatusChange()
    {
    switch( iPropertyHandler->Value() )
        {
        case EPSCTsyCallStateNone:
        case EPSCTsyCallStateAlerting:
        case EPSCTsyCallStateRinging:
            {
            switch (iState)
                {
                case EVoiceMail:
                    {
                    iCallStatus = ETrue;
                    // Cancel the menu
                    iVmbx.Close();
                    
                    break;
                    }
                case ENewMessage:
                    {                  
                    iCallStatus = ETrue;
                    // Cancel the menu
                    Cancel();
                    if (iCoeEnv && iEikonEnv->EikAppUi())
                        {
                        iEikonEnv->EikAppUi()->RemoveFromStack(this);
                        }
                    break;
                    }
                case EQueryDialog:
                    {
                    iCallStatus = ETrue;
                    
                    DismissQueryDialog();
                        
                    break;
                    }
                default:
                    // Pass
                    break;
                }
            break;
            }

        default:
            {
            // pass
            break;
            }
        }
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::Cancel
// ---------------------------------------------------------
//
void CVCApplicationControllerImpl::Cancel()
    {
    // Cancel the menu
    if ( iPopupList )
        {
        iPopupList->CancelPopup();
        iPopupList = NULL;
        }
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVCApplicationControllerImpl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::OfferKeyEventL" );
    
    TKeyResponse keyEvent( EKeyWasNotConsumed );
    
    if ( aType == EEventKey && ( aKeyEvent.iScanCode == EStdKeyNo ||
         aKeyEvent.iScanCode == EStdKeyApplication0 ) )
        {
        switch ( iState )
            {
            case EVoiceMail:
                iVmbx.Close();
                keyEvent = EKeyWasConsumed;
                break;

            case ENewMessage:
                Cancel();

                if ( iCoeEnv && iEikonEnv->EikAppUi() )
                    {
                    iEikonEnv->EikAppUi()->RemoveFromStack( this );
                    }
                keyEvent = EKeyWasConsumed;
                break;
                
            case EQueryDialog:            
                DismissQueryDialog();
                    
                keyEvent = EKeyWasConsumed;
                break;

            default:
                break;
            }
        }

    return keyEvent;
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::HandleNotifyL
// ---------------------------------------------------------
//    
TInt CVCApplicationControllerImpl::HandleNotifyL( TInt aCmdId, TInt /*aEventId*/,
                                                  CAiwGenericParamList& /*aEventParamList*/,
                                                  const CAiwGenericParamList& /*aInParamList*/ )
    {
    RUBY_DEBUG_BLOCKL( "CVCApplicationControllerImpl::HandleNotifyL" );
    
    if ( aCmdId == KAiwCmdCall )
        {
        if ( iWait.IsStarted() )
            {
            iWait.AsyncStop();
            }
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------
// CVCApplicationControllerImpl::HandleActiveObjectStatus
// ---------------------------------------------------------
//
void CVCApplicationControllerImpl::HandleActiveObjectStatus(TInt aStatus)
    {
    RUBY_DEBUG1( "CVCApplicationControllerImpl::HandleActiveObjectStatus() aStatus = %d", aStatus);
    
    // Cancel active object and note.
    iActiveObject->Cancel();
    iGlobalConfQuery->CancelConfirmationQuery();    
    
    // Check that user pressed softkey YES
    if ( (aStatus == EAknSoftkeyYes ||    
          aStatus == EAknSoftkeySelect ||
          aStatus == EAknSoftkeyOk) && 
          iProfileEngine )    
        {
        TRAP_IGNORE( iProfileEngine->SetActiveProfileL( iProfileId ) );
        }
    }
    
// ----------------------------------------------------------------------------
// CVCApplicationControllerImpl::CreateConnectionL
// Connects to RApaLsSession.
// ----------------------------------------------------------------------------
//
void CVCApplicationControllerImpl::CreateApaConnectionL()
    {
    iApaLsSession = new RApaLsSession();
       
    if ( !iApaLsSession->Handle() )
        {
        User::LeaveIfError( iApaLsSession->Connect() );
        }
    }

// ----------------------------------------------------------------------------
// CVCApplicationControllerImpl::ActivateAppL
// Used to launch applications. Checks whether an instance of the launched
// application is already running.
// ----------------------------------------------------------------------------
//
void CVCApplicationControllerImpl::ActivateAppL( const TUid aUid,
                                                 TBool aMultipleInstances )
    {
    CreateApaConnectionL();

    if ( aMultipleInstances )
        {
        ActivateAppInstanceL( aUid );
        }
    else
        {
        // Session to window server
        RWsSession wsSession;
        User::LeaveIfError( wsSession.Connect() );
        CleanupClosePushL( wsSession );
        
        TApaTaskList apaTaskList( wsSession );
        TApaTask apaTask = apaTaskList.FindApp( aUid );
        if ( apaTask.Exists() )
            {
            // Already running, close application
            apaTask.EndTask();
            }
        ActivateAppInstanceL( aUid );
            
        CleanupStack::PopAndDestroy( &wsSession );
        }
    }

// ----------------------------------------------------------------------------
// CVCApplicationControllerImpl::ActivateAppInstanceL
// Activates an instance of an application.
// ----------------------------------------------------------------------------
//
void CVCApplicationControllerImpl::ActivateAppInstanceL( const TUid aUid )
    {
    TApaAppInfo appInfo;
    User::LeaveIfError( iApaLsSession->GetAppInfo( appInfo, aUid ) );
    TFileName appName = appInfo.iFullName;
    CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();

    apaCommandLine->SetExecutableNameL( appName );
    apaCommandLine->SetCommandL( EApaCommandRun );
    
    User::LeaveIfError ( iApaLsSession->StartApp( *apaCommandLine ) );
    CleanupStack::PopAndDestroy( apaCommandLine );
    }

// ----------------------------------------------------------------------------
// CVCApplicationControllerImpl::DismissQueryDialog
// Closes query dialog
// ----------------------------------------------------------------------------
//
void CVCApplicationControllerImpl::DismissQueryDialog()
    {
    if ( iQueryDialog )
        {
        iQueryDialog->MakeLeftSoftkeyVisible( EFalse );
        TRAP_IGNORE( iQueryDialog->DismissQueryL() );
        }
    }

// End of File
