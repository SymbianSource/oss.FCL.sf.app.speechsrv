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
* Description:  Pbk info view dialog.
*
*/



// INCLUDE FILES
#include <AknIconArray.h>   // for GulArray
#include <bldvariant.hrh>
#include "pbkinfoviewdialog.h"
#include "pbkinfoviewutil.h"
#include "pbkinfoviewsindhandler.h"
#include "pbkinfoviewdefines.h"
#include <pbkinfoview.rsg>
#include "pbkinfoview.rh"
#include <e32property.h>        // RProperty
#include <PSVariables.h>        // Property values
#include <ctsydomainpskeys.h>

#include "rubydebug.h"

#include <csxhelp/vc.hlp.hrh>  // for help text id
#include <eikmenub.h>  // CEikMenuBar
#include <PbkIconId.hrh> // Phonebook icon ids
#include <aknnotewrappers.h>  // For information note
#include <StringLoader.h>

#include <MPbk2FieldPropertyArray.h>
#include <VPbkEng.rsg>
#include <CPbk2StoreConfiguration.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkFieldTypeRefsList.h>	
#include <CPbk2IconFactory.h>
#include <Pbk2FieldPropertiesFactory.h>
#include <MPbk2FieldProperty.h>
#include <TVPbkContactStoreUriPtr.h>
#include <PbkFields.hrh>

const TUint KFieldIds[] = 
    {
    R_VPBK_FIELD_TYPE_LANDPHONEGEN,    
    R_VPBK_FIELD_TYPE_MOBILEPHONEGEN,  
    R_VPBK_FIELD_TYPE_VIDEONUMBERGEN,
    R_VPBK_FIELD_TYPE_VOIPGEN,
    R_VPBK_FIELD_TYPE_EMAILGEN,
    R_VPBK_FIELD_TYPE_IMPP
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::NewL
//
// -----------------------------------------------------------------------------
//
CPbkInfoViewDialog* CPbkInfoViewDialog::NewL( TInt aContactId )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewDialog::NewL" );

    CPbkInfoViewDialog* self = new ( ELeave ) CPbkInfoViewDialog( aContactId );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }

// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::CPbkInfoViewDialog
// 
// -----------------------------------------------------------------------------
//
CPbkInfoViewDialog::CPbkInfoViewDialog( TInt aContactId )
    {
	iContactId = aContactId;
	
	if( aContactId == KVoiceTaglessContactId )
	    {
	    iVoiceTaglessContact = ETrue;
	    }
    }

// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::ConstructL
//
// -----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::ConstructL()
    {
    CAknDialog::ConstructL( R_INFOVIEW_DIALOG_MENU );
	
    // Hide tabs
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities
             ( TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        iNaviPane = (CAknNavigationControlContainer*) statusPane->ControlL
                    ( TUid::Uid(EEikStatusPaneUidNavi ) );
        iNaviPane->PushDefaultL();
        }
        
    iAvkonAppUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyLong );
	
	// Get the previous title so it can be restored
	iStatusPaneHandler = CStatusPaneHandler::NewL( iAvkonAppUi );
	iStatusPaneHandler->StoreOriginalTitleL();
	
	iPopupController = CAknInfoPopupNoteController::NewL();
	
	iResHandler = CPbkInfoViewResHandler::NewL();
	
	iSindHandler = CPbkInfoViewSindHandler::NewL( iContactId );
    } 

// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::~CPbkInfoViewDialog()
// Destructor.
// -----------------------------------------------------------------------------
//
CPbkInfoViewDialog::~CPbkInfoViewDialog()
    {
	delete iStatusPaneHandler;
	iStatusPaneHandler = NULL;
	delete iPopupController;
	iPopupController = NULL;
    delete iResHandler;
	iResHandler = NULL;
	delete iSindHandler;
	iSindHandler = NULL;
	
	if ( iAvkonAppUi )
		{
		iAvkonAppUi->RemoveFromStack( this  );
		}
		
    // Restore tabs
    if ( iNaviPane )
        {
        iNaviPane->Pop();
        }
    }

