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


#include "texttospeechappui.h"
#include "texttospeechengine.h"
#include "texttospeechsettingitemlist.h"
#include "texttospeechsettingitemlistsettings.h"
#include "texttospeech.hrh"

/**
 * Construct the CTextToSpeechSettingItemList instance
 * @param aCommandObserver command observer
 */ 
CTextToSpeechSettingItemList::CTextToSpeechSettingItemList( 
    CTextToSpeechSettingItemListSettings& aSettings, 
    MEikCommandObserver* aCommandObserver ) :
    iSettings( aSettings ), 
    iCommandObserver( aCommandObserver )
    {
    }
    
/** 
 * Destroy any instance variables
 */
CTextToSpeechSettingItemList::~CTextToSpeechSettingItemList()
    {
    }

/**
 * Handle system notification that the container's size has changed.
 */
void CTextToSpeechSettingItemList::SizeChanged()
    {
    if ( ListBox() ) 
        {
        ListBox()->SetRect( Rect() );
        }
    }

/**
 * Create one setting item at a time, identified by id.
 * CAknSettingItemList calls this method and takes ownership
 * of the returned value.  The CAknSettingItem object owns
 * a reference to the underlying data, which EditItemL() uses
 * to edit and store the value.
 */
CAknSettingItem* CTextToSpeechSettingItemList::CreateSettingItemL( TInt aId )
    {
    CAknSettingItem* item( NULL );    
    
    switch ( aId )
        {
        case ETextToSpeechSettingItemListViewEdit1:
            {            
            item = new (ELeave) CAknTextSettingItem( aId, iSettings.Edit1() );
            
            break;
            }
        case ETextToSpeechSettingItemListViewEnumeratedTextPopup1:
            {            
            item = new (ELeave) CAknEnumeratedTextPopupSettingItem( 
                aId, iSettings.EnumeratedTextPopup1() );
                    
            break;
            }
        case ETextToSpeechSettingItemListViewEnumeratedTextPopup2:
            {            
            item = new (ELeave) CAknEnumeratedTextPopupSettingItem( 
                aId, iSettings.EnumeratedTextPopup2() );
                    
            break;
            }
        case ETextToSpeechSettingItemListViewSlider1:
            {            
            item = new (ELeave) CAknSliderSettingItem( 
                aId, iSettings.Slider1() );
            
            break;
            }
        }
        
    return item;
    }
    
/**
 * Edit the setting item identified by the given id and store
 * the changes into the store.
 * @param aIndex the index of the setting item in SettingItemArray()
 * @param aCalledFromMenu true: a menu item invoked editing, thus
 *    always show the edit page and interactively edit the item;
 *    false: change the item in place if possible, else show the edit page
 */
void CTextToSpeechSettingItemList::EditItemL ( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );

    CAknSettingItem* item = ( *SettingItemArray() )[aIndex];
    
    TInt tmp1;
    TInt tmp2;
    
    switch ( item->Identifier() )
        {
        case ETextToSpeechSettingItemListViewEnumeratedTextPopup1:
            
            tmp1 = iSettings.EnumeratedTextPopup1();
            item->StoreL();
            tmp2 = iSettings.EnumeratedTextPopup1();

            if ( tmp1 != tmp2 )
                {
                TLanguage lang = 
                    iSettings.iLanguages[iSettings.EnumeratedTextPopup1()];

                iSettings.iVoices.Reset();
                iEngine->GetVoicesL( iSettings.iVoices, lang );
            
                LoadVoicesL( ETrue ); 
                
                iSettings.iVoice = 
                    iSettings.iVoices[iSettings.EnumeratedTextPopup2()];

                HandleSliderVisibility();
                HandleChangeInItemArrayOrVisibilityL();
                }
            
            break;

        case ETextToSpeechSettingItemListViewEnumeratedTextPopup2:
            
            tmp1 = iSettings.EnumeratedTextPopup2();
            item->StoreL();
            tmp2 = iSettings.EnumeratedTextPopup2();

            if ( tmp1 != tmp2 )
                {
                iSettings.iVoice = 
                    iSettings.iVoices[iSettings.EnumeratedTextPopup2()];
                }
            
            HandleSliderVisibility(); 
            HandleChangeInItemArrayOrVisibilityL();
                
            break;
        
        default:
        
            break;
                
        }
        
    item->StoreL();
    }
    
/**
 *    Handle the "Change" option on the Options menu.  This is an
 *    alternative to the Selection key that forces the settings page
 *    to come up rather than changing the value in place (if possible).
 */
void CTextToSpeechSettingItemList::ChangeSelectedItemL()
    {
    if ( ListBox()->CurrentItemIndex() >= 0 )
        {
        EditItemL( ListBox()->CurrentItemIndex(), ETrue );
        }
    }

/**
 *    Load the initial contents of the setting items.  By default,
 *    the setting items are populated with the default values from
 *     the design.  You can override those values here.
 *    <p>
 *    Note: this call alone does not update the UI.  
 *    LoadSettingsL() must be called afterwards.
 */
