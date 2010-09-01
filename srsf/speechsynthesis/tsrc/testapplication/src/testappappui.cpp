/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "TestAppAppUi.h"
#include "TestAppContainer.h"
#include <TestApplication.rsg>
#include "TestApp.hrh"
#include <aknNoteDialog.h>
#include <aknNoteWrappers.h>
#include <eikmenup.h>
#include <avkon.hrh>

#include "testappengine.h"

#include "rubydebug.h"

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CTestAppAppUi::ConstructL()
// 
// ----------------------------------------------------------
//
void CTestAppAppUi::ConstructL() 
    {
    BaseConstructL();
    
    iAppContainer = new (ELeave) CTestAppContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    
    iAppEngine = CTestAppEngine::NewL( *this );
    
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CTestAppAppUi::~CTestAppAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CTestAppAppUi::~CTestAppAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
        
    if ( iAppEngine )
        {
        TRAP_IGNORE( iAppEngine->DisconnectL() );
        delete iAppEngine;
        }
    }

// ------------------------------------------------------------------------------
// CTestAppAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CTestAppAppUi::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId==R_TEST_APPLICATION_MENU )
        {         
        if (iServerConnected)
            {      
            aMenuPane->SetItemDimmed( ETestAppCmdAppConnect, ETrue );
            aMenuPane->SetItemDimmed( ETestAppCmdAppDisconnect, EFalse );
            aMenuPane->SetItemDimmed( ETestAppCmdAppSayText, EFalse );
            aMenuPane->SetItemDimmed( ETestAppCmdAppSayTextToFile, EFalse );
            aMenuPane->SetItemDimmed( ETestAppCmdAppChangeVoice, EFalse );
            aMenuPane->SetItemDimmed( ETestAppCmdAppVolumeDown, EFalse );
            aMenuPane->SetItemDimmed( ETestAppCmdAppVolumeUp, EFalse );
            }
        else
            { 
            aMenuPane->SetItemDimmed( ETestAppCmdAppConnect, EFalse );
            aMenuPane->SetItemDimmed( ETestAppCmdAppDisconnect, ETrue );
            aMenuPane->SetItemDimmed( ETestAppCmdAppSayText, ETrue );
            aMenuPane->SetItemDimmed( ETestAppCmdAppSayTextToFile, ETrue );
            aMenuPane->SetItemDimmed( ETestAppCmdAppChangeVoice, ETrue );
            aMenuPane->SetItemDimmed( ETestAppCmdAppVolumeDown, ETrue );
            aMenuPane->SetItemDimmed( ETestAppCmdAppVolumeUp, ETrue );
            }
        }
    }

// ----------------------------------------------------
// CTestAppAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CTestAppAppUi::HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                             TEventCode aType )
    {
    TKeyResponse response( EKeyWasNotConsumed );
    
    if ( aType == EEventKey && iServerConnected )
        {
        response = EKeyWasConsumed;
        
        switch ( aKeyEvent.iCode )
            {
            case EKeyOK: 
            
                HandleCommandL( ETestAppCmdAppSayText );
                break;

            case EKeyDownArrow: 
            
                HandleCommandL( ETestAppCmdAppChangeVoice );
                break;

            case EKeyUpArrow: 
            
                HandleCommandL( ETestAppCmdAppChangeRate );
                break;

            case EKeyLeftArrow: 
            
                HandleCommandL( ETestAppCmdAppVolumeDown );
                break;

            case EKeyRightArrow: 
            
                HandleCommandL( ETestAppCmdAppVolumeUp );
                break;

            case 50: // a
                
                HandleCommandL( ETestAppCmdAppChangeAudioOutput );
                break;
                
            case 55: // p
            
                HandleCommandL( ETestAppCmdAppPause );
                break;
                
            case 56: // t
            
                HandleCommandL( ETestAppCmdAppPlay );
                break;
                
            case 57: // w
            
                HandleCommandL( ETestAppCmdAppStop );
                break;
                
            default:
            
                response = EKeyWasNotConsumed;
                
            }
        }
    
    return response;
    }

