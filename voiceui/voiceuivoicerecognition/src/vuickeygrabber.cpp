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
* Description:  
*
*/


// INCLUDE FILES

#include <eikon.hrh>
#include <apgwgnam.h>

#include "vuickeygrabber.h"

#include "vuimkeycallback.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CKeyGrabber::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CKeyGrabber* CKeyGrabber::NewL( MKeyCallback* aKeyObserver )
    {
    CKeyGrabber* self = new (ELeave) CKeyGrabber( aKeyObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }   
    
// ---------------------------------------------------------
// CKeyGrabber::~CKeyGrabber
// ---------------------------------------------------------
//       
CKeyGrabber::~CKeyGrabber()
    {
    Cancel();

    iWindowGroup.Close();
    
    iWsSession.Close();
    }

// ---------------------------------------------------------
// CKeyGrabber::DoCancel
// ---------------------------------------------------------
// 
void CKeyGrabber::DoCancel()
    {
    RUBY_DEBUG0( "CKeyGrabber::DoCancel START" );
    
    iWsSession.EventReadyCancel();
    
    iWindowGroup.CancelCaptureKey( iCameraHandle );
    iWindowGroup.CancelCaptureKey( iPocHandle );
    iWindowGroup.CancelCaptureKey( iPowerHandle );
    iWindowGroup.CancelCaptureKey( iApplicationHandle );
    
    RUBY_DEBUG0( "CKeyGrabber::DoCancel EXIT" );
    }

// ---------------------------------------------------------
// CKeyGrabber::RunL
// ---------------------------------------------------------
//     
void CKeyGrabber::RunL()
    {
    RUBY_DEBUG_BLOCK( "CKeyGrabber::RunL" );
    
    if ( iStatus.Int() == KErrNone )
        {
        TWsEvent event;
        iWsSession.GetEvent( event );
        
        iWsSession.EventReady( &iStatus );
        SetActive();
        
        //fix for ESLI-7YSA3 we terminate Vcommand when received EKeyNull.
        if ( event.Key()->iCode == EKeyNull )
        	iKeyObserver->HandleKeypressL( event.Key()->iCode );
        if ( event.Key()->iCode == EKeyCamera ||
             event.Key()->iCode == EKeyPoC ||
             event.Key()->iCode == EKeyPowerOff ||
             event.Key()->iCode == EKeyApplication )
            {
            iWsSession.SendEventToOneWindowGroupsPerClient( event );

            iKeyObserver->HandleKeypressL( event.Key()->iCode );
            }
        }
    }

// ---------------------------------------------------------
// CKeyGrabber::CKeyGrabber
// ---------------------------------------------------------
//              
CKeyGrabber::CKeyGrabber( MKeyCallback* aKeyObserver )
 : CActive( EPriorityStandard ), iKeyObserver( aKeyObserver )
    {
    }
    
// ---------------------------------------------------------
// CKeyGrabber::ConstructL
// ---------------------------------------------------------
//           
void CKeyGrabber::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CKeyGrabber::ConstructL" );

    User::LeaveIfError( iWsSession.Connect() );

    iWindowGroup = RWindowGroup ( iWsSession );
    iWindowGroup.Construct( (TUint32) &iWindowGroup, EFalse );
    
    User::LeaveIfError( iCameraHandle = iWindowGroup.CaptureKey( EKeyCamera, 0, 0 ) );
    User::LeaveIfError( iPocHandle = iWindowGroup.CaptureKey( EKeyPoC, 0, 0 ) );
    User::LeaveIfError( iPowerHandle = iWindowGroup.CaptureKey( EKeyPowerOff, 0, 0 ) );
    User::LeaveIfError( iApplicationHandle = iWindowGroup.CaptureKey( EKeyApplication, 0, 0 ) );
    
    iWindowGroup.SetOrdinalPosition( -1 );
    iWindowGroup.EnableReceiptOfFocus( EFalse );
    
    CApaWindowGroupName* name = CApaWindowGroupName::NewLC( iWsSession );
    name->SetHidden( ETrue );
    name->SetWindowGroupName( iWindowGroup );
    CleanupStack::PopAndDestroy( name );

    iWsSession.EventReady( &iStatus );
    
    CActiveScheduler::Add( this );
    SetActive();
    }

    
// End of File

