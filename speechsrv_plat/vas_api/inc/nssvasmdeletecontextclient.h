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
* Description:  MNssDeleteContextClient provides the public callback interfaces for 
*				 clients that want to delete contexts from the VAS Db. The client 
*				 must inherit and override the DeleteContextCompleted() and 
*				 DeleteContextFailed() methods to handle VAS Db responses for 
*				 deleting context(s).
*
*/


#ifndef MNSSDELETECONTEXTCLIENT_H
#define MNSSDELETECONTEXTCLIENT_H

#include <e32std.h>


// CLASS DECLARATIONS
/**
*  MNssDeleteContextClient defines the call back methods used by VAS DB
*  when replying to an asynchronous DeleteContext() request.
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssDeleteContextClient
{
public:

   /*enum TNssDeleteContextClientFailCode
   {
	  EVASDBItemNotFound = 1,
	  EVASDBNoMemory,
	  EVASDBDiskFull,
	  EVASDBFailure,
	  EVASSRSFailure
   };*/

   /**
   * Callback to indicate a context was deleted successfully.
   * @since 2.0
   * @param 
   * @return 
   */
   virtual void DeleteContextCompleted( TInt aErrorCode ) = 0;
   
   /**
   * Callback to indicate deleting a context failed.
   * @since 2.0
   * @param aFailCode - The reason the deletion failed
   * @return None
   */   
   //virtual void DeleteContextFailed(TNssDeleteContextClientFailCode aFailCode)=0;
   
 
 

 

 
};

#endif
