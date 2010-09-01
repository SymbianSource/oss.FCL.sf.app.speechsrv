/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssTagMgr implements the MNssTagMgr interface. In addition, it 
*               also provides methods for internal use by VAS components.
*
*/


#ifndef NSSVASCTAGREFERENCE_H
#define NSSVASCTAGREFERENCE_H

#include <e32std.h>
#include <e32base.h>
#include "nssvasmtagreference.h"

// CLASS DECLARATIONS
/**
*  CNssTagReference is a tag object, which only contains the name and the ID.
*  What has been dropped is RRD data and data required for speech actions.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssTagReference: public CBase, public MNssTagReference
{
public:
   CNssTagReference();

   ~CNssTagReference();

   TDesC& TagName();
   void   SetTagNameL(const TDesC& aTagName);
   TInt   TagId();
   void   SetTagId(TInt aTagId);
   
private:
   HBufC* iTagName;
   TInt   iTagId;
};

#endif // NSSVASCTAGREFERENCE_H

// End of file
