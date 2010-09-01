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
* Description:  Class which hides the dynamic loading of nssttsutility.dll
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <nssttsutilitybase.h>
#include "rubydebug.h"
#include "nssvasttsutilitywrapper.h"

// CONSTANTS
// DLL name
_LIT( KUtilityFilename, "nssttsutility.dll" );
// CreateInstanceL function ordinal number
const TInt KFunctionOrdinal = 1;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::NewL
// Two-phased constructor
// ---------------------------------------------------------
//
CNssTtsUtilityWrapper* CNssTtsUtilityWrapper::NewL( MTtsClientUtilityObserver& aObserver,
                                                    TInt aPriority,
                                                    TMdaPriorityPreference aPref )
    {
    CNssTtsUtilityWrapper* self = new ( ELeave ) CNssTtsUtilityWrapper( 
                                                 aObserver, aPriority, aPref );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::~CNssTtsUtilityWrapper
// Destructor
// ---------------------------------------------------------
//
CNssTtsUtilityWrapper::~CNssTtsUtilityWrapper()
    {
    RUBY_DEBUG0( "CNssTtsUtilityWrapper::~CNssTtsUtilityWrapper" );
    if ( IsActive() )
        {
        Cancel();
        }
    delete iUtility;
    // Close RLibrary handle    
    iLib.Close();
    }

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::CNssTtsUtilityWrapper
// C++ constructor
// ---------------------------------------------------------
//
CNssTtsUtilityWrapper::CNssTtsUtilityWrapper( MTtsClientUtilityObserver& aObserver,
                                              TInt aPriority,
                                              TMdaPriorityPreference aPref ) :
                                              CActive( EPriorityStandard ),
                                              iObserver( aObserver ),
                                              iPriority( aPriority ),
                                              iPref( aPref )
    {
    // Nothing
    }

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::ConstructL
// Second phase constructor
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CNssTtsUtilityWrapper::ConstructL" );
    
    // Load a DLL based on name with RLibrary    
    User::LeaveIfError( iLib.Load( KUtilityFilename ) );
    
    // Find the TAny* CreateInstanceL() function    
    TLibraryFunction entry = iLib.Lookup( KFunctionOrdinal );
    if ( !entry )
        {
        User::Leave( KErrNotFound );
        }
    
    // Call CreateInstanceL
    // Cannot use static_cast since entry() call returns TInt
    iUtility = ( MTtsUtilityBase* ) entry();
    
    // Do the 2nd phase construction
    iUtility->CreateInstanceSecondPhaseL( *this );
    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::AddStyleL
// Forwards call to TTS Utility
// ---------------------------------------------------------
//
TTtsStyleID CNssTtsUtilityWrapper::AddStyleL( const TTtsStyle& aStyle )
    {
    return iUtility->AddStyleL( aStyle );
    }
        
// ---------------------------------------------------------
// CNssTtsUtilityWrapper::OpenAndPlayParsedTextL
// Forwards call to TTS Utility
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::OpenAndPlayParsedTextL( CTtsParsedText& aText, 
                                                    TInt /*aPriority*/, 
                                                    TMdaPriorityPreference /*aPref*/ )
    {
    iUtility->OpenAndPlayParsedTextL( aText );
    }
                                     
// ---------------------------------------------------------
// CNssTtsUtilityWrapper::Stop
// Forwards call to TTS Utility
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::Stop()
    {
    iUtility->Stop();
    }
        
// ---------------------------------------------------------
// CNssTtsUtilityWrapper::Play
// Forwards call to TTS Utility
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::Play()
    {
    iUtility->Play();
    }
        
// ---------------------------------------------------------
// CNssTtsUtilityWrapper::Close
// Forwards call to TTS Utility
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::Close()
    {
    iUtility->Close();
    }

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::MapcCustomCommandEvent
// Callback from TTS Utility
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::MapcCustomCommandEvent( TInt /*aEvent*/, 
                                                    TInt /*aError*/ )
    {
    // Nothing
    }
	    
// ---------------------------------------------------------
// CNssTtsUtilityWrapper::MapcInitComplete
// Callback from TTS Utility
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::MapcInitComplete( TInt aError, 
                                              const TTimeIntervalMicroSeconds& aDuration )
    {
    RUBY_DEBUG1( "CNssTtsUtilityWrapper::MapcInitComplete, aError[%d]", aError );    
    iError = aError;
    iDuration = aDuration;
    iState = EInitComplete;
    Ready();
    }
    
// ---------------------------------------------------------
// CNssTtsUtilityWrapper::MapcPlayComplete
// Callback from TTS Utility
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::MapcPlayComplete( TInt aError )
    {
    RUBY_DEBUG1( "CNssTtsUtilityWrapper::MapcPlayComplete, aError[%d]", aError );       
    iError = aError;
    iState = EPlayComplete;
    Ready();
    }

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::Ready
// Sets active object ready to be run
// ---------------------------------------------------------
//    
void CNssTtsUtilityWrapper::Ready()
    {
    if ( !IsActive() )
        {
        SetActive();
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }
    }

// ---------------------------------------------------------
// CNssTtsUtilityWrapper::RunL
// From CActive
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::RunL()
    {
    // Forwards callback to other parts of VAS    
    RUBY_DEBUG_BLOCK( "CNssTtsUtilityWrapper::RunL" );    
    switch ( iState )
        {
        case EInitComplete:
            iObserver.MapcInitComplete( iError, iDuration );
            break;
            
        case EPlayComplete:
            iObserver.MapcPlayComplete( iError );
            break;
            
        default:
            RUBY_ERROR1( "CNssTtsUtilityWrapper::RunL unexpected state %d", iState );
            break;
        }
    }
	    
// ---------------------------------------------------------
// CNssSiUtilityWrapper::DoCancel
// From CActive
// ---------------------------------------------------------
//
void CNssTtsUtilityWrapper::DoCancel()
    {
    // Nothing
    }    

// End of file
