/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssVasDatabase is responsible for the issuing of requests  to
*               to the VasDb server and returning the results to the VasDb Managers.
*
*/


#ifndef CNSSVASDATABASE_H
#define CNSSVASDATABASE_H


#include <e32base.h>
#include <e32cons.h>
#include <s32file.h>
#include <d32dbms.h>
#include "nssvasrvasdbsession.h" 
#include "nssvasmgetcontextclient.h"
#include "nssvasmgettagclient.h"
#include "nssvasccontext.h"
#include "nssvasctag.h"
#include "nssvascrrd.h"
#include "nssvascvasdbdataexchanger.h"
#include "nssvasmvasdatabaseclient.h"
#include "nssvascspeechitembuilder.h"
#include "nssvasccontextbuilder.h"
#include "nssvascvasdbeventmonitor.h"
#include "nssvasctagreference.h"
#include "nssvasmcoresyncrecoveryhandler.h"

// CLASS DECLARATIONS
/**
*
*  The CNssVasDatabase class definition. 
* 
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssVASDatabase: public CBase 
    {
public:

     /**
     * Two-phased constructor.
    */
   	static CNssVASDatabase* NewL(TInt aPriority);
    /**
    * Two-phased constructor.
    */
	static CNssVASDatabase* NewLC(TInt aPriority);
	/**
	* Destructor
	*/
	~CNssVASDatabase();
	// VAS Db
	/**
    * Method to create vas database
    * @since 2.0
    * @param 
    * @return TInt a success or failure code.
    */
	TInt CreateDb();
	/**
    * Method to open vas database
    * @since 2.0
    * @param 
    * @return TInt a success or failure code.
    */
	TInt OpenDatabase();
	/**
    * Method to close vas database.
    * @since 2.0
    * @param 
    * @return void
    */
    void CloseDatabase();

    // ContextMgr Requests
	/**
    * Method to determine if modelbank and lexicon ids exist.
    * @since 2.0
    * @param aModelBankId- The modelbank id
	* @param aLexiconId - The lexicon id
    * @return TBool ETrue or EFalse.
    */
	TBool ModelBankAndLexiconExist(TUint32 &aModelBankId, TUint32 &aLexiconId);
	/**
    * Method to save context.
    * @since 2.0
    * @param aContext - .
    * @return TInt success or failure code.
    */
    TInt SaveContext( CNssContext* aContext, TInt& aNewContextId );

	/**
    * Updates the client data of the context
    * @since 2.8
    * @param aContext - .
    * @return TInt success or failure code.
    */
    TInt SaveContextClientData(CNssContext* aContext);

	/**
    * Method to get context by name
    * @since 2.0
    * @param aName -  The name of the context.
    * @return CArrayPtrFlat<CNssContext>* List of contexts.
    */
	CArrayPtrFlat<CNssContext>* GetContext(const TDesC& aName);

	/**
    * Method to get all global contexts.
    * @since 2.0
    * @return CArrayPtrFlat<CNssContext>* List of contexts.
    */
	CArrayPtrFlat<CNssContext>* GetGlobalContexts();

	/**
    * Method to get all contexts.
    * @since 2.0
    * @return CArrayPtrFlat<CNssContext>* List of contexts.
    */
	CArrayPtrFlat<CNssContext>* GetAllContexts();

	/**
    * Method to transfer context list to client side.
    * @since 2.0
    * @param 
    * @return TInt success or failure code.
    */
	TInt TransferContextList();

	/**
    * Method to delete context by name.
    * @since 2.0
    * @param aName- The name of the context to delete.
    * @return TInt success or failure code.
    */
	TInt DeleteContext( const TDesC& aName );

    /**
    * Method to delete context.
    * @since 2.0
    * @param aContext-  The context to delete.
    * @return TInt success or failure code.
    */
	TInt DeleteContext( CNssContext* aContext );

	// TagMgr Requests	
	/**
    * Method to save a tag.
    * @since 2.0
    * @param aTag The voice tag to save.
    * @return TInt sucess of failure code.
    */
	TInt SaveTag( CNssTag* aTag, TInt& aNewId );

    /**
    * Method to save several tags.
    * @since 2.8
    * @param aTagArray The voice tags to save.
    * @return TInt success or failure code.
    */
    TInt SaveTags( RPointerArray<CNssTag>* aTagArray );

    /**
    * Method to delete several tags.
    * @since 2.8
    * @param aTagArray The voice tags to delete.
    * @return TInt success or failure code.
    */
    TInt DeleteTags( const RArray<TUint32>& aTagIdArray );

    /**
    * Method to get a tag by name.
    * @since 2.0
    * @param aName- The name of the voice tag.
    * @return TInt success or failure code.
    */
	MNssTagListArray* GetTag( const TDesC& aName );

    /**
    * Initiate the Select Tag event.
    * @since 2.0
    * @param aTag The voice tag which was selected.
    * @return TNssRecognitionResult Return indicating request is valid.
    */
	MNssTagListArray* GetTag( CNssContext* aContext );
    /**
    * Method to get tag by grammar id and rule id.
    * @since 2.0
    * @param aGrammarId - The grammar id of the voice tag.
	* @param aRuleId - The rule id of the voice tag.
    * @return TInt success or failure code.
    */	
	MNssTagListArray* GetTag( const TInt aGrammarID, const TInt aRuleID );
	
	/**
    * Method to tags by theirgrammar id and rule id.
    * @param aGrammarId - The grammar id of the voice tag.
	* @param aRuleId - The rule id of the voice tag.
    * @return TInt success or failure code.
    */	
	MNssTagListArray* GetTags( TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds);
	
    /**
    * Method to get a tag by context and name.
    * @since 2.0
    * @param aContext - The context of the tag.
	* @param aName - The name of the tag.
    * @return TInt success or failure code.
    */	
	MNssTagListArray* GetTag( CNssContext* aContext, const TDesC& aName );

    /**
    * Method to get a tag by tagid.
    * @since 2.0
    * @param aTagId - The tagid of the voice tag.
    * @return TInt success or failure code.
    */
	MNssTagListArray* GetTag( const TUint32 aTagId );

   	/**
    * Method to transfer tag list to client side.
    * @since 2.0
    * @param 
    * @return TInt success or failure code.
    */
	TInt TransferTagList();

    /**
    * Method to delete tag by name.
    * @since 2.0
    * @param aName- The name of the voice tag to delete.
    * @return TInt success or failure code.
    */
	TInt DeleteTag( const TDesC& aName );
        /**
    * Initiate the Select Tag event.
    * @since 2.0
    * @param aTag The voice tag which was selected.
     * @return TInt success or failure code.
    */
	TInt DeleteTag( CNssTag *aTag );
	/**
    * Method to delete a tag by id
    * @since 2.0
    * @param aTagId- the tagid.
    * @return TInt success or failure code.
    */
	TInt DeleteTag( const TUint32 aTagId );

	// Event Monitoring methods
    /**
    * Method use to start monitoring vas database.
    * @since 2.0
    * @param 
    * @return void
    */
	void StartMonitoringDatabaseL();
	/**
    * Method use to add observers to vas database. 
    * @since 2.0
    * @param aObserver- An observer of vas database.
    * @return none
    */
	void AddObserverL( MNssVASDatabaseObserver* aObserver );
	/**
    * Method used to stop observering vas database.
    * @since 2.0
    * @param aObserver- The observer to remove.
    * @return TInt success or failure code.
    */
	TInt RemoveObserver( MNssVASDatabaseObserver* aObserver );

	/**
	* Method to determine if tags exist for a context
	* @since 2.0
	* @param aContext - The context to check tags for.
	* @return TBool ETrue or EFalse
	*/
    TBool TagExist( CNssContext* aContext );

    /**
	* Method to get the number of tags for a context
	* @since 2.0
	* @param aContext - The context to check tags for.
	* @return TInt
	*/
    TInt TagCount( CNssContext* aContext );

    /**
	* Method to get tags based on a rrd int value and position
	* @since 2.0
	* @param aNum - The rrd int value
	* @param aPosition - The int value position in the rrd intarray
	*/
	MNssTagListArray* GetTagList( CNssContext* aContext, TInt aNum, TInt aPosition );

	/**
	* Method to get tags based on a rrd text value and position
	* @since 2.0
	* @param aText - The rrd text value
	* @param aPosition - The text value position in the rrd textarray
	*/
    MNssTagListArray* GetTagList( CNssContext* aContext, TDesC& aText, TInt aPosition );

	CArrayPtrFlat<MNssTagReference>* GetTagReferenceList( CNssContext* aContext );

	TInt TransferTagReferenceList();

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

    /**
    * Changes the Rule ID of some tags.
    * When an already trained tag is retrained, the Rule ID changes.
    * This function updates the change to VAS DB.
    * @since 2.8
    * @param aNewId new model bank id
    * @return symbian-wide error code
    */
    TInt UpdateTagRuleIDs( const RArray<TNssSpeechItem>& aRetrainedTags );

