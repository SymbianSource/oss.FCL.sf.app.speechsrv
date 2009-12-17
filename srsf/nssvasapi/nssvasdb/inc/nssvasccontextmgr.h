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
* Description:  CContextManager provides context processing for the client. CContextManager
*               implements the MNssContextMgr interface, and also provides additional methods for
*               use by internal VAS components. 
*
*/


#ifndef NSSVASCCONTEXTMGR_H
#define NSSVASCCONTEXTMGR_H

// INCLUDES
//
#include <e32std.h>
#include <e32base.h>

#include "nssvasmcontextmgr.h"
#include "nssvasmcontext.h"
#include "nssvasmgetcontextclient.h"
#include "nssvasmdeletecontextclient.h"
#include "nssvasmsavecontextclient.h"
#include "nssvasmintercompcontextmgrclient.h"
#include "nssvasmvasdatabaseclient.h"
#include "nssvasmcoresrsdbeventhandler.h"
#include "nssvascoreconstant.h"

// FORWARD DECLARATIONS
class CNssContextBuilder;
class CNssVASDatabase;
class MNssResetFactoryModelsClient;

// CLASS DECLARATIONS
/**
*  CContextMgr is the implementation class for MNssContextMgr interface.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssContextMgr :
    public CActive,
    public MNssContextMgr,
    public MNssVASDatabaseClient,
    public MNssCoreSrsDBEventHandler
    {
public:

    enum TNssCallBackState
        {
	    ENone,
	    EGetContextClientGetContext,
	    EGetContextClientGetContextList,
	    EDeleteContextClient,
	    ESaveContextClient,
        ESaveClientDataClient,
	    EInterCompContextMgrClientGetContext,
	    EInterCompContextMgrClientGetContextList,
	    EInterCompContextMgrClientGetGlobalContexts
	    };

   /**
   * 
   * Destructor.
   */
   ~CNssContextMgr();

   /**
   * 2 Phase construction
   * @param aVASDatabase - Reference to the database.
   * @return void
   */
   static CNssContextMgr* NewL(CNssVASDatabase* aVASDatabase);

   /**
   * 2 Phase construction
   * @param aVASDatabase - Reference to the database.
   * @return void
   */
   static CNssContextMgr* NewLC(CNssVASDatabase* aVASDatabase);


   /**
   * Creates an empty context object
   * @since 2.8
   * @param void
   * @return A reference to the context object
   */
   MNssContext* CreateContextL();


   /**
   * GetContext is called to retrieve a context from the VAS DB. 
   * This method is asynchronous. A call back will be made to 
   * either GetContextCompleted() or GetContextFailed() as
   * defined in the MNssGetContextClient interface. It also returns an
   * error code validating success of call to VAS DB.
   * @since 2.8
   *
   * @param aContextClient - call back address
   * @param aName          - name of context to retreive
   * @return - syncronous error code from function return
   */    
   TInt GetContext(MNssGetContextClient* aContextClient,const TDesC& aName);

   /**
   * GetContextList is called to retrieve a context list from the VAS DB. 
   * This method is asynchronous. A call back will be made to 
   * either GetContextListCompleted() or GetContextFailed() as
   * defined in the MNssGetContextClient interface. It also returns an
   * error code validating success of call to VAS DB.
   * @since 2.8
   *
   * @param aContextClient - call back address 
   * @param  
   * @return - syncronous error code from function return
   */    
   TInt GetContextList(MNssGetContextClient* aContextClient);
   
   /**
   * DeleteContext is called to remove a context from the VAS DB. 
   * This method is asynchronous. A call back will be made to 
   * either DeleteContextCompleted() or DeleteContextFailed() as
   * defined in the MNssDeleteContextClient interface. It also returns an
   * error code validating success of call to VAS DB.
   * @since 2.8
   *
   * @param aContextClient - call back address
   * @param aContext       - context object to delete
   * @return syncronous error code from function return
   */     
   TInt DeleteContext(MNssDeleteContextClient* aContextClient, MNssContext* aContext);
   
   /**
   * SaveContext is called to store a context to the VAS DB. 
   * This method is asynchronous. A call back will be made to 
   * either SaveContextCompleted() or SaveContextFailed() as
   * defined in the MNssSaveContextClient interface. It also returns an
   * error code validating success of call to VAS DB.
   * @since 2.8
   *
   * @param aContextClient - call back address
   * @param aContext       - context object to save
   * @return - syncronous error code from function return
   */ 
   TInt SaveContext( MNssSaveContextClient* aContextClient, MNssContext* aContext);

   /**
   * SaveClientData is called to save client-specific data about the context.
   * This method is asynchronous. A call back will be made to either
   * SaveContextCompleted or SaveContextFailed() as defined in the
   * MNssSaveContextClient interface. It also returns an error code validating
   * success of call to VAS DB.
   *
   * @from VasMNssContextMgr
   * @since 2.8
   * @param aContext       - the client data of this context will be updated.
   */
   TInt SaveClientData( MNssContext* aContext );

   /**
   * GetGlobalContexts is called to get the list of global contexts in VAS DB. 
   * This method is synchronous (It used to be asynchronous before 2.8. A call
   * back was made to  either GetGlobalContextsCompleted() or GetContextFailed()
   * as defined in the MNssInterCompContextMgrClient interface. It also returned an
   * error code validating success of call to VAS DB). This method is for use by
   * internal VAS components.
   * @since 2.8
   *
   * @param aContextClient - call back address
   * @return - syncronous error code from function return
   */
   TMNssContextList* GetGlobalContexts();

   /**
   * Call back function from CVASDatabase, through interface MNssVASDatabaseClient
   * @since 2.8
   * @param aContextList - list of contexts
   * @param aTagList - list of tags (not used by context manager)
   * @param aTagRefList - list of tag references
   * @param aCode - success status
   * @return 
   */     
   void VASDatabaseComplete( CArrayPtrFlat<CNssContext>* aContextList, 
                             CArrayPtrFlat<CNssTag>* aTagList,
	                         CArrayPtrFlat<CNssTagReference>* aTagRefList,
	                         TNssVASDBClientReturnCode aCode);


   /**
   * Call back function from SRS DB, through interface MNssCoreSrsDBEventHandler
   * Called after saving to SRS DB is successful
   * @since 2.8
   * @param
   * @return 
   */     
   void HandleSaveSrsDBCompleted();

   /**
   * Call back function from SRS DB, through interface MNssCoreSrsDBEventHandler
   * Called after deleting from SRS DB is successful
   * @since 2.8
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
   * Call back function from SRS DB, through interface MNssCoreSrsDBEventHandler
   * Called after saving or deleting to SRS DB is unsuccessful
   * @since 2.8
   * @param aResult result code 
   * @return 
   */     
   void HandleSrsDBError(MNssCoreSrsDBEventHandler::TNssSrsDBResult aResult);

   /**
   * Determine if tag exists in the VAS Database for a context
   * @since 2.8
   * @param context pointer
   * @return ETrue if a context exists, EFalse otherwise.
   */     
   TBool TagExist(CNssContext* aContext);

   /**
   * Cancel get context / get context list / get global contexts 
   * @since 2.8
   * @param none
   * @return none
   */     
   void CancelGetContext();

   /**
   * Deletes the speaker adapted models and creates new,
   * speaker independent ones. Asynchronous.
   * @since 2.8
   * @param aClient Callback to signal success or failure
   * @return symbian-wide error code
   */
   TInt ResetFactoryModels( MNssResetFactoryModelsClient* aClient );

   /**
   * Error handler if RunL leaves.
   * @from CActive
   * @since 2.8
   * @param aClient Callback to signal success or failure
   * @return none
   */
   TInt RunError(TInt aError);

   /**
   * Look at CActive documentation for exaplanation.
   * @from CActive
   * @since 2.8
   * @param aClient Callback to signal success or failure
   * @return none
   */
   void RunL();

   /**
   * Look at CActive documentation for exaplanation.
   * @from CActive
   * @since 2.8
   * @param none
   * @return none
   */
   void DoCancel();

