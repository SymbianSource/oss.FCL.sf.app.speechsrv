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
* Description:  Implementation of TtsCommon 
*
*/


// INCLUDE FILES
#include "srsfbldvariant.hrh"
#include "ttscommonbody.h"
#include "nssttscommon.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::CTtsParsedTextBody
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTtsParsedTextBody::CTtsParsedTextBody() :
    iText( NULL ),
    iPhonemeSequence( NULL ),
    iPhonemeNotation( NULL )
    {
    // nothing
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTtsParsedTextBody::ConstructL(
    const TDesC& aText, 
    const TDesC8& aPhonemeSequence, 
    const TDesC& aPhonemeNotation )
    {
    
    if ( aText != KNullDesC )
        {
        // copy text 
        iText = HBufC::NewL( aText.Length() );
        TPtr tempPtr( iText->Des() );
        tempPtr = aText;
        }
    
    if ( aPhonemeSequence != KNullDesC8 )
        {
        // copy phoneme sequence
        iPhonemeSequence = HBufC8::NewL( aPhonemeSequence.Length() );
        TPtr8 tempPtr8( iPhonemeSequence->Des() );
        tempPtr8 = aPhonemeSequence;
        }
    
    if ( aPhonemeNotation != KNullDesC )
        {
        // copy phoneme notation 
        iPhonemeNotation = HBufC::NewL( aPhonemeNotation.Length() );
        TPtr tempPtr( iPhonemeNotation->Des() );
        tempPtr = aPhonemeNotation;
        }
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTtsParsedTextBody* CTtsParsedTextBody::NewL(
    const TDesC& aText, 
    const TDesC8& aPhonemeSequence, 
    const TDesC& aPhonemeNotation )
    {
    CTtsParsedTextBody* self = new( ELeave ) CTtsParsedTextBody;
    
    CleanupStack::PushL( self );
    self->ConstructL( aText, 
                      aPhonemeSequence, 
                      aPhonemeNotation );
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CTtsParsedTextBody::~CTtsParsedTextBody()
    {
    delete iText;
    delete iPhonemeSequence;
    delete iPhonemeNotation;
    iSegmentArray.Reset();
    }


// -----------------------------------------------------------------------------
// CTtsParsedTextBody::AddSegmentL
// Add new segment to array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTtsParsedTextBody::AddSegmentL( 
    const TTtsSegment& aSegment,
    TInt aIndex )
    {
    if ( aIndex < 0  )
        {
        User::Leave( KErrArgument ); // The index is negative
        }
    else if ( aIndex >= iSegmentArray.Count() )
        {
        // added to end
        User::LeaveIfError( iSegmentArray.Append( aSegment ) );
        }
    else
        {
        // added into middle
        User::LeaveIfError( iSegmentArray.Insert( aSegment, aIndex ) );
        }
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::DeleteSegment
// Delete segment
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTtsParsedTextBody::DeleteSegmentL( 
    TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iSegmentArray.Count() )
        {
        User::Leave( KErrArgument ); // The index is out of bounds.
        }

    iSegmentArray.Remove( aIndex );
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::IsValid
// Checks that segments do not overlap or pointer outside.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTtsParsedTextBody::IsValid() const
    {
    TInt index( 0 );
    
    if ( iPhonemeSequence == NULL &&
        iText == NULL &&
        iSegmentArray.Count() > 0 )
        {
        // no phoneme sequence and no text but segments still exist
        return EFalse;
        }
    
    // check phoneme sequence part 
    if ( iPhonemeSequence )
        {
        const TUint8* beginPhSeqPtr = iPhonemeSequence->Ptr();
        const TUint8* endPhSeqPtr = iPhonemeSequence->Ptr() 
            + iPhonemeSequence->Length() - 1;   
        
        for ( index = 0; index < iSegmentArray.Count(); index++ )
            {
            // check that pointers do not point outside
            const TTtsSegment& segment = iSegmentArray[index];
            const TDesC8& phSeq = segment.PhonemeSequencePtr();
            if ( phSeq != KNullDesC8 )
                {
                const TUint8* phSeqPtr = phSeq.Ptr();
                if ( phSeqPtr < beginPhSeqPtr || 
                    phSeqPtr > endPhSeqPtr ||
                    phSeqPtr + phSeq.Length() - 1 > endPhSeqPtr )
                    {
                    return EFalse;
                    }
                
                
                // check order and overlapping
                if ( index > 0 )
                    {
                    const TDesC8& prevPhSeq
                        = iSegmentArray[index - 1].PhonemeSequencePtr();
                    if ( prevPhSeq == KNullDesC8 )
                        {
                        // all pointer must point to either null or valid data
                        return EFalse;
                        }
                    
                    const TUint8* prevEndPhSeqPtr 
                        = prevPhSeq.Ptr() + prevPhSeq.Length() - 1;    
                    if ( prevEndPhSeqPtr >= phSeqPtr )
                        {
                        return EFalse;
                        }
                    }
                }
            }
        }
    
    // check phoneme sequence part 
    if ( iText )
        {
        const TUint16* beginTextPtr = iText->Ptr();
        const TUint16* endTextPtr = iText->Ptr() + iText->Length() - 1;
        
        for ( index = 0; index < iSegmentArray.Count(); index++ )
            {
            // check that pointers do not point outside
            const TTtsSegment& segment = iSegmentArray[index];
            const TDesC& text = segment.TextPtr();
            if ( text != KNullDesC )
                {
                const TUint16* textPtr = text.Ptr();
                if ( textPtr < beginTextPtr || 
                    textPtr > endTextPtr ||
                    textPtr + text.Length() - 1 > endTextPtr )
                    {
                    return EFalse;
                    }
                
                // check order and overlapping
                if ( index > 0 )
                    {
                    const TDesC& prevText = iSegmentArray[index - 1].TextPtr();
                    if ( prevText == KNullDesC )
                        {
                        // all pointer must point to either null or valid data
                        return EFalse;
                        }
                    const TUint16* prevEndTextPtr 
                        = prevText.Ptr() + prevText.Length() - 1;    
                    if ( prevEndTextPtr >= textPtr )
                        {
                        return EFalse;
                        }
                    }
                }
            }
        }
    
    // all checks passed
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::NumberOfSegments
// Return number of segments
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTtsParsedTextBody::NumberOfSegments() const
    {
    return iSegmentArray.Count();
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::PhonemeNotation
// Returns phoneme notation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CTtsParsedTextBody::PhonemeNotation() const
    {
    if ( iPhonemeNotation )
        {
        // Phoneme sequence has been set
        return *iPhonemeNotation;
        }
    return KNullDesC;   
    }
	
// -----------------------------------------------------------------------------
// CTtsParsedTextBody::PhonemeSequence
// Returns phoneme sequence
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CTtsParsedTextBody::PhonemeSequence() const
    {
    if ( iPhonemeSequence )
        {
        // Phoneme sequence has been set
        return *iPhonemeSequence;
        }
    return KNullDesC8;
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::SegmentL
// Returns segment of given index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TTtsSegment& CTtsParsedTextBody::SegmentL( 
    TInt aIndex ) const
    {
    if ( aIndex < 0 || aIndex >= iSegmentArray.Count() )
        {
        User::Leave( KErrArgument );
        }
    return iSegmentArray[aIndex];
    }
	
// -----------------------------------------------------------------------------
// CTtsParsedTextBody::SetPhonemeNotationL
// Sets phoneme notation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTtsParsedTextBody::SetPhonemeNotationL( 
    const TDesC& aPhonemeNotation )
    {
    if ( !iPhonemeNotation )
        {
        // copy phoneme notation 
        iPhonemeNotation = HBufC::NewL( aPhonemeNotation.Length() );
        TPtr tempPtr( iPhonemeNotation->Des() );
        tempPtr = aPhonemeNotation;
        }   
    else
        {
        TPtr bufferPtr( iPhonemeNotation->Des() );
        if ( aPhonemeNotation.Length() > bufferPtr.MaxLength() )
            {
            // new buffer does not fit into old one
            iPhonemeNotation->ReAllocL( aPhonemeNotation.Length() );   
            bufferPtr.Set( iPhonemeNotation->Des() );
            }
        bufferPtr = aPhonemeNotation;
        }
    }

// -----------------------------------------------------------------------------
// CTtsParsedTextBody::SetPhonemeSequenceL
// Sets phoneme sequence
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTtsParsedTextBody::SetPhonemeSequenceL( 
    const TDesC8& aPhonemeSequence )
    {
    
    if ( !iPhonemeSequence )
        {
        // create new phoneme sequence
        iPhonemeSequence = HBufC8::NewL( aPhonemeSequence.Length() );
        TPtr8 tempPtr8( iPhonemeSequence->Des() );
        tempPtr8 = aPhonemeSequence;
        }
    else
        {
        TPtr8 bufferPtr( iPhonemeSequence->Des() );
        if ( aPhonemeSequence.Length() > bufferPtr.MaxLength() )
            {
            // new buffer does not fit into old one
            iPhonemeSequence = iPhonemeSequence->ReAllocL( aPhonemeSequence.Length() );
            bufferPtr.Set( iPhonemeSequence->Des() );
            }
        bufferPtr = aPhonemeSequence;
        }
     
    }
	
// -----------------------------------------------------------------------------
// CTtsParsedTextBody::SetTextL
// Sets text
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTtsParsedTextBody::SetTextL( 
    const TDesC& aText )
    {
    if ( !iText )
        {
        // create new text 
        iText = HBufC::NewL( aText.Length() );
        TPtr tempPtr( iText->Des() );
        tempPtr = aText;
        }
    else 
        {
        TPtr bufferPtr( iText->Des() );
        if ( aText.Length() > bufferPtr.MaxLength() )
            {
            // new buffer does not fit into old one
            iText = iText->ReAllocL( aText.Length() ); 
            bufferPtr.Set( iText->Des() );
            }
        bufferPtr = aText;
        }
    }


// -----------------------------------------------------------------------------
// CTtsParsedTextBody::Text
// Returns text
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CTtsParsedTextBody::Text() const
    {
    if ( iText )
        {
        // text has been set
        return *iText;
        }
    return KNullDesC;
    }

//  End of File 
