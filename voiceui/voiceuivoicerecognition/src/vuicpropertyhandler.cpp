/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active object for subscribing to Publish & Suscribe properties
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <voiceuidomainpskeys.h>

#include <vuivoicerecogdefs.h>

#include "vuicpropertyhandler.h"
#include "vuicvoicerecogdialogimpl.h"

#include "rubydebug.h"

// ---------------------------------------------------------
// CPropertyHandler::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
CPropertyHandler* CPropertyHandler::NewL( TUid aFirstCategory,
                                          TUint aFirstKey, 
                                          TUid aSecondCategory,
                                          TUint aSecondKey )
    {
    RUBY_DEBUG_BLOCK( "CPropertyHandler::NewL" );
    
    CPropertyHandler* self = new ( ELeave ) CPropertyHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aFirstCategory, aFirstKey, 
                      aSecondCategory, aSecondKey );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CPropertyHandler::CPropertyHandler
// C++ constructor
// ---------------------------------------------------------
//
CPropertyHandler::CPropertyHandler()
    : CActive( EPriorityStandard )
    {
    }

// ---------------------------------------------------------
// CPropertyHandler::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CPropertyHandler::ConstructL( TUid aFirstCategory, TUint aFirstKey, 
                                   TUid aSecondCategory, TUint aSecondKey )
    {
    RUBY_DEBUG_BLOCK( "CPropertyHandler::ConstructL" );   
 
    iFirstCategory = aFirstCategory;
    iFirstKey = aFirstKey;
    
    /*****************************
    * KPSUidVoiceUiAccMonitor
    ******************************/

    
    TInt err = iProperty.Define( iFirstCategory, iFirstKey,
                                 RProperty::EInt );
    
    RUBY_DEBUG1( "CPropertyHandler::ConstructL - aFirstCategory err[%d]", err );
   
    if ( err == KErrNone || err == KErrAlreadyExists )
        {
        // Set VoiceUI P&S key to ON
        User::LeaveIfError( iProperty.Set( iFirstCategory, iFirstKey, 
                                           KVoiceUiIsOpen ) );
        }

        TInt error = iProperty.Define( aSecondCategory, aSecondKey,
                                       RProperty::EInt );
    
    RUBY_DEBUG1( "CPropertyHandler::ConstructL - aSecondCategory err[%d]", err );
     
    if ( error == KErrNone || error == KErrAlreadyExists )
        {
        // Start to listen RemCon events
        User::LeaveIfError( iProperty.Attach( aSecondCategory, aSecondKey ) );
        }
    
    CActiveScheduler::Add( this );
    // Initial subscription and process current property value
    RunL();

    iRequestIssued = ETrue;
    }

// Destructor
CPropertyHandler::~CPropertyHandler()
    {
    RUBY_DEBUG0( "CPropertyHandler::~CPropertyHandler START" );

    // Set VoiceUI P&S key to OFF
    iProperty.Set( iFirstCategory, iFirstKey, KVoiceUiIsClose );
    
    Cancel();
    iProperty.Close();

    RUBY_DEBUG0( "CPropertyHandler::~CPropertyHandler EXIT" );
    }


// ---------------------------------------------------------
// CPropertyHandler::DoCancel   
// Cancel the request to receive events
// ---------------------------------------------------------
//
void CPropertyHandler::DoCancel()
    {
    RUBY_DEBUG0( "CPropertyHandler::DoCancel START" );
    
    iProperty.Cancel();
    
    RUBY_DEBUG0( "CPropertyHandler::DoCancel EXIT" );
    }


// ---------------------------------------------------------
// CPropertyHandler::RunL
// Handle a change event.
// ---------------------------------------------------------
//
void CPropertyHandler::RunL()
    {
    RUBY_DEBUG_BLOCK( "CPropertyHandler::RunL" ); 
    
    // Resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();

    if ( iRequestIssued && iRecogObserver )
        {
        RUBY_DEBUG0( "CPropertyHandler::RunL - Call HandlePropertyValueChange for RecogObserver" );
        
        TInt value = 0;
        iProperty.Get( value );
        
        iRecogObserver->HandlePropertyValueChange( value );
        }
    else
        {
        RUBY_DEBUG0( "CPropertyHandler::RunL - Pass" );
        }
             
    }

// -----------------------------------------------------------------------------
// CPropertyHandler::Register
// -----------------------------------------------------------------------------
//
void CPropertyHandler::Register( CVoiceRecognitionDialogImpl* aRecogObserver )
    {
    RUBY_DEBUG0( "CPropertyHandler::Register START" );

    if ( aRecogObserver )
        {
        iRecogObserver = aRecogObserver;
        }
        
    RUBY_DEBUG0( "CPropertyHandler::Register EXIT" );        
    }

// -----------------------------------------------------------------------------
// CPropertyHandler::DeRegister
// -----------------------------------------------------------------------------
//
void CPropertyHandler::DeRegister( CVoiceRecognitionDialogImpl* aRecogObserver )
    {
    RUBY_DEBUG0( "CPropertyHandler::DeRegister START" );

    if ( aRecogObserver )
        {
        iRecogObserver = NULL;
        }
        
    RUBY_DEBUG0( "CPropertyHandler::DeRegister EXIT" );
    }    
    

// End of File
