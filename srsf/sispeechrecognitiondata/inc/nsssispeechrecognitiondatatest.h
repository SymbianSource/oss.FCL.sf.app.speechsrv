/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains definition of the SISpeechRecognitionDataTest structure and API.
*
*/


#ifndef __SRSIATA_TEST_H__
#define __SRSIATA_TEST_H__

//  INCLUDES
#include <e32base.h>


/**
*  Class to encapsulate diagnostic parameter values.
*
*  @lib SpeechRecognitionData.lib
*  @since 2.5
*/

class TSIDiagnostic
{
public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
	IMPORT_C TSIDiagnostic();
        /**
        * Destructor.
        */
	IMPORT_C ~TSIDiagnostic();

public:
	TInt32 iSNR;		// signal to noise ratio
	TBool iAccepted;
};


#endif
