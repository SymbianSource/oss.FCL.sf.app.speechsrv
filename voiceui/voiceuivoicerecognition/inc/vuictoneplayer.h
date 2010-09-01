/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class that handles tone playing for the 
*               VoiceUIRecognition
*
*/


#ifndef VUICTONEPLAYER_H
#define VUICTONEPLAYER_H

// INCLUDES
#include <mdaaudiosampleplayer.h>


// CONSTANTS

// DATA TYPES

// FORWARD DECLARATIONS
class CMdaAudioPlayerUtility;

// CLASS DECLARATION

/**
*  Initializes and plays tones
*/
NONSHARABLE_CLASS( CTonePlayer ) : public CBase,
                                   public MMdaAudioPlayerCallback
    {
    public:  // Constructors and destructor

         /**
        * Two-phased constructor.
        */
        static CTonePlayer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CTonePlayer();

    public: // New functions

        /**
        * Sets the observer
        * @param aObserver The class requesting the tone callbacks
        */
        void RegisterObserver( MMdaAudioPlayerCallback* aObserver );

        /**
        * Performs initialization for playing tones
        * @param aSid The sound ID
        */
        void InitToneL( TInt aSid );

        /**
        * Plays initialized tone
        * @param aSid The sound ID
        */
        void PlayTone( TInt aSid );
        
        /**
        * Check Bt Accessory Status
        * @
        */
        void ChekcAccesoryStatusL();

    public: // Functions from base classes

        /**
        * From MMdaAudioPlayerCallback.
        * @see MMdaAudioPlayerCallback for more information.
        */
        void MapcInitComplete( TInt aError, 
                               const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback
        * @see MMdaAudioPlayerCallback for more information.
        */
        void MapcPlayComplete( TInt aError );
        
    private:    // Constructors & Destructor

        /**
        * By default constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CTonePlayer();

    private:    // New functions
        
        /**
        * Returns scaled volume value
        * @return Volume value, range [0 - MaxVolume]
        */
        TInt ScaledVolume();

    private:    // Data
        
        // Audio player
        CMdaAudioPlayerUtility*         iAudioPlayer;

        // Playback observer
        MMdaAudioPlayerCallback*        iObserver;

        // Start sound path
        HBufC*                          iStartSound;
        
        // Bluetooth start sound path
        HBufC*                          iBtStartSound;
        
        // Confirmation sound path
        HBufC*                          iConfirmationSound;

        // Volume
        TInt                            iVolume;
        
        //BT Accessory flag
        TBool                           iBTAcc;
        };

#endif      // VUICTONEPLAYER_H
            
// End of File
