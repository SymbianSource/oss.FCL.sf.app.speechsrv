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
* Description:  MNssSaveTagClient provides the public callback interfaces for clients 
*				 that want to store tags to the VAS Db. The client must inherit and 
*				 override the SaveTagCompleted() and SaveTagFailed() methods to handle 
*				 VAS Db responses for saving tag(s).
*
*/


#ifndef MNSSSAVETAGCLIENT_H
#define MNSSSAVETAGCLIENT_H
#include <e32std.h>


// CLASS DECLARATIONS
/**
*  MNssSaveTagClient defines the call back methods used by VAS DB
*  when replying to an asynchronous SaveTag() request.
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssSaveTagClient
{
public:

   /*enum TNssSaveTagClientFailCode
   {
 	  EVASDBNoMemory = 2,
	  EVASDBDiskFull,
	  EVASDBFailure,
	  EVASSRSFailure
   };*/
   
   /**
   * Callback to indicate SaveTag successed.
   * @since 2.0
   * @param None
   * @return None
   */
   virtual void SaveTagCompleted( TInt aErrorCode ) = 0;
  
   /**
   * Callback to indicate SaveTag failed.
   * @since 2.0
   * @param aFailCode - The reason SaveContext failed.
   * @return None
   */  
   //virtual void SaveTagFailed(TNssSaveTagClientFailCode aFailCode)=0;
};

#endif
