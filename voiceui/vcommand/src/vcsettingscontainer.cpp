/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Settings container
*
*/


// INCLUDE FILES
#include <eiktxlbx.h>
#include <aknlists.h>       // CAknSingleGraphicStyleListBox
#include <barsread.h>       // TResourceReader
#include <eikclbd.h>        // CColumnListBoxData
#include <akntitle.h>
#include <eikspane.h>
#include <featmgr.h>
#include <hlplch.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <csxhelp/vc.hlp.hrh>

#include <vcommand.rsg>
#include "vcapp.h"
#include "vcappui.h"
#include "vcsettingscontainer.h"
#include "vcsettingsengine.h"
#include "vcsettingsview.h"
#include "vcsettingslist.h"
#include "vcommandconstants.h"

// ========================= MEMBER FUNCTIONS =================================

// ----------------------------------------------------------------------------
// CVCSettingsContainer::CVCSettingsContainer
// C++ constructor
// ----------------------------------------------------------------------------
//
CVCSettingsContainer::CVCSettingsContainer( CEikButtonGroupContainer& aCbaGroup ):
                                            iCbaGroup( aCbaGroup )
    {
    }


// ----------------------------------------------------------------------------
// CVCSettingsContainer::ConstructL
// Symbian OS 2nd phase constructor
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::ConstructL( const TRect& aRect )
    {
    CEikStatusPane* sp = STATIC_CAST( CAknAppUi*, iCoeEnv->AppUi() )->StatusPane();
    CAknTitlePane* title = STATIC_CAST( CAknTitlePane*,
            sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC( rReader, TitleResourceId() );
    title->SetFromResourceL( rReader );
    CleanupStack::PopAndDestroy(); //rReader
    
    CreateWindowL();
    iSettingsList = new ( ELeave ) CVCSettingsList();
    iSettingsList->SetContainerWindowL( *this );
    iSettingsList->ConstructL();
    iSettingsList->ListBox()->SetListBoxObserver( this );
    
    SetRect(aRect);
    ActivateL();
    }


// ----------------------------------------------------------------------------
// CVCSettingsContainer::~CVCSettingsContainer
// ----------------------------------------------------------------------------
//
CVCSettingsContainer::~CVCSettingsContainer()
    {
    delete iSettingsList; 
	}


// ----------------------------------------------------------------------------
// CVCSettingsContainer::CountComponentControls
// From CCoeControl return the number of controls owned
// ----------------------------------------------------------------------------
//
TInt CVCSettingsContainer::CountComponentControls() const
    {
    return 1;
    }


// ----------------------------------------------------------------------------
// CVCSettingsContainer::ComponentControl
// From CCoeControl returns a control
// ----------------------------------------------------------------------------
//
CCoeControl* CVCSettingsContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iSettingsList;
    }


// ----------------------------------------------------------------------------
// CVCSettingsContainer::EditCurrentL
// Open setting page for currently selected setting item.
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::EditCurrentL( TBool aCalledFromMenu )
    {
    TInt index = iSettingsList->ListBox()->CurrentItemIndex();

    iSettingsList->EditItemL( index, aCalledFromMenu );
	}


// ----------------------------------------------------------------------------
// CVCSettingsContainer::ResetL
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::ResetL()
    {
    CAknQueryDialog* dlg = CAknQueryDialog::NewL(CAknQueryDialog::EConfirmationTone);
    if ( dlg->ExecuteLD(R_VC_RESET_ADAPTATION_DIALOG) )
        {
        CVCSettingsEngine*	engine = CVCSettingsEngine::NewL();
        CleanupStack::PushL( engine );
		engine->ResetAdaptationL();
		CleanupStack::PopAndDestroy( engine ); // engine
		}
	}


// ----------------------------------------------------------------------------
// CVCSettingsContainer::SaveSettingsL
// Save all settings.
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::SaveSettingsL()
    {
    iSettingsList->SaveSettingsL();
    }


// ----------------------------------------------------------------------------
// CVCSettingsContainer::CurrentItemIndex
// ----------------------------------------------------------------------------
//
TInt CVCSettingsContainer::CurrentItemIndex() const
	{
	return iSettingsList->ListBox()->CurrentItemIndex();
	}


// ----------------------------------------------------------------------------
// CVCSettingsContainer::ItemListSize
// ----------------------------------------------------------------------------
//
TInt CVCSettingsContainer::ItemListSize() const 
    {
    return iSettingsList->ListBox()->Model()->ItemTextArray()->MdcaCount();
    }
    
    
