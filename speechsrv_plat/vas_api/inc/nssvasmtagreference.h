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
* Description:  MNssTagReference is an abstraction for a voice tag reference. 
*               MNssTagReference consist of a tag name, which can be
*               used to receive the full tag when need. 
*
*/


#ifndef NSSVASMTAGREFERENCE_H
#define NSSVASMTAGREFERENCE_H

#include <e32std.h>

/**
* MNssTagReference is the interface class for a tag reference.
* From a tagreference a full tag can be retrieved with
* the GetTag(MNssGetTagClient* aClient,MNssTagReference* aTagRef);
* method.
* @lib NssVASApi.lib
* @since 2.0
*/
class MNssTagReference
    {
    public:
    
        /**
        * Get the tagname
        * @since 2.0
        * @return tagname for this tag
        */
        virtual TDesC& TagName()=0; 

        /**
        * virtual destructor for proper clean up
        */
    	virtual ~MNssTagReference(){}
    };

#endif // NSSVASMTAGREFERENCE_H

// End of file
