/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*               information read from the resource file.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include <barsread.h>
#include "devasrresourcehandler.h"
#include <nssdevasr.rsg>
#include "rubydebug.h"
#include <data_caging_path_literals.hrh>

// LOCAL CONSTANTS AND MACROS
_LIT( KResourceFileName, "nssdevasr.rsc" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDevASRResourceHandler::CDevASRResourceHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevASRResourceHandler::CDevASRResourceHandler()
    {
    }

// -----------------------------------------------------------------------------
// CDevASRResourceHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevASRResourceHandler::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CDevASRResourceHandler::ConstructL()" );

    ReadResourceFileL();
    }

// -----------------------------------------------------------------------------
// CDevASRResourceHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDevASRResourceHandler* CDevASRResourceHandler::NewL()
    {
    CDevASRResourceHandler* self = new ( ELeave ) CDevASRResourceHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // Self
    return self;
    }

// Destructor
CDevASRResourceHandler::~CDevASRResourceHandler()
    {
    }

// -----------------------------------------------------------------------------
// CDevASRResourceHandler::ReadResourceFileL
// Reads the data from the resource file.
// -----------------------------------------------------------------------------
//
void CDevASRResourceHandler::ReadResourceFileL()
    {
    RUBY_DEBUG_BLOCK( "CDevASRResourceHandler::ReadResourceFileL()" );

    // letters for drives in search order
    const TBuf<2> KResourceDrives = _L("cz"); 

    // load resources
    RResourceFile resourceFile;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
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

    
    HBufC8* res = resourceFile.AllocReadLC( DEVASRINFO );

    TResourceReader theReader;
    theReader.SetBuffer( res );

    iSamplingRate = theReader.ReadInt16();
    iBitsPerSample = theReader.ReadInt16();
    iMicrophoneGain = theReader.ReadInt16();
    iSpeakerVolume = theReader.ReadInt16();
    iSilenceFrames = theReader.ReadInt16();
    iPadEndFrames = theReader.ReadInt16();
    iPadStartFrames = theReader.ReadInt16();
    iBufferSize = theReader.ReadInt16();
    iBuffersInWins = theReader.ReadInt16();
    iBuffersInThumb = theReader.ReadInt16();

    // Cleanup res, fs, resourceFile
    CleanupStack::PopAndDestroy( 3 );
    }

// End of File
