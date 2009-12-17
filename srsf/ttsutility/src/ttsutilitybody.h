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
* Description:  Body behind TTS Utility API
*
*/


#ifndef TTSUTILITYBODY_H
#define TTSUTILITYBODY_H

// INCLUDES
#include <nssttsutility.h>
#include <mdaaudiosampleplayer.h>
#include <nssttscustomcommands.h>
#include "srsfbldvariant.hrh"

// CLASS DECLARATION

/**
* Implementation for TTS Utility API.
*
* @lib TtsUtility.lib
* @since 2.8
*/
NONSHARABLE_CLASS( CTtsUtilityBody ) : public CBase, 
                                       public MMdaAudioPlayerCallback, 
                                       public MMMFControllerEventMonitorObserver
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTtsUtilityBody* NewL( MTtsClientUtilityObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CTtsUtilityBody();
        
    public: // New functions
        
        
        /**
        * Opens connection to a specific plugin.
        *
        * @param "TUid aUid" Plugin UID which will be instantiated
        */
        void OpenPluginL( TUid aUid );
        
        /**
        * Lists UIDs of available Text-To-Speech plugins.
        *
        * @param "RArray<TUid>& aUids" Output parameter which will contain the
        *                              list of plugin UIDs after function call.
        */
        void ListPluginsL( RArray<TUid>& aUids );        
       
        /**
        * Sets the default style parameters for synthesis.
        *
        * @param "const TTtsStyle& aStyle" Style created by the client
        */
        void SetDefaultStyleL( const TTtsStyle& aStyle );
        
        /**
        * Returns the registered default style
        *
        * @return Style reference
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */
        TTtsStyle& DefaultStyleL();
        
        /**
        * Sets the speaking rate of synthesizer.
        *
        * @param TInt aRate Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        */        
        void SetSpeakingRateL( TInt aRate );
        
        /**
        * Returns the current speaking rate value.
        *
        * @return Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */
        TInt SpeakingRateL();
       
        /**
        * Returns the list of supported languages.
        * 
        * @param "RArray<TLanguage>& aLanguages" Output parameter which contains the languages
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */
        void GetSupportedLanguagesL( RArray<TLanguage>& aLanguages );
        
        /**
        * Returns the list of supported voices for a certain language.
        * 
        * @param "TLanguage aLanguage" Language
        * @param "RArray<TVoice>& aVoices" Output parameter which contains the voices
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */
        void GetSupportedVoicesL( TLanguage aLanguage, RArray<TTtsStyle>& aVoices );
        
        /**
        * Open a descriptor ready for playback
        *
        * @param "const TDesC& aDescriptor" Descriptor containing the text to be synthesized.
        * @since 3.2        
        */
        void OpenDesL( const TDesC& aDescriptor ); 

        /**
        * Opens a descriptor and plays it.
        *
        * @param "const TDesC& aDescriptor" Descriptor to play 
        * @since 3.2
        */
        void OpenAndPlayDesL( const TDesC& aDescriptor );          
        
        /**
        * Directing output of a next synthesis to the specified file
        *
        * @param "RFile& aFile" Open file handle to save output of next synthesis.
        * @since 3.2
        */
        void SetOutputFileL( const RFile& aFile );
        
        /**
        * Adds new style to style collection.
        *
        * @param "TTtsStyle& aStyle" Reference to style.
        * @return Assigned style ID.
        */
        TTtsStyleID AddStyleL( const TTtsStyle& aStyle );

        /**
        * Deletes style from the style collection.
        *
        * @param "TTtsStyle& aStyle" Reference to style.
        * @return Error code.
        */
        TInt DeleteStyle( TTtsStyleID aID );

        /**
        * Returns number of styles.
        *
        * @return Number of styles.
        */
        TUint16 NumberOfStyles();

        /**
        * Returns style based on style ID
        *
        * @param "TTtsStyleID aStyleID" Style identifier.
        * @return Reference to found style.
        */
        TTtsStyle& StyleL( TTtsStyleID aStyleID );

        /**
        * Returns style based on index.
        *
        * @param "TUint16 aIndex" Index.
        * @return Reference to found style.
        */
        TTtsStyle& StyleL( TUint16 aIndex );

        /**
        * Begins playback of the initialised audio sample at the current volume
        * and priority levels.
        *
        * When playing of the audio sample is complete, successfully or
        * otherwise, the callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is
        * called. This function raises a CMdaAudioPlayerUtility 1 panic if the
        * audio player utility is not initialised.
        */
        void Play();

        /**
        * Stops playback of the audio sample as soon as possible.
        *
        * If the audio sample is playing, playback is stopped as soon as
        * possible. If playback is already complete, nothing further happens as
        * a result of calling this function. The callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is not
        * called.The function raises a CMdaAudioPlayerUtility 1 panic if the
        * audio player utility is not initialised.
        */
        void Stop();
        
        /**
        * Changes the current playback volume to a specified value.
        *
        * The volume can be changed before or during playback and is effective
        * immediately. The function raises a CMdaAudioPlayerUtility 1 panic if
        * the audio player utility is not initialised.
        *
        * @param	"TInt aVolume"
        *          The volume setting. This can be any value from zero to
        *          the value returned by a call to
        *          <code>CMdaAudioPlayerUtility::MaxVolume()</code>.
        *          Setting a zero value mutes the sound. Setting the
        *          maximum value results in the loudest possible sound.
        */
        void SetVolume( TInt aVolume );
        
        /**
        * Sets the number of times the audio sample is to be repeated during the
        * playback operation.
        *
        * A period of silence can follow each playing of the sample. The audio
        * sample can be repeated indefinitely.
        *
        * @param	"TInt aRepeatNumberOfTimes"
        *          The number of times the audio sample, together with
        *          the trailing silence, is to be repeated. If this is
        *          set to <code>KMdaRepeatForever</code>, then the audio
        *          sample, together with the trailing silence, is
        *          repeated indefinitely or until <code>Stop()</code> is
        *          called. If this is set to zero, then the audio sample
        *          is not repeated.
        * @param   "const TTimeIntervalMicroSeconds& aTrailingSilence"
        *          The time interval of the trailing silence.
        */
        void SetRepeats( TInt aRepeatNumberOfTimes, 
                                  const TTimeIntervalMicroSeconds& aTrailingSilence );
        
        /**
        * Returns the duration of the audio sample.
        *
        * The function raises a CMdaAudioPlayerUtility 1 panic if the audio
        * player utility is not initialised.
        *
        * @returns "TTimeIntervalMicroSeconds&"
        *          The duration in microseconds
        */
        const TTimeIntervalMicroSeconds& Duration();
        
        /**
        * Returns an integer representing the maximum volume.
        *
        * This is the maximum value which can be passed to
        * <code>CMdaAudioPlayerUtility::SetVolume()</code>. The function raises a
        * CMdaAudioPlayerUtility 1 panic if the audio player utility is not
        * initialised.
        *
        * @returns "TInt"
        *          The maximum volume. This value is platform dependent
        *          but is always greater than or equal to one.
        */
        TInt MaxVolume();

        /**
        * Opens a descriptor source and plays it.
        *
        * @param "TDesC8& aDescriptor" Descriptor to play.
        */
        void OpenAndPlayDesL( const TDesC8& aDescriptor );

        /**
        * Opens a file source and plays it.
        *
        * @param "TDesC& aFileName" File to play.
        */
        void OpenAndPlayFileL( const TDesC& aFileName );

        /**
        * Opens a parsed text source and plays it.
        *
        * @param "CTtsParsedText& aText" Parsed text object to play.
        */
        void OpenAndPlayParsedTextL( CTtsParsedText& aText );

        /**
        * Open a clip from a file
        *
        * @param	"const TDesC& aFileName" 
        *			File to open
        * @leave	Leaves with one of the system wide error codes if a problem is encountered
        *			opening the file
        */
        void OpenFileL( const TDesC& aFileName );

        /**
        * Open a clip from a descriptor
        *
        * @param	"const TDesC8& aDescriptor" 
        *			Descriptor containing audio clip
        * @leave	Leaves with one of the system wide error codes if a problem is encountered
        *			opening the file
        */
        void OpenDesL( const TDesC8& aDescriptor );

        /**
        * Open parsed text source.
        *
        * @param "CTtsParsedText& aText" Reference to parsed text object.
        */
        void OpenParsedTextL( CTtsParsedText& aText );

        /**
        * Pauses playback of the audio clip
        * @returns One of the system-wide error codes
        */
        TInt Pause();
        
        /**
        * Closes the current audio clip (allowing another clip to be opened)
        */
        void Close();
        
        /**
        * Returns the current playback position in microseconds
        *
        * @retval "TTimeIntervalMicroSeconds& aPosition"
        *          The current time position in microseconds from the start of the file
        * @returns One of the global error codes
        */
        TInt GetPosition( TTimeIntervalMicroSeconds& aPosition );
        
        /**
        * Returns the current playback position in word index.
        *
        * @retval "TInt& aWordIndex" The current position in words.
        * @return Error code.
        */
        TInt GetPosition( TInt& aWordIndex );

        /**
        * Set the current playback position in microseconds from the start of the file
        *
        * @param "TTimeIntervalMicroSeconds& aPosition"
        *          The position to move to in microseconds past the start of the file
        */
        void SetPosition( const TTimeIntervalMicroSeconds& aPosition );
        
        /**
        * Set the current playback position in word index.
        *
        * @param "TInt aWordIndex" Playback position.
        */
        void SetPosition( TInt aWordIndex );

        /**
        *
        * Set the priority for playback. This is used to arbitrate between multiple 
        * objects trying to access a single sound device
        *
        * @param "TInt aPriority"
        * @param "TMdaPriorityPreference aPref"
        *         Quality/time preference tradeoff
        * 
        * @returns "TInt" One of the global error codes
        */
        TInt SetPriority( TInt aPriority, TMdaPriorityPreference aPref );
        
        /**
        * Returns the current playback volume
        *
        * @retval "TInt& aVolume"
        *          A volume value between 0 and the value returned by <code>MaxVolume()</code>
        * @returns "TInt" One of the global error codes
        */
        TInt GetVolume( TInt& aVolume );
        
        /**
        * Sets the current playback balance
        *
        * @param "TInt& aBalance"
        *        A value between <code>KMMFBalanceMaxLeft</code> 
        *        and <code>KMMFBalanceMaxRight</code>. The default value is 
        *        <code>KMMFBalanceCenter</code>
        * @returns "TInt" One of the global error codes
        */
        TInt SetBalance( TInt aBalance = KMMFBalanceCenter );
        
        /**
        * Returns the current playback balance
        *
        * @retval "TInt& aBalance"
        *         A value between <code>KMMFBalanceMaxLeft</code> 
        *         and <code>KMMFBalanceMaxRight</code>
        * @returns "TInt" One of the global error codes
        */
        TInt GetBalance( TInt& aBalance );
        
        /**
        * Send a synchronous custom command to the controller
        * @param "aDestination"	"The destination of the message, consisting of the uid of
        *        the interface of this message."
        * @param "aFunction" "The function number to indicate which function is to be called
        *        on the interface defined in the aDestination parameter."
        * @param "aDataTo1" "A reference to the first chunk of data to be copied to the controller 
        *        framework. The exact contents of the data are dependent on the 
        *        interface being called.  Can be KNullDesC8."
        * @param "aDataTo2" "A reference to the second chunk of data to be copied to the controller 
        *        framework. The exact contents of the data are dependent on the 
        *        interface being called.  Can be KNullDesC8."
        * @param "aDataFrom" "A reference to an area of memory to which the controller framework will 
        *        write any data to be passed back to the client.  Can't be KNullDesC8."
        * @return "The result of the request. Exact range of values is dependent on the interface."
        */
        TInt CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2, 
                                         TDes8& aDataFrom);
        /**
        * Send a synchronous custom command to the controller
        * @param "aDestination" "The destination of the message, consisting of the uid of
        *		 the interface of this message."
        * @param "aFunction" "The function number to indicate which function is to be called
        *        on the interface defined in the aDestination parameter."
        * @param "aDataTo1" "A reference to the first chunk of data to be copied to the controller 
        *         framework. The exact contents of the data are dependent on the 
        *         interface being called.  Can be KNullDesC8."
        * @param "aDataTo2" "A reference to the second chunk of data to be copied to the controller 
        *         framework. The exact contents of the data are dependent on the 
        *         interface being called.  Can be KNullDesC8."
        * @return "The result of the request. Exact range of values is dependent on the interface."
        */
        TInt CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2 );
        /**
        * Send a asynchronous custom command to the controller
        * Note: This method will return immediately.  The RunL of the active object owning the 
        * <code>aStatus<\code> parameter will be called when the command is completed by the 
        * controller framework.
        * @param "aDestination" "The destination of the message, consisting of the uid of
        *        the interface of this message."
        * @param "aFunction" "The function number to indicate which function is to be called
        *        on the interface defined in the aDestination parameter."
        * @param "aDataTo1" "A reference to the first chunk of data to be copied to the controller 
        *        framework. The exact contents of the data are dependent on the 
        *        interface being called.  Can be KNullDesC8."
        * @param "aDataTo2" "A reference to the second chunk of data to be copied to the controller 
        *        framework. The exact contents of the data are dependent on the 
        *        interface being called.  Can be KNullDesC8."
        * @param "aDataFrom" "A reference to an area of memory to which the controller framework will 
        *        write any data to be passed back to the client.  Can't be KNullDesC8."
        * @param "aStatus" "The TRequestStatus of an active object.  This will contain the 
        *        result of the request on completion.  The exact range of 
        *        result values is dependent on the interface."
        */
        void CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination,  
                                          TInt aFunction, 
                                          const TDesC8& aDataTo1, 
                                          const TDesC8& aDataTo2, 
                                          TDes8& aDataFrom, 
                                          TRequestStatus& aStatus );
        /**
        * Send a asynchronous custom command to the controller
        * Note: This method will return immediately.  The RunL of the active object owning the 
        * <code>aStatus<\code> parameter will be called when the command is completed by the 
        * controller framework.
        * @param 	"aDestination"	"The destination of the message, consisting of the uid of
        *							the interface of this message."
        * @param	"aFunction"		"The function number to indicate which function is to be called
        *							on the interface defined in the aDestination parameter."
        * @param	"aDataTo1"		"A reference to the first chunk of data to be copied to the controller 
        *							framework. The exact contents of the data are dependent on the 
        *							interface being called.  Can be KNullDesC8."
        * @param	"aDataTo2"		"A reference to the second chunk of data to be copied to the controller 
        *							framework. The exact contents of the data are dependent on the 
        *							interface being called.  Can be KNullDesC8."
        * @param 	"aStatus"		"The TRequestStatus of an active object.  This will contain the 
        *							result of the request on completion.  The exact range of 
        * 							result values is dependent on the interface."
        * @since					7.0s
        */
        void CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination, 
                                          TInt aFunction, 
                                          const TDesC8& aDataTo1, 
                                          const TDesC8& aDataTo2, 
                                          TRequestStatus& aStatus );

    public: // Functions from MMdaAudioPlayerCallback
        
        void MapcInitComplete( TInt /*aError*/, const TTimeIntervalMicroSeconds& /*aDuration*/ ) {}
        void MapcPlayComplete( TInt /*aError*/ ) {}


    public: // Functions from MMMFControllerEventMonitorObserver

        void HandleEvent( const TMMFEvent& aEvent );
      
    private:
        
        /**
        * C++ default constructor.
        *
        * @param "MTtsClientUtilityObserver& aObserver" Reference to observer.
        */
        CTtsUtilityBody( MTtsClientUtilityObserver& aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // Prohibit copy constructor if not deriving from CBase.
        CTtsUtilityBody( const CTtsUtilityBody& );
        // Prohibit assigment operator if not deriving from CBase.
        CTtsUtilityBody& operator=( const CTtsUtilityBody& );

        /**
        * Utility function to open connection to controller plugin.
        *
        * @param "TUid aPluginUid" Plugin UID
        */
        void OpenControllerConnectionL( TUid aPluginUid );

        /**
        * Utility function to copy parsed text structure and convert style ids.
        *
        * @param "CTtsParsedText& aText" Parsed text which is copied
        */        
        void CopyParsedTextL( CTtsParsedText& aText );

        /**
        * Checks if there is an existing connection to some controller plugin
        * 
        * @return ETrue if there is an connection
        */
        TBool IsPluginOpen();
        
        /**
        * Stores priority settings but does not send them to the controller
        * plugin just yet.
        *
        * @param "TInt aPriority" Audio priority 
        * @param "TMdaPriorityPreference aPref" Audio preference
        */
        void StorePriority( TInt aPriority, TMdaPriorityPreference aPref );
        
        /**
        * Handles conversion from unicode to utf-8
        *
        * @param "const TDesC& aDes" Unicode descriptor
        * @return Pointer to created buffer, pushed to cleanupstack
        */
        HBufC8* ConvertToUtf8LC( const TDesC& aDes );
        
        /**
        * Adds data sink to controller according iUidOfDataSink and iOutputFile. 
        */
        void AddDataSinkL();
        
        /**
        * Responds that the correct plugin is loaded 
        *
        * @param "TTtsQuality aQuality" Synthesis quality parameter
        * @param "TLanguage aLanguage" Synthesis language
        * @param "const TTtsVoice& aVoice" Synthesis voice
        */
        void CheckAndLoadCorrectPluginL( TTtsQuality aQuality, TLanguage aLanguage, 
                                         const TTtsVoice& aVoice );
        
        /**
        * Tells if currently loaded plugin can handle synthesis with given parameters
        * 
        * @param "TLanguage aLanguage" Language to be used
        * @param "const TTtsVoice& aVoice" Voice to be used
        * @return ETrue if parameters are supported
        */
        TBool IsSupportedSpeaker( TLanguage aLanguage, const TTtsVoice& aVoice );

    private: // Data

        // Client side representation of controller plugin in use
        RMMFController iController;

        // Audio custom commands
        RMMFAudioPlayDeviceCustomCommands iAudioPlayDeviceCommands;
        RMMFAudioPlayControllerCustomCommands iAudioPlayControllerCommands;
        
        // Tts custom commands
        RTtsCustomCommands iTtsCustomCommands;
        
        // Pointer to controller event monitor
        CMMFControllerEventMonitor* iControllerEventMonitor;
        
        // Reference to observer
        MTtsClientUtilityObserver& iObserver;
        
        // Source and sink handle info
        TMMFMessageDestination iSourceHandle;
        TMMFMessageDestination iSinkHandle;
        
        // Duration of the current TTS "clip"
        TTimeIntervalMicroSeconds iDuration;
        
        // Latest read position of synthesising
        TTimeIntervalMicroSeconds iPosition;
        
        // Plugin priority settings.
        TMMFPrioritySettings iPrioritySettings;

        // Duration of current TTS 'clip'
        TTimeIntervalMicroSeconds iTime;

        // Registered styles, these will be kept in two places: locally
        // in TtsUtility and in TtsHwDevice in server side.
        RArray<TTtsStyle> iStyles;

        // Registered style IDs. (internal to TtsUtility)
        RArray<TTtsStyleID> iInternalStyleIDs;

        // Registered style IDs. (plugin id)
        RArray<TTtsStyleID> iControllerStyleIDs;

        // Increased numbering for style ids
        TTtsStyleID iCurrentStyleID;

        // Flag to tell if playback should be started immediately after priming
        // has finished.
        TBool iPlayImmediately;

        // Counter to tell how many times playback should be repeated.
        TInt iRepeats;

        // Trailing silence when repeating the playback
        TTimeIntervalMicroSeconds32 iTrailingSilence;

        // UID of controller plugin which has been created
        TUid iOpenPlugin;
        
        // Etrue if user has opened plugin by calling function OpenPluginL()
        TBool iPluginOpenedExplicitly;
    
        // Parsed text which is given to the controller plugin
        CTtsParsedText* iParsedText;
    
        // Default style structure
        TTtsStyle iDefaultStyle;
        
        // Uid of data sink to be used 
        TUid iUidOfDataSink;
        
        // Output file handle if data sink is set to file output
        RFile iFile;
    };

#endif // TTSUTILITY_H   

// End of File



