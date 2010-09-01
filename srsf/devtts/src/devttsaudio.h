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
* Description:  Audio interface for DevTts
*
*/


#ifndef DEVTTSAUDIO_H
#define DEVTTSAUDIO_H

//  INCLUDES

#include <e32base.h>
#include <sounddevice.h>    // DevSound
#include <nssdevtts.h>      // DevTTS public header


// FORWARD DECLARATION

class CAudioOutput;


// CLASS DECLARATIONS

class MDevTTSAudioObserver
    {
    public:
        
        /**
        * Notifies the observer that initialization has been completed.
        *
        * @param "TInt aError"
        *        KErrNone if successful, otherwise one of the system level 
        *        error codes.
        */
        virtual void MdtaoInitializeComplete( TInt aError ) = 0;
        
        /**
        * Audio device wants the buffer to be filled. Client should provide
        * data to the descriptor given as parameter and call PlayData when
        * data is ready to be played.
        *
        * @param "TDes8& aBuffer"
        *        Descriptor which should be filled
        * @param "TInt aSizeRequested"
        *        Number of bytes requested
        */
        virtual void MdtaoBufferToBeFilled( TDes8& aBuffer,
                                            TInt aSizeRequested ) = 0;
        
        /**
        * Called when playback process is finished, successfully or otherwise.
        *
        * @param "TInt aError"
        *        KErrNone if everything succeeded, one of the system level error
        *        codes otherwise.
        */
        virtual void MdtaoPlayFinished( TInt aError ) = 0;
        
    };

