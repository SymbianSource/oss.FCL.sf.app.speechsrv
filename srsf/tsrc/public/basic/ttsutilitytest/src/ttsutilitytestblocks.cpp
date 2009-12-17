/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <stiftesteventinterface.h>

#include <audiopreference.h>
#include <nssttscustomcommands.h>
#include <utf.h>

#include "ttsutilitytest.h"

_LIT( Kttsutilitytest, "ttsutilitytest" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cttsutilitytest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cttsutilitytest::Delete() 
    {
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    TStifFunctionInfo const KFunctions[] =
        {  
        ENTRY( "CreateTtsUtility",      Cttsutilitytest::CreateTtsUtilityL ),
        ENTRY( "DestroyTtsUtility",     Cttsutilitytest::DestroyTtsUtilityL ),
        ENTRY( "StyleSettings",         Cttsutilitytest::StyleSettingsL ),
        ENTRY( "PlaybackProperties",    Cttsutilitytest::PlaybackPropertiesL ),
        ENTRY( "Play",                  Cttsutilitytest::Play ),
        ENTRY( "Pause",                 Cttsutilitytest::Pause ),
        ENTRY( "Stop",                  Cttsutilitytest::Stop ),
        ENTRY( "OpenDes",               Cttsutilitytest::OpenDesL ),
        ENTRY( "OpenDes8",              Cttsutilitytest::OpenDes8L ),
        ENTRY( "OpenFile",              Cttsutilitytest::OpenFileL ),
        ENTRY( "OpenParsedText",        Cttsutilitytest::OpenParsedTextL ),
        ENTRY( "OpenAndPlayDes",        Cttsutilitytest::OpenAndPlayDesL ),
        ENTRY( "OpenAndPlayDes8",       Cttsutilitytest::OpenAndPlayDes8L ),
        ENTRY( "OpenAndPlayFile",       Cttsutilitytest::OpenAndPlayFileL ),
        ENTRY( "OpenAndPlayParsedText", Cttsutilitytest::OpenAndPlayParsedTextL ),
        ENTRY( "SynthesisToFile",       Cttsutilitytest::SynthesisToFileL ),
        ENTRY( "CustomCommands",        Cttsutilitytest::CustomCommandsL ),
        ENTRY( "CustomCommands2",       Cttsutilitytest::CustomCommands2L ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::CreateTtsUtilityL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::CreateTtsUtilityL()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In CreateTtsUtilityL") );
    
    if ( iTtsUtility )
        {
        User::LeaveIfError( KErrAlreadyExists );
        }
    
    iTtsUtility = CTtsUtility::NewL( *this );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::DestroyTtsUtilityL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::DestroyTtsUtilityL()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In DestroyTtsUtilityL") );
    
    delete iParsedText;
    iParsedText = NULL;
    
    iTtsUtility->Close();
    
    delete iTtsUtility;
    iTtsUtility = NULL; 
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::StyleSettingsL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::StyleSettingsL()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In StyleSettingsL") );
    
    OpenPluginL();
    
    iStyle.iLanguage = ELangEnglish;
    iStyle.iVoice    = KDefaultVoiceNameMale;
    iStyle.iQuality  = ETtsQualityLowPreferred;
    
    iTtsUtility->SetDefaultStyleL( iStyle );
    
    TTtsStyle defaultStyle = iTtsUtility->DefaultStyleL();
    if ( iStyle.iLanguage != defaultStyle.iLanguage || 
         iStyle.iVoice    != defaultStyle.iVoice || 
         iStyle.iQuality  != defaultStyle.iQuality )
        {
        User::Leave( KErrGeneral );
        }
    
    TTtsStyleID styleId = iTtsUtility->AddStyleL( iStyle ); 
    
    TTtsStyle style = iTtsUtility->StyleL( styleId ); 
    
    if ( iStyle.iLanguage != style.iLanguage || 
         iStyle.iVoice    != style.iVoice || 
         iStyle.iQuality  != style.iQuality )
        {
        User::Leave( KErrGeneral );
        }
    
    // There should be only one style
    TUint16 index = 1;
    TRAPD( error, style = iTtsUtility->StyleL( index ) );
    if ( error != KErrNotFound )
        {
        User::Leave( KErrGeneral );
        }
        
    if ( iTtsUtility->NumberOfStyles() == 1 )
        {
        iTtsUtility->DeleteStyle( styleId );
        }
    
    RArray<TLanguage> languages;
    CleanupClosePushL( languages );
    
    iTtsUtility->GetSupportedLanguagesL( languages );
    
    if ( languages.Count() > 0 )
        {
        RArray<TTtsStyle> voices;
        
        iTtsUtility->GetSupportedVoicesL( languages[0], voices );
        
        if ( voices.Count() == 0 )
            {
            User::Leave( KErrNotFound );
            }
            
        voices.Close();
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( &languages );
    
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cttsutilitytest::PlaybackPropertiesL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::PlaybackPropertiesL()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In PlaybackPropertiesL") );
    
    OpenPluginL();
    
    TInt maxVolume( iTtsUtility->MaxVolume() );
    iTtsUtility->SetVolume( maxVolume );
    
    TInt volume;
    iTtsUtility->GetVolume( volume );
    
    if ( volume != maxVolume )
        {
        User::Leave( KErrGeneral );
        }
    
    TRAP_IGNORE( TInt speakingRate = iTtsUtility->SpeakingRateL() );
    TRAP_IGNORE( iTtsUtility->SetSpeakingRateL( KTtsMaxSpeakingRate ) );
    
    iTtsUtility->SetRepeats( 1, 1000 );
    
    TInt balance;
    iTtsUtility->GetBalance( balance );
    iTtsUtility->SetBalance( balance );
    
    TTimeIntervalMicroSeconds duration = iTtsUtility->Duration();
    
    iTtsUtility->SetPriority( KAudioPriorityVoiceDial, 
                              (TMdaPriorityPreference)KAudioPrefVocosPlayback );
    
    iTtsUtility->GetPosition( duration );
    iTtsUtility->SetPosition( duration );
    
    TInt wordIndex( KErrNotFound );
    iTtsUtility->GetPosition( wordIndex );
    iTtsUtility->SetPosition( wordIndex );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::Play
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::Play()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In Play") );
    
    iTtsUtility->Play();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::Pause
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::Pause()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In Pause") );
    
    iTtsUtility->Pause();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::Stop
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::Stop()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In Stop") );
    
    iTtsUtility->Stop();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenDesL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenDesL( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenDesL") );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    iTtsUtility->OpenDesL( input );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenDes8L
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenDes8L( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenDes8L") );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    HBufC8* input8 = HBufC8::NewLC( 4 * input.Length() );
    TPtr8 refText = input8->Des();
    
    User::LeaveIfError( CnvUtfConverter::ConvertFromUnicodeToUtf8( refText, input ) );

    iTtsUtility->OpenDesL( *input8 );
    
    CleanupStack::PopAndDestroy( input8 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenFileL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenFileL( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenFileL") );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
        
    iTtsUtility->OpenFileL( input );
    
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenParsedTextL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenParsedTextL( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenParsedTextL") );
    
    TInt result( KErrGeneral );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    if ( iParsedText )
        {
        delete iParsedText;
        iParsedText = NULL;
        }
        
    iStyle.iLanguage = User::Language();
    iStyle.iVoice    = KDefaultVoiceNameFemale;
    iStyle.iQuality  = ETtsQualityLowOnly;
    
    iParsedText = CTtsParsedText::NewL();
    iParsedText->SetPhonemeNotationL( KNullDesC );
    iParsedText->SetTextL( input );
    
    iSegment.SetStyle( iStyle );
    iStyle = iSegment.StyleL();
    iSegment.ResetStyle();
    
    iSegment.SetTrailingSilenceL( iSegment.TrailingSilence() );
    
    iSegment.SetStyleID( iTtsUtility->AddStyleL( iStyle ) );
    iSegment.SetTextPtr( iParsedText->Text() );
    iParsedText->AddSegmentL( iSegment );   
    
    if ( iParsedText->IsValid() )
        {
        result = KErrNone;
        iTtsUtility->OpenParsedTextL( *iParsedText );
        }
        
    return result;
    }
// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenAndPlayDesL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenAndPlayDesL( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenAndPlayDesL") );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    iTtsUtility->OpenAndPlayDesL( input );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenAndPlayDes8L
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenAndPlayDes8L( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenAndPlayDes8L") );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    HBufC8* input8 = HBufC8::NewLC( 4 * input.Length() );
    TPtr8 refText = input8->Des();
    
    User::LeaveIfError( CnvUtfConverter::ConvertFromUnicodeToUtf8( refText, input ) );

    iTtsUtility->OpenAndPlayDesL( *input8 );
    
    CleanupStack::PopAndDestroy( input8 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenAndPlayFileL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenAndPlayFileL( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenAndPlayFileL") );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
        
    iTtsUtility->OpenAndPlayFileL( input );
    
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenAndPlayParsedTextL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::OpenAndPlayParsedTextL( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In OpenAndPlayParsedTextL") );
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    if ( iParsedText == NULL )
        {
        iStyle.iLanguage = User::Language();
        iStyle.iVoice    = KDefaultVoiceNameFemale;
        iStyle.iQuality  = ETtsQualityLowOnly;
        
        iParsedText = CTtsParsedText::NewL();
        iParsedText->SetTextL( input );
    
        iSegment.SetStyleID( iTtsUtility->AddStyleL( iStyle ) );
        iSegment.SetTextPtr( iParsedText->Text() );
        iParsedText->AddSegmentL( iSegment );   
        }
        
    iTtsUtility->OpenAndPlayParsedTextL( *iParsedText );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::SynthesisToFileL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::SynthesisToFileL( CStifItemParser& aItem )
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In SynthesisToFileL") );
    
    TPtrC fileName;
    User::LeaveIfError( aItem.GetNextString ( fileName ) );
    
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() ); 
    iFs.MkDirAll( fileName ); // Ignore error (KErrAlreadyExists comes quite often)
    
    User::LeaveIfError( iFile.Replace( iFs, fileName, EFileWrite ) ); 
    
    iTtsUtility->SetOutputFileL( iFile );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::CustomCommandsL
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::CustomCommandsL()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In CustomCommandsL") );
    
    TUid openPlugin = OpenPluginL();
    TMMFMessageDestinationPckg destination( openPlugin );
    
    TBuf8<2> dataTo;
    dataTo.AppendNum( 1 );    

    TInt err1 = iTtsUtility->CustomCommandSync( destination, ETtsCustomCommandSetAudioOutput, 
                                                dataTo, KNullDesC8 );
    
    TBuf8<16> dataFrom;
    TInt err2 = iTtsUtility->CustomCommandSync( destination, ETtsCustomCommandSetAudioOutput, 
                                                dataTo, KNullDesC8, dataFrom );

    TRequestStatus status1( KRequestPending );
    iTtsUtility->CustomCommandAsync( destination, ETtsCustomCommandSetAudioOutput, 
                                     dataTo, KNullDesC8, status1 );
    User::WaitForRequest( status1 );
    
    TRequestStatus status2( KRequestPending );
    iTtsUtility->CustomCommandAsync( destination, ETtsCustomCommandSetAudioOutput, 
                                    dataTo, KNullDesC8, dataFrom, status2 );
    
    User::WaitForRequest( status2 );
    
    TInt result( KErrNone );
    
    if ( err1 || err2 || status1.Int() || status2.Int() )
        {
        result = KErrGeneral;
        }
    
    return result;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::CustomCommands2L
// 
// -----------------------------------------------------------------------------
//
TInt Cttsutilitytest::CustomCommands2L()
    {
    TestModuleIf().Printf( 0, Kttsutilitytest, _L("In CustomCommands2L") );
    
    const TUid KUidTtsPlugin = { 0x101FF934 };
    
    RMMFController controller;
    RTtsCustomCommands ttsCustomCommands( controller );
    TMMFPrioritySettings prioritySettings;
    
    User::LeaveIfError( controller.Open( KUidTtsPlugin, prioritySettings ) );
    
    TUint16 count;
    User::LeaveIfError( ttsCustomCommands.NumberOfStyles( count ) );
    
    if ( count > 0 )
        {
        TTtsStyleID styleId( 0 );
        User::LeaveIfError( ttsCustomCommands.Style( styleId, iStyle ) );
        
        TUint16 styleId16( 0 );
        User::LeaveIfError( ttsCustomCommands.Style( styleId16, iStyle ) );
        }
        
    controller.Close();
    
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cttsutilitytest::OpenPluginL
// 
// -----------------------------------------------------------------------------
//
TUid Cttsutilitytest::OpenPluginL()
    {
    const TUid KUidTtsPlugin = { 0x101FF934 };
    
    RArray<TUid> uids;
    CleanupClosePushL( uids );    
    iTtsUtility->ListPluginsL( uids );
    
    TInt index = uids.Find( KUidTtsPlugin );
    TUid openUid( KNullUid );
    
    if ( index >= 0 )
        {
        openUid = uids[index];
        }
    else if ( uids.Count() > 0 )
        {
        openUid = uids[0];
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    iTtsUtility->OpenPluginL( openUid );
    
    CleanupStack::PopAndDestroy( &uids );
    
    return openUid;
    }

// -----------------------------------------------------------------------------
// Cttsutilitytest::MapcInitComplete
//
// -----------------------------------------------------------------------------
// 
void Cttsutilitytest::MapcInitComplete( TInt aError, 
                                        const TTimeIntervalMicroSeconds& /*aDuration*/ )
    {
    if ( aError )
        {
        User::Panic( Kttsutilitytest, aError );
        }
    
    TEventIf stifEvent( TEventIf::ESetEvent, _L( "MapcInitComplete" ) );
    TestModuleIf().Event( stifEvent );    
    }
    
// -----------------------------------------------------------------------------
// Cttsutilitytest::MapcPlayComplete
//
// -----------------------------------------------------------------------------
// 
void Cttsutilitytest::MapcPlayComplete( TInt aError )
    {
    if ( aError )
        {
        User::Panic( Kttsutilitytest, aError );
        }
    
    TEventIf stifEvent( TEventIf::ESetEvent, _L( "MapcPlayComplete" ) );
    TestModuleIf().Event( stifEvent );
    }
    
//  End of File
