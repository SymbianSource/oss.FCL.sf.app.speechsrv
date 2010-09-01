/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Speech synthesis server utility classes
*
*/


// INCLUDES

#include <swi/swispubsubdefs.h>         // For listening sw installer's actions
#include <UikonInternalPSKeys.h>    // For listening memory card insertion/removal

#include "speechsynthesisserverutilities.h"
#include "rubydebug.h"


// ----------------------------------------------------------------------------
// PanicClient
//
// ----------------------------------------------------------------------------
//
void PanicClient( const RMessagePtr2& aMessage, TSpeechSynthesisPanic aPanic )
    {
    aMessage.Panic( KPanic, aPanic );
    }
    
    
// ----------------------------------------------------------------------------
// CShutdown::NewL
// 
// ----------------------------------------------------------------------------
CShutdown* CShutdown::NewL()
    {
    CShutdown* self = new(ELeave) CShutdown();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CShutdown::CShutdown
// 
// ----------------------------------------------------------------------------
inline CShutdown::CShutdown():
    CTimer( EPriorityIdle ) 
    {
    CActiveScheduler::Add( this );
    }
    
// ----------------------------------------------------------------------------
// CShutdown::ConstructL
// 
// ----------------------------------------------------------------------------
//
inline void CShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

// ----------------------------------------------------------------------------
// CShutdown::Start
// 
// ----------------------------------------------------------------------------
//
void CShutdown::Start()
    {
    RUBY_DEBUG0( "SpeechSynthesisServer: starting shutdown timeout" );
    
    After( KSpeechSynthesisShutdownDelay );
    }

// ----------------------------------------------------------------------------
// CShutdown::RunL
//
// ----------------------------------------------------------------------------
//
void CShutdown::RunL()
    {
    RUBY_DEBUG0( "SpeechSynthesisServer: server timeout ... closing" );
    
    CActiveScheduler::Stop();
    }


// ----------------------------------------------------------------------------
// CConfigurationListener::NewL
//
// ----------------------------------------------------------------------------
//
CConfigurationListener* CConfigurationListener::NewL( MConfigurationObserver& aHost )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    CConfigurationListener* self = new(ELeave) CConfigurationListener( aHost );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self);
    
    return self;
    }

// ----------------------------------------------------------------------------
// CConfigurationListener::CConfigurationListener
//
// ----------------------------------------------------------------------------
//
CConfigurationListener::CConfigurationListener(  MConfigurationObserver& aHost ) :
    CActive( CActive::EPriorityLow ),
    iHost( aHost )
    {
    // Nothing 
    }

// ----------------------------------------------------------------------------
// CConfigurationListener::ConstructL
//
// ----------------------------------------------------------------------------
//
void CConfigurationListener::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach( KUidSystemCategory, 
                                          Swi::KUidSoftwareInstallKey ) );
    
    CActiveScheduler::Add( this );
    
    // Initial subscription
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CConfigurationListener::~CConfigurationListener
//
// ----------------------------------------------------------------------------
//
CConfigurationListener::~CConfigurationListener()
    {
    RUBY_DEBUG0( "Destructing.." );
    Cancel();
    iProperty.Close();
    }

// ----------------------------------------------------------------------------
// CConfigurationListener::DoCancel
//
// ----------------------------------------------------------------------------
//
void CConfigurationListener::DoCancel()
    {
    RUBY_DEBUG0( "DoCancel" )
    iProperty.Cancel();
    }

// ----------------------------------------------------------------------------
// CConfigurationListener::RunL
//
// ----------------------------------------------------------------------------
//
void CConfigurationListener::RunL()
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // Resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();

    // Property updated, get new value
    TInt value( 0 );
    if ( iProperty.Get( value ) == KErrNone )
        {
        TInt operation( value & Swi::KSwisOperationMask );
        TInt operationStatus( value & Swi::KSwisOperationStatusMask );
        
        RUBY_DEBUG1( "value = %d", value );
        RUBY_DEBUG2( "operation %d, status %d", operation, operationStatus );
        
        if ( operationStatus == Swi::ESwisStatusSuccess &&
                ( operation == Swi::ESwisInstall ||
                  operation == Swi::ESwisUninstall ||
                  operation == Swi::ESwisRestore ) )
            {
            RUBY_DEBUG0( "Update needed!" );
            iHost.McoConfigurationChanged();
            }
        }
    }


// ----------------------------------------------------------------------------
// CMMCListener::NewL
//
// ----------------------------------------------------------------------------
//
CMMCListener* CMMCListener::NewL( MConfigurationObserver& aHost )
    {
    RUBY_DEBUG_BLOCK( "" );
    
    CMMCListener* self = new(ELeave) CMMCListener( aHost );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self);
    
    return self;
    }

// ----------------------------------------------------------------------------
// CMMCListener::CMMCListener
//
// ----------------------------------------------------------------------------
//
CMMCListener::CMMCListener(  MConfigurationObserver& aHost ) :
    CActive( CActive::EPriorityLow ),
    iPrevValue( -1 ),
    iHost( aHost )
    {
    // Nothing 
    }

// ----------------------------------------------------------------------------
// CMMCListener::ConstructL
//
// ----------------------------------------------------------------------------
//
void CMMCListener::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach( KPSUidUikon, KUikMMCInserted ) );
    
    CActiveScheduler::Add( this );
    
    // Initial subscription
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CMMCListener::~CMMCListener
//
// ----------------------------------------------------------------------------
//
CMMCListener::~CMMCListener()
    {
    RUBY_DEBUG0( "Destructing.." );
    Cancel();
    iProperty.Close();
    }

// ----------------------------------------------------------------------------
// CMMCListener::DoCancel
//
// ----------------------------------------------------------------------------
//
void CMMCListener::DoCancel()
    {
    RUBY_DEBUG0( "DoCancel" )
    iProperty.Cancel();
    }

// ----------------------------------------------------------------------------
// CMMCListener::RunL
//
// ----------------------------------------------------------------------------
//
void CMMCListener::RunL()
    {
    RUBY_DEBUG_BLOCK( "" );
    
    // Resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt value( 0 );
    if ( iProperty.Get( value ) == KErrNone )
        {        
        RUBY_DEBUG1( "value = %d", value );
        
        // Extra calls coming -> check if status is changed
        if ( iPrevValue != value )
            {
            iPrevValue = value; 
            
            iHost.McoConfigurationChanged();
            }
        }
    }

// End of file