/**
* Audio playback functionality for DevTTS.
*/
class CDevTTSAudio : public CBase, 
                     public MDevSoundObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDevTTSAudio* NewL( MDevTTSAudioObserver& aObserver, 
                                   TInt aPriority );
        
        /**
        * Destructor.
        */
        virtual ~CDevTTSAudio();
        
    public: // New functions
        
        /**
        * Initializes the audio device. Method is asynchronous.
        * MDevTTSAudioObserver::InitializeComplete() called when initialization
        * is ready.
        */
        void InitializeL();
        
         /**
        * Configures the audio device. Method is synchronous.
        */
        void ConfigL( TUint aSamplingRate, TUint aBitsPerSample, 
                      TBool aStereo, TUint aBufferSize );
        
        /**
        * Clears the audio device resources. Counter part of InitializeL.
        */
        void Clear();
        
        /**
        * Returns the current balance setting.
        *
        * @return Balance.
        */
        TInt Balance();

        /**
        * Returns maximum volume.
        *
        * @return Max volume.
        */
        TInt MaxVolume();

        /**
        * Initialises the audio playback process.
        */
        void PlayInitL();
        
        /**
        * Plays the filled buffer, Should be called when BufferToBeFilled 
        * callback has filled the requested buffer.
        *
        * @param "TBool aLastBuffer"
        *        Flag to tell if this is the last buffer
        */
        void PlayData( TBool aLastBuffer );
        
        /**
        * Sets the priority for audio playback.
        * 
        * @param "TInt aPriority"
        *        Priority value.
        * @param "TDevTTSAudioPreference aPref"'
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
        * Sets the playback balance.
        *
        * @param "TInt aBalance"
        *        Balance value to be set.
        */
        void SetBalance( TInt aVolume );
        
        /**
        * Sets the playback volume. Range is from 0 (minumum) to MaxVolume().
        * Out of range values will be discarded.
        *
        * @param "TInt aVolume"
        *        Volume value to be set, from 0 to MaxVolume.
        */
        void SetVolume( TInt aVolume );

        /**
        * Sets the volume ramp duration
        *
        * @param "TTimeIntervalMicroSeconds& aRampDuration"
        *        Ramp duration.
        */
        void SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration );
        
        /**
        * Stops the ongoing playback operation.
        */
        void Stop();
        
        /**
        * Pauses the ongoing playback operation
        */
        void Pause();
        
        /**
        * Returns the current volume setting.
        *
        * @return Volume
        */
        TInt Volume();

    public: // Functions from base classes
        
        /**
        * From MDevSoundObserver::InitializeComplete
        * Handles initialization completion event from DevSound.
        * Called InitializeL() function completes.
        *
        * @param "TInt aError"
        *        Error code. KErrNone if successful. Other values are possible
        *        indicating a problem initializing CMMFDevSound object.
        */
        void InitializeComplete( TInt aError );
        
        /**
        * From MDevSoundObserver::ToneFinished
        * Handles tone play completion event.
        * Called when an attempt to play tone has
        * completed, successfully or otherwise.
        *
        * @param "TInt aError"
        *        Error code. The status of tone playback. KErrUnderflow playing of
        *        the tone is complete. KErrAccessDenied the sound device is in use by
        *        another higher priority client. KErrCancel playing of the audio
        *        sample is stopped by DevSound client another higher priority client.
        */
        void ToneFinished( TInt aError ); 
        
        /**
        * From MDevSoundObserver::BufferToBeFilled
        * Handles CMMFDevSound object's data request event.
        * CMMFDevSound object calls this method when and where it needs data for
        * playing or converting. The observer should notify CMMFDevSound object as
        * quickly as possible after the data is read into buffer, aBuffer by calling
        * PlayData(), otherwise the implementation might callback method PlayError()
        * on derived class object with error code KErrUnderflow.
        *
        * @param "CMMFBuffer* aBuffer"
        *         Buffer into which data should be read. The amount of data that is
        *         needed is specified in CMMFBuffer::RequestSize().
        */
        void BufferToBeFilled( CMMFBuffer* aBuffer );
        
        /**
        * From MDevSoundObserver::PlayError
        * Handles play completion or cancel event.
        * Called when an attempt to play audio sample
        * has completed, successfully or otherwise.
        *
        * @param "TInt aError"
        *        Error code. The status of playback. KErrUnderflow playing of the
        *        audio sample is complete. KErrAccessDenied the sound device is in
        *        use by another higher priority client.
        */
        void PlayError( TInt aError ); 
        
        /**
        * From MDevSoundObserver::BufferToBeEmptied
        * Handles CMMFDevSound object's data request event.
        * Called when the buffer, aBuffer gets filled
        * while recording or converting. The observer should notify CMMFDevSound
        * object as quickly as possible after data in the buffer is processed by
        * calling RecordData(), otherwise the implementation might callback
        * method RecordError() on derived class object with error code KErrOverflow.
        *
        * @param "CMMFBuffer* aBuffer"
        *        Buffer containing processed (recorded or converted) data. The amount
        *        of data that is available is specified in CMMFBuffer::RequestSize().
        */
        void BufferToBeEmptied( CMMFBuffer* aBuffer ); 
        
        /**
        * From MDevSoundObserver::RecordError
        * Handles record completion or cancel event.
        * Called when an attempt to record audio sample
        * has completed, successfully or otherwise.
        *
        * @param "TInt aError"
        *        Error code. The status of recording. KErrOverflow audio devices
        *        runs out of internal buffer. KErrAccessDenied the sound device is
        *        in use by another higher priority client.
        */
        void RecordError( TInt aError ); 
        
        /**
        * From MDevSoundObserver::ConvertError
        * Handles conversion completion or cancel event.
        * Called when an attempt to convert data from
        * source format to destination format has completed, 
        * successfully or otherwise.
        *
        * @param	"TInt aError"
        *           Error code. KErrCancel conversion operation is cancelled. KErrNone
        *           conversion is complete. Other values are possible indicating a
        *           problem converting data.
        */
        void ConvertError( TInt aError );
        
        /**
        * From MDevSoundObserver::DeviceMessage
        * Handles device event.
        * Called when a message is received from the
        * audio hardware device.
        *
        * @param "aMessageType" 
        *         Defines the type of message. Used to determine how to 
        *         interpret the contents of aMsg.
        * @param "aMsg"
        *         Message that is packed in the Descriptor format.
        */
        void DeviceMessage( TUid aMessageType, const TDesC8& aMsg );
        
        /**
        * From MDevSoundObserver::SendEventToClient
        * Handles policy request completion event.
        * Called when an attempt to acquire sound
        * device is rejected by audio policy server.
        *
        * @param "const TMMFEvent& aEvent"
        */
        void SendEventToClient( const TMMFEvent& aEvent );  
        
    private:
        
        /**
        * C++ default constructor.
        */
        CDevTTSAudio( MDevTTSAudioObserver& aObserver, TInt aPriority );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
    
        // Observer
        MDevTTSAudioObserver& iObserver;
        
        // DevSound
        CMMFDevSound* iDevsound;
        
        // Used for setting audio routing
        CAudioOutput* iAudioOutput;
        
        // DevSound capabilities
        TMMFCapabilities iDevCap;
        
        // Buffer which is being filled
        CMMFBuffer* iBuffer;

        // Numerical value of the priority setting
        TInt iPriority;
        
        // DevSound priority settings
        TMMFPrioritySettings iAudioPriority;

        // Current balance
        TInt iBalance;

        // Current volume
        TInt iVolume;
    };
    
#endif // DEVTTSAUDIO_H
    
// End of File
