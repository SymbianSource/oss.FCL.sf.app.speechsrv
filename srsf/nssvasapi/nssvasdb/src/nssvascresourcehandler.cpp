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
* Description:  Reads the VAS resource file and keeps the information in
*               member variables for later use.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include <barsread.h>
#include <NssVasResource.rsg>
#include "NssVasCVasDbSrvdef.h"
#include "NssVasCResourceHandler.h"
#include "RubyDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNssVasResourceHandler::CNssVasResourceHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssVasResourceHandler::CNssVasResourceHandler() : iDatabasePath( NULL ),
                                                   iDatabaseName( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CNssVasResourceHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNssVasResourceHandler::ConstructL()
    {
    ReadResourceFileL();
    }

// -----------------------------------------------------------------------------
// CNssVasResourceHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssVasResourceHandler* CNssVasResourceHandler::NewL()
    {
    CNssVasResourceHandler* self = new ( ELeave ) CNssVasResourceHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssVasResourceHandler::~CNssVasResourceHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CNssVasResourceHandler::~CNssVasResourceHandler()
    {
    delete iDatabasePath;
    delete iDatabaseName;
    }

// -----------------------------------------------------------------------------
// CNssVasResourceHandler::ReadResourceFileL
// Reads the data from the resource file.
// -----------------------------------------------------------------------------
//
void CNssVasResourceHandler::ReadResourceFileL()
    {
    RUBY_DEBUG_BLOCK( "CVasResourceHandler::ReadResourceFileL" );
  
    // letters for drives in search order
    const TBuf<2> KResourceDrives = _L("cz"); 

    RFs fs;
 	
    // start a file session
    User::LeaveIfError( fs.Connect() );
	CleanupClosePushL ( fs );

    // Declare a resource file
    RResourceFile resourceFile;
    
    TFileName name;
    TInt i( 0 );
    // try to find from the first driver
    name.Append( KResourceDrives[i] );
    name.Append( ':' );
    name.Append( KDC_RESOURCE_FILES_DIR );
    name.Append( KResourceFileName );

    TBool found( EFalse );
    
    while ( !found && i < KResourceDrives.Length() )
        {
        name[0] = KResourceDrives[i++];
       
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

	HBufC8* res = resourceFile.AllocReadLC(VASINFO); // Stack: file session, res file, res reader

	TResourceReader theReader;
	theReader.SetBuffer(res);

    // Get the name of the database file
    iDatabasePath = theReader.ReadHBufCL();
    iDatabaseName = theReader.ReadHBufCL();

    // Release resource reader
    // Close resource file
    // Close file session
	CleanupStack::PopAndDestroy( res );
	CleanupStack::PopAndDestroy( &resourceFile );
	CleanupStack::PopAndDestroy( &fs );
    }

// End of File
