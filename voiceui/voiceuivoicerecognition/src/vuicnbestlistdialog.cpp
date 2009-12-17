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
* Description:  Implementation of CNBestListDialog
*
*/


// INCLUDE FILES
#include <featmgr.h>

#include <aknPopup.h>
#include <data_caging_path_literals.hrh>

#include <AknIconArray.h>

#include <AknMediatorFacade.h>

#include <secondarydisplay/vuisecondarydisplayapi.h>
#include <SecondaryDisplay/AknSecondaryDisplayDefs.h>

#include <vuivoicerecognition.rsg>
#include <vuivoicerecogdefs.h>

#include "vuivoicerecognition.hrh"

#include "vuiclistquerydialog.h"
#include "vuicnbestlistdialog.h"

#include "vuimkeycallback.h"

#include "vuivoiceicondefs.h"

#include "rubydebug.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CNBestListDialog::CNBestListDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNBestListDialog::CNBestListDialog() 
 : iCurrentIndex( 0 ), iTagCount( 0 ),
   iCba( R_SOFTKEYS_SELECT_QUIT__SELECT )
    {
    }


// Destructor
CNBestListDialog::~CNBestListDialog()
    {
    RUBY_DEBUG0( "CNBestListDialog::~CNBestListDialog START" );
     
    RegisterForKeyCallback( NULL );
    
    delete iDialog;
    iDialog = NULL;
    
    iEikonEnv->EikAppUi()->RemoveFromStack( this );
        
    RUBY_DEBUG0( "CNBestListDialog::~CNBestListDialog EXIT" );          
    }


// -----------------------------------------------------------------------------
// CNBestListDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNBestListDialog::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::ConstructL" );
      
    iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityAlert );
    }


