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


/**
 * Generated helper class which manages the settings contained 
 * in 'TextToSpeechSettingItemList'.  Each CAknSettingItem maintains
 * a reference to data in this class so that changes in the setting
 * item list can be synchronized with this storage.
 */

#include <stringloader.h>
#include <texttospeech.rsg>

#include "texttospeechsettingitemlistsettings.h"

const TInt KMinSliderValue = 10;

/**
 * C/C++ constructor for settings data, cannot throw
 */
CTextToSpeechSettingItemListSettings::CTextToSpeechSettingItemListSettings()
    {
    }

/**
 * Two-phase constructor for settings data
 */
CTextToSpeechSettingItemListSettings* CTextToSpeechSettingItemListSettings::NewL()
    {
    CTextToSpeechSettingItemListSettings* data = 
        new( ELeave ) CTextToSpeechSettingItemListSettings();
    
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop( data );
    
    return data;
    }
    
/**
 * Second phase for initializing settings data
 */
void CTextToSpeechSettingItemListSettings::ConstructL()
    {
    HBufC* text = StringLoader::LoadLC( R_TEXT_TO_SPEECH_SETTING_ITEM_LIST_EDIT1 );
    SetEdit1( text->Des() );
    CleanupStack::PopAndDestroy( text );

    SetEnumeratedTextPopup1( 0 );
    SetEnumeratedTextPopup2( 0 );
    SetSlider1( KMinSliderValue );
    }

/**
 * Destructor
 */
CTextToSpeechSettingItemListSettings::~CTextToSpeechSettingItemListSettings()
    {
    iLanguages.Close();
    iVoices.Close();
    }
    
/**
 * Return reference to text
 */
TDes& CTextToSpeechSettingItemListSettings::Edit1()
    {
    return iEdit1;
    }

/**
 * Set text and limit it to maximum length if necessary
 */
void CTextToSpeechSettingItemListSettings::SetEdit1( const TDesC& aValue )
    {
    if ( aValue.Length() < KEdit1MaxLength )
        {
        iEdit1.Copy( aValue );
        }
    else
        {
        iEdit1.Copy( aValue.Ptr(), KEdit1MaxLength );
        }
    }

/**
 * Return index of selected language
 */
TInt& CTextToSpeechSettingItemListSettings::EnumeratedTextPopup1()
    {
    return iEnumeratedTextPopup1;
    }

/**
 * Set index of selected language
 */
void CTextToSpeechSettingItemListSettings::SetEnumeratedTextPopup1( const TInt& aValue )
    {
    iEnumeratedTextPopup1 = aValue;
    }

/**
 * Return index of selected voice name
 */
TInt& CTextToSpeechSettingItemListSettings::EnumeratedTextPopup2()
    {
    return iEnumeratedTextPopup2;
    }

/**
 * Set index of selected voice name
 */
void CTextToSpeechSettingItemListSettings::SetEnumeratedTextPopup2( const TInt& aValue )
    {
    iEnumeratedTextPopup2 = aValue;
    }

/**
 * Return speaking rate value 
 */
TInt& CTextToSpeechSettingItemListSettings::Slider1()
    {
    return iSlider1;
    }

/**
 * Set speaking rate value 
 */
void CTextToSpeechSettingItemListSettings::SetSlider1( const TInt& aValue )
    {
    iSlider1 = aValue;
    }

// End of file
