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
* Description:  The MNssAdaptationEventHandler class provides the call back methods to handle 
*               the events after adptation.
*
*/


#ifndef NSSVASMADAPTATIONEVENTHANDLER_H
#define NSSVASMADAPTATIONEVENTHANDLER_H

/**
* The MNssAdaptationEventHandler class provides the call back methods to handle 
*  the after adaptation.
*
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssAdaptationEventHandler
    {        
    public: 
        
    /**
	* Adapt Complete Event - Adaptation was successful
	* @since 2.0
	* @param aErrorCode KErrNone if adaptation was successfull
	* @return no return value
	*/       
    virtual void HandleAdaptComplete( TInt aErrorCode ) = 0;
    };

#endif // NSSVASMADAPTATIONEVENTHANDLER  
            
// End of File
