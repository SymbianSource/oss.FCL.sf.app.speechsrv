/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  RVasDbSession is the client-side(proxy) interface through which 
*               communication with the vasdb server is channelled.
*
*/

#ifndef RNSSVASDBSESSION_H
#define RNSSVASDBSESSION_H


#include "nssvascvasdbsrvdef.h"
#include "nssvasdbkonsts.h"
#include "nssvascvasdbdataexchanger.h"
#include "nssvasccontext.h"
#include "nssvasccontext.h"
#include "nssvasctag.h"
#include "nssvascvasdb.h"

// FORWARD DECLARATIONS
class CNssVasDb;

// CLASS DECLARATION

/**
*
*  The RVasDbSession class definition. 
* 
*  @lib NssVASApi.lib
*  @since 2.0
*/
class RNssVasDbSession
    {
	// Functions
public:

    /**
    * C++ 1st phase constructor
    */
	RNssVasDbSession();	

    /**
    * C++ destructor
    */
    ~RNssVasDbSession();
	
    /**
    * A method to create a new database. The name of the database file
    * is read from a resource file (NssVasResource.rsc).
    * @since 2.8
    * @return KErrNone or a Symbian-wide error code.
    */
	TInt  CreateDatabase();

    /**
    * A method to open the database. The name of the database file
    * is read from a resource file (NssVasResource.rsc).
    * @since 2.8
    * @return KErrNone or a Symbian-wide error code.
    */
	TInt  OpenDatabase();	

    /**
    * A method to close the database.
    * @since 2.8
    * @return KErrNone or a Symbian-wide error code.
    */
	TInt  CloseDatabase ();

    /**
    * A method to get the model bank id and lexicon id.
    * @since 2.8
    * @param aModelBankId Model Bank ID will be returned here.
    * @param aLexiconId Lexicon ID will be returned here.
    * @return ETrue if success, otherwise EFalse.
    */
    TBool ModelBankIdLexiconId( TUint32 &iModelBankId, TUint32 &iLexiconId );

    /**
    * A method to check whether a context is empty or not.
    * @since 2.8
    * @param aContextId Unique identifier of the context.
    * @return ETrue if there are tags, EFalse if the context is empty.
    */
    TBool TagExist( TInt aContextId );

    /**
    * Method to return the number of tags in a context.
    * @since 2.8
    * @param aContextId Unique identifier of the context.
    * @return KErrNone or a Symbian-wide error code.
    */
    TInt  TagCount( TInt aContextId );
	
    /**
    * Method to save a context. When a context is saved, a context ID is 
    * assigned to it. The newly assigned ID is returned
    * in the reference parameter.
    * @since 2.8
    * @param aContext Data of the context.
    * @param aContextId Reference for returning the newly assigned context ID.
    * @return KErrNone or a Symbian-wide error code.
    */
    //TInt SaveContext( TNssContext& aContext, TInt& aContextId );
	TInt SaveContext( CNssContext& aContext, TInt& aContextId );
    
    /**
    * Method to delete a context from the database.
    * @since 2.8
    * @param aName Name of the context to be deleted. 
    * @return KErrNone or a Symbian-wide error code.
    */
    TInt DeleteContext( const TDesC& aName );

    /**
    * Method to update the data of a context.
    * @since 2.8
    * @param aContext the context in serialized form.
    * @return KErrNone or a Symbian-wide error code.
    */
	//TInt UpdateContext( TNssContext& aContext );
	TInt UpdateContext( CNssContext& aContext );

    /**
    * Method to update the client data of a context.
    * @since 2.8
    * @param aContext the context in serialized form.
    * @return KErrNone or a Symbian-wide error code.
    */
    //TInt UpdateContextClientData( TNssContext& aContext );
    TInt UpdateContextClientData( CNssContext& aContext );

    /**
    * Method to get a context by name.
    * @since 2.8
    * @param aName Name of the context
    * @return Array containing the context, or NULL if it is not found.
    */
    //TNssContextListArray* GetContextByName( const TDesC& aName );
    CArrayPtrFlat<CNssContext>* GetContextByName( const TDesC& aName );

    /**
    * Method to list all global contexts.
    * @since 2.8
    * @return Array containing all global contexts.
    */
    //TNssContextListArray* GetGlobalContexts();
    CArrayPtrFlat<CNssContext>* GetGlobalContexts();

    /**
    * Method to list all contexts.
    * @since 2.8
    * @return Array containing all contexts.
    */
    //TNssContextListArray* GetAllContexts();
    CArrayPtrFlat<CNssContext>* GetAllContexts();

    /**
    * Method to save a tag. During saving, a Tag ID is assigned for the tag.
    * This ID is returned in the refrence parameter.
    * @since 2.8
    * @param aTag Tag
    * @param aNewId This variable is used to return the newly asssigned
    *               Tag ID for the user.
    * @return KErrNone or a Symbian-wide error code.
    */
	//TInt SaveTag( TNssTag& aTag, TInt& aNewId );
	TInt SaveTag( CNssTag& aTag, TInt& aNewId );

    /**
    * Method to save a group of tags. During saving, a Tag IDs are assigned
    * for the tags. These IDs is returned in the aTagIdArray.
    * @since 2.8
    * @param aTagArray An array of previously unsaved tags.
    * @param aTagIdArray Empty array, which will be filled with
    *                    newly assigned Tag IDs.
    * @return KErrNone or a Symbian-wide error code.
    */
	//TInt SaveTags( CArrayFixFlat<TNssTag>* aTagArray, RArray<TInt>& aTagIdArray );
	TInt SaveTags( CArrayPtrFlat<CNssTag>* aTagArray, RArray<TInt>& aTagIdArray );

    /**
    * Method to delete a tag by name
    * @since 2.8
    * @param aTagName Trained text of the tag ( MNssSpeechItem::SetTextL() )
    * @return KErrNone or a Symbian-wide error code.
    */
    TInt DeleteTag( const TDesC& aTagName );

    /**
    * Method to delete a tag.
    * @since 2.8
    * @param aTagId Tag identifier.
    * @return KErrNone or a Symbian-wide error code.
    */
    TInt DeleteTag( TInt aTagId );

    /**
    * Method to delete a group of tags.
    * @since 2.8
    * @param aTagIdArray Array of tag identifiers.
    * @return KErrNone or a Symbian-wide error code.
    */
	TInt DeleteTags( const RArray<TUint32>& aTagIdArray );

    /**
    * Method to update the information of an existing tag.
    * @since 2.8
    * @param aTag New information about the tag
    * @return KErrNone or a Symbian-wide error code.
    */
	//TInt UpdateTag( const TNssTag& aTag );
	TInt UpdateTag( const CNssTag& aTag );

    /**
    * Method to get references to all tags in a context.
    * @since 2.8
    * @param aContextId Context identifier.
    * @return Array containing references to all tags.
    */
	TNssTagReferenceListArray* GetTagReferenceList( TInt aContextId );

    /**
    * Method to get all tags in a context.
    * @since 2.8
    * @param aContext The context
    * @return Array containing all tags in the context
    */
    //TNssTagListArray* GetTag( const TNssContext& aContext );
    CArrayPtrFlat<CNssTag>* GetTag( const CNssContext& aContext );

    /**
    * Method to get a tag by grammar ID and rule ID.
    * @since 2.8
    * @param aGrammarIdRuleId Structure containing the Grammar ID (which
    *                         specifies a context) and Rule ID (which
    *                         specifies a tag inside a context).
    * @return Array containing the matching tag or NULL.
    */
    //TNssTagListArray* GetTag( TNssGrammarIdRuleId aGrammarIdRuleId );
    CArrayPtrFlat<CNssTag>* GetTag( TNssGrammarIdRuleId aGrammarIdRuleId );
    
    /**
    * Method to get tags by their grammar ID and rule ID.
    * @since 2.8
    * @param aGrammarIdRuleIds Structure containing the Grammar IDs (that
    *                          specify a context) and Rule IDs (that
    *                          specify a tag inside a context).
    * @return Array containing the matching tags or NULL.
    */
    //TNssTagListArray* GetTags( TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds );
    CArrayPtrFlat<CNssTag>* GetTags( TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds );

    /**
    * Method to get a tag by name (CNssSpeechItem::SetText).
    * @since 2.8
    * @param aName Tag's trained text.
    * @return Array containing all matching tags.
    */
	//TNssTagListArray* GetTag( const TDesC& aName );
	CArrayPtrFlat<CNssTag>* GetTag( const TDesC& aName );

    /**
    * Method to get a tag by RRD data, given RRD integer and the position (0-4)
    * to which the integer is compared.
    * @since 2.8
    * @param aContextId ID of the context, to which the tag belongs.
    * @param aNum Integer, which should match the RRD integer.
    * @param aPosition RRD integer array position (0-4).
    * @return Array containing all matching tags.
    */
	//TNssTagListArray* GetTag( TInt aContextId, TInt aNum, TInt aPosition );
	CArrayPtrFlat<CNssTag>* GetTag( TInt aContextId, TInt aNum, TInt aPosition );

    /**
    * Method to get a tag by RRD data, given RRD text and the position (0-4)
    * to which the text is compared.
    * @since 2.8
    * @param aContextId ID of the context, to which the tag belongs.
    * @param aText Text, which should match the RRD text.
    * @param aPosition RRD text array position (0-4).
    * @return Array containing all matching tags.
    */
	//TNssTagListArray* GetTag( TInt aContextId, TDesC& aText, TInt aPosition );
	CArrayPtrFlat<CNssTag>* GetTag( TInt aContextId, 
	                                const TDesC& aText, TInt aPosition );

    /**
    * Method to get a tag, given the tag ID.
    * @since 2.8
    * @param aTagId The ID of the tag.
    * @return Array containing the tag or NULL.
    */
	//TNssTagListArray* GetTag( TUint32 aTagId );
	CArrayPtrFlat<CNssTag>* GetTag( TUint32 aTagId );

    /**
    * Method to update the Rule IDs of some tags.
    * @since 2.8
    * @param aRetrainedTags Array of tags with changed Rule IDs
    * @return symbian-wide error code
    */
    TInt UpdateTagRuleIDs( const RArray<TNssSpeechItem>& aRetrainedTags );

    /**
    * Method to get the default model bank id
    * @since 2.8
    * @param aId reference where to put the model bank Id
    * @return symbian-wide error code
    */
    TInt GetDefaultModelBankId( TUint32& aId );

    /**
    * Modifies the context table so that all contexts start using
    * the new id as their model bank id.
    * @since 2.8
    * @param aNewId new model bank id
    * @return symbian-wide error code
    */
    TInt ResetModelBank( TUint32 aNewId );

private:

    // Pointer to the object, which makes the actual operations on database.
    CNssVasDb* iVasDb;

    // How many VAS instances in this thread have connected.
    TInt iConnected;
    };

#endif