//------------------------------------------------------------------------------
// CPbkInfoViewDialog::ExecuteLD
// Overrides CAknDialog::ExecuteLD. Checks whether the contact has any voice
// tags. If not an info note is displayed. Otherwise calls CAknDialog::ExecuteLD
// @param aResourceId The resource ID of the dialog to be loaded
// @return see CAknDialog::ExecuteLD
//------------------------------------------------------------------------------
//
TInt CPbkInfoViewDialog::ExecuteLD( TInt aResourceId )
    {
    // Check whether info view should be shown at all
    if( iContactId == KVoiceTaglessContactId )
        {
        ShowInformationNoteL( R_INFOVIEW_EMPTY_VIEW_SIM_CONTACT );
        delete this;
        return 0;
        }
    else if( iSindHandler->VoiceTagCount() == 0 )
        {
        ShowInformationNoteL( R_INFOVIEW_EMPTY_VIEW );
        delete this;
        return 0;
        }
    // Show info view
    else
        {
        return CAknDialog::ExecuteLD( aResourceId );
        }    
    }

//------------------------------------------------------------------------------
// CPbkInfoViewDialog::ActivateL (from CCoeControl)
// Called by system when dialog is activated.
//------------------------------------------------------------------------------
//
void CPbkInfoViewDialog::ActivateL()
	{
    CAknDialog::ActivateL();

    // This cannot be in ConstructL which is executed before dialog is launched
    iAvkonAppUi->AddToStackL( this );
    }

//------------------------------------------------------------------------------
// CPbkInfoViewDialog::GetHelpContext
// Method to get context sensitive help topic.
// @param aContext Help topic to open.
//------------------------------------------------------------------------------
//
void CPbkInfoViewDialog::GetHelpContext( TCoeHelpContext& aContext ) const
	{
	aContext.iMajor = TUid::Uid( PBKINFOVIEW_HELP_UID );
	aContext.iContext = KVCINFO_HLP_MAINVIEW;    
	}

//------------------------------------------------------------------------------
// CPbkInfoViewDialog::ProcessCommandL
// Handles menu events.
// @param  aCommandId Command id.
//------------------------------------------------------------------------------
//
void CPbkInfoViewDialog::ProcessCommandL( TInt aCommandId ) 
	{
	HideMenu();

	switch ( aCommandId )
		{
        case EInfoViewMenuCmdPlay:
			{
			if ( IsCallActive() )
			    {
			    CallInProgressNoteL();
			    }
			else 
			    {
			    iSindHandler->PlayVoiceCommandL( CurrentItemIndex() );
			    }
			
			break;
			}
		case EInfoViewMenuCmdHelp:
			{
			iAvkonAppUi->ProcessCommandL( EAknCmdHelp );
			break;
			}
        case EInfoViewMenuCmdExit:
			{
			iAvkonAppUi->ProcessCommandL( EAknCmdExit );
			break;
			}
        case EAknCmdExit:
        case EEikCmdExit:
			{
            //
			// Exit dialog
			//
			// CEikAppUi::ProcessCommandL starts CAknAppShutter that 
			// closes all dialogs and finally calling application. Before 
			// dialog is closed (deleted) it's OkToExitL(EEikBidCancel)
			// is called. EEikBidCancel means OkToExitL must silently
			// save and return ETrue.
			//
			iAvkonAppUi->ProcessCommandL( EAknCmdExit );

			break;
			}

		default:
			{
			break;
			}
		}
	}


