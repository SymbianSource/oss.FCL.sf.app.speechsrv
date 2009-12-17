/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CVASDBMonitor class is an active object that is a wrapper around 
*               the RDBNotifier class. Events from RDBNotifier are handled by 
*               encapsulating them in a CNssVASDBEvent object and passing it to all 
*               its observers. In addition to the events provided by RDBNotifier, 
*               CVASDBMonitor also provides additional events when a entry is added,
*               changed or deleted from the VAS DB. CVASDBMonitor owns the list of 
*               observers of the VAS DB.
*
*/


// INCLUDE FILES
#include "nssvascresourcehandler.h"
#include "nssvascvasdbeventmonitor.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssVASDBEventMonitor::ConstructL
// C++ constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
//
CNssVASDBEventMonitor::CNssVASDBEventMonitor() : CActive( CActive::EPriorityStandard) 
    {
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::ConstructL
// EPOC constructor can leave.
// ---------------------------------------------------------
//
void CNssVASDBEventMonitor::ConstructL()
    {
    iObservers = new ( ELeave ) CArrayPtrFlat<MNssVASDatabaseObserver>( 1 );

    TFileName serverSideFilename;
   
    User::LeaveIfError( iDbSession.Connect() );

    // Open the database.
    User::LeaveIfError( iDatabase.Open( iDbSession, KVasDatabaseName, KVasDatabaseFormatString ) );

    User::LeaveIfError( iNotifier.Open( iDatabase ) );
    StartMonitoring();
    }

// ---------------------------------------------------------
// CNssVASDBEventMonitor::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CNssVASDBEventMonitor* CNssVASDBEventMonitor::NewL()
    {
    CNssVASDBEventMonitor* self = new (ELeave) CNssVASDBEventMonitor;
      
    CleanupStack::PushL( self );
	CActiveScheduler::Add( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::~CNssVASDBEventMonitor
// Destructor
// ---------------------------------------------------------
//
CNssVASDBEventMonitor::~CNssVASDBEventMonitor()
    {
    Cancel();
    iNotifier.Close();
    iDatabase.Close();
    iDbSession.Close();
    if ( iObservers )
        {
        iObservers->Reset();
        delete iObservers;
        }
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::StartMonitoring
// Request function to start monitoring
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventMonitor::StartMonitoring()
    {
    if ( IsActive() )
        {
		return;
        }
	iStatus=KRequestPending;
	SetActive();
	iNotifier.NotifyChange( iStatus );
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::AddObserverL
// add observer to its list
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventMonitor::AddObserverL(MNssVASDatabaseObserver* aObserver)
    {
	iObservers->AppendL( aObserver );	
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::RemoveObserver
// remove observer from its list
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CNssVASDBEventMonitor::RemoveObserver(MNssVASDatabaseObserver* aObserver)
    {
	TBool ret = EFalse;
	if ( iObservers )
	    {
		for ( TInt i = 0;i < iObservers->Count(); i++ )
		    {
			if ( ( *iObservers )[i] == aObserver )
			    {
				iObservers->Delete( i );
				ret = ETrue;
				break;
			    }
		    }
	    }
	if ( ret )
    	{
		iObservers->Compress();
	    }
	return ( ret? KErrNone : KErrGeneral );			
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::RunL
// called when standard symbian DBMS events occur
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventMonitor::RunL()
    {
	RDbNotifier::TEvent event = STATIC_CAST( RDbNotifier::TEvent,iStatus.Int() );
	HandleDatabaseEventL( event );
	StartMonitoring();
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::DoCancel
// cancel outstanding requests to RDBNotifier
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventMonitor::DoCancel()
    {
	iNotifier.Cancel();
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::HandleDatabaseEventL
// Standard Symbian DBMS events handling
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventMonitor::HandleDatabaseEventL( RDbNotifier::TEvent aEvent )
    {
	 if(!iObservers->Count())
	    {
		return;
	    }

	 CNssVASDBEvent::TVASDBEventType type = CNssVASDBEvent::EVASDatabaseEventNull;
	 switch( aEvent )
    	 {
    	 case RDbNotifier::EClose: 
    	    type = CNssVASDBEvent::EVASDatabaseEventDatabaseClose;
    	    break; 
    	 case RDbNotifier::EUnlock:
	    	 type = CNssVASDBEvent::EVASDatabaseEventDatabaseUnlock;
		     break;
		 case RDbNotifier::ECommit:
	        type = CNssVASDBEvent::EVASDatabaseEventDatabaseCommit;
		    break;
		 case RDbNotifier::ERollback:
		    type = CNssVASDBEvent::EVASDatabaseEventDatabaseRollback;
		    break;
		 case RDbNotifier::ERecover:
		    type = CNssVASDBEvent::EVASDatabaseEventDatabaseRecover;
		    break;
		 default:
		    break;
    	 }
	 // for standard symbian DBMS events, the parameter aTag
	 // does not have any significance, so create a dummy one
	 MNssTag *dummyTag = NULL;
	 CNssVASDBEvent *vasDBEvent = CNssVASDBEvent::NewL( type, dummyTag );

	 for(TInt i = 0; i < iObservers->Count(); i++)
	     {
		 ( *iObservers )[i]->HandleVASDBEvent( vasDBEvent );
	     }
    }


// ---------------------------------------------------------
// CNssVASDBEventMonitor::HandleDatabaseEventL()
// handling of addional events ( tag added, tag deleted, tag changed)
// wk 15 release has limited functionality where these events can be 
// reported only when the events occur on the same thread as that 
// of the observer (client monitoring the event).
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventMonitor::HandleDatabaseEventL( 
                    CNssVASDBEvent::TVASDBEventType type, CNssTag *aTag )
{
	 if ( !iObservers->Count() )
	    {
    	return;
	    }

	 for ( TInt i = 0; i < iObservers->Count(); i++ )
	 {
		 CNssVASDBEvent *vasDBEvent = CNssVASDBEvent::NewL( type, aTag );
		 ( *iObservers )[i]->HandleVASDBEvent( vasDBEvent );
	 }
}

//  End of File  
