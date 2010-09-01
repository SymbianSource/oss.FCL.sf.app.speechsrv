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
* Description:  Bridge-like class that creates the actual worker and tunnels 
*                all the client calls to it
*
*/


#include "rubydebug.h"
#include <vcommandapi.h>
#include "vcommandservices.h"

/** 
* Factory function
* @param aObserver Listener to be notified whenever the VCommand set is changed
* 		 by *another* instance of CVCommandHandler. I.e. lets the client know if
* 		 his understanding of the VCommand set is up to date
*/ 
EXPORT_C CVCommandHandler* CVCommandHandler::NewL( MVCommandHandlerObserver* aObserver )
	{
	CVCommandHandler* self = new (ELeave) CVCommandHandler;
	CleanupStack::PushL( self );
	self->ConstructL( aObserver );
	CleanupStack::Pop( self );
	return self;
	}

void CVCommandHandler::ConstructL( MVCommandHandlerObserver* aObserver )
	{
	iImpl = CVCommandService::NewL( aObserver );
	}

/**
* Synchronous. Service doesn't take the ownership, but makes an own copy
* Duplicates can be added
* @todo should we check for duplicates and leave with KErrAlreadyExists?
*/
EXPORT_C void CVCommandHandler::AddCommandL( const CVCommand& aCommand )
	{
	iImpl->AddCommandL( aCommand );
	}
	
/**
* Synchronous. Service doesn't take the ownership, but makes an own copies
* Duplicates can be added
* @todo should we check for duplicates and leave with KErrAlreadyExists?
*/
EXPORT_C void CVCommandHandler::AddCommandsL( const RVCommandArray& aCommands, 
                                              TBool aIgnoreErrors )
    {
    iImpl->AddCommandsL( aCommands, aIgnoreErrors );
    }

/**
* Synchronous. Removes the command from the system
* @param aCommand Reference to the command to be removed. Existing commands are
*		 compared against aCommand. All the matches are removed from VAS
* @leave KErrNotFound No such command
*/
EXPORT_C void CVCommandHandler::RemoveCommandL( const CVCommand& aCommand )
	{
	iImpl->RemoveCommandL( aCommand );
	}

/**
* Synchronous. 
* @param aCommands Reference to the list of commands to be removed. Existing commands are
*		 compared against aCommands items. All the matches are removed from VAS
* @param aIgnoreErrors If ETrue, even if some commands fail to be removed,
*        handler will remove as many as possible
*/
EXPORT_C void CVCommandHandler::RemoveCommandsL( const RVCommandArray& aCommands, 
		      TBool aIgnoreErrors )
	{
	iImpl->RemoveCommandsL( aCommands, aIgnoreErrors );
	}
	
/**
* Synchronous
* @return an array of the commands in the system
*         Ownership of the array is transfered to the client
*         The returned CVCommandArray contains copies of all the 
*         commands currently stored in this handler
*/
EXPORT_C CVCommandArray* CVCommandHandler::ListCommandsL()
	{
	return iImpl->ListCommandsL();
	}

EXPORT_C CVCommandHandler::~CVCommandHandler()
	{
	delete iImpl;
	}

void CVCommandHandler::PlaySpokenTextL( const CStoredVCommand& aCommand, 
                                  MNssPlayEventHandler& aPlayEventHandler ) const
    {
    iImpl->PlaySpokenTextL( aCommand, aPlayEventHandler );
    }

void CVCommandHandler::PlayAlternativeSpokenTextL( const CStoredVCommand& aCommand, 
                                  MNssPlayEventHandler& aPlayEventHandler ) const
    {
    iImpl->PlayAlternativeSpokenTextL( aCommand, aPlayEventHandler );
    }
    
/**
* Not intented to be called directly.
* @see CVCommand::CancelPlaybackL
*/                                   
void CVCommandHandler::CancelPlaybackL( const CStoredVCommand& aCommand ) const
    {
    iImpl->CancelPlaybackL( aCommand );
    }
    	        	
//End of file
