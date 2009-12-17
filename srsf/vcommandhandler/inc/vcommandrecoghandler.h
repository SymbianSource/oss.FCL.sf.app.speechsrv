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
* Description:  Engine to perform the action corresponding to
*                the recognized and selected command
*
*/


 
#ifndef VCOMMANDRECOGHANDLER_H
#define VCOMMANDRECOGHANDLER_H

#include <nssvasmtagselecthandler.h>

class CVCommand;

/**
* Manages the recognized command selection
*/
class CVCRecognitionHandler : public CBase, public MNssTagSelectHandler 
	{
	public:
		/**
		* Factory function
		*/ 
		static CVCRecognitionHandler* NewL();
		
	public:
		// From MNssTagSelectHandler
		/**
	    * Handle Tag Select Event - Event indicating a voice tag has been selected.
	    * @param aClientTag The voice tag which has been selected. This object is
		*                   resonsible for deleting aClientTag.
	    * @return void
	    */
		virtual void HandleTagSelect( MNssTag* aClientTag );
		
	private:
		/** 
		* Execute a VCommand stored in the voice tag
		* @leave If aTag does not contain a VCommand according to 
		*			CVCommand::NewL codes
		*/ 
		void HandleTagSelectL( const MNssTag& aTag );
		
	};

#endif // VCOMMANDRECOGHANDER_H
