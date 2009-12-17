/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MNssContextManager interface provides context processing methods for 
*				 the client. The client  gets an instance of the context manager 
*				 from the vas db manager. The vas db manager owns the context manager 
*				 and is responsible for deleting the context manager. Clients should not 
*				 directly delete the context manager. The context manager is simply a 
*				 colllection of utility methods  that are pure virtuals. These methods are implemented
*				 internal to VAS, and thus transparent to the client.
*
*/



#ifndef MNSSCONTEXTMGR_H
#define MNSSCONTEXTMGR_H

#include <e32std.h>

#include "nssvascoreconstant.h"

//FORWARD DECLARATIONS
class MNssGetContextClient;
class MNssDeleteContextClient;
class MNssSaveContextClient;
class MNssContext;



// CLASS DECLARATIONS
/**
*  MNssContextMgr is the public interface class for context management.
*  It provides methods for context creation, saving context into the
*  VAS database, retriving contexts from VAS database, and deletion of 
*  context from the VAS database. A context manager cannot be instantiated
*  directly but needs to be got from the VAS DB manager.
*  @lib NssVASApi.lib
*  @since 2.8
*  
*/
class MNssContextMgr
   {
public:
    /**
    * Create a new empty context
    * @since 2.0
    * @param 
    * @return a reference to a new context object.
    */
    virtual MNssContext* CreateContextL()=0;

    /**
    * GetContext is called to retrieve a context from the VAS DB. 
    * This method is asynchronous. A call back will be made to 
    * either GetContextCompleted() or GetContextFailed() as
    * defined in the MNssGetContextClient interface. It also returns an
    * error code validating success of call to VAS DB.
    * @since 2.0
    * @param aContextClient - call back address
    * @param aName          - name of context to retreive
    * @return - syncronous error code from function return
    */    
    virtual TInt GetContext( MNssGetContextClient* aContextClient, const TDesC& aName ) = 0;

    /**
    * GetContextList is called to retrieve a context list from the VAS DB. 
    * This method is asynchronous. A call back will be made to 
    * either GetContextListCompleted() or GetContextFailed() as
    * defined in the MNssGetContextClient interface. It also returns an
    * error code validating success of call to VAS DB.
    * @since 2.0
    * @param aContextClient - call back address 
    * @param  
    * @return - syncronous error code from function return
    */    
    virtual TInt GetContextList( MNssGetContextClient* aContextClient ) = 0;

    /**
    * DeleteContext is called to remove a context from the VAS DB. 
    * This method is asynchronous. A call back will be made to 
    * either DeleteContextCompleted() or DeleteContextFailed() as
    * defined in the MNssDeleteContextClient interface. It also returns an
    * error code validating success of call to VAS DB.
    * @since 2.0
    * @param aContextClient - call back address
    * @param aContext       - context object to delete
    * @return syncronous error code from function return
    */     
    virtual TInt DeleteContext( MNssDeleteContextClient* aContextClient,
                                MNssContext* aContext ) = 0;
    
    /**
    * SaveContext is called to store a context to the VAS DB. 
    * This method is asynchronous. A call back will be made to 
    * either SaveContextCompleted() or SaveContextFailed() as
    * defined in the MNssSaveContextClient interface. It also returns an
    * error code validating success of call to VAS DB.
    * @since 2.0
    * @param aContextClient - call back address
    * @param aContext       - context object to save
    * @return - syncronous error code from function return
    */
    virtual TInt SaveContext( MNssSaveContextClient* aContextClient, MNssContext* aContext ) = 0;

    /**
    * SaveClientData is called to save client-specific data about the context.
    * This method is asynchronous. A call back will be made to either
    * SaveContextCompleted or SaveContextFailed() as defined in the
    * MNssSaveContextClient interface. It also returns an error code validating
    * success of call to VAS DB.
    * @since 2.8
    * @param aContextClient - call back address.
    * @param aContext       - the client data of this context will be updated.
    * @param aClientData    - buffer of serialized client data, at most 100 bytes.
    */
    virtual TInt SaveClientData( MNssContext* aContext ) = 0;
    
    /**
    * Cancel get context / get context list / get global contexts 
    * @since 3.2
    * @param none
    * @return none
    */     
    virtual void CancelGetContext() = 0;
    
    };

#endif // MNSSCONTEXTMGR_H
