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
* Description:  Class CTtsControllerPluginBody
*
*/



#ifndef TTSPLUGINBODY_H
#define TTSPLUGINBODY_H

// INCLUDES
#include <mmfcontroller.h>
#include <nssdevtts.h>

// FORWARD DECLARATIONS
class CTtsControllerPlugin;
class CTTSDataLoader;

// CLASS DECLARATION

/**
* Implementation class for Tts controller plugin.
*
* @lib TtsPlugin.lib
* @since 2.8
*/
class CTtsControllerPluginBody : public CActive, public MDevTTSObserver
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTtsControllerPluginBody* NewL( CTtsControllerPlugin* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CTtsControllerPluginBody();
        
    public: // New functions
        
        /**
        * Add a data source to the controller plugin.
        *
        *  @param "aDataSource"	"A reference to the data source to be added.
        *         The controller plugin may call <code>aDataSource.DataSourceType()
        *         <\code> to find out exactly what type of source it is."
        * @leave "" "The controller plugin may leave during this method.  
        *        If the controller plugin does not support the data source, it 
        *         should leave with <code>KErrNotSupported<\code>.
        */
        void AddDataSourceL( MDataSource& aDataSource );
        
        /**
        * Add a data sink to the controller plugin.
        *
        * @param "aDatasink" "A reference to the data sink to be added.
        *        The controller plugin may call <code>aDatasink.DatasinkType()
        *        <\code> to find out exactly what type of sink it is."
        * @leave "" "The controller plugin may leave during this method.
        *        If the controller plugin does not support the data sink, it 
        * should leave with <code>KErrNotSupported<\code>.
        */
        void AddDataSinkL( MDataSink& aDataSink );
        
        /**
        * Remove a data source from the controller plugin.
        *
        * @param "aDataSource" A reference to the data source to be removed.
        * @leave "" "The controller plugin may leave during this method.
        *        If the controller plugin does not support the removal of data 
        *        sources, it should leave with <code>KErrNotSupported<\code>.
        *        If the controller plugin leaves, the data source will not be 
        *        destroyed by the controller framework.  If it does not leave, 
        *        the data source will be destroyed.
        */
        void RemoveDataSourceL( MDataSource& aDataSource );
        
        /**
        * Remove a data sink from the controller plugin.
        *
        * @param "aDatasink" A reference to the data sink to be removed.
        * @leave "" "The controller plugin may leave during this method.
        * If the controller plugin does not support the removal of data sinks, 
        * it should leave with <code>KErrNotSupported<\code>. If the 
        * controller plugin leaves, the data sink will not be destroyed by 
        * the controller framework.  If it does not leave, the data sink
        * will be destroyed.
        */
        void RemoveDataSinkL( MDataSink& aDataSink );
        
        /**
        * Reset the controller plugin.
        *
        * The controller should revert back to its newly constructed state. If 
        * the Reset is successful (i.e. it doesn't leave), the controller 
        * framework will delete all objects added to the MMFObjectManager 
        * including any sources and sinks.
        *
        * @leave "" "The controller plugin may leave during this method.
        *        If the controller plugin does not support being reset, 
        *        it should leave with <code>KErrNotSupported<\code>."
        */
        void ResetL();
        
        /**
        * Prime the controller plugin.
        *
        * The controller must prepare to start playing, by initialising its 
        * sources, sinks and buffers. This moves the controller from the 
        * STOPPED to the PRIMED state.
        *
        * @leave "" "The controller plugin may leave during this method with
        * any of the standard system-wide error codes."
        */
        void PrimeL();
        
        /**
        * Commence playback.
        *
        * The controller must now begin transferring data from its source(s) 
        * to its sink(s). This moves the controller from the PRIMED to the 
        * PLAYING state.
        *
        * Note: this method must return once playing has commenced, and not 
        * wait until playing is complete.
        *
        * @leave "" "The controller plugin may leave during this method with 
        *        any of the standard system-wide error codes."
        */
        void PlayL();
        
        /**
        * Pause the controller plugin.
        *
        * The controller must now cease transferring data from its source(s) 
        * to its sink(s). This moves the controller from the PLAYING back to 
        * the PRIMED state.
        *
        * A subsequent call to <code>Play()<\code> will cause the controller 
        * plugin to resume playback from the point it was paused (unless 
        * there has been a call to <code>SetPosition()<\code> in the meantime.
        *
        * @leave "" "The controller plugin may leave during this method with 
        *        any of the standard system-wide error codes."
        */
        void PauseL();
        
        /**
        * Stop the controller plugin.
        *
        * The controller must now undo anything that occurred during the call 
        * to <code>Prime()<\code>. This moves the controller from the PRIMED 
        * back to the STOPPED state.
        *
        * @leave "" "The controller plugin may leave during this method with 
        *        any of the standard system-wide error codes."
        */
        void StopL();
        
        /**
        * Get the current position.
        *
        * The controller plugin should calculate the current position in microseconds.
        *
        * @return "The current position in microseconds."
        * @leave "" "The controller plugin may leave during this method with 
        *        any of the standard system-wide error codes."
        */
        TTimeIntervalMicroSeconds PositionL() const;
        
        /**
        * Set the current position.
        *
        * The controller plugin should reposition itself to the position provided.
        *
        * @param "The desired position in microseconds."
        * @leave "" "The controller plugin may leave during this method with
        *         any of the standard system-wide error codes."
        */
        void SetPositionL( const TTimeIntervalMicroSeconds& aPosition );
        
        /**
        * Get the duration of the clip.
        *
        * The controller plugin should calculate the clip duration in microseconds.
        *
        * @return "The clip duration in microseconds."
        * @leave "" "The controller plugin may leave during this method with
        *         any of the standard system-wide error codes."
        */
        TTimeIntervalMicroSeconds DurationL() const;
        
        /**
        * Set the priority settings.
        *
        * The new priority settings must be applied to any resource being used 
        * by the controller plugin that requires priority settings (for example 
        * the sound device).
        *
        * @param "aPrioritySettings" "The new priority settings"
        */
        void SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings );
        
        /**
        * Handle a custom command.
        *
        * Custom commands allow a controller plugin to define its own API.
        * If the controller framework does not understand a message from the 
        * client, it is assumed this is a custom command for the plugin and 
        * passed into this interface.
        *
        * The more common custom commands can be handled by Custom Command 
        * Parsers on behalf of the controller plugin.  This allows the 
        * controller plugin to implement a concrete interface (defined by 
        * mixin classes) rather than having to decode the command itself.
        * For more information, @see CMMFCustomCommandParserBase.
        *
        * The controller plugin must always complete the message passed 
        * into this method, even if it does not support the interface 
        * required by the message.
        *
        * @param "aMessage" "The message to be handled by the controller plugin."
        */
        void CustomCommand( TMMFMessage& aMessage );
        
        /**
        * Retrieve the number of meta data entries in the clip.
        *
        * @param "aNumberOfEntries" "The controller plugin must set this to 
        * the number of meta data entries in the clip."
        *
        * @leave "" "The controller plugin may leave during this method with 
        *        any of the standard system-wide error codes."
        */
        void GetNumberOfMetaDataEntriesL( TInt& aNumberOfEntries );
        
        /**
        * Retrieve a meta data entry from the clip.
        *
        * @param "aIndex" "The index of the meta data entry to retrieve."
        * @return "A pointer to a newly created CMMFMetaDataEntry object 
        *         containing the meta information. The controller framework 
        *         will take ownership of the object when this method returns."
        * @leave "" "The controller plugin may leave during this method with 
        *        any of the standard system-wide error codes."
        */
        CMMFMetaDataEntry* GetMetaDataEntryL( TInt aIndex );

       
    public: // MMMFAudioControllerCustomCommandImplementor
        
        /**
        Set the sample rate of the data source in samples per second.
        @param	"aSampleRate"	"The sample rate, in samples per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSourceSampleRateL(TUint aSampleRate);
        
        /**
        Set the bit rate of the data source in bits per second.
        @param	"aSampleRate"	"The bit rate, in bits per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSourceBitRateL(TUint aBitRate);
        
        /**
        Set the number of channels in the data source.
        For example, one channel for mono, two channels for stereo etc.
        @param	"aNumChannels"	"The number of channels."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSourceNumChannelsL(TUint aNumChannels);
        
        /**
        Set the format of the data source.  
        The UID corresponds to the uid of the CMMFFormat-derived ECOM plugin to be used.
        @param	"aFormatUid"	"The format plugin to be used."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSourceFormatL(TUid aFormatUid);
        
        /**
        Set the fourCC code of the source.
        @param	"aDataType"	"The fourCC code."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSourceDataTypeL(TFourCC aDataType);
        
        /**
        Set the sample rate of the data sink in samples per second.
        @param	"aSampleRate"	"The sample rate, in samples per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSinkSampleRateL(TUint aSampleRate);
        
        /**
        Set the bit rate of the data sink in bits per second.
        @param	"aSampleRate"	"The bit rate, in bits per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSinkBitRateL(TUint aRate);
        
        /**
        Set the number of channels in the data sink.
        For example, one channel for mono, two channels for stereo etc.
        @param	"aNumChannels"	"The number of channels."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSinkNumChannelsL(TUint aNumChannels);
        
        /**
        Set the format of the data sink.  
        The UID corresponds to the uid of the CMMFFormat-derived ECOM plugin to be used.
        @param	"aFormatUid"	"The format plugin to be used."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSinkFormatL(TUid aFormatUid);
        
        /**
        Set the fourCC code of the sink.
        @param	"aDataType"	"The fourCC code."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetSinkDataTypeL(TFourCC aDataType);
        
        /**
        Set the codec to be used.  Useful when recording data.
        @param	"aSourceDataType"	"The data type of the source of the codec."
        @param	"aSinkDataType"		"The data type of the sink of the codec."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacSetCodecL(TFourCC aSourceDataType, TFourCC aSinkDataType);
        
        
        /**
        Get the sample rate of the data source in samples per second.
        @param	"aRate"	"The sample rate, in samples per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSourceSampleRateL(TUint& aRate);
        
        /**
        Get the bit rate of the data source in bits per second.
        @param	"aRate"	"The bit rate, in bits per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSourceBitRateL(TUint& aRate);
        
        /**
        Get the number of channels in the data source.
        For example, one channel for mono, two channels for stereo etc.
        @param	"aNumChannels"	"The number of channels."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSourceNumChannelsL(TUint& aNumChannels);
        
        /**
        Get the format of the data source.  
        The UID corresponds to the uid of the CMMFFormat-derived ECOM plugin being used.
        @param	"aFormatUid"	"The format plugin being used."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSourceFormatL(TUid& aFormat);
        
        /**
        Get the fourCC code of the source.
        @param	"aDataType"	"The fourCC code."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSourceDataTypeL(TFourCC& aDataType);
        
        /**
        Get the sample rate of the data sink in samples per second.
        @param	"aRate"	"The sample rate, in samples per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSinkSampleRateL(TUint& aRate);
        
        /**
        Get the bit rate of the data sink in bits per second.
        @param	"aRate"	"The bit rate, in bits per second."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSinkBitRateL(TUint& aRate);
        
        /**
        Get the number of channels in the data sink.
        For example, one channel for mono, two channels for stereo etc.
        @param	"aNumChannels"	"The number of channels."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSinkNumChannelsL(TUint& aNumChannels);
        
        /**
        Get the format of the data sink.  
        The UID corresponds to the uid of the CMMFFormat-derived ECOM plugin being used.
        @param	"aFormatUid"	"The format plugin being used."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSinkFormatL(TUid& aFormat);
        
        /**
        Get the fourCC code of the sink.
        @param	"aDataType"	"The fourCC code."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSinkDataTypeL(TFourCC& aDataType);
        
        
        /**
        Get the list of sample rates supported by the data source.
        @param	"aSupportedRates"	"The supported rates.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSourceSampleRatesL(RArray<TUint>& aSupportedRates);
        
        /**
        Get the list of bit rates supported by the data source.
        @param	"aSupportedRates"	"The supported rates.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSourceBitRatesL(RArray<TUint>& aSupportedRates);
        
        /**
        Get the list of channels supported by the data source L(ie mono, stereo etc).
        @param	"aSupportedChannels"	"The supported channels.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSourceNumChannelsL(RArray<TUint>& aSupportedChannels);
        
        /**
        Get the list of fourCC codes supported by the data source.
        @param	"aSupportedDataTypes"	"The supported data types.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSourceDataTypesL(RArray<TFourCC>& aSupportedDataTypes);
        
        /**
        Get the list of sample rates supported by the data sink.
        @param	"aSupportedRates"	"The supported rates.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedRates);
        
        /**
        Get the list of bit rates supported by the data sink.
        @param	"aSupportedRates"	"The supported rates.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSinkBitRatesL(RArray<TUint>& aSupportedRates);
        
        /**
        Get the list of channels supported by the data sink L(ie mono, stereo etc).
        @param	"aSupportedChannels"	"The supported channels.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels);
        
        /**
        Get the list of fourCC codes supported by the data sink.
        @param	"aSupportedDataTypes"	"The supported data types.  Warning: Existing objects in this array will be removed by this method."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MacGetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes);


    public: // From MMMFAudioPlayDeviceCustomCommandImplementor

        /**
        Set the volume of the sound device.
        @param	"aVolume"	"The new volume"
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MapdSetVolumeL(TInt aVolume);
        
        /**
        Get the maximum volume supported by the sound device.
        @param	"aMaxVolume"	"The maximum volume, to be filled in by the controller plugin."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MapdGetMaxVolumeL(TInt& aMaxVolume);
        
        /**
        Get the current playback volume.
        @param	"aVolume"	"The volume, to be filled in by the controller."
        @leave	"This function can leave with one of the system-wide error codes. The request will be
        completed with the leave code."
        @since 7.0s
        */
        virtual void MapdGetVolumeL(TInt& aVolume);
        
        /**
        * Set a volume ramp.
        *
        * This will cause the sound device to start playing with zero volume,
        * increasing the volume over aRampDuration microseconds.
        *
        * The volume ramp can be removed by setting the ramp duration to zero.
        *
        * @param "aRampDuration" "The duration over which the volume is to be 
        *                        increased, in microseconds."
        * @leave "This function can leave with one of the system-wide error codes. 
        *        The request will be completed with the leave code."
        */
        virtual void MapdSetVolumeRampL( const TTimeIntervalMicroSeconds& aRampDuration );
        
        /**
        Set the balance between the left and right stereo audio channels.
        
          @param	"aBalance"	"Use a value between <code>KMMFBalanceMaxLeft<\code> and 
          <code>KMMFBalanceMaxRight<\code>. Centre balance can be restored by using
          <code>KMMFBalanceCenter<\code>."
          @leave	"This function can leave with one of the system-wide error codes. The request will be
          completed with the leave code."
          @since 7.0s
        */
        virtual void MapdSetBalanceL(TInt aBalance);
        
        /**
        Retrieve the balance between the left and right stereo audio channels.
        
          @param	"aBalance"	"The current balance, filled in by the controller."
          @leave	"This function can leave with one of the system-wide error codes. The request will be
          completed with the leave code."
          @since 7.0s
        */
        virtual void MapdGetBalanceL(TInt& aBalance);

    public: // Functions from MTtsCustomCommandImplementor

        /**
        * Adds a new style.
        *
        * @param "TTtsStyle& aStyle" Style to add.
        * @return Style identifier.
        */
        TTtsStyleID MttscAddStyleL( const TTtsStyle& aStyle );

        /**
        * Deletes a style based on style ID.
        *
        * @param "TTtsStyleID aID" Style ID to be deleted.
        * @return Error code.
        */
        TInt MttscDeleteStyle( TTtsStyleID aID );

        /**
        * Gets the synthesis position.
        *
        * @retval "TInt& aWordIndex" Returned position of synthesis.
        */
        void MttscGetPositionL( TInt& aWordIndex );

        /**
        * Returns the number of registered styles.
        *
        * @return Number of styles.
        */
        TUint16 MttscNumberOfStyles();

        /**
        * Opens parsed text as source.
        *
        * @param "CTtsParsedText& aText" Reference to parsed text.
        */
        void MttscOpenParsedTextL( CTtsParsedText& aText );

        /**
        * Sets the synthesis position based on word index.
        *
        * @param "TInt aWordIndex" Word index.
        */
        void MttscSetPositionL( TInt aWordIndex );

        /**
        * Returns the style based on ID.
        *
        * @param "TTtsStyleID aStyleID" Style ID.
        * @return Reference to style object.
        */
        TTtsStyle& MttscStyleL( TTtsStyleID aStyleID );

        /**
        * Returns the style based on index.
        *
        * @param "TUint16 aIndex" Style index.
        * @return Reference to style object.
        */
        TTtsStyle& MttscStyleL( TUint16 aIndex );
        
        /**
        * Sets the default style parameters for synthesis.
        *
        * @param "const TTtsStyle& aStyle" Style created by the client
        */        
        void MttscSetDefaultStyleL( const TTtsStyle& aStyle );
        
        /**
        * Returns the registered default style
        *
        * @return Style reference
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */
        TTtsStyle& MttscDefaultStyleL();
        
        /**
        * Sets the speaking rate of synthesizer.
        *
        * @param TInt aRate Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        */        
        void MttscSetSpeakingRateL( TInt aRate );
        
        /**
        * Returns the current speaking rate value.
        *
        * @return Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */
        TInt MttscSpeakingRateL();
      
        /**
        * Returns the list of supported languages.
        * 
        * @param "RArray<TLanguage>& aLanguages" Output parameter which contains 
        *        the languages
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */
        void MttscGetSupportedLanguagesL( RArray<TLanguage>& aLanguages );
        
        /**
        * Returns the list of supported voices for a certain language.
        * 
        * @param "TLanguage aLanguage" Language
        * @param "RArray<TTtsStyle>& aVoices" Output parameter which contains the voices
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        */        
        void MttscGetSupportedVoicesL( TLanguage aLanguage, RArray<TTtsStyle>& aVoices );

    public: // Functions from MDevTTSObserver

        /**
        * Invoked by DevTTS when it needs a configuration data package.
        * TODO: Add start and endpoint parameters.
        *
        * @param "TUint32 aPackageType"
        *         Packege type identifier
        * @param "TUint32 aPackageID"
        *         Package identifier
        *
        * @return Pointer to created data buffer. NULL if failure. User must 
        *         take care of the buffer deletion after usage.
        */
        HBufC8* MdtoConfigurationData( TUint32 aPackageType, 
                                       TUint32 aPackageID, 
                                       TUint32 aStartPosition, 
                                       TUint32 aEndPosition );
        
        /**
        * Called by DevTTS when an event occured. EDevTTSEventComplete is the only
        * event that can happen currently.
        *
        * @param "TDevTTSEvent aEvent"
        *         Event code
        * @param "TInt aStatus"
        *         Error code. KErrNone if success, one of the system level
        *         error codes otherwise.
        */
        void MdtoEvent( TDevTTSEvent aEvent, TInt aStatus );
        
        /**
        * Called by DevTTS device when output mode is EDevTTSClientMode and a 
        * new audio buffer has been filled by the synthesizer. 
        *
        * @param "TDesC8& aBuffer"
        *         Synthesized audio buffer.
        */
        void MdtoProcessBuffer( const TDesC8& aBuffer );
        
    protected:
        /**
        * CActive::RunL
        */
        void RunL();

        /**
        * CActive::DoCancel()
        */
        void DoCancel();

        /**
        * Handles play asynchrnously
        */
        void HandlePlay();

        /**
        * Activates this active object.
        */ 
        void Ready();

    private:
        
        /**
        * C++ default constructor.
        */
        CTtsControllerPluginBody( CTtsControllerPlugin* aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // Prohibit copy constructor.
        CTtsControllerPluginBody( const CTtsControllerPluginBody& );
        // Prohibit assigment operator.
        CTtsControllerPluginBody& operator=( const CTtsControllerPluginBody& );

        /**
        * Copies descriptor to parsed text with default style.
        */
        void ConstructParsedTextL( TDes8& aBufferPtr );

        /**
        * Copies parsed text structure to iParsedTextPointer.
        * Deletes previous if exists
        */
        void CopyParsedTextL( CTtsParsedText& aText );
        
        /**
        * Does priming from AddDataSource
        */
        void DoEarlyPrimingL();

    private:    // Data

        // TTP data loader, owned
        CTTSDataLoader*         iTtsLoader;

        // Pointer to the DevTTS, owned
        CDevTTS*                iDevTts;

        // Pointer to 'opened' parsed text object, owned
        CTtsParsedText*         iParsedTextPointer;

        // Observer, not owned.
        CTtsControllerPlugin*   iObserver;

        // Requested asynchronous function.
        TInt                    iRequestFunction;

        // Default style which will be used if no OpenParsedText is used
        TTtsStyle               iDefaultStyle;

        // Style ID of default style
        TTtsStyleID             iDefaultStyleID;
        
        // Tells if priming has been done already in AddDataSource
        TBool                   iEarlyPrimingDone;
        
        // ETrue if MttscSetDefaultStyleL() called
        TBool                   iUserSpecifiedDefaultStyle;
        
        // Output is currently either file or loudspeaker
        TDevTTSOutputMode       iTtsOutputMode;
        
        // File server session
        RFs                     iFs;
        
        // Handle to the output file, not owned. 
        RFile*                  iOutputFile;
        
        // Drive number (according TDriveNumber) where file handle is pointing
        TInt                    iOutFileDriveNumber;
        
        };

#endif // TTSPLUGINBODY_H   

// End of File

