/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles the global progress dialog for voice recognition
*
*/


// INCLUDE FILES
#include <featmgr.h>

#include <data_caging_path_literals.hrh>

#include <AknProgressDialog.h>
#include <AknsItemID.h>
#include <eikprogi.h>
#include <eikbtgpc.h>
#include <eikimage.h> 
#include <AknsUtils.h> 

#include <AknMediatorFacade.h>

#include <SecondaryDisplay/AknSecondaryDisplayDefs.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuicglobalprogressdialog.h"
#include "vuimkeycallback.h"

#include "vuivoiceicondefs.h"

#include "rubydebug.h"

_LIT( KNewLine, "\n" );

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CGlobalProgressDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlobalProgressDialog* CGlobalProgressDialog::NewL()
    {
    RUBY_DEBUG_BLOCK( "CGlobalProgressDialog::NewL" );
    
    CGlobalProgressDialog* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGlobalProgressDialog::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlobalProgressDialog* CGlobalProgressDialog::NewLC()
    { 
    CGlobalProgressDialog* self = new ( ELeave ) CGlobalProgressDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor
CGlobalProgressDialog::~CGlobalProgressDialog()
    {
    RUBY_DEBUG0( "CGlobalProgressDialog::~CGlobalProgressDialog START" );
   
    RegisterForKeyCallback( NULL );
    
    if ( iInternalState == EStarted || iInternalState == ERunning )
        {
        if ( iProgressDialog )
            {
            iProgressDialog->MakeVisible( EFalse );
            }        
        delete iProgressDialog;
        }
    
    RUBY_DEBUG0( "CGlobalProgressDialog::~CGlobalProgressDialog EXIT" );
    }

// ---------------------------------------------------------
// CGlobalProgressDialog::RegisterForKeyCallback
// Register for callback to receive notification when a softkey 
// is pressed
// ---------------------------------------------------------
//
void CGlobalProgressDialog::RegisterForKeyCallback( MKeyCallback* aKeyCallback )
    {
    RUBY_DEBUG0( "CGlobalProgressDialog::RegisterForKeyCallback START" );
    
    if ( iInternalState != EFinished )
        {
        if ( aKeyCallback )
            {
            iProgressDialog->SetCallback( this );
            }
        else
            {
            iProgressDialog->SetCallback( NULL );
            }
        }
    iKeyCallback = aKeyCallback;
    
    RUBY_DEBUG0( "CGlobalProgressDialog::RegisterForKeyCallback EXIT" );
    }            

// ---------------------------------------------------------
// CGlobalProgressDialog::SetIconL
// Set icon on the dialog
// ---------------------------------------------------------
//
void CGlobalProgressDialog::SetIconL( const TDesC& aIconFile,
                                      TInt aIconId, TInt aIconMaskId )
    {
    RUBY_DEBUG_BLOCK( "CGlobalProgressDialog::SetIconL" );
    
    // If icon should be left bland do not set it at all
    if ( aIconId != EMbmAvkonQgn_stat_bt_blank )
        {
        CEikImage* image = new ( ELeave ) CEikImage();
        CleanupStack::PushL( image );
        
        image->CreatePictureFromFileL( aIconFile, aIconId, aIconMaskId );
   
        iProgressDialog->SetIconL( image ); // ownership of the image is transferred to dialog
        
        CleanupStack::Pop( image );
        }
    }

// ---------------------------------------------------------
// CGlobalProgressDialog::SetImageL
// Set image on the dialog
// ---------------------------------------------------------
//
void CGlobalProgressDialog::SetImageL( const TDesC& aImageFile, TInt aImageId,
                                       TInt aImageMaskId )
    {
    RUBY_DEBUG_BLOCK( "CGlobalProgressDialog::SetImageL" );
    
    CEikImage* image = new ( ELeave ) CEikImage();
    CleanupStack::PushL( image );
    
    image->CreatePictureFromFileL( aImageFile, aImageId, aImageMaskId );
      
    iProgressDialog->SetImageL( image ); // ownership of the image is transferred to dialog
    
    CleanupStack::Pop( image );
    }

// ---------------------------------------------------------
// CGlobalProgressDialog::SetSecondaryDisplayDataL
// Sets additional information to be sent to secondary display
// ---------------------------------------------------------
//
void CGlobalProgressDialog::SetSecondaryDisplayDataL( TUid aCategory, TInt aDialogIndex,
                                                      const SecondaryDisplay::TMatchData* aData )
    {
    RUBY_DEBUG_BLOCK( "CGlobalProgressDialog::SetSecondaryDisplayDataL" );
   
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        iProgressDialog->PublishDialogL( aDialogIndex, aCategory );
   
        if ( aData )
            {
            // fetch akn utility for mediator support, \oem\aknmediatorfacade.h
            CAknMediatorFacade* covercl = AknMediatorFacade( iProgressDialog );

            if ( covercl )
                {
                covercl->BufStream() << aData->iType;
                covercl->BufStream() << aData->iName;
                covercl->BufStream() << aData->iNumber;
                covercl->BufStream().CommitL(); // no more data to send so commit buf
                }
            }
        }   
   }

// ---------------------------------------------------------
// CGlobalProgressDialog::ShowProgressDialogL
// Shows progress dialog
// ---------------------------------------------------------
//
void CGlobalProgressDialog::ShowProgressDialogL( const TDesC& aPrompt, const TDesC& aIconText, TInt aSoftkeys )
    {
    RUBY_DEBUG_BLOCK( "CGlobalProgressDialog::ShowProgressDialogL" );
    
    iProgressDialog->PrepareLC( R_PROGRESSNOTE );
   
    HBufC* string = HBufC::NewLC( aPrompt.Length() + aIconText.Length() + KNewLine.iTypeLength );
            
    string->Des() = aPrompt;
    string->Des() += KNewLine;
    string->Des() += aIconText;
   
    iProgressDialog->SetTextL( *string );
   
    CleanupStack::PopAndDestroy( string );
   
    CEikButtonGroupContainer* cba = &iProgressDialog->ButtonGroupContainer();
    cba->SetCommandSetL( aSoftkeys );

    iProgressDialog->RunLD();

    iInternalState = ERunning;
    }

// ---------------------------------------------------------
// CGlobalProgressDialog::UpdateProgressDialogL
// Updates the progress dialog
// ---------------------------------------------------------
//
void CGlobalProgressDialog::UpdateProgressDialogL( TInt aValue, TInt aFinalValue )
    {
    RUBY_DEBUG_BLOCK( "CGlobalProgressDialog::UpdateProgressDialogL" );
    
    if ( iProgressDialog )
        {
        CEikProgressInfo* progressInfo = iProgressDialog->GetProgressInfoL();
        progressInfo->SetFinalValue( aFinalValue );   	
        progressInfo->SetAndDraw( aValue );
            
        if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
            {
            CAknMediatorFacade* coverData = AknMediatorFacade( iProgressDialog );
            if( coverData )
                {
                coverData->ResetBuffer();
                coverData->BufStream().WriteInt32L( EAknGlobalPDialogValuesUpdate );
                coverData->BufStream().WriteInt32L( aValue );
                coverData->BufStream().WriteInt32L( aFinalValue );
                coverData->BufStream().CommitL();
                coverData->PostUpdatedDataL();
                }
            }
        }
   	}

// ---------------------------------------------------------
// CGlobalProgressDialog::SetSkinIds
// Set skin ids on the dialog
// ---------------------------------------------------------
//
void CGlobalProgressDialog::SetSkinIds( TAknsItemID aImageId, TInt aImageBitmapId,
                                        TInt aImageMaskId, TAknsItemID aIconId )
    {
    RUBY_DEBUG0( "CGlobalProgressDialog::SetSkinIds START" );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CEikImage* image = NULL;
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TBuf<KMaxFileName> fileName = AknIconUtils::AvkonIconFileName();
    
    if ( skin )
        {
        TRAP_IGNORE(
            AknsUtils::CreateIconL( skin, aImageId, bitmap, mask, fileName,
                                    aImageBitmapId, aImageMaskId );
            CleanupStack::PushL( bitmap );
            CleanupStack::PushL( mask );
            
            image = new ( ELeave ) CEikImage();
            CleanupStack::PushL( image );
            
            image->SetBitmap( bitmap ); // ownership of the bitmap is transferred to image
            image->SetMask( mask ); // ownership of the mask is transferred to image
        
            iProgressDialog->SetImageL( image ); // ownership of the image is transferred to dialog
            
            CleanupStack::Pop( image );
            CleanupStack::Pop( mask );
            CleanupStack::Pop( bitmap );
            );

        TInt iconId = 0;
        for ( ; iconId < KSindNumberOfIcons; ++iconId )
            {
            if ( aIconId == *KIconId[ iconId ] )
                {
                break;
                }
            }
            
        if ( aIconId == *KIconId[ EIconProfile ] || aIconId == *KIconId[ EIconApplication ] )
            {
            TRAP_IGNORE(
                TParse* fp = new ( ELeave ) TParse();
                CleanupStack::PushL( fp );
                User::LeaveIfError( fp->Set( KDirAndFile, &KDC_BITMAP_DIR, NULL ) );

                fileName = fp->FullName();
                CleanupStack::PopAndDestroy( fp );
                );
            }
        
        // Don't set blank icon
        if ( aIconId != KAknsIIDQgnStatBtBlank )
            {
            ASSERT ( iconId < KSindNumberOfIcons );
            TRAP_IGNORE(
                AknsUtils::CreateIconL( skin, aIconId, bitmap, mask, fileName,
                                        KBitmapId[ iconId ], KMaskId[ iconId ] );
                CleanupStack::PushL( bitmap );
                CleanupStack::PushL( mask );
                
                image = new ( ELeave ) CEikImage();
                CleanupStack::PushL( image );
                
                image->SetBitmap( bitmap ); // ownership of the bitmap is transferred to image
                image->SetMask( mask ); // ownership of the mask is transferred to image
            
                iProgressDialog->SetIconL( image ); // ownership of the image is transferred to dialog
                
                CleanupStack::Pop( image );
                CleanupStack::Pop( mask );
                CleanupStack::Pop( bitmap );
                );
            }
        }
    RUBY_DEBUG0( "CGlobalProgressDialog::SetSkinIds EXIT" );
    }
   
// ---------------------------------------------------------
// CGlobalProgressDialog::DialogDismissedL
//
// ---------------------------------------------------------
//
void CGlobalProgressDialog::DialogDismissedL( TInt aButtonId )
    {
    RUBY_DEBUG0( "CGlobalProgressDialog::DialogDismissedL START" );
        
    // Enter key selection returns EAknSoftkeyOk
    if ( aButtonId == EAknSoftkeyOk )
        {
        aButtonId = EVoiceTagSoftKeyOther;
        }
        
    if ( iInternalState == ERunning && iKeyCallback )
        {
        iInternalState = EFinished;
        iProgressDialog->SetCallback( NULL );
        iKeyCallback->HandleKeypressL( aButtonId );
        }
    
    RUBY_DEBUG0( "CGlobalProgressDialog::DialogDismissedL EXIT" );
    }    

// -----------------------------------------------------------------------------
// CGlobalProgressDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlobalProgressDialog::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CGlobalProgressDialog::ConstructL" );
      
    iProgressDialog = new ( ELeave ) CAknProgressDialog(
        ( REINTERPRET_CAST( CEikDialog**, &iProgressDialog ) ), ETrue );
        
    iProgressDialog->SetGloballyCapturing( ETrue );
    }

    
// -----------------------------------------------------------------------------
// CGlobalProgressDialog::CGlobalProgressDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGlobalProgressDialog::CGlobalProgressDialog()
 : iInternalState( EStarted )
    {
    }
    
// ---------------------------------------------------------
// CGlobalProgressDialog::ProcessFinished
// Stops the progress dialog
// ---------------------------------------------------------
//
void CGlobalProgressDialog::ProcessFinished()
    {
    RUBY_DEBUG0( "CGlobalProgressDialog::ProcessFinished START" );
    
    if ( iInternalState == ERunning )
        {
        iInternalState = EFinished;
        iProgressDialog->SetCallback( NULL );
        TRAP_IGNORE( iProgressDialog->ProcessFinishedL() );
        }
    
    RUBY_DEBUG0( "CGlobalProgressDialog::ProcessFinished EXIT" );
    }

// End of File

