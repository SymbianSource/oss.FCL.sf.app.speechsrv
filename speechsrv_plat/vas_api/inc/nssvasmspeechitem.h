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
* Description:  The MSeechItem provides speech methods to the client.
*               The methods are used by the client to request speech services from SRS. 
*
*/


#ifndef NSSVASMSPEECHITEM_H
#define NSSVASMSPEECHITEM_H

// INCLUDE FILES
#include <nssvascoreconstant.h>

// FORWARD DECLARATIONS
class MNssPlayEventHandler;
class MNssTrainVoiceEventHandler;
class MNssTrainTextEventHandler;
class CNssTrainingParameters;

/**
* The MSpeechItem class represents the speech part of a voice tag. The
* MSeechItem provides speech methods to  the client. The methods are used by
* the client  to request speech services from SRS
*
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssSpeechItem
{
  public:  //enum

    /**
    * TNssSpeechItemResult will have all the enums to represent the return values from
    * MSpeechItem's methods.
    * 
    * NOTE:
    * The caller functions should be able to handle the unknown TNssSpeechItemResult enum values.
    */         
    enum TNssSpeechItemResult
     {
        EVasErrorNone,
        EVasUnexpectedRequest,  // Unexpected request from client, when the state is not Idle
        EVasInUse,              // VAS resource is currently in use.
        EVasInvalidParameter,
        EVasPlayFailed,
        EVasTrainFailed
     };


  public:

    /**
    * The Record method is used by the client to record input speech.
    *
    * @since 2.0
    * @param aTrainVoiceEventHandler A call back object to handle the record events
    * @return TNssSpeechItemResult, synchronous return value to identify whether the
    *         synchronous process is success or fail
    */    
    virtual TNssSpeechItemResult RecordL( 
                MNssTrainVoiceEventHandler* aTrainVoiceEventHandler ) = 0;


    /**
    * Method called by the client to indicate to SRS that a playback is
    * needed.
    *
    * @since 2.0
    * @param aPlayEventHandler A call back object to handle the play events
    * @return TNssSpeechItemResult, synchronous return value to identify whether the
    *         synchronous process is success or fail
    */    
    virtual TNssSpeechItemResult PlayL( MNssPlayEventHandler* aPlayEventHandler ) = 0;

    /**
    * Method called by the client to indicate to SRS that a playback is
    * needed with a certain language.
    *
    * @since 2.8
    * @param aPlayEventHandler A call back object to handle the play events
    * @param aLanguage Language which will be used in playback
    * @return TNssSpeechItemResult, synchronous return value to identify whether the
    *         synchronous process is success or fail
    */    
    virtual TNssSpeechItemResult PlayL( MNssPlayEventHandler* aPlayEventHandler,
                                        TLanguage aLanguage ) = 0;

   /**
    * Method called by the client to indicate to SRS that training is
    * needed.
    *
    * @since 2.0
    * @param aTrainVoiceEventHandler A call back object to handle the train events
    * @return TNssSpeechItemResult, synchronous return value to identify whether the
    *         synchronous process is success or fail
    */    
    virtual TNssSpeechItemResult TrainVoiceL( 
                MNssTrainVoiceEventHandler* aTrainVoiceEventHandler ) = 0;

    /**
    * Method called by the client to  cancel the previous operation.
    *
    * @since 2.0
    * @return TNssSpeechItemResult, synchronous return value to identify whether the
    *         synchronous process is success or fail
    */    
    virtual TNssSpeechItemResult CancelL() = 0;


    /**
    * The GetTrainingCapabilities member fucntion is used by the client 
    * to get training capabilities to beused in Speaker Independent voice 
    * recognition.
    *
    * @since 2.0
    * @return the pointer to the array of Training capabilities data
    */    
    virtual CArrayFixFlat<TNssVasCoreConstant::TNssTrainingCapability>* TrainingCapabilities() = 0;


    /**
    * The GetTrainedType member function is used by the client to get 
    * train type  to be used in Speaker Independent voice recognition. 
    * 
    * @since 2.0
    * @return the enum for the trained type
    */    
    virtual TNssVasCoreConstant::TNssTrainedType TrainedType() = 0;


    /**
    * The Text member fucntion is used by the client to get text to be
    * used in Speaker Independent voice recognition. The text can also be
    * used to identify the voice tag, and can be used for user display by
    * the client.
    *
    * @since 2.0
    * @return A text buffer
    */    
    virtual TDesC& Text() = 0;


    /**
    * The SetTextL method is used by the client to initialize the Text
    * data member
    *
    * @since 2.0
    * @param aText the reference of a text buffer
    */    
    virtual void SetTextL( const TDesC& aText ) = 0;


    /**
    * Playing a tag is not supported, if the recognition technology is speaker
    * independent and speech synthesis is not present.
    *
    * @since 2.0
    * @return ETrue Playing is supported.
    *         EFalse PlayL() will leave with KErrNotSupported.
    */
    virtual TBool IsPlayingSupported() = 0;


    /**
    * Trains the speech model. The model is based on the text
    * which was given earlier at SetTextL.
    *
    * @since 2.0
    * @param aEventHandler To inform the client when the training has finished.
    * @param aTrainingParams Training parameters 
    * @return TNssSpeechItemResult Success status
    */
    virtual TNssSpeechItemResult TrainTextL( MNssTrainTextEventHandler* aEventHandler,
                                             CNssTrainingParameters* aTrainingParams ) = 0;

    /**
    * Returns the raw training text from where escape characters are not removed.
    * Should not be used by clients who want to display the string.
    *
    * @since 3.2
    * @return Reference to the text buffer
    */    
    virtual TDesC& RawText() = 0;
    
    /**
    * Returns the part of text which identified with the given id.
    * If RawText() is for example "_1Amy_1Harris_2mobile" then 
    * PartialText( _L( "1" ) ) call will return "Amy Harris".
    *
    * @since 3.2
    * @param aIdentifier ID of the text part to return
    * @return Text, ownership is transferred to client
    */
    virtual HBufC* PartialTextL( const TDesC& aIdentifier ) = 0;
};

#endif // NSSVASMSPEECHITEM_H
