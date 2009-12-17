/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class which handles the playing of tones
*
*/


#ifndef VCTONEREADER_H
#define VCTONEREADER_H

//  INCLUDES
#include <e32base.h>
#include <mmfbase.h>
#include <barsread.h>
#include <eikenv.h>
#include <avkon.rsg>
#include <mdaaudiotoneplayer.h>
#include "vctoneplayer.h"

// CONSTANTS

// CLASS DECLARATION

/**
* Class that reads the tone sequence for the Voice commands UI
*/
class CAknSoundInfo;

class CVCToneReader : public CBase,
                    	  public MMdaAudioToneObserver
    {
    public:  // Constructors and destructor

        /**
        * Symbian Two-phased constructor.
        */
        static CVCToneReader* NewL();

        /**
        * Destructor.
        */
        virtual ~CVCToneReader();

    public: // New functions

        /**
        * Sets the tone priority
        * @param aSid The sound ID
        * @return TInt
        */
        TInt TonePriority(TInt aSid);

        /**
        * Sets the appropriate priority preference
        * @param aSid The sound ID
        * @return TMdaPriorityPreference
        */
        TMdaPriorityPreference TonePriorityPreference(TInt aSid);

        /**
        * Sets the appropriate tone sequence
        * @param aSid The sound ID
        * @return HBufC8*
        */
        HBufC8* ToneSequence(TInt aSid);

        /**
        * Sets the appropriate tone volume
        * @param aSid The sound ID
        * @return TInt
        */
        TInt ToneVolume(TInt aSid);

        /**
        * Plays tone
        * @param aTonePlayer The class requesting the tone callbacks
        * @param aSid The sound ID
        */
        void PlayTone(MVCTonePlayer* aTonePlayer,TInt aSid);

    public: // Functions from base classes

        /**
        * From MMdaAudioToneObserver Called when the tone preparation is complete
        */
        void MatoPrepareComplete(TInt aError);
     
        /**
        * From MMdaAudioToneObserver Called when the tone playing is complete
        */
        void MatoPlayComplete(TInt aError);
       
    protected:  // New functions
        
        /**
        * Reads the tone sequence from the avkon resource file
        * @param aSid The sound ID
        * @param aTonePriority The tone priority
        * @param aTonePreference The tone priority preference
        * @param aToneVolume The tone volume
        * @return HBufC8*
        */
        HBufC8* ReadToneSequenceL( CAknSoundInfo* aInfo, TInt aSid, TInt* aTonePriority,
                                   TMdaPriorityPreference* aTonePreference, TInt* aToneVolume );

    private: // New functions

        /**
        * C++ default constructor.
        */
        CVCToneReader();

        /**
        * Default Symbian constructor
        */
        void ConstructL();
        
    private:    // Data

      TInt                    iTonePriority;
      TMdaPriorityPreference  iTonePreference;
      HBufC8*                 iToneSequence;
      TInt                    iToneVolume;

      TInt                    iAbortTonePriority;
      TMdaPriorityPreference  iAbortTonePreference;
      HBufC8*                 iAbortToneSequence;
      TInt                    iAbortToneVolume;

      TInt                    iErrorTonePriority;
      TMdaPriorityPreference  iErrorTonePreference;
      HBufC8*                 iErrorToneSequence;
      TInt                    iErrorToneVolume;

      TBool                   iPrepareCallback;
      MVCTonePlayer*          iTonePlayer;
      CMdaAudioToneUtility*   iToneUtility;

      CAknSoundInfo*          iAbortInfo;
      CAknSoundInfo*          iErrorInfo;
    };

#endif      // VCTONEREADER_H 
            
// End of File
