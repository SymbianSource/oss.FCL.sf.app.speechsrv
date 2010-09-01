/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MNssGetTagReference Client is the public callback interfaces for
*                clients that want to retrieve tag references from the VAS Db.
*                The client must inherit and override the GetTagCompleted() and
*                GetTagFailed() methods to handle VAS Db responses for
*                retrieving tag reference(s).
*
*/

#ifndef MNSSGETTAGREFCLIENT_H
#define MNSSGETTAGREFCLIENT_H

// includes 
#include <e32std.h>
#include <e32base.h>

// forward declarations
class MNssTagReference;

//typedef 
typedef CArrayPtrFlat<MNssTagReference> NssTagReferenceListArray;

// CLASS DECLARATIONS
/**
*  MNssGetTagReferenceClient defines the call back methods used by VAS DB
*  when replying to an asynchronous GetTagReference() request.
*  @lib NssVASApi.lib
*  @since 2.0
*/

class MNssGetTagReferenceClient
{
public:

   /*enum TNssGetTagReferenceClientFailCode
   {
	  EVASDBItemNotFound = 1, 
	  EVASDBNoMemory,
	  EVASDBDiskFull,
	  EVASDBFailure
   };*/
   
   /**
   * Callback to indicate GetTagReference successed.
   * client has to delete the tagref list
   * @since 2.0
   * @param aTagRefList 
   * @return None
   */
   virtual void GetTagReferenceListCompleted(
       CArrayPtrFlat<MNssTagReference>* aTagRefList, TInt aErrorCode ) = 0;
   
   /**
   * Callback to indicate GetTagReference failed.
   * @since 2.0
   * @param aFailCode - The reason GetTagReference failed
   * @return None
   */ 
   //virtual void GetTagReferenceFailed(TNssGetTagReferenceClientFailCode aFailCode)=0;
   
};

#endif
