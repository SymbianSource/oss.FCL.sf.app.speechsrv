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


// INCLUDE FILES
#include "vuicttsplayer.h"

#include "rubydebug.h"

// CONSTANTS
const TInt KMaxBasicSegments = 3;
const TInt KMaxEnhancedSegments = 3;
    
// -----------------------------------------------------------------------------
// CTTSPlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTTSPlayer* CTTSPlayer::NewL( MTtsClientUtilityObserver* aObserver )
    {
    CTTSPlayer* self = new (ELeave) CTTSPlayer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }   
   
// Destructor       
CTTSPlayer::~CTTSPlayer()
    {
    RUBY_DEBUG0( "CTTSPlayer::~CTTSPlayer START" );
    
    Stop();
    
    RUBY_DEBUG0( "CTTSPlayer::~CTTSPlayer EXIT" );
    }

// ---------------------------------------------------------
// CTTSPlayer::PlayL
// ---------------------------------------------------------
//      
void CTTSPlayer::PlayL( const TDesC& aText, TInt aPosition,
                        TInt aMaxLength, const TDesC& aSegment )
    {
    RUBY_DEBUG_BLOCK( "CTTSPlayer::PlayL" );
    
    Stop();
    
    CreateSegmentsL( aText, aPosition, aMaxLength, aSegment );
    
    iTtsData->iParsedText->SetTextL( aText );
        
    // Opens and plays the parsed text
    iTtsData->iTts->OpenAndPlayParsedTextL( *iTtsData->iParsedText );    
    }

// ---------------------------------------------------------
// CTTSPlayer::PlayL
// ---------------------------------------------------------
//      
void CTTSPlayer::PlayL( const TDesC& aText, TInt aPosition,
                        TInt aMaxLength, const TDesC& aSegment,
                        const TDesC& aNewSegment1, const TDesC& aNewSegment2,
                        const TDesC& aNewSegment3  )
    {
    RUBY_DEBUG_BLOCK( "CTTSPlayer::PlayL" );
    
    Stop();
    
    HBufC* text = HBufC::NewLC( aText.Length() + aNewSegment1.Length() +
                                aNewSegment2.Length() + aNewSegment3.Length() );
    
    TPtr textPtr = text->Des();
    textPtr.Append( aText );
                                  
    CreateSegmentsL( *text, aPosition, aMaxLength, aSegment );
    
    for ( int i = KMaxBasicSegments; i < KMaxBasicSegments + KMaxEnhancedSegments; ++i )
        {
        iTtsData->iStyle[i] = new (ELeave) TTtsStyle();
        iTtsData->iStyleId[i] = new (ELeave) TTtsStyleID( 0 );
        iTtsData->iSegment[i] = new (ELeave) TTtsSegment( 0 );
        
        iTtsData->iStyle[i]->iLanguage = User::Language();
        
        *(iTtsData->iStyleId[i]) = iTtsData->iTts->AddStyleL( *(iTtsData->iStyle[i]) );
    
        iTtsData->iSegment[i]->SetStyleID( *(iTtsData->iStyleId[i]) );
        }
    
    TInt location = KErrNone;
    
    textPtr.Append( aNewSegment1 );
        
    iTtsData->iSegment[3]->SetTextPtr( text->Mid( location, aNewSegment1.Length() ) );

    iTtsData->iParsedText->AddSegmentL( *(iTtsData->iSegment[3]) );
        
    if ( aNewSegment2 != KNullDesC )
        {
        location = text->Length();
        textPtr.Append( aNewSegment2 );
        
        iTtsData->iSegment[4]->SetTextPtr( text->Mid( location, aNewSegment2.Length() ) );

        iTtsData->iParsedText->AddSegmentL( *(iTtsData->iSegment[4]) );
        }
        
    if ( aNewSegment3 != KNullDesC )
        {
        location = text->Length();
        textPtr.Append( aNewSegment3 );
        
        iTtsData->iSegment[5]->SetTextPtr( text->Mid( location, aNewSegment3.Length() ) );

        iTtsData->iParsedText->AddSegmentL( *(iTtsData->iSegment[5]) );
        }
        
    iTtsData->iParsedText->SetTextL( *text );
           
    // Opens and plays the parsed text
    iTtsData->iTts->OpenAndPlayParsedTextL( *iTtsData->iParsedText );
    
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------
// CTTSPlayer::Stop
// ---------------------------------------------------------
//          
void CTTSPlayer::Stop()
    {
    RUBY_DEBUG0( "CTTSPlayer::Stop START" );
        
    if ( iTtsData )
        {
        if ( iTtsData->iTts )
            {
            iTtsData->iTts->Stop();
            iTtsData->iTts->Close();
            }

        delete iTtsData->iParsedText;
        iTtsData->iParsedText = NULL;

        delete iTtsData->iTts;
        iTtsData->iTts = NULL;
        
        for ( int i = 0; i < KMaxSegments; ++i )
            {
            delete iTtsData->iStyle[i];
            delete iTtsData->iStyleId[i];
            delete iTtsData->iSegment[i];
            }

        delete iTtsData;
        iTtsData = NULL;
        }
    
    RUBY_DEBUG0( "CTTSPlayer::Stop EXIT" );  
    }

// ---------------------------------------------------------
// CTTSPlayer::CTTSPlayer
// ---------------------------------------------------------
//              
CTTSPlayer::CTTSPlayer( MTtsClientUtilityObserver* aObserver )
 : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------
// CTTSPlayer::ConstructL
// ---------------------------------------------------------
//           
void CTTSPlayer::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CTTSPlayer::ConstructL" );
    }

