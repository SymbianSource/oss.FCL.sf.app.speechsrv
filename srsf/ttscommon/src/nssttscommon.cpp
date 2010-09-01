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
* Description:  Common data types for TTS
*
*/



// INCLUDE FILES
#include "srsfbldvariant.hrh"
#include "ttscommonbody.h"
#include "nssttscommon.h"


// ============================ MEMBER FUNCTIONS ===============================
// ----------------------------   TTtsStyle      -------------------------------

// -----------------------------------------------------------------------------
// TTtsStyle::TTtsStyle
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyle::TTtsStyle() :
    iLanguage( KTtsUndefinedLanguage ),
	iVoice( KNullDesC ),
    iRate( KTtsRateUndefined ),
    iVolume( KTtsDefaultVolume ),
    iSamplingRate( KTtsRateUndefined ),
	iQuality( ETtsQualityUndefined ), 
    iDuration( KTtsUndefinedDuration ),
	iNlp( EFalse )
    {
    // nothing
    }


// ----------------------------   TTtsSegment    -------------------------------

// -----------------------------------------------------------------------------
// TTtsSegment::TTtsSegment
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsSegment::TTtsSegment( 
    const TTtsStyleID aStyleID, 
	const TDesC& aText, 
	const TDesC8& aPhonemeSequence, 
    const TTimeIntervalMicroSeconds32& aTrailingSilenceDuration ) :
    iStyleID( aStyleID ),
    iText( aText ),
    iPhonemeSequence( aPhonemeSequence ),
    iTrailingSilenceDuration( aTrailingSilenceDuration ),
    iIncludesStyle( EFalse )
    {
    // nothing
    }
        

// -----------------------------------------------------------------------------
// TTtsSegment::SetTrailingSilenceL
// Sets the duration of trailing silence
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TTtsSegment::SetTrailingSilenceL( 
    const TTimeIntervalMicroSeconds32& aDuration )
    {
    iTrailingSilenceDuration = aDuration;
    }


// -----------------------------------------------------------------------------
// TTtsSegment::SetPhonemeSequencePtr
// Sets phoneme sequence pointer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TTtsSegment::SetPhonemeSequencePtr( 
    const TDesC8& aPhonemeSequence )
    {
    iPhonemeSequence.Set( aPhonemeSequence );
    }

// -----------------------------------------------------------------------------
// TTtsSegment::PhonemeSequencePtr
// Returns phoneme sequence pointer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& TTtsSegment::PhonemeSequencePtr() const
    {
    return iPhonemeSequence;
    }

// -----------------------------------------------------------------------------
// TTtsSegment::SetTextPtr
// Sets text pointer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TTtsSegment::SetTextPtr( 
    const TDesC& aText )
    {
    iText.Set( aText );
    }
// -----------------------------------------------------------------------------
// TTtsSegment::TrailingSilence
// Gets duration of trailing silence
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTimeIntervalMicroSeconds32& TTtsSegment::TrailingSilence() const
    {
    return iTrailingSilenceDuration;
    }

// -----------------------------------------------------------------------------
// TTtsSegment::TextPtr
// Returns text pointer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& TTtsSegment::TextPtr() const
    {
    return iText;
    }

// -----------------------------------------------------------------------------
// TTtsSegment::SetStyleID
// Sets style identifier
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TTtsSegment::SetStyleID( 
    TTtsStyleID aStyleID )
    {
    iStyleID = aStyleID;
    }

// -----------------------------------------------------------------------------
// TTtsSegment::StyleID
// Returns style identifier
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyleID TTtsSegment::StyleID() const
    {
    return iStyleID;
    }

// -----------------------------------------------------------------------------
// TTtsSegment::SetStyle
// Sets the style object for this segment.
// -----------------------------------------------------------------------------
//
EXPORT_C void TTtsSegment::SetStyle( TTtsStyle& aStyle )
    {
    iStyle = aStyle;
    iIncludesStyle = ETrue;
    }
        
