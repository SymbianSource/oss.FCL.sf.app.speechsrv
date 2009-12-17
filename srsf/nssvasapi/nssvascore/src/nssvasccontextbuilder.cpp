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
* Description:  CNssContextBuilder is the builder for context objects. It is owned 
*               by the context manager. A client builds contexts by requesting the
*               context manager which in turn forwards the request to context builder.
*
*/


// INCLUDE FILES
#include "nssvasccontextbuilder.h"
#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssContextBuilder::CNssContextBuilder
// C++ constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
// 
CNssContextBuilder::CNssContextBuilder()
    {
    // Nothing
    }

// ---------------------------------------------------------
// CNssContextBuilder::NewL
// Two-phased constructor.
// ---------------------------------------------------------
// 
CNssContextBuilder* CNssContextBuilder::NewL()
    {
    // Takes ages in strain test
    // RUBY_DEBUG0( "CNssContextBuilder::NewL" );

    CNssContextBuilder* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CNssContextBuilder::NewLC
// Two-phased constructor.
// ---------------------------------------------------------
// 
CNssContextBuilder* CNssContextBuilder::NewLC()
    {
    CNssContextBuilder* self = new (ELeave) CNssContextBuilder;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CNssContextBuilder::~CNssContextBuilder
// Destructor
// ---------------------------------------------------------
//    
CNssContextBuilder::~CNssContextBuilder()
    {
    RUBY_DEBUG0( "CNssContextBuilder::~CNssContextBuilder" );

	// do not delete the portal if ContextCount() > 0 
	if ( iContextSrsPortal )
	    {
    	iContextSrsPortal->SetTerminationState( CNssContextSrsPortal::EVASTERMINATION_PENDING );
    	if ( iContextSrsPortal->ContextCount() == 0 )
	    	{
			delete iContextSrsPortal;
		    }    	
	    }
    }

// ---------------------------------------------------------
// CNssContextBuilder::ConstructL
// creates a context srs portal
// ---------------------------------------------------------
//
void CNssContextBuilder::ConstructL()
    {
    iContextSrsPortal = new (ELeave) CNssContextSrsPortal();
    }

// ---------------------------------------------------------
// CNssContextBuilder::CreateContextL
// creates a context given all data member values.
// ---------------------------------------------------------
//
CNssContext* CNssContextBuilder::CreateContextL( const TDesC& aName, 
                                                 TInt aContextId, 
                                                 TBool aGlobal, 
                                                 TUint32 aGrammarId, 
                                                 TUint32 aLexiconId, 
                                                 TUint32 aModelBankId, 
                                                 TUint32 /*iTrainType*/, 
                                                 const TDesC8& aClientData )
    {
    // Takes ages in strain test
    // RUBY_DEBUG_BLOCK( "CNssContextBuilder::CreateContextL" );

	CNssContext *context = new (ELeave) CNssContext( iContextSrsPortal );
	CleanupStack::PushL(context);
	context->SetNameL( aName );
	context->SetContextId( aContextId );
	context->SetGlobal( aGlobal );
	context->SetGrammarId( aGrammarId );
	context->SetLexiconId( aLexiconId );
	context->SetModelBankId( aModelBankId );
    context->SetClientData( aClientData );
	CleanupStack::Pop(context);
	return context;
    }

// ---------------------------------------------------------
// CNssContextBuilder::CreateContextL
// creates a context given lexicon id and model bank id
// ---------------------------------------------------------
//
CNssContext* CNssContextBuilder::CreateContextL( TUint32 aLexiconId, 
                                                 TUint32 aModelBankId )
    {
    RUBY_DEBUG_BLOCK( "CNssContextBuilder::CreateContextL() - w/ lex id & model bank id" );

	CNssContext *context = new (ELeave) CNssContext( iContextSrsPortal );
	context->SetLexiconId( aLexiconId );
	context->SetModelBankId( aModelBankId );
	return context;
    }

// ---------------------------------------------------------
// CNssContextBuilder::CreateContextL
// creates an empty context object
// ---------------------------------------------------------
//
CNssContext* CNssContextBuilder::CreateContextL()
    {
    RUBY_DEBUG_BLOCK( "CNssContextBuilder::CreateContextL() - empty" );

    CNssContext *context = new (ELeave) CNssContext( iContextSrsPortal );
	return context;
    }

// ---------------------------------------------------------
// CNssContextBuilder::GetContextPortal
// returns the context SRS portal
// ---------------------------------------------------------
//
CNssContextSrsPortal* CNssContextBuilder::GetContextPortal()
    {
	return iContextSrsPortal;
    }

//  End of File  
