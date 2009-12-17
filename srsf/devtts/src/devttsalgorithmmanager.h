/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementations behind DevTTS interface
*
*/


#ifndef DEVTTSALGORITHMMANAGER_H
#define DEVTTSALGORITHMMANAGER_H

// INCLUDES
#include "srsfbldvariant.hrh"
#include "nssdevtts.h"
#include "devttsaudio.h"

#include <asrsttshwdevice.h>
#include <asrsttphwdevice.h>
#include <asrsnlphwdevice.h>

// CLASS DECLARATION

/**
* CTTSAlgorithmManager contains implementations behind DevTTS interface.
* Observer callbacks from TtsHwDevice and DevTTSAudio
*
*  @lib NssDevTts.lib
*/
class CTTSAlgorithmManager : public CBase, public MDevTTSAudioObserver,
                             public MTtsHwDeviceObserver,
                             public MASRSTtpHwDeviceObserver,
                             public MNlpHwDeviceObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param "MDevTTSObserver& aObserver"
        *        Reference to the callback observer.
        */ 
        static CTTSAlgorithmManager* NewL( MDevTTSObserver& aObserver/*, CConsoleBase* console*/ );
        
        /**
        * Destructor.
        */
        virtual ~CTTSAlgorithmManager();
        
    public: // New functions
        
        /**
        * Adds a new style to the collection. If equal style exists already,
        * new style is not added.
        * Will leave if an error occurs. Likely errors are: KErrNoMemory,
        * not enough memory to add new style. KErrNotSupported, the style 
        * is not supported. KErrArgument, the style contains an imposible 
        * value.
        *
        * @param "TTtsStyle& aStyle"
        *        Reference to the style which will be added to the style 
        *        collection.
        *
        * @return Identifier of added style.
        */
        TTtsStyleID AddStyleL( const TTtsStyle& aStyle );

        /**
        * Returns the current balance setting.
        *
        * @return Balance value.
        */
        TInt Balance();
        
        /**
        * Notifies that the buffer has been processed. The 
        * method is used if output mode of synthesizing is 
        * EDevTTSClientMode.
        *
        * @param "TDesC8& aBuffer"
        *        The buffer which has been processed.
        */
        void BufferProcessed( const TDesC8& aBuffer );
        
        /**
        * Request a pointer to the custom interface. Currently not in use.
        * 
        * @param "TUid aInterfaceID"
        *        Custom interface identifier.
        *
        * @return Pointer to custom interface. NULL if interface is not 
        *         found or initiation failures.
        */
        TAny* CustomInterface( TUid aInterfaceID );

        /**
        * Deletes style.
        * 
        * @param "TTtsStyleID aStyleID" Identifier of style to be deleted.
        */
        void DeleteStyleL( TTtsStyleID aStyleID );
        
        /**
        * Gives current position of synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing.
        * 
        * @param "TTimeIntervalMicroSeconds& aTime"
        *        Time of the synthesizing position
        */
        void GetPositionL( TTimeIntervalMicroSeconds& aTime ) const;
        
        /**
        * Gives current position of synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing.
        * 
        * @param "TTtsSegment& aSegment"
        *        Current segment which is synthesized, out parameter
        * @param "TInt& aWordIndex"
        *        Index of the current word, starting from the begining of 
        *        current segment
        */
        void GetPositionL( TTtsSegment& aSegment, TInt& aWordIndex ) const;
        
        /**
        * Tells if language is supported by the TTS engine or not.
        * 
        * @param "TLanguage aLanguage"
        *        Language code
        *
        * @return ETrue if supported, EFalse otherwise
        */
        TBool IsLanguageSupported( TLanguage aLanguage ) const;

        /**
        * Returns maximum volume value.
        * 
        * @return Maximum volume.
        */
        TInt MaxVolume();
        
        /**
        * Gives number of styles.
        * 
        * @return Number of styles.
        */
        TUint16 NumberOfStyles() const;
        
        /**
        * Pauses synthesizing.
        */
        void Pause();
        
        /**
        * Prepares for synthesis. The method will leave if an 
        * error occurs. Likely errors are: KErrInUse, synthesizing is 
        * currently ongoing. KErrNoMemory, not enough memory to start 
        * synthesis. KErrNotSupport, requested function is not supported.
        * 
        * @param "MTtsSegmentStream& aStream"
        *        Stream to be synthesized.
        */
        void PrimeSynthesisL( MTtsSegmentStream& aStream );
        
        /**
        * Prepares for synthesis. The method will leave if an 
        * error occurs. Likely errors are: KErrInUse, synthesizing is 
        * currently ongoing. KErrNoMemory, not enough memory to start 
        * synthesis. KErrNotSupport, requested function is not supported.
        * 
        * @param "CTtsParsedText& aText"
        *        Text containing multiple segments to be synthesized.
        */
        void PrimeSynthesisL( CTtsParsedText& aText );
        
        /**
        * Sets the priority for audio playback if in EDevTTSSoundDeviceMode.
        * 
        * @param "TInt iPriority"
        *        Priority value.
        * @param "TDevTTSAudioPreference iPref"'
        *        Preference setting. Quality, time or both.
        */
        void SetAudioPriority( TInt aPriority, TDevTTSAudioPreference aPref );

        /**
        * Sets the audio output routing
        * 
        * @param TInt aAudioOutput
        *        New output.
        */
        void SetAudioOutputL( TInt aAudioOutput );

        /**
        * Sets the playback device balance. Affects only when DevTTS is in 
        * EDevTTSSoundDeviceMode. Illegal values are neglected.
        * 
        * @param "TInt aBalance" Balance to be set. 
        */
        void SetBalance( TInt aBalance );
               
        /**
        * Sets new position for synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing. KErrArgument, position cannot be set.
        * 
        * @param "TTimeIntervalMicroSeconds& aTime"
        *        Time of the synthesizing postion.
        */
        void SetPositionL( const TTimeIntervalMicroSeconds& aTime );
        
        /**
        * Sets new position for synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing. KErrArgument, position cannot be set.
        * 
        * @param "TTTtsSegment& aSegment"
        *        The segment being synthesized.
        * @param "TInt aWordIndex"
        *        The index of word being synthesized.
        */
        void SetPositionL( const TTtsSegment& aSegment, TInt aWordIndex );
        
        /**
        * Sets the playback device volume. Affects only when DevTTS is in 
        * EDevTTSSoundDeviceMode. Range is from 0..MaxVolume(). Illegal values 
        * are neglected.
        * 
        * @param "TInt aVolume" Volume setting.
        */
        void SetVolume( TInt aVolume );

        /**
        * Sets the volume ramp duration. Start and end volumes must be set with
        * SetVolume() function.
        * 
        * @param "TTimeIntervalMicroSeconds& aRampDuration" 
        *        Ramp duration in microseconds.
        */
        void SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration );

        /**
        * Stops synthesizing and releases resources of synthesizing. 
        * The method does nothing if already stopped.
        */
        void Stop();
        
        /**
        * Receives style information. The method will leave if an error 
        * occurs. Likely errors are: KErrNotFound, style identifier not 
        * found. 
        * 
        * @param "TTtsStyleID aStyleID"
        *        Style identifier.
        *
        * @return Reference to style
        */
        TTtsStyle& StyleL( TTtsStyleID aStyleID ) const;
        
        /**
        * Receives style information. The method will leave if an error 
        * occurs. Likely errors are: KErrArgument, index is out of bounds.
        * 
        * @param "TUint16 aIndex"
        *        Index to style: 0..NumberOfStyles().
        *
        * @return Reference to style
        */ 
        TTtsStyle& StyleL( TUint16 aIndex ) const;
        
        /**
        * Starts synthesizing the text. The method is asynchronous, as soon
        * as the request has been placed, function call will return. 
        * MDevTTSObserver::MdtoEvent will be called when synthesizing finishes.
        * The method will leave if an error occurs. Likely errors are: 
        * KErrNoMemory, not enough memory. 
        * KErrInUse, synthesizing is currently ongoing
        * 
        * @param "TDevTTSOutputMode aOutputMode"
        *        Either use a sound device (EDevTTSSoundDeviceMode) or client 
        *        using callback (EDevTTSClientMode)
        */
        void SynthesizeL( TDevTTSOutputMode aOutputMode );

        /**
        * Returns current playback volume.
        * 
        * @return Volume setting.
        */       
        TInt Volume();

        /** 
        * Calls language identification for a given text.
        *
        * @since 3.0M
        * @param "CTtsParsedText& aText" Text
        * @param "TInt aNumberOfGuesses" Maximum number of guesses that will be provided
        * @param "RArray<TLanguage>& aLanguages" Output parameter which will contain the languages
        * @param "RArray<TInt>& aScores" Scores
        */
        void LanguageIdentificationL( CTtsParsedText& aText, 
                                      TInt aNumberOfGuesses, 
                                      RArray<TLanguage>& aLanguages,
                                      RArray<TInt>& aScores );

        /**
        * Calls Natural Language Processing (NLP) to normalize text.
        *
        * @since 3.0M
        * @param "CTtsParsedText& aText" Text which will be normalized
        */
        void NormalizeTextL( CTtsParsedText& aText );

        /**
        * Calls Natural Language Processing (NLP) to normalize and segment text.
        *
        * @since 3.0M
        * @param "CTtsParsedText& aText" Text which will be normalized
        */
        void NormalizeAndSegmentTextL( CTtsParsedText& aText );
        
    public: // Functions from base classes
        
        /**
        * From MDevTTSAudioObserver::InitializeComplete
        *
        * @param "TInt aError"
        *        KErrNone if everything is ok, system wide error code otherwise
        */
        void MdtaoInitializeComplete( TInt aError );
        
        /**
        * From MDevTTSAudioObserver::BufferToBeFilled
        *
        * @param "TDes8& aBuffer"
        *        Descriptor which should be filled
        * @param "TInt aSizeRequested"
        *        Number of bytes requested
        */
        void MdtaoBufferToBeFilled( TDes8& aBuffer, TInt aSizeRequested );
        
        /**
        * From MDevTTSAudioObserver::PlayFinished
        *
        * @param "TInt aError"
        *        KErrNone if everything is ok, system wide error code otherwise
        */
        void MdtaoPlayFinished( TInt aError );
        
        
        
        
        /**
        * From MTtsHwDeviceObserver::MthdTtsCompleted
        *
        * Called by the HW device when the synthesis has been completed.
        *
        * @param "TInt aStatus" Error code, KErrNone if success
        */
        void MthdTtsCompleted( TInt aStatus );
        
        /**
        * From MTtsHwDeviceObserver::MthdCustomEvent
        *
        * Called by the HW device when custom command implementation wants 
        * to notify a client.
        *
        * @param "TInt aEvent" Event code 
        * @param "TInt aStatus" Error code, KErrNone if success.
        * @param "TDesC8& aParameter" Optional parameter 
        */
        void MthdCustomEvent( TInt aEvent, TInt aStatus, 
                              const TDesC8& aParameter = KNullDesC8 );
        
        /**
        * From MTtsHwDeviceObserver::MthdProcessBuffer
        *
        * Called by the HW device when the HW device has synthesized a new audio buffer.
        *
        * @param "TDesC8& aBuffer"
        *         Synthesized audio buffer.
        */
        void MthdProcessBuffer( const TDesC8& aBuffer );
        
        /**
        * From MTtsHwDeviceObserver::MthdoConfigurationData
        *
        * Invoked by TTS HW Device when it needs a configuration data package.
        *
        * @param "TUint32 aPackageType" Type identifier. 
        * @param "TUint32 aPackageID" Identifier of package
        * @param "TUint32 aStartPosition" Start index in bytes
        * @param "TUint32 aEndPosition" End index in bytes. If the position is bigger 
        *         than the size of data package, the end part will be returned.
        * @return Pointer to buffer containing data.
        */
        HBufC8* MthdoConfigurationData( TUint32 aPackageType, 
                                        TUint32 aPackageID, 
                                        TUint32 aStartPosition, 
                                        TUint32 aEndPosition );

        
        

        /**
        * From MASRSTtpHwDeviceObserver::MathdoConfigurationData
        *
        * Callback function to load configuration data.
        * Client provides an implementation.
        *
        * @param "TUint32 aPackageType" Type identifier.
        * @param "TUint32 aPackageID" Package identifier.
        * @param "TUint32 aStartPosition" First byte.
        * @param "TUint32 aEndPosition" Last byte. If greater than the size of 
        *                               the data, rest of the data is given.
        * @return Pointer to the data buffer, TTP HW Device takes ownership.
        */	
        HBufC8* MathdoConfigurationData( TUint32 aPackageType, TUint32 aPackageID,
                                         TUint32 aStartPosition,
                                         TUint32 aEndPosition );

		/**
        * From MASRSTtpHwDeviceObserver::MathdoWordListReady
        *
        * Callback function to notify that word list is converted.
        * Client provides an implementation.
        *
        * @param "TInt aError" Error code, KErrNone if successful.
        */	
		void MathdoWordListReady( const TInt aError );
        

        /**
        * From MNlpHwDeviceObserver::MnhdNlpCompleted
        *
        * Called by the HW device when the synthesis has been completed.
        * @param aStatus Error code, KErrNone if success
        * @return none
        */
        void MnhdoNlpCompleted( TInt aStatus );
        
        /**
        * From MNlpHwDeviceObserver::MnhdoConfigurationData
        *
        * Invoked by TTS HW Device when it needs a configuration data package.
        * @param aPackageType Type identifier. 
        * @param aPackageID Identifier of package
        * @param aStartPosition Start index in bytes
        * @param aEndPosition End index in bytes. If the position is bigger 
        *         than the size of data package, the end part will be returned.
        * @return Data package in a buffer
        */
        HBufC8* MnhdoConfigurationData( TUint32 aPackageType, 
                                        TUint32 aPackageID, 
                                        TUint32 aStartPosition, 
                                        TUint32 aEndPosition );

        /**
        * From MNlpHwDeviceObserver::MnhdCustomEvent
        *
        * Returns style given the style id.
        */		
	   	const TTtsStyle& MnhdoStyleL( TTtsStyleID styleID );

       
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:
        
        /**
        * C++ default constructor.
        */
        CTTSAlgorithmManager( MDevTTSObserver& aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( /*CConsoleBase* console*/ );
        
        /**
        * Checks if state is idle and leaves in case of error.
        */
        void CheckStateL();
 
        // Prohibit copy constructor
        CTTSAlgorithmManager( const CTTSAlgorithmManager& );
        // Prohibit assigment operator
        CTTSAlgorithmManager& operator=( const CTTSAlgorithmManager& );
        
    public:     // Data
        
    protected:  // Data
        
    private:    // Data
        
        //State enumeration
        enum TDevTTSState
            {
            EDevTTSNotInitialized = 0,
            EDevTTSStopped,
            EDevTTSPrimed,
            EDevTTSSynthesizing
            };
        
        enum TTTSAudioPlayerState
            {
            ETTSAudioPlayerNotInitialized = 0,
            ETTSAudioPlayerInitializing,
            ETTSAudioPlayerIdle,
            ETTSAudioPlayerPlaying,
            ETTSAudioPlayerRecording, // Reserved for future
            ETTSAudioPlayerPaused,
            ETTSAudioPlayerError
            };
        
        
        // Flag which will be set if there is an ongoing synthesis when we are 
        // in paused state.
        TBool iPaused;
        
        // Reference to the registered oberserver
        MDevTTSObserver& iObserver;
        
        // For audio playback
        CDevTTSAudio* iAudio;
        
        // Audio device state
        TTTSAudioPlayerState iAudioState;
        
        // Pointer to the tts hw device
        CTtsHwDevice* iHw;
        
        // Stored error value, either from audio device or from TtsHwDevice
        TInt iError;
        
        // Current state of the object
        TDevTTSState iState;
        
        // Current mode of operation, buffers to audio device or to client
        TDevTTSOutputMode iMode;
        
        // Flag which is set when last buffer from TtsHwDevice has arrived
        TBool iLastBufferFromTts;
        
        // Reserved pointer for future extension
        //TAny* iReserved;
       
        // TtpHwDevice
        CASRSTtpHwDevice* iTtpHw;
        
        // Pointer to the beginning of playback data
        TPtrC8 iPlaybackBuffer;
        
        // Start point for the next playback and length of the buffer
        // These are used to index iPlaybackBuffer
        TInt iStartPoint;
        TInt iStopPoint;
        
#ifdef  DEVTTS_3DB_HACK
		TBool iApplyGain;
#endif

#ifdef DEVTTS_AUDIOBUFFER_TO_FILE
        RFs iSession;
        RFile iFile;
        TBool iFileOpen;
        TInt iFileCounter;
#endif

        // NlpHwDevice pointer
        CNlpHwDevice* iNlp;

        // ETrue if NLP has been successfully initialized
        TBool iNlpInitialized;

        // Wait loop for audio initialization
        CActiveSchedulerWait iAudioWait;
        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        
    };
    
#endif    // DEVTTSALGORITHMMANAGER_H
    
// End of File
