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


#include <aknnavide.h> 
#include <texttospeech.rsg>

#include "texttospeechappui.h"
#include "texttospeechengine.h"
#include "texttospeechsettingitemlistview.h"
#include "texttospeechsettingitemlist.h"
#include "texttospeechsettingitemlistsettings.h"

#include "texttospeech.hrh"

const TInt KMaxVolumeInUI = 10;

/**
 * First phase of Symbian two-phase construction. Should not contain any
 * code that could leave.
 */
CTextToSpeechSettingItemListView::CTextToSpeechSettingItemListView()
    {
    }
    
/** 
 * The view's destructor removes the container from the control
 * stack and destroys it.
 */
CTextToSpeechSettingItemListView::~CTextToSpeechSettingItemListView()
    {
    }

/**
 * Symbian two-phase constructor.
 * This creates an instance then calls the second-phase constructor
 * without leaving the instance on the cleanup stack.
 * @return new instance of CTextToSpeechSettingItemListView
 */
CTextToSpeechSettingItemListView* CTextToSpeechSettingItemListView::NewL()
    {
    CTextToSpeechSettingItemListView* self = 
        CTextToSpeechSettingItemListView::NewLC();
        
    CleanupStack::Pop( self );
    
    return self;
    }

/**
 * Symbian two-phase constructor.
 * This creates an instance, pushes it on the cleanup stack,
 * then calls the second-phase constructor.
 * @return new instance of CTextToSpeechSettingItemListView
 */
CTextToSpeechSettingItemListView* CTextToSpeechSettingItemListView::NewLC()
    {
    CTextToSpeechSettingItemListView* self = 
        new (ELeave) CTextToSpeechSettingItemListView();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

/**
 * Second-phase constructor for view.  
 * Initialize contents from resource.
 */ 
void CTextToSpeechSettingItemListView::ConstructL()
    {
    BaseConstructL( 
        R_TEXT_TO_SPEECH_SETTING_ITEM_LIST_TEXT_TO_SPEECH_SETTING_ITEM_LIST_VIEW );        
    }
    
/**
 * @return The UID for this view
 */
TUid CTextToSpeechSettingItemListView::Id() const
    {
    return TUid::Uid( ETextToSpeechSettingItemListViewId );
    }

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
void CTextToSpeechSettingItemListView::HandleCommandL( TInt aCommand )
    {   
    TBool commandHandled( ETrue );
    CAknVolumeControl* control( NULL );
    
    switch ( aCommand )
        { // code to dispatch to the AknView's menu and CBA commands is generated here
        
        case ETextToSpeechSettingItemListViewMenuItem1Command:
        case EAknSoftkeyChange:
        
            iTextToSpeechSettingItemList->ChangeSelectedItemL();
        
            break;
        
        case ETextToSpeechSettingItemListViewSynthesiseMenuItemCommand:
            
            iEngine->SetSpeakingRateL( iSettings->Slider1() );
            iEngine->SetVoiceL( iSettings->iVoice ); 
            iEngine->InitAndPlayText( iSettings->Edit1() ); 
            
            break;
            
        case ETextToSpeechSettingItemListViewExitMenuItemCommand:
        
            AppUi()->HandleCommandL( EEikCmdExit );
            
            break;
        
        case EKeyIncVolume:
            
            iEngine->SetVolumeL( iSettings->iVolume + 
                                 iSettings->iMaxVolume / KMaxVolumeInUI );
            
            iSettings->iVolume = iSettings->iVolume + 
                                 iSettings->iMaxVolume / KMaxVolumeInUI;
            
            control = static_cast< CAknVolumeControl* >( 
                        iVolumeControl->DecoratedControl() );
            
            control->SetValue( KMaxVolumeInUI * 
                               iSettings->iVolume / iSettings->iMaxVolume );
            
            break;
            
        case EKeyDecVolume:
            
            iEngine->SetVolumeL( iSettings->iVolume - 
                                 iSettings->iMaxVolume / KMaxVolumeInUI );
            
            iSettings->iVolume = iSettings->iVolume - 
                                 iSettings->iMaxVolume / KMaxVolumeInUI;
            
            control = static_cast< CAknVolumeControl* >( 
                        iVolumeControl->DecoratedControl() );
                        
            control->SetValue( KMaxVolumeInUI * 
                               iSettings->iVolume / iSettings->iMaxVolume );
            
            break;
            
        default:
            
            commandHandled = EFalse;
            
            break;
            
        }
        
    if ( !commandHandled && aCommand == EAknSoftkeyExit )
        {
        AppUi()->HandleCommandL( EEikCmdExit );
        }
    }

/**
 * Handles Navidecorator events (Arrow left and arrow right)
 * @param aEventID ID of event to be handled
 */
void CTextToSpeechSettingItemListView::HandleNaviDecoratorEventL( TInt aEventID )
    {
    CAknVolumeControl* control( NULL );
    
    switch ( aEventID )
        {
        case EAknNaviDecoratorEventRightTabArrow: 
            
            HandleCommandL( EKeyIncVolume );
            
            break;
            
        case EAknNaviDecoratorEventLeftTabArrow:
            
            HandleCommandL( EKeyDecVolume );
            
            break;
            
        case EAknNaviDecoratorEventNaviLabel:
            
            control = static_cast< CAknVolumeControl* >( 
                        iVolumeControl->DecoratedControl() );
    
            if ( control )
                {
                TInt value = control->Value();

                iEngine->SetVolumeL( value * 
                                     iSettings->iMaxVolume / KMaxVolumeInUI );
                
                iSettings->iVolume = value * 
                                     iSettings->iMaxVolume / KMaxVolumeInUI ;
                }
                
            break;

        }
    }
    
/**
 * Activate view
 * @param aPrevViewId 
 * @param aCustomMessageId 
 * @param aCustomMessage
 */
void CTextToSpeechSettingItemListView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                                    TUid /*aCustomMessageId*/,
                                                    const TDesC8& /*aCustomMessage*/ )
    {
    CEikButtonGroupContainer *cba = AppUi()->Cba();
    if ( cba ) 
        {
        cba->MakeVisible( EFalse );
        }
    
    if ( !iTextToSpeechSettingItemList )
        {
        iEngine = (((CTextToSpeechAppUi*)AppUi())->AppEngine());
        
        iSettings = CTextToSpeechSettingItemListSettings::NewL();

        iTextToSpeechSettingItemList = 
            new (ELeave) CTextToSpeechSettingItemList( *iSettings, this );
        
        iTextToSpeechSettingItemList->SetMopParent( this );
        iTextToSpeechSettingItemList->ConstructFromResourceL( 
            R_TEXT_TO_SPEECH_SETTING_ITEM_LIST_TEXT_TO_SPEECH_SETTING_ITEM_LIST );
        
        iTextToSpeechSettingItemList->ActivateL();
        iTextToSpeechSettingItemList->LoadSettingValuesL();
        iTextToSpeechSettingItemList->LoadSettingsL();
        
        AppUi()->AddToStackL( *this, iTextToSpeechSettingItemList );
        
        SetupStatusPaneL(); 
        }
    }

