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


#ifndef TEXTTOSPEECHSETTINGITEMLISTVIEW_H
#define TEXTTOSPEECHSETTINGITEMLISTVIEW_H

#include <aknview.h>
#include <aknnavidecoratorobserver.h> 

class CAknNavigationDecorator;
class CTextToSpeechSettingItemList;
class CTextToSpeechEngine;
class CTextToSpeechSettingItemListSettings;

/**
 * Avkon view class for TextToSpeechSettingItemListView. It is register with the view server
 * by the AppUi. It owns the container control.
 */
class CTextToSpeechSettingItemListView : public CAknView,
                                         public MAknNaviDecoratorObserver
    {

    public:
    
        // constructors and destructor
        CTextToSpeechSettingItemListView();
        static CTextToSpeechSettingItemListView* NewL();
        static CTextToSpeechSettingItemListView* NewLC();        
        void ConstructL();
        virtual ~CTextToSpeechSettingItemListView();

        // from base class CAknView
        TUid Id() const;
        void HandleCommandL( TInt aCommand );
        
        // from base class MAknNaviDecoratorObserver
        void HandleNaviDecoratorEventL( TInt aEventID );
        
    protected:
    
        // from base class CAknView
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage);
        void DoDeactivate();
        void HandleStatusPaneSizeChange();

    private:
    
        void SetupStatusPaneL();
        void CleanupStatusPane();
    
    
    private: 
    
        CTextToSpeechSettingItemList*           iTextToSpeechSettingItemList;
        CTextToSpeechEngine*                    iEngine;
        CTextToSpeechSettingItemListSettings*   iSettings;
        
        CAknNavigationDecorator*                iVolumeControl;
        
    };

#endif // TEXTTOSPEECHSETTINGITEMLISTVIEW_H            
