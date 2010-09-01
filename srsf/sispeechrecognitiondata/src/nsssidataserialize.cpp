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
* Description:  Serialization functions for CSIGrammar and CSILexicon
*
*/


// INCLUDE FILES
#include "nsssidataserialize.h"
#include <s32mem.h> // RBuf[Read/Write]Stream

// LOCAL CONSTANTS AND MACROS
const TUint32 KNibble16Limit = 0xffff;
const TUint32 KNibble4Limit  = 0xf;

// LOCAL FUNCTION PROTOTYPES
void Externalize32bitArrayL( RWriteStream& aStream, RArray<TUint32>& aArray );
void Internalize32bitArrayL( RReadStream& aStream,  RArray<TUint32>& aArray );
TInt IndexOrderForPhonemes( const TLexiconPhoneme& p1, const TLexiconPhoneme& p2 );
TInt StringOrderForPhonemes( const TLexiconPhoneme& p1, const TLexiconPhoneme& p2 );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Externalize32bitArrayL
// Writes an RArray with 32-bit entries to the stream.
// Returns: None
// -----------------------------------------------------------------------------
//
void Externalize32bitArrayL(
    RWriteStream&    aStream,  // The stream where to store the array
    RArray<TUint32>& aArray )  // The array; the entries must be 32-bit.
    {
    TInt count = aArray.Count();

    aStream.WriteInt32L( count );

    if ( count > 0 )
        {
        TInt* startPtr = (TInt*)&aArray[0];
        aStream.WriteL( (TUint8*)startPtr, sizeof(TInt) * count );
        }
    }

