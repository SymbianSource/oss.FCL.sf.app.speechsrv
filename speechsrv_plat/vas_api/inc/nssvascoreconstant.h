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
* Description:  The TNssVasCoreConstant provides constant values used by the VAS.
*
*/


#ifndef NSSVASCORECONSTANT_H
#define NSSVASCORECONSTANT_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>


// CONSTANTS
// default DB value for new item
const TInt KNssVASDbDefaultValue = -1;     

// this value is used for recording time [micro second]
const TInt KNssVASRecordTime = 5000000; //4000000;    

// this value is used for Vas API's UID
const TUid KNssVASApiUid = { 0x101F853F };    

_LIT( KVasDatabaseName, "c:nssvasdatabase.db");
_LIT( KVasDatabaseFormatString, "SECURE[10201AFE]");
static const TInt KVasDatabaseDrive = 'C';


// Char used to separate firstname-lastname
// Note that first and only first symbol is used.
// Do not make it empty (""), it will cause run-time error in 
// CNssContactHandlerImplementation 
_LIT ( KNameSeparator, "_" );


// Indexes for extension training
_LIT( KNameTrainingIndex, "1" );
_LIT( KExtensionTrainingIndex, "2" );

// Number of characters required by escape character (separator) and index
const TInt KTrainingIndexSize = 2;

// Format string which is substituted with name
_LIT( KExtensionFormatString, "%U" );


// Indexes of RRD data
// Contact ID
const TInt KVasContactIdRrdLocation = 0;
// Field ID
const TInt KVasFieldIdRrdLocation = 1;
// Type of the extension command (SINDE action)
const TInt KVasExtensionRrdLocation = 2;
// Type of the voice tag (see TVasTagType)
const TInt KVasTagTypeRrdLocation = 3;
// Extension command id
const TInt KVasExtensionCommandRrdLocation = 4;

// SINDE command
enum TVasExtensionCommand
    {
    EDefaultCommand,
    EMobileCommand,
    EGeneralCommand,
    EWorkCommand,
    EHomeCommand,
    EEmailCommand,
    EVideoCommand,
    EVoipCommand,
    EMessageCommand
    };

// Extension action of the voice tag
// Located in KVasExtensionRrdLocation
enum TVasExtensionAction
    {
    EDial,
    ENewMessage,
    ENewEmail
    }; 

// Type of the voice tag which is saved to VAS database
// Located in KVasTagTypeRrdLocation
enum TVasTagType
    {
    ETagTypeUnknown = 0,
    ETagTypeName,
    ETagTypeCompanyName,
    ETagTypeNickName
    };

// Publish & Subscribe constants

// All the SIND values are part of the whole system state. Should be set
// and read by various Nokia processes
// Therefore SIND properties are published in the system category
const TUid KSINDUID = {KUidSystemCategoryValue};

enum TSINDKeys
    {    
    ERecognitionState=0,   
    ETrainingContactsState,
    EVoiceTagSelectionTagId,
    EVoiceTagSelectionContextId,
    // Key used for cache sync updates between the VCommandHandler instances
    ECommandHandlerTickKey
    };

// Recognition state values for P&S
enum TRecognitionStateValues
    {
    ERecognitionStarted = 0, 
    ERecognitionSpeechEnd, 
    ERecognitionSuccess, 
    ERecognitionFail
    };

// Contact handler state values for P&S
// Tell if contact handler is busy training contacts and other clients
// should delay VAS operations    
enum TTrainingContactsStateValues
    {
    EContactsNoTraining = 0,
    EContactsTraining
    };

_LIT( KNssVASDBOutOfSync, "VASDB OutOfSync" );
 
/**
* The TNssVasCoreConstant provides constant values used by the VASAPI methods and clients.
*
* @lib NssVasApi.lib
* @since 2.0
*/
class TNssVasCoreConstant
{
public:

    /**
    * TNssTrainedType will have all the enums to represent the trained type.
    */    
    enum TNssTrainedType
        {
        EVasNotTrained,
        EVasTrainVoice,
        EVasTrainText
        };


    /**
    * TNssTrainingCapability will have all the enums to represent the training capabilities
    * 
    */    
    enum TNssTrainingCapability
        {
        EVasTrainCapVoice
        };
        
    // For Virtual Phonebook compliance
    // (Not used inside VAS)
    enum TNssVASErrorCode
        {	
        EVasSuccess,
        EVasFailure
        };

    enum TNssVASPanicCodes
        {
        EVasDBSrsDBOutOfSync
        };
};

#endif /* __VASCORECONSTANT_H__ */

// End of file