private:

	// C++ constructor
    CNssVASDatabase(TInt aPriority);
    // 2nd phase construction.
    void ConstructL ();
    
    void ConvertTNssTagRefToMNssTagRefL( TNssTagReferenceListArray* aTNssTagRef,
                                         CArrayPtrFlat<MNssTagReference>* aCNssTagRef);
    MNssVASDatabaseClient::TNssVASDBClientReturnCode  ConvertDBError(TInt aError);


    // Pointer to the object, which makes the actual operations on database.
    CNssVasDb* iVasDb;

    // How many VAS instances in this thread have connected.
    TInt iConnected;

    //Current state
    TInt				         iState;
    //Grammar and rule data passed to server
    TNssGrammarIdRuleId          iTNssGrammarIdRuleId;
    //Package for grammar and rule data
    TPckgCTNssGrammarIdRuleId*   iGrammarIdRuleIdPackage;
    //Taglist returned to client
    CArrayPtrFlat<CNssTag>*      iCNssTagList;
    //Contextlist returned to client
    CArrayPtrFlat<CNssContext>*  iCNssContextList;
    //Tag id
    TUint32                   iTagId;
    //Contextbuilder used to create contexts
    CNssContextBuilder*          iContextBuilder;
    //SpeechItembuilder used to create speechitems
    CNssSpeechItemBuilder*       iSpeechItemBuilder;
    //RRD int array data
    //TNssRRDIntArray*             iRRDIntArray;
    //RRD text array data
    //TNssRRDTextArray*            iRRDTextArray;
    //EventMonitor for vas db
    CNssVASDBEventMonitor*		 iVASDBEventMonitor;

    TNssTagReferenceListArray*       iTNssTagRefList;
    CArrayPtrFlat<CNssTagReference>* iCNssTagRefList;

    enum TState  //events
        {
        EStateGetContext = 0,
	    EStateTransferContext,
	    EStateGetTag,
	    EStateTransferTag,
	    EStateDeleteContext,
	    EStateDeleteTag,
	    EStateSaveContext,
	    EStateSaveTag,
	    EStateGetTagReference,
	    EStateTransferTagReference
        };

    // Flag: Is the database locked
    TBool iIsLocked;
    };

#endif
