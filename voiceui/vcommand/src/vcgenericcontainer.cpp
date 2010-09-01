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
* Description:  Container class for vcommand app
*
*/


// INCLUDE FILES

#include <StringLoader.h> 
#include <avkon.hrh>
#include <aknlists.h>
#include <eikclbd.h>
#include <aknkeys.h>
#include <fbs.h>
#include <eiklbd.h>
#include <eiklbx.h>
#include <eikclbd.h>
#include <eiklbi.h>
#include <aknlists.h> 
#include <aknPopup.h>
#include <akntitle.h>
#include <eikspane.h>
#include <aknlistquerydialog.h>
#include <sysutil.h>
#include <apgcli.h>
#include <featmgr.h>
#include <AknsUtils.h>
#include <s32strm.h>
#include <f32file.h>
#include <s32file.h>
#include <AknUtils.h>
#include <apgcli.h>
#include <collate.h>

#include <vcommand.rsg>
#include <vcommand.mbg>
#include "vcgenericcontainer.h"
#include "vcgenericview.h"
#include "vcommanduientry.h"
#include "uiarraysgenerator.h"
#include "vctextquerydialog.h"
#include "vcdoublelargestylelistbox.h"

#include "vcommand.hrh"
#include "vcappui.h"
#include "rubydebug.h"


