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
* Description:  
*
*/


#ifndef VUICTTSPLAYER_H
#define VUICTTSPLAYER_H

// INCLUDES
#include <e32base.h>

#include <nssttsutility.h>

// FORWARD DECLARATIONS
class MTtsClientUtilityObserver;

// CONSTANTS
const TInt KMaxSegments = 6;

// CLASS DECLARATION
/**
* 
*
*/
NONSHARABLE_CLASS( CTTSPlayer ) : public CBase
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CTTSPlayer* NewL( MTtsClientUtilityObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CTTSPlayer();
                                
    public:     // New functions
    
        /**
        * Starts the playback
        * @param aText Complete text to be played
        * @param aPosition First possible index of the segment
        * @param aMaxLength Maximum possible length of the segment
        * @param aSegment Text to be segmented from aText
        */
        void PlayL( const TDesC& aText, TInt aPosition = 0,
                    TInt aMaxLength = 0, const TDesC& aSegment = KNullDesC );
        
        /**
        * Starts the playback
        * @param aText Start of the text to be played
        * @param aPosition First possible index of the segment
        * @param aMaxLength Maximum possible length of the segment
        * @param aSegment Text to be segmented from aText
        * @param aNewSegment1 New segment to be added to end of text
        * @param aNewSegment2 New segment to be added to end of text
        * @param aNewSegment3 New segment to be added to end of text
        */
        void PlayL( const TDesC& aText, TInt aPosition,
                    TInt aMaxLength, const TDesC& aSegment,
                    const TDesC& aNewSegment1,
                    const TDesC& aNewSegment2 = KNullDesC,
                    const TDesC& aNewSegment3 = KNullDesC );
        
        /**
        * Stops the playback
        */
        void Stop();
             
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CTTSPlayer( MTtsClientUtilityObserver* aObserver );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
    private:    // Data
    
        /**
        * Creates segments from given text
        * @param aText Complete text to be played
        * @param aPosition First possible index of the segment
        * @param aMaxLength Maximum possible length of the segment
        * @param aSegment Text to be segmented from aText
        */
        void CreateSegmentsL( const TDesC& aText, TInt aPosition,
                              TInt aMaxLength, const TDesC& aSegment );
    
    struct TTtsData
        {
        // TTS speech styles
        TTtsStyle* iStyle[KMaxSegments];

        // TTS speech style handles
        TTtsStyleID* iStyleId[KMaxSegments];

        // TTS speech segments
        TTtsSegment* iSegment[KMaxSegments];

        // TTS player
        CTtsUtility* iTts;

        // Parsed text
        CTtsParsedText* iParsedText;

        // Initializes everything to zero
        TTtsData();
        };
        
    // Play observer
    MTtsClientUtilityObserver*      iObserver;
    
    // TTS Data
    TTtsData*                       iTtsData;
};

#endif  // VUICTTSPLAYER_H

// End of File
