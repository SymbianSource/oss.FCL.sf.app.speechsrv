/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Settings list class
*
*/


// INCLUDE FILES
#include <featmgr.h>

#include <vcommand.rsg>
#include "vcsettingslist.h"
#include "vcsettingsengine.h"
#include "vcommand.hrh"
#include "vcappui.h"
#include "vcommandconstants.h"

#include <vcommanddomaincrkeys.h>
#include <srsfdomaincrkeys.h>
#include <centralrepository.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CVCSettingsList::CVCSettingsList
// C++ constructor
// ---------------------------------------------------------------------------
//
CVCSettingsList::CVCSettingsList()
    {
    }

// ---------------------------------------------------------------------------
// CVCSettingsList::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVCSettingsList::ConstructL()
    {
    LoadSettingsL();
    ConstructFromResourceL( R_VC_SETTING_ITEM_LIST );
    }


// ----------------------------------------------------------------------------
// CVCSettingsList::~CVCSettingsList
// ----------------------------------------------------------------------------
//
CVCSettingsList::~CVCSettingsList()
    {
    }


// ---------------------------------------------------------------------------
// CVCSettingsList::CreateSettingItemL
// From CAknSettingItemList Handles creating setting items
// ---------------------------------------------------------------------------
//
CAknSettingItem* CVCSettingsList::CreateSettingItemL( TInt aIdentifier )
    {
    CAknSettingItem* settingItem = NULL;

    switch ( aIdentifier )
        {
        case EVCSynthesizerItem:
            {
            settingItem = new ( ELeave ) CAknBinaryPopupSettingItem( aIdentifier, iSynthesizer );
            break;
            }
    
        case EVCVolumeItem:
            {
            settingItem = new ( ELeave ) CAknVolumeSettingItem( aIdentifier, iVolumeValue );
            break;
            }
 
        case EVCRejectionItem: 
            {
            settingItem = new ( ELeave ) CAknSliderSettingItem( aIdentifier, iRejectionValue );
            break;
            } 
            
        case EVCVerificationItem:
            {
            settingItem = new ( ELeave ) CAknEnumeratedTextPopupSettingItem( aIdentifier, iVerification );
            break;
            }
    
          case EVCResetItem:
            {
            settingItem = new ( ELeave ) CAknBigSettingItemBase( aIdentifier );
            break;
            }
        
        default:
            {
            break;
            }
        }

    return settingItem;
    }

 
// ---------------------------------------------------------------------------
// CVCSettingsList::LoadSettingsL
// ---------------------------------------------------------------------------
//
void CVCSettingsList::LoadSettingsL()
    {
    CRepository* client = CRepository::NewLC( KCRUidVCommandSettings );
    iSynthesizer = ETrue;
    iVolumeValue = KSettingsVolume;
    iRejectionValue = KSettingsRejection;
    iVerification = KSettingsVerification;

    User::LeaveIfError( client->Get( KVCSynthesizer, iSynthesizer ) );
    User::LeaveIfError( client->Get( KVCVerification, iVerification ) );
    CleanupStack::PopAndDestroy( client ); // client
    
    client = CRepository::NewLC( KCRUidSRSFSettings );
    User::LeaveIfError( client->Get( KSRSFPlaybackVolume, iVolumeValue ) );
    User::LeaveIfError( client->Get( KSRSFRejection, iRejectionValue ) );
    CleanupStack::PopAndDestroy( client );    // client
    }


// ---------------------------------------------------------------------------
// CVCSettingsList::SaveSettingsL
// Apply settings to CCcorController.
// ---------------------------------------------------------------------------
//
void CVCSettingsList::SaveSettingsL()
    {
    StoreSettingsL();

    CRepository* client = CRepository::NewLC( KCRUidVCommandSettings );
    User::LeaveIfError( client->Set( KVCSynthesizer, iSynthesizer ) );
    User::LeaveIfError( client->Set( KVCVerification, iVerification ) );
    CleanupStack::PopAndDestroy( client );
    
    client = CRepository::NewLC( KCRUidSRSFSettings );
    User::LeaveIfError( client->Set( KSRSFPlaybackVolume, iVolumeValue ) );
    User::LeaveIfError( client->Set( KSRSFRejection, iRejectionValue ) );
    CleanupStack::PopAndDestroy( client );
    }


// ---------------------------------------------------------------------------
// CVCSettingsList::EditItemL
// Launch the setting page for the current item by calling
// EditItemL on it.
// ---------------------------------------------------------------------------
//
void CVCSettingsList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemArray* itemArray = SettingItemArray();
    TInt itemIndex = itemArray->ItemIndexFromVisibleIndex( aIndex );
    CAknSettingItem* settingItem = itemArray->At( itemIndex );
    TInt itemIdentifier = settingItem->Identifier();
    
    switch( itemIdentifier )
        {
        case EVCSynthesizerItem:
            {
            CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
            StoreSettingsL();
            
            if( iSynthesizer == ESynthesizerOff )
                {
                // Disable voice verification
                if( iVerification == EVoice )
                    {                    
                    itemIndex = itemArray->ItemIndexFromVisibleIndex( EVCVerificationItem );
                    settingItem = itemArray->At( itemIndex );
                    // Change the setting
                    iVerification = EAutomatic;
                    settingItem->LoadL();
                    // Update the view
                    HandleChangeInItemArrayOrVisibilityL();
                    
                    // Display a note
                    VoiceVerificationDisabledNoteL();
                    }
                }
            
            break;
            }
            
        case EVCVolumeItem:
        case EVCRejectionItem:
            {
            // ETrue for always opening the settings view 
            CAknSettingItemList::EditItemL( aIndex, ETrue );
            break;            
            }
            
        case EVCVerificationItem:
            {
            // ETrue for always opening the settings view 
            CAknSettingItemList::EditItemL( aIndex, ETrue );
            StoreSettingsL();
            
            if( iVerification == EVoice )
                {
                // Enable synthesizer
                if( iSynthesizer == ESynthesizerOff )
                    {                    
                    // Binary setting item, so we can just choose to the other one
                    EditItemL( EVCSynthesizerItem, EFalse );
                    
                    // Update the view
                    HandleChangeInItemArrayOrVisibilityL();
                    
                    // Display a note
                    SynthesizerEnabledNoteL();
                    }
                }
            
            break;            
            }
        
        default:
            {
            break;
            }
        }

    SaveSettingsL();
    }

// ---------------------------------------------------------------------------
// CVCSettingsList::SizeChanged
// Set the size and position of component controls.
// ---------------------------------------------------------------------------
//
void CVCSettingsList::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();

    CEikListBox* lb = ListBox();
    if( lb )
        {
        lb->SetRect( Rect() );  // Set container's rect to listbox
        }
    }
    
// ----------------------------------------------------------------------------
// CVCSettingsList::CommandTooShortNoteL
// ----------------------------------------------------------------------------
//
void CVCSettingsList::SynthesizerEnabledNoteL()
    {
    HBufC* text = StringLoader::LoadLC( R_QTN_VC_INFO_NOTE_SYNTHESIZER_ON );
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }
    
// ----------------------------------------------------------------------------
// CVCSettingsList::VoiceVerificationDisabledNoteL
// ----------------------------------------------------------------------------
//
void CVCSettingsList::VoiceVerificationDisabledNoteL()
    {
    HBufC* text = StringLoader::LoadLC( R_QTN_VC_INFO_NOTE_VOICE_VERIFICATION_OFF );
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }
    
// End of File
