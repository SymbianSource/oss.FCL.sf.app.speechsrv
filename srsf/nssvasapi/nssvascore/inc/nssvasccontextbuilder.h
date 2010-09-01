/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef NSSVASCCONTEXTBUILDER_H
#define NSSVASCCONTEXTBUILDER_H

//  INCLUDES
#include "nssvasccontext.h"
#include "nssvasccontextsrsportal.h" 

// CLASS DECLARATION

/**
*  CNssContextBuilder is a builder class for creating context objects. 
*  It has overloaded variants for the CreateContext(). A client will not create 
*  a context object directly but will request it through the context manager, 
*  which in turn calls the CreateContext() in context builder. 
*  @lib NssVASApi.lib
*  @since 2.8
*/

class CNssContextBuilder: public CBase
{
public:

     /**
    * destructor
    * @param 
    * @return 
    */
	~CNssContextBuilder();

    /**
    * 2 phase construction
    * @param 
    * @return pointer to context builder
    */
	static CNssContextBuilder* NewL();

    /**
    * 2 phase construction
    * @param 
    * @return pointer to context builder 
    */
	static CNssContextBuilder* NewLC();

public: 

    /**
    * create a context given the name, context id, global flag, grammar id, 
	* lexicon id, and modelbank id. Used by VAS internal components
	* @since 2.0
    * @param aName Name for the context
	* @param aContextId Context Id for the context
	* @param aGlobal Context is global or not
	* @param aGrammarId Grammar id for the context
	* @param aLexiconId Lexicon id for the context 
	* @param aModelBankId Model bank id for the context
    * @return 
    */
	CNssContext* CreateContextL(const TDesC& aName, TInt aContextId, TBool aGlobal, 
	                TUint32 aGrammarId, TUint32 aLexiconId, TUint32 aModelBankId, 
	                TUint32 iTrainType, const TDesC8& aClientData );

    /**
    * create a context given the lexicon id and modelbank id, other members of the 
	* context are initialized to 0. Used by VAS internal components
	* @since 2.0
    * @param aLexiconId lexicon id
	* @param aModelBankId model bank id
    * @return 
    */
	CNssContext* CreateContextL(TUint32 aLexiconId, TUint32 aModelBankId);

    /**
    * create an empty context
	* @since 2.0
    * @param 
    * @return context id
    */
	CNssContext* CreateContextL();

    /**
    * get context SRS portal
    * @since 2.8
    * @param
    * @return CNssContextSrsPortal
    */
    CNssContextSrsPortal* GetContextPortal();

private:

	/**
    * constructor
    * @param 
    * @return 
    */
	CNssContextBuilder();

    /**
    * 2 phase construction
    * @param 
    * @return context id
    */
	void ConstructL();

private:
	
	// a srs utility portal
	CNssContextSrsPortal* iContextSrsPortal;
};


#endif // NSSVASCCONTEXTBUILDER_H
            
// End of File
