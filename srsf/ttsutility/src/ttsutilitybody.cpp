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
* Description:  Implementations for methods in CTtsUtilityBody
*
*/


// INCLUDE FILES
#include "ttsutilitybody.h"
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <ecom/ecom.h>
#include <AudioPreference.h>
#include <utf.h>
#include "rubydebug.h"

// CONSTANTS

// TTS media type equals to audio media type
const TUid KUidMediaTypeTTS = { 0x101F5D07 };

// Implementation UID of Tts Plugin
const TUid KUidTtsPlugin = { 0x101FF934 };

// Implementation UID of HQ-TTS Plugin
const TUid KUidHqTtsPlugin = { 0x10201AF5 };

// Header for TTS input
_LIT8( KTtsDataHeader, "(tts)" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTtsUtilityBody::CTtsUtilityBody
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTtsUtilityBody::CTtsUtilityBody( MTtsClientUtilityObserver& aObserver ) : 
                                  iAudioPlayDeviceCommands( iController ),
                                  iAudioPlayControllerCommands( iController ),
                                  iTtsCustomCommands( iController ),
                                  iObserver( aObserver ),
                                  iTime( 0 ),
                                  iCurrentStyleID( 0 ),
                                  iTrailingSilence( 0 ),
                                  iOpenPlugin( KNullUid ),
                                  iUidOfDataSink( KUidMmfAudioOutput )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::ConstructL()
    {
    // Dummy priority settings
    iPrioritySettings.iPriority = 0;
    iPrioritySettings.iPref = ( TMdaPriorityPreference ) 0;
    iPrioritySettings.iState = EMMFStateIdle; // idle
    
    // List plugins to make sure that at least one is found
    RArray<TUid> temp;
    ListPluginsL( temp );
    temp.Close();

    // Default audio priority
    StorePriority( KAudioPriorityVoiceDial, 
                   static_cast<TMdaPriorityPreference>( KAudioPrefVocosPlayback ) );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTtsUtilityBody* CTtsUtilityBody::NewL( MTtsClientUtilityObserver& aObserver )
    {
    CTtsUtilityBody* self = new( ELeave ) CTtsUtilityBody( aObserver );
    
    CleanupStack::PushL( self );

    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::~CTtsUtilityBody
// Destructor.
// -----------------------------------------------------------------------------
//
CTtsUtilityBody::~CTtsUtilityBody()
    {
    // Cancel any pending requests
    if ( iControllerEventMonitor != NULL )
        {
        iControllerEventMonitor->Cancel();
        }

    delete iControllerEventMonitor;
    iController.Close();
 
    iStyles.Close();
    iInternalStyleIDs.Close();
    iControllerStyleIDs.Close();
    
    delete iParsedText;
    
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::HandleEvent
// Handles events from plugin via MMF.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::HandleEvent( const TMMFEvent& aEvent )
    {
    TInt result = (TInt)aEvent.iErrorCode;

#ifdef _DEBUG
    TInt event = (TInt)aEvent.iEventType.iUid;
    RUBY_DEBUG2( "CTtsUtilityBody::HandleEvent() - Event Type = %d, Event Result = %d", 
                 event, result );
#endif
    
    if ( aEvent.iEventType == KMMFEventCategoryPlaybackComplete )
        {
        if ( result == KErrNone )
            {
            iRepeats--;
            
            if ( iRepeats > 0 )
                {
                // @todo Not too good way to handle waiting, we're blocking
                // the client thread here... Should be done in plugin side..
                User::After( iTrailingSilence );
                Play();
                // Don't make the callback at this point
                return;
                }
            }
        else
            {
            iRepeats = 0;
            }
        
        iUidOfDataSink = KUidMmfAudioOutput;

        // Remove data sink - otherwise file handle stays open. 
        // Return value ignored because there is nothing we can do. 
        iController.RemoveDataSink( iSinkHandle );

        iObserver.MapcPlayComplete( result );
        
        }
    else if ( aEvent.iEventType == KMMFEventCategoryAudioLoadingComplete )
        {
        if ( result != KErrNone )
            {
            iUidOfDataSink = KUidMmfAudioOutput; 
            iController.RemoveDataSink( iSinkHandle );
            }
            
        iTime = Duration();
        iObserver.MapcInitComplete( result, iTime );
        if ( iPlayImmediately )
            {
            iPlayImmediately = EFalse;
            if ( result == KErrNone )
                {
                Play();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::AddStyleL
// Adds style to collection, calls TTS custom commands.
// -----------------------------------------------------------------------------
//
TTtsStyleID CTtsUtilityBody::AddStyleL( const TTtsStyle& aStyle )
    {
    TTtsStyleID controllerID( 0 );
    
    if ( IsPluginOpen() )
        {
        User::LeaveIfError( iTtsCustomCommands.AddStyle( aStyle, controllerID ) );
        }

    // Add style also to our own array
    iStyles.Append( aStyle );
    TTtsStyleID internalID( iCurrentStyleID ); 
    iInternalStyleIDs.Append( internalID );

    // This is zero if no open controller connection, will be updated later
    iControllerStyleIDs.Append( controllerID ); 
    iCurrentStyleID++;

    return internalID;
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::DeleteStyle
// Deletes style from collection, calls TTS custom commands.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::DeleteStyle( TTtsStyleID aID )
    {
    TTtsStyleID controllerStyleId( 0 );

    // Delete from local storage
    for ( TInt i = 0; i < iInternalStyleIDs.Count(); i++ )
        {
        if ( iInternalStyleIDs[i] == aID )
            {
            iInternalStyleIDs.Remove( i );
            iStyles.Remove( i );
            // Take note of the controller side style id before removing it
            controllerStyleId = iControllerStyleIDs[i];
            iControllerStyleIDs.Remove( i );
            break;
            }
        }

    // Reset style id counter if no styles registered anymore
    if ( iInternalStyleIDs.Count() == 0 )
        {
        iCurrentStyleID = 0;
        }

    if ( IsPluginOpen() )
        {
        // Delete style from controller plugin
        TInt error = iTtsCustomCommands.DeleteStyle( controllerStyleId );
        if ( error != KErrNone )
            {
            return error;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::NumberOfStyles
// Returns number of registered styles.
// -----------------------------------------------------------------------------
//
TUint16 CTtsUtilityBody::NumberOfStyles()
    {
    return ( TUint16 ) iStyles.Count();
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::StyleL
// Returns reference to registered style based on style ID.
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsUtilityBody::StyleL( TTtsStyleID aStyleID )
    {
    TInt i( 0 );
    TInt count( iInternalStyleIDs.Count() );
    
    while ( i < count && iInternalStyleIDs[i] != aStyleID )
        {
        i++;
        }
    
    if ( i == count )
        {
        User::Leave( KErrNotFound );
        }

    return iStyles[i];
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::StyleL
// Returns reference to registered style based on style index.
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsUtilityBody::StyleL( TUint16 aIndex )
    {
    if ( aIndex >= iStyles.Count() )
        {
        User::Leave( KErrNotFound );
        }

    return iStyles[aIndex];
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::Play
// Starts playback.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::Play()
    {
    iController.Play();
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::Stop
// Stops playback.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::Stop()
    {
    iRepeats = 0;
    iController.Stop();
    iUidOfDataSink = KUidMmfAudioOutput;
    iController.RemoveDataSink( iSinkHandle );
    iPlayImmediately = EFalse;
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetVolume
// Sets playback volume.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::SetVolume( TInt aVolume )
    {
    iAudioPlayDeviceCommands.SetVolume( aVolume );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetRepeats
// Sets number of repeats.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::SetRepeats( TInt aRepeatNumberOfTimes, 
                                  const TTimeIntervalMicroSeconds& aTrailingSilence )
    {
    iTrailingSilence = TTimeIntervalMicroSeconds32( I64LOW( aTrailingSilence.Int64() ) );
    iRepeats = aRepeatNumberOfTimes;
    }
        
// -----------------------------------------------------------------------------
// CTtsUtilityBody::Duration
// Returns duration of TTS 'clip'.
// -----------------------------------------------------------------------------
//
const TTimeIntervalMicroSeconds& CTtsUtilityBody::Duration()
    {
    iDuration = 0; 
    iController.GetDuration( iDuration ); // Ignoring return value 
    
    return iDuration; 
    } 
    
// -----------------------------------------------------------------------------
// CTtsUtilityBody::MaxVolume
// Returns maximum volume.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::MaxVolume()
    {
    TInt volume = 0;
    if ( iAudioPlayDeviceCommands.GetMaxVolume( volume ) != KErrNone )
        {
        return 0;
        }
    return volume;
    } 

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenAndPlayDesL
// Opens descriptor source and plays it immediately.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenAndPlayDesL( const TDesC8& aDescriptor )
    {
    iPlayImmediately = ETrue;
    
    TRAPD( error, OpenDesL( aDescriptor ) );
    if ( error != KErrNone )
        {
        iPlayImmediately = EFalse;
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenAndPlayFileL
// Opens file source and plays it immediately.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenAndPlayFileL( const TDesC& aFileName )
    {
    iPlayImmediately = ETrue;
    
    TRAPD( error, OpenFileL( aFileName ) );
    if ( error != KErrNone )
        {
        iPlayImmediately = EFalse;
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenAndPlayParsedTextL
// Opens parsed text source and plays it immediately.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenAndPlayParsedTextL( CTtsParsedText& aText )
    {
    iPlayImmediately = ETrue;
    
    TRAPD( error, OpenParsedTextL( aText ) );
    if ( error != KErrNone )
        {
        iPlayImmediately = EFalse;
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenDesL
// Opens descriptor source 
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenDesL( const TDesC8& aDescriptor )
    {
    if ( !iPluginOpenedExplicitly )
        {
        CheckAndLoadCorrectPluginL( iDefaultStyle.iQuality, 
                                    iDefaultStyle.iLanguage, 
                                    iDefaultStyle.iVoice );
        }

    // Add data descriptor source
    TMMFDescriptorConfig sourceCfg;
    sourceCfg().iDes = (TAny*)&aDescriptor;
    sourceCfg().iDesThreadId = RThread().Id();
    User::LeaveIfError( iController.AddDataSource( KUidMmfDescriptorSource, 
                                                   sourceCfg, iSourceHandle ) );

    // Add data sink to file or audio output
    AddDataSinkL();

    // Prime controller
    User::LeaveIfError( iController.Prime() );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenFileL
// Opens file source
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenFileL( const TDesC& aFileName )
    {
    if ( !iPluginOpenedExplicitly )
        {
        CheckAndLoadCorrectPluginL( iDefaultStyle.iQuality, 
                                    iDefaultStyle.iLanguage, 
                                    iDefaultStyle.iVoice );
        }

    // Add data file source
    TMMFFileConfig sourceCfg;
    sourceCfg().iPath = aFileName;
    User::LeaveIfError( iController.AddDataSource( KUidMmfFileSource, sourceCfg, 
                                                   iSourceHandle ) );
    // Add data sink to file or audio output
    AddDataSinkL();
        
    // Prime controller
    User::LeaveIfError( iController.Prime() );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenParsedTextL
// Opens parsed text source with the help of TTS custom commands.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenParsedTextL( CTtsParsedText& aText )
    {
    if ( !iPluginOpenedExplicitly )
        {
        if ( aText.NumberOfSegments() > 0 )
            {
            // Take the first segment
            const TTtsSegment& segment = aText.SegmentL( 0 );
        
            TTtsStyleID styleID = segment.StyleID();
        
            // Style should have been registered already
            TTtsStyle& style = StyleL( styleID );
        
            CheckAndLoadCorrectPluginL( style.iQuality, style.iLanguage, 
                                        style.iVoice );
            }
         else
            {
            User::Leave( KErrArgument );
            }
        }

    // Data source is now parsed text
    CopyParsedTextL( aText );
    User::LeaveIfError( iTtsCustomCommands.OpenParsedText( *iParsedText ) );

    // Add data sink to file or audio output
    AddDataSinkL();

    User::LeaveIfError( iController.Prime() );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::Pause
// Pauses playback.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::Pause()
    {
    return iController.Pause();
    }   

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenPluginL
// Opens connection to plugin
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenPluginL( TUid aUid )
    {
    OpenControllerConnectionL( aUid );
    
    iPluginOpenedExplicitly = ETrue;
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::ListPluginsL
// Lists available plugins
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::ListPluginsL( RArray<TUid>& aUids )
    {
    // Empty the list
    aUids.Reset();
    
    RMMFControllerImplInfoArray controllers;
    CleanupResetAndDestroyPushL( controllers );
    
    CMMFControllerPluginSelectionParameters* selectionParams = CMMFControllerPluginSelectionParameters::NewLC();
    
    // Select the media IDs to allow
    RArray<TUid> mediaIds;
    CleanupClosePushL( mediaIds );
    User::LeaveIfError( mediaIds.Append( KUidMediaTypeTTS ) );

    selectionParams->SetMediaIdsL( mediaIds, CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds );
    CleanupStack::PopAndDestroy( &mediaIds );
    
    // Check also the allowed data headers
    // Should be "(tts)"
    CMMFFormatSelectionParameters* formatSelectParams = CMMFFormatSelectionParameters::NewLC();
    formatSelectParams->SetMatchToHeaderDataL( KTtsDataHeader );
    
    selectionParams->SetRequiredPlayFormatSupportL( *formatSelectParams );
    
    CleanupStack::PopAndDestroy( formatSelectParams );
    
    // Do the list of implementation
    selectionParams->ListImplementationsL( controllers );    
    
    // Make sure at least one controller has been found
    if( controllers.Count() == 0 )
        {
        RUBY_DEBUG1( "No controllers found using media type %x", KUidMediaTypeTTS );
        User::Leave( KErrNotFound );
        }

    for ( TInt c = 0; c < controllers.Count(); c++ )
        {
        RUBY_DEBUG1( "UID = %x", controllers[c]->Uid() );
        aUids.Append( controllers[c]->Uid() );
        }

    CleanupStack::PopAndDestroy( selectionParams );
    CleanupStack::PopAndDestroy( &controllers ); 
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetDefaultStyleL
// Sets deafault style 
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::SetDefaultStyleL( const TTtsStyle& aStyle )
    {
    User::LeaveIfError( iTtsCustomCommands.SetDefaultStyle( aStyle ) );
    iDefaultStyle = aStyle;
    }
        
// -----------------------------------------------------------------------------
// CTtsUtilityBody::DefaultStyleL
// Returns default style
// -----------------------------------------------------------------------------
//
TTtsStyle& CTtsUtilityBody::DefaultStyleL()
    {   
    User::LeaveIfError( iTtsCustomCommands.GetDefaultStyle( iDefaultStyle ) );
    return iDefaultStyle;
    }
        
// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetSpeakingRateL
// Sets speaking rate
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::SetSpeakingRateL( TInt aRate )
    {
    User::LeaveIfError( iTtsCustomCommands.SetSpeakingRate( aRate ) );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::SpeakingRateL
// Returns speaking rate
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::SpeakingRateL()
    {
    TInt rate( KErrGeneral );
    User::LeaveIfError( iTtsCustomCommands.GetSpeakingRate( rate ) );
    return rate;
    }
    
// -----------------------------------------------------------------------------
// CTtsUtilityBody::GetSupportedLanguagesL
// Returns list of supported languages
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::GetSupportedLanguagesL( RArray<TLanguage>& aLanguages )
    {
    User::LeaveIfError( iTtsCustomCommands.GetSupportedLanguages( aLanguages ) );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtilityBody::GetSupportedVoicesL
// Returns list of supported voices
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::GetSupportedVoicesL( TLanguage aLanguage, 
                                           RArray<TTtsStyle>& aVoices )
    {
    User::LeaveIfError( iTtsCustomCommands.GetSupportedVoices( aLanguage, aVoices ) );
    }
    
// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenDesL
// Opens unicode descriptor
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenDesL( const TDesC& aDescriptor )
    {
    HBufC8* utf8string = ConvertToUtf8LC( aDescriptor );
    OpenDesL( utf8string->Des() );
    CleanupStack::PopAndDestroy( utf8string );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::GetSupportedVoicesL
// Opens and plays unicode descriptor
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenAndPlayDesL( const TDesC& aDescriptor )
    {
    HBufC8* utf8string = ConvertToUtf8LC( aDescriptor );
    OpenAndPlayDesL( utf8string->Des() );
    CleanupStack::PopAndDestroy( utf8string );        
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetOutputFileL
// Sets output to be written to the file handle 
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::SetOutputFileL( const RFile& aFile )
    {
    iUidOfDataSink = KUidMmfFileSink; 

    // iFile has to contain valid file handle when data sink 
    // is marked to be file sink. 
    iFile = aFile; 
    }
    
// -----------------------------------------------------------------------------
// CTtsUtilityBody::Close
// Closes current TTS synthesis source.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::Close()
    {
    iRepeats = 0;
    if ( iControllerEventMonitor != NULL )
        {
        iControllerEventMonitor->Cancel();
        }
    delete iControllerEventMonitor;
    iControllerEventMonitor = NULL;
    iController.Close();
    iPluginOpenedExplicitly = EFalse;
    iOpenPlugin = KNullUid;
    iUidOfDataSink = KUidMmfAudioOutput;
    }        

// -----------------------------------------------------------------------------
// CTtsUtilityBody::GetPosition
// Returns position of synthesis in microseconds.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::GetPosition( TTimeIntervalMicroSeconds& aPosition )
    {
    return iController.GetPosition( aPosition );
    }
       
 
// -----------------------------------------------------------------------------
// CTtsUtilityBody::GetPosition
// Returns position of synthesis in words.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::GetPosition( TInt& aWordIndex )
    {
    return iTtsCustomCommands.GetPosition( aWordIndex );
    }


// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetPosition
// Sets synthesis position in microseconds.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::SetPosition( const TTimeIntervalMicroSeconds& aPosition )
    {
    iController.SetPosition( aPosition );
    }


// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetPosition
// Sets synthesis position in words.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::SetPosition( TInt aWordIndex )
    {
    iTtsCustomCommands.SetPosition( aWordIndex );
    }


// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetPriority
// Sets playback priority.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::SetPriority( TInt aPriority, TMdaPriorityPreference aPref )
    {
    StorePriority( aPriority, aPref );

    if ( IsPluginOpen() )
        {
        return iController.SetPrioritySettings( iPrioritySettings );
        }
    else
        {
        return KErrNone;
        }
    }
    
// -----------------------------------------------------------------------------
// CTtsUtilityBody::GetVolume
// Returns volume value.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::GetVolume( TInt& aVolume )
    {
    return iAudioPlayDeviceCommands.GetVolume( aVolume );
    }
        
// -----------------------------------------------------------------------------
// CTtsUtilityBody::SetBalance
// Sets balance.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::SetBalance( TInt aBalance )
    {
    return iAudioPlayDeviceCommands.SetBalance( aBalance );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::GetBalance
// Returns current balance setting.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::GetBalance( TInt& aBalance )
    {
    return iAudioPlayDeviceCommands.GetBalance( aBalance );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::CustomCommandSync
// Sends synchronous custom command via MMF.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2, 
                                         TDes8& aDataFrom )
    {
    return iController.CustomCommandSync( aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::CustomCommandSync
// Sends synchronous custom command via MMF.
// -----------------------------------------------------------------------------
//
TInt CTtsUtilityBody::CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2 )
    {
    return iController.CustomCommandSync( aDestination, aFunction, aDataTo1, aDataTo2 );
    }


// -----------------------------------------------------------------------------
// CTtsUtilityBody::CustomCommandAsync
// Sends asynchronous custom command via MMF.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination,  
                                          TInt aFunction, 
                                          const TDesC8& aDataTo1, 
                                          const TDesC8& aDataTo2, 
                                          TDes8& aDataFrom,
                                          TRequestStatus& aStatus )
    {
    iController.CustomCommandAsync( aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::CustomCommandAsync
// Sends asynchronous custom command via MMF.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination, 
                                          TInt aFunction, 
                                          const TDesC8& aDataTo1, 
                                          const TDesC8& aDataTo2, 
                                          TRequestStatus& aStatus )
    {
    iController.CustomCommandAsync( aDestination, aFunction, aDataTo1, aDataTo2, aStatus );
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::IsPluginOpen
// Returns ETrue if connection is established with controller plugin
// -----------------------------------------------------------------------------
//
TBool CTtsUtilityBody::IsPluginOpen()
    {
    if ( iOpenPlugin == KNullUid )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::OpenControllerConnectionL
// Opens controller connection if it has been closed
// Changes plugin if the one requested is not loaded currently
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::OpenControllerConnectionL( TUid aPluginUid )
    {
    RUBY_DEBUG_BLOCK( "CTtsUtilityBody::OpenControllerConnectionL(aPluginUid)" );
    
    if ( iOpenPlugin != aPluginUid )
        {
        // Clear previously loaded plugin if needed
        if ( IsPluginOpen() )
            {
            Close();
            }

        TInt error = iController.Open( aPluginUid, iPrioritySettings );
        if ( error ) 
            {
            RUBY_DEBUG2( "Unable to load controller plugin, error = %d, UID = %x", 
                         error, aPluginUid );            
            User::Leave( error );
            }
        else
            {
            if ( iControllerEventMonitor == NULL )
                {
                // Start the event monitor
                iControllerEventMonitor = CMMFControllerEventMonitor::NewL( *this, iController );
                iControllerEventMonitor->Start();
                }
            }
      
        iOpenPlugin = aPluginUid;

        // Add already registered styles to controller plugin too         
        for ( TInt i( 0 ); i < iStyles.Count(); i++ )
            {
            TTtsStyleID controllerStyleId( 0 );
            User::LeaveIfError( iTtsCustomCommands.AddStyle( iStyles[i], controllerStyleId ) );
            iControllerStyleIDs[i] = controllerStyleId;
            } 
        
        // Set priority settings
        User::LeaveIfError( SetPriority( iPrioritySettings.iPriority, 
                                         (TMdaPriorityPreference) iPrioritySettings.iPref ) );
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::CopyParsedTextL
// Copies parsed text structure and substitutes style ids to corresponding plugin
// ids.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::CopyParsedTextL( CTtsParsedText& aText )
    {
    RUBY_DEBUG_BLOCK( "CTtsUtilityBody::CopyParsedTextL" );

    // Delete previous object if exists
    delete iParsedText;
    iParsedText = NULL;

    // New instance
    iParsedText = CTtsParsedText::NewL( aText.Text(), aText.PhonemeSequence(),
                                        aText.PhonemeNotation() );

    // Segments

    // Indexes for starting points for segment's text and phoneme part in
    // CTtsParsedText object's text and phoneme sequence    
    TInt textIndex( 0 );
    TInt phonemeIndex( 0 );

    for ( TInt i = 0; i < aText.NumberOfSegments(); i++ )
        {
        TTtsSegment segment = aText.SegmentL( i );
        
        TInt styleIndex = iInternalStyleIDs.Find( aText.SegmentL( i ).StyleID() );
        User::LeaveIfError( styleIndex );
        segment.SetStyleID( iControllerStyleIDs[styleIndex] );
        
        TInt segTextLength = aText.SegmentL( i ).TextPtr().Length();
        TInt segPhonemeSeqLength = aText.SegmentL( i )
                                      .PhonemeSequencePtr().Length();
        
        // Text
        if ( textIndex + segTextLength
             <= iParsedText->Text().Length() )
            {
            segment.SetTextPtr( iParsedText->Text()
                                .Mid( textIndex, segTextLength ) );
            
            textIndex += segTextLength;
            }
        // Segments are somehow constructed illegally. Copy the whole text.
        else
            {
            segment.SetTextPtr( iParsedText->Text() );
            }
        
        // Phoneme sequence
        if ( phonemeIndex + segPhonemeSeqLength
             <= iParsedText->PhonemeSequence().Length() )
            {
            segment.SetPhonemeSequencePtr(
                iParsedText->PhonemeSequence().Mid( phonemeIndex, segPhonemeSeqLength ) );
            
            phonemeIndex += segPhonemeSeqLength;
            }
        else
            {
            segment.SetPhonemeSequencePtr(
                iParsedText->PhonemeSequence() );
            }

        // Add segment to the new instance of CTtsParsedText
        iParsedText->AddSegmentL( segment );
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::StorePriority
// Stores priority values locally to be sent to the plugin when connection is
// opened.
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::StorePriority( TInt aPriority, TMdaPriorityPreference aPref )
    {
    iPrioritySettings.iPriority = aPriority;
    iPrioritySettings.iPref = aPref;
    iPrioritySettings.iState = EMMFStateIdle;
    }    

// -----------------------------------------------------------------------------
// CTtsUtilityBody::ConvertToUtf8LC
// Converts unicode descriptor to utf-8
// -----------------------------------------------------------------------------
//
HBufC8* CTtsUtilityBody::ConvertToUtf8LC( const TDesC& aDes )
    {
    HBufC8* utf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aDes );
    CleanupStack::PushL( utf8 );
    return utf8;
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::AddDataSinkL
// Adds data sink (file and audio supported)
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::AddDataSinkL()
    {
    RUBY_DEBUG_BLOCKL( "CTtsUtilityBody::AddDataSinkL" );
    
    if ( iUidOfDataSink == KUidMmfAudioOutput )
        {
        User::LeaveIfError( iController.AddDataSink( KUidMmfAudioOutput, 
                                                     KNullDesC8, iSinkHandle ) ); 
        }
    else if ( iUidOfDataSink == KUidMmfFileSink )
        {
        TMMFFileHandleConfig destCfg( &iFile );

        TInt error = iController.AddDataSink( KUidMmfFileSink,destCfg, iSinkHandle ); 
        if ( error )
            {
            iUidOfDataSink = KUidMmfAudioOutput; 
            User::Leave( error ); 
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::CheckAndLoadCorrectPluginL
// 
// -----------------------------------------------------------------------------
//
void CTtsUtilityBody::CheckAndLoadCorrectPluginL( TTtsQuality aQuality, 
                                                  TLanguage aLanguage, 
                                                  const TTtsVoice& aVoice )
    {
    RUBY_DEBUG_BLOCKL( "CTtsUtilityBody::CheckAndLoadCorrectPluginL" );
    
    TInt error( KErrNone );
    
    switch ( aQuality )
        {
        case ETtsQualityHighOnly:

            OpenControllerConnectionL( KUidHqTtsPlugin );
            
            break;
            
        case ETtsQualityLowOnly:
        case ETtsQualityUndefined:
        
            OpenControllerConnectionL( KUidTtsPlugin );
                
            break;
            
        case ETtsQualityHighPreferred:

            TRAP( error, OpenControllerConnectionL( KUidHqTtsPlugin ) );
            if ( error || !IsSupportedSpeaker( aLanguage, aVoice ) )
                {
                // Try to use Klatt if Hqtts not found or specific language/speaker 
                // not available
                OpenControllerConnectionL( KUidTtsPlugin );
                }
                
            break;
            
        case ETtsQualityLowPreferred:

            TRAP( error, OpenControllerConnectionL( KUidTtsPlugin ) );
            if ( error || !IsSupportedSpeaker( aLanguage, aVoice ) )
                {
                // Try to use Hqtts if Klatt not found or specific language/speaker 
                // not available
                OpenControllerConnectionL( KUidHqTtsPlugin );
                }
        
            break;
        }
    }

// -----------------------------------------------------------------------------
// CTtsUtilityBody::IsSupportedSpeaker
// 
// -----------------------------------------------------------------------------
//
TBool CTtsUtilityBody::IsSupportedSpeaker( TLanguage aLanguage, 
                                           const TTtsVoice& aVoice )
    {
    TBool answer( EFalse );
    
    if ( aLanguage == KTtsUndefinedLanguage )
        {
        // Undefined language -> plugin can use default language and voice.
        answer = ETrue;
        }
    else
        {
        RArray<TLanguage> languages;
        iTtsCustomCommands.GetSupportedLanguages( languages );

        if ( languages.Find( aLanguage ) != KErrNotFound )
            {
            if ( aVoice == KNullDesC )
                {
                // Language found and undefined voice
                answer = ETrue;
                }
            else
                {
                RArray<TTtsStyle> voices;
                iTtsCustomCommands.GetSupportedVoices( aLanguage, voices );
                
                TInt counter( 0 );

                while ( answer == EFalse && counter < voices.Count() )
                    {
                    if ( voices[counter].iVoice == aVoice )
                        {
                        // Both language and voice are supported 
                        // by currently loaded plugin.
                        answer = ETrue;
                        }
                        
                    counter++;
                    }
                        
                voices.Close();
                }
            }
            
        languages.Close();
        }
        
    return answer;
    }
    
// End of File
