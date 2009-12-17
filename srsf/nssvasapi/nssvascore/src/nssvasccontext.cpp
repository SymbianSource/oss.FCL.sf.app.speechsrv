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
* Description:  CNssContext class does all context processing. It implements 
*               MNssContext interface.
*
*/


// INCLUDE FILES
#include "nssvasccontext.h"
#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================  

// ---------------------------------------------------------
// CNssContext::CNssContext
// C++ default constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
//
CNssContext::CNssContext( CNssContextSrsPortal* aContextSrsPortal )
    {
    // These take forever in strain test
    RUBY_DEBUG0( "CNssContext::CNssContext" );
    
    iContextSrsPortal = aContextSrsPortal;
    iGlobal = EFalse;
    iContextId = KNssVASDbDefaultValue; 
    iContextSrsPortal->Register();
    iRecognitionMode = ENSSSdSiMode;
    }

// ---------------------------------------------------------
// CNssContext::~CNssContext
// Destructor
// ---------------------------------------------------------
//
CNssContext::~CNssContext()
    {
    // These take forever in strain test:
    // RUBY_DEBUG0( "CNssContext::~CNssContext" );
    
    if ( iContextSrsPortal )
        {
        iContextSrsPortal->Deregister();
        }
    if ( iName )
        {
        delete iName;
        }
    }

// ---------------------------------------------------------
// CNssContext::ContextName
// returns the name of the context as a reference
// ---------------------------------------------------------
//
TDesC& CNssContext::ContextName()
    {
    return *iName;
    }

// ---------------------------------------------------------
// CNssContext::IsGlobal
// returns the global flag
// ---------------------------------------------------------
//
TBool CNssContext::IsGlobal()
    {
    return iGlobal;
    }

// ---------------------------------------------------------
// CNssContext::SetNameL
// sets the name of the context to aName, 
// old name, if any is deleted.
// ---------------------------------------------------------
//
void CNssContext::SetNameL( const TDesC& aName )
    {
    if ( iName )
        {
        delete iName;
        iName = NULL;
        }
    iName = aName.AllocL(); 
    }

// ---------------------------------------------------------
// CNssContext::SetGlobal
// sets the global flag to ETrue or EFalse
// ---------------------------------------------------------
//
void CNssContext::SetGlobal( TBool aGlobal )
    {
	iGlobal = aGlobal;
	}

// ---------------------------------------------------------
// CNssContext::ContextId
// return the context id
// for internal use
// ---------------------------------------------------------
//
TInt CNssContext::ContextId() const
    {
	return iContextId;
    }

// ---------------------------------------------------------
// CNssContext::GrammarId
// returns the grammar id
// for internal use
// ---------------------------------------------------------
//
TUint32 CNssContext::GrammarId() const
	{
	return iGrammarId;
	}

// ---------------------------------------------------------
// CNssContext::LexiconId
// returns the lexicon id
// for internal use
// ---------------------------------------------------------
//
TUint32 CNssContext::LexiconId() const
	{
	return iLexiconId;
	}

// ---------------------------------------------------------
// CNssContext::ModelBankId
// returns the model bank id
// for internal use
// ---------------------------------------------------------
//
TUint32 CNssContext::ModelBankId() const
	{
    return iModelBankId;
	}

// ---------------------------------------------------------
// CNssContext::RecognitionMode
// returns the recognition technology
// for internal use
// ---------------------------------------------------------
//
TNSSRecognitionMode CNssContext::RecognitionMode() const
	{
    return iRecognitionMode;
	}

// ---------------------------------------------------------
// CNssContext::SetContextId
// sets the context id to the given value
// for internal use
// ---------------------------------------------------------
//
void CNssContext::SetContextId( TInt aContextId )
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------
// CNssContext::SetGrammarId
// sets the grammar id to the given value
// for internal use
// ---------------------------------------------------------
//
void CNssContext::SetGrammarId( TUint32 aGrammarId )
	{
	iGrammarId = aGrammarId;
	}

// ---------------------------------------------------------
// CNssContext::SetLexiconId
// sets the lexicon id to the given value
// for internal use
// ---------------------------------------------------------
//
void CNssContext::SetLexiconId( TUint32 aLexiconId )
	{
	iLexiconId = aLexiconId;
	}

// ---------------------------------------------------------
// CNssContext::SetModelBankId
// sets the model bank id to the given value
// for internal use
// ---------------------------------------------------------
//
void CNssContext::SetModelBankId( TUint32 aModelBankId )
	{
	iModelBankId = aModelBankId;
	}

// ---------------------------------------------------------
// CNssContext::SetRecognitionMode
// sets the model bank id to the given value
// for internal use
// ---------------------------------------------------------
//
void CNssContext::SetRecognitionMode( TNSSRecognitionMode aRecogMode )
	{
	iRecognitionMode = aRecogMode;
	}

