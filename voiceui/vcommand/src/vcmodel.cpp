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
* Description:  Model class for vcommand application wrapping CVCommandHandler
*
*/


// INCLUDE FILES

#include "vcappui.h"
#include "vcmodel.h"
#include "vcplaybackdialog.h"
#include "vcommanduientry.h"
#include "vcmodelappuiinterface.h"

#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVCModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCModel* CVCModel::NewL( MVcModelAppUiInterface* aAppUi )
    {
    CVCModel* self = new (ELeave) CVCModel( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CVCModel::CVCModel
// C++ constructor
// ----------------------------------------------------------------------------
//
CVCModel::CVCModel( MVcModelAppUiInterface* aAppUi ) : iAppUi( aAppUi )
    {   
    }
 
// ----------------------------------------------------------------------------
// CVCModel::ConstructL
// Two-phased constructor
// ----------------------------------------------------------------------------
//
void CVCModel::ConstructL() 
    {
    iVcHandler = CVCommandHandler::NewL( this );
    
    LoadVCommandsL();
    }

// ----------------------------------------------------------------------------
// CVCModel::~CVCModel
// Destructor
// ----------------------------------------------------------------------------
//
CVCModel::~CVCModel() 
    {
    delete iVcHandler;
    delete iListBoxVCommands;
    }
    
// ----------------------------------------------------------------------------
// CVCModel::GetVCommandL
// ----------------------------------------------------------------------------
//
const CVCommandUiEntry& CVCModel::GetVCommandL( const TDesC& aWrittenText )
    {
    RUBY_DEBUG_BLOCKL( "CVCModel::GetVCommandL" );
    
    TInt index( KErrNotFound );
    index = FindVCommandIndexL( aWrittenText );
    User::LeaveIfError( index );
    
    return iListBoxVCommands->At( index );
    }
    
// ----------------------------------------------------------------------------
// CVCModel::At
// ----------------------------------------------------------------------------
//
const CVCommandUiEntry& CVCModel::At( TInt aIndex ) const
    {
    // An index that is out of bounds causes a USER-130 panic.
    return iListBoxVCommands->At( aIndex );
    }
    
// ----------------------------------------------------------------------------
// CVCModel::At
// ----------------------------------------------------------------------------
//
TInt CVCModel::Count() const
    {
    return iListBoxVCommands->Count();
    }
    
// ----------------------------------------------------------------------------
// CVCModel::VCommandArrayL
// ----------------------------------------------------------------------------
//
CVCommandUiEntryArray* CVCModel::VCommandArrayL()
    {    
    return iListBoxVCommands->CloneL();
    }

// ----------------------------------------------------------------------------
// CVCModel::Service
// ----------------------------------------------------------------------------
//
CVCommandHandler& CVCModel::Service()
    {    
    return *iVcHandler;
    }
    
// ----------------------------------------------------------------------------
// CVCModel::LoadVCommandsL
// ----------------------------------------------------------------------------
//
void CVCModel::LoadVCommandsL()
    {
    RUBY_DEBUG_BLOCKL( "CVCModel::LoadVCommandsL" );
    
    // CVCommandHandler::ListCommandsL must be completed when exiting
    // the application or a panic might occur.
    iAppUi->StartAtomicOperationLC();
    CVCommandUiEntryArray* listBoxVCommands
        = CVCommandUiEntryArray::NewL( iVcHandler->ListCommandsL() );
        
    delete iListBoxVCommands;
    iListBoxVCommands = listBoxVCommands;
        
    CleanupStack::PopAndDestroy(); // StartAtomicOperationLC
    }
    
// ----------------------------------------------------------------------------
// CVCModel::iView->FindVCommandIndexL
// ----------------------------------------------------------------------------
//
TInt CVCModel::FindVCommandIndexL( const TDesC& aWrittenText )
    {
    RUBY_DEBUG_BLOCKL( "CVCModel::FindVCommandIndexL" );
    
    for( TInt i = 0; i < iListBoxVCommands->Count(); i++ ) 
        {
        if( aWrittenText == iListBoxVCommands->At( i ).WrittenText() )
            {
            return i;
            }
        }
    
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CVCModel::HasCommandL
// ----------------------------------------------------------------------------
//
TBool CVCModel::HasCommandL( const TDesC& aWrittenText )
    {
    return FindVCommandIndexL( aWrittenText ) != KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CVCModel::SetNewSpokenTextL
// ----------------------------------------------------------------------------
//
void CVCModel::SetNewSpokenTextL( const TDesC& aWrittenText,
                                  const TDesC& aNewSpokenText ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCModel::UpdateVCommandL" );
    
    iAppUi->StartAtomicOperationLC();
    
    TInt index( KErrNotFound );
    index = FindVCommandIndexL( aWrittenText );
    
    User::LeaveIfError( index );
    
    const CVCommand& oldCommand = iListBoxVCommands->At( index ).Command();

    CVCCommandUi* updatedCommandUi( NULL );
    CVCFolderInfo* folderInfo
        = CVCFolderInfo::NewL( oldCommand.CommandUi().FolderInfo() );
    CleanupStack::PushL( folderInfo );
    updatedCommandUi = CVCCommandUi::NewL( oldCommand.CommandUi().WrittenText(), 
                                           *folderInfo,
                                           oldCommand.CommandUi().Modifiable(),
                                           oldCommand.CommandUi().Tooltip(), 
                                           oldCommand.CommandUi().IconUid(),
                                           aNewSpokenText,
                                           oldCommand.CommandUi().ConfirmationNeeded() );
    CleanupStack::PopAndDestroy( folderInfo );
    CleanupStack::PushL( updatedCommandUi );

    CVCommand* updatedCommand( NULL );
    updatedCommand = CVCommand::NewL( oldCommand.SpokenText(),
                                      oldCommand.Runnable(),
                                      *updatedCommandUi );

    CleanupStack::PopAndDestroy( updatedCommandUi );
    CleanupStack::PushL( updatedCommand );
    iVcHandler->AddCommandL( *updatedCommand );

    iVcHandler->RemoveCommandL( oldCommand );

    CleanupStack::PopAndDestroy( updatedCommand ); 

    LoadVCommandsL();
    
    CleanupStack::PopAndDestroy(); // StartAtomicOperationLC
    }
    
// ----------------------------------------------------------------------------
// CVCModel::CommandSetChanged
// ----------------------------------------------------------------------------
//
void CVCModel::CommandSetChanged()
    {
    iAppUi->CommandSetChanged();
    }
    
// End of File