// ----------------------------------------------------------------------------
// CPbkInfoViewDialog::IsCallActive
// ----------------------------------------------------------------------------
//
TBool CPbkInfoViewDialog::IsCallActive()
    {
    TBool callActive( EFalse );
    TInt state( 0 );
    TInt err = RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallState,
                               state );
                               
    // note! errors are handled as a call is active    
    if( err || state == EPSCTsyCallStateConnected 
            || state == EPSCTsyCallStateAlerting
            || state == EPSCTsyCallStateRinging
            || state == EPSCTsyCallStateDialling
            || state == EPSCTsyCallStateAnswering ) 
        {
        callActive = ETrue;
        }
    else
        {
        TInt callType( 0 );
        TInt err =  RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType,
                                callType );
        if ( err || callType == EPSCTsyCallTypeH324Multimedia ) 
            {
            callActive = ETrue;
            }
        }    
    
    return callActive;
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewDialog::CallInProgressNoteL
// ----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::CallInProgressNoteL()
    {
    HBufC* text = StringLoader::LoadLC( R_TEXT_CALL_IN_PROGRESS );
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

    
// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::OkToExitL
// Called by framework before exiting the dialog.
// @param aButtonId Button id.
// @return ETrue to exit\ EFalse to not to exit.
// -----------------------------------------------------------------------------
//
TBool CPbkInfoViewDialog::OkToExitL( TInt aButtonId )
    {
    if( iSindHandler->IsPlaying() )
	    {
		iSindHandler->CancelPlaybackL();
		}
    
    switch ( aButtonId )
        {
        case EAknSoftkeyBack:
            {
            return ETrue;
            }
            
        case EAknSoftkeyClose:
        case EAknSoftkeyExit:
            {
            iAvkonAppUi->ProcessCommandL( EAknCmdExit );
            return ETrue;
            }
            
        case EAknSoftkeySelect: // Middle soft key, no exit
            {
            if ( IsCallActive() )
			    {
			    CallInProgressNoteL();
			    }
			else 
			    {
			    iSindHandler->PlayVoiceCommandL( CurrentItemIndex() );
			    }
            
            return EFalse;
            }
            
         default:
            {
            return CAknDialog::OkToExitL( aButtonId );
            }   
        }
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewDialog::OfferKeyEventL
// Called by framework for key event handling.
// ----------------------------------------------------------------------------
//
TKeyResponse CPbkInfoViewDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                 TEventCode aType )
	{
	if ( aType == EEventKey )
		{
		switch ( aKeyEvent.iCode )
			{
			case EKeyEscape:  // Framework calls this when dialog must shut
				{
				return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        		}
        		
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                CAknDialog::OfferKeyEventL( aKeyEvent, aType );
                if( iSindHandler->VoiceTagCount() > 0 )
                    {
                    ShowPopupL();
                    }
                return EKeyWasConsumed;
                }
			
            default:
			    break;
			}
		}
		
	return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
	}

// ----------------------------------------------------------------------------
// CPbkInfoViewDialog::HandleResourceChange
// Called when display resolution changes.
// ----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::HandleResourceChange( TInt aType )
    {
    // Handle change in layout orientation
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                           mainPaneRect );
        SetRect( mainPaneRect );
        //iListBox->SetSize( mainPaneRect.Size() );
        iListBox->SetSize( mainPaneRect.Size() );
        CCoeControl::HandleResourceChange( aType );
		DrawDeferred();
		}
	else
	    {
	    CCoeControl::HandleResourceChange( aType );
	    }
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewDialog::PostLayoutDynInitL
// Called by framework after dialog is shown.
// ----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::PostLayoutDynInitL()
	{
	}

// ----------------------------------------------------------------------------
// CPbkInfoViewDialog::PreLayoutDynInitL
// Called by framework before dialog is shown.
// ----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::PreLayoutDynInitL()
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewDialog::PreLayoutDynInitL" );
    
    iListBox = ( CAknDoubleGraphicStyleListBox* ) ControlOrNull ( EInfoViewDialogList );
    
   	__ASSERT_ALWAYS( iListBox, TPbkInfoViewUtil::Panic( KErrGeneral ) );
	
	iListBox->SetListBoxObserver( this );
	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
	                                                      CEikScrollBarFrame::EAuto );                                 
    iListBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
	
	TDesC* des;
	// Set empty list text
	if( iVoiceTaglessContact )
	    {
    	des = StringLoader::LoadLC( R_INFOVIEW_EMPTY_VIEW_SIM_CONTACT );
    	iListBox->View()->SetListEmptyTextL( *des );
	    }
	else
	    {
    	des = StringLoader::LoadLC( R_INFOVIEW_EMPTY_VIEW );
    	iListBox->View()->SetListEmptyTextL( *des );
	    }
	CleanupStack::PopAndDestroy( des );
    
    // Set icons

	CArrayPtr<CGulIcon>* icons = new (ELeave) CAknIconArray( KDefaultArraySize );
	CleanupStack::PushL( icons );
	
	CreateFieldIconsL( icons );
    
    CleanupStack::Pop( icons );

	CreateListBoxItemsL();
	
	iListBox->UpdateScrollBarsL();
	iListBox->ScrollBarFrame()->MoveVertThumbTo( 0 );

	iStatusPaneHandler->SetTitleL( R_INFOVIEW_DIALOG_TITLE );
	
	UpdateCbaL( R_SOFTKEYS_OPTIONS_BACK__PLAY );
	}

// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::DynInitMenuPaneL
// Called by framework before menu is shown.
// @param aResourceId Menu resource id.
// @param aMenuPane Pointer to the menu.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::DynInitMenuPaneL( TInt aResourceID,
                                           CEikMenuPane* aMenuPane )
	{
	if ( aResourceID == R_INFOVIEW_DIALOG_MENU_PANE )
	    {
	    // No play function in menu if there are no voice tags
	    if( iSindHandler->VoiceTagCount() == 0 )
            {
    	    aMenuPane->SetItemDimmed( EInfoViewMenuCmdPlay, ETrue );
            }
	    }
	}
	
// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::IconIndex
// Returns TInfoViewDialogIconIndex enumeration icon index.
// @param aIconId enumeration TPbkIconId from PbkIconId.hrh.
// @return TInt Icon index.
// -----------------------------------------------------------------------------
//
TInt CPbkInfoViewDialog::IconIndex( TInt aIconId )
	{
	TInt iconIndex;
	
	switch ( aIconId )
		{
		case EPbkqgn_prop_nrtyp_mobile:
		    iconIndex = EIconIndexMobile;
		    break;
		case EPbkqgn_prop_nrtyp_phone:
		    iconIndex = EIconIndexPhone;
		    break;
		case EPbkqgn_prop_nrtyp_video:
		    iconIndex = EIconIndexVideo;
		    break;
		case EPbkqgn_prop_nrtyp_email:
		    iconIndex = EIconIndexEmail;
		    break;
		case EPbkqgn_prop_nrtyp_voip:
			if ( isXsp )
				{
				iconIndex = EIconIndexXsp;
				}
			else
				{
				iconIndex = EIconIndexVoip;
				}
		    break;
		default:
		    iconIndex = EIconIndexEmpty;    
		    break;
		}
		
    return iconIndex; 
	}
		
// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::CreateListBoxItemsL
// Creates listbox items.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::CreateListBoxItemsL()
	{
	RUBY_DEBUG_BLOCK( "CPbkInfoViewDialog::CreateListBoxItemsL" );
	
	CAknDoubleGraphicStyleListBox* listBox = iListBox;
	CDesCArray* items = (CDesCArray*) listBox->Model()->ItemTextArray();
	items->Reset();
    listBox->ItemDrawer()->ClearAllPropertiesL();
	
	TInt count = iSindHandler->VoiceTagCount();
	
	// Create dialog entries
	for ( TInt i = 0; i < count; i++ )
		{
		// Line header
		HBufC* firstLine = iSindHandler->VoiceTagLabelLC( i );
		// Phone number, email address, etc...
		HBufC* secondLine = iSindHandler->VoiceTagValueL( i ).AllocLC();
        isXsp = EFalse;
		if( iSindHandler->FieldIdL() == EPbkFieldIdXsp )
			{
			isXsp = ETrue;
			}
		TInt iconIndex = IconIndex( iSindHandler->IconIdL( i ) );

		TPtr ptr1 = firstLine->Des();
		AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr1 );
		TPtr ptr2 = secondLine->Des();
		AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr2 );

		HBufC* listBoxItem = NULL;
		listBoxItem = TDialogUtil::ConstructListBoxItemLC( firstLine->Des(), 
		                                                   secondLine->Des(),
                                                           iconIndex );         

		items->AppendL( listBoxItem->Des() );
		
		CleanupStack::PopAndDestroy( listBoxItem );
		CleanupStack::PopAndDestroy( secondLine );
		CleanupStack::PopAndDestroy( firstLine );
		}

	listBox->HandleItemAdditionL();

    TInt current = CurrentItemIndex();
    if (current == KErrNotFound)
        {
		current = 0;
        }
		
	iListBox->SetCurrentItemIndexAndDraw( current );
	}
	
// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::HandleListBoxEventL (from MEikListBoxObserver)
// From MEikListBoxObserver, called by framework.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                              TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {            
            if ( iSindHandler->VoiceTagCount() > 0 )
                {
                ShowPopupL();
                
                if ( IsCallActive() )
                    {
                    CallInProgressNoteL();
                    }
                else 
                    {
                    iSindHandler->PlayVoiceCommandL( CurrentItemIndex() );
                    }
              
                }
            break;
            }
          
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::CurrentItemIndex
// Gets current listbox item.
// -----------------------------------------------------------------------------
//
TInt CPbkInfoViewDialog::CurrentItemIndex()
	{
	TInt ret;
	
	if ( iListBox->Model()->NumberOfItems() == 0 )
		{
		ret = KErrNotFound;
		}
    else
        {
        ret = iListBox->CurrentItemIndex();
        }
	
	return ret;
	}
	
// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::UpdateCbaL
// Updates dialog cba.
// @param aResourceId Resource id.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::UpdateCbaL( TInt aResourceId )
    {
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    cba.SetCommandSetL( aResourceId );
    cba.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::ShowPopupL
// Shows popup for a voice command in the list.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::ShowPopupL()
    {
    iPopupController->SetTextL( iSindHandler->PopupTextL( CurrentItemIndex() ) );
    iPopupController->ShowInfoPopupNote();
    }
    
// -----------------------------------------------------------------------------
// CPbkInfoViewDialog::ShowInformationNoteL
//
// -----------------------------------------------------------------------------
//
void CPbkInfoViewDialog::ShowInformationNoteL( TInt aResourceId )
	{
	TDesC* noteText = StringLoader::LoadLC( aResourceId );
    	
    CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE );
    dialog->ExecuteLD( *noteText );
    
    CleanupStack::PopAndDestroy( noteText );
	}

void CPbkInfoViewDialog::CreateFieldIconsL(CArrayPtr<CGulIcon>* aIconArray)
	{
    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(configuration);
	    
    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    CleanupStack::PushL( uriArray ); 
    if ( uriArray->IsIncluded(VPbkContactStoreUris::DefaultCntDbUri()) == EFalse )
        {
        uriArray->AppendL( VPbkContactStoreUris::DefaultCntDbUri() );	
	    }    
    
    CVPbkContactManager* contactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PushL( contactManager );
	    
    TInt arraySize = sizeof KFieldIds / sizeof KFieldIds[0];
    TInt count =0;
    const MVPbkFieldType* fieldType = NULL;
    CGulIcon* icon = NULL;
			
    CVPbkFieldTypeRefsList* fieldTypeList = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( fieldTypeList );
    
    for( count = 0; count < arraySize;  ++count )
        {
        fieldType = contactManager->FieldTypes().Find( KFieldIds[count] );
        fieldTypeList->AppendL(*fieldType);	
        }
			
    MPbk2FieldPropertyArray* propertyArray = Pbk2FieldPropertiesFactory::CreateLC(*fieldTypeList,
                                                 &contactManager->FsSession() );
    
    CPbk2IconFactory* pbk2IconFactory = CPbk2IconFactory::NewL();
    CleanupStack::PushL( pbk2IconFactory );
    
    for( count = 0; count < propertyArray->Count(); ++ count)
        {
        icon = pbk2IconFactory->CreateIconL( propertyArray->At(count).IconId());
        aIconArray->AppendL(icon);
        }

	iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( aIconArray );
	
    //Destroy: configuration,uriArray,contactManager
    //fieldTypeList,propertyArray,pbk2IconFactory
    CleanupStack::PopAndDestroy( 6,configuration ); 	
    
	TFileName bitmapName;
	CPbkInfoViewResHandler::GetBitmapFileName( bitmapName );
	
	aIconArray->AppendL( TDialogUtil::CreateIconL(
	                KAknsIIDQgnLogoEmpty, bitmapName,
	                EMbmAvkonQgn_prop_empty,
	                EMbmAvkonQgn_prop_empty_mask ) );
	}
//  End of File  