void CTextToSpeechSettingItemList::LoadSettingValuesL()
    {

    iEngine = (((CTextToSpeechAppUi*)iEikonEnv->AppUi())->AppEngine());

    iSettings.iVolume    = iEngine->VolumeL();
    iSettings.iMaxVolume = iEngine->MaxVolumeL();
    iSettings.iVoice     = iEngine->VoiceL();
    
    iSettings.iLanguages.Reset();
    iEngine->GetLanguagesL( iSettings.iLanguages );
    
    iSettings.iVoices.Reset();
    iEngine->GetVoicesL( iSettings.iVoices, iSettings.iVoice.iLanguage );
    
    LoadLanguagesL();
    LoadVoicesL( EFalse );
    
    iSettings.SetSlider1( iEngine->SpeakingRateL() );
    CAknSettingItem* sliderItem = ( *SettingItemArray() )[3];
    sliderItem->LoadL();

    HandleSliderVisibility();
        
    // Required when there is only one setting item.
    SettingItemArray()->RecalculateVisibleIndicesL();

    HandleChangeInItemArrayOrVisibilityL();		
    }
    
/**
 * Add correct languages to popup list
 */
void CTextToSpeechSettingItemList::LoadLanguagesL()
    {
    CAknSettingItemArray* itemArray = SettingItemArray();
    CAknEnumeratedTextPopupSettingItem* item = 
        (CAknEnumeratedTextPopupSettingItem*) (*itemArray)[1];
    
	// Load texts dynamically.
	CArrayPtr<CAknEnumeratedText>* texts = item->EnumeratedTextArray();
	texts->ResetAndDestroy();
    
	CAknEnumeratedText* enumText( NULL );
	
	TInt selectedItem( 0 );
	
	for ( TInt i( 0 ); i < iSettings.iLanguages.Count(); i++ ) 
	    {
	    TLanguage lang( iSettings.iLanguages[i] );
	    
	    TBuf<8> tmp;	    
	    tmp.Num( (TInt)lang );
	    
	    HBufC* text = tmp.AllocLC();
	    
        enumText = new (ELeave) CAknEnumeratedText( i, text );
        
    	CleanupStack::PushL( enumText );
	    texts->AppendL( enumText );
	    CleanupStack::Pop( enumText );
	    
	    CleanupStack::Pop( text );
	    
	    if ( lang == iSettings.iVoice.iLanguage )
	        {
	        selectedItem = i;
	        }
	    }
	
	iSettings.SetEnumeratedTextPopup1( selectedItem );
	item->LoadL();
    }

/**
 * Add correct voice names to popup list
 */    
void CTextToSpeechSettingItemList::LoadVoicesL( TBool aResetSelection )
    {
    CAknSettingItemArray* itemArray = SettingItemArray();
    CAknEnumeratedTextPopupSettingItem* item = 
        (CAknEnumeratedTextPopupSettingItem*) (*itemArray)[2];

	// Load texts dynamically.
	CArrayPtr<CAknEnumeratedText>* texts = item->EnumeratedTextArray();
	texts->ResetAndDestroy();

	CAknEnumeratedText* enumText( NULL );
	
	TInt selectedItem( 0 );
	
	for ( TInt i( 0 ); i < iSettings.iVoices.Count(); i++ ) 
	    {
	    HBufC* text = iSettings.iVoices[i].iVoiceName.AllocLC();	    
        enumText = new (ELeave) CAknEnumeratedText( i, text );
        CleanupStack::Pop( text );

    	CleanupStack::PushL( enumText );
	    texts->AppendL( enumText );
	    CleanupStack::Pop( enumText );
	    
	    if ( !aResetSelection && 
	         iSettings.iVoices[i].iVoiceName == iSettings.iVoice.iVoiceName  )
	        {
	        selectedItem = i;
	        }
	    }
	
	iSettings.SetEnumeratedTextPopup2( selectedItem );
	item->LoadL();
    }

/**
 * Hide slider if current voice doesn't support speaking rate, 
 * otherwise show it. 
 */
void CTextToSpeechSettingItemList::HandleSliderVisibility()
    {
    CAknSettingItem* sliderItem = ( *SettingItemArray() )[3];
    
    if ( iSettings.iVoice.iVoiceName == _L("DefaultMale") || 
         iSettings.iVoice.iVoiceName == _L("DefaultFemale") )
        {
        sliderItem->SetHidden( ETrue );
        }
    else
        {
        sliderItem->SetHidden( EFalse );
        }
    }

/** 
 * Handle key event (override)
 * @param aKeyEvent key event
 * @param aType event code
 * @return EKeyWasConsumed if the event was handled, else EKeyWasNotConsumed
 */
TKeyResponse CTextToSpeechSettingItemList::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                           TEventCode aType )
    {
    TKeyResponse response( EKeyWasConsumed );
    
    if ( aKeyEvent.iCode == EKeyLeftArrow )
        {
        iCommandObserver->ProcessCommandL( EKeyDecVolume );
        }
    else if ( aKeyEvent.iCode == EKeyRightArrow )
        {
        iCommandObserver->ProcessCommandL( EKeyIncVolume );
        }
    else
        {
        response = CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
        }
    
    return response;
    }

// End of file