// CONSTANTS
const TChar KTabulator = '\t';
const TInt KArrayGranularity( 12 );
// Show tooltip for four seconds
const TInt KPopupVisibilityTime = 4000;
_LIT( KLocReplace, "%U" );
// Bytes that should be free in the disk drive in addition to critical level 
// before voice command editing is allowed (10k)
const TInt KEditDiskUsage = 10240;

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
CVCGenericContainer::CVCGenericContainer( CEikMenuBar* aMenuBar,
                                          CVCModel& aModel,
                                          CVCGenericView& aView ):
    iMenuBar( aMenuBar ), iView( aView ), iModel( aModel )
    { 
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::ConstructL( const TRect& aRect ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::ConstructL" );
    
    SetTitlePaneL( R_VC_MAIN_VIEW_TITLE );
    CreateWindowL();

    iPopupController = CAknInfoPopupNoteController::NewL();
    iPopupController->SetTimePopupInView( KPopupVisibilityTime );
    
    ConstructListBoxL();

    SetRect( aRect );
    SetObserver( this );
    ActivateL();
    } 


// ----------------------------------------------------------------------------
// CVCGenericContainer::~CVCGenericContainer
// ----------------------------------------------------------------------------
//
CVCGenericContainer::~CVCGenericContainer() 
    {
	delete iFolderTitles;
	iItemIsFolder.Close();
	delete iPopupController;
    
    if ( iListBox )
        {
        static_cast<CVCDoubleLargeStyleListBox*>( iListBox )->AddScrollBarObserver( NULL );
        }
    delete iListBox; 
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::SetTitlePaneL
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::SetTitlePaneL(TInt aResourceId) const 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::SetTitlePaneL" );
    
    // Set up status pane
    CEikStatusPane* statusPane =
        ( static_cast<CAknAppUi*> ( iCoeEnv->AppUi() ) )->StatusPane();
    CAknTitlePane* title = 
        static_cast<CAknTitlePane*>( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC( rReader, aResourceId );
    title->SetFromResourceL( rReader );
    
    // Set title for folder views
    if( iView.FolderTitle() != KNullDesC() )
        {
        title->SetTextL( iView.FolderTitle() );
        }
    
    CleanupStack::PopAndDestroy(); // rReader
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CVCGenericContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                  TEventCode aType ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::OfferKeyEventL" );
    
    if( iHandlingKeyPressInProgress )
        {
        return EKeyWasConsumed;
        }
     
    // Special case when for some reason vcommand database is empty.
    // Only exiting is allowed
    if( ListBoxCount() <= 0 && aKeyEvent.iCode != EKeyCBA2 ) 
        {
        return EKeyWasConsumed;
        }
     
    SetHandlingKeyPressInProgress( ETrue );
    TKeyResponse retval( EKeyWasConsumed );
    TBool isFolder = IsItemFolderL( CurrentSelection() );
        
    switch ( aKeyEvent.iCode ) 
        {
        case EKeyBackspace: // C - key. Deletes an application
            {
            if ( iView.MenuBar()->ItemSpecificCommandsEnabled() )
                {
                TDesC* secondRow = GetSecondRowNameLC( CurrentSelection() );
                if( !isFolder && secondRow->Length() > 0 ) 
                    {
                    SetHandlingKeyPressInProgress( EFalse );
                    iView.HandleCommandL( EVCCmdDelete );    
                    }
                else 
                    {
                    SetHandlingKeyPressInProgress( EFalse );
                    iView.HandleCommandL( EVCCmdRemove );
                    }
                CleanupStack::PopAndDestroy( secondRow );
                }
            break;  
            }

        case EKeyUpArrow: 
        case EKeyDownArrow: 
            {
            retval = iListBox->OfferKeyEventL( aKeyEvent, aType );
            TRAP_IGNORE(
                ShowTooltipL( CurrentSelection() );
                iView.SetMiddleSoftkeyLabelL( CurrentSelection() ) );
            break;
            }
            
        case EKeyCBA2:
            {
            if( iView.FolderTitle() == KNullDesC() ) 
                {
                iView.HandleCommandL( EAknSoftkeyExit );
                }
            }
        
        default: 
            {
            retval = iListBox->OfferKeyEventL( aKeyEvent, aType );
            break;
            }
        } 
        
    SetHandlingKeyPressInProgress( EFalse );
    return retval;
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::IsItemFolderL
// ----------------------------------------------------------------------------
//
TBool CVCGenericContainer::IsItemFolderL( TInt aListBoxIndex ) const 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::IsItemFolderL" );
                  
    if( aListBoxIndex >= 0 && aListBoxIndex < iItemIsFolder.Count() )
        {
        return iItemIsFolder[aListBoxIndex];
        }
    else
        {
        return false;
        }
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::ConstructListBoxL
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::ConstructListBoxL() 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::ConstructListBoxL" );
    
    CVCAppUi* appUi = static_cast<CVCAppUi*>( iEikonEnv->AppUi() );
        
    iListBox = CVCDoubleLargeStyleListBox::NewL();
    iListBox->SetContainerWindowL( *this ); // set's iListBox to be contained in this container
    
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iListBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
    // Ownership not transferred
    iItemArray = 
        static_cast<CDesC16ArrayFlat*>( iListBox->Model()->ItemTextArray() );
    
    // Folder title array    
    iFolderTitles = new (ELeave) CDesC16ArrayFlat( KArrayGranularity );
        
    // Populates the listbox
    PopulateListBoxL();
    
    CEikFormattedCellListBox* listbox = static_cast<CEikFormattedCellListBox*>( iListBox );
    
    // Set listbox observer
    listbox->SetListBoxObserver( this );
    
    static_cast<CVCDoubleLargeStyleListBox*>( iListBox )->AddScrollBarObserver( this );
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::PopulateListBoxL
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::PopulateListBoxL() 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::PopulateListBoxL" );
    
    CAknIconArray* icons = new ( ELeave ) CAknIconArray( KArrayGranularity );
    CleanupStack::PushL( icons );
    
    CUiArraysGenerator* generator = CUiArraysGenerator::NewLC();
    generator->FillArraysL( iModel, iView.FolderTitle(), *icons, *iFolderTitles, 
                            iItemIsFolder, *iItemArray );
    
    CleanupStack::PopAndDestroy( generator );

    CEikFormattedCellListBox* listbox = static_cast<CEikFormattedCellListBox*>( iListBox );
    // Takes ownership of icons
    listbox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    CleanupStack::Pop( icons );

    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                         CEikScrollBarFrame::EAuto );
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::SortItemListL
// Insertion sort implementation
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::SortItemListL() 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::SortItemListL" );
    
    // Case insentitive comparing
    TCollationMethod cm = *Mem::CollationMethodByIndex( 0 ); // Default collation method
    cm.iFlags |= TCollationMethod::EFoldCase;
    
    TInt i( 1 );
    while( i < iItemArray->Count() )
        {
        TDesC* strToCompare = GetFirstRowNameLC( i );
        TBool isFolder = IsItemFolderL( i );
        
        TDesC* firstRow( NULL );
        TInt j = i - 1;
        // Previous item is greater than strToCompare or previous is not a
        // folder and current item is
        while( j >= 0 &&
               ( (firstRow = GetFirstRowNameLC( j ))->CompareC( *strToCompare, 0, &cm ) > 0 || 
               isFolder && !IsItemFolderL( j ) ) ) 
            {
            CleanupStack::PopAndDestroy( firstRow );
            // Pointers are expected to be on the cleanup stack if they are non-null
            firstRow = NULL;
            if( !isFolder && IsItemFolderL( j ) ) // folders are first on the list
                {
                break;
                }
            j--;
            }
        // Pointers popped and destroyed are nullified, so non-null pointers
        // are still on the cleanup stack
        if( firstRow )
            {
            CleanupStack::PopAndDestroy( firstRow );
            }
        j++; // the last index which satisfied the conditions
        
        if( j < i ) 
            {
            iItemArray->InsertL( j, iListBox->Model()->ItemText( i ) );
            iItemArray->Delete( i + 1 );
            
            // No need to sort iItemIsFolder. Folders are in the beginning
            // and other items are in the end. This hasn't changed.
            }

        i++;
        CleanupStack::PopAndDestroy( strToCompare );
        }
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                               TListBoxEvent aEventType ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::HandleListBoxEventL" );
    
    switch ( aEventType ) 
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked: 
            {
            if( IsItemFolderL( CurrentSelection() ) ) 
                {
                SetHandlingKeyPressInProgress( EFalse );
                iView.HandleCommandL( EVCCmdOpen );
                }
            else
                {
                SetHandlingKeyPressInProgress( EFalse );
                iView.HandleCommandL( EVCCmdPlayback );
                ShowTooltipL( CurrentSelection() );
                }
            break;
            }
            
        default:
            {
            ShowTooltipL( -1 );
            }
        
        break;
        }
    }


