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

#include <utf.h>

#include "ttscontrollerplugintest.h"

_LIT( Kttscontrollerplugintest, "ttscontrollerplugintest" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cttscontrollerplugintest::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Create",                Cttscontrollerplugintest::CreateL ),
        ENTRY( "CreateFilePlayer",      Cttscontrollerplugintest::CreateFilePlayerL ),
        ENTRY( "CreateDesPlayer",       Cttscontrollerplugintest::CreateDesPlayerL ),
        ENTRY( "OpenFile",              Cttscontrollerplugintest::OpenFileL ),
        ENTRY( "OpenFileHandle",        Cttscontrollerplugintest::OpenFileHandleL ),
        ENTRY( "OpenDes",               Cttscontrollerplugintest::OpenDesL ),
        ENTRY( "Play",                  Cttscontrollerplugintest::Play ),
        ENTRY( "SetVolumeAndRamp",      Cttscontrollerplugintest::SetVolumeAndRampL ),
        ENTRY( "Destroy",               Cttscontrollerplugintest::Destroy ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::CreateL
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::CreateL()
    {
    // Print to UI
    _LIT( KFunction, "In CreateL" );
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );

    iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::CreateFilePlayerL
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::CreateFilePlayerL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KFunction, "In CreateFilePlayerL" );
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    TPtrC file;
    User::LeaveIfError( aItem.GetNextString ( file ) );
    TFileName fileName( file );
    
    iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL( fileName, *this );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::CreateDesPlayerL
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::CreateDesPlayerL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KFunction, "In CreateDesPlayerL" );
    
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    // Allow asynchronous request to leak handle 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles  ); 
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    iInputText = HBufC8::NewL( 4 * input.Length() );
    TPtr8 refText = iInputText->Des();
    
    User::LeaveIfError( CnvUtfConverter::ConvertFromUnicodeToUtf8( refText, input ) );
        
    iAudioPlayer = CMdaAudioPlayerUtility::NewDesPlayerL( *iInputText, *this );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::OpenFileL
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::OpenFileL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KFunction, "In OpenFileL" );
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    iAudioPlayer->Close();
    
    TPtrC file;
    User::LeaveIfError( aItem.GetNextString ( file ) );
    TFileName fileName( file );
    
    iAudioPlayer->OpenFileL( fileName );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::OpenFileHandleL
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::OpenFileHandleL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KFunction, "In OpenFileHandleL" );
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    iFile.Close();
    iFs.Close();
    iAudioPlayer->Close();

    TPtrC file;
    User::LeaveIfError( aItem.GetNextString ( file ) );
    TFileName fileName( file );

    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFile.Open( iFs, fileName, EFileRead|EFileStreamText ) );
    
    iAudioPlayer->OpenFileL( iFile );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::OpenDesL
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::OpenDesL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KFunction, "In OpenFileL" );
    
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    iAudioPlayer->Close();
    
    TPtrC input;
    User::LeaveIfError( aItem.GetNextString ( input ) );
    
    iInputText = HBufC8::NewL( 4 * input.Length() );
    TPtr8 refText = iInputText->Des();
    
    User::LeaveIfError( CnvUtfConverter::ConvertFromUnicodeToUtf8( refText, input ) );
    
    iAudioPlayer->OpenDesL( *iInputText );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::Play
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::Play()
    {
    // Print to UI
    _LIT( KFunction, "In Play" );
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    iAudioPlayer->Play();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::SetVolumeAndRampL
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::SetVolumeAndRampL()
    {
    // Print to UI
    _LIT( KFunction, "In SetVolumeAndRampL" );
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    iAudioPlayer->SetVolume( iAudioPlayer->MaxVolume() );
    iAudioPlayer->SetVolumeRamp( 100000 );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::Destroy
// 
// -----------------------------------------------------------------------------
//
TInt Cttscontrollerplugintest::Destroy()
    {
    // Print to UI
    _LIT( KFunction, "In Destroy" );
    TestModuleIf().Printf( 0, Kttscontrollerplugintest, KFunction );
    
    iFile.Close();
    iFs.Close();
    
    delete iAudioPlayer;
    iAudioPlayer = NULL;
    
    delete iInputText;
    iInputText = NULL;
    
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::MapcInitComplete
//
// -----------------------------------------------------------------------------
// 
void Cttscontrollerplugintest::MapcInitComplete( TInt aError, 
                                                 const TTimeIntervalMicroSeconds& /*aDuration*/ )
    {
    if ( aError )
        {
        User::Panic( Kttscontrollerplugintest, aError );
        }
    
    TEventIf stifEvent( TEventIf::ESetEvent, _L( "MapcInitComplete" ) );
    TestModuleIf().Event( stifEvent );    
    }
    
// -----------------------------------------------------------------------------
// Cttscontrollerplugintest::MapcPlayComplete
//
// -----------------------------------------------------------------------------
// 
void Cttscontrollerplugintest::MapcPlayComplete( TInt aError )
    {
    if ( aError )
        {
        User::Panic( Kttscontrollerplugintest, aError );
        }
    
    TEventIf stifEvent( TEventIf::ESetEvent, _L( "MapcPlayComplete" ) );
    TestModuleIf().Event( stifEvent );
    }
    
//  End of File
