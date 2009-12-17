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
* Description:  Generic view class for vcommand app
*
*/


// INCLUDE FILES

#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <AknGlobalNote.h>
#include <AknWaitDialog.h>
#include <hlplch.h>

#include <ctsydomainpskeys.h>

#include <vcommand.rsg>
#include "vcappui.h"
#include "vcgenericview.h"
#include "vcfolderview.h"
#include "vcgenericcontainer.h" 
#include "vcommandconstants.h"
#include "vcommand.hrh"
#include "vcommandconstants.h"
#include "vcplaybackdialog.h"
#include "voiceuibldvariant.hrh"

#include <vcommanddomaincrkeys.h>
#include <centralrepository.h>

#include <coemain.h>
#include <apgcli.h>
#include <apgtask.h>
#include <apacmdln.h>

#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CVCGenericView::CVCGenericView
// C++ constructor
// ----------------------------------------------------------------------------
//
CVCGenericView::CVCGenericView( CVCModel& aModel ): 
    iModel( aModel )
    {   
    }
 
// ----------------------------------------------------------------------------
// CVCGenericView::ConstructL
// Two-phased constructor
// ----------------------------------------------------------------------------
//
void CVCGenericView::ConstructL( TInt aResourceId ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::ConstructL" );
    
    BaseConstructL( aResourceId );
    SetFolderTitleL( KNullDesC() );
    }

// ----------------------------------------------------------------------------
// CVCGenericView::~CVCGenericView
// Destructor
// ----------------------------------------------------------------------------
//
CVCGenericView::~CVCGenericView() 
    {
    delete iFolderName;
    
    if( iContainer ) 
        {  
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// TUid CVCGenericView::Id
// ----------------------------------------------------------------------------
//
TUid CVCGenericView::Id() const 
    {
    return KUidVCMainView;
    }

// ----------------------------------------------------------------------------
// CVCGenericView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CVCGenericView::HandleCommandL( TInt aCommand ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::HandleCommandL" );
    
    // We will only handle one event at a time
    if( !iContainer->IsHandlingKeyPressInProgress() )
        {
        iContainer->SetHandlingKeyPressInProgress( ETrue );
        DoHandleCommandL( aCommand );
        iContainer->SetHandlingKeyPressInProgress( EFalse );
        }
    }

// ----------------------------------------------------------------------------
// CVCGenericView::DoHandleCommandL
// ----------------------------------------------------------------------------
//
// Defines basic operations for listbox items. Those operations are: opening a folder,
// editing an item, playing an item and launching help. More sophisticated features
// can be defined in inherited classes.
//
void CVCGenericView::DoHandleCommandL( TInt aCommand ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::DoHandleCommandL" );
    
    switch ( aCommand ) 
        {
        case EAknSoftkeySelect: // Middle softkey
            {
            TInt current( iContainer->CurrentSelection() );
            
            // Msk can be pressed before the listbox is populated in which case
            // we ignore the key press
            if( current != KErrNotFound )
                {
                if( iContainer->IsItemFolderL( current ) )
                    {
                    DoHandleCommandL( EVCCmdOpen );
                    }
                else
                    {
                    DoHandleCommandL( EVCCmdEdit );
                    }                
                }
            
            break;
            }
        
        case EAknSoftkeyExit: // exit the application
            {
            static_cast<CVCAppUi*>( iEikonEnv->AppUi() )->ExitProgram();
            break;
            }
      
        case EAknSoftkeyBack: // back to main view from folder
            {
#ifdef __VCOMMAND_CONTROL_PANEL
            if( Id() == KUidVCMainView )
                {
                static_cast<CVCAppUi*>( iEikonEnv->AppUi() )->ExitProgram();
                break; 
                }
            else // Id() == KUidVCFolderView
                {
                AppUi()->ActivateLocalViewL( KUidVCMainView );
                break;
                }
#else
            AppUi()->ActivateLocalViewL( KUidVCMainView );
            break;
#endif
            }
      
        case EVCCmdOpen: // open folder
            {
            TInt current = iContainer->CurrentSelection();
            static_cast <CVCFolderView*>
                ( AppUi()->View( KUidVCFolderView ) )
                ->SetFolderTitleL( iContainer->GetFolderTitle( current ) );
            // Store the focused folder. Used when returning from folder view.
            iFocusedIndex = current;
            AppUi()->ActivateLocalViewL( KUidVCFolderView );
            
            break; 
            }

        case EVCCmdEdit: // edit voice command extra text
            {
            TRAPD(err,  iContainer->ShowEditTextL() );
            if ( err == KErrNoMemory || err == KErrDiskFull ) User::Leave(err);
            break;
            }

        case EVCCmdPlayback: // playback voice command
            {
            TRAP_IGNORE( PlaybackVoiceCommandL() );
            break;
            }

        case EVCCmdDelete: // delete a voice command
            {
            TRAP_IGNORE( iContainer->DeleteVCommandL() );
            break;
            }
     
        case EVCCmdSettings: // modify settings
            {
            iFocusedIndex = iContainer->CurrentSelection();
            AppUi()->ActivateLocalViewL( KUidVCSettingsView );
            break;
            }
   
        case EVCCmdStartVui: // start  in tutorial mode
            {
            TRAP_IGNORE( LaunchDemoVoiceUiL() );
            break;
            }
        
        case EAknCmdHelp: // launch help dialog 
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                                                 AppUi()->AppHelpContextL() );
            break;
            }
        
        case EVCCmdRemove: // item can not be removed
            {
            CannotDeleteNoteL();
            break;
            }
            
        case EVCCmdIgnore:
            {
            break; // do nothing
            }
        
        default: 
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }      
    }   

