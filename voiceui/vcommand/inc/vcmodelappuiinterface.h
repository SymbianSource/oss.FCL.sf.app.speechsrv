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
* Description:  Interface implemented by the application ui class to abstract
*                the relation with the model class
*
*/


#ifndef MVCMODELAPPUIINTERFACE_H
#define MVCMODELAPPUIINTERFACE_H

// CLASS DECLARATION

class MVcModelAppUiInterface
    {
    public: 
        /**
        * Notifies the the command set has changed and the view may need to be
        * updated
        */
        virtual void CommandSetChanged() = 0;
        
        /**
        * Called before starting VCommandHandler operations that should be
        * executed fully before application can be exited. Leave safe operation.
        * A subsequent call to CleanupStack::PopAndDestroy should be done
        * after such operations have completed
        */
        virtual void StartAtomicOperationLC() = 0;
    };

#endif  // MVCMODELAPPUIINTERFACE_H

// End of File


