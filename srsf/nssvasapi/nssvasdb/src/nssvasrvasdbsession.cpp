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
* Description:  RNssVasDbSession is the client-side(proxy) interface through which 
*               communication with the vasdb server is channelled.
*
*/


#include "nssvasrvasdbsession.h"
#include "nssvascvasdb.h"

// Complementary macro for TRAPD:
// TRAPD( err, doSomething() );
// REACT( err, return( err ) );
#define REACT(a,b) if ( a < 0 ) {b;}
	
// -----------------------------------------------------------------------------
// RNssVasDbSession::RNssVasDbSession
// C++ constructor
// -----------------------------------------------------------------------------
//
RNssVasDbSession::RNssVasDbSession()
    {
    iConnected = 0;
    }
	
// -----------------------------------------------------------------------------
// RNssVasDbSession::~RNssVasDbSession
// C++ destructor.
// -----------------------------------------------------------------------------
//
RNssVasDbSession::~RNssVasDbSession()
    {
    delete iVasDb;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::CreateDatabase
// Method to create the database.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::CreateDatabase()
    {
    TRAPD( error, iVasDb->CreateDatabaseL() );
    
    return error;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::OpenDatabase
// Method to open the database.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::OpenDatabase()
    {
    if ( iConnected )
        {
        iConnected++;
        return KErrDbAlreadyOpen;
        }

    TRAPD( error, iVasDb->OpenDatabaseL() ); 

    if ( error == KErrNone )
        {
        iConnected++;
        }

    return error;
    }
	
// -----------------------------------------------------------------------------
// RNssVasDbSession::CloseDatabase
// Method to close the database.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::CloseDatabase()
    {
    // Check if already closed (or never opened)
    if ( iConnected <= 0 )
        {
        return( KErrNone );
        }

    // Some other VAS instance in the same thread may be using the database.
    iConnected--;

    // If not, close it.
    if ( iConnected <= 0 )
        {
        return iVasDb->CloseDatabase();
        }

    return( KErrNone );
    }
	
// -----------------------------------------------------------------------------
// RNssVasDbSession::ModelBankIdLexiconId
// A method to get the model bank id and lexicon id.
// -----------------------------------------------------------------------------
//
TBool RNssVasDbSession::ModelBankIdLexiconId( TUint32& aModelBankId, TUint32& aLexiconId )
    {
    TBool ret = EFalse;

    TRAPD( err, ret = iVasDb->GetModelBankIdLexiconIdL( aModelBankId, aLexiconId ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::TagExist
// Method to check whether the context has tags or it is empty.
// -----------------------------------------------------------------------------
//
TBool RNssVasDbSession::TagExist(TInt aContextId)
    {
    TBool tagExists = EFalse;

    TRAPD( err, tagExists = iVasDb->TagExistL( aContextId ) );
    REACT( err, tagExists = EFalse );

    return( tagExists );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::TagCount
// Method to return the number of tags in a context.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::TagCount(TInt aContextId)
    {
    TInt ret = 0;

    TRAPD( err, ret = iVasDb->TagCountL( aContextId ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::SaveContext
// Method to save a context. When a context is saved, a context ID is 
// assigned to it. The newly assigned ID is returned
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::SaveContext( CNssContext& aContext, TInt& aContextId )
    {
    TRAPD( error, iVasDb->SaveContextL( aContext, aContextId ) );

	return error;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::UpdateContext
// Method to update the data of a context.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::UpdateContext(CNssContext& aContext)
    {
    TInt ret = KErrNone;

    TRAP( ret, iVasDb->UpdateContextL( aContext ) ); 

    return ret;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::UpdateContextClientData
// Method to update the client data of a context.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::UpdateContextClientData(CNssContext& aContext)
    {
    TInt ret = KErrNone;

    TRAP( ret, iVasDb->UpdateContextClientDataL( aContext ) );

    return ret;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetContextByName
// Method to get a context by name.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssContext>* RNssVasDbSession::GetContextByName(const TDesC& aName)
    {
    //CArrayFixFlat<CNssContext>* res = 0;
    CArrayPtrFlat<CNssContext>* res = 0;

    TRAPD( err, res = iVasDb->GetContextByNameL( aName ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetAllGlobalContexts
// Method to list all global contexts.
// -----------------------------------------------------------------------------
//
//CArrayFixFlat<CNssContext>* RNssVasDbSession::GetGlobalContexts()
CArrayPtrFlat<CNssContext>* RNssVasDbSession::GetGlobalContexts()
    {
    //CArrayFixFlat<CNssContext>* res = 0;
    CArrayPtrFlat<CNssContext>* res = 0;

    TRAPD( err, res = iVasDb->GetGlobalContexts() );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetAllContexts
// Method to list all contexts.
// -----------------------------------------------------------------------------
//
//CArrayFixFlat<CNssContext>* RNssVasDbSession::GetAllContexts()
CArrayPtrFlat<CNssContext>* RNssVasDbSession::GetAllContexts()
    {
    //CArrayFixFlat<CNssContext>* res = 0;
    CArrayPtrFlat<CNssContext>* res = 0;

    TRAPD( err, res = iVasDb->GetAllContexts() );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTag
// Method to get all tags in a context.
// -----------------------------------------------------------------------------
//
//TNssTagListArray* RNssVasDbSession::GetTag( const TNssContext& aContext )
CArrayPtrFlat<CNssTag>* RNssVasDbSession::GetTag( const CNssContext& aContext )
    {
    //TNssTagListArray* res = NULL;
    CArrayPtrFlat<CNssTag>* res = NULL;

    TRAPD( err, res = iVasDb->GetTagL( aContext ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTag
// Method to get a tag by grammar ID and rule ID.
// -----------------------------------------------------------------------------
//
//TNssTagListArray* RNssVasDbSession::GetTag( TNssGrammarIdRuleId aGrammarIdRuleId )
CArrayPtrFlat<CNssTag>* RNssVasDbSession::GetTag( TNssGrammarIdRuleId aGrammarIdRuleId )
    {
    //TNssTagListArray* res = NULL;
    CArrayPtrFlat<CNssTag>* res = NULL;

    TRAPD( err, res = iVasDb->GetTagL( aGrammarIdRuleId ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTags
// Method to get tags by their grammar ID and rule ID.
// -----------------------------------------------------------------------------
//
//TNssTagListArray* RNssVasDbSession::GetTags( TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds )
CArrayPtrFlat<CNssTag>* RNssVasDbSession::GetTags( 
                        TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds )
    {
    //TNssTagListArray* res = NULL;
    CArrayPtrFlat<CNssTag>* res = NULL;

    TRAPD( err, res = iVasDb->GetTagsL( aGrammarIdRuleIds ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTag
// Method to get a tag by name (CNssSpeechItem::SetText).
// -----------------------------------------------------------------------------
//
//TNssTagListArray* RNssVasDbSession::GetTag( const TDesC& aName )
CArrayPtrFlat<CNssTag>* RNssVasDbSession::GetTag( const TDesC& aName )
    {
    //TNssTagListArray* res = NULL;
    CArrayPtrFlat<CNssTag>* res = NULL;

    TRAPD( err, res = iVasDb->GetTagL( aName ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTag
// Method to get a tag by RRD data, given RRD integer and the position (0-4)
// to which the integer is compared.
// -----------------------------------------------------------------------------
//
//TNssTagListArray* RNssVasDbSession::GetTag( TInt aContextId, TInt aNum, TInt aPosition )
CArrayPtrFlat<CNssTag>* RNssVasDbSession::GetTag( TInt aContextId, TInt aNum, TInt aPosition )
    {
    //TNssTagListArray* res = NULL;
    CArrayPtrFlat<CNssTag>* res = NULL;

    TRAPD( err, res = iVasDb->GetTagL( aContextId, aNum, aPosition ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTag
// Method to get a tag by RRD data, given RRD text and the position (0-4)
// to which the text is compared.
// -----------------------------------------------------------------------------
//
//TNssTagListArray* RNssVasDbSession::GetTag( TInt aContextId, TDesC& aText, TInt aPosition )
CArrayPtrFlat<CNssTag>* RNssVasDbSession::GetTag( TInt aContextId, 
                                                  const TDesC& aText, TInt aPosition )
    {
    //TNssTagListArray* res = NULL;
    CArrayPtrFlat<CNssTag>* res = NULL;

    TRAPD( err, res = iVasDb->GetTagL( aContextId, aText, aPosition ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTag
// Method to get a tag, given the tag ID.
// -----------------------------------------------------------------------------
//
//TNssTagListArray* RNssVasDbSession::GetTag( TUint32 aTagId )
CArrayPtrFlat<CNssTag>* RNssVasDbSession::GetTag( TUint32 aTagId )
    {
    //TNssTagListArray* res = NULL;
    CArrayPtrFlat<CNssTag>* res = NULL;

    TRAPD( err, res = iVasDb->GetTagL( aTagId ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( res );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::SaveTag
// Method to save a tag. During saving, a Tag ID is assigned for the tag.
// This ID is returned in the refrence parameter.
// -----------------------------------------------------------------------------
//
//TInt RNssVasDbSession::SaveTag( TNssTag& aTag, TInt& aNewId )
TInt RNssVasDbSession::SaveTag( CNssTag& aTag, TInt& aNewId )
    {
    TInt ret = 0;

    TRAPD( err, iVasDb->SaveTagL( aTag, aNewId ) );
    REACT( err, ret = err );

    return ret;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::SaveTags
// Method to save a group of tags. During saving, a Tag IDs are assigned
// for the tags. These IDs is returned in the aTagIdArray.
// -----------------------------------------------------------------------------
//
//TInt RNssVasDbSession::SaveTags( CArrayFixFlat<TNssTag>* aTagArray, RArray<TInt>& aTagIdArray )
TInt RNssVasDbSession::SaveTags( CArrayPtrFlat<CNssTag>* aTagArray, RArray<TInt>& aTagIdArray )
    {
    TRAPD( ret, iVasDb->SaveTagsL( aTagArray, aTagIdArray ) );

    return ret;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::DeleteTag
// Method to delete a group of tags.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::DeleteTags( const RArray<TUint32>& aTagIdArray )
    {
    TRAPD( ret, iVasDb->DeleteTagsL( aTagIdArray ) );

    return ret;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::UpdateTag
// Method to update the information of an existing tag.
// -----------------------------------------------------------------------------
//
//TInt RNssVasDbSession::UpdateTag( const TNssTag& /*aTag*/ )
TInt RNssVasDbSession::UpdateTag( const CNssTag& /*aTag*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::UpdateTagRuleIDs
// Method to update the Rule IDs of some tags.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::UpdateTagRuleIDs( const RArray<TNssSpeechItem>& aRetrainedTags )
    {
    TRAPD( ret, iVasDb->UpdateTagRuleIDsL( aRetrainedTags ) );
    return( ret );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::DeleteTag
// Method to delete a tag by name.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::DeleteTag( const TDesC& aTagName )
    {
    TRAPD( error, iVasDb->DeleteTagL( aTagName ) );

    return( error );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::DeleteTag
// Method to delete a tag by identifier.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::DeleteTag( TInt aTagId )
    {
    TRAPD( error, iVasDb->DeleteTagL( aTagId ) );

    return( error );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::DeleteContext
// Method to delete a context from the database.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::DeleteContext( const TDesC& aName )
    {
    TRAPD( err, iVasDb->DeleteContextL( aName ) );

    return( err );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetTagReferenceList
// Method to get references to all tags in a context.
// -----------------------------------------------------------------------------
//
TNssTagReferenceListArray* RNssVasDbSession::GetTagReferenceList( TInt aContextId )
    {
    TNssTagReferenceListArray* result = 0;
   
    TRAPD( err, result = iVasDb->GetTagReferenceListL( aContextId ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return( result );
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::GetDefaultModelBankId
// Method to get the default model bank id
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::GetDefaultModelBankId( TUint32& aId )
    {
    TRAPD( err, iVasDb->GetDefaultModelBankIdL( aId ) );
    return err;
    }

// -----------------------------------------------------------------------------
// RNssVasDbSession::ResetModelBank
// Modifies the context table so that all contexts start using
// the new id as their model bank id.
// -----------------------------------------------------------------------------
//
TInt RNssVasDbSession::ResetModelBank( TUint32 aNewId )
    {
    TRAPD( err, iVasDb->ResetModelBankL( aNewId ) );
    return err;
    }
