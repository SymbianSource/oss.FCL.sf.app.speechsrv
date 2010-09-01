/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This interface defines the call back methods used by VAS DB
*                when replying to an asynchronous DeleteContext() request.
*
*/


#ifndef MNSSVASDATABASECLIENT_H
#define MNSSVASDATABASECLIENT_H

#include <e32std.h>
#include "nssvasccontext.h"
#include "nssvasctag.h"
#include "nssvasctagreference.h"


// CLASS DECLARATIONS
/**
*  MNssVASDatabaseClient defines the call back methods from the VAS DB
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssVASDatabaseClient
{
public:

   enum TNssVASDBClientReturnCode
   {
      EVASDBSuccess,
      EVASDBItemNotFound,
	  EVASDBNoMemory,
	  EVASDBDiskFull,
	  EVASDBFailure
   };

   /**
   * Callback from VAS DB
   * @param aEvent 
   * @return None
   */
   virtual void VASDatabaseComplete( CArrayPtrFlat<CNssContext>* aContextList, 
                                     CArrayPtrFlat<CNssTag>* aTagList, 
	                                 CArrayPtrFlat<CNssTagReference>* aTagRefList,
	                                 TNssVASDBClientReturnCode aCode ) = 0;
   
};

#endif
