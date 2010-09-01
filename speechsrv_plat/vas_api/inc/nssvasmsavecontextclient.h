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
* Description:  MNssSaveContextClient provides the public callback interfaces for 
*				 clients that want to store contexts to the VAS Db. The client must 
*				 inherit and override the SaveContextCompleted() and SaveContextFailed()
*				 methods to handle VAS Db responses for storing context(s).
*
*/


#ifndef MNSSSAVECONTEXTCLIENT_H
#define MNSSSAVECONTEXTCLIENT_H
#include <e32std.h>


// CLASS DECLARATIONS
/**
*  MNssSaveContextClient is the interface for save context call backs.
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssSaveContextClient
{
public:

   /*enum TNssSaveContextClientFailCode
   {
	  EVASDBNoMemory = 2,
	  EVASDBDiskFull,
	  EVASDBFailure,
	  EVASSRSFailure
   };*/
   /**
   * Callback to indicate SaveContext successed.
   * @since 2.0
   * @param aErrorCode KErrNone if saving of context has completed successfully
   * @return None
   */
   virtual void SaveContextCompleted( TInt aErrorCode ) = 0;
   
   /**
   * Callback to indicate SaveContext failed.
   * @since 2.0
   * @param aFailCode - The reason SaveContext failed.
   * @return None
   */
   //virtual void SaveContextFailed(TNssSaveContextClientFailCode aFailCode)=0;

};

#endif
