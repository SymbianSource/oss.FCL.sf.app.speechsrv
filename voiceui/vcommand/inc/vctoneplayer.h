/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The MPlayEventHandler class provides the call back methods to handle 
*                the events during Play operation.
*
*/



#ifndef VCTONEPLAYER_H
#define VCTONEPLAYER_H

#include <e32base.h>

class MVCTonePlayer
    {
    public:

        virtual void MntpToneComplete()=0;
        virtual void MntpToneError()=0;
    };



#endif // VCTONEPLAYER_H



// End of File
