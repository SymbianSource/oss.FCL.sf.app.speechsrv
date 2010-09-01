/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#include "nssvasctagreference.h"
#include "nssvasctagmgr.h"

CNssTagReference::CNssTagReference()
{
   iTagName = NULL;
}

CNssTagReference::~CNssTagReference()
{
   delete iTagName;
}

void CNssTagReference::SetTagNameL(const TDesC& aTagName)
{
   if(iTagName)
   {
      delete iTagName;
      iTagName = NULL;
   }
   iTagName = aTagName.AllocL(); 
}
TDesC& CNssTagReference::TagName()
{
   return *iTagName;
}

void CNssTagReference::SetTagId(TInt aTagId)
{
   iTagId = aTagId;
}

TInt CNssTagReference::TagId()
{
   return iTagId;
}
