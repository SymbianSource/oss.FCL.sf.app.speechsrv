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


#ifndef TEXTTOSPEECHSETTINGITEMLIST_H
#define TEXTTOSPEECHSETTINGITEMLIST_H

#include <aknsettingitemlist.h>

class MEikCommandObserver;
class CTextToSpeechSettingItemListSettings;

/**
 * CTextToSpeechSettingItemList 
 */
class CTextToSpeechSettingItemList : public CAknSettingItemList
    {

    public: 
    
        CTextToSpeechSettingItemList( CTextToSpeechSettingItemListSettings& settings, 
                                      MEikCommandObserver* aCommandObserver );
                                      
        virtual ~CTextToSpeechSettingItemList();

        // overrides of CAknSettingItemList
        CAknSettingItem* CreateSettingItemL( TInt id );
        void EditItemL ( TInt aIndex, TBool aCalledFromMenu );
        virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                             TEventCode aType );

        // utility function for menu
        void ChangeSelectedItemL();

        void LoadSettingValuesL();
        
    private:
    
        // override of CAknSettingItemList
        void SizeChanged();
        
        void LoadLanguagesL();
        void LoadVoicesL( TBool aResetSelection );
        
        void HandleSliderVisibility();

    private:

        // current settings values
        CTextToSpeechSettingItemListSettings&   iSettings;
        
        CTextToSpeechEngine*                    iEngine;
        MEikCommandObserver*                    iCommandObserver;
    
    };
    
#endif // TEXTTOSPEECHSETTINGITEMLIST_H
