/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssTagMgr implements the MNssTagMgr interface. In addition, it also provides
*               methods for internal use by VAS components.
*
*/


#ifndef NSSVASCTAGMGR_H
#define NSSVASCTAGMGR_H

#include <e32std.h>
#include <e32base.h>
#include "nssvasmtagmgr.h"
#include "nssvasmgettagclient.h"
#include "nssvasmdeletetagclient.h"
#include "nssvasmsavetagclient.h"
#include "nssvasmintercomptagmgrclient.h"
#include "nssvasmcoresrsdbeventhandler.h"
#include "nssvasmvasdatabaseclient.h"
#include "nssvascoreconstant.h"
#include "nssvasmtagreference.h"
#include "nssvasmgettagreferenceclient.h"
#include "nssvasmcoresyncrecoveryhandler.h"
#include "nssvascvasdbdataexchanger.h"

// FORWARD DECLARATION
class CNssSpeechItemBuilder; 
class CNssContextMgr;
class CNssTag;
class CNssVASDatabase;

// CLASS DECLARATIONS
/**
*  CNssTagMgr implements the MNssTagMgr interface for tag management services.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssTagMgr:
    public CActive,
    public MNssTagMgr, 
    public MNssCoreSrsDBEventHandler, 
    public MNssVASDatabaseClient
{
public:

   /**
   * destructor
   */     
   ~CNssTagMgr();
 
   /**
   * 2 phase constructor
   * @param aVasDatabase - pointer to the VAS Database
   * @param aContextManager - pointer to the context manager
   * @return pointer the created tag manager
   */     
   static CNssTagMgr* NewL(CNssVASDatabase* aVasDatabase, CNssContextMgr *aContextManager);

   /**
   * 2 phase constructor
   * @param aVasDatabase - pointer to the VAS Database
   * @param aContextManager - pointer to the context manager
   * @return pointer the created tag manager
   */     
   static CNssTagMgr* NewLC(CNssVASDatabase* aVasDatabase, CNssContextMgr *aContextManager);

   /**
   * Creates a new tag
   * @since 2.0
   * @param aContext - pointer to context 
   * @return newly created tag
   */   
   MNssTag* CreateTagL(MNssContext* aContext);
   
   /**
   * gets tag list, implementation of interface MNssTagMgr
   * @since 2.0
   * @param aTagClient - call back address
   * @param aContext - context for which to get tags
   * @return synchronous error codes
   */   
   TInt GetTagList(MNssGetTagClient* aTagClient,MNssContext* aContext);
   
   /**
   * gets tag list, implementation of interface MNssTagMgr
   * @since 2.0
   * @param aTagClient - call back address
   * @param aName - name for which to get tags
   * @return synchronous error codes
   */   
   TInt GetTagList(MNssGetTagClient* aTagClient, MNssContext* aContext, const TDesC& aName);
   
  
   /**
   * delete tag, implementation of interface MNssTagMgr
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTag - tag to delete
   * @return synchronous error codes
   */   
   TInt DeleteTag(MNssDeleteTagClient* aTagClient,MNssTag* aTag);

   /**
   * save tag, implementation of interface MNssTagMgr
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTag - tag to save
   * @return synchronous error codes
   */   
   TInt SaveTag(MNssSaveTagClient* aTagClient,MNssTag* aTag);
   
   /**
   * get a tag based on grammar id and rule id.
   * for use by VAS components, call back returns a list with one element.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aGrammarId - grammar id
   * @param aRuleId - rule id
   * @return synchronous error codes
   */ 
   CNssTag* GetTag( TUint32 aGrammarId, TUint32 aRuleId );
   
   /**
   * get tags based on their grammar id and rule id.
   * for use by VAS components, call back returns a list with one element.
   * @param aGrammarIdRuleIds array of grammarid-ruleid pairs
   * return array list. It is to be destroyed by the client
   */ 
   MNssTagListArray* GetTags( TNssGrammarIdRuleIdListArray&  aGrammarIdRuleIds);
   

   /**
   * call back implementation from MNssVASDatabaseClient
   * @since 2.0
   * @param aContextList - list of contexts (not used in tag manager)
   * @param aTagList - list of tags
   * @param aCode - error code if failed
   * @return
   */     
   void VASDatabaseComplete( CArrayPtrFlat<CNssContext>* aContextList, 
                             CArrayPtrFlat<CNssTag>* aTagList,
	                         CArrayPtrFlat<CNssTagReference>* aTagRefList,
	                         MNssVASDatabaseClient::TNssVASDBClientReturnCode aCode);


   /**
   * call back implementation from MNssCoreSrsDBEventHandler
   * @since 2.0
   * @param
   * @return
   */     
   void HandleSaveSrsDBCompleted();

   /**
   * call back implementation from MNssCoreSrsDBEventHandler
   * @since 2.0
   * @param 
   * @return
   */     
   void HandleDeleteSrsDBCompleted();

   /**
   * Call back function from SRS DB, through interface MNssCoreSrsDBEventHandler
   * Called after deleting from SRS DB is successful
   * @since 2.8
   * @param
   * @return 
   */     
   void HandleResetSrsDBCompleted();

   /**
   * call back implementation from MNssCoreSrsDBEventHandler
   * @since 2.0
   * @param 
   * @return
   */     
   void HandleSrsDBError(MNssCoreSrsDBEventHandler::TNssSrsDBResult aResult);

   /**
   * utility function for CNssTag List to MNssTag List conversion, and call back to client
   * @since 2.0
   * @param aSourceList - CNssTag List
   * @return
   */     
   void CNssTagListToMNssTagListConvertorL(CArrayPtrFlat<CNssTag>* aSourceList);

   /**
   * Cancel get tag, for use by inter component clients only, not provided for 
   * external clients.
   * @since 2.0
   * @param none
   * @return none
   */     
   void CancelGetTag();

   /**
   * gets a tagCount, implementation of interface MNssTagMgr
   * @since 2.0
   * @param aContext - context for which to get tag count.
   * @return synchronous error codes or tagCount
   */   
   TInt TagCount(MNssContext* aContext);

   /**
   * get the tag(s) based on partial rrd int data and position
   * @since 2.0
   * @param aTInt - the rrd int value.
   * @param aPosition - the position of the int value in rrd int array
   * @return synchronous error codes or tagCount
   */ 
   
   TInt GetTagList(MNssGetTagClient* aTagClient,MNssContext* aContext,TInt aNum,TInt aPosition);
   /**
   * get the tag(s) based on partial rrd text data and position
   * @since 2.0
   * @param aTInt - the rrd text value.
   * @param aPosition - the position of the int value in rrd int array
   * @return synchronous error codes or tagCount
   */      
   
   TInt GetTagList(MNssGetTagClient* aTagClient,MNssContext* aContext,TDesC& aText,TInt aPosition);
   /**
   * Get a list of tag references based on context. 
   * @since 2.0
   * @param aTagReferenceClient - call back address
   * @param aContext - a context
   * @return asynchronous return error code
   */
   TInt GetTagReferenceList(MNssGetTagReferenceClient* aTagRefClient,MNssContext* aContext);
   /**
   * Get a tag based on tagreference. 
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTagReference - a tagreferenc
   * @return asynchronous return error code
   */
   TInt GetTag(MNssGetTagClient* aTagClient,MNssTagReference* aTagReference);

   void CNssTagRefListToMNssTagRefListConvertorL(CArrayPtrFlat<CNssTagReference>* aSourceList);

   /********************** ACTIVE OBJECT *******************/
    TInt RunError( TInt aError );
    void RunL();
    void DoCancel();

