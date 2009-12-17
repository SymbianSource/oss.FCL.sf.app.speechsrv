/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DevTTS interface for Text-to-Speech
*
*/


#ifndef DEVTTS_H
#define DEVTTS_H

//  INCLUDES
#include <e32base.h>
#include <e32def.h>
// TTSCommon include
#include <nssttscommon.h>

// CONSTANTS
const TInt KDevTTSBalanceRight = 100;
const TInt KDevTTSBalanceRightMiddle = 50;
const TInt KDevTTSBalanceMiddle = 0;
const TInt KDevTTSBalanceLeftMiddle = -50;
const TInt KDevTTSBalanceLeft = -100;

// DATA TYPES
enum TDevTTSEvent
    {
    EDevTTSEventComplete
    };

enum TDevTTSOutputMode
    {
    EDevTTSSoundDeviceMode = 0,
    EDevTTSClientMode
    };

enum TDevTTSAudioPreference
    {
    EDevTTSPriorityPreferenceNone = 0,
    EDevTTSPriorityPreferenceTime, 
    EDevTTSPriorityPreferenceQuality,
    EDevTTSPriorityPreferenceTimeAndQuality 
    };

// FORWARD DECLARATIONS
class CTTSAlgorithmManager;

// CLASS DECLARATIONS

/**
*  MDevTTSObserver
*
*  Interface for a set of DevTTS callback functions.
*
*  This mixin class is intended to be inherited by the DevTTS client. Functions
*  of this class get called when DevTTS specific events occur.
*
*  @lib nssdevtts.lib
*  @since 2.8
*/
class MDevTTSObserver
    {
    
    public:  // New functions
        
        /**
        * Invoked by DevTTS when it needs a configuration data package.
        *
        * @since 2.8
        * @param "TUint32 aPackageType" Packege type identifier
        * @param "TUint32 aPackageID" Package identifier
        * @param "TUint32 aStartPosition" Start position in bytes from beginning of data
        * @param "TUint32 aEndPosition" End index in bytes. If the position is bigger 
        *         than the size of data package, the end part will be returned.
        *
        * @return Pointer to created data buffer. NULL if failure. User must 
        *         take care of the buffer deletion after usage.
        */
        virtual HBufC8* MdtoConfigurationData( TUint32 aPackageType, 
                                               TUint32 aPackageID, 
                                               TUint32 aStartPosition, 
                                               TUint32 aEndPosition ) = 0;
        
        /**
        * Called by DevTTS when an event occured. EDevTTSEventComplete is the only
        * event that can happen currently.
        *
        * @since 2.8
        * @param "TDevTTSEvent aEvent"
        *         Event code
        * @param "TInt aStatus"
        *         Error code. KErrNone if success, one of the system level
        *         error codes otherwise.
        */
        virtual void MdtoEvent( TDevTTSEvent aEvent, TInt aStatus ) = 0;
        
        /**
        * Called by DevTTS device when output mode is EDevTTSClientMode and a 
        * new audio buffer has been filled by the synthesizer. 
        *
        * @since 2.8
        * @param "TDesC8& aBuffer"
        *         Synthesized audio buffer.
        */
        virtual void MdtoProcessBuffer( const TDesC8& aBuffer ) = 0;
    };

