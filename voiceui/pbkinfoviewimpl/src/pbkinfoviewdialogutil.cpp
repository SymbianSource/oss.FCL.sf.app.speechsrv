/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class for info view dialog.
*
*/


// INCLUDE FILES

#include "pbkinfoviewdialogutil.h"
#include "pbkinfoviewutil.h"
#include "pbkinfoviewdefines.h"

#include <akntitle.h>           // CAknTitlePane
#include <akncontext.h>         // CAknTitlePane
#include <aknnotewrappers.h>    // CAknInformationNote
#include <eikmenup.h>           // CEikMenuPane
#include <aknlists.h>           // CAknSingleHeadingPopupMenuStyleListBox

#include <StringLoader.h>

#include "rubydebug.h"


// ============================ MEMBER FUNCTIONS ===============================


/*******************************************************************************
 * class TDialogUtil
 *******************************************************************************/

// -----------------------------------------------------------------------------
// TDialogUtil::ConstructListBoxItemL
// @param aID Item ID of the masked bitmap to be created.
// @param aFilename Filename to be used to construct the item, 
// @param aFileBitmapId Id (for bitmap) in the file. 
// @param aFileMaskId Id (for mask) in the file.
// @return CGulIcon* Pointer to the newly created CGulIcon object.
//         Ownership of the object is transferred to the caller.
// -----------------------------------------------------------------------------
//
HBufC* TDialogUtil::ConstructListBoxItemLC( const TDesC& aFirstLine,
                                            const TDesC& aSecondLine,
                                            TInt aIndex1 )
	{
	_LIT( KFormat, "%d\t%S\t%S" );
	
	TInt len = aFirstLine.Length() + aSecondLine.Length() + KBufSize16;
	
	HBufC* hBuf = HBufC::NewLC( len );
	TPtr ptr = hBuf->Des();
	
	ptr.Format( KFormat , aIndex1, &aFirstLine, &aSecondLine );
	
	return hBuf;
	}
	
// -----------------------------------------------------------------------------
// TDialogUtil::CreateIconL
//
// Constructs an independent CGulIcon object with fallback support.
// If no matching item (aId) is found in the currently active skin,
// attempts to construct the item using the given file (last 3 parameters).
// @param aID Item ID of the masked bitmap to be created.
// @param aFilename Filename to be used to construct the item, 
// @param aFileBitmapId Id (for bitmap) in the file. 
// @param aFileMaskId Id (for mask) in the file.
// @return CGulIcon* Pointer to the newly created CGulIcon object.
//         Ownership of the object is transferred to the caller.
// -----------------------------------------------------------------------------
//
CGulIcon* TDialogUtil::CreateIconL( TAknsItemID aId, const TDesC& aFileName, TInt aFileBitmapId, TInt aFileMaskId )
	{
    CGulIcon* icon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(), aId, aFileName, aFileBitmapId, aFileMaskId);
	return icon;
	}

/*******************************************************************************
 * class CStatusPaneHandler
 *******************************************************************************/

