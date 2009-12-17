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
* Description:     Data type defininitions for TTS
*     %version: 13 %
*
*  Copyright © 2004-2006 Nokia Corporation.
*/





#ifndef TTSCOMMON_H
#define TTSCOMMON_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS
// Undefined synthesizing language. The default language of TTS processor will be used.
const TLanguage KTtsUndefinedLanguage( ELangOther );

// Undefined duration of segment. The duration will be defined using other properties.
const TInt32 KTtsUndefinedDuration( -1 );

// Undefined rate
const TInt KTtsRateUndefined( -1 );

// Default volume, which is maximum volume (100%). 
const TInt KTtsDefaultVolume( 100 );

// Max length of voice name
const TInt KVoiceNameMaxLength( 50 );

// Maximum speaking rate
const TInt KTtsMaxSpeakingRate( 100 ); 

// 
_LIT( KDefaultVoiceNameMale,   "DefaultMale" );
_LIT( KDefaultVoiceNameFemale, "DefaultFemale" );

// DATA TYPES

// Style identifier
typedef TUint32 TTtsStyleID;

// Voice identifier
typedef TBuf<KVoiceNameMaxLength> TTtsVoice;
    
// Preferred quality of TTS output
enum TTtsQuality
    {
    ETtsQualityLowOnly, // Low quality
    ETtsQualityHighOnly, // High quality
    ETtsQualityHighPreferred, // High quality preferred, low quality fall back can be used
    ETtsQualityLowPreferred, // Low quality preferred
    ETtsQualityUndefined // Default quality will be used
    };

// Custom command ids for CTtsUtility::CustomCommand(As|S)ync()
enum TTtsCustomCommandId
    {
    ETtsCustomCommandSetAudioOutput      = 0, // For setting audio output routing
    ETtsCustomCommandSetPluginParameter  = 1, // For setting plugin parameter
    };

// FORWARD DECLARATIONS
class CTtsParsedTextBody;

// CLASS DECLARATION

/**
*  Class to encapsulate speaking style
*
*  @lib nssttscommon.lib
*  @since 2.8
*/
class TTtsStyle 
    {
    public: // Constructor and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TTtsStyle();
                
    public: // Data
    
        // Synthesizing language
        TLanguage iLanguage;

        // Voice identifier
        TTtsVoice iVoice;
             
        // Speaking rate in words-per-minute. See TTtsRate.
        TInt iRate;
        
        // Volume scale in percentages. The range is 0..100.
        // Zero means silent and 100 maximum volume. 
        TInt iVolume;
        
        // Sampling rate
        TInt iSamplingRate;
        
        // TTS quality (preferred or exclusive)
        TTtsQuality iQuality;
        
        // Desired time to take to read the segment in microseconds
        TTimeIntervalMicroSeconds32 iDuration;
        
        // Natural languagege processing on/off
        TBool iNlp;
        
    };

/**
*  Class to store segment information
*
*  @lib nssttscommon.lib
*  @since 2.8
*/
class TTtsSegment
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        * @since 2.8
        * @param aStyleID Indentifier of style.
        * @param aText Text for segment
        * @param aPhonemeSequence Corresponding phoneme sequence for text
        * @param aTrailingSilenceDuration The duration of trailing silence.
        */
        IMPORT_C TTtsSegment( const TTtsStyleID aStyleID, 
            const TDesC& aText = KNullDesC, 
            const TDesC8& aPhonemeSequence = KNullDesC8, 
            const TTimeIntervalMicroSeconds32& aTrailingSilenceDuration = KTtsUndefinedDuration );
                
    public: // New functions
        
        /**
        * Sets duration of trailing silence.
        * @since 2.8
        * @param aDuration Duration of silence.
        * @return None
        */
        IMPORT_C void SetTrailingSilenceL( const TTimeIntervalMicroSeconds32& aDuration );
        
        /**
        * Sets phoneme sequence pointer. The phoneme sequence cannot be deleted 
        * during the life time of this object.
        * @since 2.8
        * @param aPhonemeSequence Phoneme sequence.
        * @return None
        */
        IMPORT_C void SetPhonemeSequencePtr( const TDesC8& aPhonemeSequence );
        
        /**
        * Returns phoneme sequence pointer of the segment.
        * @since 2.8
        * @param None
        * @return Pointer to phoneme sequence
        */
        IMPORT_C const TDesC8& PhonemeSequencePtr() const;
        
        /**
        * Sets text pointer. The text cannot be deleted
        * during the life time of this object.
        * @since 2.8
        * @param aText Text.
        * @return None
        */
        IMPORT_C void SetTextPtr( const TDesC& aText );
        
        /**
        * Returns text pointer of the segment.
        * @since 2.8
        * @param None
        * @return Pointer to text.
        */
        IMPORT_C const TDesC& TextPtr() const;
        
        /**
        * Gets duration of trailing silence.
        * @since 2.8
        * @param None
        * @return Duration of silence.
        */
        IMPORT_C const TTimeIntervalMicroSeconds32& TrailingSilence() const;
        
        /**
        * Sets style for segment. The style cannot be deleted 
        * during this object life time.
        * @since 2.8
        * @param aStyleID The identifier of the style.
        * @return None
        */
        IMPORT_C void SetStyleID( TTtsStyleID aStyleID );
        
        /**
        * Returns identifier of used style.
        * @since 2.8
        * @param None
        * @return Identifier of style.
        */
        IMPORT_C TTtsStyleID StyleID() const;

        /**
        * Sets style object for segment.
        *
        * @since 3.1
        * @param aStyle Style reference.
        */
        IMPORT_C void SetStyle( TTtsStyle& aStyle );
        
        /**
        * Returns style object.
        *
        * @since 3.1
        * @return Reference to style.
        */
        IMPORT_C TTtsStyle& StyleL();

        /**
        * Resets previously set style and segment will use the style ID.
        *
        * @since 3.1
        */
        IMPORT_C void ResetStyle();
      
    private:    // Data
    
        // Style identifier for segment
        TTtsStyleID iStyleID;
        
        // Pointer to text associated for segment
        TPtrC iText;
        
        // Pointer to phoneme sequence associated for segment
        TPtrC8 iPhonemeSequence;
        
        // Duration of trailing silence
        TTimeIntervalMicroSeconds32 iTrailingSilenceDuration;

        // Style assigned directly to segment
        TTtsStyle iStyle;

        // On if style is directly assigned to the segment
        TBool iIncludesStyle;
    };
    
