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
* Description:  Wrapper class which forwards calls to TTS Utility
*
*/


#ifndef NSSVASTTSUTILITYWRAPPER_H
#define NSSVASTTSUTILITYWRAPPER_H

// INCLUDES
#include <e32base.h>
#include <nssttsutilityobserver.h>
#include <nssttscommon.h>

// FORWARD DECLARATIONS
class MTtsUtilityBase;

// CLASS DECLARATION

/**
* CNssTtsUtilityWrapper
* @since 3.1
* @see nssttsutility.h
* @see nssttsutilitybase.h
* @see nssttsutilityobserver.h
*/
NONSHARABLE_CLASS ( CNssTtsUtilityWrapper ) : public CActive, public MTtsClientUtilityObserver
    {
    public: // Constructors & destructor

        /**
        * Two-phased constructor
        * 
        * @since 3.1
        */
        static CNssTtsUtilityWrapper* NewL( MTtsClientUtilityObserver& aObserver,
                TInt aPriority = EMdaPriorityNormal,
                TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality );

        /**
        * Destructor.
        */
	    virtual ~CNssTtsUtilityWrapper();

    public: // New functions
    
        /**
        * Adds new style to style collection.
        *
        * @since 3.1
        * @param "TTtsStyle& aStyle" Reference to style.
        * @return Assigned style ID.
        */
        TTtsStyleID AddStyleL( const TTtsStyle& aStyle );
        
       /**
        * Opens a parsed text source and plays it.
        *
        * @since 3.1
        * @param "CTtsParsedText& aText" Parsed text object to play.
        * @param "TInt aPriority" Playback priority.
        * @param "TMdaPriorityPreference aPref" Playback priority preference.
        */
        void OpenAndPlayParsedTextL( CTtsParsedText& aText, 
                TInt aPriority = EMdaPriorityNormal, 
                TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality );
                                     
        /**
        * Stops playback of the audio sample as soon as possible.
        *
        * If the audio sample is playing, playback is stopped as soon as
        * possible. If playback is already complete, nothing further happens as
        * a result of calling this function. The callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is not
        * called.
        *
        * @since 3.1
        */
        void Stop();                                     
        
        /**
        * Begins playback of the initialised audio sample at the current volume
        * and priority levels.
        *
        * When playing of the audio sample is complete, successfully or
        * otherwise, the callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is
        * called.
        *
        * @since 3.1
        */
        void Play();
        
        /**
        * Closes the current audio clip (allowing another clip to be opened)
        *
        * @since 3.1
        */
        void Close();
 
    private: // Functions

	    /**
        * C++ constructor.
        */
	    CNssTtsUtilityWrapper( MTtsClientUtilityObserver& aObserver,
                               TInt aPriority,
                               TMdaPriorityPreference aPref );

	    /**
        * EPOC constructor.
        */
	    void ConstructL();
	    
	    /*
	    * From MTtsClientUtilityObserver
	    */
	    void MapcCustomCommandEvent( TInt aEvent, TInt aError );
	    
	    /*
	    * From MTtsClientUtilityObserver
	    */
        void MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration );

	    /*
	    * From MTtsClientUtilityObserver
	    */
	    void MapcPlayComplete( TInt aError );
	    
	    /**
	    * From CActive
	    */
	    void RunL();
	    
	    /**
	    * From CActive
	    */
	    void DoCancel();
	    
	    /**
	    * Sets the active object ready to be run
	    */
	    void Ready();
	        
	private: // Member variables
	
	    // Handle to library loader
	    RLibrary iLib;
	    
	    // UID of the utility client
	    TUid iClientUid;
	    
	    // Pointer to the utility base class
	    MTtsUtilityBase* iUtility;
	    
	    // Reference to the observer
	    MTtsClientUtilityObserver& iObserver;
	    
	    // Audio priority and preference
        TInt iPriority;
        TMdaPriorityPreference iPref;
        
        // Stored callback parameters
        TInt iError;
        TTimeIntervalMicroSeconds iDuration;   
	    
	    // Callback function
	    enum TTtsUtilityVasCallback
	        {
            EInitComplete,
            EPlayComplete
            };
        
        TTtsUtilityVasCallback iState;
    };

#endif // NSSVASSIUTILITYWRAPPER_H

// End of file
