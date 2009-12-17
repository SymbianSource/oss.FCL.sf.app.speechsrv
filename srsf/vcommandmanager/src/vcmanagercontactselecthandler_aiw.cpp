/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


// INCLUDE FILES
#include "vcmanagercontactselecthandler_aiw.h"
#include "vcmanagerdialer_aiw.h"

#include <CPbkFieldInfo.h>

#include <vascvpbkhandler.h>

#include <cntdef.h>
#include <eikenv.h>

#include <featmgr.h> // FeatureManager.

#ifdef __SIND_EXTENSIONS
#include <sendui.h>
#include <SendUiConsts.h>
#include <CMessageData.h>
#endif // __SIND_EXTENSIONS

#include "rubydebug.h"

#ifdef __WINS__
// Text to show in emulator when dialing
_LIT( KDialingNoteWins, "Dialing" );
#endif

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVcManagerContactSelectHandler::CVcManagerContactSelectHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CVcManagerContactSelectHandler::CVcManagerContactSelectHandler()
 : iVideoTelephonySupported( EFalse ), iVoIPFeatureSupported( EFalse )     
    {
    // Nothing
    }

// ---------------------------------------------------------
// CVcManagerContactSelectHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVcManagerContactSelectHandler* CVcManagerContactSelectHandler::NewL(
                                       MNssContextMgr* aContextManager,
                                       MNssTagSelectNotification* aTagSelectNotification )
    {
    CVcManagerContactSelectHandler* self = new (ELeave) CVcManagerContactSelectHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL( aContextManager, aTagSelectNotification );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CVcManagerContactSelectHandler::~CVcManagerContactSelectHandler
// Destructor
// ---------------------------------------------------------
//
CVcManagerContactSelectHandler::~CVcManagerContactSelectHandler()
    {
    RUBY_DEBUG0( "CVcManagerContactSelectHandler::~CVcManagerContactSelectHandler" );

    delete iPbkHandler;

    if ( iTagSelectNotification )
        {
        iTagSelectNotification->Deregister( iContext, this );
        }

    delete iContext;

#ifdef __SIND_EXTENSIONS
    delete iSendUi;
#endif // __SIND_EXTENSIONS

    delete iGlobalDialer;
    }

// ---------------------------------------------------------
// CVcManagerContactSelectHandler::HandleTagSelect
// EPOC default constructor can leave.
// ---------------------------------------------------------
//
void CVcManagerContactSelectHandler::ConstructL( MNssContextMgr* aContextManager,
                                                 MNssTagSelectNotification* aTagSelectNotification )
    {
    RUBY_DEBUG_BLOCK( "CVcManagerContactSelectHandler::ConstructL" );
    
    iTagSelectNotification = aTagSelectNotification;
    
    iContext = aContextManager->CreateContextL();
    iContext->SetNameL( KNssGCHNameDialContext );
    iContext->SetGlobal( ETrue );
    iTagSelectNotification->RegisterL( iContext, this );

    RUBY_DEBUG0( "CVcManagerContactSelectHandler::ConstructL creating instance of CVasVPbkHandler" );
    iPbkHandler = CVasVPbkHandler::NewL();
    iPbkHandler->InitializeL();
    
    FeatureManager::InitializeLibL();
    iVideoTelephonySupported = FeatureManager::FeatureSupported( KFeatureIdCsVideoTelephony );
    
    iVoIPFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdCommonVoip );

    // Frees the TLS. Must be done after FeatureManager is used.
    FeatureManager::UnInitializeLib();
    
    // Take Eikon environment
    iEikonEnvironment = CEikonEnv::Static();
    }

// ---------------------------------------------------------
// CVcManagerContactSelectHandler::HandleTagSelect
// Dials number
// ---------------------------------------------------------
//
void CVcManagerContactSelectHandler::HandleTagSelect( MNssTag* aTag )
    {
    RUBY_DEBUG2( "CVcManagerContactSelectHandler::HandleTagSelect VOIP %i video %i", iVoIPFeatureSupported, iVideoTelephonySupported );

    TRAP_IGNORE( DoHandleTagSelectL( aTag ) );
    }

