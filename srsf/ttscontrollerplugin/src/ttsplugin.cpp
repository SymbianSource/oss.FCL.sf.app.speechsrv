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
* Description:  Implementations for CTtsControllerPlugin methods.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <implementationproxy.h>
#include <mmfcontrollerframeworkbase.h>
#include <nssttscustomcommandparser.h>
#include <mmfaudio.h>
#include "ttsplugin.h"
#include "ttspluginbody.h"
#include "ttspluginimplementationuids.hrh"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::CTtsControllerPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTtsControllerPlugin::CTtsControllerPlugin() : iBody( NULL )
    {
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::ConstructL()
    {
    // Implementation body
    iBody = CTtsControllerPluginBody::NewL( this );

    CMMFAudioControllerCustomCommandParser* audConParser = CMMFAudioControllerCustomCommandParser::NewL( *this );
    CleanupStack::PushL( audConParser );
    // Parser now owned by controller framework
    AddCustomCommandParserL( *audConParser ); 
    CleanupStack::Pop( audConParser );

    CMMFAudioPlayDeviceCustomCommandParser* audPlayDevParser = CMMFAudioPlayDeviceCustomCommandParser::NewL( *this );
    CleanupStack::PushL( audPlayDevParser );
    // Parser now owned by controller framework
    AddCustomCommandParserL( *audPlayDevParser );
    CleanupStack::Pop();//audPlayDevParser

    CTtsCustomCommandParser* ttsParser = CTtsCustomCommandParser::NewL( *this );
    CleanupStack::PushL( ttsParser );
    // Parser now owned by controller framework
    AddCustomCommandParserL( *ttsParser );
    CleanupStack::Pop();//ttsParser

    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTtsControllerPlugin* CTtsControllerPlugin::NewL()
    {
    CTtsControllerPlugin* self = new( ELeave ) CTtsControllerPlugin;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::~CTtsControllerPlugin
// Destructor.
// -----------------------------------------------------------------------------
//
CTtsControllerPlugin::~CTtsControllerPlugin()
    {
    if ( iBody != NULL )
        {
        delete iBody;
        }
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::InitializationReady
//
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::InitializationReady( TInt aError )
    {
    TMMFEvent event;
    event.iEventType = KMMFEventCategoryAudioLoadingComplete;
    event.iErrorCode = aError;

    DoSendEventToClient( event );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::SynthesisReady
//
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::SynthesisReady( TInt aError )
    {
    TMMFEvent event;
    event.iEventType = KMMFEventCategoryPlaybackComplete;
    event.iErrorCode = aError;

    DoSendEventToClient( event );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::AddDataSourceL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::AddDataSourceL( MDataSource& aDataSource )
    {
    iBody->AddDataSourceL( aDataSource );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::AddDataSinkL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::AddDataSinkL( MDataSink& aDataSink )
    {
    iBody->AddDataSinkL( aDataSink );
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::RemoveDataSourceL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::RemoveDataSourceL( MDataSource& aDataSource )
    {
    iBody->RemoveDataSourceL( aDataSource );
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::RemoveDataSinkLn
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::RemoveDataSinkL( MDataSink& aDataSink )
    {
    iBody->RemoveDataSinkL( aDataSink );
    }
  
      
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::ResetL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::ResetL()
    {
    iBody->ResetL();
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::PrimeL
// ?Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::PrimeL()
    {
    iBody->PrimeL();
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::PlayL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::PlayL()
    {
    iBody->PlayL();
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::PauseL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::PauseL()
    {
    iBody->PauseL();
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::StopL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::StopL()
    {
    iBody->StopL();
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::PositionL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CTtsControllerPlugin::PositionL() const
    {
    return iBody->PositionL();
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::SetPositionL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::SetPositionL( const TTimeIntervalMicroSeconds& aPosition )
    {
    iBody->SetPositionL( aPosition );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::DurationL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CTtsControllerPlugin::DurationL() const
    {
    return iBody->DurationL();
    }
       
 
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::SetPrioritySettings
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings )
    {
    iBody->SetPrioritySettings( aPrioritySettings );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::CustomCommand
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::CustomCommand( TMMFMessage& aMessage )
    {
    iBody->CustomCommand( aMessage );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::GetNumberOfMetaDataEntriesL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::GetNumberOfMetaDataEntriesL( TInt& aNumberOfEntries )
    {
    iBody->GetNumberOfMetaDataEntriesL( aNumberOfEntries );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::GetMetaDataEntryL
// Forwards call to implementation object.
// -----------------------------------------------------------------------------
//
CMMFMetaDataEntry* CTtsControllerPlugin::GetMetaDataEntryL( TInt aIndex )
    {
    return iBody->GetMetaDataEntryL( aIndex );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSourceSampleRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSourceSampleRateL( TUint aSampleRate )
    {
    iBody->MacSetSourceSampleRateL( aSampleRate );
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSourceBitRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSourceBitRateL( TUint aBitRate )
    {
    iBody->MacSetSourceBitRateL( aBitRate );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSourceNumChannelsL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSourceNumChannelsL( TUint aNumChannels )
    {
    iBody->MacSetSourceNumChannelsL( aNumChannels );
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSourceFormatL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSourceFormatL( TUid aFormatUid )
    {
    iBody->MacSetSourceFormatL( aFormatUid );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSourceDataTypeL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSourceDataTypeL( TFourCC aDataType )
    {
    iBody->MacSetSourceDataTypeL( aDataType );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSinkSampleRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSinkSampleRateL( TUint aSampleRate )
    {
    iBody->MacSetSinkSampleRateL( aSampleRate );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSinkBitRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSinkBitRateL( TUint aRate )
    {
    iBody->MacSetSinkBitRateL( aRate );
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSinkNumChannelsL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSinkNumChannelsL( TUint aNumChannels )
    {
    iBody->MacSetSinkNumChannelsL( aNumChannels );
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSinkFormatL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSinkFormatL( TUid aFormatUid )
    {
    iBody->MacSetSinkFormatL( aFormatUid );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetSinkDataTypeL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetSinkDataTypeL(  TFourCC aDataType )
    {
    iBody->MacSetSinkDataTypeL( aDataType );
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacSetCodecL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacSetCodecL( TFourCC aSourceDataType,
                                         TFourCC aSinkDataType )
    {
    iBody->MacSetCodecL( aSourceDataType, aSinkDataType );
    }
        
        
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSourceSampleRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSourceSampleRateL( TUint& aRate )
    {
    iBody->MacGetSourceSampleRateL( aRate );
    }
       
 
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSourceBitRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSourceBitRateL( TUint& aRate )
    {
    iBody->MacGetSourceBitRateL( aRate );
    }
      
  
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSourceNumChannelsL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSourceNumChannelsL( TUint& aNumChannels )
    {
    iBody->MacGetSourceNumChannelsL( aNumChannels );
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSourceFormatL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
// 
void CTtsControllerPlugin::MacGetSourceFormatL( TUid& aFormat )
    {
    iBody->MacGetSourceFormatL( aFormat );
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSourceDataTypeL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSourceDataTypeL( TFourCC& aDataType )
    {
    iBody->MacGetSourceDataTypeL( aDataType );
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSinkSampleRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSinkSampleRateL( TUint& aRate )
    {
    iBody->MacGetSinkSampleRateL( aRate );
    }
    
    
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSinkBitRateL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSinkBitRateL( TUint& aRate )
    {
    iBody->MacGetSinkBitRateL( aRate );
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSinkNumChannelsL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSinkNumChannelsL( TUint& aNumChannels )
    {
    iBody->MacGetSinkNumChannelsL( aNumChannels );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSinkFormatL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSinkFormatL( TUid& aFormat )
    {
    iBody->MacGetSinkFormatL( aFormat );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSinkDataTypeL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSinkDataTypeL( TFourCC& aDataType )
    {
    iBody->MacGetSinkDataTypeL( aDataType );
    }
       
 
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSourceSampleRatesL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSourceSampleRatesL( RArray<TUint>& aSupportedRates )
    {
    iBody->MacGetSupportedSourceSampleRatesL( aSupportedRates );
    }
     
   
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSourceBitRatesL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSourceBitRatesL( RArray<TUint>& aSupportedRates )
    {
    iBody->MacGetSupportedSourceBitRatesL( aSupportedRates );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSourceNumChannelsL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSourceNumChannelsL( RArray<TUint>& aSupportedChannels )
    {
    iBody->MacGetSupportedSourceNumChannelsL( aSupportedChannels );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSourceDataTypesL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSourceDataTypesL( RArray<TFourCC>& aSupportedDataTypes )
    {
    iBody->MacGetSupportedSourceDataTypesL( aSupportedDataTypes );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSinkSampleRatesL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSinkSampleRatesL( RArray<TUint>& aSupportedRates )
    {
    iBody->MacGetSupportedSinkSampleRatesL( aSupportedRates );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSinkBitRatesL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSinkBitRatesL( RArray<TUint>& aSupportedRates )
    {
    iBody->MacGetSupportedSinkBitRatesL( aSupportedRates );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSinkNumChannelsL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSinkNumChannelsL( RArray<TUint>& aSupportedChannels )
    {
    iBody->MacGetSupportedSinkNumChannelsL( aSupportedChannels );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MacGetSupportedSinkDataTypesL
// Not supported by TTS controller plugin.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MacGetSupportedSinkDataTypesL( RArray<TFourCC>& aSupportedDataTypes )
    {
    iBody->MacGetSupportedSinkDataTypesL( aSupportedDataTypes );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MapdSetVolumeL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MapdSetVolumeL( TInt aVolume ) 
    {
    iBody->MapdSetVolumeL( aVolume );
    }
       
 
// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MapdGetMaxVolumeL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MapdGetMaxVolumeL( TInt& aMaxVolume )
    {
    iBody->MapdGetMaxVolumeL( aMaxVolume );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MapdGetVolumeL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MapdGetVolumeL( TInt& aVolume )
    {
    iBody->MapdGetVolumeL( aVolume );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MapdSetVolumeRampL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MapdSetVolumeRampL( const TTimeIntervalMicroSeconds& aRampDuration )
    {
    iBody->MapdSetVolumeRampL( aRampDuration );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MapdSetBalanceL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MapdSetBalanceL( TInt aBalance )
    {
    iBody->MapdSetBalanceL( aBalance );
    }
        

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MapdGetBalanceL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MapdGetBalanceL( TInt& aBalance )
    {
    iBody->MapdGetBalanceL( aBalance );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscAddStyleL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
TTtsStyleID CTtsControllerPlugin::MttscAddStyleL( const TTtsStyle& aStyle )
    {
    return iBody->MttscAddStyleL( aStyle );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscDeleteStyle
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
TInt CTtsControllerPlugin::MttscDeleteStyle( TTtsStyleID aID )
    {
    return iBody->MttscDeleteStyle( aID );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscGetPositionL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MttscGetPositionL( TInt& aWordIndex )
    {
    iBody->MttscGetPositionL( aWordIndex );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscNumberOfStyles
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
TUint16 CTtsControllerPlugin::MttscNumberOfStyles()
    {
    return iBody->MttscNumberOfStyles();
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscOpenParsedTextL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MttscOpenParsedTextL( CTtsParsedText& aText )
    {
    iBody->MttscOpenParsedTextL( aText );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscSetPositionL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MttscSetPositionL( TInt aWordIndex )
    {
    iBody->MttscSetPositionL( aWordIndex );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscAddStyleL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsControllerPlugin::MttscStyleL( TTtsStyleID aStyleID )
    {
    return iBody->MttscStyleL( aStyleID );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscStyleL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsControllerPlugin::MttscStyleL( TUint16 aIndex )
    {
    return iBody->MttscStyleL( aIndex );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscSetDefaultStyleL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MttscSetDefaultStyleL( const TTtsStyle& aStyle )
    {
    iBody->MttscSetDefaultStyleL( aStyle );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscDefaultStyleL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsControllerPlugin::MttscDefaultStyleL()
    {
    return iBody->MttscDefaultStyleL();
    }

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscSetSpeakingRateL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MttscSetSpeakingRateL( TInt aRate )
    {
    iBody->MttscSetSpeakingRateL( aRate );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscSpeakingRateL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
TInt CTtsControllerPlugin::MttscSpeakingRateL()
    {
    return iBody->MttscSpeakingRateL();
    }

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscGetSupportedLanguagesL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MttscGetSupportedLanguagesL( RArray<TLanguage>& aLanguages )
    {
    return iBody->MttscGetSupportedLanguagesL( aLanguages );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPlugin::MttscGetSupportedVoicesL
// Forward call to the implementation object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPlugin::MttscGetSupportedVoicesL( TLanguage aLanguage, 
                                                     RArray<TTtsStyle>& aVoices )
    {
    iBody->MttscGetSupportedVoicesL( aLanguage, aVoices );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


/**
* ImplementationTable
*/
const TImplementationProxy ImplementationTable[] =
    {
        {{ KTtsImplementationUid }, ( TProxyNewLPtr ) CTtsControllerPlugin::NewL }
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Needed for MMF Controller Plugin goodness.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

// End of File  
