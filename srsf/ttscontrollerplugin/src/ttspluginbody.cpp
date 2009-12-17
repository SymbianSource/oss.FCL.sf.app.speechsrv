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
* Description:  Implementations for CTtsControllerPluginBody methods.
*
*/


// INCLUDE FILES
#include <AudioPreference.h>
#include <e32base.h>
#include <mmfdes.h>
#include <mmffile.h>
#include <reent.h>
#include <sysutil.h>
#include <utf.h>

#include "ttsplugin.h"
#include "ttspluginbody.h"
#include "ttsplugindataloader.h"
#include "ttspluginresourcehandler.h"
#include "rubydebug.h"
#include "srsfbldvariant.hrh"

// CONSTANTS
// Playback state
const TInt KPlay = 1; 

// Package types for general & language specific TTP data
// const TUint32 KTtpGeneralDataPackageType = 1;
const TUint32 KTtpLanguageDataPackageType = 0;
const TUint32 KTtsDataPackageType = 2;

// Header used to direct input to us from CMdaAudioPlayerUtility
_LIT8( KTtsContentHeader, "(tts)" );

// TTS language can be set by adding "(l:lang_id)" to the beginning of descriptor
// or file input. 
_LIT8( KTtsLanguageStartTag, "(l:" );
_LIT8( KTtsLanguageEndTag, ")" );

_LIT( KTextFileExtension, ".txt" );

