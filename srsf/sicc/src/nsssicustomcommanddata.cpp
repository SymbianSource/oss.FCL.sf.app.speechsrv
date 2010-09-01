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
* Description:  Private serialization functions for Custom Commands
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include "nsssicustomcommanddata.h"
#include "rubydebug.h"

// LOCAL FUNCTION PROTOTYPES
void CleanupForPointerArrays( TAny* anArray );
void CleanupResetAndDestroyAndDeletePushL( RPointerArray<MDesCArray>* anArray );


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupForPointerArrays
//
// Cleanup function for RPointerArray, which calls ResetAndDestroy()
// before deleting.
//
// Returns: None
// -----------------------------------------------------------------------------
//
void CleanupForPointerArrays(
    TAny* anArray) // Pointer to RPointerArray<MDesCArray>
    {
    RPointerArray<MDesCArray> *array = (RPointerArray<MDesCArray> *)anArray;
    array->ResetAndDestroy();
    delete array;
    }

// -----------------------------------------------------------------------------
// CleanupResetAndDestroyAndDeletePushL
//
// Pushes the RPointerArray to the cleanup stack, so that
// both the container and its contents are deleted in case of a leave.
//
// Returns: None
// -----------------------------------------------------------------------------
//
void CleanupResetAndDestroyAndDeletePushL(
    RPointerArray<MDesCArray> *anArray) // Array to be destroyed and deleted
    {
    TCleanupItem cleanupItem( CleanupForPointerArrays, (TAny *)anArray );
    CleanupStack::PushL( cleanupItem );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSpeakerIndependentCustomCommandParser::ExternalizeDesCArrayL
// Externalizes a string array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
CBufFlat* SICustomCommandData::ExternalizeDesCArrayL(
	const MDesCArray& aArray)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);

    // Write the number of strings
	stream.WriteInt32L(aArray.MdcaCount());

    // Write strings: First string length, then the characters
    for( TInt k = 0; k < aArray.MdcaCount(); k++ )
        {
        TPtrC text( aArray.MdcaPoint( k ) );

        stream.WriteInt32L( text.Length() );

        if ( text.Length() > 0 )
            {
            stream.WriteL( text );
            }
        }

	CleanupStack::PopAndDestroy(1); //stream
	CleanupStack::Pop(); //dataCopyBuffer;
	return dataCopyBuffer;	
	}

// -----------------------------------------------------------------------------
// CSpeakerIndependentCustomCommandParser::InternalizeDesCArrayL
// Internalizes a string array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
MDesCArray *SICustomCommandData::InternalizeDesCArrayL(
	const TDesC8& aBuffer)
	{
	CDesCArrayFlat* phraseArray = new CDesCArrayFlat(KExpandSize);
	CleanupStack::PushL(phraseArray);

	RDesReadStream stream;
	stream.Open(aBuffer);
	CleanupClosePushL(stream);

	TInt count = stream.ReadInt32L();

    TInt   phraseBufLength = KDefaultPhraseLen;
    HBufC *phraseBuf = HBufC::NewLC( phraseBufLength );
    TPtr   phrase    = phraseBuf->Des();

    for( TInt k = 0; k < count; k++ )
        {
        TInt length = stream.ReadInt32L();

        if ( length > phraseBufLength )
            {
            CleanupStack::PopAndDestroy( phraseBuf );
            phraseBuf = HBufC::NewLC( length );
            phraseBufLength = length;
            phrase.Set( phraseBuf->Des() );
            }

        stream.ReadL( phrase, length );
        phraseArray->AppendL( phrase );
        phrase.Zero();
        }

	CleanupStack::PopAndDestroy(2); //phraseBuf, stream
	CleanupStack::Pop(phraseArray); //phraseArray;
	return phraseArray;	
	}

// -----------------------------------------------------------------------------
// CSpeakerIndependentCustomCommandParser::ExternalizeDesCArrayArrayL
// Externalizes an array of string arrays.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
CBufFlat* SICustomCommandData::ExternalizeDesCArrayArrayL(
	const RPointerArray<MDesCArray>& aArrayArray)
	{
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);

    stream.WriteInt32L(aArrayArray.Count());

    for( TInt arrayK = 0; arrayK < aArrayArray.Count(); arrayK++ )
        {
        const MDesCArray& aArray = *aArrayArray[ arrayK ];
        // Write the number of strings
	    stream.WriteInt32L(aArray.MdcaCount());

        // Write strings: First string length, then the characters
        for( TInt k = 0; k < aArray.MdcaCount(); k++ )
            {
            TPtrC text( aArray.MdcaPoint( k ) );

            stream.WriteInt32L( text.Length() );

            if ( text.Length() > 0 )
                {
                stream.WriteL( text );
                }
            }
        }

	CleanupStack::PopAndDestroy(1); //stream
	CleanupStack::Pop(); //dataCopyBuffer;
	return dataCopyBuffer;	
	
	}