// ---------------------------------------------------------
// CTTSPlayer::CreateSegmentsL
// ---------------------------------------------------------
//      
void CTTSPlayer::CreateSegmentsL( const TDesC& aText, TInt aPosition,
                                  TInt aMaxLength, const TDesC& aSegment )
    {
    RUBY_DEBUG_BLOCK( "CTTSPlayer::CreateSegmentsL" );
    
    TInt leftLength = aText.Length();
    TInt midLength = KErrNone;
    TInt rightLength = KErrNone;
    
    TInt location = KErrNotFound;    
    if ( aSegment != KNullDesC )
        {
        location = aText.Right( aText.Length() - aPosition ).FindC( aSegment ) + aPosition;
        
        if ( location != aPosition )
            {
            location = aPosition + aMaxLength - aSegment.Length();
            }
        
        // Calculate segment lengths
        if ( location == KErrNone )
            {
            leftLength = aSegment.Length();
            rightLength = aText.Length() - aSegment.Length();
            }
        else if ( location + aSegment.Length() < aText.Length() )
            {
            leftLength = location;
            midLength = aSegment.Length();
            rightLength = aText.Length() - location - aSegment.Length();
            }
        else if ( location != KErrNotFound )
            {
            leftLength = aText.Length() - aSegment.Length();
            rightLength = aSegment.Length();
            }
        }
    
    iTtsData = new (ELeave) TTtsData;
    
    iTtsData->iTts = CTtsUtility::NewL( *iObserver );
    
    iTtsData->iParsedText = CTtsParsedText::NewL();

    for ( int i = 0; i < KMaxBasicSegments; ++i )
        {
        iTtsData->iStyle[i] = new (ELeave) TTtsStyle();
        iTtsData->iStyleId[i] = new (ELeave) TTtsStyleID( 0 );
        iTtsData->iSegment[i] = new (ELeave) TTtsSegment( 0 );
        
        iTtsData->iStyle[i]->iLanguage = User::Language();
        
        *(iTtsData->iStyleId[i]) = iTtsData->iTts->AddStyleL( *(iTtsData->iStyle[i]) );
    
        iTtsData->iSegment[i]->SetStyleID( *(iTtsData->iStyleId[i]) );
        }
    
    // Create segments
    iTtsData->iSegment[0]->SetTextPtr( aText.Left( leftLength ) );

    iTtsData->iParsedText->AddSegmentL( *(iTtsData->iSegment[0]) );

    if ( midLength != KErrNone )
        {
        iTtsData->iSegment[1]->SetTextPtr( aText.Mid( location, midLength ) );
        iTtsData->iSegment[2]->SetTextPtr( aText.Right( rightLength ) );

        iTtsData->iParsedText->AddSegmentL( *(iTtsData->iSegment[1]) );
        iTtsData->iParsedText->AddSegmentL( *(iTtsData->iSegment[2]) );
        }
    else if ( rightLength != KErrNone )
        {
        iTtsData->iSegment[1]->SetTextPtr( aText.Right( rightLength ) );

        iTtsData->iParsedText->AddSegmentL( *(iTtsData->iSegment[1]) );
        }
    }
    
// -----------------------------------------------------------------------------
// CTTSPlayer::TTtsData
// Initializes everything to zero.
// -----------------------------------------------------------------------------
//
CTTSPlayer::TTtsData::TTtsData() 
 : iTts( NULL ), iParsedText( NULL )
    {
    for ( int i = 0; i < KMaxSegments; ++i )
        {
        iStyle[i] = NULL;
        iStyleId[i] = NULL;
        iSegment[i] = NULL;
        }
    }
    
// End of File

