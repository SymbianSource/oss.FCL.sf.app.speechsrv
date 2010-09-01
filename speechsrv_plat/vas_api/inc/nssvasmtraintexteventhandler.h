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
* Description:  The MNssTrainTextEventHandler class provides the call back methods to handle 
*               the events during Train From Text operation.
*
*/


#ifndef NSSVASMTRAINTEXTEVENTHANDLER_H
#define NSSVASMTRAINTEXTEVENTHANDLER_H

/**
* The MNssTrainTextEventHandler class provides the call back methods to handle 
* the events during Train From Text operation.
*
* @lib NssVASApi.lib
* @since 2.0
*/
class MNssTrainTextEventHandler
  {     
   public: 
   
    /**
	* Train Complete Event - Training was successful
	* @since 2.0
	* @param aErrorCode KErrNone if training was successfull
	*/       
    virtual void HandleTrainComplete( TInt aErrorCode ) = 0;
};

#endif // NSSVASMTRAINTEXTEVENTHANDLER_H   
            
// End of File