// ----------------------------------------------------------------------------
// CVCSettingsContainer::CurrentItemIndex
// ----------------------------------------------------------------------------
//
TInt CVCSettingsContainer::CurrentItemIdentifier() const
	{
	CAknSettingItemArray* itemArray = iSettingsList->SettingItemArray();
	TInt itemIndex = itemArray->ItemIndexFromVisibleIndex( CurrentItemIndex() );
    CAknSettingItem* settingItem = itemArray->At( itemIndex );
    return settingItem->Identifier();
	}


// ----------------------------------------------------------------------------
// CVCSettingsContainer::OfferKeyEventL
// Key event handling
// ----------------------------------------------------------------------------
//
TKeyResponse CVCSettingsContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                   TEventCode aType )
    {  
    TKeyResponse ret = iSettingsList->OfferKeyEventL( aKeyEvent, aType );
    
    // Change MSK
    if( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        SetMiddleSoftkeyLabelL();
        }
        
    return ret;
    }


// ----------------------------------------------------------------------------
// CVCSettingsContainer::GetHelpContext
// Gives the help context to be displayed
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidHelp; 
    aContext.iContext = KHLP_VC_SET;
    }
    
    
// ----------------------------------------------------------------------------
// CVCSettingsContainer::TitleResourceId()
// Returns profiles views status pane title .
// ----------------------------------------------------------------------------
//
TInt CVCSettingsContainer::TitleResourceId() const
    {
    return R_VC_MAIN_VIEW_TITLE;
    }
    
    
// ----------------------------------------------------------------------------
// CVCSettingsContainer::IsTrainingOn
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::FocusChanged(TDrawNow aDrawNow)
	{
    if( iSettingsList )
        {
        iSettingsList->SetFocus( IsFocused(), aDrawNow );
        }
    }
     
     
// ----------------------------------------------------------------------------
// CVCSettingsContainer::HandleResourceChange
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::HandleResourceChange(TInt aType)
    {   
    if ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                           mainPaneRect);
        SetRect( mainPaneRect );
        }
    CCoeControl::HandleResourceChange( aType );
    }     
        
        
// ----------------------------------------------------------------------------
// CVCSettingsContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::SizeChanged()
    {
    if (iSettingsList)
        {
        iSettingsList->SetRect(Rect());
        }
    }
    
// ----------------------------------------------------------------------------
// CVCSettingsContainer::HandleControlEventL
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::HandleControlEventL( CCoeControl* /*aControl*/,
                                                TCoeEvent /*aEventType*/)
    {
    }
    
// ---------------------------------------------------------------------------
// CVCSettingsContainer::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CVCSettingsContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                TListBoxEvent aEventType )
    {
    switch ( aEventType ) 
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {
            if ( CurrentItemIdentifier() == EVCResetItem )
                {
                ResetL();
                }
            else
                {
                EditCurrentL( EFalse );
                }            
            break;
            }
            
        default:
            break;
        }
    }

    
// ----------------------------------------------------------------------------
// CVCSettingsContainer::SetMiddleSoftkeyLabelL
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::SetMiddleSoftkeyLabelL() const
    {    
    RemoveCommandFromMSK();
    
    if( CurrentItemIdentifier() == EVCResetItem )
        {
        DoSetMiddleSoftKeyLabelL( R_QTN_VC_SET_MSK_RESET, EVCCmdReset );
        }
    else
        {
        DoSetMiddleSoftKeyLabelL( R_QTN_VC_SET_MSK_CHANGE, EVCCmdChange );
        }
    }
    
// ----------------------------------------------------------------------------
// CVCSettingsContainer::SetMiddleSoftKeyLabelL
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::DoSetMiddleSoftKeyLabelL( const TInt aResourceId,
                                                const TInt aCommandId  ) const
    {
    HBufC* mskText = StringLoader::LoadLC( aResourceId );
    TPtr mskPtr = mskText->Des();
    iCbaGroup.AddCommandToStackL( KVcMskControlID, aCommandId, mskPtr );
    CleanupStack::PopAndDestroy( mskText );
    }
    
// ----------------------------------------------------------------------------
// CVCSettingsContainer::RemoveCommandFromMSK
// ----------------------------------------------------------------------------
//
void CVCSettingsContainer::RemoveCommandFromMSK() const
    {
    iCbaGroup.RemoveCommandFromStack( KVcMskControlID, EVCCmdChange );
    iCbaGroup.RemoveCommandFromStack( KVcMskControlID, EVCCmdReset );
    }  

// End of File