/**
*  CDevTTS is the interface towards DevTTS module. Actual implementation is
*  not included in this class.
*
*  @lib nssdevtts.lib
*  @since 2.8
*/
class CDevTTS : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CDevTTS* NewL( MDevTTSObserver& aObserver/*, CConsoleBase* console*/ );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CDevTTS();
        
    public: // New functions
        
        /**
        * Adds a new style to the collection. If equal style exists already,
        * new style is not added.
        * Will leave if an error occurs. Likely errors are: KErrNoMemory,
        * not enough memory to add new style. KErrNotSupported, the style 
        * is not supported. KErrArgument, the style contains an imposible 
        * value.
        *
        * @since 2.8
        * @param "TTtsStyle& aStyle"
        *        Reference to the style which will be added to the style 
        *        collection.
        *
        * @return Identifier of added style.
        */
        IMPORT_C TTtsStyleID AddStyleL( const TTtsStyle& aStyle );
        
        /**
        * Returns the current balance setting.
        *
        * @since 2.8
        * @return Balance value.
        */
        IMPORT_C TInt Balance();

        /**
        * Notifies that the buffer has been processed. The 
        * method is used if output mode of synthesizing is 
        * EDevTTSClientMode.
        *
        * @since 2.8
        * @param "TDesC8& aBuffer"
        *        The buffer which has been processed.
        */
        IMPORT_C void BufferProcessed( const TDesC8& aBuffer );
        
        /**
        * Request a pointer to the custom interface. Currently not in use.
        * 
        * @since 2.8
        * @param "TUid aInterfaceID"
        *        Custom interface identifier.
        *
        * @return Pointer to custom interface. NULL if interface is not 
        *         found or initiation failures.
        */
        IMPORT_C TAny* CustomInterface( TUid aInterfaceID );
        
        /**
        * Deletes style.
        * 
        * @since 2.8
        * @param "TTtsStyleID aStyleID" Identifier of style to be deleted.
        */
        IMPORT_C void DeleteStyleL( TTtsStyleID aStyleID );

        /**
        * Gives current position of synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing.
        * 
        * @since 2.8
        * @param "TTimeIntervalMicroSeconds& aTime"
        *        Time of the synthesizing position
        */
        IMPORT_C void GetPositionL( TTimeIntervalMicroSeconds& aTime ) const;
        
        /**
        * Gives current position of synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing.
        * 
        * @since 2.8
        * @param "TTtsSegment& aSegment"
        *        Current segment which is synthesized, out parameter
        * @param "TInt& aWordIndex"
        *        Index of the current word, starting from the begining of 
        *        current segment
        */
        IMPORT_C void GetPositionL( TTtsSegment& aSegment, TInt& aWordIndex ) const;
        
        /**
        * Tells if language is supported by the TTS engine or not.
        * 
        * @since 2.8
        * @param "TLanguage aLanguage"
        *        Language code
        *
        * @return ETrue if supported, EFalse otherwise
        */
        IMPORT_C TBool IsLanguageSupported( TLanguage aLanguage ) const;
        
        /**
        * Returns maximum volume value.
        * 
        * @since 2.8
        * @return Maximum volume.
        */
        IMPORT_C TInt MaxVolume();

        /**
        * Gives number of styles.
        * 
        * @since 2.8
        * @return Number of styles.
        */
        IMPORT_C TUint16 NumberOfStyles() const;
        
        /**
        * Pauses synthesis process.
        * @since 2.8
        */
        IMPORT_C void Pause();
        
        /**
        * Prepares for synthesis. The method will leave if an 
        * error occurs. Likely errors are: KErrInUse, synthesizing is 
        * currently ongoing. KErrNoMemory, not enough memory to start 
        * synthesis. KErrNotSupport, requested function is not supported.
        * 
        * @since 2.8
        * @param "MTtsSegmentStream& aStream"
        *        Stream to be synthesized.
        */
        IMPORT_C void PrimeSynthesisL( MTtsSegmentStream& aStream );
        
        /**
        * Prepares for synthesis. The method will leave if an 
        * error occurs. Likely errors are: KErrInUse, synthesizing is 
        * currently ongoing. KErrNoMemory, not enough memory to start 
        * synthesis. KErrNotSupport, requested function is not supported.
        * 
        * @since 2.8
        * @param "CTtsParsedText& aText"
        *        Text containing multiple segments to be synthesized.
        */
        IMPORT_C void PrimeSynthesisL( CTtsParsedText& aText );
        
        /**
        * Sets the priority for audio playback if in EDevTTSSoundDeviceMode.
        * 
        * @since 2.8
        * @param "TInt aPriority"
        *        Priority value.
        * @param "TDevTTSAudioPreference aPref"'
        *        Preference setting. Quality, time or both.
        */
        IMPORT_C void SetAudioPriority( TInt aPriority, TDevTTSAudioPreference aPref );
        
        /**
        * Sets the audio output routing
        * 
        * @since 3.2
        * @param "TInt aAudioOutput"
        *        New output.
        */
        IMPORT_C void SetAudioOutputL( TInt aAudioOutput );
        
        /**
        * Sets the playback device balance. Affects only when DevTTS is in 
        * EDevTTSSoundDeviceMode. Illegal values are neglected.
        * 
        * @since 2.8
        * @param "TInt aBalance" Balance to be set. 
        */
        IMPORT_C void SetBalance( TInt aBalance );
       
        /**
        * Sets new position for synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing. KErrArgument, position cannot be set.
        * 
        * @since 2.8
        * @param "TTimeIntervalMicroSeconds& aTime"
        *        Time of the synthesizing postion.
        */
        IMPORT_C void SetPositionL( const TTimeIntervalMicroSeconds& aTime );
        
        /**
        * Sets new position for synthesizing. The method will leave if an 
        * error occurs. Likely errors are: KErrNotReady, no synthesizing is 
        * ongoing. KErrArgument, position cannot be set.
        * 
        * @since 2.8
        * @param "TTtsSegment& aSegment"
        *        The segment being synthesized.
        * @param "TInt aWordIndex"
        *        The index of word being synthesized.
        */
        IMPORT_C void SetPositionL( const TTtsSegment& aSegment, TInt aWordIndex );
        
        /**
        * Sets the playback device volume. Affects only when DevTTS is in 
        * EDevTTSSoundDeviceMode. Range is from 0..MaxVolume(). Illegal values 
        * are neglected.
        * 
        * @since 2.8
        * @param "TInt aVolume" Volume setting.
        */
        IMPORT_C void SetVolume( TInt aVolume );

        /**
        * Sets the volume ramp duration. Start and end volumes must be set with
        * SetVolume() function.
        * 
        * @since 2.8
        * @param "TTimeIntervalMicroSeconds& aRampDuration" 
        *        Ramp duration in microseconds.
        */
        IMPORT_C void SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration );

        /**
        * Stops synthesizing and releases resources of synthesizing. 
        * The method does nothing if already stopped.
        * @since 2.8
        */
        IMPORT_C void Stop();
        
        /**
        * Receives style information. The method will leave if an error 
        * occurs. Likely errors are: KErrNotFound, style identifier not 
        * found. 
        * 
        * @since 2.8
        * @param "TTtsStyle& aStyle"
        *        Reference to style object where to assign the requested style.
        * @param "TTtsStyleID aStyleID"
        *        Style identifier.
        */
        IMPORT_C TTtsStyle& StyleL( TTtsStyleID aStyleID ) const;
        
        /**
        * Receives style information. The method will leave if an error 
        * occurs. Likely errors are: KErrArgument, index is out of bounds.
        * 
        * @since 2.8
        * @param "TTtsStyle& aStyle"
        *        Reference to style object where to assign the requested style.
        * @param "TUint16 aIndex"
        *        Index to style: 0..NumberOfStyles().
        */
        IMPORT_C TTtsStyle& StyleL( TUint16 aIndex ) const;
        
        /**
        * Starts synthesizing the text. The method is asynchronous, as soon
        * as the request has been placed, function call will return. 
        * MDevTTSObserver::MdtoEvent will be called when synthesizing finishes.
        * The method will leave if an error occurs. Likely errors are: 
        * KErrNoMemory, not enough memory. 
        * KErrInUse, synthesizing is currently ongoing
        * 
        * @since 2.8
        * @param "TDevTTSOutputMode aOutputMode"
        *        Either use a sound device (EDevTTSSoundDeviceMode) or client 
        *        using callback (EDevTTSClientMode)
        */
        IMPORT_C void SynthesizeL( TDevTTSOutputMode aOutputMode );
        
        /**
        * Returns current playback volume.
        * 
        * @since 2.8
        * @return Volume setting.
        */       
        IMPORT_C TInt Volume();

        /** 
        * Calls language identification for a given text.
        *
        * @since 3.0M
        * @param "CTtsParsedText& aText" Text
        * @param "TInt aNumberOfGuesses" Maximum number of guesses that will be provided
        * @param "RArray<TLanguage>& aLanguages" Output parameter which will contain the languages
        * @param "RArray<TInt>& aScores" Scores
        */
        IMPORT_C void LanguageIdentificationL( CTtsParsedText& aText, 
                                               TInt aNumberOfGuesses, 
                                               RArray<TLanguage>& aLanguages,
                                               RArray<TInt>& aScores );

        /**
        * Calls Natural Language Processing (NLP) to normalize text.
        *
        * @since 3.0M
        * @param "CTtsParsedText& aText" Text which will be normalized
        */
        IMPORT_C void NormalizeTextL( CTtsParsedText& aText );

        /**
        * Calls Natural Language Processing (NLP) to normalize and segment text.
        *
        * @since 3.0M
        * @param "CTtsParsedText& aText" Text which will be normalized
        */
        IMPORT_C void NormalizeAndSegmentTextL( CTtsParsedText& aText );

    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:
        
        /**
        * C++ default constructor.
        */
        CDevTTS();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MDevTTSObserver& aObserver/*, CConsoleBase* console*/ );
        
        /**
        * Prohibit copy constructor
        */
        CDevTTS( const CDevTTS& );
        
        /** 
        * Prohibit assignment operator
        */
        CDevTTS& operator=( const CDevTTS& );
        
    public:     // Data
        
    protected:  // Data
        
    private:    // Data
        
        // Pointer to the implementation class
        CTTSAlgorithmManager* iImpl;
        
        // Reserved pointer for future extension
        TAny* iReserved;
        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        
    };
    
#endif // DEVTTS_H   
    
// End of File