// ----------------------------------------------------------------------------
// CVCGenericView::LaunchDemoVoiceUiL
// ----------------------------------------------------------------------------
//
void CVCGenericView::LaunchDemoVoiceUiL() const
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::LaunchDemoVoiceUiL" );

    CRepository* client = CRepository::NewLC( KCRUidVCommandSettings );
    User::LeaveIfError( client->Set( KVuiDemoMode, ETrue ) );
    CleanupStack::PopAndDestroy( client );

    TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
    TApaTask apaTask = apaTaskList.FindApp( KVoiceUiUid );
    
    if ( apaTask.Exists() )
        {
        apaTask.BringToForeground();
        }
    else
        {
        RApaLsSession apaLsSession;
        User::LeaveIfError( apaLsSession.Connect() );
        
        TApaAppInfo appInfo;
        User::LeaveIfError( apaLsSession.GetAppInfo( appInfo, KVoiceUiUid ) );
        
        TFileName appName = appInfo.iFullName;
        CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();

        apaCommandLine->SetExecutableNameL( appName );
        apaCommandLine->SetCommandL( EApaCommandRunWithoutViews );
        User::LeaveIfError ( apaLsSession.StartApp( *apaCommandLine ) );
        CleanupStack::PopAndDestroy( apaCommandLine );
    
        apaLsSession.Close();
        }
    }
    
// ----------------------------------------------------------------------------
// CVCGenericView::CheckMiddleSoftkeyLabelL
// ----------------------------------------------------------------------------
//
void CVCGenericView::SetMiddleSoftkeyLabelL( TInt aIndex ) const
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::SetMiddleSoftkeyLabelL" );
    
    RemoveCommandFromMSK();
    
    if( iModel.Count() == 0 )
        {
        DoSetMiddleSoftKeyLabelL( R_TEXT_SOFTKEY_EMPTY, EVCCmdIgnore );
        }
    else if( iContainer->IsItemFolderL( aIndex ) )
        {
        DoSetMiddleSoftKeyLabelL( R_QTN_VC_OPEN, EVCCmdOpen );
        }
    else
        {
        DoSetMiddleSoftKeyLabelL( R_QTN_VC_EDIT, EVCCmdEdit );
        }
    }
    
