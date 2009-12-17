/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUIMVOICERECOGDIALOGCALLBACK_H
#define VUIMVOICERECOGDIALOGCALLBACK_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* COMMENT
*/
class MVoiceRecognitionDialogCallback
    {
    public:
        /**
        * Called when the dialog is dismissed.
        */
        virtual void DialogDismissed() = 0;
    };

#endif  // VUIMVOICERECOGDIALOGCALLBACK_H

// End of File