// -----------------------------------------------------------------------------
// CStatusPaneHandler::NewLC
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CStatusPaneHandler* CStatusPaneHandler::NewL(CAknAppUi* aAppUi)
    {
    CStatusPaneHandler* self = new (ELeave) CStatusPaneHandler(aAppUi);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CStatusPaneHandler::~CStatusPaneHandler()
    {
    TInt error( KErrNone );
    
	TRAP( error, RestorePreviousTitleL() );
	if ( error != KErrNone )
		{
		RUBY_DEBUG1( "RestoreOriginalTitleL() error: %d", error );
		}
	
	TRAP( error, RestoreOriginalImageL() );
	if ( error != KErrNone )
		{
		RUBY_DEBUG1( "RestoreOriginalImageL() error: %d", error );
		}	
    }

// -----------------------------------------------------------------------------
// CStatusPaneHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStatusPaneHandler::ConstructL(void)
    {
    }

// -----------------------------------------------------------------------------
// CStatusPaneHandler::CStatusPaneHandler
//
// -----------------------------------------------------------------------------
//
CStatusPaneHandler::CStatusPaneHandler( CAknAppUi* aAppUi ) : iAppUi( aAppUi )
	{
	__ASSERT_ALWAYS( iAppUi, TPbkInfoViewUtil::Panic( KErrGeneral ) );

	iOriginalTitleStored = EFalse;
	iOriginalTitle = KNullDesC;
	}

// ----------------------------------------------------------------------------
// CStatusPaneHandler::GetPreviousTitleL
// Gets the title from previous view.
// @param aAppUi Pointer to UI class.
// @param aText Title will be put to this variable.
// @return TBool ETrue: success / EFalse: failure.
// ----------------------------------------------------------------------------
//
TBool CStatusPaneHandler::GetPreviousTitleL( CAknAppUi* aAppUi, TDes& aTitle )
	{
	aTitle = KNullDesC;

	CEikStatusPane* statusPane = aAppUi->StatusPane();
    if ( statusPane && 
         statusPane->PaneCapabilities
             ( TUid::Uid( EEikStatusPaneUidTitle) ).IsPresent() )
		{
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
            (statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        if ( titlePane->Text() ) 
			{
		    TPbkInfoViewUtil::StrCopy( aTitle, *titlePane->Text() );
			return ETrue;
    		}
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CStatusPaneHandler::SetTitleL
// Sets the dialog title.
// @param aAppUi Pointer to the UI class.
// @param aText Title text.
// ----------------------------------------------------------------------------
//
void CStatusPaneHandler::SetTitleL( CAknAppUi* aAppUi, const TDesC& aTitle )
	{
	CEikStatusPane* statusPane = aAppUi->StatusPane();
    if ( statusPane && 
         statusPane->PaneCapabilities
             (TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
		{
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
            (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
        if ( titlePane->Text() ) 
			{
	   		titlePane->SetTextL(aTitle);
			}
		}
	}

// ----------------------------------------------------------------------------
// CStatusPaneHandler::StoreOriginalTitleL
// Sets the member variable for previous title.
// ----------------------------------------------------------------------------
//
void CStatusPaneHandler::StoreOriginalTitleL()
	{
	TBool ret = GetPreviousTitleL( iAppUi, iOriginalTitle );
	
	if ( ret )
		{
		iOriginalTitleStored = ETrue;
		}
	}

// ----------------------------------------------------------------------------
// CStatusPaneHandler::RestorePreviousTitleL
// Restores previous title when returning to previous view.
// ----------------------------------------------------------------------------
//
void CStatusPaneHandler::RestorePreviousTitleL()
	{
	if ( iOriginalTitleStored )
		{
		SetTitleL( iAppUi, iOriginalTitle );
		}
	}

// ----------------------------------------------------------------------------
// CStatusPaneHandler::SetTitleL
// Sets the dialog title.
// @param aResourceId Resource id from resource file.
// ----------------------------------------------------------------------------
//
void CStatusPaneHandler::SetTitleL( TInt aResourceId )
	{
	// Get title from resource file
	HBufC* dialogTitle = StringLoader::LoadLC( aResourceId );
			
	SetTitleL( iAppUi, dialogTitle->Des() );
	CleanupStack::PopAndDestroy( dialogTitle );
	}

// ----------------------------------------------------------------------------
// CStatusPaneHandler::RestoreOriginalImageL
// Restores previous image when returning to previous view.
// ----------------------------------------------------------------------------
//
void CStatusPaneHandler::RestoreOriginalImageL()
	{
	if (!iOriginalImage)
		{
		return; // image has not been replaced
		}

	CEikImage* image = NULL;
	TRAPD(err, image = SetImageL( iAppUi, iOriginalImage ) );

	if (err == KErrNone && image)
		{
		delete image; // delete our own image
		}
	else
		{
		delete iOriginalImage; // delete original image (could not restore it)
		iOriginalImage = NULL;
		}
	}

// ----------------------------------------------------------------------------
// CStatusPaneHandler::SetImageL
// Sets dialog image.
// ----------------------------------------------------------------------------
//
CEikImage* CStatusPaneHandler::SetImageL( CAknAppUi* aAppUi, CEikImage* aIcon )
	{
	CEikStatusPane* statusPane = aAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities
         (TUid::Uid(EEikStatusPaneUidContext)).IsPresent() )
		{
        CAknContextPane* contextPane = static_cast<CAknContextPane*>
            ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidContext ) ) );
        if ( contextPane ) 
			{
	   		return contextPane->SwapPicture( aIcon );
			}
		}

	return NULL;
	}

//  End of File  
