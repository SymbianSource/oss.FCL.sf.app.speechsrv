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
* Description:  This file contains definition of the SISpeechRecognitionDataClient structure and API.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "nsssispeechrecognitiondataclient.h"
#include "nsssispeechrecognitiondatadevasr.h"
#include "nsssispeechrecognitiondatatest.h"

// CONSTANTS
const TInt32 KBinaryClientResultSetID = 0xAACF1235;

// Does the CSIClientResultSet have an embedded CSIResultSet?
const TInt KNoResultSet  = 0;
const TInt KYesResultSet = 1;

// -----------------------------------------------------------------------------
// CSIClientResult::~CSIClientResult
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResult::~CSIClientResult()
{
}

// -----------------------------------------------------------------------------
// CSIClientResult::CSIClientResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIClientResult::CSIClientResult()
	:	iGrammarID(0),
		iRuleID(0)
{
}

// -----------------------------------------------------------------------------
// CSIClientResult::CSIClientResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIClientResult::CSIClientResult(TSIGrammarID aGrammarID, TSIRuleID aRuleID)
	:	iGrammarID(aGrammarID),
		iRuleID(aRuleID)
{
}

// -----------------------------------------------------------------------------
// CSIClientResult::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIClientResult::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSIClientResult::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResult* CSIClientResult::NewLC()
{
	CSIClientResult* self = new (ELeave) CSIClientResult();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSIClientResult::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResult* CSIClientResult::NewL()
{
	CSIClientResult* self = NewLC();
	CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CSIClientResult::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResult* CSIClientResult::NewLC(TSIGrammarID aGrammarID, TSIRuleID aRuleID)
{
	CSIClientResult* self = new (ELeave) CSIClientResult(aGrammarID, aRuleID);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSIClientResult::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResult* CSIClientResult::NewL(TSIGrammarID aGrammarID, TSIRuleID aRuleID)
{
	CSIClientResult* self = NewLC(aGrammarID, aRuleID);
	CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CSIClientResult::SetGrammarID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResult::SetGrammarID(TSIGrammarID aGrammarID)
{
	iGrammarID = aGrammarID;
}

// -----------------------------------------------------------------------------
// CSIClientResult::GrammarID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIGrammarID CSIClientResult::GrammarID() const
{
	return iGrammarID;
}

// -----------------------------------------------------------------------------
// CSIClientResult::SetRuleID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResult::SetRuleID(TSIRuleID aRuleID)
{
	iRuleID = aRuleID;
}

// -----------------------------------------------------------------------------
// CSIClientResult::RuleID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleID CSIClientResult::RuleID() const
{
	return iRuleID;
}
 
/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIClientResultSet::~CSIClientResultSet
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResultSet::~CSIClientResultSet()
{
	iResultArray.ResetAndDestroy();
	iResultArray.Close();
	delete  iSIResultSet;
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::CSIClientResultSet
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIClientResultSet::CSIClientResultSet()
  : iMaxResults(0),
    iResultCount(0),
    iResultArray(10),
    iSIResultSet(NULL)
{
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIClientResultSet::ConstructL()
{
    iSIResultSet = CSIResultSet::NewL();
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResultSet* CSIClientResultSet::NewLC()
{
	CSIClientResultSet* self = new (ELeave) CSIClientResultSet();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIClientResultSet* CSIClientResultSet::NewL()
{
	CSIClientResultSet* self = NewLC();
	CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::AddL
// Adds a result to the set.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResultSet::AddL( const CSIClientResult* aResult)
	{
    if ( iResultArray.Count() >= iMaxResults )
        {
        User::Leave( KErrOverflow );
        }

	TInt error = iResultArray.Append(aResult);
	User::LeaveIfError( error ); 
	}


// -----------------------------------------------------------------------------
// CSIClientResultSet::DeleteL
// Removes a result from the set.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C  void  CSIClientResultSet::DeleteL(TInt aIndex) 	
{
	if(aIndex<0 || aIndex >= iResultArray.Count())
		User::Leave(KErrArgument);
	iResultArray.Remove(aIndex);
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::SetMaxResultsL
// Sets the maximum number of results, which the client wants get.
// Less results may be returned for various reasons.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResultSet::SetMaxResultsL(TInt aMaxResults)
{
	iMaxResults = aMaxResults;

    while( iResultArray.Count() > iMaxResults )
        {
        delete iResultArray[ iMaxResults ];
        iResultArray.Remove( iMaxResults );
        }
/* 
	for (TInt i = 0; i < aMaxResults; i++)
	{
		CSIClientResult* result = CSIClientResult::NewLC();
		User::LeaveIfError(iResultArray.Append(result));
		CleanupStack::Pop();
	}
*/
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::MaxResults
// Returns the maximum number of results.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIClientResultSet::MaxResults() const
{
	return iMaxResults;
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::AtL
// Used to get a result.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIClientResult& CSIClientResultSet::AtL(TInt aIndex) const
{
 
	if(aIndex<0 || aIndex >= iResultArray.Count())
		User::Leave(KErrArgument);
	return *(iResultArray[aIndex]);
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::SetResultCount
// Sets the actual number of results, as opposed to maximum number.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResultSet::SetResultCount(TInt aResultCount)
{
	iResultCount = aResultCount;
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::ResultCount
// Gets the actual number of results, which is smaller or equal to maximum number.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIClientResultSet::ResultCount() const
{
	return iResultCount;
}
 

// -----------------------------------------------------------------------------
// CSIClientResultSet::SIClientResultSet
// Returns the nested lower-level result set.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C  CSIResultSet& CSIClientResultSet::SIResultSet() const
{
		return *iSIResultSet;
}
	
// -----------------------------------------------------------------------------
// CSIClientResultSet::SetSIResultSet
// Sets the nested result set.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResultSet::SetSIResultSet(CSIResultSet* aSIResultSet)
{
	delete iSIResultSet;
	iSIResultSet = aSIResultSet;
}

// -----------------------------------------------------------------------------
// CSIClientResultSet::ExternalizeL
// Stores the object to the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResultSet::ExternalizeL( RWriteStream& aStream ) const
    {
    // Write format ID
    aStream.WriteInt32L( KBinaryClientResultSetID );

    aStream.WriteInt32L( iMaxResults  );
    aStream.WriteInt32L( iResultCount );

    TInt count = iResultArray.Count();
    aStream.WriteInt32L( count );

    for ( TInt k = 0; k < count; k++ )
        {
        CSIClientResult* result = iResultArray[ k ];

        aStream.WriteInt32L( result->GrammarID() );
        aStream.WriteInt32L( result->RuleID()    );
        }

    if ( iSIResultSet == 0 )
        {
        aStream.WriteInt32L( KNoResultSet );
        }
    else
        {
        aStream.WriteInt32L( KYesResultSet );

        iSIResultSet->ExternalizeL( aStream );
        }
    }

// -----------------------------------------------------------------------------
// CSIClientResultSet::InternalizeL
// Restores the object from the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIClientResultSet::InternalizeL( RReadStream& aStream )
    {
    if ( aStream.ReadInt32L() != KBinaryClientResultSetID )
        {
        User::Leave( KErrCorrupt );
        }

    iMaxResults = aStream.ReadInt32L();
    iResultCount= aStream.ReadInt32L();

    TInt count  = aStream.ReadInt32L();

    for ( TInt k( 0 ); k < count; k++ )
        {
        CSIClientResult* result = CSIClientResult::NewLC();

        result->SetGrammarID( (TSIGrammarID)aStream.ReadInt32L() );
        result->SetRuleID   ( (TSIRuleID)   aStream.ReadInt32L() );

        AddL( result );

        CleanupStack::Pop( result );
        }

    TInt32 hasNestedResultSet = aStream.ReadInt32L();

    if ( hasNestedResultSet == KYesResultSet )
        {
        iSIResultSet->InternalizeL( aStream );
        }

    }

//  End of File