// ----------------------------------------------------------------------------
//  CVCGenericContainer::HandleScrollEventL
// ----------------------------------------------------------------------------
void CVCGenericContainer::HandleScrollEventL( CEikScrollBar* /*aScrollBar*/,
                                              TEikScrollEvent /*aEventType*/ )
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericView::HandleScrollEventL" );
    
    ShowTooltipL( -1 );
    }


// ----------------------------------------------------------------------------
//  CVCGenericContainer::DeleteVoiceCommandL
// ----------------------------------------------------------------------------
void CVCGenericContainer::DeleteVCommandL() 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::DeleteVCommandL" );
    
    // Get corresponding CVCommand-object
    TDesC* firstRow = GetFirstRowNameLC( CurrentSelection() );
    
    const CVCommandUiEntry& vcommand = iModel.GetVCommandL( *firstRow );
    CleanupStack::PopAndDestroy( firstRow );
    
    TDesC* secondRow = GetSecondRowNameLC( CurrentSelection() );
    
    // Show "cannot delete" note if the voice command is not user edited
    if ( secondRow->Length() <= 0 ||
        ( vcommand.UserText() == KNullDesC() &&
          vcommand.WrittenText() != vcommand.SpokenText() ) )
        {
        CleanupStack::PopAndDestroy( secondRow );
        return iView.DoHandleCommandL( EVCCmdRemove );    
        }
    else
        {        
        HBufC* stringholder = StringLoader::LoadL( R_QTN_VC_DELETE_COMMAND, *secondRow, iEikonEnv );
        CleanupStack::PushL( stringholder );
        
        CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
         
        if ( dlg->ExecuteLD( R_REMOVE_APPLICATION_DIALOG, *stringholder ) ) // not cancelled: ret != 0
            {
            UpdateTextL( CurrentSelection(), KNullDesC(), *secondRow );
            }
        
        CleanupStack::PopAndDestroy( stringholder );
        CleanupStack::PopAndDestroy( secondRow );
        }
    }


