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
* Description:  Class to wrap a CVCommand object
*
*/


// INCLUDE FILES

#include <vcommandapi.h>

#include "vcommanduientry.h"

#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVCommandUiEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCommandUiEntry* CVCommandUiEntry::NewL( const CVCommand& aCommand )                                         
    {
    CVCommandUiEntry* self = new (ELeave) CVCommandUiEntry( aCommand );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::CVCommandUiEntry
// C++ constructor
// ----------------------------------------------------------------------------
//
CVCommandUiEntry::CVCommandUiEntry( const CVCommand& aCommand ):
    iCommand( aCommand )
    {   
    }

// ----------------------------------------------------------------------------
// CVCommandUiEntry::~CVCommandUiEntry
// Destructor
// ----------------------------------------------------------------------------
//
CVCommandUiEntry::~CVCommandUiEntry() 
    {
    }
 
// ----------------------------------------------------------------------------
// CVCommandUiEntry::UserText
// ----------------------------------------------------------------------------
//
const TDesC& CVCommandUiEntry::UserText() const
    {
    return iCommand.CommandUi().UserText();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::WrittenText
// ----------------------------------------------------------------------------
//
const TDesC& CVCommandUiEntry::WrittenText() const
    {
    return iCommand.CommandUi().WrittenText();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::SpokenText
// ----------------------------------------------------------------------------
//
const TDesC& CVCommandUiEntry::SpokenText() const
    {
    return iCommand.SpokenText();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::AlternativeSpokenText
// ----------------------------------------------------------------------------
//
const TDesC& CVCommandUiEntry::AlternativeSpokenText() const
    {
    return iCommand.AlternativeSpokenText();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::IconLC
// ----------------------------------------------------------------------------
//
CGulIcon* CVCommandUiEntry::IconLC() const
    {
    return iCommand.CommandUi().IconLC();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::FolderListedName
// ----------------------------------------------------------------------------
//
const TDesC& CVCommandUiEntry::FolderListedName() const
    {
    return iCommand.CommandUi().FolderInfo().ListedName();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::FolderTitle
// ----------------------------------------------------------------------------
//
const TDesC& CVCommandUiEntry::FolderTitle() const
    {
    return iCommand.CommandUi().FolderInfo().Title();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::FolderIconLC
// ----------------------------------------------------------------------------
//
CGulIcon* CVCommandUiEntry::FolderIconLC() const
    {
    return iCommand.CommandUi().FolderInfo().IconLC();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::Tooltip
// ----------------------------------------------------------------------------
//
const TDesC& CVCommandUiEntry::Tooltip() const
    {
    return iCommand.CommandUi().Tooltip();
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::PlaySpokenTextL
// ----------------------------------------------------------------------------
//
void CVCommandUiEntry::PlaySpokenTextL( CVCommandHandler& aService,
    MNssPlayEventHandler& aPlayEventHandler ) const
    {
    iCommand.PlaySpokenTextL( aService, aPlayEventHandler );
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::PlayAlternativeSpokenTextL
// ----------------------------------------------------------------------------
//
void CVCommandUiEntry::PlayAlternativeSpokenTextL( CVCommandHandler& aService,
    MNssPlayEventHandler& aPlayEventHandler ) const
    {
    iCommand.PlayAlternativeSpokenTextL( aService, aPlayEventHandler );
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::CancelPlaybackL
// ----------------------------------------------------------------------------
//
void CVCommandUiEntry::CancelPlaybackL( CVCommandHandler& aService ) const
    {
    iCommand.CancelPlaybackL( aService );
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::Command
// ----------------------------------------------------------------------------
//
const CVCommand& CVCommandUiEntry::Command() const
    {
    return iCommand;
    }
    
// ----------------------------------------------------------------------------
// CVCommandUiEntry::IsEqual
// ----------------------------------------------------------------------------
//
TBool CVCommandUiEntry::operator==( const CVCommandUiEntry& aCommand ) const
    {
    return iCommand.Runnable() == aCommand.Command().Runnable();
    }
    
// End of File