// ---------------------------------------------------------
// CVcManagerContactSelectHandler::DoHandleTagSelect
// Dials number or create message
// ---------------------------------------------------------
//
void CVcManagerContactSelectHandler::DoHandleTagSelectL( MNssTag* aTag )
    {
    RUBY_DEBUG_BLOCK( "CVcManagerContactSelectHandler::DoHandleTagSelectL" );
   
    CleanupDeletePushL( aTag );
   
    HBufC* name = aTag->SpeechItem()->PartialTextL( KNameTrainingIndex );        
    CleanupStack::PushL( name ); 
   
    iContactId = aTag->RRD()->IntArray()->At( KVasContactIdRrdLocation );
  
    iPbkHandler->FindContactFieldL( aTag );
       
    TVasExtensionAction action =
         (TVasExtensionAction)aTag->RRD()->IntArray()->At( KVasExtensionRrdLocation );
    TVasExtensionCommand command =
         (TVasExtensionCommand)aTag->RRD()->IntArray()->At( KVasExtensionCommandRrdLocation );

    if ( action == EDial )
        {
        DialL( *name, iPbkHandler->TextL(), iPbkHandler->FieldTypeL(), command ); 
        }
#ifdef __SIND_EXTENSIONS  
    else
        {
        CMessageData* messageData = CMessageData::NewL();
        CleanupStack::PushL( messageData );
                   
        // created when needed first time
        if ( !iSendUi )
            {
            iSendUi = CSendUi::NewL();
            }
         
        TUid sendUid( KSenduiMtmSmsUid );
        switch ( action )
            {
            case ENewMessage:
                // @todo Check that KSenduiMtmUniMessageUid is ok to use
                // instead of SenduiMtmMmsUid or KSenduiMtmSmsUid
                sendUid = KSenduiMtmUniMessageUid;
                messageData->AppendToAddressL( iPbkHandler->TextL(), *name );  
                break;
                   
            case ENewEmail:
                sendUid = KSenduiMtmSmtpUid;
                messageData->AppendToAddressL( iPbkHandler->TextL(), *name );
                break;
                   
            default:
                RUBY_ERROR0( "CVcManagerContactSelectHandler::DoHandleTagSelectL wrong action id" );
            }
              
        RUBY_DEBUG1( "CVcManagerContactSelectHandler::DoHandleTagSelectL action %i",
                      action );               
        // launch editor as a stand-alone application so that 
        // it can/will be restored if put to background
        iSendUi->CreateAndSendMessageL( sendUid, messageData, KNullUid, EFalse );
        
        CleanupStack::PopAndDestroy( messageData );
        } 
#endif // __SIND_EXTENSIONS        

    CleanupStack::PopAndDestroy( name );
    CleanupStack::Pop( aTag );
    }


// ---------------------------------------------------------
// CVcManagerContactSelectHandler::DialL
// Dials contact
// ---------------------------------------------------------
//
#ifndef __WINS__
void CVcManagerContactSelectHandler::DialL( const TDesC& aName,
                                            const TDesC& aPhoneNumber,
                                            TFieldType aFieldType,
                                            TVasExtensionCommand aCommandId )
#else
void CVcManagerContactSelectHandler::DialL( const TDesC& /*aName*/,
                                            const TDesC& aPhoneNumber,
                                            TFieldType aFieldType,
                                            TVasExtensionCommand aCommandId )
#endif // __WINS__                                            
    {
    RUBY_DEBUG2( "CVcManagerContactSelectHandler::DialL fieldtype %i commandId %i", aFieldType, aCommandId );

    // select call type
    TCallType callType( EVoice );

    if ( ( aCommandId == EVoipCommand || ( aCommandId == EDefaultCommand &&
           aFieldType == KUidContactFieldVCardMapVOIP ) ) && iVoIPFeatureSupported )
        {
        RUBY_DEBUG0( "CVcManagerContactSelectHandler::SetupDialData VOIP" );

        callType = EVoIP;
        }
    
    if ( ( aCommandId == EVideoCommand || ( aCommandId == EDefaultCommand &&
           aFieldType == KUidContactFieldVCardMapVIDEO ) ) && iVideoTelephonySupported )
        {
        RUBY_DEBUG0( "CVcManagerContactSelectHandler::SetupDialData video" );

        callType = EVideo;
        }

#ifndef __WINS__
    if ( !iGlobalDialer )
        {
        iGlobalDialer = CVcManagerDialer::NewL();
        }
    iGlobalDialer->ExecuteL( aName, aPhoneNumber, callType );
#else
    iEikonEnvironment->AlertWin( KDialingNoteWins, aPhoneNumber );
#endif // __WINS__

    RUBY_DEBUG0( "CVcManagerContactSelectHandler::DialL done" );
    }

// End of file
