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
* Description:  MNssInterCompTagMgrClient provides the public callback interfaces 
*				 for internal VAS clients that want to retrieve tags from the VAS Db. 
*				 The client must inherit and override the following methods to 
*				 handle VAS Db responses for retrieving tag(s).
*
*/


#ifndef MNSSINTERCOMPTAGMGRCLIENT_H
#define MNSSINTERCOMPTAGMGRCLIENT_H

#include <e32std.h>



class CNssTag;

// CLASS DECLARATIONS
/**
*  MNssInterCompTagMgrClient defines the call back methods used by VAS DB.
*  @lib NssVASApi.lib
*  @since 2.0
*/

class MNssInterCompTagMgrClient
{
public:
   
   /**
   * Callback to indicate GetTag successed.
   * client has to ResetAndDestroy() after using the list.
   * @since 2.0
   * @param aTag tag returned
   * @param aErrorCode KErrNone if  getting of tag was successfull
   * @return None
   */
   virtual void GetTagCompleted( CNssTag* aTag, TInt aErrorCode ) = 0;
};

#endif
