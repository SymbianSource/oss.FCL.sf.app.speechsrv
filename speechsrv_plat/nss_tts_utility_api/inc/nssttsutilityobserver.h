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
* Description:  Observer mixin class for TtsUtility
*
*/


#ifndef NSSTTSUTILITYOBSERVER_H
#define NSSTTSUTILITYOBSERVER_H

// INCLUDES
#include <mdaaudiosampleplayer.h>

// CLASS DECLARATION

/**
* Tts Utility callback observer
*
* @lib nssttsutility.lib
* @since 2.8
*/
class MTtsClientUtilityObserver : public MMdaAudioPlayerCallback
    {
    public:
        /**
        * Called when asynchronous custom command finishes.
        *
        * @since 2.8
        * @param "TInt aEvent" Event code.
        * @param "TInt aError" Error code.
        */
        virtual void MapcCustomCommandEvent( TInt aEvent, TInt aError ) = 0;
    };

#endif // NSSTTSUTILITYOBSERVER_H   

// End of File
