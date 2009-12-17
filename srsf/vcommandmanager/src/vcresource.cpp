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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include "vcresource.h"
#include "rubydebug.h"
#include <defaultvoicecommands.rsg>
#include <barsc.h>
#include <barsread.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <numberconversion.h>

// CONSTANTS

// File name extension for EngineeringEnglish resource file
_LIT( KDefaultLanguageExtension, ".rsc" );

// Drives where resource file will be searched
const TBuf<2> KResourceDrives = _L("cz");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcResource::CVcResource
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcResource::CVcResource( RFs& aRFs )
 : iRFs( aRFs )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVcResource::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVcResource::ConstructL( const TDesC& aFileName )
    {
    RUBY_DEBUG_BLOCKL( "CVcResource::ConstructL" );
    
    ResolveResourceFileL( aFileName );
    }

// -----------------------------------------------------------------------------
// CVcResource::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVcResource* CVcResource::NewL( RFs& aRFs, const TDesC& aFileName )
    {
    CVcResource* self = new( ELeave ) CVcResource( aRFs );
    
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CVcResource::~CVcResource
// Destructor.
// -----------------------------------------------------------------------------
//
CVcResource::~CVcResource()
    {
    iKeys.Close();
    iCommands.ResetAndDestroy();
    iCommands.Close();
    iIntegerKeys.Close();
    iIntegerValues.Close();
    }

// -----------------------------------------------------------------------------
// CVcResource::GetCommand
// Gets the localized string based on integer key
// -----------------------------------------------------------------------------
//
HBufC& CVcResource::GetCommandL( TInt aKey )
    {
    RUBY_DEBUG_BLOCKL( "CVcResource::GetCommandL(TInt)" );
    
    TInt index = iKeys.Find( aKey );
    User::LeaveIfError( index );

    return *iCommands[ index ];
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ReadValueFromLocFile
// Gets the localized string based on descriptor key 
// -----------------------------------------------------------------------------
//
HBufC& CVcResource::GetCommandL( const TDesC& aKey )
    {
    RUBY_DEBUG_BLOCKL( "CVcResource::GetCommandL(TDesC)" );
    
    TInt length( 0 );
    TInt result( 0 );
    TDigitType dt( EDigitTypeWestern );
    result = NumberConversion::ConvertFirstNumber( aKey, length, dt );

    if ( length == 0 )
        {
        User::Leave( KErrNotFound );
        }

    return GetCommandL( result );
    }

// -----------------------------------------------------------------------------
// CVcResource::GetValueL
// Gets the localized string based on integer key
// -----------------------------------------------------------------------------
//
TInt CVcResource::GetValueL( TInt aKey )
    {
    RUBY_DEBUG_BLOCKL( "CVcResource::GetValueL(TInt)" );
    
    TInt index = iIntegerKeys.Find( aKey );
    User::LeaveIfError( index );

    return iIntegerValues[ index ];
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ReadValueFromLocFile
// Gets the localized string based on descriptor key 
// -----------------------------------------------------------------------------
//
TInt CVcResource::GetValueL( const TDesC& aKey )
    {
    RUBY_DEBUG_BLOCKL( "CVcResource::GetValueL(TDesC)" );
    
    TInt length( 0 );
    TInt result( 0 );
    TDigitType dt( EDigitTypeWestern );
    result = NumberConversion::ConvertFirstNumber( aKey, length, dt );

    if ( length == 0 )
        {
        User::Leave( KErrNotFound );
        }

    return GetValueL( result );
    }


// -----------------------------------------------------------------------------
// CVcResource::ResolveResourceFileL
// Resolves the correct localized resource file name
// -----------------------------------------------------------------------------
//
void CVcResource::ResolveResourceFileL( const TDesC& aFileName )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveResourceFileL" );
    
    // Resource file found/not found flag
    TBool found( EFalse );
    
    TFileName name;

    name.Append( 'a' ); // this will be replaced by real drive letter
    name.Append( ':' );
    name.Append( KDC_RESOURCE_FILES_DIR );
    name.Append( aFileName );
    name.Append( KDefaultLanguageExtension );
             
    TDriveList drivelist;
    User::LeaveIfError( iRFs.DriveList( drivelist ) ); 
    TInt drive( EDriveA );
    
    while ( !found && drive <= EDriveZ )
        {
        if ( drivelist[drive] ) 
            {
            TChar driveLetter; 
			User::LeaveIfError( iRFs.DriveToChar( drive, driveLetter ) );
            name[0] = driveLetter;
                    
            BaflUtils::NearestLanguageFile( iRFs, name );
         
            if ( BaflUtils::FileExists( iRFs, name ) )
                {
                // Read resource
                found = ETrue;
                ReadResourceFileL( iRFs, name );
                }
            }
        drive++;
        }
        
    }

// -----------------------------------------------------------------------------
// CVcResource::ReadResourceFileL
// Loads the resource file content
// -----------------------------------------------------------------------------
//
void CVcResource::ReadResourceFileL( RFs& aRfs, const TDesC& aFileName )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ReadResourceFileL" );
    
    RResourceFile resourceFile;

    resourceFile.OpenL( aRfs, aFileName );
    CleanupClosePushL( resourceFile );

	HBufC8* res = resourceFile.AllocReadLC( VOICECOMMANDS );

	TResourceReader reader;
	reader.SetBuffer( res );

    TInt index( 0 );
    TInt number( 0 );
    
    // The first WORD contains the number 
    // of structs within the resource
    number = reader.ReadInt16();

    for ( index = 0; index < number ; index++ )
        {
        TInt key = reader.ReadInt16();
        iKeys.Append( key );
        HBufC* command = reader.ReadHBufCL();
        iCommands.Append( command );
        }

	CleanupStack::PopAndDestroy( res );
	
	
	res = resourceFile.AllocReadLC( VCOMMANDINTEGERS );

	reader.SetBuffer( res );

    index = 0;
    number = 0;
    
    // The first WORD contains the number 
    // of structs within the resource
    number = reader.ReadInt16();

    for ( index = 0; index < number ; index++ )
        {
        iIntegerKeys.Append( reader.ReadInt16() );
        iIntegerValues.Append( reader.ReadInt16() );
        }

	CleanupStack::PopAndDestroy( res );
	CleanupStack::PopAndDestroy( &resourceFile );
    }
    
// End of File