private:

   enum TCallBackState
   {
	   ENone,
	   EGetTagClient,
	   EDeleteTagClient,
	   ESaveTagClient,
	   EInterCompTagMgrClient,
	   EGetTagReferenceClient
	};

   /**
   * C++ constructor
   * @param 
   * @return 
   */  
   CNssTagMgr();
   
   /**
   * C++ constructor
   * @param aVasDatabase - pointer to the VAS Database
   * @param aContextManager - pointer to the context manager
   * @return 
   */  
   CNssTagMgr(CNssVASDatabase* aVasDatabase, CNssContextMgr *aContextManager);

   /**
   * EPOC constructor
   */     
   void ConstructL();


private:

	// pointer to the VAS Database
	CNssVASDatabase*  iVasDatabase;

	// pointer to the speech item builder, tag manager owns this builder
	CNssSpeechItemBuilder* iSpeechItemBuilder;

	// pointer to a context manager, association
	CNssContextMgr*	iContextManager;

	// state of the tag manager
	TCallBackState iState;

	// get tag client call back address
	MNssGetTagClient* iGetTagClient;

	// delete tag client call back address
	MNssDeleteTagClient* iDeleteTagClient;

	// save tag clienat call back address
	MNssSaveTagClient* iSaveTagClient;

	// internal VAS components as clients, call back address
	MNssInterCompTagMgrClient* iInterCompTagMgrClient;
	
	// local pointer to tag, for error handling
	CNssTag*			iLocalTag; 

	// local pointer to speech item, for error handling
	CNssSpeechItem*	iLocalSpeechItem;
	
	// for error handling
	TBool			iRecovery;
    
	// get tag reference client call back address
	MNssGetTagReferenceClient*  iGetTagReferenceClient;

    // Tag array for GetTag[List]
    CArrayPtrFlat<MNssTag>* iTagList;

    // Tag array for GetTagReferenceList
    CArrayPtrFlat<MNssTagReference>* iTagRefList;
};

#endif // NSSVASCTAGMGR_H

// End of file
