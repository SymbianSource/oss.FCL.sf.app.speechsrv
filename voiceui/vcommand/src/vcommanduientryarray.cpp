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
* Description:  A list of CVCommandUiEntry-objects
*
*/



#include <vcommandapi.h>
#include <mmfcontrollerpluginresolver.h>
#include "vcommanduientryarray.h"
#include "rubydebug.h"

// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::NewL
// ----------------------------------------------------------------------------
//
CVCommandUiEntryArray* CVCommandUiEntryArray::NewL( CVCommandArray* aArray )
	{
	RUBY_DEBUG_BLOCK( "CVCommandUiEntryArray::NewL" );
	CVCommandUiEntryArray* self = new (ELeave) CVCommandUiEntryArray( aArray );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::CVCommandUiEntryArray
// ----------------------------------------------------------------------------
//	
CVCommandUiEntryArray::CVCommandUiEntryArray( CVCommandArray* aArray )
    : iCommands(aArray)
    {
    }

// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::ConstructL
// ----------------------------------------------------------------------------
//	
void CVCommandUiEntryArray::ConstructL()
	{
	RUBY_DEBUG_BLOCKL( "CVCommandUiEntryArray::ConstructL" );
	
	for( TInt i( 0 ); i < iCommands->Count(); i++ ) 
		{
		CVCommandUiEntry* addition
		    = CVCommandUiEntry::NewL( iCommands->At( i ) );
		CleanupStack::PushL( addition );
		iListBoxCommands.AppendL( addition );
		CleanupStack::Pop( addition );
		}
	}	

// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::~CVCommandUiEntryArray
// ----------------------------------------------------------------------------
//
CVCommandUiEntryArray::~CVCommandUiEntryArray()
	{
	iListBoxCommands.ResetAndDestroy();
	delete iCommands;
	}

// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::At
// ----------------------------------------------------------------------------
//
const CVCommandUiEntry& CVCommandUiEntryArray::At( TInt aIndex ) const
	{
	return *iListBoxCommands[ aIndex ];
	}

// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::operator[]
// ----------------------------------------------------------------------------
//
const CVCommandUiEntry& CVCommandUiEntryArray::operator[]( TInt aIndex ) const
	{
	return At( aIndex );
	}

// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::Count
// ----------------------------------------------------------------------------
//
TInt CVCommandUiEntryArray::Count() const
	{
	return iListBoxCommands.Count();
	}
	
// ----------------------------------------------------------------------------
// CVCommandUiEntryArray::iView->Clone
// ----------------------------------------------------------------------------
//
CVCommandUiEntryArray* CVCommandUiEntryArray::CloneL()
    {
    RVCommandArray vcommands;
    CleanupResetAndDestroyPushL( vcommands );
    
    for( TInt i( 0 ); i < iCommands->Count(); i++ )
        {
        vcommands.AppendL( CVCommand::NewL( iCommands->At( i ) ) );
        }
    
    CVCommandArray* array = CVCommandArray::NewL( vcommands );
    CleanupStack::PopAndDestroy( &vcommands );
    
    CleanupStack::PushL( array );
    
    CVCommandUiEntryArray* ret = CVCommandUiEntryArray::NewL( array );
    
    CleanupStack::Pop( array );
    
    return ret;
    }


//End of file