/**
 * Deactivate view
 */
void CTextToSpeechSettingItemListView::DoDeactivate()
    {
    CleanupStatusPane();
    
    CEikButtonGroupContainer *cba = AppUi()->Cba();
    if ( cba ) 
        {
        cba->MakeVisible( ETrue );
        cba->DrawDeferred();
        }
    
    if ( iTextToSpeechSettingItemList )
        {
        AppUi()->RemoveFromStack( iTextToSpeechSettingItemList );
        
        delete iTextToSpeechSettingItemList;
        iTextToSpeechSettingItemList = NULL;
        
        delete iSettings;
        iSettings = NULL;
        }    
    }

/**
 * Show volume indicator in status pane
 */
void CTextToSpeechSettingItemListView::SetupStatusPaneL()
    {
    if ( !iVolumeControl )
        {
        CAknNavigationControlContainer* naviPane = 
            static_cast< CAknNavigationControlContainer* >( 
                iEikonEnv->AppUiFactory()->StatusPane()->ControlL( 
                    TUid::Uid( EEikStatusPaneUidNavi ) ) );

        iVolumeControl = 
            naviPane->CreateVolumeIndicatorL( R_AVKON_NAVI_PANE_VOLUME_INDICATOR );
        
        // Subscribe for changes in volume indicator 
        iVolumeControl->SetNaviDecoratorObserver( this );
        iVolumeControl->SetControlType( CAknNavigationDecorator::ENaviLabel );
        
        CAknVolumeControl* control = 
            static_cast< CAknVolumeControl* >( iVolumeControl->DecoratedControl() );
        
        control->SetValue( KMaxVolumeInUI * 
                           iSettings->iVolume / iSettings->iMaxVolume );

        naviPane->PushL( *iVolumeControl );
        } 
    }

/**
 * Delete volume indicator
 */
void CTextToSpeechSettingItemListView::CleanupStatusPane()
    {
    delete iVolumeControl;
    iVolumeControl = NULL;
    }

/** 
 * Handle status pane size change for this view (override)
 */
void CTextToSpeechSettingItemListView::HandleStatusPaneSizeChange()
    {
    CAknView::HandleStatusPaneSizeChange();
    
    // this may fail, but we're not able to propagate exceptions here
    TRAP_IGNORE( SetupStatusPaneL() ); 
    }
    
// End of file
