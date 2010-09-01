/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class is used for reading pbkinfoview resources.
*
*/


// INCLUDES
#include "pbkinfoviewreshandler.h"
#include "pbkinfoviewutil.h"

#include <data_caging_path_literals.hrh> // For resource and bitmap directories

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbkInfoViewResHandler::NewL
//
// -----------------------------------------------------------------------------
//
CPbkInfoViewResHandler* CPbkInfoViewResHandler::NewL()
    {
    CPbkInfoViewResHandler* self = new (ELeave) CPbkInfoViewResHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }
    
// -----------------------------------------------------------------------------
// CPbkInfoViewResHandler::CPbkInfoViewResHandler
//
// -----------------------------------------------------------------------------
//
CPbkInfoViewResHandler::CPbkInfoViewResHandler()
	{
	// Nothing
	}

// -----------------------------------------------------------------------------
// CPbkInfoViewResHandler::~CPbkInfoViewResHandler()
// Destructor.
// -----------------------------------------------------------------------------
//
CPbkInfoViewResHandler::~CPbkInfoViewResHandler()
    {
	if ( iResId )
		{
        CEikonEnv::Static()->DeleteResourceFile( iResId );
		}
    }
 
// -----------------------------------------------------------------------------
// CPbkInfoViewResHandler::ConstructL
//
// -----------------------------------------------------------------------------
//
void CPbkInfoViewResHandler::ConstructL( void )
    {
	// Add resource file
	TFileName buf;
	GetResFileName( buf );
	iResId = TPbkInfoViewUtil::AddResFileL( buf );
    }
    
// -----------------------------------------------------------------------------
// CPbkInfoViewResHandler::GetResFileName
// Finds out the pbkinfoview resource file name.
// @param aText File name will be put here.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewResHandler::GetResFileName( TDes& aText )
	{
	TParse parse;
	parse.Set( KResFileName, &KDC_RESOURCE_FILES_DIR, NULL );
	TPbkInfoViewUtil::StrCopy( aText, parse.FullName() );
	}

// -----------------------------------------------------------------------------
// CAspResHandler::GetBitmapFileName
// Finds out the phonebook bitmap file name.
// @param aText File name will be put here.
// -----------------------------------------------------------------------------
//
void CPbkInfoViewResHandler::GetBitmapFileName(TDes& aText)
	{
	TParse parse;
	parse.Set( KBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );
	TPbkInfoViewUtil::StrCopy( aText, parse.FullName() );
	}

// End of file
