/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The MNssResetFactoryModelsClient class provides the call back 
*               methods to handle the events after resetting factory models.
*
*/


#ifndef NSSVASMRESETFACTORYMODELSCLIENT_H
#define NSSVASMRESETFACTORYMODELSCLIENT_H

/**
* The MNssResetFactoryModelsClient class provides the call back methods to handle 
* the after adaptation.
*
* @lib NssVASApi.lib
* @since 2.8
*/
class MNssResetFactoryModelsClient
    {
    public: 
      

    /**
	* Resetting factory models is completed
	*
	* @param aErrorCode KErrNone if resetting was successfull
	*/       
    virtual void HandleResetComplete( TInt aErrorCode ) = 0;

    };

#endif // NSSVASMRESETFACTORYMODELSCLIENT_H
            
// End of File
