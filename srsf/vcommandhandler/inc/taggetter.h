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
* Description:  Fetches the VAS tags from VAS DB
*
*/


 
#ifndef TAGGETTER_H
#define TAGGETTER_H

#include "asyncworker.h"
#include <nssvasmgettagclient.h>
#include <nssvasmgettagclient.h>
#include <nssvasmcontext.h>
#include <nssvasmtagmgr.h>

/*
* Fetches the context voice tags from VAS. Makes the asynchronous process
* look like synchronous to the clients
*/
class CTagGetter : public CAsyncWorker, public MNssGetTagClient
	{
	public:
		static CTagGetter* NewL();
		static CTagGetter* NewLC();
		
    	/**
    	* Retrieves the list of tags for a given context. Synchronous.
    	* Leaves two objects on the cleanup stack!
    	* First PopAndDestroy will ResetAndDestroy content
    	* Second one will destroy the MNssTagListArray itself
    	*/
        MNssTagListArray* GetTagListLC2( MNssTagMgr& aTagManager, const MNssContext& aContext );
        
        /**
    	* Retrieves the list of tags for a given context and vcommand id. Synchronous.
    	* Leaves two objects on the cleanup stack!
    	* First PopAndDestroy will ResetAndDestroy content
    	* Second one will destroy the MNssTagListArray itself
    	*/
        MNssTagListArray* GetTagListLC2( MNssTagMgr& aTagManager, const MNssContext& aContext, 
                                         TInt aCommandId );
        
        // From  MNssGetTagClient
           
        /**
        * Callback to indicate GetTag successed.
        * client has to delete the tag list
        * @param aTagList 
        * @param aErrorCode KErrNone if getting of tag list was successfull
        * @return None
        */
        virtual void GetTagListCompleted( MNssTagListArray* aTagList,
                                          TInt aErrorCode );
    
    private:
    
        /** 
        * After one of the GetTagListLC2 functions requested a list of 
        * tags from VAS, they call this function to wait for the query
        * result and process it 
        * Leaves two objects on the cleanup stack!
    	* First PopAndDestroy will ResetAndDestroy content
    	* Second one will destroy the MNssTagListArray itself
    	* @param aNssErr Error code from the GetTagList request
    	*/
        MNssTagListArray* WaitAndFormTagListLC2( TInt aNssErr );
        
    private:
        // Temporary holder for the tag list. Is used with async callbacks
        MNssTagListArray*           iTagList;
		
	};

#endif // TAGGETTER_H