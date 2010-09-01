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


#include <nssvascvasdbmgr.h>
#include <nssvasmcontextmgr.h>
#include <nssvascselectnotificationbuilder.h>
#include <nssvasmtagselectnotification.h>
#include "vcommandinternalapi.h"
#include "rubydebug.h"
#include "vcommandrecogmanager.h"
#include "vcommandrecoghandler.h"

/**
* Factory function
*/
EXPORT_C CVCRecognitionManager* CVCRecognitionManager::NewL()
	{
	CVCRecognitionManager* self = new (ELeave) CVCRecognitionManager;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
/**
* Destructor
*/	
EXPORT_C CVCRecognitionManager::~CVCRecognitionManager()
	{
	RUBY_DEBUG0( "CVCRecognitionManager::~CVCRecognitionManager start" );
	delete iRecognitionHandler;
	delete iSelectNotificationBuilder;
	RUBY_DEBUG0( "CVCRecognitionManager::~CVCRecognitionManager end" );
	}

/**
* Second-phase constructor
*/
void CVCRecognitionManager::ConstructL()
	{
	RUBY_DEBUG_BLOCK( "CVCRecognitionManager::ConstructL" );
	CNssVASDBMgr* vasDbManager = CNssVASDBMgr::NewL();
	CleanupStack::PushL( vasDbManager );
	
    vasDbManager->InitializeL();
    MNssContextMgr* contextManager = vasDbManager->GetContextMgr();
      
    iSelectNotificationBuilder = CNssSelectNotificationBuilder::NewL();
    iSelectNotificationBuilder->InitializeL();
    MNssTagSelectNotification* tagSelectNotification = 
    				iSelectNotificationBuilder->GetTagSelectNotification();

    iRecognitionHandler = CVCRecognitionHandler::NewL();
    
    MNssContext* context = contextManager->CreateContextL();
    CleanupDeletePushL( context );
    context->SetNameL( KVoiceCommandContext );
    context->SetGlobal( ETrue );
    tagSelectNotification->RegisterL( context, iRecognitionHandler );
    // Context is copied inside RegisterL - no need to keep it anymore
    CleanupStack::PopAndDestroy( context );
	CleanupStack::PopAndDestroy( vasDbManager );
	}
	
//End of file
