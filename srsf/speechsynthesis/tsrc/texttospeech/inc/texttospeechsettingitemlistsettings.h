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


#ifndef TEXTTOSPEECHSETTINGITEMLISTSETTINGS_H
#define TEXTTOSPEECHSETTINGITEMLISTSETTINGS_H


#include <speechsynthesis.h>

const int KEdit1MaxLength = 255;

/**
 * Setting item list settings 
 */
class CTextToSpeechSettingItemListSettings : CBase
    {

    public:
    
        // construct and destroy
        static CTextToSpeechSettingItemListSettings* NewL();
        
        virtual ~CTextToSpeechSettingItemListSettings();
        
        TDes& Edit1();
        void SetEdit1(const TDesC& aValue);
        
        TInt& EnumeratedTextPopup1();
        void SetEnumeratedTextPopup1(const TInt& aValue);
        
        TInt& EnumeratedTextPopup2();
        void SetEnumeratedTextPopup2(const TInt& aValue);
        
        TInt& Slider1();
        void SetSlider1(const TInt& aValue);

    private:
    
        // constructors
        void ConstructL();
        CTextToSpeechSettingItemListSettings();
    
    public:
    
        TInt                    iVolume;
        TInt                    iMaxVolume;
    
        TVoice                  iVoice;
        
        RArray<TLanguage>       iLanguages;
        RArray<TVoice>          iVoices;
        
        
    protected:
    
        TBuf<KEdit1MaxLength>   iEdit1;                 // Text to be synthesised
        TInt                    iEnumeratedTextPopup1;  // Language       
        TInt                    iEnumeratedTextPopup2;  // Voice
        TInt                    iSlider1;               // Speaking rate
            
    };
    
#endif // TEXTTOSPEECHSETTINGITEMLISTSETTINGS_H