private:
	
   /**
   * C++ Constructor.
   */
   CNssContextMgr();

   /**
   * C++ overloaded Constructor.
   */
   CNssContextMgr(CNssVASDatabase* aVASDatabase);

   /**
   * Symbian constructor
   */     
   void ConstructL();

   /**
   * Utility function for CContext to MNssContext List conversion, and call back to client
   * @param aSourceList a CContext list
   * @return 
   */  
   void CNssContextListToMNssContextListConvertorL(CArrayPtrFlat<CNssContext>* aSourceList);

private:
    // VAS Database
    CNssVASDatabase* iVasDatabase;

    // context builder
    CNssContextBuilder* iContextBuilder;

    // internal state
    TNssCallBackState iState;

    // external get context client
    MNssGetContextClient* iGetContextClient;

    // external delete context client
    MNssDeleteContextClient* iDeleteContextClient;

    // external save context client
    MNssSaveContextClient* iSaveContextClient;

    // internal inter-component client
    MNssInterCompContextMgrClient* iInterCompContextMgrClient;

    // flag to denote if model bank and lexicon exist or not for
    // VAS has a single model bank and lexicon
    TBool iModelBankAndLexiconExist;

    // model bank id, if model bank exists; else its not a valid id
    TSIModelBankID iModelBankId;

    // lexicon id, if lexicon exists; else its not a valid id
    TUint32 iLexiconId;

    // local pointer to context, used to handle error conditions
    CNssContext* iLocalContext; 

    // flag used to handle error conditions
    TBool iRecovery; 

    // For GetContext: Stores the context list
    CArrayPtrFlat<CNssContext> *iContextList;

    // Callback for ResetFactoryModels
    MNssResetFactoryModelsClient* iResetClient;
    };

#endif // NSSVASCCONTEXTMGR_H

// End of file
