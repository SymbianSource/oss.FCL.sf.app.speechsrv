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
* Description:  MNssGetTagClient provides the public callback interfaces for clients 
*               that want to retrieve tags from the VAS Db. The client must inherit 
*               and override the GetTagCompleted() and GetTagFailed() methods to 
*               handle VAS Db responses for retrieving tag(s).
*
*/


#ifndef MNSSGETTAGCLIENT_H
#define MNSSGETTAGCLIENT_H

// includes 
#include <e32std.h>
#include <e32base.h>

// forward declarations
class MNssTag;

//typedef 
typedef CArrayPtrFlat<MNssTag> MNssTagListArray;

// CLASS DECLARATIONS
/**
*  MNssGetTagClient defines the call back methods used by VAS DB
*  when replying to an asynchronous GetTag() request.
*  @lib NssVASApi.lib
*  @since 2.0
*/

class MNssGetTagClient
{
public:
#ifdef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY

   /**
   * Callback to indicate GetTag successed.
   * client has to delete the tag list
   * @since 3.2
   * @param aTagList 
   */
   virtual void GetTagListCompleted( MNssTagListArray* aTagList,
                                     TInt aErrorCode ) = 0;
#else
   enum TNssGetTagClientFailCode
   {
	  EVASDBItemNotFound = 1, 
	  EVASDBNoMemory,
	  EVASDBDiskFull,
	  EVASDBFailure
   };
   
   /**
   * Callback to indicate GetTag successed.
   * client has to delete the tag list
   * @since 2.0
   * @param aTagList 
   */
   virtual void GetTagListCompleted( MNssTagListArray* /*aTagList*/,
                                     TInt /*aErrorCode*/ ) {};   
   
   /**
   * Callback to indicate GetTag successed.
   * client has to delete the tag list
   * @since 2.0
   * @param aTagList 
   */
   virtual void GetTagListCompleted( MNssTagListArray* /*aTagList*/ ) {};
   
   /**
   * Callback to indicate GetTag failed.
   * @since 2.0
   * @param aFailCode - The reason GetTag failed
   */ 
   virtual void GetTagFailed( TNssGetTagClientFailCode /*aFailCode*/ ) {};
 
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY   
};

#endif
