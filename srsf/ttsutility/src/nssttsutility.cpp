/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementations for methods in CTtsUtility
*
*/


// INCLUDE FILES
#include "nssttsutility.h"
#include "ttsutilitybody.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTtsUtility::CTtsUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTtsUtility::CTtsUtility() 
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTtsUtility::CreateInstanceL
// Creates CTtsUtility instance
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CTtsUtility::CreateInstanceL()
    {
    CTtsUtility* utility = new (ELeave) CTtsUtility();
    return static_cast<MTtsUtilityBase*>( utility );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::CreateInstanceSecondPhaseL
// Second phase construction after CreateInstanceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::CreateInstanceSecondPhaseL( MTtsClientUtilityObserver& aObserver )
    {
    ConstructL( aObserver );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTtsUtility::ConstructL( MTtsClientUtilityObserver& aObserver )
    {
    iBody = CTtsUtilityBody::NewL( aObserver );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CTtsUtility* CTtsUtility::NewL( MTtsClientUtilityObserver& aObserver )
    {
    CTtsUtility* self = new( ELeave ) CTtsUtility( );
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CTtsUtility::~CTtsUtility
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CTtsUtility::~CTtsUtility()
    {
    if ( iBody != NULL )
        {
        delete iBody;
        }
    }


// -----------------------------------------------------------------------------
// CTtsUtility::OpenPluginL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenPluginL( TUid aUid )
    {
    iBody->OpenPluginL( aUid );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::ListPluginsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::ListPluginsL( RArray<TUid>& aUids )
    {
    iBody->ListPluginsL( aUids );
    }
                
// -----------------------------------------------------------------------------
// CTtsUtility::SetDefaultStyleL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::SetDefaultStyleL( const TTtsStyle& aStyle )
    {
    iBody->SetDefaultStyleL( aStyle );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::DefaultStyleL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyle& CTtsUtility::DefaultStyleL()
    {
    return iBody->DefaultStyleL();
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::SetSpeakingRateL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::SetSpeakingRateL( TInt aRate )
    {
    iBody->SetSpeakingRateL( aRate );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::SpeakingRateL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::SpeakingRateL()
    {
    return iBody->SpeakingRateL();
    }
     
// -----------------------------------------------------------------------------
// CTtsUtility::OpenDesL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenDesL( const TDesC& aDescriptor )
    {
    iBody->OpenDesL( aDescriptor );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::OpenAndPlayDesL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenAndPlayDesL( const TDesC& aDescriptor )
    {
    iBody->OpenAndPlayDesL( aDescriptor );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::GetSupportedLanguagesL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::GetSupportedLanguagesL( RArray<TLanguage>& aLanguages )
    {
    iBody->GetSupportedLanguagesL( aLanguages );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::GetSupportedVoicesL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::GetSupportedVoicesL( TLanguage aLanguage, 
                                                RArray<TTtsStyle>& aVoices )
    {
    iBody->GetSupportedVoicesL( aLanguage, aVoices );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::SetOutputFileL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::SetOutputFileL( const RFile& aFileName )
    {
    iBody->SetOutputFileL( aFileName );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::AddStyleL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyleID CTtsUtility::AddStyleL( const TTtsStyle& aStyle )
    {
    return iBody->AddStyleL( aStyle );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::DeleteStyle
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::DeleteStyle( TTtsStyleID aID )
    {
    return iBody->DeleteStyle( aID );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::NumberOfStyles
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUint16 CTtsUtility::NumberOfStyles()
    {
    return iBody->NumberOfStyles();
    }

// -----------------------------------------------------------------------------
// CTtsUtility::StyleL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyle& CTtsUtility::StyleL( TTtsStyleID aStyleID )
    {
    return iBody->StyleL( aStyleID );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::StyleL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyle& CTtsUtility::StyleL( TUint16 aIndex )
    {
    return iBody->StyleL( aIndex );
    }


// -----------------------------------------------------------------------------
// CTtsUtility::Play
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::Play()
    {
    iBody->Play();
    }

// -----------------------------------------------------------------------------
// CTtsUtility::Stop
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::Stop()
    {
    iBody->Stop();
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::SetVolume
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::SetVolume( TInt aVolume )
    {
    iBody->SetVolume( aVolume );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::SetRepeats
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::SetRepeats( TInt aRepeatNumberOfTimes, 
                                       const TTimeIntervalMicroSeconds& aTrailingSilence )
    {
    iBody->SetRepeats( aRepeatNumberOfTimes, aTrailingSilence );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::Duration
// 
// -----------------------------------------------------------------------------
//
EXPORT_C const TTimeIntervalMicroSeconds& CTtsUtility::Duration()
    {
    return iBody->Duration();
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::MaxVolume
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::MaxVolume()
    {
    return iBody->MaxVolume();
    }
        

// -----------------------------------------------------------------------------
// CTtsUtility::OpenAndPlayDesL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenAndPlayDesL( const TDesC8& aDescriptor )
    {
    iBody->OpenAndPlayDesL( aDescriptor );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::OpenAndPlayFileL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenAndPlayFileL( const TDesC& aFileName )
    {
    iBody->OpenAndPlayFileL( aFileName );
    }


// -----------------------------------------------------------------------------
// CTtsUtility::OpenAndPlayParsedTextL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenAndPlayParsedTextL( CTtsParsedText& aText )
    {
    iBody->OpenAndPlayParsedTextL( aText );
    }


// -----------------------------------------------------------------------------
// CTtsUtility::OpenFileL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenFileL( const TDesC& aFileName )
    {
    iBody->OpenFileL( aFileName );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::OpenDesL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenDesL( const TDesC8& aDescriptor )
    {
    iBody->OpenDesL( aDescriptor );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::OpenParsedTextL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::OpenParsedTextL( CTtsParsedText& aText )
    {
    iBody->OpenParsedTextL( aText );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::Pause
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::Pause()
    {
    return iBody->Pause();
    }   
        
// -----------------------------------------------------------------------------
// CTtsUtility::Close
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::Close()
    {
    iBody->Close();
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::GetPosition
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::GetPosition( TTimeIntervalMicroSeconds& aPosition )
    {
    return iBody->GetPosition( aPosition );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::GetPosition
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::GetPosition( TInt& aWordIndex )
    {
    return iBody->GetPosition( aWordIndex );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::SetPosition
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::SetPosition( const TTimeIntervalMicroSeconds& aPosition )
    {
    iBody->SetPosition( aPosition );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::SetPosition
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::SetPosition( TInt aWordIndex )
    {
    iBody->SetPosition( aWordIndex );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::SetPriority
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::SetPriority( TInt aPriority, TMdaPriorityPreference aPref )
    {
    return iBody->SetPriority( aPriority, aPref );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::GetVolume
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::GetVolume( TInt& aVolume )
    {
    return iBody->GetVolume( aVolume );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::SetBalance
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::SetBalance( TInt aBalance )
    {
    return iBody->SetBalance( aBalance );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::GetBalance
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::GetBalance( TInt& aBalance )
    {
    return iBody->GetBalance( aBalance );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtility::CustomCommandSync
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                              TInt aFunction, 
                                              const TDesC8& aDataTo1, 
                                              const TDesC8& aDataTo2, 
                                              TDes8& aDataFrom )
    {
    return iBody->CustomCommandSync( aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::CustomCommandSync
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsUtility::CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                              TInt aFunction, 
                                              const TDesC8& aDataTo1, 
                                              const TDesC8& aDataTo2 )
    {
    return iBody->CustomCommandSync( aDestination, aFunction, aDataTo1, aDataTo2 );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::CustomCommandAsync
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination,  
                                               TInt aFunction, 
                                               const TDesC8& aDataTo1, 
                                               const TDesC8& aDataTo2, 
                                               TDes8& aDataFrom, 
                                               TRequestStatus& aStatus )
    {
    iBody->CustomCommandAsync( aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus );
    }

// -----------------------------------------------------------------------------
// CTtsUtility::CustomCommandAsync
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsUtility::CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination, 
                                               TInt aFunction, 
                                               const TDesC8& aDataTo1, 
                                               const TDesC8& aDataTo2, 
                                               TRequestStatus& aStatus )
    {
    iBody->CustomCommandAsync( aDestination, aFunction, aDataTo1, aDataTo2, aStatus );
    }

// End of File  
