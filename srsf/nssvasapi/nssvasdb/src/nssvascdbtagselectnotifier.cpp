/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssDBTagSelectNotifier provides tag select notification to 
*               its clients (observers). It itslef receives tag select notification
*               from Recognition Handler.
*
*/


// includes
#include "nssvascdbtagselectnotifier.h"
#include "nssvascoreconstant.h"
#include <e32property.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::CNssDBTagSelectNotifier
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------
//
CNssDBTagSelectNotifier::CNssDBTagSelectNotifier(): CActive( EPriorityStandard )
    {
    // Nothing
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::CNssDBTagSelectNotifier
// overloaded constructor
// ---------------------------------------------------------
//
CNssDBTagSelectNotifier::CNssDBTagSelectNotifier( CNssVASDatabase* aVasDatabase ): 
                         CActive( EPriorityStandard ), iVasDatabase( aVasDatabase )
    {
    // Nothing
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::ConstructL
// symbian constructor
// ---------------------------------------------------------
//
void CNssDBTagSelectNotifier::ConstructL()
    {
	iObserverList = new (ELeave) CArrayPtrFlat<MNssDBTagSelectNotifierClient>(1);
	
    // Define P&S properties
    TInt err = RProperty::Define( KSINDUID, EVoiceTagSelectionTagId, RProperty::EInt );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    err = RProperty::Define( KSINDUID, EVoiceTagSelectionContextId, RProperty::EInt );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
        
    User::LeaveIfError( iProperty.Attach( KSINDUID, EVoiceTagSelectionTagId ) );
    CActiveScheduler::Add( this );
    // Subsrcibe for change notifications
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::NewL
// 2 phase Construction
// ---------------------------------------------------------
//
CNssDBTagSelectNotifier* CNssDBTagSelectNotifier::NewL( CNssVASDatabase* aVasDatabase )
    {
	CNssDBTagSelectNotifier* self = NewLC( aVasDatabase );
	CleanupStack::Pop( self );
	return ( self );
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::NewLC
// 2 phase Construction
// ---------------------------------------------------------
//
CNssDBTagSelectNotifier* CNssDBTagSelectNotifier::NewLC( CNssVASDatabase* aVasDatabase )
    {
	CNssDBTagSelectNotifier* self = new ( ELeave )CNssDBTagSelectNotifier( aVasDatabase );
	CleanupStack::PushL( self );
	self->ConstructL();
	return ( self );
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::~CNssDBTagSelectNotifier
// destructor
// ---------------------------------------------------------
//
CNssDBTagSelectNotifier::~CNssDBTagSelectNotifier()
    {
	if ( iObserverList )
	    {
		iObserverList->ResetAndDestroy();
		delete iObserverList;
		iObserverList = NULL;
    	}
    Cancel();
    iProperty.Close();
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::RegisterForNotification
// registers a client for notification 
// ---------------------------------------------------------
//
TInt CNssDBTagSelectNotifier::RegisterForNotification( MNssDBTagSelectNotifierClient* aObserver )
    {
    TRAPD( err, iObserverList->AppendL( aObserver ) );
    return err;
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::Deregister
// Deregister a client from notification
// ---------------------------------------------------------
//
TInt CNssDBTagSelectNotifier::Deregister( MNssDBTagSelectNotifierClient* aObserver )
    {
    TBool ret = EFalse;
    if ( iObserverList )
	      {
		    for ( TInt i = 0;i < iObserverList->Count(); i++ )
			    if ( (*iObserverList)[i] == aObserver )
			        {
				      iObserverList->Delete( i );
				      ret = ETrue;
			        }
    	   }
	  if ( ret )
	      {
		    iObserverList->Compress();
	      }
	  return (ret ? KErrNone : KErrGeneral);
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::HandleSelection
// calls all its clients, when notified of a tag slection.
// ---------------------------------------------------------
//
void CNssDBTagSelectNotifier::HandleSelection( CNssTag* aTag )
    {
    CNssContext* context = ( CNssContext* ) aTag->Context();
    RProperty::Set( KSINDUID, EVoiceTagSelectionContextId, context->ContextId() );
    RProperty::Set( KSINDUID, EVoiceTagSelectionTagId, aTag->TagId() );
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::DoHandleSelectionCallback
// Forwards the tag selection callback
// ---------------------------------------------------------
//
void CNssDBTagSelectNotifier::DoHandleSelectionCallback( TInt aTagId, TInt aContextId )
    {
    if ( iObserverList->Count() )
        {
        // Go through all observers that have been registered to this instance
        for ( TInt i = 0; i < iObserverList->Count(); i++ )
            {
            // Find the tags based on ID
            MNssTagListArray* array = iVasDatabase->GetTag( aTagId );
            
            for ( TInt j = 0; j < array->Count(); j++ )
                {
                CNssTag* tag = ( CNssTag* )array->At( j );
                CNssContext* context = ( CNssContext* ) tag->Context();
                // Check that context IDs match
                if ( context->ContextId() == aContextId )
                    {
                    // Do the callback
                    (*iObserverList)[i]->HandleSelection( tag );
                    }
                }
                       
            array->ResetAndDestroy();
            delete array;
	        }
        }
    }
    
// ---------------------------------------------------------
// CNssDBTagSelectNotifier::DoCancel
// From CActive
// ---------------------------------------------------------
//
void CNssDBTagSelectNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------
// CNssDBTagSelectNotifier::RunL
// From CActive
// ---------------------------------------------------------
//
void CNssDBTagSelectNotifier::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();

    // property updated, get new value
   iProperty.Get( KSINDUID, EVoiceTagSelectionTagId, iTagId );
   iProperty.Get( KSINDUID, EVoiceTagSelectionContextId, iContextId );
   DoHandleSelectionCallback( iTagId, iContextId );
   }
   
// End of file