// ----------------------------------------------------------------------------
// CVCGenericView::SetMiddleSoftKeyLabelL
// ----------------------------------------------------------------------------
//
void CVCGenericView::DoSetMiddleSoftKeyLabelL( const TInt aResourceId,
                                               const TInt aCommandId  ) const
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::DoSetMiddleSoftKeyLabelL" );
    
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        HBufC* mskText = StringLoader::LoadLC( aResourceId );
        TPtr mskPtr = mskText->Des();
        cbaGroup->AddCommandToStackL( KVcMskControlID, aCommandId, mskPtr );
        CleanupStack::PopAndDestroy( mskText );
        }
    }
    
// ----------------------------------------------------------------------------
// CVCGenericView::RemoveCommandFromMSK
// ----------------------------------------------------------------------------
//
void CVCGenericView::RemoveCommandFromMSK() const
    {
    if ( Cba() )
        {
        Cba()->RemoveCommandFromStack( KVcMskControlID, EVCCmdOpen );
        Cba()->RemoveCommandFromStack( KVcMskControlID, EVCCmdEdit );
        }
    }

// ----------------------------------------------------------------------------
// CVCGenericView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CVCGenericView::HandleClientRectChange() 
    {
    if ( iContainer ) 
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ----------------------------------------------------------------------------
// CVCGenericView::DoActivateL
// ----------------------------------------------------------------------------
//
void CVCGenericView::DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid, const TDesC8& ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::DoActivateL" );
    
    if( !iContainer ) 
        {
        iContainer = new ( ELeave ) CVCGenericContainer( MenuBar(), iModel, *this );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        
        iContainer->ActivateL();
        }
    
    // Returning from folder or settings view
    if( aPrevViewId.iViewUid.iUid == KUidVCFolderView.iUid ||
        aPrevViewId.iViewUid.iUid == KUidVCSettingsView.iUid )
        {
        // Set the focus on the folder that was opened previously
        iContainer->SetCurrentSelection( iFocusedIndex );
        }
    
    // Set msk label  
    SetMiddleSoftkeyLabelL( iContainer->CurrentSelection() );
    }

// ----------------------------------------------------------------------------
// CVCGenericView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CVCGenericView::DoDeactivate() 
    {
    if( iContainer ) 
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }
    

