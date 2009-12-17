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
* Description:  This file contains definition of the SISpeechRecognitionDataDevASR structure and API.
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <s32mem.h>
#include "nsssispeechrecognitiondatadevasr.h"
#include "nsssispeechrecognitiondatatest.h"
#include "nsssidataserialize.h"

#include "rubydebug.h"

// CONSTANTS

/** Maximal number of pronunciations, that is allowed to be stored per lexicon.
    0 means no limit.
    4800 is a maximal number of pronunciations for 400 contacts (400*12) */
const TInt KMaxPronunciations = 4800;

// Starting value for lexicon usage counter
const TInt KInitialCounterValue = 1;

// Define this if SINDE lexicon optimization is on
#define __SIND_LEXICON_OPT

/*****************************************************************************/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIParameters::CSIParameters
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIParameters::CSIParameters()
{
}

// -----------------------------------------------------------------------------
// CSIParameters::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIParameters::ConstructL()
{
} 

// -----------------------------------------------------------------------------
// CSIParameters::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIParameters* CSIParameters::NewL()
{
	CSIParameters* self = NewLC();
  	CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CSIParameters::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIParameters* CSIParameters::NewLC()
{
	CSIParameters* self = new (ELeave) CSIParameters();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;
}

// -----------------------------------------------------------------------------
// CSIParameters::~CSIParameters
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIParameters::~CSIParameters()
{
	iParameterValues.Reset();
	iParameterIDs.Reset();
}

// -----------------------------------------------------------------------------
// CSIParameters::SetParameterL
// Sets a value X to parameter Y.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIParameters::SetParameterL( const TInt aParameterID, const TInt aParameterValue )
{
	RUBY_DEBUG_BLOCKL("CSIParameters::SetParameterL");
    TInt index = iParameterIDs.Find( aParameterID );

    // If the parameter has been set earlier, update the value
    if ( index != KErrNotFound )
        {
        iParameterValues[index] = aParameterValue;
        }
    else{
		// create new item for ID and value
		User::LeaveIfError( iParameterIDs.Append( aParameterID ) );
		TInt error = iParameterValues.Append( aParameterValue );
		if ( error )
		    {
			// remove the previously added
			iParameterIDs.Remove( iParameterIDs.Count() - 1 );
			User::Leave( error );
		    }
        }
}

// -----------------------------------------------------------------------------
// CSIParameters::ParameterL
// Returns the value of a parameter.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIParameters::ParameterL( const TInt aParameterID ) const
{
	RUBY_DEBUG_BLOCK("CSIParameters::ParameterL");
    TInt index = iParameterIDs.Find( aParameterID );

    // Leave, if not found.
    User::LeaveIfError( index );

    return iParameterValues[index];
}

// -----------------------------------------------------------------------------
// CSIParameters::InternalizeL
// Restores the object from the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIParameters::InternalizeL( RReadStream& aStream )
    {
    RUBY_DEBUG_BLOCKL("CSIParameters::InternalizeL");
    if ( iParameterIDs.Count() > 0 )
        {
        User::Leave( KErrInUse );
        }

    // Check ID 
    if ( aStream.ReadInt32L() != KBinaryParameterID )
        User::Leave( KErrCorrupt );

    // Param count; can be 0.
    TInt count = aStream.ReadInt32L();

    // Read values
    for( TInt k( 0 ); k < count; k++ )
        {
        TInt id    = aStream.ReadInt32L();
        TInt value = aStream.ReadInt32L();

        SetParameterL( id, value );
        }
    }

// -----------------------------------------------------------------------------
// CSIParameters::ExternalizeL
// Stores the object to a stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIParameters::ExternalizeL(RWriteStream& aStream ) const
    {

    TInt count = iParameterIDs.Count();

    // Write ID 
    aStream.WriteInt32L( KBinaryParameterID );

    // Param count; can be 0.
    aStream.WriteInt32L( count );

    for( TInt k( 0 ); k < count; k++ )
        {
        aStream.WriteInt32L( iParameterIDs   [k] );
        aStream.WriteInt32L( iParameterValues[k] );
        }
    }

// -----------------------------------------------------------------------------
// CSIParameters::ListParametersL
// Populates given arrays with parameter IDs and values.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIParameters::ListParametersL( RArray<TInt>& aParameterIDs, 
                                              RArray<TInt>& aParameterValues ) const
    {
    RUBY_DEBUG_BLOCKL("CSIParameters::ListParametersL");
    TInt i = 0;

    aParameterIDs.Reset();
    aParameterValues.Reset();

    // Loop through all parameter IDs
    for ( i = 0; i < iParameterIDs.Count(); i++ )
        {
        aParameterIDs.Append( iParameterIDs[i] );
        }

    // Loop through all parameter values
    for ( i = 0; i < iParameterValues.Count(); i++ )
        {
        aParameterValues.Append( iParameterValues[i] );
        }
    }

/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIRuleVariant::CSIRuleVariant
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRuleVariant::CSIRuleVariant( const TSIRuleVariantID aRuleVariantID, 
										const TSILexiconID aLexiconID)
										:	iRuleVariantID(aRuleVariantID),
										iLexiconID(aLexiconID),
										iLanguage( ELangEnglish) // UK English as default
										
