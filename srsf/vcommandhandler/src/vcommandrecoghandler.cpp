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


#include <nssvasmtag.h>
#include <vcommandapi.h>
#include "vcommandinternalapi.h"
#include "rubydebug.h"
#include "tagcommandconverter.h"
#include "vcommandrecoghandler.h"

/**
* Factory function
*/
CVCRecognitionHandler* CVCRecognitionHandler::NewL()
	{
	CVCRecognitionHandler* self = new (ELeave) CVCRecognitionHandler;
	return self;
	}

// From MNssTagSelectHandler
/**
* Handle Tag Select Event - Event indicating a voice tag has been selected.
* @param aClientTag The voice tag which has been selected. This object is
*                   resonsible for deleting aClientTag.
* @return void
*/
void CVCRecognitionHandler::HandleTagSelect( MNssTag* aClientTag )
	{
	RUBY_DEBUG0( "CVCRecognitionHandler::HandleTagSelect start" );
	TRAPD( err, HandleTagSelectL( *aClientTag ) );
	if( err != KErrNone )
		{
		RUBY_ERROR1( "CVCRecognitionHandler::HandleTagSelectL failed with [%d] ", err );
		}
	
	delete aClientTag;
	RUBY_DEBUG0( "CVCRecognitionHandler::HandleTagSelect end" );
	}
	
void CVCRecognitionHandler::HandleTagSelectL( const MNssTag& aTag )
	{
	RUBY_DEBUG_BLOCK( "CVCRecognitionHandler::HandleTagSelectL" );
	CStoredVCommand* command = CTagCommandConverter::TagToCommandLC( aTag );
	command->Runnable().ExecuteL();
	CleanupStack::PopAndDestroy( command );
	}
	
//End of file
