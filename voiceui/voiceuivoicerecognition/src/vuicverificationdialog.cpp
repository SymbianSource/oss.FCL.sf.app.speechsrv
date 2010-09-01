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
* Description:  
*
*/


// INCLUDE FILES
#include <featmgr.h>

#include <AknMediatorFacade.h>

#include <SecondaryDisplay/AknSecondaryDisplayDefs.h>

#include <vuivoicerecognition.rsg>
#include <vuivoicerecogdefs.h>

#include "vuivoicerecognition.hrh"

#include "vuiclistquerydialog.h"
#include "vuicverificationdialog.h"

#include "vuimkeycallback.h"

#include "rubydebug.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVerificationDialog::CVerificationDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVerificationDialog::CVerificationDialog() 
 : iCurrentIndex( 0 )
    {
    }


// Destructor
CVerificationDialog::~CVerificationDialog()
    {
    RUBY_DEBUG0( "CVerificationDialog::~CVerificationDialog START" );
     
    RegisterForKeyCallback( NULL );
    
    delete iDialog;
    iDialog = NULL;
    
    iEikonEnv->EikAppUi()->RemoveFromStack( this );
        
    RUBY_DEBUG0( "CVerificationDialog::~CVerificationDialog EXIT" );          
    }


// -----------------------------------------------------------------------------
// CVerificationDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVerificationDialog::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVerificationDialog::ConstructL" );
      
    iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityAlert );
    }