{
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIRuleVariant::ConstructL()
{
	CSIParameters::ConstructL();
} 

// -----------------------------------------------------------------------------
// CSIRuleVariant::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRuleVariant* CSIRuleVariant::NewL(const TSIRuleVariantID aRuleVariantID, 
											  const TSILexiconID aLexiconID)
{
	CSIRuleVariant* self =  NewLC(aRuleVariantID, aLexiconID );
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRuleVariant* CSIRuleVariant::NewLC(const TSIRuleVariantID aRuleVariantID, 
											  const TSILexiconID aLexiconID)
{
	CSIRuleVariant* self = new (ELeave) CSIRuleVariant(aRuleVariantID, aLexiconID );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::~CSIParameters
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRuleVariant::~CSIRuleVariant()
{
	iPronunciationIDs.Reset();
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::RuleVariantID
// Returns the ID of the rule variant.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleVariantID CSIRuleVariant::RuleVariantID() const
{
	return iRuleVariantID;
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::LexiconID
// Returns the ID of the lexicon, which contains the pronunciations.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TSILexiconID CSIRuleVariant::LexiconID() const
{
	return iLexiconID;
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::SetPronunciationIDsL
// Sets the pronunciation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIRuleVariant::SetPronunciationIDsL( const TSIPronunciationIDSequence& aPronunciationIDs)
{
	RUBY_DEBUG_BLOCKL("CSIRuleVariant::SetPronunciationIDsL");
	for ( TInt i( 0 ); i < aPronunciationIDs.Count(); i++)
	{
		User::LeaveIfError( iPronunciationIDs.Append( aPronunciationIDs[i] ) );
	}
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::GetPronunciationIDsL
// Fetches the pronunciation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIRuleVariant::GetPronunciationIDsL( TSIPronunciationIDSequence& aPronunciationIDs ) const
{
	RUBY_DEBUG_BLOCKL("CSIRuleVariant::GetPronunciationIDsL");
	aPronunciationIDs.Reset();
	for ( TInt i( 0 ); i < iPronunciationIDs.Count(); i++)
	{
		User::LeaveIfError( aPronunciationIDs.Append( iPronunciationIDs[i] ) );
	}
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::SetLanguage
// Sets the pronunciation language.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIRuleVariant::SetLanguage(TLanguage aLanguage)
{
	iLanguage=aLanguage;
}

// -----------------------------------------------------------------------------
// CSIRuleVariant::Language
// Gets the pronunciation language.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TLanguage CSIRuleVariant::Language() const
{
	return iLanguage;
}

/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIRule::CSIRule
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRule::CSIRule(TSIRuleID aRuleID)
:	iRuleID(aRuleID)
{
}

// -----------------------------------------------------------------------------
// CSIRule::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIRule::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSIRule::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRule* CSIRule::NewL(const TSIRuleID aRuleID)
{
	CSIRule* self = NewLC(aRuleID); 
	CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSIRule::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRule* CSIRule::NewLC(const TSIRuleID aRuleID)
{
	CSIRule* self = new (ELeave) CSIRule(aRuleID);
    CleanupStack::PushL( self );
    self->ConstructL();
	return self;
}

// -----------------------------------------------------------------------------
// CSIRule::~CSIRule
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRule::~CSIRule()
{
	iRuleVariantArray.ResetAndDestroy();
}





// -----------------------------------------------------------------------------
// CSIRule::RuleID
// Returns the ID of the rule.
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleID CSIRule::RuleID() const
{
	return iRuleID;
} 

// -----------------------------------------------------------------------------
// CSIRule::Count
// Returns the number of rule variants.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIRule::Count() const
{
	return iRuleVariantArray.Count();
}


// -----------------------------------------------------------------------------
// CSIRule::Compare
// Compares two CSIRule objects to each other based on ids.
// -----------------------------------------------------------------------------
//
TInt CSIRule::Compare( const CSIRule& aFirst, 
                       const CSIRule& aSecond )
    {
    if ( aFirst.RuleID() < aSecond.RuleID() )
        {
        return -1;
        }
    if ( aFirst.RuleID() > aSecond.RuleID() )
        {
        return 1;
        }
    // IDs are equal
    return 0;
    }

// -----------------------------------------------------------------------------
// CSIRule::AtL
// Used to get a variant.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRuleVariant& CSIRule::AtL(TInt aIndex) const
{
	// too much output RUBY_DEBUG_BLOCK("CSIRule::AtL"); 
	if(aIndex<0 || aIndex >= Count()) 
		{
		RUBY_DEBUG2("[Error!]CSIRule::AtL index [%d] is out of bounds. Count is [%d]. Leaving with KErrArgument", aIndex, Count());
		User::Leave(KErrArgument);
		}		
	return *(iRuleVariantArray[aIndex]);
}

// -----------------------------------------------------------------------------
// CSIRule::Find
// Returns the index of the variant with given id.
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt CSIRule::Find(TSIRuleVariantID aRuleVariantID) const
{
	for ( TInt i(0); i < iRuleVariantArray.Count(); i++ )
	{
		if ( aRuleVariantID == iRuleVariantArray[i]->RuleVariantID() )
		{
			return i;
		}
	} 
	return( KErrNotFound );
}

// -----------------------------------------------------------------------------
// CSIRule::AddL
// Adds a rule variant to the rule.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIRule::AddL( CSIRuleVariant* aRuleVariant )
{
	RUBY_DEBUG_BLOCKL("CSIRule::AddL");
    // Check that the rule isn't already there
	if ( Find( aRuleVariant->RuleVariantID() ) != KErrNotFound )
	{
		User::Leave( KErrAlreadyExists );
	}

    User::LeaveIfError( iRuleVariantArray.Append( aRuleVariant ) );
}

// -----------------------------------------------------------------------------
// CSIRule::RuleVariantL
// Used to get a rule variant with given ID.
// -----------------------------------------------------------------------------
//
EXPORT_C  CSIRuleVariant& CSIRule::RuleVariantL(TSIRuleVariantID aRuleVariantID) const
{
	RUBY_DEBUG_BLOCK("CSIRule::RuleVariantL");
	TInt aIndex = Find(aRuleVariantID);
	
    User::LeaveIfError( aIndex );
	
    return(AtL(aIndex));
}

// -----------------------------------------------------------------------------
// CSIRule::DeleteL
// Deletes the rule variant with given ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIRule::DeleteL(TSIRuleVariantID aRuleVariantID)
{
	RUBY_DEBUG_BLOCK("CSIRule::DeleteL");
    TInt index = User::LeaveIfError( Find( aRuleVariantID ) );

    delete  iRuleVariantArray[index];
	iRuleVariantArray.Remove(index);
}





/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIGrammar::CSIGrammar
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIGrammar::CSIGrammar(TSIGrammarID aGrammarID)
:	iGrammarID(aGrammarID)
{
}

// -----------------------------------------------------------------------------
// CSIGrammar::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIGrammar::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSIGrammar::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIGrammar* CSIGrammar::NewL(const TSIGrammarID aGrammarID)
{
    CSIGrammar* self = NewLC(aGrammarID);
	CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSIGrammar::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIGrammar* CSIGrammar::NewLC(const TSIGrammarID aGrammarID)
{
	CSIGrammar* self = new (ELeave) CSIGrammar(aGrammarID);
    CleanupStack::PushL( self );
    self->ConstructL();
	return self;
	
}

// -----------------------------------------------------------------------------
// CSIGrammar::~CSIGrammar
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIGrammar::~CSIGrammar()
{
	iRuleArray.ResetAndDestroy();
	iRuleArray.Close();
}



// -----------------------------------------------------------------------------
// CSIGrammar::GrammarID
// Returns the ID of the given grammar.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIGrammarID CSIGrammar::GrammarID() const
{
	return iGrammarID;
}


// -----------------------------------------------------------------------------
// CSIGrammar::AddL
// Adds a rule to the grammar.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIGrammar::AddL(CSIRule* aRule)
{
	// too much output RUBY_DEBUG_BLOCKL("CSIGrammar::AddL");

    // Order function which is used when finding the correct place to insert
    TLinearOrder<CSIRule> order( CSIRule::Compare );

    // InsertInOrderL does not allow duplicates
    iRuleArray.InsertInOrderL( aRule, order );
}

// -----------------------------------------------------------------------------
// CSIGrammar::DeleteL
// Deletes a rule with given ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIGrammar::DeleteL(TSIRuleID aRuleID)
{
	RUBY_DEBUG_BLOCK("CSIGrammar::DeleteL");
	TInt index = Find(aRuleID);
    User::LeaveIfError( index );

	delete iRuleArray[index];
	iRuleArray.Remove(index);
}

// -----------------------------------------------------------------------------
// CSIGrammar::AtL
// Used to get a rule.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C CSIRule& CSIGrammar::AtL(TInt anIndex) const
{
	if(anIndex<0 || anIndex >= Count())
		{
		RUBY_DEBUG2("[Error!]CSIGrammar::AtL index [%d] is out of bounds. Count is [%d]. Leaving with KErrArgument", anIndex, Count());
		User::Leave(KErrArgument);
		}
		
	return *(iRuleArray[anIndex]);
	
}

// -----------------------------------------------------------------------------
// CSIGrammar::Find
// Finds the index of the rule with given ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIGrammar::Find(TSIRuleID aRuleID) const
{
    // Order function which is used when finding the correct item
    TLinearOrder<CSIRule> order( CSIRule::Compare );

    TInt retVal = KErrNotFound;

    TRAPD( error,

        // Construct a 'dummy' CSIPronunciation so that comparison can be done 
        // against it
        CSIRule* rule = CSIRule::NewL( aRuleID );

        CleanupStack::PushL( rule );

        // Use binary search since array is kept in the order of rule ids
        retVal = iRuleArray.FindInOrderL( rule, order );
    
        CleanupStack::PopAndDestroy( rule );
        );

    if ( error != KErrNone )
        {
        return error;
        }
    return retVal;
}

// -----------------------------------------------------------------------------
// CSIGrammar::Count
// Returns the number of rules.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIGrammar::Count() const
{
	return iRuleArray.Count();
}

// -----------------------------------------------------------------------------
// CSIGrammar::RuleL
// Used to get a rule with given ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C  CSIRule&  CSIGrammar::RuleL(TSIRuleID aRuleID) const
{
	RUBY_DEBUG_BLOCK("CSIGrammar::RuleL");
	TInt aIndex = Find(aRuleID);
#ifdef _DEBUG
	if( aIndex < 0 ) 
		{
		RUBY_DEBUG2("CSIGrammar::RuleL Error [%d] finding aRuleID [%d]. Leaving", aIndex, aRuleID);
		}
#endif	
    User::LeaveIfError( aIndex );
	
    return(AtL(aIndex));
}

// -----------------------------------------------------------------------------
// CSIGrammar::ExternalizeL
// Stores the object to the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIGrammar::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteInt32L( KBinaryGrammarID );

    CSIGrammarSerializer* grammarSerializer = CSIGrammarSerializer::NewLC( *this );
    grammarSerializer->ExternalizeL( aWriteStream );
    CleanupStack::PopAndDestroy( grammarSerializer );

    aWriteStream.WriteInt32L( KBinaryGrammarID );
    }

// -----------------------------------------------------------------------------
// CSIGrammar::InternalizeL
// Restores the object from a stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIGrammar::InternalizeL( RReadStream& aReadStream )
    {
    RUBY_DEBUG_BLOCK("CSIGrammar::InternalizeL");
    if ( iRuleArray.Count() > 0 )
        {
        User::Leave( KErrInUse );
        }

    // Read header: Format ID, grammar ID and number of rules.
    if ( aReadStream.ReadInt32L() != KBinaryGrammarID )
        {
        User::Leave( KErrCorrupt );
        }

    CSIGrammarSerializer* grammarSerializer
        = CSIGrammarSerializer::NewLC( aReadStream );

    grammarSerializer->RestoreL( *this, iGrammarID );
    CleanupStack::PopAndDestroy( grammarSerializer );

    if ( aReadStream.ReadInt32L() != KBinaryGrammarID )
        {
        User::Leave( KErrCorrupt );
        }
    }

/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSICompiledGrammar::CSICompiledGrammar
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSICompiledGrammar::CSICompiledGrammar(TSIGrammarID aGrammarID 
												)
												: CSIGrammar(aGrammarID), iValidData(EFalse),  iGrammarCompilerData( NULL )
{
}

// -----------------------------------------------------------------------------
// CSICompiledGrammar::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSICompiledGrammar::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSICompiledGrammar::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSICompiledGrammar* CSICompiledGrammar::NewL(TSIGrammarID aGrammarID)
{
	CSICompiledGrammar* self = NewLC(aGrammarID) ;    
	CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSICompiledGrammar::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSICompiledGrammar* CSICompiledGrammar::NewLC(TSIGrammarID aGrammarID)
{
	CSICompiledGrammar* self = new (ELeave) CSICompiledGrammar(aGrammarID) ;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSICompiledGrammar::~CSICompiledGrammar
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSICompiledGrammar::~CSICompiledGrammar()
{
	delete iGrammarCompilerData;
}

// -----------------------------------------------------------------------------
// CSICompiledGrammar::AddL
// Adds a new rule to the grammar.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSICompiledGrammar::AddL(CSIRule* aRule)
{
	RUBY_DEBUG_BLOCKL("CSICompiledGrammar::AddL");
	iValidData = EFalse;
	CSIGrammar::AddL(aRule);
}

// -----------------------------------------------------------------------------
// CSICompiledGrammar::DeleteL
// Delets a rule with given ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSICompiledGrammar::DeleteL(TSIRuleID aRuleID)
{
	RUBY_DEBUG_BLOCK("CSICompiledGrammar::DeleteL");
	iValidData = EFalse;
	CSIGrammar::DeleteL(aRuleID);
}


// -----------------------------------------------------------------------------
// CSICompiledGrammar::SetCompiledData
// Sets the compiled grammar data.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSICompiledGrammar::SetCompiledData( HBufC8* aCompilerData )
{	 
	delete  iGrammarCompilerData;
	iGrammarCompilerData = aCompilerData;
}


// -----------------------------------------------------------------------------
// CSICompiledGrammar::CompiledData
// Gets a descriptor to the compiled grammar data.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C   TDesC8&  CSICompiledGrammar::CompiledData() const
{
	return *iGrammarCompilerData;
}

// -----------------------------------------------------------------------------
// CSICompiledGrammar::ExternalizeL
// Stores the object to the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSICompiledGrammar::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteInt32L( KBinaryCompiledGrammarID );

    CSIGrammar::ExternalizeL( aWriteStream );

    // Read compiled grammar data
    if ( iGrammarCompilerData )
        {
        TInt length = iGrammarCompilerData->Length();
        aWriteStream.WriteInt32L( length );
        aWriteStream.WriteL( iGrammarCompilerData->Des(), length );
        }
    else
        {
        aWriteStream.WriteInt32L( 0 );
        }

    // Corruption check
    aWriteStream.WriteInt32L( KBinaryCompiledGrammarID );
    }

// -----------------------------------------------------------------------------
// CSICompiledGrammar::InternalizeL
// Restores the object from a stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSICompiledGrammar::InternalizeL( RReadStream& aReadStream )
    {
    RUBY_DEBUG_BLOCK("CSICompiledGrammar::InternalizeL");
    if ( aReadStream.ReadInt32L() != KBinaryCompiledGrammarID )
        User::Leave( KErrCorrupt );

    CSIGrammar::InternalizeL( aReadStream );

    // Read compiled grammar data
    TInt length = aReadStream.ReadInt32L();

    if ( length > 0 )
        {
        iGrammarCompilerData = HBufC8::NewL( length );
        TPtr8 compilerDataPtr = iGrammarCompilerData->Des();
        aReadStream.ReadL( compilerDataPtr, length );
        }

    // Corruption check
    if ( aReadStream.ReadInt32L() != KBinaryCompiledGrammarID )
        User::Leave( KErrCorrupt );
    }

/*****************************************************************************/
// -----------------------------------------------------------------------------
// TSIRuleVariantInfo::TSIRuleVariantInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleVariantInfo::TSIRuleVariantInfo( 
												const TSIGrammarID aGrammarID, 			
												const TSIRuleID aRuleID, 
												const TSIRuleVariantID aRuleVariantID )
												:	iGrammarID( aGrammarID ),
												iRuleID( aRuleID ),
												iRuleVariantID( aRuleVariantID )
{
    // nothing
}

// -----------------------------------------------------------------------------
// TSIRuleVariantInfo::~TSIRuleVariantInfo
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleVariantInfo::~TSIRuleVariantInfo()
{
	// nothing
}


// -----------------------------------------------------------------------------
// TSIRuleVariantInfo::GrammarID
// Returns the grammar ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIGrammarID TSIRuleVariantInfo::GrammarID() const
{
    return iGrammarID;
}
// -----------------------------------------------------------------------------
// TSIRuleVariantInfo::RuleID
// Return rule identifier.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleID TSIRuleVariantInfo::RuleID() const
{
    return iRuleID;
}

// -----------------------------------------------------------------------------
// TSIRuleVariantInfo::RuleVariantID
// Return rule variant identifier.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleVariantID TSIRuleVariantInfo::RuleVariantID() const
{
    return iRuleVariantID;
}

/******************************************************************************/

// -----------------------------------------------------------------------------
// CSIPronunciation::CSIPronunciation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciation::CSIPronunciation( const TSIPronunciationID aPronunciationID,
                                             const TSIModelBankID aModelBankID )
                                            : iPronunciationID( aPronunciationID ),
                                              iModelBankID( aModelBankID ),
                                              iPhonemeSequence( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPronunciation::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CSIPronunciation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciation* CSIPronunciation::NewL( const TSIPronunciationID aPronunciationID, 
                                                   const TSIModelBankID aModelBankID )
    {
    CSIPronunciation* self = NewLC ( aPronunciationID, aModelBankID );
    CleanupStack::Pop( self );
    return self;
    }



// -----------------------------------------------------------------------------
// CSIPronunciation::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciation* CSIPronunciation::NewLC( const TSIPronunciationID aPronunciationID, 
                                                    const TSIModelBankID aModelBankID )
    {
    CSIPronunciation* self = new (ELeave) CSIPronunciation( aPronunciationID, aModelBankID );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::~CSIPronunciation
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciation::~CSIPronunciation()
    {
    delete iPhonemeSequence;
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::Compare
// Compares two CSIPronunciation objects to each other based on ids.
// -----------------------------------------------------------------------------
//
TInt CSIPronunciation::Compare( const CSIPronunciation& aFirst, 
                                const CSIPronunciation& aSecond )
    {
    if ( aFirst.PronunciationID() < aSecond.PronunciationID() )
        {
        return -1;
        }
    if ( aFirst.PronunciationID() > aSecond.PronunciationID() )
        {
        return 1;
        }
    // IDs are equal
    return 0;
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::ComparePhonemes
// Compares two CSIPronunciation objects to each other based pronunciations.
// -----------------------------------------------------------------------------
//
TInt CSIPronunciation::ComparePhonemes( const CSIPronunciation& aFirst, 
                                        const CSIPronunciation& aSecond )
    {
    if ( aFirst.PhonemeSequence() < aSecond.PhonemeSequence() )
        {
        return -1;
        }
    if ( aFirst.PhonemeSequence() > aSecond.PhonemeSequence() )
        {
        return 1;
        }
    // Phoneme sequences are equal
    return 0;
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::PronunciationID
// Returns the pronunciation ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TSIPronunciationID CSIPronunciation::PronunciationID() const
    {
    return iPronunciationID;
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::ModelBankID
// Returns the pronunciation ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TSIModelBankID CSIPronunciation::ModelBankID() const
    {
    return iModelBankID;
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::SetPhonemeSequenceL
// Sets Phoneme Sequence
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPronunciation::SetPhonemeSequenceL( const TDesC8& aPhonemeSequence )
    {
    if ( iPhonemeSequence )
        {
        // delete old data
        delete iPhonemeSequence;
        iPhonemeSequence = NULL;
        }
	
    iPhonemeSequence = HBufC8::NewL( aPhonemeSequence.Length() );
    (*iPhonemeSequence) = aPhonemeSequence;
    }


// -----------------------------------------------------------------------------
// CSIPronunciation::PhonemeSequence
// returns Phoneme Sequence
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPronunciation::PhonemeSequence() const
    {
    return *iPhonemeSequence;
    }

// -----------------------------------------------------------------------------
// CSIPronunciation::SetPronunciationID
// Sets the pronunciation ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPronunciation::SetPronunciationID( TSIPronunciationID aPronunciationID )
    {
    iPronunciationID=aPronunciationID;
    }
    
/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSILexicon::CSILexicon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSILexicon::CSILexicon( TSILexiconID aLexiconID )
                               : iLexiconID(aLexiconID)
    {
    }

// -----------------------------------------------------------------------------
// CSILexicon::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSILexicon::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSILexicon::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSILexicon* CSILexicon::NewL( TSILexiconID aLexiconID )
    {
    CSILexicon* self = NewLC( aLexiconID );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CSILexicon::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSILexicon* CSILexicon::NewLC( TSILexiconID aLexiconID )
    {
    CSILexicon* self = new ( ELeave ) CSILexicon( aLexiconID );
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;
    }

// -----------------------------------------------------------------------------
// CSILexicon::~CSILexicon
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSILexicon::~CSILexicon()
    {
    iPronunciationArray.ResetAndDestroy();
    iPronunciationArray.Close();
    iPronunOrder.Close();
    }

// -----------------------------------------------------------------------------
// CSILexicon::LexiconID
// Gets the ID of the lexicon.
// -----------------------------------------------------------------------------
//
EXPORT_C TSIGrammarID CSILexicon::LexiconID() const
    {
    return iLexiconID;
    }

// -----------------------------------------------------------------------------
// CSILexicon::AddL
// Adds a pronunciation.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSILexicon::AddL( CSIPronunciation* aPronunciation )
    {
#ifdef __SIND_LEXICON_OPT
    RUBY_DEBUG_BLOCKL( "CSILexicon::AddL (SINDE optimized)" );
    
    RUBY_ASSERT_DEBUG( iPronunciationArray.Count() == iPronunOrder.Count(), User::Leave( KErrCorrupt ) );
    
    // Check if current lexicon already holds maximal allowed number of 
    // pronunciations or that there is no limit
    if( ( KMaxPronunciations != 0 ) && ( iPronunciationArray.Count() > KMaxPronunciations ) )
    	{
    	User::Leave( KErrNoMemory );
    	}

    if ( aPronunciation == NULL )
        {
        User::Leave( KErrArgument );
        }
        
    // Check that pronunciation is not null
    const TDesC8& phonemeSeq = aPronunciation->PhonemeSequence();

    if ( &phonemeSeq == NULL )
        {
        User::Leave( KErrArgument );
        }

    // Check if CSIPronunciation which is given as parameter has the usage counter already
    TRAPD( error, TInt temp = aPronunciation->ParameterL( KLexiconReferenceCounter ) );
    TBool counterFound( EFalse );
    
    if ( error == KErrNone )
        {
        counterFound = ETrue;
        }
    else
        {
        counterFound = EFalse;
        // New counter
        aPronunciation->SetParameterL( KLexiconReferenceCounter, KInitialCounterValue );
        }

    // 1. Insert first based on the phoneme sequence order
    TLinearOrder<CSIPronunciation> phonemeOrder( CSIPronunciation::ComparePhonemes );
    TRAP( error, iPronunOrder.InsertInOrderL( aPronunciation, phonemeOrder ) );
    
    // Store the index for later use
    TInt phonemeSeqIndex = iPronunOrder.FindInOrder( aPronunciation, phonemeOrder );
      
    // Don't add the same data again
    if ( error == KErrAlreadyExists )
        {
        // Append counter only if it was not found from the parameter
        // This way externalize/internalize works ok, counter won't be touched when
        // there is one already
        if ( !counterFound )
            {
            TInt counterValue( 0 );
            // Just increase the usage counter
            CSIPronunciation* existingPronun = iPronunOrder[phonemeSeqIndex];
            counterValue = existingPronun->ParameterL( KLexiconReferenceCounter );
            counterValue++;
            existingPronun->SetParameterL( KLexiconReferenceCounter, counterValue );
            }
        User::Leave( KErrAlreadyExists );
        }
    else
        {
        User::LeaveIfError( error );
        
        TLinearOrder<CSIPronunciation> order( CSIPronunciation::Compare );
        
        // 2. Insert to the array which is kept in the order of pronunciation id
        TRAPD( error, iPronunciationArray.InsertInOrderL( aPronunciation, order ) );
        if ( error != KErrNone )
            {
            // Remove previously added element from pronunciation ordered table
            iPronunOrder.Remove( phonemeSeqIndex );
            User::Leave( error );
            }
        }

#else
    RUBY_DEBUG_BLOCKL("CSILexicon::AddL");
    
    // Check if current lexicon already hold maximal alloud number of 
    // pronunciations or that there is no limit
    if( ( KMaxPronunciations != 0 ) && ( iPronunciationArray.Count() > KMaxPronunciations ) )
    	{
    	User::Leave( KErrNoMemory );
    	}
    // Quick check, if pronun id of the added pronunciation is bigger than the
    // last one which is already in array, we can add that to the last
    if ( iPronunciationArray.Count() == 0 )
        {
        User::LeaveIfError( iPronunciationArray.Append( aPronunciation ) );
        return;
        }
    else
        {
        if ( iPronunciationArray[iPronunciationArray.Count() - 1]->PronunciationID() < aPronunciation->PronunciationID() )
            {
            User::LeaveIfError( iPronunciationArray.Append( aPronunciation ) );
            return;
            }
        }

    // Check that pronunciation is not null
    const TDesC8& phonemeSeq = aPronunciation->PhonemeSequence();

    if ( &phonemeSeq == NULL )
        {
        User::Leave( KErrArgument );
        }

    // Order function which is used when finding the correct place to insert
    TLinearOrder<CSIPronunciation> order( CSIPronunciation::Compare );

    // InsertInOrderL does not allow duplicates, that is what we actually want!
    iPronunciationArray.InsertInOrderL( aPronunciation, order );
#endif // __SIND_LEXICON_OPT
    }

// -----------------------------------------------------------------------------
// CSILexicon::AddPronunciationToEndL
// Adds pronunciation to end of the array
// -----------------------------------------------------------------------------
//
void CSILexicon::AddPronunciationToEndL( CSIPronunciation* aPronunciation )
    {   
    TLinearOrder<CSIPronunciation> phonemeOrder( CSIPronunciation::ComparePhonemes );    
    TInt error = iPronunciationArray.Append( aPronunciation );
    if ( error != KErrNone )
        {
        // Remove previously added element from pronunciation ordered table
        iPronunOrder.Remove( iPronunOrder.FindInOrder( aPronunciation, phonemeOrder ) );
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CSILexicon::AtL
// Used to get a pronunciation.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciation& CSILexicon::AtL( TInt anIndex ) const
    {
    RUBY_DEBUG_BLOCKL("CSILexicon::AtL");
    if ( anIndex < 0 || anIndex >= Count() )
        {
        RUBY_DEBUG2("CSILexicon::AtL index [%d] is ut of bounds. Count [%d]. Leaving with KErrArg", anIndex, Count());
        User::Leave( KErrArgument );
        }
	return *( iPronunciationArray[anIndex] );
    } 

// -----------------------------------------------------------------------------
// CSILexicon::DeleteL
// Deletes a pronunciation with given ID
// -----------------------------------------------------------------------------
//
EXPORT_C void CSILexicon::DeleteL( TSIPronunciationID aPronunciationID )
    {
#ifdef __SIND_LEXICON_OPT
    RUBY_DEBUG_BLOCK( "CSILexicon::DeleteL (SINDE optimized)" );
    
    TInt pronunIdIndex = Find( aPronunciationID );
    User::LeaveIfError( pronunIdIndex );
    
    CSIPronunciation& pronunciation = AtL( pronunIdIndex );
    
    TInt counter = pronunciation.ParameterL( KLexiconReferenceCounter );
    counter--;
    
    // If counter reaches zero, then pronunciation should be also deleted
    if ( counter == 0 )
        {
        // Find the index in array ordered by phoneme sequence
        TLinearOrder<CSIPronunciation> phonemeOrder( CSIPronunciation::ComparePhonemes );
        TInt phonemeSeqIndex = iPronunOrder.FindInOrder( &pronunciation, phonemeOrder );
        User::LeaveIfError( phonemeSeqIndex );
       
        // Remove from array ordered by pronunciation        
        iPronunOrder.Remove( phonemeSeqIndex );
        
        // Remove from array ordered by pronunciation id
        delete iPronunciationArray[pronunIdIndex];
        iPronunciationArray.Remove( pronunIdIndex );        
        }
    else
        {
        pronunciation.SetParameterL( KLexiconReferenceCounter, counter );
        }
#else
    RUBY_DEBUG_BLOCK("CSILexicon::DeleteL");
    TInt index = Find( aPronunciationID );
    User::LeaveIfError( index );

    delete iPronunciationArray[index];
    iPronunciationArray.Remove( index );
#endif
    }

// -----------------------------------------------------------------------------
// CSILexicon::Find
// Finds the index of pronunciation with the given ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSILexicon::Find( TSIPronunciationID aPronunciationID ) const
    {
    // Order function which is used when finding the correct item
    TLinearOrder<CSIPronunciation> order( CSIPronunciation::Compare );

    TInt retVal = KErrNotFound;

    // Construct a 'dummy' CSIPronunciation so that comparison can be done 
    // against it
    CSIPronunciation* pronun = NULL;
    TRAPD( error, pronun = CSIPronunciation::NewL( aPronunciationID, 0 ) );
    if ( error )
        {
        return error;
        }

    // Use binary search since array is kept in the order of pronunciation ids
    TRAP( error, retVal = iPronunciationArray.FindInOrderL( pronun, order ) );
    delete pronun;
    if ( error )
        {
        return error;
        }
    return retVal;
    } 

// -----------------------------------------------------------------------------
// CSILexicon::Find
// Finds the index of the pronunciation with the given phoneme sequence.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSILexicon::Find( const TDesC8& aPhonemeSequence ) const
    {
#ifdef __SIND_LEXICON_OPT    
    // Order function which is used when finding the correct item
    TLinearOrder<CSIPronunciation> order( CSIPronunciation::ComparePhonemes );

    TInt retVal = KErrNotFound;

    // Construct a 'dummy' CSIPronunciation so that comparison can be done 
    // against it
    CSIPronunciation* pronun = NULL;
    TRAPD( error, 
        pronun = CSIPronunciation::NewL( 0, 0 );
        pronun->SetPhonemeSequenceL( aPhonemeSequence );
        // Use binary search since array is kept in the order of pronunciation ids
        retVal = iPronunOrder.FindInOrderL( pronun, order );
        ); // TRAPD
    delete pronun;
    if ( error )
        {
        return error;
        }
    
    // retVal is index to iPronunOrder array, but what we actually need is 
    // index to iPronunciationArray
    pronun = iPronunOrder[ retVal ];
            
    return Find( pronun->PronunciationID() );
#else    
    for ( TInt i=0; i < iPronunciationArray.Count(); i++ )
	{
		if ( aPhonemeSequence == iPronunciationArray[i]->PhonemeSequence() )
		{
			return i;
		}
	}
	return KErrNotFound;
#endif // __SIND_LEXICON_OPT	
    }

// -----------------------------------------------------------------------------
// CSILexicon::Count
// Counts the number of pronunciations.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSILexicon::Count() const
    {
    return iPronunciationArray.Count();
    }

// -----------------------------------------------------------------------------
// CSILexicon::ExternalizeL
// Stores the object to the stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSILexicon::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    // Externalization involves so many data structures,
    // That I put them to a dedicated class, CSILexiconSerializer.
    CSILexiconSerializer* serializer = CSILexiconSerializer::NewLC( *this );

    aWriteStream.WriteInt32L( KBinaryLexiconID );
    serializer->ExternalizeL( aWriteStream );
    aWriteStream.WriteInt32L( KBinaryLexiconID );

    CleanupStack::PopAndDestroy( serializer );
    }

// -----------------------------------------------------------------------------
// CSILexicon::InternalizeL
// Restores the object from the stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSILexicon::InternalizeL( RReadStream& aReadStream )
    {
    // Header should be KBinaryLexiconID
    if ( aReadStream.ReadInt32L() != KBinaryLexiconID )
        {
        User::Leave( KErrCorrupt );
        }

    CSILexiconSerializer* serializer
        = CSILexiconSerializer::NewLC( aReadStream );

    serializer->RestoreL( *this, iLexiconID );

    CleanupStack::PopAndDestroy( serializer );

    // Last integer should be KBinaryLexiconID
    if ( aReadStream.ReadInt32L() != KBinaryLexiconID )
        {
        User::Leave( KErrCorrupt );
        }
    }

/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIModel::CSIModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModel::CSIModel( const TSIModelID aModelID )
:	iModelID(aModelID),
iAcousticModel(NULL)
{
}

// -----------------------------------------------------------------------------
// CSIModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIModel::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSIModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModel* CSIModel::NewL( const TSIModelID aModelID = 0 )
{
	CSIModel* self = NewLC( aModelID );
 	CleanupStack::Pop( self );
    return self;
}


// -----------------------------------------------------------------------------
// CSIModel::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModel* CSIModel::NewLC( const TSIModelID aModelID = 0 )
{
	CSIModel* self = new (ELeave) CSIModel( aModelID );
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;
}

// -----------------------------------------------------------------------------
// CSIModel::~CSIModel
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModel::~CSIModel()
{
	delete iAcousticModel;
}

// -----------------------------------------------------------------------------
// CSIModel::SetModelID
// Sets the model ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIModel::SetModelID(const TSIModelID aModelID)
{
	iModelID = aModelID;
}

// -----------------------------------------------------------------------------
// CSIModel::ModelID
// Returns the model ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIModelID CSIModel::ModelID() const
{
	return iModelID;
}

// -----------------------------------------------------------------------------
// CSIModel::SetAcousticModel
// Sets the model data. CSIModel takes ownership of the data.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIModel::SetAcousticModel( HBufC8*  aAcousticModel)
{	 
	delete iAcousticModel;
	iAcousticModel=aAcousticModel;
}

// -----------------------------------------------------------------------------
// CSIModel::AcousticModel
// Used to get the acoustic data of the model.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C   TDesC8& CSIModel::AcousticModel() const
{
	return *iAcousticModel;
}

/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIModelBank::CSIModelBank
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModelBank::CSIModelBank( const TSIModelBankID aModelBankID)
:	iModelBankID(aModelBankID)
{
}

// -----------------------------------------------------------------------------
// CSIModelBank::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIModelBank::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSIModelBank::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModelBank* CSIModelBank::NewL(const TSIModelBankID aModelBankID)
{
	CSIModelBank* self =NewLC(aModelBankID);
 	CleanupStack::Pop( self );
    return self;
}


// -----------------------------------------------------------------------------
// CSIModelBank::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModelBank* CSIModelBank::NewLC(const TSIModelBankID aModelBankID)
{
	CSIModelBank* self = new (ELeave) CSIModelBank(aModelBankID);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSIModelBank::~CSIModelBank
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModelBank::~CSIModelBank()
{
	iModelArray.ResetAndDestroy();
	iModelArray.Close();
}

// -----------------------------------------------------------------------------
// CSIModelBank::ModelBankID
// Returns the ID of the model bank.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIModelBankID CSIModelBank::ModelBankID() const
{
	return iModelBankID;
}

// -----------------------------------------------------------------------------
// CSIModelBank::AddL
// Adds a new model to the bank.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIModelBank::AddL( const CSIModel* aModel)
{
	if(Find(aModel->ModelID()) == KErrNotFound)
	{
		iModelArray.Append(aModel);
	}
	else
	{
		User::Leave(KErrAlreadyExists);
		
	}
}

// -----------------------------------------------------------------------------
// CSIModelBank::AtL
// Used to get a model from the bank.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C CSIModel& CSIModelBank::AtL( const TInt anIndex) const
{
	if(anIndex<0 || anIndex >= Count())
		User::Leave(KErrArgument);
	
	return *(iModelArray[anIndex]);
}

// -----------------------------------------------------------------------------
// CSIModelBank::Find
// Finds a model with the given ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIModelBank::Find(const TSIModelID aModelID) const
{
	for(TInt i=0; i < iModelArray.Count(); i++)
	{
		if(aModelID == iModelArray[i]->ModelID())
		{
			return i;
		}
	}
	return KErrNotFound;
}

// -----------------------------------------------------------------------------
// CSIModelBank::DeleteL
// Deletes a model with the given ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIModelBank::DeleteL(TSIModelID aModelID)
{
	TInt index = Find(aModelID );
	if ( index != KErrNotFound)
	{
		delete iModelArray[index];
		iModelArray.Remove(index);
	}
	
	else  {
		User::Leave( KErrNotFound );
	}
}

// -----------------------------------------------------------------------------
// CSIModelBank::Count
// Returns the number of models in the bank.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIModelBank::Count() const
{
	return iModelArray.Count();
}

/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::CSIPronunciationInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciationInfo::CSIPronunciationInfo( RPointerArray<HBufC8> aPronunciation,
                                                     TLanguage aLanguage )
                                                   : iPronunciationArray( aPronunciation ),
                                                     iLanguage( aLanguage )
{
    // nothing
}

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPronunciationInfo::ConstructL()
{
	
	CSIParameters::ConstructL();
} 

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciationInfo* CSIPronunciationInfo::NewL( RPointerArray<HBufC8> aPronunciation ,TLanguage aLanguage  )
{
	CSIPronunciationInfo* self = NewLC( aPronunciation,aLanguage );
	CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciationInfo* CSIPronunciationInfo::NewLC( RPointerArray<HBufC8> aPronunciation ,TLanguage aLanguage  )
{
	CSIPronunciationInfo* self = new (ELeave) CSIPronunciationInfo( aPronunciation,aLanguage );
    CleanupStack::PushL( self );
    self->ConstructL();
	return self;
}

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::~CSIPronunciationInfo
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPronunciationInfo::~CSIPronunciationInfo()
{
	iPronunciationArray.ResetAndDestroy();
	
} 


// -----------------------------------------------------------------------------
// CSIPronunciationInfo::SetPronunciationL
// Sets the index and phoneme sequence.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPronunciationInfo::SetPronunciationL(TInt aIndex, HBufC8* aPronunciation)
{
	if( aIndex < 0 || aIndex > iPronunciationArray.Count() )
        {
        // wrong index
		User::Leave( KErrArgument );
        }
    else if ( aIndex < iPronunciationArray.Count() )
        {
        // replace old pronunciation
        delete iPronunciationArray[aIndex];
	    iPronunciationArray[aIndex] = aPronunciation;
        }
    else if ( aIndex == iPronunciationArray.Count() )
        {
        // append new pronunciation
        iPronunciationArray.Append( aPronunciation );
        }
}

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::PhonemeSequence
// Return phoneme sequence.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CSIPronunciationInfo::PronunciationL(TInt aIndex) const
{
	if(aIndex<0 || aIndex >= Count())
		User::Leave(KErrArgument);
	
    return *(iPronunciationArray[aIndex]);
}

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::Language
// Returns language.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TLanguage CSIPronunciationInfo::Language() const
{
	return iLanguage;
}

// -----------------------------------------------------------------------------
// CSIPronunciationInfo::Count
// Returns the number of pronunciations.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPronunciationInfo::Count() const
{
	return  iPronunciationArray.Count();
}



/******************************************************************************/

// -----------------------------------------------------------------------------
// CSITtpWordList::CSITtpWordList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSITtpWordList::CSITtpWordList() 
: 
iReserved( NULL )
{
}

// -----------------------------------------------------------------------------
// CSITtpWordList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSITtpWordList::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSITtpWordList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSITtpWordList* CSITtpWordList::NewL()
{
    CSITtpWordList* self = NewLC();
    CleanupStack::Pop( self );	
    return self;
}

// -----------------------------------------------------------------------------
// CSITtpWordList::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSITtpWordList* CSITtpWordList::NewLC()
{
    CSITtpWordList* self = new ( ELeave ) CSITtpWordList();   
    CleanupStack::PushL( self );
    self->ConstructL();
     return self;
}

// Destructor
// -----------------------------------------------------------------------------
// CSITtpWordList::~CSITtpWordList
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSITtpWordList::~CSITtpWordList()
{
	iWordArray.ResetAndDestroy();
	iIndexArray.Close();
	iPronunciationArray.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
// CSITtpWordList::AddL
// Add word to list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSITtpWordList::AddL(
								   MDesCArray* aWords)
{
	TInt error = iWordArray.Append( aWords );
	User::LeaveIfError( error );
	
}

// -----------------------------------------------------------------------------
// CSITtpWordList::Count
// Return number of words.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSITtpWordList::Count() const
{
	return iWordArray.Count();
}

// -----------------------------------------------------------------------------
// CSITtpWordList::At
// Give text of given index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C  MDesCArray& CSITtpWordList::AtL( 
										  const TInt aIndex ) const
{
	if(aIndex<0 || aIndex >= Count())
		User::Leave(KErrArgument);
	return *(iWordArray[aIndex]);
}

// -----------------------------------------------------------------------------
// CSITtpWordList::DeleteL
// Deletes a word.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C   void CSITtpWordList::DeleteL(TInt aIndex) 	{
	if(aIndex<0 || aIndex >= Count())
		User::Leave(KErrArgument);
	delete iWordArray[aIndex];
	iWordArray.Remove(aIndex);
	
	// delete pronunciation which index match to given
	for ( TInt i(0); i < iIndexArray.Count(); i++ )
	{
		if ( iIndexArray[i] == aIndex )
		{
			delete iPronunciationArray[i];
			iPronunciationArray.Remove(i);
		}
	}
}

// -----------------------------------------------------------------------------
// CSITtpWordList::SetPronunciationL
// Put pronunciation to given index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSITtpWordList::AppendPronunciationL( 
													const TInt aIndex, 
													CSIPronunciationInfo* aPronunciation )
{
	TInt error = iPronunciationArray.Append( aPronunciation );
	User::LeaveIfError( error );
	error = iIndexArray.Append(  aIndex );
	if ( error )
	{
		// remove the added pronunciation
		iPronunciationArray.Remove( iPronunciationArray.Count() - 1 );
		User::Leave( error );
	}
}

// -----------------------------------------------------------------------------
// CSITtpWordList::GetPronunciationsL
// Give pronunciation information of given index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSITtpWordList::GetPronunciationsL( 
												 const TInt aIndex,
												 RPointerArray<CSIPronunciationInfo>& aPronunciations ) const
{
	// clear given array
	aPronunciations.Reset();
	
	// add pronunciation which index match to given
	for ( TInt i(0); i < iIndexArray.Count(); i++ )
	{
		if ( iIndexArray[i] == aIndex )
		{
			TInt error = aPronunciations.Append( iPronunciationArray[i] );
			User::LeaveIfError( error );
		}
	}
}
/*****************************************************************************/

// -----------------------------------------------------------------------------
// CSIResult::CSIResult
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResult::CSIResult()
:	iGrammarID(0),
iRuleID(0), iRuleVariantID(0)
{
}

// -----------------------------------------------------------------------------
// CSIResult::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResult::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSIResult::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResult* CSIResult::NewL()
{
	CSIResult* self = NewLC();
  	CleanupStack::Pop( self );
    return self;
}



// -----------------------------------------------------------------------------
// CSIResult::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResult* CSIResult::NewLC()
{
	CSIResult* self = new (ELeave) CSIResult();
    CleanupStack::PushL( self );
    self->ConstructL();
     return self;
}

// -----------------------------------------------------------------------------
// CSIResult::~CSIResult
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResult::~CSIResult()
{
	delete iSIPronunciation;
}

// -----------------------------------------------------------------------------
// CSIResult::SetGrammarID
// Sets the grammar ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResult::SetGrammarID(const TSIGrammarID aGrammarID)
{
	iGrammarID = aGrammarID;
}

// -----------------------------------------------------------------------------
// CSIResult::GrammarID
// Returns the grammar ID.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIGrammarID CSIResult::GrammarID() const
{
	return iGrammarID;
}

// -----------------------------------------------------------------------------
// CSIResult::SetRuleID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResult::SetRuleID(const TSIRuleID aRuleID)
{
	iRuleID = aRuleID;
}

// -----------------------------------------------------------------------------
// CSIResult::RuleID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleID CSIResult::RuleID() const
{
	return iRuleID;
}

// -----------------------------------------------------------------------------
// CSIResult::SetRuleVariantID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResult::SetRuleVariantID(const TSIRuleVariantID aRuleVariantID)
{
	iRuleVariantID = aRuleVariantID;
}

// -----------------------------------------------------------------------------
// CSIResult::RuleVariantID
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TSIRuleVariantID CSIResult::RuleVariantID() const
{
	return iRuleVariantID;
}

// -----------------------------------------------------------------------------
// CSIResult::SetScore
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResult::SetScore(const TInt32 aScore)
{
	iScore = aScore;
}

// -----------------------------------------------------------------------------
// CSIResult::Score
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CSIResult::Score() const
{
	return iScore;
}

// -----------------------------------------------------------------------------
// CSIResult::Pronunciation
// Returns the pronunciation of the recognized word.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C  CSIPronunciation& CSIResult::Pronunciation() const 
{
	return *iSIPronunciation;
}

// -----------------------------------------------------------------------------
// CSIResult::SetPronunciation
// Returns the recognized pronunciation.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResult::SetPronunciation( CSIPronunciation* aSIPronunciation) 
{										     
	iSIPronunciation=aSIPronunciation;
}


/*****************************************************************************/


// -----------------------------------------------------------------------------
// CSIResultSet::CSIResultSet
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResultSet::CSIResultSet()
:	iAdaptationData(NULL)
{
}

// -----------------------------------------------------------------------------
// CSIResultSet::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResultSet::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CSIResultSet::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResultSet* CSIResultSet::NewL()
{
	CSIResultSet* self = NewLC();
	CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CSIResultSet::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResultSet* CSIResultSet::NewLC()
{
	CSIResultSet* self = new (ELeave) CSIResultSet();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;
}

// -----------------------------------------------------------------------------
// CSIResultSet::~CSIResultSet
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIResultSet::~CSIResultSet()
{
	iResultArray.ResetAndDestroy();
	iResultArray.Close();
	delete iAdaptationData ;
}

// -----------------------------------------------------------------------------
// CSIResultSet::AddL
// Adds a result to the set.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResultSet::AddL( const CSIResult* aResult)
{
	TInt error = iResultArray.Append(aResult);
	User::LeaveIfError( error ); 
}

// -----------------------------------------------------------------------------
// CSIResultSet::AtL
// Used to get a result.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C  CSIResult& CSIResultSet::AtL(const TInt aIndex)
{
	if(aIndex<0 || aIndex >= Count())
		User::Leave(KErrArgument);
	return *(iResultArray[aIndex]);
}

EXPORT_C  const CSIResult& CSIResultSet::AtL(const TInt aIndex) const
{
	if(aIndex<0 || aIndex >= Count())
		User::Leave(KErrArgument);
	return *(iResultArray[aIndex]);
}

// -----------------------------------------------------------------------------
// CSIResultSet::Count
// Counts the number of results in the set.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIResultSet::Count() const
{
	return iResultArray.Count();
}

// -----------------------------------------------------------------------------
// CSIResultSet::DeleteL
// Deletes a result. 
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C  void  CSIResultSet::DeleteL(TInt aIndex) 	
{
	if(aIndex<0 || aIndex >= Count())
		User::Leave(KErrArgument);
	iResultArray.Remove(aIndex);
}

// -----------------------------------------------------------------------------
// CSIResultSet::SetAdaptationData
// Sets the adaptation data.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C  void CSIResultSet::SetAdaptationData( HBufC8* aAdaptationData )
{
	delete iAdaptationData;
	iAdaptationData =aAdaptationData;
}

// -----------------------------------------------------------------------------
// CSIResultSet::AdaptationData
// Gets the adaptation data.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C   TDesC8& CSIResultSet::AdaptationData( )
{  
	return *iAdaptationData;
}

// -----------------------------------------------------------------------------
// CSIResultSet::ExternalizeL
// Stores the object to the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResultSet::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteInt32L( KBinaryResultSetID );

    if ( iAdaptationData != NULL )
        {
        aWriteStream.WriteInt32L( iAdaptationData->Length() );
        aWriteStream.WriteL( *iAdaptationData );
        }
    else{
        aWriteStream.WriteInt32L( 0 );
        }

    aWriteStream.WriteInt32L( iResultArray.Count() );

    // Write the data
    for ( TInt k( 0 ); k < iResultArray.Count(); k++ )
        {
        CSIResult*        result = iResultArray[ k ];
        const CSIPronunciation& pronun = result->Pronunciation();

        aWriteStream.WriteInt32L( result->GrammarID() );
        aWriteStream.WriteInt32L( result->RuleID() );
        aWriteStream.WriteInt32L( result->RuleVariantID() );
        aWriteStream.WriteInt32L( result->Score() );
        result->CSIParameters::ExternalizeL( aWriteStream );

        // !!!! Crashes, if pronunciation is zero.

        // Pronunciations
        const TDesC8& seq = pronun.PhonemeSequence();
        TInt length = seq.Length();

        aWriteStream.WriteInt32L( length );
        aWriteStream.WriteL( seq );

        aWriteStream.WriteInt32L( pronun.PronunciationID() );
        aWriteStream.WriteInt32L( pronun.ModelBankID() );
        pronun.ExternalizeL( aWriteStream );
        }

    aWriteStream.WriteInt32L( KBinaryResultSetID );
    }

// -----------------------------------------------------------------------------
// CSIResultSet::InternalizeL
// Restores the object from the stream.
// (other items were commented in the headers)
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIResultSet::InternalizeL( RReadStream& aReadStream )
    {
    if ( iAdaptationData != NULL || iResultArray.Count() > 0 )
        {
        User::Leave( KErrInUse );
        }

    if ( aReadStream.ReadInt32L() != KBinaryResultSetID )
        {
        User::Leave( KErrCorrupt );
        }

    TInt length = aReadStream.ReadInt32L();

    if ( length )
        {
        iAdaptationData = HBufC8::NewL( length );
        TPtr8 adaptationDataDes = iAdaptationData->Des();
        aReadStream.ReadL( adaptationDataDes, length );
        }
    else{
        iAdaptationData = 0;
        }

    TInt resultCount = aReadStream.ReadInt32L();

    for ( TInt k = 0; k < resultCount; k++ )
        {
        CSIResult*        result = CSIResult::NewLC();

        result->SetGrammarID    ( (TSIGrammarID)    aReadStream.ReadInt32L() );
        result->SetRuleID       ( (TSIRuleID)       aReadStream.ReadInt32L() );
        result->SetRuleVariantID( (TSIRuleVariantID)aReadStream.ReadInt32L() );
        result->SetScore        ( aReadStream.ReadInt32L() );
        result->CSIParameters::InternalizeL( aReadStream );

        // Pronunciations

        TInt length = aReadStream.ReadInt32L();
        HBufC8* seq = HBufC8::NewLC( length );

        TPtr8 seqPtr = seq->Des();
        aReadStream.ReadL( seqPtr, length );

        TSIPronunciationID pronunciationID = aReadStream.ReadInt32L();
        TSIModelBankID modelBankID 
            = (TSIModelBankID)aReadStream.ReadInt32L();

        CSIPronunciation* pronun
            = CSIPronunciation::NewLC( pronunciationID, modelBankID );
        pronun->SetPhonemeSequenceL( *seq );

        pronun->CSIParameters::InternalizeL( aReadStream );

        result->SetPronunciation( pronun );

        AddL( result );

        CleanupStack::Pop( pronun );
        CleanupStack::PopAndDestroy( seq );
        CleanupStack::Pop( result );
        }

    if ( aReadStream.ReadInt32L() != KBinaryResultSetID )
        User::Leave( KErrCorrupt );
    }

//		void DeleteL(TInt aIndex);

//  End of File
