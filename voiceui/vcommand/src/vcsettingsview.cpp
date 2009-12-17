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
* Description:  Settings view
*
*/


// INCLUDE FILES

#include <avkon.hrh>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <akntitle.h>  // CAknTitlePane
#include <featmgr.h>
#include <eikmenup.h>
#include <barsread.h>
#include <StringLoader.h>
#include <hlplch.h>

#include <vcommand.rsg>
#include "vcommand.hrh"
#include "vcappui.h"
#include "vcsettingsview.h"
#include "vcsettingscontainer.h"
#include "vcommandconstants.h"
#include "rubydebug.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CVCSettingsView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVCSettingsView::ConstructL()
    {
    BaseConstructL( R_VC_SETTING_LIST_VIEW );
    }


// Destructor
CVCSettingsView::~CVCSettingsView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    }


// ---------------------------------------------------------------------------
// CVCSettingsView::Id
// From CAknView, returns Uid of View
// ---------------------------------------------------------------------------
//
TUid CVCSettingsView::Id() const
    {
    return KUidVCSettingsView;
    }


// ---------------------------------------------------------------------------
// CVCSettingsView::HandleCommandL
// From MEikMenuObserver delegate commands from the menu
// ---------------------------------------------------------------------------
//
void CVCSettingsView::HandleCommandL( TInt aCommand )
    {
    if( !iOperationInProgress )
        {
        iOperationInProgress = ETrue;
        
        switch ( aCommand )
            {
            case EVCMenuCmdChange:
                {
                // ETrue for opening the settings view
                iContainer->EditCurrentL( ETrue );
                break;
                }
                
            case EVCCmdChange:
                {
    		    iContainer->EditCurrentL( EFalse );
                break;
                }

    		case EVCCmdReset:
    			{
    			iContainer->ResetL();
    			break;
    			}
    			
            case EAknCmdHelp:
            	{
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                                                     AppUi()->AppHelpContextL() );
                break;
            	}

            case EAknSoftkeyBack:
                {
                AppUi()->ActivateLocalViewL( iPreviousView.iViewUid );
                break;
                }

            default:
                {
                AppUi()->HandleCommandL( aCommand );
                break;
                }
            }
            
        iOperationInProgress = EFalse;
        }
    

    }

// ---------------------------------------------------------------------------
// CVCSettingsView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CVCSettingsView::DoActivateL( const TVwsViewId& aPrevViewId,
                                   TUid /*aCustomMessageId*/,
                                   const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CVCSettingsContainer( *(Cba()) );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        iContainer->ActivateL();
        }
        
    iPreviousView = aPrevViewId;
        
    iContainer->SetMiddleSoftkeyLabelL();
    }

// ---------------------------------------------------------------------------
// CVCSettingsView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CVCSettingsView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CVCSettingsView::DynInitMenuPaneL
// Dynamically customize menu items
// ---------------------------------------------------------------------------
//
void CVCSettingsView::DynInitMenuPaneL( TInt aResourceId,
                                            CEikMenuPane* aMenuPane )
    {
	if ( aResourceId == R_VC_SETTINGS_MENU )
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
            }

		TInt index = iContainer->CurrentItemIndex();

		if ( index + 1 != iContainer->ItemListSize() )
			{
			aMenuPane->SetItemDimmed( EVCCmdReset, ETrue );
			}
		else
			{
			aMenuPane->SetItemDimmed( EVCMenuCmdChange, ETrue );
			}
        }
	}

// ---------------------------------------------------------
// CVCSettingsView::HandleClientRectChange
// ---------------------------------------------------------
//
void CVCSettingsView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
            mainPaneRect );
        iContainer->SetRect( mainPaneRect );
        }
    }
    
    
// End of File
