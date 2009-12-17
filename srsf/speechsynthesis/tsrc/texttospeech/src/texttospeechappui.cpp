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


#include "texttospeechappui.h"
#include "texttospeechengine.h"
#include "texttospeechsettingitemlistview.h"


/**
 * Construct the CTextToSpeechAppUi instance
 */ 
CTextToSpeechAppUi::CTextToSpeechAppUi()
    {
    }

/** 
 * The appui's destructor removes the container from the control
 * stack and destroys it.
 */
CTextToSpeechAppUi::~CTextToSpeechAppUi()
    {
    delete iTextToSpeechEngine;
    }

/**
 * Initialise container
 */
void CTextToSpeechAppUi::InitializeContainersL()
    {
    iTextToSpeechSettingItemListView = CTextToSpeechSettingItemListView::NewL();
    AddViewL( iTextToSpeechSettingItemListView );
    SetDefaultViewL( *iTextToSpeechSettingItemListView );
    }

/**
 * Handle a command for this appui (override)
 * @param aCommand command id to be handled
 */
void CTextToSpeechAppUi::HandleCommandL( TInt aCommand )
    {
    TBool commandHandled( EFalse );
    
    if ( !commandHandled ) 
        {
        if ( aCommand == EAknSoftkeyExit || aCommand == EEikCmdExit)
            {
            Exit();
            }
        }
    }

/** 
 * Override of the HandleResourceChangeL virtual function
 */
void CTextToSpeechAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknViewAppUi::HandleResourceChangeL( aType );
    }
                
/** 
 * Override of the HandleKeyEventL virtual function
 * @return EKeyWasConsumed if event was handled, EKeyWasNotConsumed if not
 * @param aKeyEvent 
 * @param aType 
 */
TKeyResponse CTextToSpeechAppUi::HandleKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                                  TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

/** 
 * Override of the HandleViewDeactivation virtual function
 *
 * @param aViewIdToBeDeactivated 
 * @param aNewlyActivatedViewId 
 */
void CTextToSpeechAppUi::HandleViewDeactivation( const TVwsViewId& aViewIdToBeDeactivated, 
                                                 const TVwsViewId &aNewlyActivatedViewId )
    {
    CAknViewAppUi::HandleViewDeactivation( aViewIdToBeDeactivated, 
                                           aNewlyActivatedViewId );
    }

/**
 * Completes the second phase of Symbian object construction. 
 */ 
void CTextToSpeechAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    
    InitializeContainersL();
    
    iTextToSpeechEngine = CTextToSpeechEngine::NewL();
    }

/**
* Returns pointer to application engine
*/
CTextToSpeechEngine* CTextToSpeechAppUi::AppEngine()
    {
    return iTextToSpeechEngine;
    }

// Enf of file
