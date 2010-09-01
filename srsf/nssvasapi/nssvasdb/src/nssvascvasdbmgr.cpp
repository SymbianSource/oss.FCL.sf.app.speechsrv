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
* Description:  CNssVASDBMgr is the starting point to begin interaction with VAS. 
*               The client must first create and Initialize the CNssVASDBMgr; then 
*               request the ContextMgr, TagMgr, VASDBEventNotifier, or 
*               TagSelectNotifier to begin using VAS services. 
*
*/


// INCLUDES

#include <f32file.h>
#include "nssvascvasdbmgr.h"
#include "nssvascvasdbbuilder.h"
#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVASDbMgr::CVASDbMgr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssVASDBMgr::CNssVASDBMgr ()
    {
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::ConstructL
// EPOC constructor
// -----------------------------------------------------------------------------
//
void CNssVASDBMgr::ConstructL()
    {
	iVasDBBuilder = CNssVASDBBuilder::InstanceL();
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::NewL
// 2-phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CNssVASDBMgr* CNssVASDBMgr::NewL()
    {
    RUBY_DEBUG_BLOCK( "CNssVASDBMgr::NewL" );
    
	CNssVASDBMgr* self = NewLC();
	CleanupStack::Pop( self );
	return( self );
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::NewLC
// 2-phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CNssVASDBMgr* CNssVASDBMgr::NewLC()
    {
	CNssVASDBMgr* self = new(ELeave)CNssVASDBMgr;
	CleanupStack::PushL( self );
	self->ConstructL();
	return( self );
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::~CNssVASDBMgr
// 2-phase constructor
// -----------------------------------------------------------------------------
//
CNssVASDBMgr::~CNssVASDBMgr()
    {
    RUBY_DEBUG0( "CNssVASDBMgr::~CNssVASDBMgr" );

    if ( iVasDBBuilder )
        {
        CNssVASDBBuilder::RemoveInstance();
        }
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::InitializeL
// Initializes the VAS DB Manager
// -----------------------------------------------------------------------------
//
#ifdef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY 
EXPORT_C void CNssVASDBMgr::InitializeL()
    {
	iVasDBBuilder->InitializeL();
    }
#else
EXPORT_C TInt CNssVASDBMgr::InitializeL()
    {
	iVasDBBuilder->InitializeL();
	return KErrNone;
    }
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY

// -----------------------------------------------------------------------------
// CVASDbMgr::GetContextMgr
// gets the context manager
// -----------------------------------------------------------------------------
//
EXPORT_C MNssContextMgr* CNssVASDBMgr::GetContextMgr()
    {
	return iVasDBBuilder->GetContextMgr();
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::GetContextMgr
// gets the tag manager
// -----------------------------------------------------------------------------
//
EXPORT_C MNssTagMgr* CNssVASDBMgr::GetTagMgr()
    {
	return  iVasDBBuilder->GetTagMgr();
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::GetContextMgr
// gets the VAS DB Event notifier
// -----------------------------------------------------------------------------
//
EXPORT_C MNssVASDBEventNotifier* CNssVASDBMgr::GetVASDBEventNotifier()
    {
	return iVasDBBuilder->GetVASDBEventNotifier();
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::GetContextMgr
// gets the tag select notifier
// -----------------------------------------------------------------------------
//
EXPORT_C CNssDBTagSelectNotifier* CNssVASDBMgr::GetTagSelectNotifier()
    {
	return iVasDBBuilder->GetTagSelectNotifier();
    }

// -----------------------------------------------------------------------------
// CVASDbMgr::GetContextMgr
// Resets factory models
// -----------------------------------------------------------------------------
//
EXPORT_C void CNssVASDBMgr::ResetFactoryModelsL( MNssResetFactoryModelsClient* aClient )
    {
    User::LeaveIfError( 
        iVasDBBuilder->GetContextMgr()->ResetFactoryModels( aClient )
        );
    }

