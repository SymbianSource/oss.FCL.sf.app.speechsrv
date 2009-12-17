/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class reads the resource file and provides access to the
*               information read from the resource file to the controller.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include <barsread.h>
#include "siresourcehandler.h"
#include <nsssisrscontrollerplugin.rsg>
//*#include	"SDConsoleTest.h"		// Unit Test
#include "nsssispeechrecognitiondatacommon.h"
#include "nsssispeechrecognitiondatadevasr.h"
#include "rubydebug.h"

// LOCAL CONSTANTS AND MACROS
_LIT( KResourceFileName, "\\nsssisrscontrollerplugin.rsc" );
_LIT( KResourceDir, "\\resource" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIResourceHandler::CSIResourceHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIResourceHandler::CSIResourceHandler()
    {
    }

// -----------------------------------------------------------------------------
// CSIResourceHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIResourceHandler::ConstructL()
	{
    RUBY_DEBUG_BLOCK( "CSIResourceHandler::ConstructL" );
	ReadResourceFileL();
	}

// -----------------------------------------------------------------------------
// CSIResourceHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIResourceHandler* CSIResourceHandler::NewL()
	{
    RUBY_DEBUG_BLOCK( "CSIResourceHandler::NewL" );
	CSIResourceHandler* self = new(ELeave) CSIResourceHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// Destructor
// -----------------------------------------------------------------------------
// CSIResourceHandler::CSIResourceHandler
// Destructor 
// -----------------------------------------------------------------------------
//
CSIResourceHandler::~CSIResourceHandler()
	{
	delete iDbFileName;
	delete iDataFilenamePrefix;
	delete iDataFilenamePostfix;
	delete iDataFilenameSeperator;
	}

// -----------------------------------------------------------------------------
// CSIResourceHandler::ReadResourceFileL
// Reads the data from the resource file.
// -----------------------------------------------------------------------------
//
void CSIResourceHandler::ReadResourceFileL()
	{
    // letters for drives in search order
    const TBuf<2> KResourceDrivers = _L("cz"); 

    // load resources
	RResourceFile resourceFile;
	RFs fs;
	User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TFileName name;
    TInt i( 0 );
    // try to find from the first driver
    name.Append( KResourceDrivers[i] );
    name.Append( ':' );
    name.Append( KResourceDir );
    name.Append( KResourceFileName );

    TBool found( EFalse );
    
    while ( !found && i < KResourceDrivers.Length() )
        {
        name[0] = KResourceDrivers[i++];
       
        if ( BaflUtils::FileExists(fs, name) )
            {
            // open resource
            resourceFile.OpenL( fs, name );
            CleanupClosePushL( resourceFile );
            found = ETrue;
            }
        }
            
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
	
	// Read Resource file according to it's structure defination
	HBufC8* res = resourceFile.AllocReadLC(SICONTROLLERINFO);

	TResourceReader reader;
	reader.SetBuffer(res);

    iMaxModelsPerBank = reader.ReadInt16();
	iMaxModelsSystem = reader.ReadInt16();

	// Plugin database file name
	iDbFileName = reader.ReadHBufCL();	
	
	// Language package data file path and prefix
	iDataFilenamePrefix = reader.ReadHBufCL();
	
	// Language package data file postfix		
	iDataFilenamePostfix  = reader.ReadHBufCL();
	
	// Language package data file name seperator	
	iDataFilenameSeperator = reader.ReadHBufCL();

	// Cleanup reader, resourceFile, and fs
	CleanupStack::PopAndDestroy(3);
	} 
//  End of File
