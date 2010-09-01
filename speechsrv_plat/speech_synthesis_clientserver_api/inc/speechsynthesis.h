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
* Description:  Client side header of Speech synthesis server
*
*/


#ifndef SPEECHSYNTHESIS_H
#define SPEECHSYNTHESIS_H

// INCLUDES

#include <e32std.h>


// CONSTANTS

/**
* Maximum length of text-to-speech voice name. 
* 
* @see TVoiceName
*/
const TInt KMaxNameLength = 64;

/**
* Text-to-speech voice name type definition. 
* 
* @see TVoice
*/
typedef TBuf<KMaxNameLength> TVoiceName;


// CLASSES

/**
* Class containing basic information of text-to-speech voice. 
*/
class TVoice
    {

    public:

        /**
        * Language spoken by the voice
        */
        TLanguage               iLanguage;

        /**
        * Name of the voice
        */
        TVoiceName              iVoiceName;
    
        /**
        * Sampling rate of the voice
        */
        TInt                    iSamplingRate;

        /**
        * TVoice constructor
        */
        inline TVoice():
            iLanguage( ELangNone ),
            iSamplingRate( KErrNotFound )
            {
            // Nothing
            }
    };

/**
* Speech synthesis client side interface
*

* @code
* // Simple example how to use synthesis server synchronously
* _LIT( KText, "This is a text sample" );
* RSpeechSynthesis s;
* TRequestStatus status;
*
* // Open connection
* s.Open();
*
* // Initialise
* s.InitialiseSynthesis( KText, status );
* User::WaitForRequest( status );
*
* // Start synthesis when initialisation is ready
* s.Synthesise( status );
* User::WaitForRequest( status );
*
* // Close connection
* s.Close();
* @endcode
*
* @code
* // Another example how to use synthesis server synchronously
* _LIT( KText, "Second text sample" );
* RSpeechSynthesis s;
* TRequestStatus status;
*
* // Open connection
* s.Open();
*
* // Set volume to maximum value    
* s.SetVolumeL( * s.MaxVolumeL() );
*
* // Set speaking rate to half of maximum value
* s.SetSpeakingRateL( * s.MaxSpeakingRateL() / 2 );
* 
* // Set synthesis language but leave voice name open
* TVoice voice;
* voice.iLanguage = User::Language();
* s.SetVoiceL( voice );
*
* // Initialise
* s.InitialiseSynthesis( KText, status );
* User::WaitForRequest( status );
* 
* // Ask estimated duration of synthesis. 
* // Note: Leaves if current TVoice doesn't support duration
* TTimeIntervalMicroSeconds duration = s.DurationL();
*
* // Start synthesis when initialisation is ready
* s.Synthesise( status );
* User::WaitForRequest( status );
*
* // Close connection
* s.Close();
* @endcode
*/
class RSpeechSynthesis : public RSessionBase
    {
    
    public:
        
        /**
        * Possible parameter values for SetAudioOutputL()
        * 
        * @see RSpeechSynthesis::SetAudioOutputL()
        */
        enum TTtsAudioOutput
        	{
        	ETtsOutputDefault = 0,  ///< Default
        	ETtsOutputAll = 1,      ///< All output devices are used
        	ETtsOutputNoOutput = 2, ///< No output
        	ETtsOutputPrivate = 3,  ///< Private output device is used
        	ETtsOutputPublic = 4    ///< Public output device is used
        	};
        
    public:
        
        /**
        * Constructor
        */
        IMPORT_C RSpeechSynthesis();
        
        /**
        * Connect to synthesis server
        *
        * @return KErrNone if successful, otherwise system-wide error code. 
        */
        IMPORT_C TInt Open();
        
        /**
        * Close connection to synthesis server
        */    
        IMPORT_C void Close();
        
        /**
        * Initialise text to be synthesised. If there is synthesis already 
        * ongoing aStatus will be completed with KErrInUse. 
        * 
        *
        * @param aText Text to be synthesized. Text has to be valid until 
        *        aStatus is completed because this function is asynchronous 
        *        and the request that it represents may not complete until 
        *        some time after the call to the function has returned. 
        *        It is important, therefore, that this descriptor remain valid, 
        *        or remain in scope, until you have been notified that 
        *        the request is complete.
        * @param aStatus Status of priming. 
        */
        IMPORT_C void InitialiseSynthesis( const TDesC& aText,
                                           TRequestStatus& aStatus );
        
        /**
        * Initialise text to be synthesised to the file. If there is synthesis 
        * already ongoing aStatus will be completed with KErrInUse. 
        *
        * @param aText Text to be synthesized. Text has to be valid until 
        *        aStatus is completed because this function is asynchronous 
        *        and the request that it represents may not complete until 
        *        some time after the call to the function has returned. 
        *        It is important, therefore, that this descriptor remain valid, 
        *        or remain in scope, until you have been notified that 
        *        the request is complete.
        * @param aFileName Output is written to this file. File name has to be
        *        valid until aStatus is completed. Client should have capabilities 
        *        needed for writing location defined in aFileName, otherwise aStatus
        *        will be completed by KErrPermissionDenied. 
        * @param aStatus Status of priming
        */
        IMPORT_C void InitialiseSynthesis( const TDesC& aText, 
                                           const TFileName& aFileName,
                                           TRequestStatus& aStatus ); 
        
        /** 
        * @todo Not supported. aStatus will be always completed with KErrNotSupported.
        *
        * Initialise text to be synthesised to the descriptor. If there is synthesis 
        * already ongoing aStatus will be completed with KErrInUse. 
        *
        * @param aText Text to be synthesized. Text has to be valid until 
        *        aStatus is completed because this function is asynchronous 
        *        and the request that it represents may not complete until 
        *        some time after the call to the function has returned. 
        *        It is important, therefore, that this descriptor remain valid, 
        *        or remain in scope, until you have been notified that 
        *        the request is complete.
        * @param aDataBuffer Reference to a descriptor where output is stored
        *        after Synthesise() is called. If all the data does not fit 
        *        into aDataBuffer Synthesise can be called several times. 
        * @param aStatus Status of priming
        */
        IMPORT_C void InitialiseSynthesis( const TDesC& aText,
                                           TDes8& aDataBuffer, 
                                           TRequestStatus& aStatus );
        
        /**
        * Synthesise the text or resume paused synthesis. 
        * InitialiseSynthesis() has to be called succesfully before call to 
        * this function will succeed. aStatus is completed with KErrNotReady 
        * if initialisation has not been done. 
        *
        * @param aStatus Status of synthesis
        *
        * @see RSpeechSynthesis::InitialiseSynthesis() 
        * @see RSpeechSynthesis::PauseL() 
        */
        IMPORT_C void Synthesise( TRequestStatus& aStatus );     
        
        /**
        * Stop ongoing synthesis. Ongoing synthesis request is completed 
        * with KErrAbort. Does nothing if there is no synthesis ongoing. 
        *
        * @leave KErrInUse if request is made to stop other client's synthesis. 
        */
        IMPORT_C void StopL();
        
        /**
        * Pause synthesis. Resuming can be done by calling Synthesize(). 
        * Ongoing synthesis request is completed with KErrCancel. 
        *
        * @leave KErrNotSupported if trying to pause synthesis to file. 
        * @leave KErrNotReady if there is no synthesis ongoing. 
        */
        IMPORT_C void PauseL();
        
        /**
        * Returns estimated duration of synthesis. This function returns valid 
        * duration only after synthesis is initialised and before synthesis has
        * completed. 
        *
        * @return Estimated duration of speech synthesis playback. 
        *
        * @leave KErrNotReady if state is incorrect
        * @leave KErrNotSupported if current voice settings doesn't support 
        *        duration information
        */
        IMPORT_C TTimeIntervalMicroSeconds DurationL();
        
        /**
        * Get supported synthesis languages
        *
        * @param aLanguages Contains supported languages after function call
        *
        */
        IMPORT_C void GetLanguagesL( RArray<TLanguage>& aLanguages );
        
        /**
        * Get available voices 
        *
        * @param aVoices Contains supported voices in TVoice structure after 
        *        the function call. 
        * @param aLanguage Defines a language that returned voices can speak. 
        *
        */
        IMPORT_C void GetVoicesL( RArray<TVoice>& aVoices, TLanguage aLanguage );
        
        /**
        * Get current voice. 
        *
        * @return Current voice. 
        *
        */
        IMPORT_C TVoice VoiceL();
        
        /**
        * Change voice. Change is effective after next InitialiseSynthesis() call 
        * is made and until new SetVoiceL() call is made or Close() is called. 
        *
        * @param aVoice New voice to be set. aVoice.iLanguage is the only mandatory 
        *        parameter,others can be undefined and server may ignore 
        *        those if invalid values are given. 
        *
        * @leave KErrArgument if aVoice is not valid
        */
        IMPORT_C void SetVoiceL( const TVoice& aVoice ); 
        
        /**
        * Returns maximum speaking rate.
        *
        * @return Maximum speaking rate
        *
        */
        IMPORT_C TInt MaxSpeakingRateL(); 
        
        /**
        * Returns the current speaking rate value.
        *
        * @return Current speaking rate
        *
        */
        IMPORT_C TInt SpeakingRateL(); 
        
        /**
        * Sets the speaking rate. Range is 1..MaxSpeakingRateL(). Setting will 
        * be effective after next InitialiseSynthesis() call and until new rate 
        * is set or Close() is called. Note: All voices doesn't support speaking 
        * rate changes and in that case there is no audible change. 
        *
        * @param aSpeakingRate Rate value
        *
        * @leave KErrArgument if speaking rate is out of range. 
        *
        * @see MaxSpeakingRateL()
        */
        IMPORT_C void SetSpeakingRateL( TInt aSpeakingRate ); 
        
        /**
        * Get maximum volume
        *
        * @return Maximum volume
        *
        */
        IMPORT_C TInt MaxVolumeL(); 
        
        /**
        * Get current volume.
        *
        * @return Volume level between 0 and MaxVolumeL()
        *
        */
        IMPORT_C TInt VolumeL(); 
        
        /**
        * Change volume. Will be effective until new volume is set or Close()
        * is called.
        *
        * @param aVolume New volume, accepted values are in range from 0 to MaxVolumeL()
        *
        * @leave KErrArgument if volume value is out of range, 
        *
        * @see MaxVolumeL()
        */
        IMPORT_C void SetVolumeL( TInt aVolume ); 
        
        /**
        * Sets the playback priority and preference values. Setting will 
        * be effective after next InitialiseSynthesis() call and until new priority 
        * is set or Close() is called. Client needs Nokia VID to use
        * this function. 
        *
        * @param aPriority Audio priority
        * @param aPreference Audio preference
        *
        * @leave KErrPermissionDenied if client doesn't have Nokia VID. 
        */
        IMPORT_C void SetAudioPriorityL( TInt aPriority, TInt aPreference ); 
        
        /**
      	* Sets the audio output. Setting will be effective immediately and until
      	* new value is set or connection closed. Client needs WriteDeviceData 
      	* capability if setting output to ETtsOutputAll
        *
      	* @param aAudioOutput - where the audio is routed. 
      	*
      	* @leave KErrPermissionDenied if client doesn't have WriteDeviceData when
      	*        setting output to ETtsOutputAll. 
        *
      	*/
        IMPORT_C void SetAudioOutputL( TTtsAudioOutput aAudioOutput );
        
        /**
        * Send custom command to the server. By using this function it is possible 
        * to make adjustments to the synthesis process. 
        * Note: Currently there is no supported commands. 
        *
        * @param aCommand A custom command 
        * @param aValue Parameter value for aCommand 
        * 
        * @leave KErrNotSupported aCommand is not supported. 
        * @leave KErrArgument if aValue is not valid. 
        */
        IMPORT_C void CustomCommandL( TInt aCommand, TInt aValue ); 
        
    private:
        
        /**
        * Internal function used for handling initialisation. 
        */
        void DoInitialiseSynthesisL( const TDesC& aText, 
                                     const TFileName& aFileName, 
                                     TRequestStatus& aStatus ); 
        
    };
    
#endif // SPEECHSYNTHESIS_H 
