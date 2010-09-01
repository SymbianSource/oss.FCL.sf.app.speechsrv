/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssVasDbBuilder is owned by CNssVASDBMgr. It is responsible for creating
*               the context manaher, tag manager, vas db event notifier, and tag select 
*               notifier. This class is a singleton, and is created using the InstanceL(),
*               and deleted using the RemoveInstance() methods. 
*
*/


#include "nssvascvasdbbuilder.h"
#include "rubydebug.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::InstanceL
// This function returns an instance of CNssVASDBBuilder.
// Must be called one to one with RemoveInstance(). 
// -----------------------------------------------------------------------------
//
CNssVASDBBuilder* CNssVASDBBuilder::InstanceL()
    {
    RUBY_DEBUG_BLOCK( "CNssVASDBBuilder::InstanceL" );

	TThreadLocalStorage* pTls = 0;
	if (Dll::Tls() == 0)
	    {
		CNssVASDBBuilder* instance = CNssVASDBBuilder::NewL();
		CleanupStack::PushL(instance);
		pTls = new (ELeave) TThreadLocalStorage ();
		CleanupStack::Pop(instance);
		pTls->iInstance = instance;
		pTls->iInstanceCount = 0;
		Dll::SetTls (pTls);
	    }
	else
	    {
		pTls = (TThreadLocalStorage *) Dll::Tls();
	    }
	pTls->iInstanceCount++;
	return pTls->iInstance;
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::RemoveInstance
// This method removes one instance of this class by decrementing
// the number of instances. When the number of instances hit 0,
// the object is deleted. Must be called one to one with InstanceL(). 
// -----------------------------------------------------------------------------
//
void CNssVASDBBuilder::RemoveInstance() 
    {
    RUBY_DEBUG0( "CNssVASDBBuilder::RemoveInstance" );

	TThreadLocalStorage* pTls = (TThreadLocalStorage *) Dll::Tls();
	if (--pTls->iInstanceCount <= 0)
	    {
		delete pTls->iInstance;
		delete (TThreadLocalStorage *) Dll::Tls();
		Dll::SetTls (0);
	    }
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::CNssVASDBBuilder
// C++ constructor
// -----------------------------------------------------------------------------
//
CNssVASDBBuilder::CNssVASDBBuilder()
    {
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::ConstructL
// EPOC Constructor
// -----------------------------------------------------------------------------
//
void CNssVASDBBuilder::ConstructL()
    {
	iVasDatabase = CNssVASDatabase::NewL(CActive::EPriorityStandard);
	iContextMgr = CNssContextMgr::NewL(iVasDatabase);
    iTagMgr = CNssTagMgr::NewL(iVasDatabase, iContextMgr);
    iDBTagSelectNotifier = CNssDBTagSelectNotifier::NewL(iVasDatabase);
    //iVASDBEventNotifier = new (ELeave) CNssVASDBEventNotifier(iVasDatabase);
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::NewL
// 2 phase Construction
// -----------------------------------------------------------------------------
//
CNssVASDBBuilder* CNssVASDBBuilder::NewL()
    {
    RUBY_DEBUG_BLOCK( "CNssVASDBBuilder::NewL" );

	CNssVASDBBuilder* self = NewLC();
	CleanupStack::Pop(self);
	return (self);
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::NewLC
// 2 phase Construction
// -----------------------------------------------------------------------------
//
CNssVASDBBuilder* CNssVASDBBuilder::NewLC()
    {
	CNssVASDBBuilder* self = new(ELeave)CNssVASDBBuilder;
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::~CNssVASDBBuilder
// destructor
// -----------------------------------------------------------------------------
//
CNssVASDBBuilder::~CNssVASDBBuilder()
    {
    RUBY_DEBUG0( "CNssVASDBBuilder::~CNssVASDBBuilder" );

	if(iContextMgr)
	    {
	    delete iContextMgr;
		iContextMgr = NULL;
	    }
	if(iTagMgr)
	    {
		delete iTagMgr;
		iTagMgr = NULL;
	    }
	if(iDBTagSelectNotifier)
	    {
		delete iDBTagSelectNotifier;
		iDBTagSelectNotifier = NULL;
	    }
	if(iVASDBEventNotifier)
	    {
		delete iVASDBEventNotifier;
		iVASDBEventNotifier = NULL;
	    }
	if(iVasDatabase)
	    {
		iVasDatabase->CloseDatabase();
		delete iVasDatabase;
		iVasDatabase = NULL;
	    }
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::InitializeL
// initialises CNssVASDatabase, by connecting, creating (if not created)
// and opening the datatabase
// -----------------------------------------------------------------------------
//
void CNssVASDBBuilder::InitializeL()
    {
    RUBY_DEBUG_BLOCK( "CNssVASDBBuilder::InitializeL" );
    if( iInitialized )
    	{
    	RUBY_DEBUG0( "CNssVASDBBuilder::InitializeL Already initialized. Ignoring" );
    	return;
    	}

    TInt err = iVasDatabase->CreateDb();
    if ( err != KErrNone )
        {
        User::Leave( err ); // failed to create database
        }

	// open database 
    err = iVasDatabase->OpenDatabase();
    if ( err != KErrNone && err != KErrDbAlreadyOpen )
        {
        RUBY_DEBUG0( "CNssVASDBBuilder::InitializeL ERROR cannot open VAS DB" );
        
        User::Leave( err );
        }
    iInitialized = ETrue;
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::GetContextMgr
// gets the context manager
// -----------------------------------------------------------------------------
//
CNssContextMgr* CNssVASDBBuilder::GetContextMgr()
    {
	return iContextMgr;
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::GetTagMgr
// gets the tag manager
// -----------------------------------------------------------------------------
//
CNssTagMgr* CNssVASDBBuilder::GetTagMgr()
    {
	return  iTagMgr;
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::GetVASDBEventNotifier
// gets the vas db event notifier
// -----------------------------------------------------------------------------
//
CNssVASDBEventNotifier* CNssVASDBBuilder::GetVASDBEventNotifier()
    {
	return iVASDBEventNotifier; 
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::GetTagSelectNotifier
// gets the tag select notifier
// -----------------------------------------------------------------------------
//
CNssDBTagSelectNotifier* CNssVASDBBuilder::GetTagSelectNotifier()
    {
	return iDBTagSelectNotifier; 
    }

// -----------------------------------------------------------------------------
// CNssVASDBBuilder::Close
// closes the database
// -----------------------------------------------------------------------------
//
void CNssVASDBBuilder::Close()
    {
    RUBY_DEBUG0( "CNssVASDBBuilder::Close" );

	if(iVasDatabase)
	    {
		iVasDatabase->CloseDatabase();
	    }
    }

