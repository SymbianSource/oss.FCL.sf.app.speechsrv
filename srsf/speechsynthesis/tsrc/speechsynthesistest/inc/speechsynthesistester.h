/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef SPEECHSYNTHESISTESTER_H
#define SPEECHSYNTHESISTESTER_H

#include <e32base.h>
#include <speechsynthesis.h>

// CLASS DECLARATIONS

NONSHARABLE_CLASS( MTesterObserver )
    {
    
    public:
    
        virtual void SynthesisInitComplete( TInt aError ) = 0;
        virtual void SynthesisComplete( TInt aError ) = 0;
        
    };


/**
*  This class encapsulates resource file handling functions
*/
NONSHARABLE_CLASS( CSpeechSynthesisTester ) : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSpeechSynthesisTester* NewL( MTesterObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CSpeechSynthesisTester();

    public:
    
        TInt Open();
        void Close();
        
        void SynthesisInit(  TDesC& aText, const TDesC& aFileName = KNullDesC );
        void Synthesise();
        void StopL();
        void PauseL();
        
        void GetSupportedLanguagesL( RArray<TLanguage>& aLanguages );
        
        void GetSupportedVoicesL( RArray<TVoice>& aVoices, TLanguage aLanguage );
        TVoice VoiceL( );
        void SetVoiceL( TVoice& aVoice );

        TInt MaxSpeakingRateL();
        TInt SpeakingRateL();
        void SetSpeakingRateL( TInt aRate );
        
        TInt MaxVolumeL();
        TInt VolumeL();
        void SetVolumeL( TInt aVolume );

        void SetAudioPriorityL( TInt aPriority, TInt aPreference );
        
        void CustomCommandL( TInt aCommand, TInt aValue );
    
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

        /**
        * C++ default constructor.
        */
        CSpeechSynthesisTester( MTesterObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private: // Data

        RSpeechSynthesis            iSpeechSynthesis;
        
        enum TTestStatus
            {
            EIdle,
            EPriming,
            EPlaying
            };

        TTestStatus                 iTestStatus;
        
        MTesterObserver&            iObserver;
    };

#endif // SPEECHSYNTHESISTESTER_H

// End of File