// -----------------------------------------------------------------------------
// CSpeakerIndependentCustomCommandParser::InternalizeDesCArrayArrayL
// Internalizes an array of string arrays array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RPointerArray<MDesCArray> *SICustomCommandData::InternalizeDesCArrayArrayL(
	const TDesC8& aBuffer)
	{
    RPointerArray<MDesCArray>* phraseArrayArray = new (ELeave) RPointerArray<MDesCArray>(KExpandSize);
    CleanupResetAndDestroyAndDeletePushL(phraseArrayArray);

	RDesReadStream stream;
	stream.Open(aBuffer);
	CleanupClosePushL(stream);

	TInt arrayCount = stream.ReadInt32L();

    TInt   phraseBufLength = KDefaultPhraseLen;
    HBufC *phraseBuf = HBufC::NewL( phraseBufLength );
    TPtr   phrase    = phraseBuf->Des();
    CleanupStack::PushL( phraseBuf );

    for( TInt arrayK = 0; arrayK < arrayCount; arrayK++ )
        {
        
	    TInt count = stream.ReadInt32L();

    	CDesCArrayFlat* phraseArray = new CDesCArrayFlat(KExpandSize);
        CleanupStack::PushL( phraseArray );

        for( TInt k = 0; k < count; k++ )
            {
            TInt length = stream.ReadInt32L();

            if ( length > phraseBufLength )
                {
                HBufC* newPhraseBuf = HBufC::NewL( length );

                CleanupStack::Pop( phraseArray );
                CleanupStack::PopAndDestroy( phraseBuf );

                phraseBuf = newPhraseBuf;
                phraseBufLength = length;
                phrase.Set( phraseBuf->Des() );

                CleanupStack::PushL( phraseBuf );
                CleanupStack::PushL( phraseArray );
                }

            stream.ReadL( phrase, length );
            phraseArray->AppendL( phrase );
            phrase.Zero();
            }

        User::LeaveIfError( phraseArrayArray->Append( phraseArray ) );
        CleanupStack::Pop( phraseArray );
        }

	CleanupStack::PopAndDestroy(2); //phraseBuf, stream
	CleanupStack::Pop(1); //phraseArrayArray
	return phraseArrayArray;	
	}


// -----------------------------------------------------------------------------
// CSpeakerIndependentCustomCommandParser::InternalizeDesCArrayArrayL
// Internalizes an array of string arrays array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* SICustomCommandData::ReadMessageDataLC( TMMFMessage& aMessage, TInt aSlot )
    {
    // MMF messages have 2 slots
    if ( aSlot != 1 && aSlot != 2 )
    	{
        RUBY_DEBUG0( "SICustomCommandData::ReadMessageDataLC Leaving with KErrArgument" );
    	User::Leave( KErrArgument );
    	}
        

    TInt size = 0;

    if ( aSlot == 1 )
        {
        size = aMessage.SizeOfData1FromClient();
        }
    else
        {
        size = aMessage.SizeOfData2FromClient();
        }

    HBufC8* data = HBufC8::NewLC( size );
    TPtr8 ptr = data->Des();

    if ( aSlot == 1 )
        {
        aMessage.ReadData1FromClientL( ptr );
        }
    else{
        aMessage.ReadData2FromClientL( ptr );
        }

    return( data );
    }


// -----------------------------------------------------------------------------
// CSpeakerIndependentCustomCommandParser::InternalizePronunArrayL
// Internalizes an array of string arrays array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void SICustomCommandData::InternalizePronunArrayL(
    const TDesC8& aData,
    TInt aCount,
    RArray<TSIPronunciationID>& aArray )
    {
    const TSIPronunciationID* pronunPtr = (const TSIPronunciationID*)aData.Ptr();

    for ( TInt k = 0; k < aCount; k++ )
        {
        User::LeaveIfError( aArray.Append( *pronunPtr ) );
        pronunPtr++;
        }
    }


// End of File  
