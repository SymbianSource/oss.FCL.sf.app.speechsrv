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
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <StifTestEventInterface.h>

#include "speechsynthesistest.h"

#include "rubydebug.h"


// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTest::Delete() 
    {
    delete iTester;
    iTester = NULL;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::RunMethodL( CStifItemParser& aItem ) 
    {
    TStifFunctionInfo const KFunctions[] =
        {
        ENTRY( "Open",                      CSpeechSynthesisTest::OpenL ),
        ENTRY( "Close",                     CSpeechSynthesisTest::CloseL ),
        
        ENTRY( "InitialiseSynthesis",       CSpeechSynthesisTest::SynthesisInitL ),
        ENTRY( "InitialiseSynthesisFile",   CSpeechSynthesisTest::SynthesisInitFileL ),
        ENTRY( "Synthesise",                CSpeechSynthesisTest::SynthesiseL ),
        ENTRY( "Stop",                      CSpeechSynthesisTest::StopL ),
        ENTRY( "Pause",                     CSpeechSynthesisTest::PauseL ),

        ENTRY( "GetSupportedLangs",         CSpeechSynthesisTest::GetSupportedLanguagesL ),    
        
        ENTRY( "GetSupportedVoices",        CSpeechSynthesisTest::GetSupportedVoicesL ),
        ENTRY( "Voice",                     CSpeechSynthesisTest::VoiceL ),
        ENTRY( "SetVoice",                  CSpeechSynthesisTest::SetVoiceL ),
        
        ENTRY( "MaxVolume",                 CSpeechSynthesisTest::MaxVolumeL ),
        ENTRY( "Volume",                    CSpeechSynthesisTest::VolumeL ),
        ENTRY( "SetVolume",                 CSpeechSynthesisTest::SetVolumeL ),
        
        ENTRY( "MaxSpeakingRate",           CSpeechSynthesisTest::MaxSpeakingRateL ),
        ENTRY( "SpeakingRate",              CSpeechSynthesisTest::SpeakingRateL ),
        ENTRY( "SetSpeakingRate",           CSpeechSynthesisTest::SetSpeakingRateL ),
        
        ENTRY( "SetAudioPriority",          CSpeechSynthesisTest::SetAudioPriorityL ),
        ENTRY( "CustomCommand",             CSpeechSynthesisTest::CustomCommandL ),
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::OpenL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::OpenL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt result = iTester->Open();
    
    CheckResultL( expectedResult, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::CloseL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::CloseL( CStifItemParser& aItem )
    {
    // Expected result not used. 
    ReadExpectedResultL( aItem ); 
    
    iTester->Close();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SynthesisInitL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SynthesisInitL( CStifItemParser& aItem )
    {
    iExpectedResult = ReadExpectedResultL( aItem ); 
    
    TPtrC tmp;
    
    iText.Delete( 0, iText.MaxLength() );
    
    while ( aItem.GetNextString( tmp ) == KErrNone )
        {
        iText.Append( tmp );
        iText.Append( _L(" ") );
        }

    iTester->SynthesisInit( iText );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SynthesisInitFileL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SynthesisInitFileL( CStifItemParser& aItem )
    {
    iExpectedResult = ReadExpectedResultL( aItem ); 
    
    TInt result( KErrArgument );
    
    TPtrC tmp;
    TFileName fileName;
    
    iText.Delete( 0, iText.MaxLength() );
    
    while ( aItem.GetNextString( tmp ) == KErrNone )
        {
        if ( tmp != _L("filename:") )
            {
            iText.Append( tmp );
            iText.Append( _L(" ") );
            }
        else
            {
            break;
            }
        }

    if ( aItem.GetNextString( tmp ) == KErrNone )
        {
        fileName.Append( tmp );
        
        iTester->SynthesisInit( iText, fileName );
        
        result = KErrNone;
        }
    
    return result;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SynthesiseL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SynthesiseL( CStifItemParser& aItem )
    {
    iExpectedResult = ReadExpectedResultL( aItem ); 
    
    iTester->Synthesise();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::StopL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::StopL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TRAPD( result, iTester->StopL() );
    
    CheckResultL( expectedResult, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::PauseL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::PauseL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TRAPD( result, iTester->PauseL() );
    
    CheckResultL( expectedResult, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::GetSupportedLanguagesL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::GetSupportedLanguagesL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    RArray<TLanguage> languages;
    CleanupClosePushL( languages );
    
    TRAPD( result, iTester->GetSupportedLanguagesL( languages ) );
    
    CleanupStack::PopAndDestroy( &languages );
    
    CheckResultL( expectedResult, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::GetSupportedVoicesL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::GetSupportedVoicesL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    RArray<TVoice> voices;
    CleanupClosePushL( voices );
    
    TInt tmp;
    User::LeaveIfError( aItem.GetNextInt( tmp ) );
    TLanguage language( (TLanguage)tmp );
    
    TRAPD( result, iTester->GetSupportedVoicesL( voices, language ) ); 
    
    CleanupStack::PopAndDestroy( &voices );
    
    CheckResultL( expectedResult, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::VoiceL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::VoiceL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TVoice voice;
    
    TRAPD( result, voice = iTester->VoiceL() );
    
    CheckResultL( expectedResult, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SetVoiceL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SetVoiceL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    // @todo Implement this!
    TInt result = KErrNotSupported;
    
    CheckResultL( expectedResult, result );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::MaxVolumeL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::MaxVolumeL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt maxVolume( -1 );
    
    TRAPD( result, maxVolume = iTester->MaxVolumeL() );
    
    CheckResultL( expectedResult, result );
    
    TestModuleIf().Printf( 0, _L("MaxVolume:"), _L("%d"), maxVolume );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::VolumeL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::VolumeL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt volume( -1 );
    
    TRAPD( result, volume = iTester->VolumeL() );
    
    CheckResultL( expectedResult, result );
    
    TestModuleIf().Printf( 1, _L("Volume:"), _L("%d"), volume );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SetVolumeL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SetVolumeL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt newVolume;
    aItem.GetNextInt( newVolume );
    
    TRAPD( result, iTester->SetVolumeL( newVolume * iTester->MaxVolumeL() / 10 ) );
    
    CheckResultL( expectedResult, result ); 
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::MaxSpeakingRateL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::MaxSpeakingRateL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt maxSpeakingRate( -1 ); 
    
    TRAPD( result, maxSpeakingRate = iTester->MaxSpeakingRateL() ); 
    
    CheckResultL( expectedResult, result ); 
    
    TestModuleIf().Printf( 2, _L("maxSpeakingRate:"), _L("%d"), maxSpeakingRate );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SpeakingRateL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SpeakingRateL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt rate( -1 );
    
    TRAPD( result, rate = iTester->SpeakingRateL() );
    
    CheckResultL( expectedResult, result ); 
    
    TestModuleIf().Printf( 3, _L("SpeakingRate:"), _L("%d"), rate );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SetSpeakingRateL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SetSpeakingRateL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt newRate;
    aItem.GetNextInt( newRate );
    
    TRAPD( result, iTester->SetSpeakingRateL( newRate ) );
    
    CheckResultL( expectedResult, result ); 
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SetAudioPriorityL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::SetAudioPriorityL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TInt priority( -1 );
    aItem.GetNextInt( priority );
    
    TInt preference( -1 );
    aItem.GetNextInt( preference );
    
    TRAPD( result, iTester->SetAudioPriorityL( priority, preference ) );
    
    CheckResultL( expectedResult, result ); 
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::CustomCommandL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::CustomCommandL( CStifItemParser& aItem )
    {
    TInt expectedResult = ReadExpectedResultL( aItem ); 
    
    TRAPD( result, iTester->CustomCommandL( 0, 0 ) );
    
    CheckResultL( expectedResult, result );
        
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SynthesisInitComplete
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTest::SynthesisInitComplete( TInt aError )
    {
    RUBY_DEBUG0( "SynthesisInitComplete - START" );
    
    if ( iExpectedResult != aError )
        {
        RUBY_ERROR2( "CSpeechSynthesisTest::SynthesisInitComplete -- %d != %d", 
                     iExpectedResult, aError );
        User::Panic( _L("InitComplete"), aError );
        }
    
    TEventIf stifEvent( TEventIf::ESetEvent,  _L( "SynthesisInitComplete" ) );
    
    TestModuleIf().Event( stifEvent );
    
    RUBY_DEBUG0( "SynthesisInitComplete - EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::SynthesisComplete
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTest::SynthesisComplete( TInt aError )
    {
    RUBY_DEBUG0( "SynthesisComplete - START" );
    
    if ( iExpectedResult != aError )
        {
        RUBY_ERROR2( "CSpeechSynthesisTest::SynthesisComplete -- %d != %d", 
                     iExpectedResult, aError );
        User::Panic( _L("Complete"), aError );
        }

    TEventIf stifEvent( TEventIf::ESetEvent,  _L( "SynthesisComplete" ) );
    
    TestModuleIf().Event( stifEvent );
    
    RUBY_DEBUG0( "SynthesisComplete - EXIT" );
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::ReadExpectedResultL
// 
// -----------------------------------------------------------------------------
//
TInt CSpeechSynthesisTest::ReadExpectedResultL( CStifItemParser& aItem )
    {
    TInt expectedResult( -1 );
    
    User::LeaveIfError( aItem.GetNextInt( expectedResult ) );

    return expectedResult;
    }

// -----------------------------------------------------------------------------
// CSpeechSynthesisTest::CheckResultL
// 
// -----------------------------------------------------------------------------
//
void CSpeechSynthesisTest::CheckResultL( TInt aExpectedResult, TInt aResult )
    {
    RUBY_DEBUG_BLOCKL( "" );
    
    if ( aExpectedResult != aResult )
        {
        RUBY_ERROR2( "aExpectedResult = %d - aResult = %d", aExpectedResult, aResult );
        
        User::Leave( KErrCompletion );
        }
    }
    
//  End of File
