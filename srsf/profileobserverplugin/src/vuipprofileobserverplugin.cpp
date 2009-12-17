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
* Description:  profile observer vuipf-plugin
*
*/


#include "vuipprofileobserverplugin.h"
#include "vcommandprofileobserver.h"
#include "rubydebug.h"

#include <ProEngFactory.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVUIPprofileobserverplugin::CVUIPprofileobserverplugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVUIPprofileobserverplugin::CVUIPprofileobserverplugin()
    {
    }
   
 
// -----------------------------------------------------------------------------
// CVUIPprofileobserverplugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVUIPprofileobserverplugin::ConstructL() 
    {
    RUBY_DEBUG_BLOCK( "CVUIPprofileobserverplugin::ConstructL()" );
    
    iProfileObserver = CVCommandProfileObserver::NewL();
    }
 
   
// -----------------------------------------------------------------------------
// CVUIPprofileobserverplugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVUIPprofileobserverplugin* CVUIPprofileobserverplugin::NewL() 
    {
    RUBY_DEBUG_BLOCK( "CVUIPprofileobserverplugin::NewL()" );
    
    CVUIPprofileobserverplugin* self = new( ELeave ) CVUIPprofileobserverplugin();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    
    return self;
    }


// -----------------------------------------------------------------------------
// CVUIPprofileobserverplugin::~CVUIPprofileobserverplugin
// Destructor.
// -----------------------------------------------------------------------------
//
CVUIPprofileobserverplugin::~CVUIPprofileobserverplugin()
    {
    RUBY_DEBUG0( "CVUIPprofileobserverplugin::~CVUIPprofileobserverplugin()" );
    
    delete iProfileObserver;
    }


// -----------------------------------------------------------------------------
// CVUIPprofileobserverplugin::InitializeL
// Handle voice event
// -----------------------------------------------------------------------------
//

void CVUIPprofileobserverplugin::InitializeL( MVoiceEventExecutor& /*aEventHandler*/ )
    {
    }


// End of File