// -----------------------------------------------------------------------------
// Internalize32bitArrayL
// Reads an array from the stream.
// Returns: None
// -----------------------------------------------------------------------------
//
void Internalize32bitArrayL(
    RReadStream& aStream,     // The stream to read from
    RArray<TUint32>& aArray ) // The array to be populated
    {
    TInt count = aStream.ReadInt32L();
    TInt err = KErrNone;

    for ( TInt k( 0 ); k < count; k++ )
        {
        TInt value = aStream.ReadInt32L();
        err |= aArray.Append( value );
        }

    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// IndexOrderForPhonemes
// 
// -----------------------------------------------------------------------------
//
TInt IndexOrderForPhonemes( const TLexiconPhoneme& p1, const TLexiconPhoneme& p2 )
    {
    return( p1.iIndex - p2.iIndex );
    }

// -----------------------------------------------------------------------------
// StringOrderForPhonemes
// 
// -----------------------------------------------------------------------------
//
TInt StringOrderForPhonemes( const TLexiconPhoneme& p1, const TLexiconPhoneme& p2 )
    {
    return( p1.iPhoneme.Compare( p2.iPhoneme ) );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Implementation of CSILexiconSerializer starts from here.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CSILexiconSerializer::CSILexiconSerializer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSILexiconSerializer::CSILexiconSerializer()
    {
    iPronunIDs = NULL;
    iModelBankIDs = NULL;
    iParamBuf  = NULL;
    iCount     = 0; 
    iLexiconID = 0; // !!!! Change to KInvalidLexiconID.
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::~CSILexiconSerializer
// Destructor.
// -----------------------------------------------------------------------------
//
CSILexiconSerializer::~CSILexiconSerializer()
    {
    iPronunciations.ResetAndDestroy();
    iIndexPronuns.ResetAndDestroy();

    iConversionTable.Close();

    delete iPronunIDs;
    delete iModelBankIDs;
    delete iParamBuf;
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::NextPhonemeL
// Stores the next phoneme to phonemeBuf and increases the cursor.
// Returns true if there are still more phonemes.
// -----------------------------------------------------------------------------
//
bool CSILexiconSerializer::NextPhonemeL(
    TDes8& phonemeBuf,
    const TDesC8& phonemeSeq,
    TInt&  aReadIterator )
    {
    TInt length = phonemeSeq.Length();

    // Out of phonemes?
    if ( length == aReadIterator )
        {
        return( false );
        }

    // Cut those phonemes away, which have been processed already.
    TPtrC8 unfinishedPhonemes = phonemeSeq.Right( length - aReadIterator );

    // Make SeparatorLoc point to the end of the next phoneme.
    TInt separatorLoc = unfinishedPhonemes.Locate( KPhonemeSeparator );

    // Phoneme length is from [start of unprocessed string] to [separator]...
    TInt phonemeLength = separatorLoc;

    // ...unless we're dealing with the last phoneme. Then
    // phonemeLength is from [start of unprocessed string] to [end of string].
    if ( separatorLoc == KErrNotFound )
        phonemeLength = length - aReadIterator;

    // Check that the phoneme is not too long.
    if ( phonemeLength > KMaxPhonemeLength )
        {
        User::Leave( KErrArgument );
        }

    if ( phonemeLength == 0 )
        {
        return( false );
        }

    // Copy
    phonemeBuf.Copy( unfinishedPhonemes.Ptr(), phonemeLength );

    // Update iterator
    aReadIterator += phonemeLength; // The phoneme
    if ( aReadIterator < length )   // The separator
        {
        aReadIterator++;
        }

    return( true );
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::SavePronunciationL
// Converts the ASCII phoneme strings to internal index strings.
// -----------------------------------------------------------------------------
//
void CSILexiconSerializer::SavePronunciationL(CSIPronunciation* aPronun)
    {
    TLinearOrder<TLexiconPhoneme> order( StringOrderForPhonemes );

    // This phoneme is used for searching
    TLexiconPhoneme searchPhoneme;
    TDes8& phonemeBuf = searchPhoneme.iPhoneme;

    const TDesC8& phonemeSeq = aPronun->PhonemeSequence();

    HBufC8* indexPronun = HBufC8::NewLC( phonemeSeq.Length() );

    TInt phonemeReadIter = 0;

    while( NextPhonemeL( phonemeBuf, phonemeSeq, phonemeReadIter ) )
        {
        // Search the phoneme from conversion table.
        TInt loc = iConversionTable.FindInOrder( searchPhoneme, order );

        // Not found? Add the phoneme as a new phoneme.
        if ( loc == KErrNotFound )
            {
            searchPhoneme.iIndex = iConversionTable.Count();
            User::LeaveIfError(
                iConversionTable.InsertInOrder( searchPhoneme, order )
                );

            // Can't fail: the phoneme has just been added.
            loc = iConversionTable.FindInOrder( searchPhoneme, order );

            // Make sure that phoneme count fits in TUint8
            if ( searchPhoneme.iIndex >= 250 )
                {
                User::Leave( KErrOverflow );
                }
            }

        // Phoneme to index
        // Case to TUint8 is safe - Checked 10 lines earlier.
        TUint8 index = (TUint8)iConversionTable[ loc ].iIndex;
        indexPronun->Des().Append( index );
        }

    // Add pronunciation to the list of "index-pronunciations"
    User::LeaveIfError( iIndexPronuns.Append( indexPronun ) );
    CleanupStack::Pop( indexPronun );
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::Index2PhonemeLC
// Restores an internal index string into ASCII phonemes.
// -----------------------------------------------------------------------------
//
HBufC8* CSILexiconSerializer::Index2PhonemeLC(const TDesC8& aIndexSeq)
    {
    TLinearOrder<TLexiconPhoneme> order( IndexOrderForPhonemes );
    iConversionTable.Sort( order );

    TInt length = 0;

    TInt k( 0 );
    for( k = 0; k < aIndexSeq.Length(); k++ )
        {
        TInt index = aIndexSeq[ k ];
        length += iConversionTable[ index ].iPhoneme.Length();
        length++; // Separator ('-')
        }

    HBufC8* buf = HBufC8::NewLC( length );
    TPtr8 ptr = buf->Des();

    TInt lastK = aIndexSeq.Length() -1; // Index of the last character

    for( k = 0; k < aIndexSeq.Length(); k++ )
        {
        TInt index = aIndexSeq[ k ];
        ptr.Append( iConversionTable[ index ].iPhoneme );

        // Add separator, except for the last phoneme
        if ( k != lastK )
            {
            ptr.Append( KPhonemeSeparator );
            }
        }

    return( buf );
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSILexiconSerializer* CSILexiconSerializer::NewLC( const CSILexicon& aLexicon )
    {
    CSILexiconSerializer* me = new (ELeave) CSILexiconSerializer;
    CleanupStack::PushL( me );

    me->ConstructL( aLexicon );

    return( me );
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSILexiconSerializer* CSILexiconSerializer::NewLC( RReadStream& aStream )
    {
    CSILexiconSerializer* me = new (ELeave) CSILexiconSerializer;
    CleanupStack::PushL( me );

    me->InternalizeL( aStream );

    return( me );
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSILexiconSerializer::ConstructL( const CSILexicon& aLexicon )
    {
    iLexiconID = aLexicon.LexiconID();
    iCount     = aLexicon.Count();

    iParamBuf = CBufFlat::NewL( 100 ); // Grow in 100 byte doses

    RArray<TSIModelBankID> modelBankIDs;
    RArray<TSIPronunciationID> pronunIDs;

    CleanupClosePushL( modelBankIDs );
    CleanupClosePushL( pronunIDs );

    RBufWriteStream paramStream;
    paramStream.Open( *iParamBuf );
    CleanupClosePushL( paramStream );

    // In serialization format, the 5 types of data in pronunciations
    // are put to 4 piles. Then, the common properties of the data is exploited.
    for ( TInt k = 0; k < aLexicon.Count(); k++ )
        {
        CSIPronunciation* pronun = &aLexicon.AtL( k );

        // 4 types of pronunciation data
        TInt err = KErrNone;
        
        // This is a hack to get the modelid aligned to 4-byte boundary,
        // for some reason this does not happen in winscw build if 
        // TSIModelBankID is taken from stack.
        TSIModelBankID* modelid = new (ELeave) TSIModelBankID;
        *modelid = pronun->ModelBankID();
        
        err |= modelBankIDs.Append( *modelid );

        // Delete temporary modelid 
        delete modelid;
        
        err |= pronunIDs.Append( pronun->PronunciationID() );

        User::LeaveIfError( err );

        SavePronunciationL( pronun );

        // Parameters
        pronun->CSIParameters::ExternalizeL( paramStream );
        }

    // Code the piled values
    // Casts are safe, as all variables are 32-bit (they are just called
    // with many different names - int, unsigned long, unsigned int)
    iPronunIDs = CNibble16Coder::NewL( pronunIDs );
    iModelBankIDs = CRLECoder::NewL( modelBankIDs );

    CleanupStack::PopAndDestroy( &paramStream );

    CleanupStack::PopAndDestroy( 2 ); // Close the RArrays
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::ExternalizeL
// Stores the object to the stream.
// -----------------------------------------------------------------------------
//
void CSILexiconSerializer::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( KBinaryLexiconID );

    aStream.WriteInt32L( iLexiconID );
    aStream.WriteInt32L( iCount );

    // Write model bank IDs.
    iModelBankIDs->ExternalizeL( aStream );
    // Write pronunciation IDs
    iPronunIDs->ExternalizeL( aStream );

    // Write pronunciations (converted from phonemes to indices)
    aStream.WriteInt32L( iIndexPronuns.Count() );

    TInt k( 0 );
    for ( k = 0; k < iIndexPronuns.Count(); k++ )
        {
        HBufC8* pronun = iIndexPronuns[ k ];

        // There is no fixed limit on the length of a phoneme sequence.
        // It's not even guaranteed that the length fits in 32 bits, damnit!
        if ( pronun->Length() > 64000 )
            {
            User::Leave( KErrNotSupported );
            }

        aStream.WriteUint16L( pronun->Length() );
        aStream.WriteL( pronun->Ptr(), pronun->Length() );
        }

    // Write parameters
    aStream.WriteInt32L( iParamBuf->Size() );
    aStream.WriteL( iParamBuf->Ptr(0) );

    // Write phoneme<->index conversion table
    aStream.WriteInt32L( iConversionTable.Count() );
    for ( k = 0; k < iConversionTable.Count(); k++ )
        {
        aStream.WriteL( (TUint8*)&iConversionTable[k],
                        sizeof( TLexiconPhoneme ) );
        }

    // For corruption checking
    aStream.WriteInt32L( KBinaryLexiconID );
    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::InternalizeL
// Restores the object from a stream
// -----------------------------------------------------------------------------
//
void CSILexiconSerializer::InternalizeL(RReadStream& aStream)
    {
    if ( aStream.ReadInt32L() != KBinaryLexiconID )
        User::Leave( KErrArgument );

    iLexiconID = (TSILexiconID)aStream.ReadUint32L();
    iCount     = aStream.ReadInt32L();

    // Read model bank IDs
    iModelBankIDs = CRLECoder::NewL( aStream );
    // Read pronunciation IDs
    iPronunIDs = CNibble16Coder::NewL( aStream );

    // Read pronunciations (converted from phonemes to indices)
    TInt indexCount = aStream.ReadInt32L();

    if ( indexCount != iCount )
        {
        User::Leave( KErrCorrupt );
        }

    TInt k( 0 );
    for ( k = 0; k < indexCount; k++ )
        {
        HBufC8* pronun = 0;

        TUint16 length = aStream.ReadUint16L();
        pronun         = HBufC8::NewLC( length );

        TPtr8 pronunPtr = pronun->Des();
        aStream.ReadL( pronunPtr, length );

        User::LeaveIfError( iIndexPronuns.Append( pronun ) );

        CleanupStack::Pop( pronun );
        }

    // Read parameters
    TInt size = aStream.ReadInt32L();
    iParamBuf = CBufFlat::NewL( size+4 );
    iParamBuf->ExpandL( 0, size );

    TPtr8 paramBufPtr = iParamBuf->Ptr(0);
    aStream.ReadL( paramBufPtr, size );

    // Read phoneme<->index conversion table
    TInt conversionCount = aStream.ReadInt32L();
    for ( k = 0; k < conversionCount; k++ )
        {
        TLexiconPhoneme lexiconPhoneme;

        aStream.ReadL( (TUint8*)&lexiconPhoneme,
                        sizeof( TLexiconPhoneme ) );

        User::LeaveIfError( iConversionTable.Append( lexiconPhoneme ) );
        }

    // For corruption checking
    if ( aStream.ReadInt32L() != KBinaryLexiconID  )
        {
        User::Leave( KErrCorrupt );
        }

    }

// -----------------------------------------------------------------------------
// CSILexiconSerializer::RestoreL
// Populates CSILexicon with the newly internalized data.
// -----------------------------------------------------------------------------
//
void CSILexiconSerializer::RestoreL( CSILexicon& aLexicon, TSILexiconID& aLexiconID )
    {
    if ( aLexicon.Count() > 0 )
        {
        User::Leave( KErrInUse );
        }

    TSIModelBankID modelBankID = 0;
    TSIPronunciationID pronunID = 0;

    RBufReadStream paramStream;
    paramStream.Open( *iParamBuf );
    CleanupClosePushL( paramStream );

    iModelBankIDs->DecodeReset();
    iPronunIDs->DecodeReset();

    TInt iterPronunPos = 0;

    for ( TInt pronunK = 0; pronunK < iCount; pronunK++ )
        {
        modelBankID = iModelBankIDs->NextL();
        pronunID   = iPronunIDs->NextL();

        CSIPronunciation* pronun
            = CSIPronunciation::NewLC( pronunID, modelBankID ); 

        HBufC8* indexSequence = iIndexPronuns[ iterPronunPos ];
        iterPronunPos++;

        HBufC8* phonemeSequence = Index2PhonemeLC( *indexSequence );
        pronun->SetPhonemeSequenceL( *phonemeSequence );
        CleanupStack::PopAndDestroy( phonemeSequence );

        pronun->CSIParameters::InternalizeL( paramStream );

        aLexicon.AddL( pronun );
        CleanupStack::Pop( pronun );
        }

    CleanupStack::PopAndDestroy( &paramStream );

    // LexiconID can't be written to CSILexicon with public API.
    aLexiconID = iLexiconID;
    }


// -----------------------------------------------------------------------------
// Implementation of CSIGrammarSerializer starts from here.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CSIGrammarSerializer::CSIGrammarSerializer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIGrammarSerializer::CSIGrammarSerializer()
    {
    iGrammarID = KInvalidGrammarID;
    iCount = 0;
    iRuleIDs                = NULL;
    iRuleVariantCounts      = NULL;
    iRuleVariantIDs         = NULL;
    iRuleVariantLexiconIDs  = NULL;
    iRuleVariantLanguages   = NULL;
    iPronunIDSeqLengths     = NULL;
    iPronunIDSequences      = NULL;
    iParamBuf               = NULL;
    }


// -----------------------------------------------------------------------------
// CSIGrammarSerializer::~CSIGrammarSerializer
// Destructor.
// -----------------------------------------------------------------------------
//
CSIGrammarSerializer::~CSIGrammarSerializer()
    {
    delete iRuleIDs;
    delete iRuleVariantCounts;
    delete iRuleVariantIDs;
    delete iRuleVariantLexiconIDs;
    delete iRuleVariantLanguages;
    delete iPronunIDSeqLengths;
    delete iPronunIDSequences;
    delete iParamBuf;
    }


// -----------------------------------------------------------------------------
// CSIGrammarSerializer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIGrammarSerializer* CSIGrammarSerializer::NewLC( const CSIGrammar& aGrammar )
    {
    CSIGrammarSerializer* me = new (ELeave) CSIGrammarSerializer();
    CleanupStack::PushL( me );

    me->ConstructL( aGrammar );

    return( me );
    }

// -----------------------------------------------------------------------------
// CSIGrammarSerializer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIGrammarSerializer::ConstructL( const CSIGrammar& aGrammar )
    {
    iParamBuf = CBufFlat::NewL( 100 ); // Grow in 100 byte doses
    RBufWriteStream paramStream;
    paramStream.Open( *iParamBuf );
    CleanupClosePushL( paramStream ); // Cleanp stack: param stream

    RArray<TSIRuleID> ruleIDs;
    RArray<TUint32> ruleVariantCounts;
    RArray<TUint32> ruleVariantIDs;
    RArray<TSILexiconID> ruleVariantLexiconIDs;
    RArray<TUint32> ruleVariantLanguages;
    RArray<TUint32> pronunIDSeqLengths;
    RArray<TSIPronunciationID> pronunIDSequences;

    CleanupClosePushL( ruleIDs );
    CleanupClosePushL( ruleVariantCounts );
    CleanupClosePushL( ruleVariantIDs );
    CleanupClosePushL( ruleVariantLexiconIDs );
    CleanupClosePushL( ruleVariantLanguages );
    CleanupClosePushL( pronunIDSeqLengths );
    CleanupClosePushL( pronunIDSequences  ); // Cleanup stack: param stream, 7 tmp arrays

    TInt err = KErrNone;

    // Pile the data according to type (8 types)
    for ( TInt ruleK( 0 ); ruleK < aGrammar.Count(); ruleK++ )
        {
        CSIRule& rule = aGrammar.AtL( ruleK );

        err |= ruleIDs.Append( rule.RuleID() );
        err |= ruleVariantCounts.Append( rule.Count() );

        for ( TInt variantK( 0 ); variantK < rule.Count(); variantK++ )
            {
            CSIRuleVariant& variant = rule.AtL( variantK );

            err |= ruleVariantIDs.Append( variant.RuleVariantID() );
            
            // This is a hack to get the lexid aligned to 4-byte boundary,
            // for some reason this does not happen in winscw build if 
            // TSILexiconID is taken from stack.
            TSILexiconID* lexid = new (ELeave) TSILexiconID;
            *lexid = variant.LexiconID();
           
            err |= ruleVariantLexiconIDs.Append( *lexid );
            
            delete lexid;
            
            err |= ruleVariantLanguages.Append( variant.Language() );

            TSIPronunciationIDSequence seq;
            CleanupClosePushL( seq ); // Cleanup stack: param stream, 7 tmp arrays, seq
            
            variant.GetPronunciationIDsL( seq );

            err |= pronunIDSeqLengths.Append( seq.Count() );

            for ( TInt k( 0 ); k < seq.Count(); k++ )
                {
                err |= pronunIDSequences.Append( seq[k] );
                }

            CleanupStack::PopAndDestroy( &seq );
            // Cleanup stack: param stream, 7 tmp arrays

            // Store parameters to a buffer.
            variant.ExternalizeL( paramStream );
            }

        User::LeaveIfError( err );
        }

    // Code the arrays
    iGrammarID             = aGrammar.GrammarID();
    iCount                 = aGrammar.Count();

    iRuleIDs               = CNibble16Coder::NewL( ruleIDs );
    iRuleVariantCounts     = CNibble4Coder::NewL( ruleVariantCounts );
    iRuleVariantIDs        = CNibble4Coder::NewL( ruleVariantIDs );
    iRuleVariantLexiconIDs = CRLECoder::NewL( ruleVariantLexiconIDs );
    // Cast is safe, since TLanguage is 32-bit
    iRuleVariantLanguages  = CNibble4Coder::NewL( ruleVariantLanguages );
    iPronunIDSeqLengths    = CNibble4Coder::NewL( pronunIDSeqLengths );
    iPronunIDSequences     = CNibble16Coder::NewL( pronunIDSequences );

    CleanupStack::PopAndDestroy( 7 ); // Destory 7 tmp arrays
    CleanupStack::PopAndDestroy( &paramStream ); // Destroy parameter stream
    }

// -----------------------------------------------------------------------------
// CSIGrammarSerializer::ExternalizeL
// Stores the object to the stream.
// -----------------------------------------------------------------------------
//
void CSIGrammarSerializer::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteUint32L( ( TUint32 ) KBinaryGrammarID );
    aStream.WriteUint32L( iGrammarID );
    aStream.WriteUint32L( iCount );

    iRuleIDs              ->ExternalizeL( aStream );
    iRuleVariantCounts    ->ExternalizeL( aStream );
    iRuleVariantIDs       ->ExternalizeL( aStream );
    iRuleVariantLexiconIDs->ExternalizeL( aStream );
    iRuleVariantLanguages ->ExternalizeL( aStream );
    iPronunIDSeqLengths   ->ExternalizeL( aStream );
    iPronunIDSequences    ->ExternalizeL( aStream );

    aStream.WriteInt32L( iParamBuf->Size() );
    aStream.WriteL( iParamBuf->Ptr(0) );

    aStream.WriteUint32L( ( TUint32 ) KBinaryGrammarID );
    }

// -----------------------------------------------------------------------------
// CSIGrammarSerializer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIGrammarSerializer* CSIGrammarSerializer::NewLC(RReadStream& aStream)
    {
    CSIGrammarSerializer* me = new (ELeave) CSIGrammarSerializer();
    CleanupStack::PushL( me );

    me->ConstructL( aStream );

    return( me );
    }

// -----------------------------------------------------------------------------
// CSIGrammarSerializer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIGrammarSerializer::ConstructL( RReadStream& aStream )
    {
    if ( aStream.ReadInt32L() != KBinaryGrammarID )
        {
        User::Leave( KErrCorrupt );
        }

    iGrammarID = (TSIGrammarID)aStream.ReadUint32L();
    iCount     = aStream.ReadUint32L();

    iRuleIDs              = CNibble16Coder::NewL( aStream );
    iRuleVariantCounts    = CNibble4Coder::NewL( aStream );
    iRuleVariantIDs       = CNibble4Coder::NewL( aStream );
    iRuleVariantLexiconIDs= CRLECoder::NewL( aStream );
    iRuleVariantLanguages = CNibble4Coder::NewL( aStream );
    iPronunIDSeqLengths   = CNibble4Coder::NewL( aStream );
    iPronunIDSequences    = CNibble16Coder::NewL( aStream );

    TInt32 paramSize = aStream.ReadInt32L();
    iParamBuf = CBufFlat::NewL( paramSize + 4 );
    iParamBuf->ExpandL( 0, paramSize );
    TPtr8 paramBufPtr = iParamBuf->Ptr(0);
    aStream.ReadL( paramBufPtr, paramSize );

    if ( aStream.ReadInt32L() != KBinaryGrammarID )
        {
        User::Leave( KErrCorrupt );
        }
    }

// -----------------------------------------------------------------------------
// CSIGrammarSerializer::RestoreL
// Populates the CSIGrammar with newly internalized data.
// -----------------------------------------------------------------------------
//
void CSIGrammarSerializer::RestoreL( CSIGrammar& aGrammar, TSIGrammarID& aGrammarID )
    {
    if ( aGrammar.Count() != 0 )
        {
        User::Leave( KErrInUse );
        }

    aGrammarID = iGrammarID;

    TBool ready = iRuleIDs && iRuleVariantCounts && iRuleVariantIDs &&
                  iRuleVariantLexiconIDs && iRuleVariantLanguages &&
                  iPronunIDSeqLengths && iPronunIDSequences;

    if ( !ready )
        {
        User::Leave( KErrNotReady );
        }

    iRuleIDs->DecodeReset();
    iRuleVariantCounts->DecodeReset();
    iRuleVariantIDs->DecodeReset();
    iRuleVariantLexiconIDs->DecodeReset();
    iRuleVariantLanguages->DecodeReset();
    iPronunIDSeqLengths->DecodeReset();
    iPronunIDSequences->DecodeReset();

    RBufReadStream paramStream;
    CleanupClosePushL( paramStream );

    paramStream.Open( *iParamBuf );

    for ( TInt ruleK( 0 ); ruleK < iCount; ruleK++ )
        {
        CSIRule* rule = CSIRule::NewLC( iRuleIDs->NextL() );

        TInt ruleVariantCount = iRuleVariantCounts->NextL();

        for ( TInt variantK( 0 ); variantK < ruleVariantCount; variantK++ )
            {
            CSIRuleVariant* variant = CSIRuleVariant::NewLC(
                (TSIRuleVariantID)iRuleVariantIDs->NextL(),
                iRuleVariantLexiconIDs->NextL()
                );

            variant->SetLanguage(
                (enum TLanguage)iRuleVariantLanguages->NextL() );

            RArray<TSIPronunciationID> pronunIDSeq;
            CleanupClosePushL( pronunIDSeq );

            TInt seqLength = iPronunIDSeqLengths->NextL();
            for ( TInt pronunIdK( 0 ); pronunIdK < seqLength; pronunIdK++ )
                {
                TInt err = pronunIDSeq.Append( iPronunIDSequences->NextL() );
                User::LeaveIfError( err );
                }

            variant->SetPronunciationIDsL( pronunIDSeq );
            CleanupStack::PopAndDestroy( &pronunIDSeq );

            variant->CSIParameters::InternalizeL( paramStream );
            rule->AddL( variant );
            CleanupStack::Pop( variant );
            }

        aGrammar.AddL( rule );
        CleanupStack::Pop( rule );
        }

    CleanupStack::PopAndDestroy( &paramStream );
    }


// -----------------------------------------------------------------------------
// Implementation of CNibble16Coder starts from here.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CNibble16Coder::CNibble16Coder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNibble16Coder::CNibble16Coder()
: iMainArray(50),
  iOverflowArray(10)
    {
    DecodeReset();
    }

// -----------------------------------------------------------------------------
// CNibble16Coder::NewL
// Two-phased constructor. Encodes the values given as the argument.
// -----------------------------------------------------------------------------
//
CNibble16Coder* CNibble16Coder::NewL( const RArray<TUint32>& aPlainArray )
    {
    CNibble16Coder* me = new (ELeave) CNibble16Coder;
    CleanupStack::PushL( me );

    TInt count = aPlainArray.Count();;
    TInt sizeNeeded = (count+1) / 2;

    // Grow the array to be big enough
    TInt k( 0 );
    for ( k = 0; k < sizeNeeded; k++ )
        {
        User::LeaveIfError( me->iMainArray.Append( 0xffffffff ) );
        }

    TUint16* storagePtr = 0;

    if ( count > 0 )
        {
        storagePtr = (TUint16*)&me->iMainArray[0];
        }

    for( k = 0; k < count; k++ )
        {
        TSIPronunciationID id = aPlainArray[ k ];
        TBool overflow = false;

        // If the 32-bit value doesn't fit to 16 bits:
        //  * Put overflow sign to main array
        //  * Put the real ID to overflow array
        if ( id >= KNibble16Limit )
            {
            overflow = true;
            id = KNibble16Limit;
            }

        // Write to the main array
        // ([overflow sign] OR [id, which fits to 16 bits])
        *(storagePtr++) = (TUint16)id;

        // If it didn't fit to 16 bits, save it to the overflow array.
        if ( overflow )
            {
            TInt err = me->iOverflowArray.Append( aPlainArray[k] );
            User::LeaveIfError( err );
            }
        }

    CleanupStack::Pop( me );
    return( me );
    }

// -----------------------------------------------------------------------------
// CNibble16Coder::NewL
// Two-phased constructor. Internalizes an array from stream.
// -----------------------------------------------------------------------------
//
CNibble16Coder* CNibble16Coder::NewL( RReadStream& aStream )
    {
    CNibble16Coder* me = new (ELeave) CNibble16Coder;
    CleanupStack::PushL( me );

    if ( aStream.ReadInt32L() != KBinaryNibble16ID )
        {
        User::Leave( KErrCorrupt );
        }

    Internalize32bitArrayL( aStream, me->iMainArray     );
    Internalize32bitArrayL( aStream, me->iOverflowArray );

    CleanupStack::Pop( me );
    return( me );
    }

// -----------------------------------------------------------------------------
// CNibble16Coder::~CNibble16Coder
// Destructor
// -----------------------------------------------------------------------------
//
CNibble16Coder::~CNibble16Coder()
    {
    iMainArray.Close();
    iOverflowArray.Close();
    }

// -----------------------------------------------------------------------------
// CNibble16Coder::DecodeReset
// Moves cursor to the beginning of the array.
// -----------------------------------------------------------------------------
//
void CNibble16Coder::DecodeReset()
    {
    iPosition = 0;
    iOverflowPos = 0;
    }

// -----------------------------------------------------------------------------
// CNibble16Coder::NextL
// Returns a value in the array and moves the cursor forward.
// -----------------------------------------------------------------------------
//
TUint32 CNibble16Coder::NextL()
    {
    if ( iPosition >= 2*iMainArray.Count() )
        User::Leave( KErrOverflow );

    TUint16* storagePtr = (TUint16*)&iMainArray[0];

    // Get the [pronunciation ID] OR [overflow sign] from main array.
    TSIPronunciationID id = storagePtr[ iPosition ];

    // It was overflow sign; get whole 32-bit ID from overflow array
    if ( id == KNibble16Limit ) // It was pronunciation ID
        {
        if ( iOverflowPos == iOverflowArray.Count() )
            {
            User::Leave( KErrOverflow );
            }

        id = iOverflowArray[ iOverflowPos ];
        iOverflowPos++;
        }

    // Update iterator
    iPosition++;

    return( id );
    }

// -----------------------------------------------------------------------------
// CNibble16Coder::ExternalizeL
// Externalizes the array into a stream.
// -----------------------------------------------------------------------------
//
void CNibble16Coder::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( KBinaryNibble16ID );
    Externalize32bitArrayL( aStream, iMainArray );
    Externalize32bitArrayL( aStream, iOverflowArray );
    }


// -----------------------------------------------------------------------------
// Implementation of CNibble4Coder starts from here.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CNibble4Coder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNibble4Coder* CNibble4Coder::NewL( const RArray<TUint32>& aPlainArray )
    {
    CNibble4Coder* me = new (ELeave) CNibble4Coder;
    CleanupStack::PushL( me );
    me->DecodeReset();

    TInt count = aPlainArray.Count();

    for ( TInt k( 0 ); k < count; k++ )
        {
        TUint compactValue = aPlainArray[ k ];
        TBool overflow = EFalse;

        // If the value does not fit to 4 bytes, put an overflow sign to main
        // array, and save the value to a separate overflow array.
        if ( compactValue >= KNibble4Limit )
            {
            compactValue = KNibble4Limit;
            overflow = true;
            }

        // Save the compact value
        if ( me->iSlot == 0 )
            {
            User::LeaveIfError( me->iMainArray.Append( compactValue ) );
            me->iSlot++;
            }
        else{
            // We save 4-bit values to 32-bit variable. So, there are 8 slots
            // in each 32-bit entry.

            // Shift the value to correct slot. 
            // Earlier, we have made sure, that compactValue < 16.
            compactValue = compactValue << (4 * me->iSlot);

            // Put it to the array.
            me->iMainArray[ me->iPosition ]
                = compactValue | me->iMainArray[ me->iPosition ];

            // Update iterator
            me->iSlot++;
            if ( me->iSlot == 8 )
                {
                me->iPosition++;
                me->iSlot = 0;
                }
            }

        // If the value didn't fit to 4 bits, save it to 32-bit overflow array.
        if ( overflow )
            {
            TInt ret = me->iOverflowArray.Append( aPlainArray[ k ] );
            User::LeaveIfError( ret );
            }
        }

    // If the number of entries was not divisible by 8, we have
    // uninitialized slots in the end. Initialize them with 0xf.

    // If the decoder finds one of these 0xf's, it goes to overflow array,
    // notices that all entries in the overflow array have been processed,
    // and leaves.
    if ( me->iSlot != 0 )
        {
        TUint unusedValue = 0xffffffff;
        unusedValue = unusedValue << (4 * me->iSlot);

        me->iMainArray[ me->iPosition ] 
            = unusedValue | me->iMainArray[ me->iPosition ];
        }

    me->DecodeReset();
    CleanupStack::Pop( me );
    return me;
    }

// -----------------------------------------------------------------------------
// CNibble4Coder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNibble4Coder* CNibble4Coder::NewL( RReadStream& aStream )
    {
    CNibble4Coder* me = new (ELeave) CNibble4Coder;
    CleanupStack::PushL( me );
    me->DecodeReset();

    if ( aStream.ReadInt32L() != KBinaryNibble4ID )
        {
        User::Leave( KErrCorrupt );
        }

    Internalize32bitArrayL( aStream, me->iMainArray     );
    Internalize32bitArrayL( aStream, me->iOverflowArray );

    CleanupStack::Pop( me );
    return( me );
    }

// -----------------------------------------------------------------------------
// CNibble4Coder::CNibble4Coder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNibble4Coder::CNibble4Coder()
: iMainArray(50),
  iOverflowArray(10)
    {
    DecodeReset();
    }

// -----------------------------------------------------------------------------
// CNibble4Coder::~CNibble4Coder
// Destructor.
// -----------------------------------------------------------------------------
//
CNibble4Coder::~CNibble4Coder()
    {
    iMainArray.Close();
    iOverflowArray.Close();
    }

// -----------------------------------------------------------------------------
// CNibble4Coder::DecodeReset
// Moves cursor to the beginning of the array.
// -----------------------------------------------------------------------------
//
void CNibble4Coder::DecodeReset()
    {
    iPosition = 0;
    iSlot     = 0;
    iOverflowPos = 0;
    }

// -----------------------------------------------------------------------------
// CNibble4Coder::NextL
// Returns a value in the array and moves cursor forwards.
// -----------------------------------------------------------------------------
//
TUint32 CNibble4Coder::NextL()
    {
    if ( iPosition >= iMainArray.Count() )
        {
        User::Leave( KErrOverflow );
        }

    TUint result = iMainArray[ iPosition ];

    // In binary, the array entry is something like:
    // hhhhhhhhhhhhhhhhrrrrllllllllllll (32 bits)
    // r = result bits we're after
    // h = higher bits
    // l = lower bits

    // Drop the lower bits
    result = result >> (4 * iSlot );
    // Drop the higher bits
    result = result & 0xf;

    // If the value didn't fit to 4 bits, get it from the overflow array.
    if ( result >= KNibble4Limit )
        {
        if ( iOverflowPos >= iOverflowArray.Count() )
            {
            User::Leave( KErrOverflow );
            }

        result = iOverflowArray[ iOverflowPos ];
        iOverflowPos++;
        }

    // Update iterator
    iSlot++;
    if ( iSlot == 8 )
        {
        iPosition++;
        iSlot = 0;
        }

    return( result );
    }

// -----------------------------------------------------------------------------
// CNibble4Coder::ExternalizeL
// Stores the object to the stream.
// -----------------------------------------------------------------------------
//
void CNibble4Coder::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( KBinaryNibble4ID );
    Externalize32bitArrayL( aStream, iMainArray );
    Externalize32bitArrayL( aStream, iOverflowArray );
    }


// -----------------------------------------------------------------------------
// Implementation of CRLECoder starts from here.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CRLECoder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRLECoder* CRLECoder::NewL( const RArray<TUint16>& aPlainArray )
    {
    CRLECoder* me = new (ELeave) CRLECoder;
    CleanupStack::PushL( me );
    me->DecodeReset();

    if ( aPlainArray.Count() == 0 )
        {
        CleanupStack::Pop();
        return( me );
        }

    TInt count = aPlainArray.Count();

    if ( count > 0 )
        {
        CRLECoder::TValueAndCount valueCount;

        valueCount.iValue = aPlainArray[ 0 ];
        valueCount.iCount = 1;

        for ( TInt k( 1 ); k < count; k++ )
            {
            if ( aPlainArray[ k ] == valueCount.iValue )
                {
                valueCount.iCount++;
                }
            else{
                User::LeaveIfError( me->iRleArray.Append( valueCount ) );

                valueCount.iCount = 1;
                valueCount.iValue = aPlainArray[ k ];
                }
            }

        User::LeaveIfError( me->iRleArray.Append( valueCount ) );
        }

    CleanupStack::Pop( me );
    return me;
    }

// -----------------------------------------------------------------------------
// CRLECoder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRLECoder* CRLECoder::NewL( RReadStream& aStream )
    {
    CRLECoder* me = new (ELeave) CRLECoder;
    CleanupStack::PushL( me );
    me->DecodeReset();

    if ( aStream.ReadInt32L() != KBinaryRLEID )
        {
        User::Leave( KErrCorrupt );
        }

    TInt count = aStream.ReadInt32L();
    TInt err = KErrNone;

    TUint32 value;
    CRLECoder::TValueAndCount* valuePtr = (CRLECoder::TValueAndCount*)&value;

    for ( TInt k( 0 ); k < count; k++ )
        {
        value = aStream.ReadInt32L();
        err |= me->iRleArray.Append( *valuePtr );
        }

    User::LeaveIfError( err );

    CleanupStack::Pop( me );
    return( me );
    }

// -----------------------------------------------------------------------------
// CRLECoder::CRLECoder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRLECoder::CRLECoder()
: iRleArray(50)
    {
    DecodeReset();
    }

// -----------------------------------------------------------------------------
// CRLECoder::~CRLECoder
// Destructor.
// -----------------------------------------------------------------------------
//
CRLECoder::~CRLECoder()
    {
    iRleArray.Close();
    }

// -----------------------------------------------------------------------------
// CRLECoder::DecodeReset
// Moves cursor to the beginning of the array.
// -----------------------------------------------------------------------------
//
void CRLECoder::DecodeReset()
    {
    iPosition = 0;
    iRepetition = 0;
    }

// -----------------------------------------------------------------------------
// CRLECoder::NextL
// Returns a value in the array and moves cursor forwards.
// -----------------------------------------------------------------------------
//
TUint16 CRLECoder::NextL()
    {
    if ( iPosition >= iRleArray.Count() ) 
        {
        User::Leave( KErrOverflow );
        }

    TUint16 result = iRleArray[iPosition].iValue;

    iRepetition++;
    TUint16 maxRepetitions = iRleArray[iPosition].iCount;

    if ( iRepetition >= maxRepetitions )
        {
        iPosition++;
        iRepetition = 0;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CRLECoder::ExternalizeL
// Stores the object to the stream.
// -----------------------------------------------------------------------------
//
void CRLECoder::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( KBinaryRLEID );
    TInt count = iRleArray.Count();

    aStream.WriteInt32L( count );

    if ( count > 0 )
        {
        TValueAndCount* startPtr = &(iRleArray[0]);
        aStream.WriteL( (TUint8*)startPtr, sizeof(TValueAndCount) * count );
        }

    }


// -----------------------------------------------------------------------------
// Implementation of CYesNoCoder starts from here.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// CYesNoCoder::DecodeReset
// Moves cursor to the beginning of the array.
// -----------------------------------------------------------------------------
//
void CYesNoCoder::DecodeReset()
    {
    iPosition = 0;
    iMask = 0x00000001;
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::NextBit
// Moves the cursor forwards.
// -----------------------------------------------------------------------------
//
void CYesNoCoder::NextBit(void)
    {
    // Increase the iterator.
    iMask <<= 1;
    if ( iMask == 0 )
        {
        iPosition++;
        iMask = 1;
        }
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CYesNoCoder* CYesNoCoder::NewL()
    {
    CYesNoCoder* me = new (ELeave) CYesNoCoder;

    me->DecodeReset();

    return( me );
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CYesNoCoder* CYesNoCoder::NewL( RReadStream& aStream )
    {
    CYesNoCoder* me = new (ELeave) CYesNoCoder;
    CleanupStack::PushL( me );
    me->DecodeReset();

    Internalize32bitArrayL( aStream, me->iStore );

    CleanupStack::Pop( me );
    return( me );
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::CYesNoCoder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CYesNoCoder::CYesNoCoder()
: iStore(10)
    {
    DecodeReset();
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::~CYesNoCoder
// Destructor.
// -----------------------------------------------------------------------------
//
CYesNoCoder::~CYesNoCoder()
    {
    iStore.Close();
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::EncodeL
// Encodes a bit.
// -----------------------------------------------------------------------------
//
void CYesNoCoder::EncodeL( TBool aValue )
    {
    // Do we need to grow the size of the array?
    if ( iMask == 1 )
        {
        User::LeaveIfError( iStore.Append( 0 ) );
        }

    // Store the value, if it is 'yes'.
    if ( aValue )
        {
        iStore[ iPosition ] |= iMask;
        }

    // Increase the iterator
    NextBit();
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::NextL
// Returns a value in the array and moves cursor forwards.
// -----------------------------------------------------------------------------
//
TBool CYesNoCoder::NextL()
    {
    TBool result = iStore[ iPosition ] & iMask;

    // Increase the iterator
    NextBit();

    return( result );
    }

// -----------------------------------------------------------------------------
// CYesNoCoder::ExternalizeL
// Stores the object to the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
void CYesNoCoder::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( KBinaryYesNoID );
    Externalize32bitArrayL( aStream, iStore );
    }

// End of File  