const TInt KSamplingRate = 16000; // Hz

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::CTtsControllerPluginBody
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTtsControllerPluginBody::CTtsControllerPluginBody( CTtsControllerPlugin* aObserver ) : 
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iTtsOutputMode( EDevTTSSoundDeviceMode ) 
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::ConstructL()
    {
    RUBY_DEBUG1( "CTtsControllerPluginBody::ConstructL [%x]", this );

    User::LeaveIfError( iFs.Connect() );

    // Loads resource file
    CTtsPluginResourceHandler* resourceHandler = 
        CTtsPluginResourceHandler::NewL( iFs );

    TFileName generalTTPFileName( *resourceHandler->iTtpGeneralFilename );
    TFileName languageTTPFileName( *resourceHandler->iTtpLanguangeFilename );
    TFileName postfixOfTTPFileName( *resourceHandler->iTtpFilenamePostfix );
    TFileName TTSFileName( *resourceHandler->iTtsFilename );
    TFileName postfixOfTTSFileName( *resourceHandler->iTtsFilenamePostfix );

    delete resourceHandler; 
    resourceHandler = NULL;
    
    // TTP Data loader
    iTtsLoader = CTTSDataLoader::NewL( iFs,
                                       generalTTPFileName, 
                                       languageTTPFileName, 
                                       postfixOfTTPFileName,
                                       TTSFileName,
                                       postfixOfTTSFileName );

    // DevTTS
    iDevTts = CDevTTS::NewL( *this );

    // Add default style to DevTTS
    iDefaultStyleID = iDevTts->AddStyleL( iDefaultStyle );

    // Add ourselves to active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTtsControllerPluginBody* CTtsControllerPluginBody::NewL( CTtsControllerPlugin* aObserver )
    {
    CTtsControllerPluginBody* self = new( ELeave ) CTtsControllerPluginBody( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::~CTtsControllerPluginBody
// Destructor.
// -----------------------------------------------------------------------------
//
CTtsControllerPluginBody::~CTtsControllerPluginBody()
    {
    delete iParsedTextPointer;
    delete iDevTts;
    delete iTtsLoader;
    
    iFs.Close();
    
    CloseSTDLIB();
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::AddDataSourceL
// Adds data suorce.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::AddDataSourceL( MDataSource& aDataSource )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::AddDataSourceL" );

    iEarlyPrimingDone = EFalse;

    // Check the source type
    if ( aDataSource.DataSourceType() == KUidMmfDescriptorSource )
        {
        CMMFDescriptor* des = STATIC_CAST( CMMFDescriptor*, &aDataSource );

        RUBY_DEBUG1( "Descriptor source detected, size: %x bytes", des->Size() );

        // Copy data to CMMFDescriptorBuffer
        CMMFDescriptorBuffer* desBuf = CMMFDescriptorBuffer::NewL( des->Size() );
        CleanupStack::PushL( desBuf );
        des->ReadBufferL( desBuf, 0 );
      
        // To 'normal' descriptor
        TDes8& descriptor = desBuf->Data();

        RUBY_DEBUG1( "Created descriptor, size: %x", descriptor.Size() );

        ConstructParsedTextL( descriptor );

        CleanupStack::PopAndDestroy( desBuf );
          
        DoEarlyPrimingL();
        }
    else if ( aDataSource.DataSourceType() == KUidMmfFileSource )
        {
        RUBY_DEBUG0( "File source detected" );
        
        CMMFFile* mmfFile = STATIC_CAST( CMMFFile*, &aDataSource );
        
        RUBY_DEBUG1( "Filename: %s", mmfFile->FullName().Ptr() );
        
        // Check that correct filename extension is used
        if ( mmfFile->FullName().Find( KTextFileExtension ) == KErrNotFound )
            {
            User::Leave( KErrNotSupported );
            }
        
        // Open file first if not yet open
        mmfFile->SourcePrimeL();
        RFile file( mmfFile->FileL() );
        
        // Find the size of tts input file
        TInt fileSize( 0 );
        User::LeaveIfError( file.Size( fileSize ) );
        
        // Create buffer for data
        HBufC8* fileBuffer = HBufC8::NewLC( fileSize );
                
        // Make sure we are in the beginnig of the file
        TInt tmp( 0 );
        file.Seek( ESeekStart, tmp );
        
        // Read data
        TPtr8 bufferPtr( fileBuffer->Des() );
        User::LeaveIfError( file.Read( bufferPtr ) );
        
        ConstructParsedTextL( bufferPtr );
        
        // No need to store the 8-bit text
        CleanupStack::PopAndDestroy( fileBuffer );
        
        DoEarlyPrimingL();    
        }
    else
        {
        RUBY_ERROR0( "Unknown source!" );
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::AddDataSinkL
// Adds data sink.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::AddDataSinkL( MDataSink& aDataSink )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::AddDataSinkL" );
    
    if ( aDataSink.DataSinkType() == KUidMmfAudioOutput )
        {
        iTtsOutputMode = EDevTTSSoundDeviceMode;
        }
    else if ( aDataSink.DataSinkType() == KUidMmfFileSink )
        {
        iTtsOutputMode = EDevTTSClientMode;
        
        CMMFFile* mmfFile = STATIC_CAST( CMMFFile*, &aDataSink );

        mmfFile->SinkPrimeL(); 
        iOutputFile = &mmfFile->FileL(); 

        TDriveInfo driveInfo;    
        User::LeaveIfError( iOutputFile->Drive( iOutFileDriveNumber, driveInfo ) );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::RemoveDataSourceL
// Removes data source.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::RemoveDataSourceL( MDataSource& /*aDataSource*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::RemoveDataSourceL" );
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::RemoveDataSinkL
// Removes data sink.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::RemoveDataSinkL( MDataSink& /*aDataSink*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::RemoveDataSinkL" );
    
    if ( iOutputFile )
        {
        iOutputFile->Close(); 
        iOutputFile = NULL; 
        }
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::ResetL
// Resets plugin & DevTTS to starting state.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::ResetL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::ResetL" );

    iDevTts->Stop();
    delete iDevTts;
    iDevTts = NULL;
    iDevTts = CDevTTS::NewL( *this );
    
    // Add default style to DevTTS
    iDefaultStyleID = iDevTts->AddStyleL( iDefaultStyle );

    delete iParsedTextPointer;
    iParsedTextPointer = NULL;
    iEarlyPrimingDone = EFalse;
    
    if ( iOutputFile )
        {
        iOutputFile->Close(); 
        iOutputFile = NULL; 
        }
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::PrimeL
// Initializes synthesis. 
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::PrimeL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::PrimeL" );
    TInt error( KErrNone );

    if ( iEarlyPrimingDone )
        {
        iEarlyPrimingDone = EFalse;
        }
    else
        {
        // Make sure that parsed text contains at least something to synthesize
        if ( iParsedTextPointer->Text() != KNullDesC  || 
             iParsedTextPointer->PhonemeSequence() != KNullDesC8 ) 
            {
            TRAP( error, iDevTts->PrimeSynthesisL( *iParsedTextPointer ) );
            }
        }
    iObserver->InitializationReady( error );        
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::DoEarlyPrimingL
// Initializes synthesis. 
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::DoEarlyPrimingL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::DoEarlyPrimingL" );

    // Make sure that parsed text contains at least something to synthesize
    if ( iParsedTextPointer->Text() != KNullDesC  || 
         iParsedTextPointer->PhonemeSequence() != KNullDesC8 )
        {
        iDevTts->PrimeSynthesisL( *iParsedTextPointer );
        }
    iEarlyPrimingDone = ETrue;    
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::PlayL
// Starts playback.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::PlayL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::PlayL" );

    if ( IsActive() )
		{
		User::Leave( KErrServerBusy );
		}

	iRequestFunction = KPlay;
    Ready();
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::HandlePlay
// Async handler for play
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::HandlePlay()
    {
    // Make sure that parsed text contains at least something to synthesize
    if ( iParsedTextPointer->Text() != KNullDesC || 
         iParsedTextPointer->PhonemeSequence() != KNullDesC8 )
        {
        
        TRAPD( error, iDevTts->SynthesizeL( iTtsOutputMode ) );
        // Do client callback now if leave happens, otherwise wait for the callback 
        // from DevTTS
        if ( error != KErrNone )
            {
            // Try if we can get rid of the problem by first doing initialization
            TRAP( error, iDevTts->PrimeSynthesisL( *iParsedTextPointer ) );
            if ( error != KErrNone )
                {
                iObserver->SynthesisReady( error );
                }
            // Then try to start synthesis again
            TRAP( error, iDevTts->SynthesizeL( iTtsOutputMode ) );
            if ( error != KErrNone )
                {
                iDevTts->Stop();
                iObserver->SynthesisReady( error );
                }
            }
        }
    else
        {
        iObserver->SynthesisReady( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::PauseL
// Pauses playback & synthesis.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::PauseL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::PauseL" );

    iDevTts->Pause();
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::StopL
// Stops playback & synthesis.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::StopL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::StopL" );
    iEarlyPrimingDone = EFalse;
    iDevTts->Stop();
    
    if ( iOutputFile )
        {
        iOutputFile->Close();
        iOutputFile = NULL; 
        }
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::PositionL
// Returns the current playback position in microseconds.
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CTtsControllerPluginBody::PositionL() const
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::PositionL" );

    TTimeIntervalMicroSeconds time( 0 );

    iDevTts->GetPositionL( time );

    return time;
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::SetPositionL
// Sets the playback position.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::SetPositionL( const TTimeIntervalMicroSeconds& /*aPosition*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::SetPositionL" );
    // @todo Remove from comments when DevTTS does not anymore return 
    // KErrNotSupported
    // iDevTts->SetPositionL( aPosition );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::DurationL
// Returns duration of synthesized output.
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CTtsControllerPluginBody::DurationL() const
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::DurationL" );

    TTimeIntervalMicroSeconds duration( 0 );

    if ( iParsedTextPointer )
        {
        // Count overall duration
        for ( TInt i = 0; i < iParsedTextPointer->NumberOfSegments(); i++ )
            {
            TTtsSegment segment = iParsedTextPointer->SegmentL( i );
            TTtsStyleID styleID = segment.StyleID();
            TTtsStyle style = iDevTts->StyleL( styleID );
            duration = duration.Int64() + style.iDuration.Int();
            }
        }

    return duration;
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::SetPrioritySettings
// Sets the playback priority.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings )
    {
    RUBY_DEBUG0( "CTtsControllerPluginBody::SetPrioritySettings" );

    iDevTts->SetAudioPriority( aPrioritySettings.iPriority, 
                               (TDevTTSAudioPreference)aPrioritySettings.iPref );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::CustomCommand
// One supported custom command at the moment.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::CustomCommand( TMMFMessage& aMessage )
    {
    RUBY_DEBUG0( "CTtsControllerPluginBody::CustomCommand" );
    
    TInt result( KErrNone );

    if ( aMessage.Function() == ETtsCustomCommandSetAudioOutput )
        {
        TBuf8<2> data;
        
        if ( aMessage.SizeOfData1FromClient() <= 2  && 
             aMessage.ReadData1FromClient( data ) == KErrNone )
            {
            TLex8 parser( data );

            TInt newOutput;
            result = parser.Val( newOutput );
            
            if ( result == KErrNone )
                {
                TRAP( result, iDevTts->SetAudioOutputL( newOutput) );
                }
            }
        else
            {
            result = KErrArgument;
            }
        }
    else
        {
        result = KErrNotSupported; 
        }
        
    aMessage.Complete( result );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::GetNumberOfMetaDataEntriesL
// Not supported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::GetNumberOfMetaDataEntriesL( TInt& /*aNumberOfEntries*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::GetNumberOfMetaDataEntriesL" );
 
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::GetMetaDataEntryL
// Not supported.
// -----------------------------------------------------------------------------
//
CMMFMetaDataEntry* CTtsControllerPluginBody::GetMetaDataEntryL( TInt /*aIndex*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::GetMetaDataEntryL" );

    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSourceSampleRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSourceSampleRateL( TUint /*aSampleRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSourceBitRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSourceBitRateL( TUint /*aBitRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSourceNumChannelsL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSourceNumChannelsL( TUint /*aNumChannels*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSourceFormatL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSourceFormatL( TUid /*aFormatUid*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSourceDataTypeL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSourceDataTypeL( TFourCC /*aDataType*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSinkSampleRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSinkSampleRateL( TUint /*aSampleRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSinkBitRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSinkBitRateL( TUint /*aRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSinkNumChannelsL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSinkNumChannelsL( TUint /*aNumChannels*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSinkFormatL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSinkFormatL( TUid /*aFormatUid*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetSinkDataTypeL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetSinkDataTypeL( TFourCC /*aDataType*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacSetCodecL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacSetCodecL( TFourCC /*aSourceDataType*/, TFourCC /*aSinkDataType*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSourceSampleRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSourceSampleRateL( TUint& /*aRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSourceBitRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSourceBitRateL( TUint& /*aRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSourceNumChannelsL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSourceNumChannelsL( TUint& /*aNumChannels*/ )
    { 
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSourceFormatL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
// 
void CTtsControllerPluginBody::MacGetSourceFormatL( TUid& /*aFormat*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSourceDataTypeL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSourceDataTypeL( TFourCC& /*aDataType*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSinkSampleRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSinkSampleRateL( TUint& /*aRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSinkBitRateL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSinkBitRateL( TUint& /*aRate*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSinkNumChannelsL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSinkNumChannelsL( TUint& /*aNumChannels*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSinkFormatL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSinkFormatL( TUid& /*aFormat*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSinkDataTypeL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSinkDataTypeL( TFourCC& /*aDataType*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSourceSampleRatesL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSourceSampleRatesL( RArray<TUint>& /*aSupportedRates*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSourceBitRatesL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSourceBitRatesL( RArray<TUint>& /*aSupportedRates*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSourceNumChannelsL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSourceNumChannelsL( RArray<TUint>& /*aSupportedChannels*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSourceDataTypesL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSourceDataTypesL( RArray<TFourCC>& /*aSupportedDataTypes*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSinkSampleRatesL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSinkSampleRatesL( RArray<TUint>& /*aSupportedRates*/)
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSinkBitRatesL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSinkBitRatesL( RArray<TUint>& /*aSupportedRates*/)
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSinkNumChannelsL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSinkNumChannelsL( RArray<TUint>& /*aSupportedChannels*/ )
    {
    User::Leave( KErrNotSupported );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MacGetSupportedSinkDataTypesL
// Leaves with value KErrNotSupported.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MacGetSupportedSinkDataTypesL( RArray<TFourCC>& /*aSupportedDataTypes*/ )
    {
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MapdSetVolumeL
// Sets playback volume.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MapdSetVolumeL( TInt aVolume ) 
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MapdSetVolumeL" );

    iDevTts->SetVolume( aVolume );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MapdGetMaxVolumeL
// Returns the maximum volume setting.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MapdGetMaxVolumeL( TInt& aMaxVolume )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MapdGetMaxVolumeL" );

    aMaxVolume = iDevTts->MaxVolume();
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MapdGetVolumeL
// Returns the current volume.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MapdGetVolumeL( TInt& aVolume )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MapdGetVolumeL" );

    aVolume = iDevTts->Volume();
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MapdSetVolumeRampL
// Sets volume ramp time.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MapdSetVolumeRampL( const TTimeIntervalMicroSeconds& aRampDuration )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MapdSetVolumeRampL" );

    iDevTts->SetVolumeRamp( aRampDuration );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MapdSetBalanceL
// Sets playback balance.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MapdSetBalanceL( TInt aBalance )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MapdSetBalanceL" );

    iDevTts->SetBalance( aBalance );
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MapdGetBalanceL
// Returns current balance setting.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MapdGetBalanceL( TInt& aBalance )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MapdGetBalanceL" );

    aBalance = iDevTts->Balance();
    }

// ================= FUNCTIONS FROM MDevTTSObserver start ======================


// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MdtoConfigurationData
// Called when DevTTS needs data.
// -----------------------------------------------------------------------------
//
HBufC8* CTtsControllerPluginBody::MdtoConfigurationData( TUint32 aPackageType, 
                                                         TUint32 aPackageID,
                                                         TUint32 aStartPosition, 
                                                         TUint32 aEndPosition )
    {
    RUBY_DEBUG2( "CTtsControllerPluginBody::MdtoConfigurationData [%d][%d]", aPackageType, aPackageID );

    if ( aPackageType == KTtpLanguageDataPackageType )
        {
        return iTtsLoader->LoadLanguageData( aPackageType, (TLanguage)aPackageID,
                                             aStartPosition, aEndPosition );
        }
    else if ( aPackageType == KTtsDataPackageType )
        {
        return iTtsLoader->LoadTtsData( aPackageType, aPackageID, 
                                        aStartPosition, aEndPosition );
        }
    else
        {
        return iTtsLoader->LoadGeneralData( aPackageType, aPackageID, 
                                            aStartPosition, aEndPosition );
        }
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MdtoEvent
// Called when synthesis has ended.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MdtoEvent( TDevTTSEvent aEvent, TInt aStatus )
    {
    RUBY_DEBUG2( "CTtsControllerPluginBody::MdtoEvent [%d][%d]", aEvent, aStatus );

    switch ( aEvent )
        {
        case EDevTTSEventComplete:
        
            if ( iOutputFile )
                {
                iOutputFile->Close(); 
                iOutputFile = NULL; 
                }
                
            // Call the upper level
            iObserver->SynthesisReady( aStatus );
            break;

        default:
            break;
        }
    }
        
// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MdtoProcessBuffer
// Called when synthesis is directed to file and the next output buffer is ready.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MdtoProcessBuffer( const TDesC8& aBuffer )
    {
    RUBY_DEBUG0( "CTtsControllerPluginBody::MdtoProcessBuffer" );
    
    TInt error( KErrCorrupt ); 
    
    if ( iOutputFile )
        {
        TBool diskFull( EFalse );
        
        TRAP( error, diskFull = 
            SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aBuffer.Size(), 
                                                   iOutFileDriveNumber ) );
        if ( !error )
            {
            if ( diskFull )
                {
                error = KErrDiskFull; 
                }
            else
                {
                error = iOutputFile->Write( aBuffer );
                }
            }
        }
    
    if ( error )
        {
        RUBY_ERROR1( "CTtsControllerPluginBody::MdtoProcessBuffer - Write failed with %d",
                      error );
        
        // Stop synthesis
        TRAP_IGNORE( StopL() );
        
        // Inform observer we don't continue
        iObserver->SynthesisReady( error );
        }

    // We are ready to receive the next buffer. 
    iDevTts->BufferProcessed( aBuffer ); 
    }
        
// ================== FUNCTIONS FROM MDevTTSObserver end =======================


// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscAddStyleL
// Adds a new style to style collection.
// -----------------------------------------------------------------------------
//
TTtsStyleID CTtsControllerPluginBody::MttscAddStyleL( const TTtsStyle& aStyle )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscAddStyleL" );

    return iDevTts->AddStyleL( aStyle );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscDeleteStyle
// Removes style from style collection.
// -----------------------------------------------------------------------------
//
TInt CTtsControllerPluginBody::MttscDeleteStyle( TTtsStyleID aID )
    {
    RUBY_DEBUG0( "CTtsControllerPluginBody::MttscDeleteStyle" );

    TRAPD( error, iDevTts->DeleteStyleL( aID ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscGetPositionL
// Returns current synthesis position in words.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MttscGetPositionL( TInt& /*aWordIndex*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscGetPositionL" );

    // @todo DevTTS wants this starting from the beginning of segment
    // iDevTts->GetPositionL( aWordIndex );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscNumberOfStyles
// Returns number of registered styles.
// -----------------------------------------------------------------------------
//
TUint16 CTtsControllerPluginBody::MttscNumberOfStyles()
    {
    RUBY_DEBUG0( "CTtsControllerPluginBody::MttscNumberOfStyles" );

    return iDevTts->NumberOfStyles();
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscOpenParsedTextL
// Opens parsed text object as source.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MttscOpenParsedTextL( CTtsParsedText& aText )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscOpenParsedTextL" );

    // Go through styles inside the parsed text
    for ( TInt i = 0; i < aText.NumberOfSegments(); i++ )
        {
        TTtsSegment segment = aText.SegmentL( i );
        TTtsStyleID styleID = segment.StyleID();
        TTtsStyle& style = iDevTts->StyleL( styleID );
        }

    CopyParsedTextL( aText );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscSetPositionL
// Sets the synthesis position in words.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MttscSetPositionL( TInt /*aWordIndex*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscSetPositionL" );

    // @todo DevTTS returns this from starting of certain segment
    // iDevTts->SetPositionL( aWordIndex );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscStyleL
// Returns reference to registered style based on ID.
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsControllerPluginBody::MttscStyleL( TTtsStyleID aStyleID )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscStyleL(aStyleID)" );

    return iDevTts->StyleL( aStyleID );
    }


// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscStyleL
// Returns reference to registered style based in index.
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsControllerPluginBody::MttscStyleL( TUint16 aIndex )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscStyleL(aIndex)" );

    return iDevTts->StyleL( aIndex );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscSetDefaultStyleL
// Sets the default style
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MttscSetDefaultStyleL( const TTtsStyle& aStyle )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscSetDefaultStyleL" ); 

    // Remove old default style from devTts
    iDevTts->DeleteStyleL( iDefaultStyleID );
    
    // Copy new default style 
    iDefaultStyle = aStyle;
    
    // Add new default style to devTTS
    iDefaultStyleID = iDevTts->AddStyleL( iDefaultStyle );
    
    iUserSpecifiedDefaultStyle = ETrue;
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscDefaultStyleL
// Returns reference to the default style
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsControllerPluginBody::MttscDefaultStyleL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscDefaultStyleL" );
    
    return iDefaultStyle;
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscSetSpeakingRateL
// Not supported
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MttscSetSpeakingRateL( TInt /*aRate*/ )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscSetSpeakingRateL" );
    
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscSpeakingRateL
// Not supported
// -----------------------------------------------------------------------------
//
TInt CTtsControllerPluginBody::MttscSpeakingRateL()
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscSpeakingRateL" );
    
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscGetSupportedLanguagesL
// Appends supported languages to aLanguages 
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MttscGetSupportedLanguagesL( RArray<TLanguage>& aLanguages )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscGetSupportedLanguagesL" );
    
    iTtsLoader->GetSupportedLanguagesL( aLanguages );
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::MttscGetSupportedVoicesL
// Appends supported voices of given language to aVoices
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::MttscGetSupportedVoicesL( TLanguage aLanguage, 
                                                         RArray<TTtsStyle>& aVoices )
    {
    RUBY_DEBUG_BLOCK( "CTtsControllerPluginBody::MttscGetSupportedVoicesL" );
    
    if ( iDevTts->IsLanguageSupported( aLanguage ) )
        {
        TTtsStyle style;
        style.iLanguage     = aLanguage;
        style.iSamplingRate = KSamplingRate;
        style.iRate         = KErrNotSupported;
        style.iVolume       = KTtsDefaultVolume;
        
        style.iVoice = KDefaultVoiceNameMale;
        aVoices.AppendL( style );
        
        style.iVoice = KDefaultVoiceNameFemale;
        aVoices.AppendL( style );
        }
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::DoCancel
// Cancel handle from CActive class.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::DoCancel()
	{
	}

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::RunL
// RunL from CActive class.  Check which message got saved and call the
// appropriate handle function.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::RunL()
	{
	switch ( iRequestFunction )
        {
        case KPlay:
            HandlePlay();
            break;
        default:
            // No action
            break;
        }
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::Ready
// Activates the active object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::Ready()
	{
	TRequestStatus* pRS = &iStatus;
    User::RequestComplete( pRS, KErrNone );
	SetActive();
	}

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::ConstructParsedTextL
// Converts the descriptor to unicode & puts it to parsed text object.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::ConstructParsedTextL( TDes8& aBufferPtr )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // Check if header is there and strip it out if needed
    TInt index = aBufferPtr.Find( KTtsContentHeader );
    if ( index != KErrNotFound )
        {
        aBufferPtr.Replace( index, KTtsContentHeader().Length(), KNullDesC8 );
        }

    TInt languageId( KErrNotFound );
    index = aBufferPtr.Find( KTtsLanguageStartTag );
    if ( index != KErrNotFound )
        {
        RUBY_DEBUG1( "KTtsLanguageStartTag found, index %d", index );
        
        TInt relativeEndIndex( aBufferPtr.Mid( index ).Find( KTtsLanguageEndTag ) );
        
        if ( relativeEndIndex != KErrNotFound )
            {
            const TInt tagLength( KTtsLanguageStartTag().Length() );
            
            TPtrC8 lang( aBufferPtr.Mid( index + tagLength, relativeEndIndex - tagLength ) );
            
            TInt64 tmp;
            TLex8 parser( lang );

            if ( parser.Val( tmp, EDecimal ) == KErrNone )
                {
                languageId = tmp;
                RUBY_DEBUG1( "Detected language selection %d", languageId );
                }
        
            aBufferPtr.Replace( index, relativeEndIndex + 1, KNullDesC8 );
            }
        }

    // Convert text to 16-bit unicode
    HBufC16* unicode = HBufC16::NewL( aBufferPtr.Size() );
    CleanupStack::PushL( unicode );
    TPtr16 bufferPtr16( unicode->Des() );
    User::LeaveIfError( CnvUtfConverter::ConvertToUnicodeFromUtf8( bufferPtr16, aBufferPtr ) );
    
    // Create new parsed text which will be filled with input data
    delete iParsedTextPointer;
    iParsedTextPointer = NULL;
    iParsedTextPointer = CTtsParsedText::NewL( bufferPtr16 );
    
    // CTtsParsedText makes a copy of data
    CleanupStack::PopAndDestroy( unicode );
    
    // Construct segment & add it to the parsed text
    TTtsSegment iSegment( iDefaultStyleID );
    iSegment.SetTextPtr( iParsedTextPointer->Text() );
    iParsedTextPointer->AddSegmentL( iSegment );

    if ( !iUserSpecifiedDefaultStyle )
        {
        // Reset style language so that UI language will be used if
        // default default style in use 
        TTtsStyle& style = iDevTts->StyleL( iDefaultStyleID );
        
        if ( languageId == KErrNotFound )
            {
            style.iLanguage = User::Language();
            }
        else
            {
            style.iLanguage = (TLanguage)languageId;
            }
        }
    }

// -----------------------------------------------------------------------------
// CTtsControllerPluginBody::CopyParsedTextL
// Copies parsed text.
// -----------------------------------------------------------------------------
//
void CTtsControllerPluginBody::CopyParsedTextL( CTtsParsedText& aText )
    {
    // Delete previous object if exists
    delete iParsedTextPointer;
    iParsedTextPointer = NULL;

    // New instance
    iParsedTextPointer = CTtsParsedText::NewL( aText.Text(), aText.PhonemeSequence(),
                                               aText.PhonemeNotation() );
                                               
    // Segments:
    
    // Indexes for starting points for segment's text and phoneme part in
    // CTtsParsedText object's text and phoneme sequence    
    TInt textIndex( 0 );
    TInt phonemeIndex( 0 );
        
    for ( TInt i = 0; i < aText.NumberOfSegments(); i++ )
        {
        TTtsSegment segment = aText.SegmentL( i );
        
        segment.SetStyleID( aText.SegmentL( i ).StyleID() );
        
        TInt segTextLength = aText.SegmentL( i ).TextPtr().Length();
        TInt segPhonemeSeqLength = aText.SegmentL( i )
                                      .PhonemeSequencePtr().Length();
        
        // Text
        if ( textIndex + segTextLength
             <= iParsedTextPointer->Text().Length() )
            {
            segment.SetTextPtr( iParsedTextPointer->Text()
                                .Mid( textIndex, segTextLength ) );
            
            textIndex += segTextLength;
            }
        // Segments are somehow constructed illegally. Copy the whole text.
        else
            {
            segment.SetTextPtr( iParsedTextPointer->Text() );
            }
        
        // Phoneme sequence
        if ( phonemeIndex + segPhonemeSeqLength
             <= iParsedTextPointer->PhonemeSequence().Length() )
            {
            segment.SetPhonemeSequencePtr(
                iParsedTextPointer->
                PhonemeSequence().Mid( phonemeIndex, segPhonemeSeqLength ) );
            
            phonemeIndex += segPhonemeSeqLength;
            }
        else
            {
            segment.SetPhonemeSequencePtr(
                iParsedTextPointer->PhonemeSequence() );
            }

        // Add segment to the new instance of CTtsParsedText
        iParsedTextPointer->AddSegmentL( segment );
        }    
    }

// End of File  
