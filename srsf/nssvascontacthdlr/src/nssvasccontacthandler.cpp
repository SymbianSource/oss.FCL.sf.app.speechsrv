/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Responsible for maintaining synchronization between the contact DB
*               and the VAS DB
*
*/


// INCLUDE FILES
#include "nssvasccontacthandler.h"
#include "nssvasccontacthandlerimpl.h"
#include "rubydebug.h"

// CONSTANTS
// Thread name
_LIT( KCHThreadName, "NssVasCHThread" );
// Thread priority
const TThreadPriority KCHThreadPriority = EPriorityAbsoluteLow;
// Thread heap min-max
const TInt KCHHeapMinSize = KMinHeapSize;
const TInt KCHHeapMaxSize = 0x300000;

/**
*  Utility class to handle cleanup of ContactHandler thread
*/
NONSHARABLE_CLASS( CContactHandlerStopper ) : public CAsyncOneShot
    {
    public:
        static CContactHandlerStopper* NewL( TInt aPriority );
        virtual ~CContactHandlerStopper();
        void StopCH();
    protected:
        void RunL();
        void DoCancel();
    private:
        CContactHandlerStopper( TInt aPriority );
    };

// ---------------------------------------------------------
// CNssContactHandler::CNssContactHandler
// C++ constructor.
// ---------------------------------------------------------
//       
CNssContactHandler::CNssContactHandler():
    iStopper( NULL )
    {
    // Nothing
    }

