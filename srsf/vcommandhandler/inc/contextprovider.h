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
* Description:  Utility object to provide a VAS context
*
*/


 
#ifndef CONTEXTPROVIDER_H
#define CONTEXTPROVIDER_H

#include "asyncworker.h"

#include <nssvasmgetcontextclient.h>
#include <nssvasmsavecontextclient.h>
#include <nssvascvasdbmgr.h>

class MNssContext;
class MNssContextMgr;

/**
* Provides the access to the VAS context
*/
class CContextProvider : public CAsyncWorker, public MNssGetContextClient,
                         public MNssSaveContextClient
	{
	public:
		static CContextProvider* NewL( CNssVASDBMgr& aVasDbManager );
		static CContextProvider* NewLC( CNssVASDBMgr& aVasDbManager );
		~CContextProvider();
		
		/**
		* Returns the VCommand VAS context. If it doesn't exists,
		* it is being created. client is responsible for the context deletion
		* 
		*
		* @todo If support for different contexts is needed, 
		* 1) Rename the method into GetContextLC;
		* 2) Add the optional
		*    argument "const TDesC& aContextName = KVoiceCommandContext"
		*/
		MNssContext* GetVCommandContextLC();
		               
    public:
    
    // From MNssGetContextClient
        /**
        * Callback to indicate GetContext successed.
        * client has to delete the context after using the context.
        * @since 2.0
        * @param aContext
        * @param aErrorCode KErrNone if getting of context was successfull
        * @return None
        */
        virtual void GetContextCompleted( MNssContext* aContext,
                                          TInt aErrorCode );

        /**
        * Callback to indicate GetContext successed.
        * client has to ResetAndDestroy() after using the list.
        * @since 2.0
        * @param aContextList A list of contexts.
        * @param aErrorCode KErrNone if getting of context list was successfull
        * @return None
        */
        virtual void GetContextListCompleted(
            MNssContextListArray *aContextList, TInt aErrorCode );
        
	public:        
    
    // From MNssSaveContextClient
       /**
       * Callback to indicate SaveContext successed.
       * @since 2.0
       * @param aErrorCode KErrNone if saving of context was successfull
       * @return None
       */
       virtual void SaveContextCompleted( TInt aErrorCode );
       
    private:
		/**
        * Creates VCommand context. To be called during the GetVCommandContextLC
        * sequence
        */
        MNssContext* CreateVCommandContextLC();

   	
    private:
    	// VAS context manager. Not owned
    	MNssContextMgr*             iContextManager;
    	
    	// Temporary variable to be used with async callbacks 
        MNssContext*                iVasContext;
        
        // Extra waiter for the SaveContext operation
        CAsyncWorker* iSaveContextWaiter;
	};


#endif // CONTEXTPROVIDER_H