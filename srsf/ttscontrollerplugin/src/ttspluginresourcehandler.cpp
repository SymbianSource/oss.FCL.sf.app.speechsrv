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
*                information read from the resource file.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include "ttspluginresourcehandler.h"
#include <ttspluginsettings.rsg>

// LOCAL CONSTANTS AND MACROS
_LIT( KResourceFileName, "\\ttspluginsettings.rsc" );
_LIT( KResourceDir, "\\resource" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTtsPluginResourceHandler::CTtsPluginResourceHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTtsPluginResourceHandler::CTtsPluginResourceHandler()
    {
    }

// -----------------------------------------------------------------------------
// CTtsPluginResourceHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTtsPluginResourceHandler::ConstructL( RFs& aFs )
    {
    ReadResourceFileL( aFs );
    }

// -----------------------------------------------------------------------------
// CTtsPluginResourceHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTtsPluginResourceHandler* CTtsPluginResourceHandler::NewL( RFs& aFs )
    {
    CTtsPluginResourceHandler* self = new ( ELeave ) CTtsPluginResourceHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aFs );
    CleanupStack::Pop( self ); 
    return self;
    }

// -----------------------------------------------------------------------------
// CTtsPluginResourceHandler::~CTtsPluginResourceHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CTtsPluginResourceHandler::~CTtsPluginResourceHandler()
    {
    delete iTtpGeneralFilename;
    delete iTtpLanguangeFilename;
    delete iTtpFilenamePostfix;
    delete iTtsFilename;
    delete iTtsFilenamePostfix;
    }

// -----------------------------------------------------------------------------
// CTtsPluginResourceHandler::ReadResourceFileL
// Reads the data from the resource file.
// -----------------------------------------------------------------------------
//
void CTtsPluginResourceHandler::ReadResourceFileL( RFs& aFs )
    {
    // letters for drives in search order
    const TBuf<2> KResourceDrivers = _L("cz"); 
    
    // load resources
    RResourceFile resourceFile;
    
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
       
        if ( BaflUtils::FileExists( aFs, name ) )
            {
            // open resource
            resourceFile.OpenL( aFs, name );
            CleanupClosePushL( resourceFile );
            found = ETrue;
            }
        }
            
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
        
	HBufC8* res = resourceFile.AllocReadLC( TTSPLUGININFO );

    TResourceReader theReader;
    theReader.SetBuffer( res );

    iTtpGeneralFilename = theReader.ReadHBufCL();
    iTtpLanguangeFilename = theReader.ReadHBufCL();
    iTtpFilenamePostfix = theReader.ReadHBufCL();
    iTtsFilename = theReader.ReadHBufCL();
    iTtsFilenamePostfix = theReader.ReadHBufCL();

    CleanupStack::PopAndDestroy( res );
    CleanupStack::PopAndDestroy( &resourceFile );
    }

// End of File
