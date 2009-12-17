/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <centralrepository.h>
#include <vcommanddomaincrkeys.h>
#include <voiceuidomainpskeys.h>

#include <vascvpbkhandler.h>

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuictoneplayer.h"
#include "vuicpropertyhandler.h"
#include "vuictutorial.h"
#include "vuicttsplayer.h"

#include "rubydebug.h"
    
// -----------------------------------------------------------------------------
// CDataStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDataStorage* CDataStorage::NewL( CVoiceRecognitionDialogImpl* aVoiceRecognitionImpl )
    {
    RUBY_DEBUG_BLOCK( "CDataStorage::NewL" );
    
    CDataStorage* self = new (ELeave) CDataStorage( aVoiceRecognitionImpl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }    
    
// Destructor       
CDataStorage::~CDataStorage()
    {
    RUBY_DEBUG0( "CDataStorage::~CDataStorage START" );
    
    if ( iTonePlayer )
        {
        iTonePlayer->RegisterObserver( NULL );
        }
    delete iTonePlayer;
    
    delete iPbkHandler;
    
    delete iTtsPlayer;
    
    delete iRecognitionHandlerBuilder;             // Deletes also iRecognitionHandler
    delete iVerificationRecognitionHandlerBuilder; // Deletes also iVerificationRecognitionHandler
    delete iAccessoryButtonMonitor;
    delete iTutorial;
    
    // Contains original tags
    if ( iAdditionalTagList )
        {
        iAdditionalTagList->ResetAndDestroy();
        }
    delete iAdditionalTagList;
    
    // Contains original tags
    if ( iCompleteTagList )
        {
        iCompleteTagList->ResetAndDestroy();
        }
    delete iCompleteTagList;
    
    delete iTag;
    delete iContext;
    
    RUBY_DEBUG0( "CDataStorage::~CDataStorage EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetRecognitionHandlerBuilder
// -----------------------------------------------------------------------------
//
void CDataStorage::SetRecognitionHandlerBuilder( CNssRecognitionHandlerBuilder* aRecognitionHandlerBuilder )
    {
    RUBY_DEBUG0( "CDataStorage::SetRecognitionHandlerBuilder START" );
    
    if ( iRecognitionHandlerBuilder && iRecognitionHandlerBuilder != aRecognitionHandlerBuilder )
        {
        delete iRecognitionHandlerBuilder; // Deletes also iRecognitionHandler
        iRecognitionHandler = NULL;
        }
    
    iRecognitionHandlerBuilder = aRecognitionHandlerBuilder;
    
    RUBY_DEBUG0( "CDataStorage::SetRecognitionHandlerBuilder EXIT" );
    }

// -----------------------------------------------------------------------------
// CDataStorage::SetRecognitionHandler
// -----------------------------------------------------------------------------
//        
void CDataStorage::SetRecognitionHandler( MNssRecognitionHandler* aRecognitionHandler )
    {
    RUBY_DEBUG0( "CDataStorage::SetRecognitionHandler START" );
        
    iRecognitionHandler = aRecognitionHandler;
    
    RUBY_DEBUG0( "CDataStorage::SetRecognitionHandler EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetVerificationRecognitionHandlerBuilder
// -----------------------------------------------------------------------------
//
void CDataStorage::SetVerificationRecognitionHandlerBuilder( CNssRecognitionHandlerBuilder* aRecognitionHandlerBuilder )
    {
    RUBY_DEBUG0( "CDataStorage::SetVerificationRecognitionHandlerBuilder START" );
    
    if ( iVerificationRecognitionHandlerBuilder && iVerificationRecognitionHandlerBuilder != aRecognitionHandlerBuilder )
        {
        delete iVerificationRecognitionHandlerBuilder; // Deletes also iRecognitionHandler
        iVerificationRecognitionHandler = NULL;
        }
    
    iVerificationRecognitionHandlerBuilder = aRecognitionHandlerBuilder;
    
    RUBY_DEBUG0( "CDataStorage::SetVerificationRecognitionHandlerBuilder EXIT" );
    }

// -----------------------------------------------------------------------------
// CDataStorage::SetVerificationRecognitionHandler
// -----------------------------------------------------------------------------
//        
void CDataStorage::SetVerificationRecognitionHandler( MNssRecognitionHandler* aRecognitionHandler )
    {
    RUBY_DEBUG0( "CDataStorage::SetVerificationRecognitionHandler START" );
        
    iVerificationRecognitionHandler = aRecognitionHandler;
    
    RUBY_DEBUG0( "CDataStorage::SetVerificationRecognitionHandler EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetAdditionalTagList
// -----------------------------------------------------------------------------
//    
void CDataStorage::SetAdditionalTagList( CArrayPtrFlat<MNssTag>* aAdditionalTagList )
    {
    RUBY_DEBUG0( "CDataStorage::SetAdditionalTagList START" );
    
    if ( iAdditionalTagList && iAdditionalTagList != aAdditionalTagList )
        {
        iAdditionalTagList->ResetAndDestroy();
        delete iAdditionalTagList;
        }
    
    iAdditionalTagList = aAdditionalTagList;
    
    RUBY_DEBUG0( "CDataStorage::SetAdditionalTagList EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetCompleteTagList
// -----------------------------------------------------------------------------
//    
void CDataStorage::SetCompleteTagList( CArrayPtrFlat<MNssTag>* aCompleteTagList )
    {
    RUBY_DEBUG0( "CDataStorage::SetCompleteTagList START" );
    
    if ( iCompleteTagList && iCompleteTagList != aCompleteTagList )
        {
        iCompleteTagList->ResetAndDestroy();
        delete iCompleteTagList;
        }
    
    iCompleteTagList = aCompleteTagList;
    
    RUBY_DEBUG0( "CDataStorage::SetCompleteTagList EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetTag
// -----------------------------------------------------------------------------
//
void CDataStorage::SetTag( MNssTag* aTag, TBool aDestroyIfExists )
    {
    RUBY_DEBUG0( "CDataStorage::SetTag START" );
    
    if ( iTag && aDestroyIfExists && iTag != aTag )
        {
        delete iTag;
        }
    
    iTag = aTag;
    
    RUBY_DEBUG0( "CDataStorage::SetTag EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetContext
// -----------------------------------------------------------------------------
//
void CDataStorage::SetContext( MNssContext* aContext )
    {
    RUBY_DEBUG0( "CDataStorage::SetContext START" );
    
    if ( iContext && iContext != aContext )
        {
        delete iContext;
        }
    
    iContext = aContext;
    
    RUBY_DEBUG0( "CDataStorage::SetContext EXIT" );
    }

// -----------------------------------------------------------------------------
// CDataStorage::SetAdaptationEnabled
// -----------------------------------------------------------------------------
//
void CDataStorage::SetAdaptationEnabled( const TBool aAdaptationEnabled )
    {
    RUBY_DEBUG0( "CDataStorage::SetAdaptationEnabled START" );
    
    iAdaptationEnabled = aAdaptationEnabled;
    
    RUBY_DEBUG0( "CDataStorage::SetAdaptationEnabled EXIT" );
    }

// -----------------------------------------------------------------------------
// CDataStorage::SetPlayDuration
// -----------------------------------------------------------------------------
//
void CDataStorage::SetPlayDuration( const TTimeIntervalMicroSeconds& aPlayDuration )
    {
    RUBY_DEBUG0( "CDataStorage::SetPlayDuration START" );
    
    iPlayDuration = aPlayDuration;
    
    RUBY_DEBUG0( "CDataStorage::SetPlayDuration EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetVerificationMode
// -----------------------------------------------------------------------------
//
void CDataStorage::SetVerificationMode( const TInt aMode )
    {
    RUBY_DEBUG0( "CDataStorage::SetVerificationMode START" );
    
    if ( aMode == KResetMode )
        {
        TRAP_IGNORE( 
            CRepository* client = CRepository::NewLC( KCRUidVCommandSettings );
            client->Get( KVCVerification, iVerificationMode );
            CleanupStack::PopAndDestroy( client ) );
        }
    else
        {
        iVerificationMode = aMode;
        }
    
    RUBY_DEBUG0( "CDataStorage::SetVerificationMode EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::SetVerificationTryCount
// -----------------------------------------------------------------------------
//
void CDataStorage::SetVerificationTryCount( const TInt aCount )
    {
    RUBY_DEBUG0( "CDataStorage::SetVerificationTryCount START" );
    
    iVerificationTryCount = aCount;
    
    RUBY_DEBUG0( "CDataStorage::SetVerificationTryCount EXIT" );
    }

// -----------------------------------------------------------------------------
// CDataStorage::SetDeviceLockMode
// -----------------------------------------------------------------------------
//
void CDataStorage::SetDeviceLockMode( const TBool aDeviceLockMode )
    {
    RUBY_DEBUG0( "CDataStorage::SetDeviceLockMode START" );
    
    iDeviceLockMode = aDeviceLockMode;
    
    RUBY_DEBUG0( "CDataStorage::SetDeviceLockMode EXIT" );
    }

// -----------------------------------------------------------------------------
// CDataStorage::VoiceRecognitionImpl
// -----------------------------------------------------------------------------
//
CVoiceRecognitionDialogImpl* CDataStorage::VoiceRecognitionImpl() const
    {
    RUBY_DEBUG0( "CDataStorage::VoiceRecognitionImpl" );
    
    return iVoiceRecognitionImpl;
    }
      
// -----------------------------------------------------------------------------
// CDataStorage::RecognitionHandlerBuilder
// -----------------------------------------------------------------------------
//
CNssRecognitionHandlerBuilder* CDataStorage::RecognitionHandlerBuilder() const
    {
    RUBY_DEBUG0( "CDataStorage::RecognitionHandlerBuilder" );
    
    return iRecognitionHandlerBuilder;
    }
       
// -----------------------------------------------------------------------------
// CDataStorage::RecognitionHandler
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler* CDataStorage::RecognitionHandler() const
    {
    RUBY_DEBUG0( "CDataStorage::RecognitionHandler" );
    
    return iRecognitionHandler;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::VerificationRecognitionHandlerBuilder
// -----------------------------------------------------------------------------
//
CNssRecognitionHandlerBuilder* CDataStorage::VerificationRecognitionHandlerBuilder() const
    {
    RUBY_DEBUG0( "CDataStorage::VerificationRecognitionHandlerBuilder" );
    
    return iVerificationRecognitionHandlerBuilder;
    }
       
// -----------------------------------------------------------------------------
// CDataStorage::VerificationRecognitionHandler
// -----------------------------------------------------------------------------
//
MNssRecognitionHandler* CDataStorage::VerificationRecognitionHandler() const
    {
    RUBY_DEBUG0( "CDataStorage::VerificationRecognitionHandler" );
    
    return iVerificationRecognitionHandler;
    }
      
// -----------------------------------------------------------------------------
// CDataStorage::TonePlayer
// -----------------------------------------------------------------------------
//
CTonePlayer* CDataStorage::TonePlayer() const
    {
    RUBY_DEBUG0( "CDataStorage::TonePlayer" );
    
    return iTonePlayer;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::TtsPlayer
// -----------------------------------------------------------------------------
//
CTTSPlayer* CDataStorage::TtsPlayer() const
    {
    RUBY_DEBUG0( "CDataStorage::TtsPlayer" );
    
    return iTtsPlayer;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::PbkHandler
// -----------------------------------------------------------------------------
//
MVasBasePbkHandler* CDataStorage::PbkHandler() const
    {
    RUBY_DEBUG0( "CDataStorage::PbkHandler" );
    
    return iPbkHandler;
    }
      
// -----------------------------------------------------------------------------
// CDataStorage::AccessoryButtonMonitor
// -----------------------------------------------------------------------------
//
CPropertyHandler* CDataStorage::AccessoryButtonMonitor() const
    {
    RUBY_DEBUG0( "CDataStorage::AccessoryButtonMonitor" );
    
    return iAccessoryButtonMonitor;
    }
        
// -----------------------------------------------------------------------------
// CDataStorage::Tutorial
// -----------------------------------------------------------------------------
//
CTutorial* CDataStorage::Tutorial() const
    {
    RUBY_DEBUG0( "CDataStorage::Tutorial" );
    
    return iTutorial;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::AdditionalTagList
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<MNssTag>* CDataStorage::AdditionalTagList() const
    {
    RUBY_DEBUG0( "CDataStorage::AdditionalTagList" );
    
    return iAdditionalTagList;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::CompleteTagList
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<MNssTag>* CDataStorage::CompleteTagList() const
    {
    RUBY_DEBUG0( "CDataStorage::CompleteTagList" );
    
    return iCompleteTagList;
    }

// -----------------------------------------------------------------------------
// CDataStorage::Tag
// -----------------------------------------------------------------------------
//
MNssTag* CDataStorage::Tag() const
    {
    RUBY_DEBUG0( "CDataStorage::Tag" );
    
    return iTag;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::Context
// -----------------------------------------------------------------------------
//
MNssContext* CDataStorage::Context() const
    {
    RUBY_DEBUG0( "CDataStorage::Context" );
    
    return iContext;
    }
        
// -----------------------------------------------------------------------------
// CDataStorage::SynthesizerMode
// -----------------------------------------------------------------------------
//
TInt CDataStorage::SynthesizerMode() const
    {
    RUBY_DEBUG0( "CDataStorage::SynthesizerMode" );
    
    return iSynthesizerMode;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::AdaptationEnabled
// -----------------------------------------------------------------------------
//
TBool CDataStorage::AdaptationEnabled() const
    {
    RUBY_DEBUG0( "CDataStorage::AdaptationEnabled" );
    
    return iAdaptationEnabled;
    }

// -----------------------------------------------------------------------------
// CDataStorage::PlayDuration
// -----------------------------------------------------------------------------
//    
TTimeIntervalMicroSeconds CDataStorage::PlayDuration() const
    {
    RUBY_DEBUG0( "CDataStorage::PlayDuration" );
    
    return iPlayDuration;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::VerificationMode
// -----------------------------------------------------------------------------
//    
TInt CDataStorage::VerificationMode() const
    {
    RUBY_DEBUG0( "CDataStorage::VerificationMode" );
    
    return iVerificationMode;
    }
    
// -----------------------------------------------------------------------------
// CDataStorage::VerificationTryCount
// -----------------------------------------------------------------------------
//    
TInt CDataStorage::VerificationTryCount() const
    {
    RUBY_DEBUG0( "CDataStorage::VerificationTryCount" );
    
    return iVerificationTryCount;
    }

// -----------------------------------------------------------------------------
// CDataStorage::DeviceLockMode
// -----------------------------------------------------------------------------
//    
TBool CDataStorage::DeviceLockMode() const
    {
    RUBY_DEBUG0( "CDataStorage::DeviceLockMode" );
    
    return iDeviceLockMode;
    }
    
// ---------------------------------------------------------
// CDataStorage::CDataStorage
// ---------------------------------------------------------
//              
CDataStorage::CDataStorage( CVoiceRecognitionDialogImpl* aVoiceRecognitionImpl )
 : iVoiceRecognitionImpl( aVoiceRecognitionImpl ), iDeviceLockMode( EFalse )
    {
    }

// ---------------------------------------------------------
// CDataStorage::ConstructL
// ---------------------------------------------------------
//           
void CDataStorage::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CDataStorage::ConstructL" );

    iTonePlayer = CTonePlayer::NewL();
    iTonePlayer->RegisterObserver( iVoiceRecognitionImpl );
    
    iPbkHandler = CVasVPbkHandler::NewL();
    
    iTtsPlayer = CTTSPlayer::NewL( iVoiceRecognitionImpl );

    iAccessoryButtonMonitor = CPropertyHandler::NewL( KPSUidVoiceUiAccMonitor,
                                                      KVoiceUiOpenKey,
                                                      KPSUidVoiceUiAccMonitor,
                                                      KVoiceUiAccessoryEvent );
        
    iTutorial = CTutorial::NewL();

    TRAP_IGNORE( 
        CRepository* client = CRepository::NewLC( KCRUidVCommandSettings );
        client->Get( KVCSynthesizer, iSynthesizerMode );
        client->Get( KVCVerification, iVerificationMode );
        CleanupStack::PopAndDestroy( client ) );

    iVerificationTryCount = KVerificationTries;
    }
    
// End of File

