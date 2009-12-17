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
* Description:  The MNssTrainVoiceEventHandler class provides the call back methods to handle 
*               the events during Train operation.
*
*/


#ifndef NSSVASMTRAINVOICEEVENTHANDLER_H
#define NSSVASMTRAINVOICEEVENTHANDLER_H

/**
* The MNssTrainVoiceEventHandler class provides the call back methods to handle 
* the events during Train operation.
*
* @lib NssVASApi.lib
* @since 2.0
*/
class MNssTrainVoiceEventHandler
  {
   public:  //enum

    /**
    * TNssTrainResult will have all the enums to represent the result values from
    * HandleTrainFailed method.
    * 
    * NOTE:
    * The HandleTrainFailed() should handle the undefined error for TNssTrainResult.
    */         
    enum TNssTrainResult
        {
        EVasTrainFailed = 1,  
        EVasRecordFailed,
        EVasTrainFailedNoSpeech,
        EVasTrainFailedTooEarly,
        EVasTrainFailedTooLong,
        EVasTrainFailedTooShort,
        EVasTagSpaceUnavailable,
        EVasTrainNoMemory,
        EVasTrainAccessDeny,
        EVasTrainUndefinedError10,   // for place holder
        EVasTrainUndefinedError9,
        EVasTrainUndefinedError8,
        EVasTrainUndefinedError7,
        EVasTrainUndefinedError6,
        EVasTrainUndefinedError5,
        EVasTrainUndefinedError4,
        EVasTrainUndefinedError3,
        EVasTrainUndefinedError2,
        EVasTrainUndefinedError1
        };

        
   public: 
        

    /**
    * Ready To Record Event - Recognizer is ready to receive the input speech
    * @since 2.0
    * @param no parameter
    * @return no return value
    */       
    virtual void HandleReadyToRecord() = 0;

		
	/**
	* Record Started Event - Recognizer is receiving the input speech 
    * @since 2.0
    * @param no parameter
    * @return no return value
    */
	virtual void HandleRecordStarted() = 0;

	/**
	* End Of Utterance Event - Recognizer has finished receiving the input speech 
    * @since 2.0
    * @param no parameter
    * @return no return value
    */
	virtual void HandleEouDetected() = 0;

	/**
	* Train Complete Event - Recognizer has trained the input speech
	* by creating a model.
    * @since 2.0
    * @param aErrorCode KErrNone if training was successfull
    * @return no return value
	*/
	virtual void HandleTrainComplete( TInt aErrorCode ) = 0;

};

#endif // NSSVASMTRAINVOICEEVENTHANDLER_H   
            
// End of File