// ---------------------------------------------------------
// CNssContext::SaveToSrsL
// for internal use
// This method is used to save the context to the SRS.
// This creates the model bank, 
// lexicon, and grammar for a context. Model bank, and lexicon 
// are created only for the first time, and are thus, fixed.
// For every context, there is one grammar id. After, model bank,
// lexicon, and grammar are created, the corresponding Ids are 
// set in the context object.
// ---------------------------------------------------------
//
void CNssContext::BeginSaveToSrsL( MNssCoreSrsDBEventHandler* aSrsDBEventHandler )
	{
	iContextSrsPortal->BeginSaveContextL( this, aSrsDBEventHandler );
	}

// ---------------------------------------------------------
// CNssContext::DeleteFromSrsL
// for internal use
// This method is used to delete a context from the SRS.
// This deletes the grammar from the SRS. 
// ---------------------------------------------------------
//
void CNssContext::BeginDeleteFromSrsL( MNssCoreSrsDBEventHandler* aSrsDBEventHandler )
	{
	iContextSrsPortal->BeginDeleteContextL( this, aSrsDBEventHandler );
	}

// ---------------------------------------------------------
// CNssContext::CommitSrsChanges
// for internal use
// This method is used to commit a context change from the SRS.
// ---------------------------------------------------------
//
TInt CNssContext::CommitSrsChanges()
	{
	return iContextSrsPortal->CommitSaveContext();
	}

// ---------------------------------------------------------
// CNssContext::RollbackSrsChanges
// for internal use
// This method is used to commit a context change from the SRS.
// ---------------------------------------------------------
//
TInt CNssContext::RollbackSrsChanges()
	{
	return iContextSrsPortal->RollbackSaveContext();
	}

// ---------------------------------------------------------
// CNssContext::SetModelBankAndLexiconExist
// for internal use
// sets the iModelBankAndLexiconExist flag to ETrue or EFalse
// ---------------------------------------------------------
//
void CNssContext::SetModelBankAndLexiconExist( TBool aVal )
    {
	iModelBankAndLexiconExist = aVal;
    }

// ---------------------------------------------------------
// CNssContext::ModelBankAndLexiconExist
// for internal use
// gets the value of the iModelBankAndLexiconExist flag
// ---------------------------------------------------------
//
TBool CNssContext::ModelBankAndLexiconExist()
    {
	return iModelBankAndLexiconExist;
    }

// ---------------------------------------------------------
// CNssContext::operator=
// for internal use
// overloaded assignment operator
// ---------------------------------------------------------
//
CNssContext& CNssContext::operator=( const CNssContext& aContext )
    {
    RUBY_DEBUG0( "CNssContext::operator=" );
    
    if ( this != &aContext )
        {
        if ( iName )
            {
            delete iName;
            }
        iName = (aContext.iName)->Alloc();
        iContextId = aContext.iContextId;
        iGlobal = aContext.iGlobal;
        iGrammarId = aContext.iGrammarId;
        iLexiconId = aContext.iLexiconId;
        iModelBankId = aContext.iModelBankId;
        iContextSrsPortal = aContext.iContextSrsPortal;
        iModelBankAndLexiconExist = aContext.iModelBankAndLexiconExist;
        iRecognitionMode = aContext.iRecognitionMode;
        }
    return *this;
    }

// ---------------------------------------------------------
// CNssContext::CopyL
// for internal use
// creates a copy of this context
// ---------------------------------------------------------
//
CNssContext* CNssContext::CopyL()
    {
    // Takes ages in strain test
    RUBY_DEBUG0( "CNssContext::CopyL" );
    
    CNssContext* copy = new (ELeave) CNssContext( iContextSrsPortal );
    copy->iName = iName->Alloc();
    copy->iContextId = iContextId;
    copy->iGlobal = iGlobal;
    copy->iGrammarId = iGrammarId;
    copy->iLexiconId = iLexiconId;
    copy->iModelBankId = iModelBankId;
    copy->iContextSrsPortal = iContextSrsPortal;
    copy->iModelBankAndLexiconExist = iModelBankAndLexiconExist;
    copy->iClientData.Copy( iClientData );
    
    return copy;
    }

// ---------------------------------------------------------
// CNssContext::SetClientData
// Sets/Resets the client data. The data must be saved
// to VAS DBwith CNssContextMgr::SaveClientData.
// ---------------------------------------------------------
//
void CNssContext::SetClientData(const TDesC8& aData)
    {
    iClientData.Copy( aData );
    }

// ---------------------------------------------------------
// CNssContext::ClientData
// Gets the client data.
// ---------------------------------------------------------
//
const TDesC8& CNssContext::ClientData(void)
    {
    return iClientData;
    }

//  End of File  