// ---------------------------------------------------------
// CNBestListDialog::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CNBestListDialog* CNBestListDialog::NewL()                                                           
    {
    CNBestListDialog* self = new (ELeave) CNBestListDialog();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CNBestListDialog::RegisterForKeyCallback
// -----------------------------------------------------------------------------
//
void CNBestListDialog::RegisterForKeyCallback( MKeyCallback* aKeyCallback )
    {
    iKeyCallback = aKeyCallback;
    }

// ---------------------------------------------------------
// TKeyResponse CNBestListDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CNBestListDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode /*aType*/ )
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::OfferKeyEventL" );

    TKeyResponse keyStatus = EKeyWasNotConsumed;

    TInt event = ConvertKeyEventL( aKeyEvent );
    
    // Check if event should be ignored
    if ( iDialog && iDialog->IsMenuVisible() )
        {
        switch( event )
            {
            case EVoiceTagSoftKeyQuit:
                {
                break;
                }
            
            default:
                {
                event = ENoKeypress;
                break;
                }
            }
        }

    if ( event != ENoKeypress )
        {
        switch( event )
            {
            case ESelectKeypress:
            case EDirectSelectKeypress:
                {                   
                keyStatus = EKeyWasConsumed;
                break;
                }
            
            case EUpKeypress:
                {
                --iCurrentIndex;
                if ( iCurrentIndex <= -1 )
                    {
                    iCurrentIndex = iTagCount - 1;    
                    }
                break;
                }
                        
            case EDownKeypress:
                {
                ++iCurrentIndex;
                if ( iCurrentIndex >= iTagCount )
                    {
                    iCurrentIndex = 0;
                    }
                break;
                }
               
            case EVoiceTagSoftKeyQuit:
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
    
// ---------------------------------------------------------
// CNBestListDialog::ProcessCommandL
// ---------------------------------------------------------
//      
void CNBestListDialog::ProcessCommandL( TInt aCommandId )
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::ProcessCommandL" );
    
    TInt event = KErrNone;
    
    if ( aCommandId == EVoiceTagSoftKeySelect )
        {
        event = ESelectKeypress;
        }
    else if ( aCommandId == EVoiceTagSoftKeyOpen )
        {
        event = EOpenKeypress;
        }
    else if ( aCommandId == EVoiceTagSoftKeyQuit )
        {
        event = EVoiceTagSoftKeyQuit;
        }
    else if ( aCommandId == EAknSoftkeyOptions )
        {
        event = EOptionsKeypress;
        }
    else if ( aCommandId == EDragKeypress )
        {
        event = EDragKeypress;
        
        // Don't forward keypress if current index hasn't changed
        if ( iCurrentIndex == iDialog->ListBox()->CurrentItemIndex() )
            {
            return;
            }

        iCurrentIndex = iDialog->ListBox()->CurrentItemIndex();
        }
    else if ( aCommandId == EScrollKeypress )
        {
        event = EScrollKeypress;
        }
                    
    iKeyCallback->HandleKeypressL( event );
    }

// ---------------------------------------------------------------------------
// CNBestListDialog::MediatorCommandL
// ---------------------------------------------------------------------------
//
TAknDialogMediatorObserverCommand CNBestListDialog::MediatorCommandL( TUid /*aDomain*/, 
                                                                      TUid aCategory, 
                                                                      TInt aCommandId, 
                                                                      const TDesC8& aData )
    { 
    RUBY_DEBUG_BLOCK( "CNBestListDialog::MediatorCommandL" );
    
    TAknDialogMediatorObserverCommand retValue = EHandleAsNormal;
    
    if ( aCategory == SecondaryDisplay::KCatVoiceUi && 
         aCommandId == SecondaryDisplay::ECmdShowNBestListQuery )
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
// CNBestListDialog::NotifyMediatorExit
// ---------------------------------------------------------------------------
//
void CNBestListDialog::NotifyMediatorExit() 
    { 
    RUBY_DEBUG0( "CNBestListDialog::NotifyMediatorExit" );
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CNBestListDialog::CreateNBestListPopupLC
// -----------------------------------------------------------------------------
//
void CNBestListDialog::CreateNBestListPopupLC( CDesC16ArrayFlat* aItems,
                                               const TDesC& aHeader )
    {
    RUBY_DEBUG0( "CNBestListDialog::ShowNBestListPopupLC START" );
       
    iDialog = CListQueryDialog::NewL( &iCurrentIndex, &iDialog, this, R_NBEST_OPTIONS_MENU );
    iDialog->PrepareLC( R_NBEST_POPUP_LIST );
    
    if ( aHeader != KNullDesC )
        {
        iDialog->QueryHeading()->SetTextL( aHeader );
        }

    iDialog->SetOwnershipType( ELbmOwnsItemArray );
    iDialog->SetItemTextArray( aItems );
    
    iDialog->ListControl()->Listbox()->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
        
    iTagCount = aItems->Count();
    
    CreateAndSetSecondaryDisplayDataL( aItems );
        
    // Create icon array
    CArrayPtr<CGulIcon>* icons = CreateIconArrayL();    
    iDialog->SetIconArrayL( icons );
    
    RUBY_DEBUG0( "CNBestListDialog::ShowNBestListPopupLC EXIT" );
    }

// -----------------------------------------------------------------------------
// CNBestListDialog::ShowNBestListPopupL
// -----------------------------------------------------------------------------
//
TInt CNBestListDialog::ShowNBestListPopupL()
    {
    RUBY_DEBUG0( "CNBestListDialog::ShowNBestListPopupL START" );

    // Show N-Best List
    TInt returnValue = iDialog->RunLD();
    
    // Touch selection returns EAknSoftkeyOk
    if ( returnValue == EAknSoftkeyOk )
        {
        returnValue = EVoiceTagSoftKeySelect;
        }
            
    RUBY_DEBUG0( "CNBestListDialog::ShowNBestListPopupL EXIT" );

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CNBestListDialog::SetSoftkeys
// -----------------------------------------------------------------------------
//    
void CNBestListDialog::SetSoftkeysL( TInt aResourceId )
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::SetSoftkeysL" );
    
    iCba = aResourceId;
    
    // Set softkey labels
    CEikButtonGroupContainer* cba = &iDialog->ButtonGroupContainer();
    cba->SetCommandSetL( aResourceId );
    cba->DrawDeferred();
    }
    
// -----------------------------------------------------------------------------
// CNBestListDialog::SetCommandsHidden
// -----------------------------------------------------------------------------
//    
void CNBestListDialog::SetCommandsHidden( TBool aHidden )
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::SetCommandsHidden" );
    
    iDialog->SetCommandVisibility( aHidden );
    }
    
// -----------------------------------------------------------------------------
// CNBestListDialog::ConvertKeyEventL
// -----------------------------------------------------------------------------
// 
TInt CNBestListDialog::ConvertKeyEventL( const TKeyEvent& aKeyEvent )
    {
    TInt event = ENoKeypress;
    
    switch( aKeyEvent.iCode )
        {
        case EKeyNo:
        case EKeyEscape:
            {
            event = EVoiceTagSoftKeyQuit;
            break;
            }

        case EKeyDevice3:
            {
            event = ESelectKeypress;
            break;
            }
            
        case EKeyYes:
            {
            event = EDirectSelectKeypress;
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
    
// ---------------------------------------------------------
// CNBestListDialog::CreateIconArrayL
// Creates new icon array
// ---------------------------------------------------------
//
CArrayPtr<CGulIcon>* CNBestListDialog::CreateIconArrayL()
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::CreateIconArrayL" );
    
    CAknIconArray* icons = new ( ELeave ) CAknIconArray( KSindNumberOfIcons );
    CleanupStack::PushL( icons );

    // 0. Phone
    // 1. Home
    // 2. Work
    // 3. Mobile
    // 4. Fax
    // 5. Pager
    // 6. Blank
    // 7. Profile
    // 8. Application or function
    // 9. Video call icon
    //10. VoIP icon
    
    for ( int i = 0; i < KSindNumberOfIcons; ++i )
        {
        AppendIconToArrayL( icons, *KIconId[ i ], KBitmapId[ i ], KMaskId[ i ] );
        }
        
    CleanupStack::Pop( icons );
    return icons;
    }


// ---------------------------------------------------------
// CNBestListDialog::AppendIconToArrayL
// ---------------------------------------------------------
//
void CNBestListDialog::AppendIconToArrayL( CAknIconArray* aArray,
                                           const TAknsItemID& aID,
                                           TInt aBitmapId,
                                           TInt aMaskId )
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::AppendIconToArrayL" );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TBuf<KMaxFileName> fileName = AknIconUtils::AvkonIconFileName();
    
    if ( aID == *KIconId[ EIconProfile ] || aID == *KIconId[ EIconApplication ] )
        {
        TParse* fp = new ( ELeave ) TParse();
        CleanupStack::PushL( fp );
        User::LeaveIfError( fp->Set( KDirAndFile, &KDC_BITMAP_DIR, NULL ) );

        fileName = fp->FullName();
        CleanupStack::PopAndDestroy( fp );
        }

    AknsUtils::CreateIconL( skin, aID, bitmap, mask, fileName,
                            aBitmapId, aMaskId );
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop( icon );
    }

// ---------------------------------------------------------
// CNBestListDialog::CreateAndSetSecondaryDisplayDataL
// ---------------------------------------------------------
//    
void CNBestListDialog::CreateAndSetSecondaryDisplayDataL( CDesC16ArrayFlat* aItems )
    {
    RUBY_DEBUG_BLOCK( "CNBestListDialog::CreateAndSetSecondaryDisplayDataL" );

    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {        
        // Create dummy id list (needed for list query automation)
        CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
        CleanupStack::PushL( array );
        for( TInt i = 0; i < iTagCount; ++i )
            {
            array->AppendL( i );
            }

        // Publish to secondary display
        iDialog->PublishDialogL( SecondaryDisplay::ECmdShowNBestListQuery,
                                 SecondaryDisplay::KCatVoiceUi,
                                 array );
                                 
        iDialog->SetMediatorObserver( this );
        
        CleanupStack::Pop( array ); // Dialog takes ownership of array

        // Generate n-best data for secondary display
        CAknMediatorFacade* covercl = AknMediatorFacade( iDialog );
        if( covercl )
            {
            // Current index
            covercl->BufStream().WriteInt32L( iCurrentIndex );

            // Item count and items
            covercl->BufStream().WriteInt32L( iTagCount );
            for( TInt i = 0; i < iTagCount; ++i )
                {
                TPtrC16 item = aItems->MdcaPoint( i );
                covercl->BufStream().WriteInt32L( item.Length() );
                covercl->BufStream().WriteL( item, item.Length() );
                }

            covercl->BufStream().CommitL(); // no more data to send so commit buf
            }
        }
    }

// ---------------------------------------------------------
// CNBestListDialog::HandleShortKeypressL
// ---------------------------------------------------------
//
TBool CNBestListDialog::HandleShortKeypressL( CDesC16ArrayFlat* aItems )
    {        
    RUBY_DEBUG_BLOCK( "CNBestListDialog::HandleShortKeypressL" );

    TBool returnValue = ETrue;

    if ( aItems )
        {
        iDialog->SetItemTextArray( aItems );        
        iDialog->ListControl()->Listbox()->SetCurrentItemIndexAndDraw( iCurrentIndex );
        
        CreateAndSetSecondaryDisplayDataL( aItems );
        }
    else
        {
        ++iCurrentIndex;
        if ( iCurrentIndex == iTagCount )
            {    
            returnValue = EFalse;
            }

        if ( returnValue ) 
            {
            // Move focus to the current item
            TKeyEvent keyEvent;
            keyEvent.iRepeats = 0;
            keyEvent.iCode = EKeyDownArrow;
        
            iDialog->OfferKeyEventL( keyEvent, EEventKey );
            }
        }
    
    return returnValue;                
    }
    
// ---------------------------------------------------------------------------
// CNBestListDialog::SelectedIndex
// ---------------------------------------------------------------------------
//    
TInt CNBestListDialog::SelectedIndex()
    {
    RUBY_DEBUG1( "CNBestListDialog::SelectedIndex - index = %d", iCurrentIndex );
    
    TInt index = iCurrentIndex;
    
    if ( index < 0 )
        {
        index = 0;
        }
    
    return index;
    }
    
//  End of File
