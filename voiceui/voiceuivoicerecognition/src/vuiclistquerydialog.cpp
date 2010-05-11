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
* Description:  
*
*/


// INCLUDE FILES
#include <e32base.h>

#include <vuivoicerecognition.rsg>
#include <vuivoicerecogdefs.h>

#include "vuivoicerecognition.hrh"

#include "vuiclistquerydialog.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CListQueryDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CListQueryDialog* CListQueryDialog::NewL( TInt *aIndex,
                                          CListQueryDialog** aSelf,
                                          MEikCommandObserver* aObserver,
                                          TInt aMenuTitleResourceId )
    {
    CListQueryDialog* self = new (ELeave) CListQueryDialog( aObserver,
                                                            aMenuTitleResourceId,
                                                            aIndex, aSelf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }   
    
// Destructor       
CListQueryDialog::~CListQueryDialog()
    {
    RUBY_DEBUG0( "CListQueryDialog::~CListQueryDialog START" );

    MakeVisible( EFalse );

    if ( iSelf )
        {
        *iSelf = NULL;
        iSelf = NULL;
        }

    RUBY_DEBUG0( "CListQueryDialog::~CListQueryDialog EXIT" );
    }

// ---------------------------------------------------------
// CListQueryDialog::IsMenuVisible
// ---------------------------------------------------------
// 
TBool CListQueryDialog::IsMenuVisible()
    {
    RUBY_DEBUG0( "CListQueryDialog::IsMenuVisible" );
    return MenuShowing();
    }
    
// ---------------------------------------------------------
// CListQueryDialog::SetCommandVisibility
// ---------------------------------------------------------
// 
void CListQueryDialog::SetCommandVisibility( TBool aHidden )
    {
    RUBY_DEBUG0( "CListQueryDialog::SetCommandVisibility" );
    iCommandsHidden = aHidden;
    }

// ---------------------------------------------------------
// CListQueryDialog::PostLayoutDynInitL
// ---------------------------------------------------------
// 
void CListQueryDialog::PostLayoutDynInitL()
    {
    RUBY_DEBUG_BLOCK( "CListQueryDialog::PostLayoutDynInitL" );
  
    if ( iObserver && iMenuResourceId )
        {
        if ( iMenuBar )
            {
            iMenuBar->TitleArray()->ResetAndDestroy();
            iMenuBar->SetMenuTitleResourceId( iMenuResourceId );
            iMenuBar->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );
            }
        else
            {
            CEikMenuBar* menuBar = new ( ELeave ) CEikMenuBar();
            CleanupStack::PushL( menuBar );
        
            menuBar->ConstructL( this, NULL, iMenuResourceId );
            menuBar->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );
        
            iEikonEnv->EikAppUi()->AddToStackL( menuBar,
                                                ECoeStackPriorityMenu,
                                                ECoeStackFlagRefusesFocus);
        
            iMenuBar = menuBar;
            CleanupStack::Pop( menuBar ); // now owned by this Dialog.
    		}
        }
    
    // Set scrollbar observer so that touch scrolling can be observed
    ListBox()->ScrollBarFrame()->SetScrollBarFrameObserver( this );
    
    CAknDialog::PostLayoutDynInitL();
    }
        
// ---------------------------------------------------------
// CListQueryDialog::OkToExitL
// ---------------------------------------------------------
// 
TBool CListQueryDialog::OkToExitL( TInt aButtonId )
    {
    RUBY_DEBUG_BLOCK( "CListQueryDialog::OkToExitL" );
    
    TBool returnValue = CAknListQueryDialog::OkToExitL( aButtonId );

    switch( aButtonId )
        {
        case EAknSoftkeyOptions:
            {            
            DisplayMenuL();
            
            returnValue = EFalse;
            
            if ( iObserver )
                {
                iObserver->ProcessCommandL( EAknSoftkeyOptions );
                }
            
            break;
            }
            
        case EVoiceTagSoftKeySelect:
            {
            returnValue = ETrue;
            
            break;
            }

        default:
            {
            break;
            }
        }
    return returnValue;
    }

// ---------------------------------------------------------
// CListQueryDialog::ProcessCommandL
// ---------------------------------------------------------
//      
void CListQueryDialog::ProcessCommandL( TInt aCommandId )
    {
    RUBY_DEBUG_BLOCK( "CListQueryDialog::ProcessCommandL" );
    
    if ( !iObserver )
        {
        User::Leave( KErrArgument );
        }
    
    iObserver->ProcessCommandL( aCommandId );
    }

// ---------------------------------------------------------
// CListQueryDialog::DynInitMenuPaneL
// ---------------------------------------------------------
//     
void CListQueryDialog::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    RUBY_DEBUG_BLOCK( "CListQueryDialog::DynInitMenuPaneL" );
    
    if ( aMenuPane && aResourceId == R_NBEST_MENU_PANE )
        {
        aMenuPane->SetItemDimmed( EVoiceTagSoftKeyOpen, iCommandsHidden );
        }
    }

// ---------------------------------------------------------
// CListQueryDialog::HandleListBoxEventL
// ---------------------------------------------------------
//
void CListQueryDialog::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    RUBY_DEBUG_BLOCK( "CListQueryDialog::HandleListBoxEventL" );
    
    TBool forwardEvent = ETrue;
    
    if ( aEventType == EEventItemClicked )
        {
        if ( iObserver )
            {
            forwardEvent = EFalse;
            iObserver->ProcessCommandL( EDragKeypress );
            }
        }    
    else if ( aEventType == EEventItemDraggingActioned )
        {
        if ( iObserver )
            {
            iObserver->ProcessCommandL( EDragKeypress );
            }
        }
    else if ( aEventType == EEventItemDoubleClicked )
        {
        if ( iObserver )
            {
            forwardEvent = EFalse;
            iObserver->ProcessCommandL( EVoiceTagSoftKeySelect );
            }
        }
    else if ( aEventType == EEventFlickStopped )
        {
        if ( iObserver )
            {
            iObserver->ProcessCommandL( EDragKeypress );
            }
        }
        
    if ( forwardEvent )
        {
        CAknListQueryDialog::HandleListBoxEventL( aListBox, aEventType );
        }
    }

// ---------------------------------------------------------
// CListQueryDialog::HandleScrollEventL
// ---------------------------------------------------------
// 
void CListQueryDialog::HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType )
    {
    RUBY_DEBUG_BLOCK( "CListQueryDialog::HandleScrollEventL" );
    
    if ( aEventType == EEikScrollThumbDragVert )
        {
        if ( iObserver )
            {
            iObserver->ProcessCommandL( EScrollKeypress );
            }
        }
    
    ListBox()->HandleScrollEventL( aScrollBar, aEventType );
    }

// ---------------------------------------------------------
// CListQueryDialog::CListQueryDialog
// ---------------------------------------------------------
//              
CListQueryDialog::CListQueryDialog( MEikCommandObserver* aObserver,
                                    TInt aMenuTitleResourceId,
                                    TInt *aIndex, CListQueryDialog** aSelf )
 : CAknListQueryDialog( aIndex ), iObserver( aObserver),
   iMenuResourceId( aMenuTitleResourceId ), iSelf( aSelf )
    {
    }

// ---------------------------------------------------------
// CListQueryDialog::ConstructL
// ---------------------------------------------------------
//           
void CListQueryDialog::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CListQueryDialog::ConstructL" );
    }
    
// End of File

