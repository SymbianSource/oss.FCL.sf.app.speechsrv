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
* Description:  CenRep handling for DevTTS.
*
*/


// INCLUDE FILES
#include "devttscenrep.h"
#include "rubydebug.h"
#include "srsfinternalcrkeys.h"

// CONSTANTS

// Maximum value of the volume setting in Central Repository
const TInt KMaxRepositoryVolume = 10;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDevTtsCenRep::CDevTtsCenRep
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevTtsCenRep::CDevTtsCenRep() 
    {
    // Nothing
    }


// -----------------------------------------------------------------------------
// CDevTtsCenRep::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevTtsCenRep::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "" );
	iRepository = CRepository::NewL( KCRUidSRSFSettings );
    }


// -----------------------------------------------------------------------------
// CDevTtsCenRep::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDevTtsCenRep* CDevTtsCenRep::NewL()
    {
    CDevTtsCenRep* self = new( ELeave ) CDevTtsCenRep();
 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// -----------------------------------------------------------------------------
// CDevTtsCenRep::~CDevTtsCenRep
// Destructor
// -----------------------------------------------------------------------------
//
CDevTtsCenRep::~CDevTtsCenRep()
    {
    delete iRepository;
    }


// -----------------------------------------------------------------------------
// CDevTtsCenRep::Volume
// Returns the volume setting from CenRep
// -----------------------------------------------------------------------------
//
TInt CDevTtsCenRep::Volume( TInt aMaxVolume ) 
    {
    RUBY_DEBUG0( "" );
    TInt volume( aMaxVolume );
    // Get from CenRep
    TInt error = iRepository->Get( KSRSFPlaybackVolume, volume );
    if ( error == KErrNone )
        {
        RUBY_DEBUG2( "Volume repository value:[%d], maxvolume:[%d]", volume, aMaxVolume );
        // Scale value
        volume = ( volume * aMaxVolume ) / KMaxRepositoryVolume;
        RUBY_DEBUG1( "Scaled volume value:[%d]", volume );
        
        return volume;
        }
    // Return max volume by default
    return aMaxVolume;
    }

// End of File  
