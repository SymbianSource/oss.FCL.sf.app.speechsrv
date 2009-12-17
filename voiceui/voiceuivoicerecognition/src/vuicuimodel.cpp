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
* Description:  
*
*/


// INCLUDE FILES

#include "vuicuimodel.h"
#include "vuicglobalnote.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CUiModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUiModel* CUiModel::NewL()
    {
    RUBY_DEBUG_BLOCK( "CUiModel::NewL" );
    
    CUiModel* self = new (ELeave) CUiModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    } 

// Destructor       
CUiModel::~CUiModel()
    {
    RUBY_DEBUG0( "CUiModel::~CUiModel START" );
    
    delete iNote;   
    
    RUBY_DEBUG0( "CUiModel::~CUiModel EXIT" );
    }

// ---------------------------------------------------------
// CUiModel::Note
// ---------------------------------------------------------
//  
CGlobalNote& CUiModel::Note() const
    {
    RUBY_DEBUG0( "CUiModel::Note" );
    
    return *iNote;
    }

// ---------------------------------------------------------
// CUiModel::CUiModel
// ---------------------------------------------------------
//              
CUiModel::CUiModel()
    {
    }

// ---------------------------------------------------------
// CUiModel::ConstructL
// ---------------------------------------------------------
//           
void CUiModel::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CUiModel::ConstructL" );

    iNote = CGlobalNote::NewL();
    }

    
// End of File