// ----------------------------------------------------------------------------
//  CVCGenericContainer::ShowTooltipL
// ----------------------------------------------------------------------------
void CVCGenericContainer::ShowTooltipL( TInt aItemIndex ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::ShowTooltipL" );
    
    if( aItemIndex >= 0 && aItemIndex < iItemArray->Count() &&
        !IsItemFolderL( aItemIndex ) )
        {
        TDesC* firstRow = GetFirstRowNameLC( CurrentSelection() );
        
        const CVCommandUiEntry& vcommand = iModel.GetVCommandL( *firstRow );
        
        CleanupStack::PopAndDestroy( firstRow );

        TName ttText = vcommand.Tooltip();
        
        const TDesC& writtenText = vcommand.WrittenText();
        TDesC* spokenText = vcommand.AlternativeSpokenText().AllocLC();
        // Command is not user edited
        if( *spokenText == KNullDesC() )
            {
            CleanupStack::PopAndDestroy( spokenText );
            spokenText = vcommand.SpokenText().AllocLC();
            }
            
        TName ttText2;    
        if( ttText.Find( KLocReplace ) != KErrNotFound )
            {
            // Replace %U with the vc api spoken text
            StringLoader::Format( ttText2, ttText, -1, *spokenText );
            ttText.Copy( ttText2 );
            }
        else
            {
            // Replace %0U with the vc api spoken text
            StringLoader::Format( ttText2, ttText, 0, *spokenText );
            // Replace %1U with the vc api written text
            StringLoader::Format( ttText, ttText2, 1, writtenText );            
            }
            
        iPopupController->SetTextL( ttText );
        iPopupController->RestoreDefaultPosition();
        iPopupController->ShowInfoPopupNote();
        
        CleanupStack::PopAndDestroy( spokenText );
        }
    else 
        {
        iPopupController->HideInfoPopupNote();
        }
    }
    

