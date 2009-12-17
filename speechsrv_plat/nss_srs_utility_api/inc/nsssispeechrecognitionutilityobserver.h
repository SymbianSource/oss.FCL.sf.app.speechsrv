/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the speech recognition utility.observer
*
*/



#ifndef NSSSISPEECHRECOGNITIONUTILITYOBSERVER_H
#define NSSSISPEECHRECOGNITIONUTILITYOBSERVER_H

//  INCLUDES

#include <nsssispeechrecognitiondataclient.h>


// CLASS DECLARATION

/**
*  This is the main class of Speech Recognition Utility Observer
*
*  @lib SpeechRecognitionUtility.lib
*  @since 2.0
*/

class MSISpeechRecognitionUtilityObserver
{

public:
      /**
        * 
        *	Pure virtual function that receives speech recognition utility events.
        * @since 2.0
        * @param 	aEvent			event code indicating the event type.
        * @return	-
        */
		virtual void MsruoEvent( TUid aEvent, TInt aResult )=0;

};

#endif	// of SPEECHRECOGNITIONUTILITYOBSERVER_H

// End of File
