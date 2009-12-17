/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Internal interface for TtsCommon
*
*/



#ifndef TTSCOMMONBODY_H
#define TTSCOMMONBODY_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "srsfbldvariant.hrh"
#include "ttscommonbody.h"
#include "nssttscommon.h"

// CLASS DECLARATION

/**
*  Class to store parsed text for TTS.
*
*  @lib ttscommon.lib
*  @since 2.8
*/
class CTtsParsedTextBody : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
		* @param aText Text to be parsed.
		* @param aPhonemeSequence Corresponding phoneme sequence. 
		*        If the sequence is not given, it will be determined automatically.
		* @param aPhonemeNotation Identifier of phoneme notation. 
		*		 If the descriptor is empty, the TTS engine will use default notation.
        */
		static CTtsParsedTextBody* NewL( const TDesC& aText, 
									     const TDesC8& aPhonemeSequence, 
									     const TDesC& aPhonemeNotation);
        
        /**
        * Destructor.
        */
        virtual ~CTtsParsedTextBody();

    public: // New functions

        /**
        * Adds new segment and takes its ownership.
        * @since 2.8
        * @param aSegment A new segment.aIndex Index where to add segment. 
		*        If index is equal or larger than NumberOfSegments(), the segment will be added into end.
        * @return None
        */
		void AddSegmentL( const TTtsSegment& aSegment, TInt aIndex );

        /**
        * Deletes a segment
        * @since 2.8
        * @param aIndex The index of segment to be deleted. The index must be 0..NumberOfSegments()-1.
        * @return None
        */
		void DeleteSegmentL( TInt aIndex );

        /**
        * Checks that the internal structure is valid.
        * @since 2.8
        * @param None
        * @return ETrue if valid and EFalse if invalid.
        */
        TBool IsValid() const;

        /**
        * Returns the number of segments.
        * @since 2.8
        * @param None
        * @return The number of segments
        */
		TInt NumberOfSegments() const;

        /**
        * Returns phoneme notation definition.
        * @since 2.8
        * @param None
        * @return Notation descriptor. KNullDesC8 if none.
        */
		const TDesC& PhonemeNotation() const;
	
        /**
        * Returns phoneme sequence.
        * @since 2.8
        * @param None
        * @return Phoneme sequence. KNullDesC8 if none.
        */
		const TDesC8& PhonemeSequence() const;

        /**
        * Returns segment from given index
        * @since 2.8
        * @param aIndex The index of segment to be deleted. The index must be 0..NumberOfSegments()-1.
        * @return Requested segment.
        */
		const TTtsSegment& SegmentL( TInt aIndex ) const;
	
        /**
        * Sets a phoneme notation. The descriptor is copied and can be released after the function call.
        * @since 2.8
        * @param aPhonemeNotation Phoneme notation.
        * @return None
        */
		void SetPhonemeNotationL( const TDesC& aPhonemeNotation );

        /**
        * Sets a phoneme sequence. The descriptor is copied and can be released after the function call.
        * @since 2.8
        * @param aPhonemeSequence Phoneme sequence.
        * @return None
        */
		void SetPhonemeSequenceL( const TDesC8& aPhonemeSequence );
	
        /**
        * Sets a text. The descriptor is copied and can be released after the function call.
        * @since 2.8
        * @param aText Text.
        * @return None
        */
		void SetTextL( const TDesC& aText );

        /**
        * Returns text.
        * @since 2.8
        * @param None
        * @return Text. KNullDesC8 if none.
        */
		const TDesC& Text() const;

    private:

        /**
        * C++ default constructor.
        */
        CTtsParsedTextBody();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aText, 
					     const TDesC8& aPhonemeSequence, 
						 const TDesC& aPhonemeNotation );


    private:    // Data
        // Buffer containing text
        HBufC* iText;

        // Buffer containing phoneme sequence
        HBufC8* iPhonemeSequence;

        // Buffer containing phoneme notation
        HBufC* iPhonemeNotation;

        // Array containing segments
        RArray<TTtsSegment> iSegmentArray;

    };

#endif      // TTSCOMMONBODY_H   
            
// End of File
