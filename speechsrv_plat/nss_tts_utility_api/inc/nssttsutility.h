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
* Description:  Utility API for client to access TTS functionality.
*
*/


#ifndef NSSTTSUTILITY_H
#define NSSTTSUTILITY_H

// INCLUDES
#include <mdaaudiosampleplayer.h>
#include <nssttscommon.h>
#include <nssttsutilitybase.h>
#include <nssttsutilityobserver.h>

// FORWARD DECLARATIONS
class CTtsUtilityBody;

// CLASS DECLARATION

/**
* Tts Utility interface.  
*
* @lib nssttsutility.lib
* @since 2.8
*/
class CTtsUtility : public CBase, public MTtsUtilityBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @since 2.8
        * @param "MTtsClientUtilityObserver& aObserver" Observer reference.
        * @return Pointer to created object.
        */
        IMPORT_C static CTtsUtility* NewL( MTtsClientUtilityObserver& aObserver );
 
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CTtsUtility();

        /*
        * Creates instance of CTtsUtility
        * Needed when dynamically loading nssttsutility.dll
        * Direct users of nssttsutility.lib should not use this.
        * Returns M-class pointer to MTtsUtilityBase as TAny*        
        *
        * @return Created instance
        * @see CreateInstanceSecondPhaseL
        */
        IMPORT_C static TAny* CreateInstanceL();

        /**
        * Does the second phase initialization of CTtsUtility
        * Needed when dynamically loading nssttsutility.dll
        * Direct users of nssttsutility.lib should not use this        
        *
        * @param "const MTtsClientUtilityObserver& aObserver" Observer reference
        */
        IMPORT_C void CreateInstanceSecondPhaseL( MTtsClientUtilityObserver& aObserver );   
        
    public: // New functions in 3.2

        /**
        * Opens connection to a specific plugin.
        *
        * @param "TUid aUid" Plugin UID which will be instantiated
        * @since 3.2        
        */
        IMPORT_C void OpenPluginL( TUid aUid );
        
        /**
        * Lists UIDs of available Text-To-Speech plugins.
        *
        * @param "RArray<TUid>& aUids" Output parameter which will contain the
        *                              list of plugin UIDs after function call.
        * @since 3.2         
        */
        IMPORT_C void ListPluginsL( RArray<TUid>& aUids );              
                       
        /**
        * Sets the default style parameters for synthesis.
        *
        * @param "const TTtsStyle& aStyle" Style created by the client
        * @since 3.2        
        */
        IMPORT_C void SetDefaultStyleL( const TTtsStyle& aStyle );
        
        /**
        * Returns the registered default style
        *
        * @return Style reference
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2             
        */
        IMPORT_C TTtsStyle& DefaultStyleL();
        
        /**
        * Sets the speaking rate of synthesizer.
        *
        * @param TInt aRate Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @since 3.2        
        */        
        IMPORT_C void SetSpeakingRateL( TInt aRate );
        
        /**
        * Returns the current speaking rate value.
        *
        * @return Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2        
        */
        IMPORT_C TInt SpeakingRateL();
       
        /**
        * Open a descriptor ready for playback
        *
        * @param "const TDesC& aDescriptor" Descriptor containing the text to be synthesized.
        * @since 3.2        
        */
        IMPORT_C void OpenDesL( const TDesC& aDescriptor ); 

        /**
        * Opens a descriptor and plays it.
        *
        * @param "const TDesC& aDescriptor" Descriptor to play 
        * @since 3.2        
        */
        IMPORT_C void OpenAndPlayDesL( const TDesC& aDescriptor );  
        
        /**
        * Returns the list of supported languages.
        * 
        * @param "RArray<TLanguage>& aLanguages" Output parameter which contains the languages
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2        
        */
        IMPORT_C void GetSupportedLanguagesL( RArray<TLanguage>& aLanguages );
        
        /**
        * Returns the list of supported voices for a certain language.
        * 
        * @param "TLanguage aLanguage" Language
        * @param "RArray<TTtsStyle>& aVoices" Output parameter which contains the voices
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2
        */        
        IMPORT_C void GetSupportedVoicesL( TLanguage aLanguage, RArray<TTtsStyle>& aVoices );
        
        /**
        * Sets data sink to be a file for the next synthesis event
        *
        * @param "RFile& aFile" Open file handle where the output is written 
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2
        */
        IMPORT_C void SetOutputFileL( const RFile& aFile ); 
        
    public: // New functions          
        
        /**
        * Adds new style to style collection.
        *
        * @since 2.8
        * @param "TTtsStyle& aStyle" Reference to style.
        * @return Assigned style ID.
        */
        IMPORT_C TTtsStyleID AddStyleL( const TTtsStyle& aStyle );

        /**
        * Deletes style from the style collection.
        *
        * @since 2.8
        * @param "TTtsStyle& aStyle" Reference to style.
        * @return Error code.
        */
        IMPORT_C TInt DeleteStyle( TTtsStyleID aID );

        /**
        * Returns number of styles.
        *
        * @since 2.8
        * @return Number of styles.
        */
        IMPORT_C TUint16 NumberOfStyles();

        /**
        * Returns style based on style ID
        *
        * @since 2.8
        * @param "TTtsStyleID aStyleID" Style identifier.
        * @return Reference to found style.
        */
        IMPORT_C TTtsStyle& StyleL( TTtsStyleID aStyleID );

        /**
        * Returns style based on index.
        *
        * @since 2.8
        * @param "TUint16 aIndex" Index.
        * @return Reference to found style.
        */
        IMPORT_C TTtsStyle& StyleL( TUint16 aIndex );

        /**
        * Begins playback of the initialised audio sample at the current volume
        * and priority levels.
        *
        * When playing of the audio sample is complete, successfully or
        * otherwise, the callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is
        * called. 
        *
        * @since 2.8
        */
        IMPORT_C void Play();

        /**
        * Stops playback of the audio sample as soon as possible.
        *
        * If the audio sample is playing, playback is stopped as soon as
        * possible. If playback is already complete, nothing further happens as
        * a result of calling this function. The callback function
        * <code>MMdaAudioPlayerCallback::MapcPlayComplete()</code> is not
        * called.
        *
        * @since 2.8
        */
        IMPORT_C void Stop();
        
        /**
        * Changes the current playback volume to a specified value.
        *
        * The volume can be changed before or during playback and is effective
        * immediately. The function raises a CMdaAudioPlayerUtility 1 panic if
        * the audio player utility is not initialised.
        *
        * @since 2.8
        * @param	"TInt aVolume"
        *          The volume setting. This can be any value from zero to
        *          the value returned by a call to
        *          <code>CMdaAudioPlayerUtility::MaxVolume()</code>.
        *          Setting a zero value mutes the sound. Setting the
        *          maximum value results in the loudest possible sound.
        */
        IMPORT_C void SetVolume( TInt aVolume );
        
        /**
        * Sets the number of times the audio sample is to be repeated during the
        * playback operation.
        *
        * A period of silence can follow each playing of the sample. The audio
        * sample can be repeated indefinitely.
        *
        * @since 2.8
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
        IMPORT_C void SetRepeats( TInt aRepeatNumberOfTimes, 
                                  const TTimeIntervalMicroSeconds& aTrailingSilence );
        
        /**
        * Returns the duration of the audio sample.
        *
        * The function raises a CMdaAudioPlayerUtility 1 panic if the audio
        * player utility is not initialised.
        *
        * @since 2.8
        * @returns "TTimeIntervalMicroSeconds&"
        *          The duration in microseconds
        */
        IMPORT_C const TTimeIntervalMicroSeconds& Duration();
        
        /**
        * Returns an integer representing the maximum volume.
        *
        * This is the maximum value which can be passed to
        * <code>CMdaAudioPlayerUtility::SetVolume()</code>. The function raises a
        * CMdaAudioPlayerUtility 1 panic if the audio player utility is not
        * initialised.
        *
        * @since 2.8
        * @returns "TInt"
        *          The maximum volume. This value is platform dependent
        *          but is always greater than or equal to one.
        */
        IMPORT_C TInt MaxVolume();
        
        /**
        * Opens a descriptor source and plays it.
        *
        * @since 2.8
        * @param "TDesC8& aDescriptor" Descriptor to play (in UTF-8 format).
        */
        IMPORT_C void OpenAndPlayDesL( const TDesC8& aDescriptor );

        /**
        * Opens a file source and plays it.
        *
        * @since 2.8
        * @param "TDesC& aFileName" File to play.
        */
        IMPORT_C void OpenAndPlayFileL( const TDesC& aFileName );

        /**
        * Opens a parsed text source and plays it.
        *
        * @since 2.8
        * @param "CTtsParsedText& aText" Parsed text object to play.
        */
        IMPORT_C void OpenAndPlayParsedTextL( CTtsParsedText& aText );

        /**
        * Open a clip from a file
        *
        * @since 2.8
        * @param	"const TDesC& aFileName" 
        *			File to open
        * @leave	Leaves with one of the system wide error codes if a problem is encountered
        *			opening the file
        */
        IMPORT_C void OpenFileL( const TDesC& aFileName );

        /**
        * Open a clip from a descriptor
        *
        * @since 2.8
        * @param	"const TDesC8& aDescriptor" 
        *			Descriptor containing audio clip
        * @leave	Leaves with one of the system wide error codes if a problem is encountered
        *			opening the file
        */
        IMPORT_C void OpenDesL( const TDesC8& aDescriptor );

        /**
        * Open parsed text source.
        *
        * @since 2.8
        * @param "CTtsParsedText& aText" Reference to parsed text object.
        */
        IMPORT_C void OpenParsedTextL( CTtsParsedText& aText );

        /**
        * Pauses playback of text to speech
        *
        * @since 2.8
        * @returns One of the system-wide error codes
        */
        IMPORT_C TInt Pause();
        
        /**
        * Closes the connection to the controller plugin.
        *
        * @since 2.8
        */
        IMPORT_C void Close();
        
        /**
        * Returns the current playback position in microseconds
        *
        * @since 2.8
        * @retval "TTimeIntervalMicroSeconds& aPosition"
        *          The current time position in microseconds from the start of the file
        * @returns One of the global error codes
        */
        IMPORT_C TInt GetPosition( TTimeIntervalMicroSeconds& aPosition );
        
        /**
        * Returns the current playback position in word index.
        *
        * @since 2.8
        * @retval "TInt& aWordIndex" The current position in words.
        * @return Error code.
        */
        IMPORT_C TInt GetPosition( TInt& aWordIndex );

        /**
        * Set the current playback position in microseconds from the start of the file
        *
        * @since 2.8
        * @param "TTimeIntervalMicroSeconds& aPosition"
        *          The position to move to in microseconds past the start of the file
        */
        IMPORT_C void SetPosition( const TTimeIntervalMicroSeconds& aPosition );
        
        /**
        * Set the current playback position in word index.
        *
        * @since 2.8
        * @param "TInt aWordIndex" Playback position.
        */
        IMPORT_C void SetPosition( TInt aWordIndex );

        /**
        *
        * Set the priority for playback. This is used to arbitrate between multiple 
        * objects trying to access a single sound device
        *
        * @since 2.8
        * @param "TInt aPriority" Audio priority for playback
        * @param "TMdaPriorityPreference aPref" Preference value for playback
        * 
        * @returns "TInt" One of the global error codes
        */
        IMPORT_C TInt SetPriority( TInt aPriority, TMdaPriorityPreference aPref );
        
        /**
        * Returns the current playback volume
        *
        * @since 2.8
        * @retval "TInt& aVolume"
        *          A volume value between 0 and the value returned by <code>MaxVolume()</code>
        * @returns "TInt" One of the global error codes
        */
        IMPORT_C TInt GetVolume( TInt& aVolume );
        
        /**
        * Sets the current playback balance
        *
        * @since 2.8
        * @param "TInt& aBalance"
        *        A value between <code>KMMFBalanceMaxLeft</code> 
        *        and <code>KMMFBalanceMaxRight</code>. The default value is 
        *        <code>KMMFBalanceCenter</code>
        * @returns "TInt" One of the global error codes
        */
        IMPORT_C TInt SetBalance( TInt aBalance = KMMFBalanceCenter );
        
        /**
        * Returns the current playback balance
        *
        * @since 2.8
        * @retval "TInt& aBalance"
        *         A value between <code>KMMFBalanceMaxLeft</code> 
        *         and <code>KMMFBalanceMaxRight</code>
        * @returns "TInt" One of the global error codes
        */
        IMPORT_C TInt GetBalance( TInt& aBalance );
        
        /**
        * Send a synchronous custom command to the controller
        *
        * @since 2.8
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
        IMPORT_C TInt CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2, 
                                         TDes8& aDataFrom);
        /**
        * Send a synchronous custom command to the controller
        *
        * @since 2.8
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
        IMPORT_C TInt CustomCommandSync( const TMMFMessageDestinationPckg& aDestination, 
                                         TInt aFunction, 
                                         const TDesC8& aDataTo1, 
                                         const TDesC8& aDataTo2 );
        /**
        * Send a asynchronous custom command to the controller
        * Note: This method will return immediately.  The RunL of the active object owning the 
        * <code>aStatus<\code> parameter will be called when the command is completed by the 
        * controller framework.
        *
        * @since 2.8
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
        IMPORT_C void CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination,  
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
        *
        * @since 2.8
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
        IMPORT_C void CustomCommandAsync( const TMMFMessageDestinationPckg& aDestination, 
                                          TInt aFunction, 
                                          const TDesC8& aDataTo1, 
                                          const TDesC8& aDataTo2, 
                                          TRequestStatus& aStatus );

    private:
        
        /**
        * C++ default constructor.
        *
        * @since 2.8
        */
        CTtsUtility();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        * 
        * @since 2.8
        * @param "MTtsClientUtilityObserver& aObserver" Observer reference.
        */
        void ConstructL( MTtsClientUtilityObserver& aObserver );
        
        // Prohibit copy constructor
        CTtsUtility( const CTtsUtility& );
        // Prohibit assigment operator
        CTtsUtility& operator=( const CTtsUtility& );
        
    private: // Data

        // Real implementation
        CTtsUtilityBody* iBody;

        // Reserved pointer for future extension
        TAny* iReserved;
        
    };

#endif // NSSTTSUTILITY_H   

// End of File
