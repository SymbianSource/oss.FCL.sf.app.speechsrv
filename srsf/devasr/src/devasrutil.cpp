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
* Description:  This file contains the implementation of DevASR Utility.
*
*/


// INCLUDE FILES

#include "devasrutil.h"
#include "rubydebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CQueItem::CQueItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CQueItem::CQueItem() : iDataBlock( NULL, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CQueItem::~CQueItem
// Destructor
// -----------------------------------------------------------------------------
//
CQueItem::~CQueItem()
    {
    delete iBuf;
    }

// -----------------------------------------------------------------------------
// CQueItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CQueItem::ConstructL( TUint8* aBuf, TInt aLength )
    {
    RUBY_DEBUG_BLOCK( "CQueItem::ConstructL()" );

    iFinalBlock = EFalse;
    iDataBlock.Set( aBuf, aLength );

    iBuf = aBuf;
    }

// -----------------------------------------------------------------------------
// CQueItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CQueItem* CQueItem::NewL( TUint8* aBuf, TInt aLength )
    {
    CQueItem* self = new ( ELeave ) CQueItem();
    CleanupStack::PushL( self );
    self->ConstructL( aBuf, aLength );
    CleanupStack::Pop();
    return self;
    }