// ---------------------------------------------------------
// CVerificationDialog::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVerificationDialog* CVerificationDialog::NewL()                                                           
    {
    CVerificationDialog* self = new (ELeave) CVerificationDialog();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CVerificationDialog::RegisterForKeyCallback
// -----------------------------------------------------------------------------
//
void CVerificationDialog::RegisterForKeyCallback( MKeyCallback* aKeyCallback )
    {
    iKeyCallback = aKeyCallback;
    }

// ---------------------------------------------------------
// TKeyResponse CVerificationDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVerificationDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode /*aType*/ )
    {
    RUBY_DEBUG_BLOCK( "CVerificationDialog::OfferKeyEventL" );

    TKeyResponse keyStatus = EKeyWasNotConsumed;

    TInt event = ConvertKeyEventL( aKeyEvent );
    
    if ( event != ENoKeypress )
        {
        switch( event )
            {
            case ESelectKeypress:
                {                   
                keyStatus = EKeyWasConsumed;
                break;
                }
            
            case EUpKeypress:
                {
                --iCurrentIndex;
                if ( iCurrentIndex <= -1 )
                    {
                    iCurrentIndex = KVerificationCommands - 1;    
                    }
                break;
                }
                        
            case EDownKeypress:
                {
                ++iCurrentIndex;
                if ( iCurrentIndex >= KVerificationCommands )
                    {
                    iCurrentIndex = 0;
                    }
                break;
                }
                
            case EVoiceTagSoftKeyCancel:
                {
                iEikonEnv->EikAppUi()->RemoveFromStack( this );
                break;
                }
        
            default:
                break;            
            }
         
         if ( iKeyCallback )
             {
             iKeyCallback->HandleKeypressL( event );
            }
         }
         
    return keyStatus;
    }
    
// ---------------------------------------------------------------------------
// CVerificationDialog::MediatorCommandL
// ---------------------------------------------------------------------------
//
TAknDialogMediatorObserverCommand CVerificationDialog::MediatorCommandL( TUid /*aDomain*/, 
                                                                         TUid aCategory, 
                                                                         TInt aCommandId, 
                                                                         const TDesC8& aData )
    { 
    RUBY_DEBUG_BLOCK( "CVerificationDialog::MediatorCommandL" );
    
    TAknDialogMediatorObserverCommand retValue = EHandleAsNormal;
    
    if ( aCategory == SecondaryDisplay::KCatVoiceUi && 
         aCommandId == SecondaryDisplay::ECmdShowVoiceVerificationQuery )
        {
        if ( aData.Length() == sizeof( SAknDialogFeedback ) ) 
            {
            const SAknDialogFeedback& params = 
                reinterpret_cast<const SAknDialogFeedback&>( *( aData.Ptr() ) );
            
            if ( params.iKey.iCode == EKeyCBA1 )
                {
                // LSK should select current selection
                iKeyCallback->HandleKeypressL( ESelectKeypress );
                
                retValue = EDoNothingWithThisCommand;
                }
            }
        else
            {
            SAknIntegerUpdate* params = (SAknIntegerUpdate*) &aData;
        
            if ( params->iCmd == EAknListQueryUpdateSelection )
                {
                iCurrentIndex = params->iId;
                iDialog->ListControl()->Listbox()->SetCurrentItemIndexAndDraw( iCurrentIndex );

                // Just send EUpKeypress or EDownKeypress event to notify that selection
                // has changed
                iKeyCallback->HandleKeypressL( EDownKeypress );
                }
            }
        }
    return retValue;
    } 

// ---------------------------------------------------------------------------
// CVerificationDialog::NotifyMediatorExit
// ---------------------------------------------------------------------------
//
void CVerificationDialog::NotifyMediatorExit() 
    { 
    RUBY_DEBUG0( "CVerificationDialog::NotifyMediatorExit" );
    // Do nothing
    } 

// -----------------------------------------------------------------------------
// CVerificationDialog::CreateVerificationPopupLC
// -----------------------------------------------------------------------------
//
void CVerificationDialog::CreateVerificationPopupLC( const TDesC& aHeader )
    {
    RUBY_DEBUG0( "CVerificationDialog::CreateVerificationPopupLC START" );
       
    iDialog = CListQueryDialog::NewL( &iCurrentIndex, &iDialog );
    iDialog->PrepareLC( R_VERIFICATION_QUERY );
    
    iDialog->QueryHeading()->SetTextL( aHeader );
    
    SecondaryDisplay::TVerificationData data;
    data.iName.Append( aHeader );
    
    SetSecondaryDisplayDataL( data );
    
    RUBY_DEBUG0( "CVerificationDialog::CreateVerificationPopupLC EXIT" );
    }

// -----------------------------------------------------------------------------
// CVerificationDialog::ShowVerificationPopupL
// -----------------------------------------------------------------------------
//
TInt CVerificationDialog::ShowVerificationPopupL()
    {
    RUBY_DEBUG0( "CVerificationDialog::ShowVerificationPopupL START" );

    // Show verification dialog
    TInt returnValue = iDialog->RunLD();
    
    // Touch selection returns EAknSoftkeyOk
    if ( returnValue == EAknSoftkeyOk )
        {
        returnValue = EVoiceTagSoftKeySelect;
        }
            
    RUBY_DEBUG0( "CVerificationDialog::ShowVerificationPopupL EXIT" );

    return returnValue;
    }
        
// -----------------------------------------------------------------------------
// CVerificationDialog::ConvertKeyEventL
// -----------------------------------------------------------------------------
// 
TInt CVerificationDialog::ConvertKeyEventL( const TKeyEvent& aKeyEvent )
    {
    TInt event = ENoKeypress;
    
    switch( aKeyEvent.iCode )
        {
        case EKeyNo:
        case EKeyEscape:
            {
            event = EVoiceTagSoftKeyCancel;
            break;
            }

        case EKeyDevice3:
        case EKeyYes:
            {
            event = ESelectKeypress;
            break;
            }

        case EKeyUpArrow:
            {
            event = EUpKeypress;
            break;
            }
                        
        case EKeyDownArrow:
            {
            event = EDownKeypress;
            break;
            }
                                        
        default:
            {
            event = ENoKeypress;
            break;            
            }
        }
    return event;
    }
    
// ---------------------------------------------------------------------------
// CVerificationDialog::SelectedCommand
// ---------------------------------------------------------------------------
//    
TInt CVerificationDialog::SelectedCommand( TInt aId )
    {
    RUBY_DEBUG0( "CVerificationDialog::SelectedCommand START" );
    
    TInt command;
    TInt index = iCurrentIndex;
    
    if ( aId != KErrGeneral )
        {
        index = aId;
        }
        
    switch( index )
        {
        case 0:
            command = EVoiceTagSoftKeySelect;
            break;

        case 1:
            command = EVoiceTagSoftKeyOther;
            break;

        case 2:
            command = EVoiceTagSoftKeyCancel;
            break;
                                        
        default:
            command = KErrGeneral;
            break;            
        }
    
    RUBY_DEBUG0( "CVerificationDialog::SelectedCommand EXIT" );
    
    return command;
    }
    
// ---------------------------------------------------------
// CVerificationDialog::SetSecondaryDisplayDataL
// Sets additional information to be sent to secondary display
// ---------------------------------------------------------
//
void CVerificationDialog::SetSecondaryDisplayDataL( SecondaryDisplay::TVerificationData& aData )
    {
    RUBY_DEBUG_BLOCK( "CVerificationDialog::SetSecondaryDisplayDataL" );
   
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        // Create dummy id list (needed for list query automation)
        CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
        CleanupStack::PushL( array );
        for( TInt i = 0; i < 3; ++i )
            {
            array->AppendL( i );
            }
            
        iDialog->PublishDialogL( SecondaryDisplay::ECmdShowVoiceVerificationQuery,
                                 SecondaryDisplay::KCatVoiceUi,
                                 array );
                                 
        iDialog->SetMediatorObserver( this );
        
        CleanupStack::Pop( array ); // Dialog takes ownership of array
   
        // fetch akn utility for mediator support, \oem\aknmediatorfacade.h
        CAknMediatorFacade* covercl = AknMediatorFacade( iDialog );

        if ( covercl )
            {
            covercl->BufStream() << aData.iName;
            covercl->BufStream().CommitL(); // no more data to send so commit buf
            }
        }
    }
    
//  End of File