/**
*  Class to define functions for TTS segment stream. 
*  Using the class, a client can implement similar classes to CParsedText.
*
*  @lib nssttscommon.lib
*  @since 2.8
*/
class MTtsSegmentStream
    {
    public: // New functions
        
        /**
        * Returns used phoneme notation of the stream.
        * @since 2.8
        * @return Descriptor of used phoneme notation.
        */
        virtual TDesC& MTtsPhonemeNotation() = 0;
        
        /**
        * Returns the next segment in the stream.
        * @since 2.8
        * @param aSegment The requested segment.
        * @return System wide error code. KErrNone if success. 
        *         KErrUnderflow if there is no available segment.
        */
        virtual TInt MTtsReceiveSegment( TTtsSegment& aSegment ) = 0;
        
        /**
        * Notifies that the segment has been processed and it can be released.
        * @since 2.8
        * @param aSegment The processed segment.
        */
        virtual void MTtsSegmentDone( TTtsSegment& aSegment ) = 0;
        
    };

/**
*  Class to store parsed text for TTS.
*
*  @lib nssttscommon.lib
*  @since 2.8
*/
class CTtsParsedText : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aText Text to be parsed.
        * @param aPhonemeSequence Corresponding phoneme sequence. 
        *        If the sequence is not given, it will be determined automatically.
        * @param aPhonemeNotation Identifier of phoneme notation. 
        *		 If the descriptor is empty, the TTS engine will use default notation.
        * @return Created instance of CTtsParsedText
        */
        IMPORT_C static CTtsParsedText* NewL( const TDesC& aText = KNullDesC, 
            const TDesC8& aPhonemeSequence = KNullDesC8, 
            const TDesC& aPhonemeNotation = KNullDesC );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CTtsParsedText();
        
    public: // New functions
        
        /**
        * Adds new segment and takes its ownership.
        * @since 2.8
        * @param aSegment A new segment.
        * @param aIndex Index where to add segment. If index is equal or larger 
        *        than NumberOfSegments(), the segment will be added into end.
        */
        IMPORT_C void AddSegmentL( const TTtsSegment& aSegment, TInt aIndex = KMaxTInt );
        
        /**
        * Deletes a segment
        * @since 2.8
        * @param aIndex The index of segment to be deleted. The index must be 0..NumberOfSegments()-1.
        */
        IMPORT_C void DeleteSegmentL( TInt aIndex );
        
        /**
        * Checks that the internal structure is valid. 
        * @since 2.8
        * @return ETrue if valid and EFalse if invalid.
        */
        IMPORT_C TBool IsValid() const;
        
        /**
        * Returns the number of segments.
        * @since 2.8
        * @return The number of segments
        */
        IMPORT_C TInt NumberOfSegments() const;
        
        /**
        * Returns phoneme notation definition.
        * @since 2.8
        * @return Notation descriptor. KNullDesC8 if none.
        */
        IMPORT_C const TDesC& PhonemeNotation() const;
        
        /**
        * Returns phoneme sequence.
        * @since 2.8
        * @return Phoneme sequence. KNullDesC8 if none.
        */
        IMPORT_C const TDesC8& PhonemeSequence() const;
        
        /**
        * Returns segment from given index
        * @since 2.8
        * @param aIndex The index of segment to be returned. The index must be 0..NumberOfSegments()-1.
        * @return Requested segment.
        */
        IMPORT_C const TTtsSegment& SegmentL( TInt aIndex ) const;
        
        /**
        * Sets a phoneme notation. The descriptor is copied and can be released after the function call.
        * @since 2.8
        * @param aPhonemeNotation Phoneme notation.
        */
        IMPORT_C void SetPhonemeNotationL( const TDesC& aPhonemeNotation );
        
        /**
        * Sets a phoneme sequence. The descriptor is copied and can be released after the function call.
        * @since 2.8
        * @param aPhonemeSequence Phoneme sequence.
        */
        IMPORT_C void SetPhonemeSequenceL( const TDesC8& aPhonemeSequence );
        
        /**
        * Sets a text. The descriptor is copied and can be released after the function call.
        * @since 2.8
        * @param aText Text.
        */
        IMPORT_C void SetTextL( const TDesC& aText );
        
        /**
        * Returns text.
        * @since 2.8
        * @return Text. KNullDesC8 if none.
        */
        IMPORT_C const TDesC& Text() const;
        
    private:
        
        /**
        * C++ default constructor.
        */
        CTtsParsedText();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aText, 
                         const TDesC8& aPhonemeSequence, 
                         const TDesC& aPhonemeNotation );
        
        
    private: // Data
    
        // Implementation for the class
        CTtsParsedTextBody* iBody;
        
        // Reserved pointer for future extension
        TAny* iReserved;
        
    };   
#endif // TTSCOMMON_H
    
// End of File