// -----------------------------------------------------------------------------
// TTtsSegment::StyleL
// Returns style object assigned to this segment.
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyle& TTtsSegment::StyleL()
    {
    if ( !iIncludesStyle )
        {
        User::Leave( KErrNotFound );
        }
    return iStyle;
    }

// -----------------------------------------------------------------------------
// TTtsSegment::ResetStyle
// Resets style object, style id will be used instead.
// -----------------------------------------------------------------------------
//
EXPORT_C void TTtsSegment::ResetStyle()
    {
    iIncludesStyle = EFalse;
    }

// ----------------------------  CTtsParsedText  -------------------------------

// -----------------------------------------------------------------------------
// CTtsParsedText::CTtsParsedText
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTtsParsedText::CTtsParsedText() :
    iBody( NULL ),
    iReserved( NULL )
    {
    // nothing
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTtsParsedText::ConstructL(
    const TDesC& aText, 
    const TDesC8& aPhonemeSequence, 
    const TDesC& aPhonemeNotation )
    {
    iBody = CTtsParsedTextBody::NewL( aText, 
                                      aPhonemeSequence, 
                                      aPhonemeNotation );
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CTtsParsedText* CTtsParsedText::NewL(
    const TDesC& aText, 
    const TDesC8& aPhonemeSequence, 
    const TDesC& aPhonemeNotation )
    {
    CTtsParsedText* self = new( ELeave ) CTtsParsedText;
    
    CleanupStack::PushL( self );
    self->ConstructL( aText, 
                      aPhonemeSequence, 
                      aPhonemeNotation );
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
EXPORT_C CTtsParsedText::~CTtsParsedText()
    {
    delete iBody;
    }


// -----------------------------------------------------------------------------
// CTtsParsedText::AddSegmentL
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsParsedText::AddSegmentL( 
    const TTtsSegment& aSegment,
    TInt aIndex )
    {
    iBody->AddSegmentL( aSegment, aIndex );
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::DeleteSegment
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsParsedText::DeleteSegmentL( 
    TInt aIndex )
    {
    iBody->DeleteSegmentL( aIndex );
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::IsValid
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsParsedText::IsValid() const
    {
    return iBody->IsValid();
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::NumberOfSegments
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTtsParsedText::NumberOfSegments() const
    {
    return iBody->NumberOfSegments();
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::PhonemeNotation
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CTtsParsedText::PhonemeNotation() const
    {
    return iBody->PhonemeNotation();
    }
	
// -----------------------------------------------------------------------------
// CTtsParsedText::PhonemeSequence
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CTtsParsedText::PhonemeSequence() const
    {
    return iBody->PhonemeSequence();
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::SegmentL
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTtsSegment& CTtsParsedText::SegmentL( 
    TInt aIndex ) const
    {
    return iBody->SegmentL( aIndex );
    }
	
// -----------------------------------------------------------------------------
// CTtsParsedText::SetPhonemeNotationL
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsParsedText::SetPhonemeNotationL( 
    const TDesC& aPhonemeNotation )
    {
    iBody->SetPhonemeNotationL( aPhonemeNotation );
    }

// -----------------------------------------------------------------------------
// CTtsParsedText::SetPhonemeSequenceL
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsParsedText::SetPhonemeSequenceL( 
    const TDesC8& aPhonemeSequence )
    {
    iBody->SetPhonemeSequenceL( aPhonemeSequence );
    }
	
// -----------------------------------------------------------------------------
// CTtsParsedText::SetTextL
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CTtsParsedText::SetTextL( 
    const TDesC& aText )
    {
    iBody->SetTextL( aText );
    }


// -----------------------------------------------------------------------------
// CTtsParsedText::Text
// Call corresponding function of CTtsParsedTextBody
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CTtsParsedText::Text() const
    {
    return iBody->Text();
    }

//  End of File  
