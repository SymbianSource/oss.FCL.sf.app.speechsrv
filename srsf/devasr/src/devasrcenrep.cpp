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
* Description:  CenRep handling for DevASR
*
*/


// INCLUDE FILES
#include "devasrcenrep.h"
#include "rubydebug.h"
#include "srsfinternalcrkeys.h"

// CONSTANTS

// Maximum value of the rejection setting in Central Repository
const TInt KMaxRepositoryRejection = 10;

// Maximum value of the rejection setting in the engine
const TInt KMaxEngineRejection = 1024;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDevAsrCenRep::CDevAsrCenRep
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevAsrCenRep::CDevAsrCenRep() 
    {
    // Nothing
    }


// -----------------------------------------------------------------------------
// CDevAsrCenRep::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevAsrCenRep::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "" );
	iRepository = CRepository::NewL( KCRUidSRSFSettings );
    }


// -----------------------------------------------------------------------------
// CDevAsrCenRep::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDevAsrCenRep* CDevAsrCenRep::NewL()
    {
    CDevAsrCenRep* self = new( ELeave ) CDevAsrCenRep();
 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// -----------------------------------------------------------------------------
// CDevAsrCenRep::~CDevAsrCenRep
// Destructor
// -----------------------------------------------------------------------------
//
CDevAsrCenRep::~CDevAsrCenRep()
    {
    delete iRepository;
    }


// -----------------------------------------------------------------------------
// CDevAsrCenRep::RejectionValue
// Returns the rejection setting from CenRep
// -----------------------------------------------------------------------------
//
TInt CDevAsrCenRep::RejectionValue() 
    {
    RUBY_DEBUG0( "" );
    TInt rejection( KMaxEngineRejection );
    // Get from CenRep
    TInt error = iRepository->Get( KSRSFRejection, rejection );
    if ( error == KErrNone )
        {
        RUBY_DEBUG1( "Rejection repository value:[%d]", rejection );
        // Scale value
        rejection = ( rejection * KMaxEngineRejection ) / KMaxRepositoryRejection;
        RUBY_DEBUG1( "Scaled rejection value:[%d]", rejection );
        }
    return rejection;
    }

// End of File  