// ---------------------------------------------------------
// CNssContactHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CNssContactHandler* CNssContactHandler::NewL()
    {
    RUBY_DEBUG_BLOCK("CNssContactHandler::NewL");
    CNssContactHandler* self = new (ELeave) CNssContactHandler();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CNssContactHandler::ConstructL
// Second-phase constructor.
// ---------------------------------------------------------
//
void CNssContactHandler::ConstructL()
    {
    // Create processing thread
    RThread chThread;

    iThreadNameBuf.Append( _L( "[" ) );
    // Add this pointer to the thread name to get it unique
    iThreadNameBuf.AppendNum( ( TInt ) this );
    iThreadNameBuf.Append( _L( "]" ) );
    iThreadNameBuf.Append( KCHThreadName );

    //RUBY_DEBUG1( "CNssContactHandler::ConstructL starting a thread with name: %S", iThreadNameBuf );

    User::LeaveIfError( chThread.Create( iThreadNameBuf, CHThreadFunction, KDefaultStackSize, KCHHeapMinSize, KCHHeapMaxSize, this ) );

    chThread.SetPriority( KCHThreadPriority );
    chThread.Resume();
    chThread.Close();    
    }

// ---------------------------------------------------------
// CNssContactHandler::~CNssContactHandler
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CNssContactHandler::~CNssContactHandler()
    {
    RUBY_DEBUG0("CNssContactHandlerImplementation::~CNssContactHandler");

    // Check if Contact Handler thread is still alive
    RThread chThread;

    _LIT( KProcessThreadDelimeter, "::" );
    TBuf<256> fullThreadName;
	RProcess currentProcess;
    fullThreadName.Append( currentProcess.FullName() );
    fullThreadName.Append( KProcessThreadDelimeter );
    fullThreadName.Append( iThreadNameBuf );

    if ( chThread.Open( fullThreadName ) == KErrNone )
        {
        // Increase the thread priority to get the cleanup done quickly
        // Fixes priority inversion problem
        chThread.SetPriority( RThread().Priority() );
        RUBY_DEBUG1("CNssContactHandlerImplementation::~CNssContactHandler increased priority to: %d", chThread.Priority() );

        if ( iStopper )
            {
            // Start AO which stops the ActiveScheduler in ContactHandler thread
            // -> ContactHandler thread will clean itself
            iStopper->StopCH();
            iStopper = NULL;

            // Wait that cleanup of ContactHandler thread has been done properly
            TRequestStatus threadCompletion;
            chThread.Rendezvous( threadCompletion );
            User::WaitForRequest( threadCompletion );
            }

        chThread.Kill( KErrNone );

        chThread.Close();
        }
    else
        {
        //RUBY_ERROR1( "CNssContactHandlerImplementation::~CNssContactHandler failed to open thread: %S", iThreadNameBuf );
        }
    }

// ---------------------------------------------------------
// CNssContactHandler::DisableEventHandling
// Disables the event handling.
// ---------------------------------------------------------
//
EXPORT_C void CNssContactHandler::DisableEventHandling()
   {
   RUBY_DEBUG0( "CNssContactHandler::DisableEventHandling" );
   }

// ---------------------------------------------------------
// CNssContactHandler::EnableEventHandling
// Enables event handling
//---------------------------------------------------------
//
EXPORT_C void CNssContactHandler::EnableEventHandling()
   {
   RUBY_DEBUG0( "CNssContactHandler::EnableEventHandling" );
   }

// ---------------------------------------------------------
// CNssContactHandler::SetStopperPointer
// Sets the pointer to stopper object.
//---------------------------------------------------------
//
void CNssContactHandler::SetStopperPointer( CContactHandlerStopper* aStopper )
    {
    iStopper = aStopper;
    }

// ---------------------------------------------------------
// CNssContactHandler::CHThreadFunction
// ContactHandler thread function.
// ---------------------------------------------------------
//
TInt CNssContactHandler::CHThreadFunction( TAny* aParams )
    {
    TInt result = KErrNone;
    // CleanupStack creation
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    // ActiveScheduler creation
    CActiveScheduler* scheduler = NULL;
    TRAPD( error, scheduler = new (ELeave) CActiveScheduler() );
    if ( error == KErrNone )
        {
        // Take created ActiveScheduler into use
        CActiveScheduler::Install( scheduler );

        // Create utility object which is used when stopping this thread
        CNssContactHandler* self = ( CNssContactHandler* )aParams;
        CContactHandlerStopper* stopper = NULL;
        TRAP( error, stopper  = CContactHandlerStopper::NewL( EPriorityNormal ) );
        if ( error == KErrNone )
            {
            self->SetStopperPointer( stopper );

            // Implementation of VAS ContactHandler
            CNssContactHandlerImplementation* impl = NULL;
            TRAP( error, impl = CNssContactHandlerImplementation::NewL() );

            if ( error == KErrNone )
                {
                CActiveScheduler::Start();

                delete impl;
                self->SetStopperPointer( NULL );
                delete stopper;
                delete scheduler;
                delete cleanupStack;

                // Signal that cleanup of ContactHandler thread is now ok
                RThread().Rendezvous( KErrNone );

                result = KErrNone;                
                }  // If created implementation
            else
                {
                RUBY_ERROR1( "CNssContactHandler::CHThreadFunction Failed to create CH implementation. Error [%d]",
                            error );
                            
                self->SetStopperPointer( NULL );
                delete stopper;
                delete scheduler;
                result = error;
                }
                
            // Self pointer not needed anymore
            self = NULL;

            }  // if created stopper
        else 
            {
            RUBY_ERROR1( "CNssContactHandler::CHThreadFunction Failed to create CContactHandlerStopper. Error [%d]", 
                        error );
            delete scheduler;
            result = error;
            }
        
        }  // if created scheduler
   else 
        {
        RUBY_ERROR1( "CNssContactHandler::CHThreadFunction Failed to create the active scheduler. Error [%d]", 
                    error );
        result = error;
        }
    
    return result;
    }

// ---------------------------------------------------------
// CContactHandlerStopper::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CContactHandlerStopper* CContactHandlerStopper::NewL( TInt aPriority )
	{
	CContactHandlerStopper* self = new (ELeave) CContactHandlerStopper( aPriority );
    return self;
    }

// ---------------------------------------------------------
// CContactHandlerStopper::~CContactHandlerStopper
// Desctructor.
// ---------------------------------------------------------
//
CContactHandlerStopper::~CContactHandlerStopper()
	{
    Cancel();
	}

// ---------------------------------------------------------
// CContactHandlerStopper::CContactHandlerStopper
// C++ constructor.
// ---------------------------------------------------------
//
CContactHandlerStopper::CContactHandlerStopper( TInt aPriority ) : 
    CAsyncOneShot( aPriority )
	{
	// Nothing
	}

// ---------------------------------------------------------
// CContactHandlerStopper::StopCH
// Method to request stopping.
// ---------------------------------------------------------
//
void CContactHandlerStopper::StopCH()
    {
    Call();
    }

// ---------------------------------------------------------
// CContactHandlerStopper::RunL
// From CActive.
// ---------------------------------------------------------
//
void CContactHandlerStopper::RunL()
    {
    CActiveScheduler::Stop();
    }

// ---------------------------------------------------------
// CContactHandlerStopper::DoCancel
// From CActive.
// ---------------------------------------------------------
//
void CContactHandlerStopper::DoCancel()
    {
    // Nothing
    }

// End of file
