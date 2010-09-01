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


#ifndef TEXTTOSPEECHAPPUI_H
#define TEXTTOSPEECHAPPUI_H

#include <aknviewappui.h>

class CTextToSpeechSettingItemListView;
class CTextToSpeechEngine;

/**
 * The AppUi class handles application-wide aspects of the user interface, including
 * view management and the default menu, control pane, and status pane.
 */
class CTextToSpeechAppUi : public CAknViewAppUi
    {

    public: 
        
        // constructor and destructor
        CTextToSpeechAppUi();
        virtual ~CTextToSpeechAppUi();
        
        // from CCoeAppUi
        virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, 
                                              TEventCode aType );

        // from CEikAppUi
        void HandleCommandL( TInt aCommand );
        void HandleResourceChangeL( TInt aType );

        // from CAknAppUi
        void HandleViewDeactivation( const TVwsViewId& aViewIdToBeDeactivated, 
                                     const TVwsViewId &aNewlyActivatedViewId );
        
        // Return pointer to application engine
        CTextToSpeechEngine* AppEngine();

    private:
        
        void ConstructL();
        
        void InitializeContainersL();
    
    private: 
    
        CTextToSpeechSettingItemListView*   iTextToSpeechSettingItemListView;
        
        CTextToSpeechEngine*                iTextToSpeechEngine; 
    
    };

#endif // TEXTTOSPEECHAPPUI_H            
