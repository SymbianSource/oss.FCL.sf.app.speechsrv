/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The TNssSpeechItemConstant provides constant values used by internal
*               VASAPI methods.
*
*/


#ifndef NSSVASTSPEECHITEMCONSTANT_H
#define NSSVASTSPEECHITEMCONSTANT_H

/**
* The TNssSpeechItemConstant provides constant values used by internal VASAPI 
* methods.
*
*  @lib NssVasApi.lib
*  @since 2.0
*/
class TNssSpeechItemConstant
{
public:
   
    // The states for SpeechItem object 
    enum TNssState
    {
        EIdle,
        EBusy,
        EPlayMemoryStart,
        EPlayMemoryComplete,
        EPlayStart,
        EPlayComplete,
        ETrainStart,
        ETrainComplete,
        ESaveToSrsStart,
        ESaveToSrsComplete,
        EDeleteToSrsStart,
        EDeleteToSrsComplete,
        ETerminate,
        ECommitComplete,
        EAddPronounciationComplete,
        EAddRuleComplete,
        ERemoveRuleComplete,
        ERemovePronounciationComplete,
        ERemoveModelCompelete,
        EWaitForCommitChanges
    };

    // The results for the SpeechItem operations
    enum TNssResult
    {
        EErrorNone,
        EInvalidState,
        EInvalidEventHandler
    };

};

#endif // NSSVASTSPEECHITEMCONSTANT

// End of file