// ----------------------------------------------------------------------------
// CVCGenericContainer::IsHandlingKeyPressInProgress
// ----------------------------------------------------------------------------
//                   
TBool CVCGenericContainer::IsHandlingKeyPressInProgress()
    {
    return iHandlingKeyPressInProgress;
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::SetHandlingKeyPressInProgress
// ----------------------------------------------------------------------------
//                           
void CVCGenericContainer::SetHandlingKeyPressInProgress( TBool aValue )
    {
    iHandlingKeyPressInProgress = aValue;
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::CountComponentControls
// ----------------------------------------------------------------------------
//
TInt CVCGenericContainer::CountComponentControls() const 
    {
    return 1; // return nbr of controls inside this container
    }
    

// ----------------------------------------------------------------------------
// CVCGenericContainer::HandleControlEventL(
//     (CCoeControl* aControl,TCoeEvent aEventType)
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::HandleControlEventL( CCoeControl* /*aControl*/, 
                                                   TCoeEvent /*aEventType*/ )    
    {
    // Do nothing
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::ShowEditTextL()
// Gets the application name from listbox and extracts it.
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::ShowEditTextL()
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::ShowEditTextL" );
    
    if( !CheckCriticalLevelL() )
        {
        TBuf<KMaxEditableTextLength> voiceCmd;
        TInt index = CurrentSelection();
        TDesC* secondRow = GetSecondRowNameLC( index );
        
        // 
        if( secondRow->Length() <= 0 ) 
            {
            TDesC* firstRow = GetFirstRowNameLC( index );
            const CVCommand* orgCommand = &iModel.GetVCommandL( *firstRow ).Command();
            voiceCmd.Append( orgCommand->SpokenText() );
            CleanupStack::PopAndDestroy( firstRow );
            }
        else 
            {
            voiceCmd.Append( *secondRow );
            }

        CleanupStack::PopAndDestroy( secondRow );

        TBuf<KMaxEditableTextLength> spokenText;
        spokenText.Copy( voiceCmd );
        
        // Save the currently selected command
        TDesC* firstRow = GetFirstRowNameLC( index );
        const CVCommand* orgCommand = &iModel.GetVCommandL( *firstRow ).Command();
        CleanupStack::PopAndDestroy( firstRow );
        
        CVCommand* command = CVCommand::NewL( *orgCommand );
        CleanupStack::PushL( command );            

        // Create the data query dialog        
        CVCTextQueryDialog* dlg = CVCTextQueryDialog::NewL( voiceCmd );
        // Enable T9
        dlg->SetPredictiveTextInputPermitted( ETrue );

        TBool res;
        // Check whether Japanese is in use
        if ( FeatureManager::FeatureSupported( KFeatureIdJapanese )
             && User::Language() == ELangJapanese )
            {
            res = ( dlg->ExecuteLD( R_VC_DATA_QUERY_JAPANESE ) == EAknSoftkeyOk );
            }
        else
            {
            res = ( dlg->ExecuteLD( R_VC_DATA_QUERY ) == EAknSoftkeyOk );
            }
        dlg = NULL;
        
        // User didn't cancel and the index is still in correct range
        if ( res && index < iItemArray->Count() )
            {
            // Get current command
            firstRow = GetFirstRowNameLC( index );
            const CVCommand* currentCommand = &iModel.GetVCommandL( *firstRow ).Command();
            CleanupStack::PopAndDestroy( firstRow );
            
            // If only whitespace was given, TrimAll modifies the name into an empty string
            voiceCmd.TrimAll();
            
            if ( voiceCmd.Length() )
                {
                // The selected command is the same and the name was changed
                if ( *command == *currentCommand && spokenText != voiceCmd )
                    {
                    if( !CheckNameValidityL( voiceCmd ) ) // New name base was valid. Exit from query
                        {
                        UpdateTextL( index, voiceCmd, spokenText );
                        }
                    else
                        {
                        ShowEditTextL(); // Query is shown again
                        }
                    }
                }
            else
                {
                // Remove command if possible since the inserted string was empty
                TDesC* secondRow = GetSecondRowNameLC( index );                
                if( !IsItemFolderL( index ) && secondRow->Length() > 0 ) 
                    {
                    SetHandlingKeyPressInProgress( EFalse );
                    iView.HandleCommandL( EVCCmdDelete );    
                    }
                CleanupStack::PopAndDestroy( secondRow );
                }
            }
        CleanupStack::PopAndDestroy( command );
        }
    }

// ----------------------------------------------------------------------------
// CVCGenericContainer::CheckCriticalLevelL
// Checks for CL disk space
// ----------------------------------------------------------------------------
//
TBool CVCGenericContainer::CheckCriticalLevelL()
    {
    RFs session = iCoeEnv->FsSession();
    TBool noMemory = SysUtil::FFSSpaceBelowCriticalLevelL( &session, KEditDiskUsage );
   
    if ( noMemory )
        {
        CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
        dlg->ExecuteLD( R_NOT_ENOUGH_MEMORY_DIALOG );
        }
        
    return noMemory;
    }

// ----------------------------------------------------------------------------
// CVCGenericContainer::UpdateTextL
// Sets the new application name to the list
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::UpdateTextL( TInt aIndex, const TDesC& aNewName,
                                       const TDesC& aOldName ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::UpdateTextL" );
    
    TDesC* firstRow = GetFirstRowNameLC( aIndex );
    
    if( iModel.GetVCommandL( *firstRow ).UserText() != aNewName )
        {
        TPtrC itemText = iListBox->Model()->ItemText( aIndex );
        TInt tabIndex = itemText.Locate( KTabulator );
        
        TBuf<KVCMaxNameSize> applicationName;
        
        applicationName.Append( itemText.Left( tabIndex ) );
        applicationName.Append( KTabulator );
        
        applicationName.Append( *firstRow );
    	applicationName.Append( KTabulator );
        
        // Add user edited text to second line
    	if( aNewName.Size() > 0 ) 
    	    {
            applicationName.Append( aNewName );
    	    }
    	    
    	CVCAppUi* appUi = static_cast <CVCAppUi*> ( iEikonEnv->AppUi() );
    	
    	HBufC* stringholder;
        // We are removing the edited command
        if( aNewName == KNullDesC() )
            {
            stringholder = StringLoader::LoadL( R_QTN_VC_WAIT_NOTE_REMOVING, aOldName, iEikonEnv );
            CleanupStack::PushL( stringholder );
            appUi->ShowWaitNoteL( *stringholder );
            }
        // We are modifying a command
        else
            {
            stringholder = StringLoader::LoadLC( R_QTN_VC_WAIT_NOTE_CREATING_COMMAND, iEikonEnv );
            appUi->ShowWaitNoteL( *stringholder );
            }
        
        // Update extra text field for VCommand
        TRAPD( error, iModel.SetNewSpokenTextL( *firstRow, aNewName ) );
        // Untrainable text
        if( error == KErrArgument || error == KErrGeneral )
            {
            appUi->DeleteWaitNoteL();
            
            appUi->ShowErrorNoteL( R_QTN_VC_ERROR_NOTE_INVALID_TEXT );
            
            ShowEditTextL();
            }
        else
            {
            appUi->DeleteWaitNoteL();
            User::LeaveIfError( error );
            
            // Update listbox
            iItemArray->Delete( aIndex );
            iItemArray->InsertL( aIndex, applicationName );    
            
            iListBox->HandleItemAdditionL();
    
            }
                
        CleanupStack::PopAndDestroy( stringholder );
        }
    
    CleanupStack::PopAndDestroy( firstRow );
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::CurrentSelection
// ----------------------------------------------------------------------------
//
TInt CVCGenericContainer::CurrentSelection() const 
    {   
    return iListBox->CurrentItemIndex();
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::SetCurrentSelection
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::SetCurrentSelection( TInt aIndex )
    {   
    if ( aIndex >= 0 && aIndex < iItemArray->Count() )
        {
        iListBox->SetCurrentItemIndexAndDraw( aIndex );
        }
    else if ( iItemArray->Count() > 0 )
        {
        iListBox->SetCurrentItemIndexAndDraw( 0 );
        }
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::SizeChanged() 
    {
    if( iListBox ) 
        {
        iListBox->SetRect( Rect() );
        }
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::FocusChanged
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::FocusChanged( TDrawNow aDrawNow ) 
    {
    if( iListBox ) 
        {
        iListBox->SetFocus( IsFocused(), aDrawNow );
        }
    }
    

// ----------------------------------------------------------------------------
// CVCGenericContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CVCGenericContainer::ComponentControl( TInt /*aIndex*/ ) const 
    {
    return iListBox;
    }


// --------------------------------------------------------------------------- 
// CVCGenericContainer::GetHelpContext
// Gives the help context to be displayed
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::GetHelpContext( TCoeHelpContext& aContext ) const 
    {
    aContext.iMajor = KUidHelp;
    if ( iView.Id() == KUidVCMainView )
        {
        aContext.iContext = KHLP_VC_SIND_MAIN;
        }
    else
        {
        aContext.iContext = KHLP_VC_SIND_FOLDER;
        }
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::CheckNameValidityL
// ----------------------------------------------------------------------------
//
TBool CVCGenericContainer::CheckNameValidityL( const TDesC& aName ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::CheckNameValidityL" );
    
    TBool nameFound = EFalse;
    CVCAppUi* appUi = static_cast < CVCAppUi* > ( iEikonEnv->AppUi() );

    for( TInt i( 0 ); i < iModel.Count(); i++ ) 
        {        
        if( aName.Compare( iModel.At( i ).WrittenText() ) == 0 ||
            aName.Compare( iModel.At( i ).SpokenText() ) == 0 ) 
            {
            nameFound = ETrue;
            break;
            }
            
        if( aName.Compare( KNullDesC() ) != 0 &&
            aName.Compare( iModel.At( i ).UserText() ) == 0 )
            {
            nameFound = ETrue;
            break;
            }
        }

    if( nameFound ) 
        {    
        appUi->NameAlreadyInUseNoteL( aName );
        }

    return nameFound;
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::GetFirstRowNameLCC
// ----------------------------------------------------------------------------
//
TDesC* CVCGenericContainer::GetFirstRowNameLC( TInt aIndex ) const
    {    
    TBuf<KVCMaxNameSize> name;
    name.Zero();

    // Get the application name
    TPtrC first = iListBox->Model()->ItemText( aIndex );
    
    // Locate the first tab char
    TInt index = first.Locate( KTabulator );
    
    // delete the icon from the beginning and get the name
    TPtrC second = first.Right( first.Length() - index - 1 );

    // check if the name has more tab chars in it
    index = second.Locate( KTabulator );

    if( index == second.Length()-1 || index == KErrNotFound ) 
        {
        name.Append( second );
        AknTextUtils::StripCharacters( name, KAknStripListControlChars );
        }
    else 
        {
        TInt reverse = second.LocateReverse( KTabulator );
        
        if( index == reverse ) 
            {
        
            TPtrC third = second.Left( index );
            name.Append( third );
            AknTextUtils::StripCharacters( name, KAknStripListControlChars );
            }
        }

    return name.AllocLC();
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::GetSecondRowNameLC
// ----------------------------------------------------------------------------
//
TDesC* CVCGenericContainer::GetSecondRowNameLC( TInt aIndex ) const
    {    
    TBuf<KVCMaxNameSize> secondRow;
    
    TPtrC temp = iListBox->Model()->ItemText( aIndex );
    
    TPtrC text = temp.Right( temp.Length() - 2 );
    TInt foundTab = text.Locate( KTabulator );

    if( foundTab != KErrNotFound ) 
        {
        TInt index = text.LocateReverse( KTabulator );
        secondRow.Append( text.Right( text.Length() - index - 1 ) );
        AknTextUtils::StripCharacters( secondRow, KAknStripListControlChars );
        }
    return secondRow.AllocLC();
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::GetFolderTitle
// ----------------------------------------------------------------------------
//    
TPtrC CVCGenericContainer::GetFolderTitle( TInt aIndex )
    {
    return (*iFolderTitles)[aIndex];
    }



// ----------------------------------------------------------------------------
// CVCGenericContainer::ListBoxSize
// ----------------------------------------------------------------------------
//
TInt CVCGenericContainer::ListBoxCount() const
    {
    if( !iListBox ) 
        {
        return 0;
        }
    return iItemArray->Count();
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::HandleResourceChange
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::HandleResourceChangeL( TInt aType ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::HandleResourceChangeL" );
    
    CCoeControl::HandleResourceChange( aType );        
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::RefreshListboxL
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::RefreshListboxL( CVCommandUiEntryArray& oldCommands,
                                           CVCommandUiEntryArray& newCommands )
    {
    RUBY_DEBUG_BLOCKL( "CVCGenericContainer::RefreshListboxL" );
    
    TInt currentIndex = CurrentSelection();
    
    RArray<CVCommandUiEntry> commands = FindCommandsToBeAddedL( oldCommands,
                                                         newCommands );
    CleanupClosePushL( commands );
    AddCommandsL( commands );
    CleanupStack::PopAndDestroy();
        
    commands = FindCommandsToBeRemovedL( oldCommands, newCommands );
    CleanupClosePushL( commands );
    RemoveCommands( commands );                                  
    CleanupStack::PopAndDestroy( &commands );

    // Owned by listbox
    CAknIconArray* icons = (CAknIconArray*) iListBox->ItemDrawer()->ColumnData()->IconArray();

    CUiArraysGenerator* generator = CUiArraysGenerator::NewLC();
    generator->UpdateFolderArraysL( iModel, iView.FolderTitle(), *icons,
                                    *iFolderTitles, iItemIsFolder, *iItemArray );
    
    CleanupStack::PopAndDestroy( generator );
    
    iListBox->HandleItemAdditionL();
    iListBox->HandleItemRemovalL();
    
    SortItemListL();
    
    if ( currentIndex >= iItemArray->Count() )
        {
        if (iItemArray->Count() > 0)
            {       
            SetCurrentSelection( iItemArray->Count() - 1 );
            }
        else
            {
            // group has been renamed (ui language change) -> go back to main menu
            iView.DoHandleCommandL( EAknSoftkeyBack );
            }
        }
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::FindCommandLsToBeAddedL
// ----------------------------------------------------------------------------
//
RArray<CVCommandUiEntry> CVCGenericContainer::FindCommandsToBeAddedL(
    CVCommandUiEntryArray& oldCommands, CVCommandUiEntryArray& newCommands )
    {
    RArray<CVCommandUiEntry> addedCmds;
    CleanupClosePushL( addedCmds );
    
    // Compare items in newCommands with the items in oldCommands and see
    // which are not found. These should be added to the listbox.
    for( TInt i( 0 ); i < newCommands.Count(); i++ )
        {
        const CVCommandUiEntry& cmd = newCommands[i];
        TBool found( EFalse );
        
        for( TInt j( 0 ); j < oldCommands.Count(); j++ )
            {
            const CVCommandUiEntry& compareCmd = oldCommands[j];
                        
            if( cmd == compareCmd )
                {
                found = ETrue;
                break;
                }
            }
            
        if( !found )
            {
            addedCmds.AppendL( cmd );
            }
        }
        
    CleanupStack::Pop();
    return addedCmds;
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::AddCommandsL
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::AddCommandsL( RArray<CVCommandUiEntry>& aCommands )
    {
    CEikFormattedCellListBox* listbox
        = static_cast<CEikFormattedCellListBox*>( iListBox );
    CAknIconArray* icons
        = static_cast<CAknIconArray*>(listbox->ItemDrawer()->ColumnData()->IconArray());
    
    // Update the listbox and the icon array
    for( TInt i( 0 ); i < aCommands.Count(); i++ )
        {
        CVCommandUiEntry& cmd = aCommands[i];
        
        // Check the folder
        if( cmd.FolderTitle() == iView.FolderTitle() )
            {
            icons->AppendL( cmd.IconLC() );
            CleanupStack::Pop(); // IconLC
            
            CVCommandUiEntry& cmd = aCommands[i];
            
            // Append to the end
            iItemArray->AppendL( *CreateListboxItemLC( icons->Count()-1, // Last index
                                                       cmd.WrittenText(),
                                                       KNullDesC() ) );
            CleanupStack::PopAndDestroy(); // CreateListboxItemLC
            
            iItemIsFolder.Append( EFalse );
            }
        }
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::FindCommandLsToBeRemovedL
// ----------------------------------------------------------------------------
//
RArray<CVCommandUiEntry> CVCGenericContainer::FindCommandsToBeRemovedL(
    CVCommandUiEntryArray& oldCommands, CVCommandUiEntryArray& newCommands )
    {
    RArray<CVCommandUiEntry> removedCmds;
    CleanupClosePushL( removedCmds );
        
    // Compare items in oldCommands with the items in newCommands and see
    // which are not found. These should be removed from the listbox.
    for( TInt i( 0 ); i < oldCommands.Count(); i++ )
        {
        const CVCommandUiEntry& cmd = oldCommands[i];
        TBool found( EFalse );
        
        for( TInt j( 0 ); j < newCommands.Count(); j++ )
            {
            const CVCommandUiEntry& compareCmd = newCommands[j];
                        
            if( cmd == compareCmd )
                {
                found = ETrue;
                break;
                }
            }
            
        if( !found )
            {
            removedCmds.AppendL( cmd );
            }
        }

    CleanupStack::Pop();
    return removedCmds;
    }


// ----------------------------------------------------------------------------
// CVCGenericContainer::RemoveCommands
// ----------------------------------------------------------------------------
//
void CVCGenericContainer::RemoveCommands( RArray<CVCommandUiEntry>& aCommands )
    {
    CEikFormattedCellListBox* listbox
        = static_cast<CEikFormattedCellListBox*>( iListBox );
    CAknIconArray* icons
        = static_cast<CAknIconArray*>(listbox->ItemDrawer()->ColumnData()->IconArray());
    
    // Update the listbox
    for( TInt i( 0 ); i < aCommands.Count(); i++ )
        {
        CVCommandUiEntry& cmd = aCommands[i];
        
        // Check the folder
        if( cmd.FolderTitle() == iView.FolderTitle() )
            {
            for( TInt j( 0 ); j < iItemArray->Count(); j++ )
                {
                TPtrC16 item = (*iItemArray)[j];
                
                // Written text is unique. When changing a spoken text we 
                // check that the new spoken text is not a written text
                // for any command. That is why it is enough to search for
                // the written text in the item here
                if( item.Find( cmd.WrittenText() ) != KErrNotFound )
                    {
                    iItemArray->Delete( j );
                    iItemArray->Compress();
                    }
                }
                
            // Icon array now has excess items, but that is no problem.
            // It is more efficient to leave the icons as they are.
            }
        }
    }
    

// ----------------------------------------------------------------------------
// CVCGenericContainer::SearchItemArrayL
// ----------------------------------------------------------------------------
//
TInt CVCGenericContainer::SearchItemArrayL( const TDesC& aText  )
    {
    TInt ret( KErrNotFound );
    
    for( TInt i( 0 ); i < iItemArray->Count(); i++ )
        {
        if( (*iItemArray)[i].Find( aText ) != KErrNotFound )
            {
            ret = i;
            break;
            }
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CVCGenericContainer::CreateListboxItem
// ----------------------------------------------------------------------------
//  
TDesC* CVCGenericContainer::CreateListboxItemLC( TInt aIconArrayIndex,
                                               const TDesC& aFirstRow,
                                               const TDesC& aSecondRow )
    {
    TBuf<KVCMaxNameSize> name;
    name.AppendNum( aIconArrayIndex );
    name.Append( KTabulator );
    name.Append( aFirstRow );
    name.Append( KTabulator );
    name.Append( aSecondRow );
    
    return name.AllocLC();
    }


// End of File

