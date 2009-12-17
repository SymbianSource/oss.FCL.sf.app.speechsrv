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
* Description:  MNssInterCompContextMgrClient provides the public callback interfaces 
*				 for internal VAS clients that want to retrieve contexts from the VAS Db. 
*				 The client must inherit and override the following methods to 
*				 handle VAS Db responses for retrieving context(s).
*
*/


#ifndef MNSSINTERCOMPCONTEXTMGRCLIENT_H
#define MNSSINTERCOMPCONTEXTMGRCLIENT_H

#include <e32std.h>
#include <e32base.h>


// FORWARD DECLARATION
class CNssContext;

// TYPEDEFS
typedef CArrayPtrFlat<CNssContext> TMNssContextList;

// CLASS DECLARATIONS
/**
*  MNssInterCompContextMgrClient defines the call back methods used by VAS DB.
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssInterCompContextMgrClient
{
public:
   /**
   * Callback to indicate GetContext successed.
   * client has to delete the context after using the context.
   * @since 2.0
   * @param aContext 
   * @return None
   */
  virtual void GetContextCompleted( CNssContext* aContext,
                                    TInt aErrorCode ) = 0;

   /**
   * Callback to indicate GetContext successed.
   * client has to ResetAndDestroy() after using the list.
   * @since 2.0
   * @param aContextList A list of contexts. 
   * @return None
   */
  virtual void GetContextListCompleted( TMNssContextList *aContextList,
                                        TInt aErrorCode ) = 0;


  /**
   * Callback to indicate GetContext successed.
   * client has to ResetAndDestroy() after using the list.
   * @since 2.0
   * @param aContextList A list of contexts. 
   * @return None
   */
  virtual void GetGlobalContextsCompleted( TMNssContextList *aContextList,
                                           TInt aErrorCode ) = 0;

};

#endif
