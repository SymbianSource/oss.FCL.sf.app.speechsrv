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
* Description:  The MNssPlayEventHandler class provides the call back methods to handle 
*               the events during Play operation.
*
*/



#ifndef NSSVASMPLAYEVENTHANDLER_H
#define NSSVASMPLAYEVENTHANDLER_H


//  INCLUDES
#include <e32base.h>
#include <nssvascoreconstant.h>


/**
* The MNssPlayEventHandler class provides the call back methods to handle 
*  the events during Play operation.
*
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssPlayEventHandler
{

  public:

    /**
    * TPlayResult will have all the enums to represent the result values from
    * HandlePlayError method.
    * 
    * NOTE:
    * The HandlePlayError() should handle the undefined error for TPlayResult.
    */         
    enum TNssPlayResult
        {
        EVasErrorNone,
        EVasPlayFailed,
        EVasPlayNoMemory,
        EVasPlayAccessDeny,
        EVasPlayUndefinedError10,   // for place holder
        EVasPlayUndefinedError9,
        EVasPlayUndefinedError8,
        EVasPlayUndefinedError7,
        EVasPlayUndefinedError6,
        EVasPlayUndefinedError5,
        EVasPlayUndefinedError4,
        EVasPlayUndefinedError3,
        EVasPlayUndefinedError2,
        EVasPlayUndefinedError1
        };

    /**
    * The HandlePlayStarted method is a virtual method implemented by the
    * client and is called when play is started
    * @since 2.0
    * @param aDuration - the duration of the utterance data
    * @return no return value
    */       
    virtual void HandlePlayStarted( TTimeIntervalMicroSeconds32 aDuration ) = 0;

    /**
    * The HandlePlayComplete method is a virtual method implemented by the
    * client and is called when play is completed
    * @since 2.0
    * @param aErrorCode EVasErrorNone if playing was successfull
    * @return no return value
    */       
    virtual void HandlePlayComplete( TNssPlayResult aErrorCode ) = 0;
};



#endif // NSSVASMPLAYEVENTHANDLER_H

// End of file