// ----------------------------------------------------------------------------
//  CVCGenericView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
void CVCGenericView::DynInitMenuPaneL( TInt aResourceId,
                                       CEikMenuPane* aMenuPane ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::DynInitMenuPaneL" );
    
    if( aMenuPane ) 
        {   
        // Check which menu item we are handling
        if( aResourceId == R_VCOMMAND_MENU ) 
            {
            // if help is not enabled, do not show "help"
            if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) ) 
                {
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                }
                
            TInt listBoxIndex( iContainer->CurrentSelection() );
            
            if( listBoxIndex >= 0 )
                {
                TBool isFolder = iContainer->IsItemFolderL( listBoxIndex );

                if( isFolder ) // Folder is highlighted.
                    {
                    aMenuPane->SetItemDimmed( EVCCmdPlayback, ETrue );
                    aMenuPane->SetItemDimmed( EVCCmdEdit, ETrue );
                    aMenuPane->SetItemDimmed( EVCCmdDelete, ETrue );
                    }
                else // Folder is not highlighted.
                    { 
                    aMenuPane->SetItemDimmed( EVCCmdOpen, ETrue );
                    
                    // Show delete item only if the command is user edited
                    TDesC* secondRow = iContainer->GetSecondRowNameLC(
                                       iContainer->CurrentSelection() );
                    if( secondRow->Length() == 0 )
                        {
                        aMenuPane->SetItemDimmed( EVCCmdDelete, ETrue );
                        }
                    CleanupStack::PopAndDestroy( secondRow );
                    }                
                }
            else // No items in listbox
                {
                aMenuPane->SetItemDimmed( EVCCmdPlayback, ETrue );
                aMenuPane->SetItemDimmed( EVCCmdEdit, ETrue );
                aMenuPane->SetItemDimmed( EVCCmdDelete, ETrue );
                aMenuPane->SetItemDimmed( EVCCmdOpen, ETrue );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
//  CVCGenericView::PlaybackVoiceCommandL
// ----------------------------------------------------------------------------
//
void CVCGenericView::PlaybackVoiceCommandL() 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::PlaybackVoiceCommandL" );
    
    CVCAppUi* appUi = static_cast <CVCAppUi*> ( iEikonEnv->AppUi() );
    
    TInt state = appUi->CheckCallStateL();
    TBool videoCallState = appUi->IsVideoCallL();
    
    if( ( state == EPSCTsyCallStateNone || state == EPSCTsyCallStateUninitialized ||
           state == EPSCTsyCallStateHold || state == KErrNotFound ) && !videoCallState ) 
        {        
        TInt listboxIndex = iContainer->CurrentSelection();
        TDesC* listboxItem = iContainer->GetFirstRowNameLC( listboxIndex );
        
        const CVCommandUiEntry& vcommand = iModel.GetVCommandL( *listboxItem );
        CleanupStack::PopAndDestroy( listboxItem );
        
        CVCPlaybackDialog* dlg = new (ELeave) CVCPlaybackDialog(
            iModel.Service(), vcommand );
        dlg->PrepareLC( R_PLAYBACK_VOICE_COMMAND_DIALOG );
        dlg->RunLD();
        dlg = NULL;
        }
    else 
        {
        // Display call in progress note
        appUi->CallInProgressNoteL();
        }
    }

// ----------------------------------------------------------------------------
// CVCGenericView::CannotDeleteNoteL
// ----------------------------------------------------------------------------
//
void CVCGenericView::CannotDeleteNoteL() 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::CannotDeleteNoteL" );
    
    TDesC* name;
    name = iContainer
        ->GetSecondRowNameLC( iContainer->CurrentSelection() );
    
    if( name->Length() == 0 )
        {
        CleanupStack::PopAndDestroy( name );
        name = iContainer
            ->GetFirstRowNameLC( iContainer->CurrentSelection() );
        }
    
    HBufC* stringholder = StringLoader::LoadL( R_QTN_VC_INFO_CANNOT_DEL,
                                               *name, iEikonEnv );
    CleanupStack::PopAndDestroy( name );
    CleanupStack::PushL( stringholder );
    
    CAknNoteWrapper* note = new ( ELeave ) CAknNoteWrapper();
    note->ExecuteLD( R_CANNOT_DELETE_NOTE, *stringholder );
    CleanupStack::PopAndDestroy( stringholder );
    }
    
// -----------------------------------------------------------------------------
// CVCGenericView::HandleSizeChange
// Handle screen size change.
// -----------------------------------------------------------------------------
//
void CVCGenericView::HandleSizeChange() 
    {
    if (iContainer) 
        {
        TRect genericPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, genericPaneRect );
        iContainer->SetRect( genericPaneRect );
        }
    }    

// -----------------------------------------------------------------------------
// CVCGenericView::FolderTitle
// Returns the title of the view's active folder.
// -----------------------------------------------------------------------------
//
const TDesC& CVCGenericView::FolderTitle() const 
    {
    return *iFolderName;
    }

// -----------------------------------------------------------------------------
// CVCGenericView::SetFolderTitleL
// -----------------------------------------------------------------------------
//
void CVCGenericView::SetFolderTitleL( const TDesC& aNewName ) 
    {
    delete iFolderName;
    iFolderName = NULL;
    iFolderName = aNewName.AllocL();    
    }

// -----------------------------------------------------------------------------
// CVCGenericView::RefreshViewL
// -----------------------------------------------------------------------------
//   
void CVCGenericView::RefreshViewL()
    {
    // This is the focused view
    if( iContainer )
        {
        CVCommandUiEntryArray* oldCommands = iModel.VCommandArrayL();
        CleanupStack::PushL( oldCommands );

        // Update model
        iModel.LoadVCommandsL();
            
        CVCommandUiEntryArray* newCommands = iModel.VCommandArrayL();
        CleanupStack::PushL( newCommands );
    
        iContainer->RefreshListboxL( *oldCommands, *newCommands );
    
        CleanupStack::PopAndDestroy( newCommands );
        CleanupStack::PopAndDestroy( oldCommands );  
        }
    }

// End of File
