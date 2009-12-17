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
* Description:  MNssGetContextClient provides the public callback interfaces for 
*               clients that want to retrieve contexts from the VAS Db. The 
*               client must inherit and override the GetContextCompleted() and 
*               GetContextFailed() methods to handle VAS Db responses for 
*               retrieving context(s).
*
*/


#ifndef NSSVASMGETCONTEXTCLIENT_H
#define NSSVASMGETCONTEXTCLIENT_H
#include <e32std.h>
#include <e32base.h>

//FORWARD DECLARATIONS
class MNssContext;

typedef CArrayPtrFlat<MNssContext> MNssContextListArray;

// CLASS DECLARATIONS
/**
*  MNssGetContextClient defines the call back methods used by VAS DB
*  when replying to an asynchronous GetContext() request.
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssGetContextClient
{
public:
#ifdef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY

   /**
   * Callback to indicate GetContext successed.
   * client has to delete the context after using the context.
   * @since 3.2
   * @param aContext 
   * @return None
   */
  virtual void GetContextCompleted( MNssContext* aContext,
                                    TInt aErrorCode ) = 0;

   /**
   * Callback to indicate GetContext successed.
   * client has to ResetAndDestroy() after using the list.
   * @since 3.2
   * @param aContextList A list of contexts. 
   * @return None
   */
  virtual void GetContextListCompleted( MNssContextListArray *aContextList,
                                        TInt aErrorCode ) = 0;

#else

  enum TNssGetContextClientFailCode
  {
	  EVASDBItemNotFound = 1,
	  EVASDBNoMemory,
	  EVASDBDiskFull,
	  EVASDBFailure
  };

   /**
   * Callback to indicate GetContext successed.
   * client has to delete the context after using the context.
   * @since 2.0
   * @param aContext 
   */
  virtual void GetContextCompleted( MNssContext* /*aContext*/,
                                    TInt /*aErrorCode*/ ) {};

   /**
   * Callback to indicate GetContext successed.
   * client has to ResetAndDestroy() after using the list.
   * @since 2.0
   * @param aContextList A list of contexts. 
   */
  virtual void GetContextListCompleted( MNssContextListArray* /*aContextList*/,
                                        TInt /*aErrorCode*/ ) {};

   /**
   * Callback to indicate GetContext successed.
   * client has to delete the context after using the context.
   * @since 2.0
   * @param aContext 
   */
  virtual void GetContextCompleted( MNssContext* /*aContext*/ ) {};

   /**
   * Callback to indicate GetContext successed.
   * client has to ResetAndDestroy() after using the list.
   * @since 2.0
   * @param aContextList A list of contexts. 
   */
  virtual void GetContextListCompleted( MNssContextListArray* /*aContextList*/ ) {};

   /**
   * Callback to indicate GetContext failed.
   * @since 2.0
   * @param aFailCode The reason GetContext failed
   * @return None
   */  
  virtual void GetContextFailed( TNssGetContextClientFailCode /*aFailCode*/ ) {};
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY

};

#endif
