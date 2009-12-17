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
* Description:  Definition of the implementation class for the
*                application controller interface 
*
*/


#ifndef __CVCAPPLICATIONCONTROLLERIMPL_H__
#define __CVCAPPLICATIONCONTROLLERIMPL_H__


//  INCLUDES
#include "appcontrollerconstants.h"
#include "appcontroller.hrh"
#include <e32base.h>
#include <e32std.h>
#include <barsread.h>
#include <apgcli.h>
#include <eikenv.h>
#include <coecntrl.h>
#include <eikappui.h>
#include <AiwCommon.h>

// Phone
#include <PhCltTypes.h>


// Bluetooth engine API
#include <btengsettings.h>


// Vmbx Engine API
#include <vmnumber.h>
#include <w32std.h>
#include <PbkFields.hrh>
#include <vmbx.rsg>

#include <e32property.h>        // RProperty
#include <PSVariables.h>        // Property values
#include <MProfileEngine.h>

// FORWARD DECLARATIONS
class CAknPopupList;
class CAppControllerPropertyHandler;
class CUidNameArray;
class CSendUi;
class CAppControllerActiveObject;
class CAknGlobalConfirmationQuery;

// CLASS DECLARATION

/**
* This is the implementation class for launching applications and initiating profiles.
* @lib AppController.lib
* @since 2.0
*/

class CVCApplicationControllerImpl : public CCoeControl,
                                                      public MAiwNotifyCallback
    {
    public:  // Constructors and destructor

    enum TState
      {
      EVoiceMail,
      ENewMessage,
      EQueryDialog
      };

        /**
        * Two-phased constructor.
        */
        static CVCApplicationControllerImpl* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CVCApplicationControllerImpl();

    public: // New functions
        
        /**
        * Generic function used to execute a voice command
        * @param aCommandId     ID of command to be executed.
        * @param aParameters    The name of the application of profile.
        * @return TInt          KErrNone if OK, else value indicating error situation.
        */
        TInt ExecuteCommandL( TUint aCommandId, const TDesC& aParameters );
        
        /**
        * Generic function to be used to execute voice command
        * @since Averell 2.0
        * @param aCommandId     ID of command to be executed.
        * @param aParameters    The name of the application of profile.
        * @return TInt          KErrNone if OK, else value indicating error situation.
        */
        TInt DoExecuteCommandL( TUint aCommandId, const TDesC& aParameters );

    public: // Functions from base classes

        /*
        * From CCoeControl
        * @see CCoeControl
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
        
        /*
        * From MAiwNotifyCallback
        * @see MAiwNotifyCallback For more detailed information
        */
        TInt HandleNotifyL( TInt aCmdId, TInt aEventId,
                            CAiwGenericParamList& aEventParamList,
                            const CAiwGenericParamList& aInParamList );
        
    public: // New functions

        /**
        * Handles the incoming call 
        * @since 2.8
        * @return void
        */
        void HandleCurrentCallStatusChange();

        /**
        * Receives active object events and acts according to them
        * @since 2.8
        * @param aStatus active object status
        * @return void
        */
        void HandleActiveObjectStatus(TInt aStatus);
        
    private: // Constructors

        /**
        * C++ default constructor.
        */
        CVCApplicationControllerImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Initiates profiles
        * @since 2.0
        * @param aCommandId The Profile Id
        * @return void
        */
        void ProfileEngL(TUint aCommandId);

        /**
        * Call to the Voice mailbox number and if no number is defined
        * it asks user to enter it and thern makes the call.
        * @since 2.0
        * @return TInt Standard Symbian error code
        */
        TInt VmbxEngL();

        /**
        * Activates/deactivates the Bluetooth power mode 
        * @since 2.0
        * @return TInt Standard Symbian error code
        */
#ifdef __BT
        TInt BTEngL();
#endif

        /**
        * Cancels the NewMessage Dialog
        * @since 2.0
        * @return void
        */
        void Cancel();

        /**
        * Gets the flags associated with offline mode.
        * @param aOfflineModeOff Offline mode flag
        * @param aBTActivationEnabled BT activation flag
        * @return None.
        */
        void GetOfflineModeFlagsL( TInt& aOfflineModeOff, 
                                   TInt& aBTActivationEnabled ) const;

        /**
        * Returns CSendUi object
        * @since 3.0
        * @return CSendUi
        */
        CSendUi& MHandlesSendAppUi();

        /**
        * Activates the user selected message editor
        * @since 2.6
        * @param aType a message type (sms, mms, email).
        * @return TInt an error code
        */
        TInt ActivateMessageL( TEditorType aType );

        /**
        * Gets the flags associated with SIM card.
        * @param aSIMCardMode, SIM card mode flag
        * @return None.
        */
        void GetSIMCardMode( TInt& aSIMCardMode ) const;
        
        /**
        * Connects to RApaLsSession.
        * @since 3.0
        * @return void
        */  
        void CreateApaConnectionL();

        /* Activates an instance of an application.
        * @since 3.0
        * @param aUid Application UID
        * @param aCommandLine Command line parameters.
        * @return void
        */
        void ActivateAppInstanceL( const TUid aUid );
    
        /**
        * Used to launch applications. Checks whether an instance of the launched
        * application is already running.
        * @since 3.0
        * @param aUid The Uid of the specific application.
        * @param aCommandLine Command line parameters.
        * @param aMultipleInstances If true, the application is activated
        *        in multiple instances.
        * @return void
        */
        void ActivateAppL( const TUid aUid,
                           TBool aMultipleInstances = EFalse );
                           
        /**
        * Does voice call to voicemailbox number
        *
        * @param aNumber Phone number to be called
        */                   
        void DialVmbxL( const TDesC& aNumber );
        
        /* 
        * Closes query dialog
        */
        void DismissQueryDialog();
        
    private:     // Data
        
        //for the new message menu
        CAknPopupList*                       iPopupList; // Not owned
        RApaLsSession*                       iApaLsSession;    
        TInt                                 iResourceFileOffset;
        CAppControllerPropertyHandler*       iPropertyHandler;
        TBool                                iPSEventCheckStatus;
        
        CVCApplicationControllerImpl::TState iState;
        RVmbxNumber                          iVmbx;
        TBool                                iCallStatus;
        // array for supported message types
        CUidNameArray*                       iMsgTypes;
        // send ui for launching message editors
        CSendUi*                             iSendUi;
        CAppControllerActiveObject*          iActiveObject;
        CAknGlobalConfirmationQuery*         iGlobalConfQuery;
        MProfileEngine*                      iProfileEngine;
        TInt                                 iProfileId;
        CAknQueryDialog*                     iQueryDialog;
        CActiveSchedulerWait                 iWait;
    };

#endif      // CVCAPPLICATIONCONTROLLERIMPL_H   
            
// End of File