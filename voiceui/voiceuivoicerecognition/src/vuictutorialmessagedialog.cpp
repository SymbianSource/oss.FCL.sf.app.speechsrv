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
* Description:  Handles dialog used in tutorial mode
*
*/


// INCLUDE FILES 
#include <featmgr.h>

#include <StringLoader.h>

#include <AknNotifierController.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuictutorialmessagedialog.h"
#include "vuicpropertyhandler.h"
#include "vuicmessagequerydialog.h"

#include "rubydebug.h"

// CONSTANTS
_LIT( KSpace, " " );

// ---------------------------------------------------------
// CTutorialMessageDialog::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CTutorialMessageDialog* CTutorialMessageDialog::NewL()
    {
    CTutorialMessageDialog* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CTutorialMessageDialog::NewLC
// Two-phased constructor.
// ---------------------------------------------------------
//
CTutorialMessageDialog* CTutorialMessageDialog::NewLC()
    {
    CTutorialMessageDialog* self = new (ELeave) CTutorialMessageDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor       
CTutorialMessageDialog::~CTutorialMessageDialog()
    {    
    delete iDlg;
    }        

// ---------------------------------------------------------
// CTutorialMessageDialog::ShowMessageDialogL
// ---------------------------------------------------------
//
TInt CTutorialMessageDialog::ShowMessageDialogL( TInt aDialogResourceId,
                                                 TInt aHeaderId,
                                                 TInt aMessagePart1Id,
                                                 TInt aMessagePart2Id,
                                                 TInt aCommandResourceId,
                                                 SecondaryDisplay::TVUISecondaryDisplayDialogs aDialog )
    {
    RUBY_DEBUG_BLOCK( "CTutorialMessageDialog::ShowMessageDialogL" );
    
    // Load resources
    HBufC* header = StringLoader::LoadLC( aHeaderId );        
    HBufC* part1 = StringLoader::LoadLC( aMessagePart1Id );
    HBufC* part2 = NULL;
    HBufC* message = part1;
    
    if ( aMessagePart2Id )
        {
        part2 = StringLoader::LoadLC( aMessagePart2Id );
        message = ConcatenateLC( *part1, *part2 );
        }
        
    // Create dialog
    iDlg = CMessageQueryDialog::NewL( &iDlg );
    
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) &&
         aDialog != SecondaryDisplay::ECmdVoiceNoNote )
	    {
        // Initializes cover support
        iDlg->PublishDialogL( aDialog, SecondaryDisplay::KCatVoiceUi );
        }
    
    iDlg->PrepareLC( aDialogResourceId ); 
        
    // Set softkey labels
    CEikButtonGroupContainer* cba = &iDlg->ButtonGroupContainer();
    cba->SetCommandSetL( aCommandResourceId );
    iCommands = aCommandResourceId;
    
    // Set message & header texts
    iDlg->SetMessageTextL( *message );
    iDlg->QueryHeading()->SetTextL( *header );   
    
    // Show dialog and block until dialog is dismissed
    // RunLD pops iDlg (pushed in PrepareLC) from CleanupStack
    TInt returnValue = iDlg->RunLD();
    
    // Enter selection returns EAknSoftkeyOk
    if ( returnValue == EAknSoftkeyOk )
        {
        if ( aCommandResourceId == R_SOFTKEYS_NEXT_CANCEL__NEXT )
            {
            returnValue = EVoiceInfoSoftKeyNext;
            }
        else if ( aCommandResourceId == R_SOFTKEYS_ACTIVATE_CANCEL__ACTIVATE )
            {
            returnValue = EVoiceInfoSoftKeyActivate;
            }
        else
            {
            returnValue = EAknSoftkeyQuit;
            }
        }
    
    // Cleanup resources
    if ( aMessagePart2Id )
        {
        CleanupStack::PopAndDestroy( message );
        CleanupStack::PopAndDestroy( part2 );
        }
    CleanupStack::PopAndDestroy( part1 );
    CleanupStack::PopAndDestroy( header );
      
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CTutorialMessageDialog::CTutorialMessageDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTutorialMessageDialog::CTutorialMessageDialog()
    {
    // Nothing
    }
    
// -----------------------------------------------------------------------------
// CTutorialMessageDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTutorialMessageDialog::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CTutorialMessageDialog::ConstructL" );
    }

// ---------------------------------------------------------
// CTutorialMessageDialog::ConcatenateLC
// ---------------------------------------------------------
//    
HBufC* CTutorialMessageDialog::ConcatenateLC( const TDesC& aString1,
                                              const TDesC& aString2 )
    {
    HBufC* string = NULL;
    if ( aString2.Length() )
        {
        string = HBufC::NewLC( aString1.Length() + aString2.Length() + KSpace.iTypeLength );
        string->Des() = aString1;
        string->Des() += KSpace;
        string->Des() += aString2;
        }
    else
        {
        string = HBufC::NewLC( aString1.Length() );
        string->Des() = aString1;
        }
    return string;
    }

// End of File


