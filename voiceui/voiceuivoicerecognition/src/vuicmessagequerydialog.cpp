/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "vuicmessagequerydialog.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CMessageQueryDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMessageQueryDialog* CMessageQueryDialog::NewL( CMessageQueryDialog** aSelf )
    {
    CMessageQueryDialog* self = new (ELeave) CMessageQueryDialog( aSelf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }   
    
// Destructor       
CMessageQueryDialog::~CMessageQueryDialog()
    {
    RUBY_DEBUG0( "CMessageQueryDialog::~CMessageQueryDialog START" );

    MakeVisible( EFalse );

    if ( iSelf )
        {
        *iSelf = NULL;
        iSelf = NULL;
        }

    RUBY_DEBUG0( "CMessageQueryDialog::~CMessageQueryDialog EXIT" );
    }

// ---------------------------------------------------------
// CMessageQueryDialog::CMessageQueryDialog
// ---------------------------------------------------------
//              
CMessageQueryDialog::CMessageQueryDialog( CMessageQueryDialog** aSelf )
 : CAknMessageQueryDialog(), iSelf( aSelf )
    {
    }

// ---------------------------------------------------------
// CMessageQueryDialog::ConstructL
// ---------------------------------------------------------
//           
void CMessageQueryDialog::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CMessageQueryDialog::ConstructL" );
    }
    
// End of File

