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
* Description:  Base class for TTS Utility API
*
*/


#ifndef NSSTTSUTILITYBASE_H
#define NSSTTSUTILITYBASE_H

// INCLUDES
#include <mdaaudiosampleplayer.h>
#include <nssttscommon.h>
#include <nssttsutilityobserver.h>

// CLASS DECLARATION

/**
* Tts Utility interface.  
*
* @since 3.1
*/
class MTtsUtilityBase
    {
    public: // New functions
        
        /**
        * 2nd phase constructor which can be called from outside.
        * 
        * @since 3.1
        * @param "const MTtsClientUtilityObserver& aObserver" Observer reference.
        */
        virtual void CreateInstanceSecondPhaseL( MTtsClientUtilityObserver& aObserver ) = 0;   
        
        /**
        * Destructor. Defined here so that client can use M-class pointer to destroy
        * the concrete C-object
        */                                         
        virtual ~MTtsUtilityBase() {};        
        
        /**
        * Opens connection to a specific plugin.
        *
        * @param "TUid aUid" Plugin UID which will be instantiated
        * @since 3.2        
        */
        virtual void OpenPluginL( TUid aUid ) = 0;
        
        /**
        * Lists UIDs of available Text-To-Speech plugins.
        *
        * @param "RArray<TUid>& aUids" Output parameter which will contain the
        *                              list of plugin UIDs after function call.
        * @since 3.2         
        */
        virtual void ListPluginsL( RArray<TUid>& aUids ) = 0;              
                       
        /**
        * Sets the default style parameters for synthesis.
        *
        * @param "const TTtsStyle& aStyle" Style created by the client
        * @since 3.2        
        */
        virtual void SetDefaultStyleL( const TTtsStyle& aStyle ) = 0;
        
        /**
        * Returns the registered default style
        *
        * @return Style reference
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2             
        */
        virtual TTtsStyle& DefaultStyleL() = 0;
        
        /**
        * Sets the speaking rate of synthesizer.
        *
        * @param TInt aRate Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @since 3.2        
        */        
        virtual void SetSpeakingRateL( TInt aRate ) = 0;
        
        /**
        * Returns the current speaking rate value.
        *
        * @return Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2        
        */
        virtual TInt SpeakingRateL() = 0;
       
        /**
        * Open a descriptor ready for playback
        *
        * @param "const TDesC& aDescriptor" Descriptor containing the text to be synthesized.
        * @since 3.2        
        */
        virtual void OpenDesL( const TDesC& aDescriptor ) = 0; 

        /**
        * Opens a descriptor and plays it.
        *
        * @param "const TDesC& aDescriptor" Descriptor to play 
        * @since 3.2        
        */
        virtual void OpenAndPlayDesL( const TDesC& aDescriptor ) = 0;    
        
        /**
        * Returns the list of supported languages.
        * 
        * @param "RArray<TLanguage>& aLanguages" Output parameter which contains the languages
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2        
        */
        virtual void GetSupportedLanguagesL( RArray<TLanguage>& aLanguages ) = 0;
        
        /**
        * Returns the list of supported voices for a certain language.
        * 
        * @param "TLanguage aLanguage" Language
        * @param "RArray<TTtsStyle>& aVoices" Output parameter which contains the voices
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2
        */        
        virtual void GetSupportedVoicesL( TLanguage aLanguage, RArray<TTtsStyle>& aVoices ) = 0;        
        
        /**
        * Sets data sink to be a file for the next synthesis event
        *
        * @param "RFile& aFile" Open file handle where the output is written
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2
        */
        virtual void SetOutputFileL( const RFile& aFile ) = 0; 
        
        /**
        * Adds new style to style collection.
        *
        * @since 3.1
        * @param "TTtsStyle& aStyle" Reference to style.
        * @return Assigned style ID.
        */
        virtual TTtsStyleID AddStyleL( const TTtsStyle& aStyle ) = 0;

        /**
        * Deletes style from the style collection.
        *
        * @since 3.1
        * @param "TTtsStyle& aStyle" Reference to style.
        * @return Error code.
        */
        virtual TInt DeleteStyle( TTtsStyleID aID ) = 0;

        /**
        * Returns number of styles.
        *
        * @since 3.1
        * @return Number of styles.
        */
        virtual TUint16 NumberOfStyles() = 0;

        /**
        * Returns style based on style ID
        *
        * @since 3.1
        * @param "TTtsStyleID aStyleID" Style identifier.
        * @return Reference to found style.
        */
        virtual TTtsStyle& StyleL( TTtsStyleID aStyleID ) = 0;

        /**
        * Returns style based on index.
        *
        * @since 3.1
        * @param "TUint16 aIndex" Index.
        * @return Reference to found style.
        */
        virtual TTtsStyle& StyleL( TUint16 aIndex ) = 0;

        /**
        * Begins playback of the initialised audio sample at the current volume
        * and priority levels.
        *
        * When playing of the audio sample is complete, successfully or
        * otherwise, the callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is
        * called. This function raises a CMdaAudioPlayerUtility 1 panic if the
        * audio player utility is not initialised.
        *
        * @since 3.1
        */
        virtual void Play() = 0;

        /**
        * Stops playback of the audio sample as soon as possible.
        *
        * If the audio sample is playing, playback is stopped as soon as
        * possible. If playback is already complete, nothing further happens as
        * a result of calling this function. The callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is not
        * called.The function raises a CMdaAudioPlayerUtility 1 panic if the
        * audio player utility is not initialised.
        *
        * @since 3.1
        */
        virtual void Stop() = 0;
        
        /**
        * Changes the current playback volume to a specified value.
        *
        * The volume can be changed before or during playback and is effective
        * immediately. The function raises a CMdaAudioPlayerUtility 1 panic if
        * the audio player utility is not initialised.
        *
        * @since 3.1
        * @param	"TInt aVolume"
        *          The volume setting. This can be any value from zero to
        *          the value returned by a call to
        *          <code>CMdaAudioPlayerUtility::MaxVolume()</code>.
        *          Setting a zero value mutes the sound. Setting the
        *          maximum value results in the loudest possible sound.
        */
        virtual void SetVolume( TInt aVolume ) = 0;
        
        /**
        * Sets the number of times the audio sample is to be repeated during the
        * playback operation.
        *
        * A period of silence can follow each playing of the sample. The audio
        * sample can be repeated indefinitely.
        *
        * @since 3.1
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
        virtual void SetRepeats( TInt aRepeatNumberOfTimes, 
                                 const TTimeIntervalMicroSeconds& aTrailingSilence ) = 0;
        
        /**
        * Returns the duration of the audio sample.
        *
        * The function raises a CMdaAudioPlayerUtility 1 panic if the audio
        * player utility is not initialised.
        *
        * @since 3.1
        * @returns "TTimeIntervalMicroSeconds&"
        *          The duration in microseconds
        */
        virtual const TTimeIntervalMicroSeconds& Duration() = 0;
        
        /**
        * Returns an integer representing the maximum volume.
        *
        * This is the maximum value which can be passed to
        * <code>CMdaAudioPlayerUtility::SetVolume()</code>. The function raises a
        * CMdaAudioPlayerUtility 1 panic if the audio player utility is not
        * initialised.
        *
        * @since 3.1
        * @returns "TInt"
        *          The maximum volume. This value is platform dependent
        *          but is always greater than or equal to one.
        */
        virtual TInt MaxVolume() = 0;
        
        /**
        * Opens a descriptor source and plays it.
        *
        * @since 3.1
        * @param "TDesC8& aDescriptor" Descriptor to play.
        */
        virtual void OpenAndPlayDesL( const TDesC8& aDescriptor ) = 0;

        /**
        * Opens a file source and plays it.
        *
        * @since 3.1
        * @param "TDesC& aFileName" File to play.
        */
        virtual void OpenAndPlayFileL( const TDesC& aFileName ) = 0;

        /**
        * Opens a parsed text source and plays it.
        *
        * @since 3.1
        * @param "CTtsParsedText& aText" Parsed text object to play.
        */
        virtual void OpenAndPlayParsedTextL( CTtsParsedText& aText ) = 0;

        /**
        * Open a clip from a file
        *
        * @since 3.1
        * @param	"const TDesC& aFileName" 
        *			File to open
        * @leave	Leaves with one of the system wide error codes if a problem is encountered
        *			opening the file
        */
        virtual void OpenFileL( const TDesC& aFileName ) = 0;

        /**
        * Open a clip from a descriptor
        *
        * @since 3.1
        * @param	"const TDesC8& aDescriptor" 
        *			Descriptor containing audio clip
        * @leave	Leaves with one of the system wide error codes if a problem is encountered
        *			opening the file
        */
        virtual void OpenDesL( const TDesC8& aDescriptor ) = 0;

        /**
        * Open parsed text source.
        *
        * @since 3.1
        * @param "CTtsParsedText& aText" Reference to parsed text object.
        */
        virtual void OpenParsedTextL( CTtsParsedText& aText ) = 0;

        /**
        * Pauses playback of the audio clip
        *
        * @since 3.1
        * @returns One of the system-wide error codes
        */
        virtual TInt Pause() = 0;
        
        /**
        * Closes the current audio clip (allowing another clip to be opened)
        *
        * @since 3.1
        */
        virtual void Close() = 0;
        
        /**
        * Returns the current playback position in microseconds
        *
        * @since 3.1
        * @retval "TTimeIntervalMicroSeconds& aPosition"
        *          The current time position in microseconds from the start of the file
        * @returns One of the global error codes
        */
        virtual TInt GetPosition( TTimeIntervalMicroSeconds& aPosition ) = 0;
        
        /**
        * Returns the current playback position in word index.
        *
        * @since 3.1
        * @retval "TInt& aWordIndex" The current position in words.
        * @return Error code.
        */
        virtual TInt GetPosition( TInt& aWordIndex ) = 0;

        /**
        * Set the current playback position in microseconds from the start of the file
        *
        * @since 3.1
        * @param "TTimeIntervalMicroSeconds& aPosition"
        *          The position to move to in microseconds past the start of the file
        */
        virtual void SetPosition( const TTimeIntervalMicroSeconds& aPosition ) = 0;
        
        /**
        * Set the current playback position in word index.
        *
        * @since 3.1
        * @param "TInt aWordIndex" Playback position.
        */
        virtual void SetPosition( TInt aWordIndex ) = 0;

        /**
        *
        * Set the priority for playback. This is used to arbitrate between multiple 
        * objects trying to access a single sound device
        *
        * @since 3.1
        * @param "TInt aPriority"
        * @param "TMdaPriorityPreference aPref"
        *         Quality/time preference tradeoff
        * 
        * @returns "TInt" One of the global error codes
        */
        virtual TInt SetPriority( TInt aPriority, TMdaPriorityPreference aPref ) = 0;
        
        /**
        * Returns the current playback volume
        *
        * @since 3.1
        * @retval "TInt& aVolume"
        *          A volume value between 0 and the value returned by <code>MaxVolume()</code>
        * @returns "TInt" One of the global error codes
        */
        virtual TInt GetVolume( TInt& aVolume ) = 0;
        
        /**
        * Sets the current playback balance
        *
        * @since 3.1
        * @param "TInt& aBalance"
        *        A value between <code>KMMFBalanceMaxLeft</code> 
        *        and <code>KMMFBalanceMaxRight</code>. The default value is 
        *        <code>KMMFBalanceCenter</code>
        * @returns "TInt" One of the global error codes
        */
        virtual TInt SetBalance( TInt aBalance = KMMFBalanceCenter ) = 0;
        
        /**
        * Returns the current playback balance
        *
        * @since 3.1
        * @retval "TInt& aBalance"
        *         A value between <code>KMMFBalanceMaxLeft</code> 
        *         and <code>KMMFBalanceMaxRight</code>
        * @returns "TInt" One of the global error codes
        */
        virtual TInt GetBalance( TInt& aBalance ) = 0;
        
        /**
        * Send a synchronous custom command to the controller
        *
        * @since 3.1
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
        virtual TInt CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                        TInt aFunction, 
                                        const TDesC8& aDataTo1, 
                                        const TDesC8& aDataTo2, 
                                        TDes8& aDataFrom) = 0;
        /**
        * Send a synchronous custom command to the controller
        *
        * @since 3.1
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
        virtual TInt CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                        TInt aFunction, 
                                        const TDesC8& aDataTo1, 
                                        const TDesC8& aDataTo2 ) = 0;
        /**
        * Send a asynchronous custom command to the controller
        * Note: This method will return immediately.  The RunL of the active object owning the 
        * <code>aStatus<\code> parameter will be called when the command is completed by the 
        * controller framework.
        *
        * @since 3.1
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
        virtual void CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination,  
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2, 
                                         TDes8& aDataFrom, 
                                         TRequestStatus& aStatus ) = 0;
        /**
        * Send a asynchronous custom command to the controller
        * Note: This method will return immediately.  The RunL of the active object owning the 
        * <code>aStatus<\code> parameter will be called when the command is completed by the 
        * controller framework.
        *
        * @since 3.1
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
        * @param "aStatus" "The TRequestStatus of an active object.  This will contain the 
        *        result of the request on completion.  The exact range of 
        *        result values is dependent on the interface."
        */
        virtual void CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination, 
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2, 
                                         TRequestStatus& aStatus ) = 0;
    };

#endif // NSSTTSUTILITYBASE_H   

// End of File
