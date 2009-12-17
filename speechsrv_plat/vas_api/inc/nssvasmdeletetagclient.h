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
* Description:  MNssDeleteTagClient provides the public callback interfaces for 
*				 clients that want to delete tags from the VAS Db. The client must 
*				 inherit and override the DeleteTagCompleted() and DeleteTagFailed() 
*				 methods to handle VAS Db responses for deleting tag(s).
*
*/


#ifndef MNSSDELETETAGCLIENT_H
#define MNSSDELETETAGCLIENT_H
#include <e32std.h>

// CLASS DECLARATIONS
/**
*  MNssDeleteTagClient defines the call back methods used by VAS DB
*  in response to an asynchronous DeleteTag() request.
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssDeleteTagClient
{
public:

   /*enum TNssDeleteTagClientFailCode
   {
	  EVASDBItemNotFound = 1,
	  EVASDBNoMemory,
	  EVASDBDiskFull,
	  EVASDBFailure,
	  EVASSRSFailure
   };*/
   /**
   * Callback to indicate a tag was deleted successfully.
   * @since 2.0
   * @param aErrorCode KErrNone if deletetion was successfull
   * @return 
   */
   virtual void DeleteTagCompleted( TInt aErrorCode )=0;
  /**
   * Callback to indicate deleting a tag failed.
   * @since 2.0
   * @param aFailCode - The reason the deletion failed
   * @return 
   */  
   //virtual void DeleteTagFailed(TNssDeleteTagClientFailCode aFailCode)=0;

};

#endif
