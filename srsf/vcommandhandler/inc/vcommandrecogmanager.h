/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constructs / destroyes the recognition selection handler
*
*/


 
#ifndef VCOMMANDRECOGMANAGER_H
#define VCOMMANDRECOGMANAGER_H

class CVCRecognitionHandler;
class CNssSelectNotificationBuilder;

/**
* Prepares the recognition selection handling facilities and destroys them.
* In particular establishes and managed the connections to VAS
*/
class CVCRecognitionManager : public CBase 
	{
	public:
		/** 
		* Factory function
		* @todo Remove comments for such a simple and standard methods?
		*/ 
		IMPORT_C static CVCRecognitionManager* NewL();
		
		/**
		* Destructor
		*/ 
		IMPORT_C virtual ~CVCRecognitionManager();
	
	private:
		void ConstructL();
		
	private:
		// Selection handling logic
		CVCRecognitionHandler*	iRecognitionHandler;
		
		// VAS counterpart
		CNssSelectNotificationBuilder* iSelectNotificationBuilder;
	};

#endif // VCOMMANDRECOGMANAGER_H
