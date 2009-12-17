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


#ifndef TEXTTOSPEECHENGINE_H
#define TEXTTOSPEECHENGINE_H

// INCLUDES

#include <e32base.h>
#include <speechsynthesis.h>

/**
* Application engine 
*/
class CTextToSpeechEngine : public CActive
    {
    public: // Constructors and destructor

        static CTextToSpeechEngine* NewL();

        virtual ~CTextToSpeechEngine();

        void InitAndPlayText( const TDesC& aText );
        void InitAndPlayTextToFile( const TDesC& aText,
                                     const TFileName& aFileName );
        
        void Play( );
        void PauseL();
        void StopL();
        
        TInt VolumeL();
        void SetVolumeL( TInt aVolume );
        TInt MaxVolumeL();
        
        TInt SpeakingRateL();
        void SetSpeakingRateL( TInt aRate );
        TInt MaxSpeakingRateL();
        
        TVoice VoiceL();
        void SetVoiceL( const TVoice& aVoice ); 
        void GetLanguagesL( RArray<TLanguage>& aLanguages );
        void GetVoicesL( RArray<TVoice>& aVoices, TLanguage aLanguage );
        
    protected: // From CActive
    
       /**
        * CActive::RunL
        */
        void RunL();

        /**
        * CActive::DoCancel()
        */
        void DoCancel();
                
    private:    
    
        CTextToSpeechEngine();

        void ConstructL();

    private: // Data

        enum TState
            {
            EIdle,
            EPriming,
            ESpeaking
            };

        RSpeechSynthesis            iServer;
        
        TState                      iState; 

        TFileName                   iFileName;        
        
    };

#endif

// End of File
