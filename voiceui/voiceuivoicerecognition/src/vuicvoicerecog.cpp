/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


// INCLUDE FILES
#include "vuicvoicerecog.h"

#include "rubydebug.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVoiceRecog::CVoiceRecog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVoiceRecog::CVoiceRecog()
    {
    }

// -----------------------------------------------------------------------------
// CVoiceRecog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVoiceRecog::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVoiceRecog::ConstructL" );
    }

// ---------------------------------------------------------------------------
// CVoiceRecog::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoiceRecog* CVoiceRecog::NewLC()
    {
    CVoiceRecog* self = new (ELeave) CVoiceRecog();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CVoiceRecog::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVoiceRecog* CVoiceRecog::NewL() 
    {   
    CVoiceRecog* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CVoiceRecog::~CVoiceRecog()
    {
    RUBY_DEBUG0( "CVoiceRecog::~CVoiceRecog START" );

    RUBY_DEBUG0( "CVoiceRecog::~CVoiceRecog EXIT" );  
    }

//  End of File
