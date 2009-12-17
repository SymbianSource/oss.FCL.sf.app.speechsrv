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
* Description:  Callback for a key press
*
*/

#ifndef VUIMKEYCALLBACK_H
#define VUIMKEYCALLBACK_H

/**
* Callback for a key press
* @lib VoiceUiRecognition.lib
*/
class MKeyCallback
    {
    public:
        virtual void HandleKeypressL( TInt aSoftkey ) = 0;
    };

#endif    // VUIMKEYCALLBACK_H

// End of File