// ----------------------------------------------------
// CTestAppAppUi::HandleCommandL
// 
// ----------------------------------------------------
//
void CTestAppAppUi::HandleCommandL(TInt aCommand)
    {
    TInt value( -1 );
    TVoice voice;
    
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
        case EKeyPhoneEnd:
        case EKeyEscape: 
            Exit();
            break;
            
        case ETestAppCmdAppConnect:
            
            ShowNoteL( _L("Connect") );
            
            iAppEngine->ConnectL();
            iServerConnected = ETrue;            
            
            ChangeTextL( 0, _L("Server: Connected") );
            
            break;

        case ETestAppCmdAppDisconnect:
            
            ShowNoteL( _L("Disconnect") );
            
            iAppEngine->DisconnectL();
            iServerConnected = EFalse; 
            
            ChangeTextL( 0, _L("Server: Disconnected") ); 
            
            break;
            
        case ETestAppCmdAppSayText:
            
            ShowNoteL( _L("Say text") );
            
            iAppEngine->InitTextL();
            
            break;
            
        case ETestAppCmdAppSayTextToFile:
            
            ShowNoteL( _L("Say text to file") );
            
            iAppEngine->InitTextToFileL();
            
            break;

        case ETestAppCmdAppChangeVoice:
            
            voice = iAppEngine->ChangeVoiceL();
            
            ShowNoteL( voice.iVoiceName, voice.iLanguage );
            ChangeTextL( 1, voice.iVoiceName, voice.iLanguage );
            
            break;
            
        case ETestAppCmdAppVolumeDown:
            
            value = iAppEngine->VolumeDownL();
            
            ShowNoteL( _L("Volume"), value );
            ChangeTextL( 2, _L("Volume"), value );
            
            break;
            
        case ETestAppCmdAppVolumeUp:
            
            value = iAppEngine->VolumeUpL();
            
            ShowNoteL( _L("Volume"), value );
            ChangeTextL( 2, _L("Volume"), value );
            
            break;
            
        case ETestAppCmdAppChangeRate:
            
            value = iAppEngine->RateUpL();
            
            ShowNoteL( _L("Speaking rate"), value );
            ChangeTextL( 3, _L("Speking Rate"), value );
            break;
            
        case ETestAppCmdAppChangeAudioOutput:
            
            iAppEngine->ChangeAudioOutputL();
            
            ShowNoteL( _L("Audio output changed") );
            break;
            
        case ETestAppCmdAppPause:
            
            ShowNoteL( _L("Paused") );
            iAppEngine->PauseL();
            
            break;
            
        case ETestAppCmdAppPlay:
            
            ShowNoteL( _L("Play") );
            iAppEngine->PlayL();
            
            break;

        case ETestAppCmdAppStop:
            
            ShowNoteL( _L("Stopped") );
            
            iAppEngine->StopL();
            
            break;
            
        default:
            
            break;      
        }
    }

// ----------------------------------------------------
// CTestAppAppUi::ShowNoteL
// 
// ----------------------------------------------------
//
void CTestAppAppUi::ShowNoteL( TPtrC aText, TInt aNumber )
    {
    TBuf<64> buf( aText );
    
    if ( aNumber > -1 )
        {
        buf.Append( _L(": ") );
        buf.AppendNum( aNumber );
        }
        
    iEikonEnv->InfoMsg( buf );
    
    ChangeTextL( 5, buf.PtrZ() ); 
    }
    
// ----------------------------------------------------
// CTestAppAppUi::ChangeTextL
// 
// ----------------------------------------------------
//
void CTestAppAppUi::ChangeTextL( TInt aIndex, 
                                 TPtrC aText, 
                                 TInt aNumber )
    {
    TBuf<64> buf( aText );

    if ( aNumber != -1 )
        {
        buf.Append( _L(": ") );
        buf.AppendNum( aNumber );
        }
    
    iAppContainer->ChangeTextL( aIndex, buf );
    }

// End of File  
