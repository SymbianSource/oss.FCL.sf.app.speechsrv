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
* Description:  Data buffer loader for TTPHwDevice test executable
*
*/


// INCLUDE FILES
#include    "ttsplugindataloader.h"




// CONSTANTS

_LIT( KDriveZ, "z:" );

// Assuming language data file name is something like SRSF_2_x[y|yz].bin
const TInt KBeginningOfLanguageId = 7;
#ifdef __SIND_AUTOCONF_LID
const TUint32 KTtpAutoConfLidPackageType = 1;
const TUint32 KTtpAutoConfLidPackageID   = 1;
const TInt KLidBufGranularity = 1000;
#endif  // __SIND_AUTOCONF_LID

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTTSDataLoaderLoader::CTTSDataLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTTSDataLoader::CTTSDataLoader( RFs& aFs,
                                TFileName& aGeneralFileName,
                                TFileName& aPrefixOfTTPFileName,
                                TFileName& aPostfixOfTTPFileName, 
                                TFileName& aTTSFilename,
                                TFileName& aPostfixOfTTSFilename ) :
    iGeneralFileName( aGeneralFileName ),
    iPrefixOfTTPFileName( aPrefixOfTTPFileName ),
    iPostfixOfTTPFileName( aPostfixOfTTPFileName ),
    iTTSFileName( aTTSFilename ),
    iPostfixOfTTSFileName( aPostfixOfTTSFilename ),
    iFs( aFs )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTTSDataLoader::ConstructL()
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTTSDataLoader* CTTSDataLoader::NewL( RFs& aFs,
                                      TFileName& aGeneralFileName,
                                      TFileName& aPrefixOfTTPFileName,
                                      TFileName& aPostfixOfTTPFileName,
                                      TFileName& aTTSFilename,
                                      TFileName& aPostfixOfTTSFilename )
    {
    CTTSDataLoader* self = new ( ELeave ) CTTSDataLoader( aFs,
                                                          aGeneralFileName,
                                                          aPrefixOfTTPFileName,
                                                          aPostfixOfTTPFileName,
                                                          aTTSFilename,
                                                          aPostfixOfTTSFilename );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::~CTTSDataLoader
// Destructor.
// -----------------------------------------------------------------------------
//
CTTSDataLoader::~CTTSDataLoader()
    {
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::LoadGeneralData
// Load general TTP data
// -----------------------------------------------------------------------------
//
HBufC8* CTTSDataLoader::LoadGeneralData( const TInt aPackageType,
                                         const TInt aDataID,
                                         TUint32 aStartPosition, 
                                         TUint32 aEndPosition )
    {
    TFileName fileName( iGeneralFileName );
    fileName.AppendNum( aPackageType );
    fileName.Append( _L( "_" ) );
    fileName.AppendNum( aDataID );
    fileName.Append( iPostfixOfTTPFileName );

    HBufC8* buffer( NULL );
#ifdef __SIND_AUTOCONF_LID

	// If LID data is requested, construct it from installed LID binary files
	// Otherwise run normal data loading

	if( aPackageType == KTtpAutoConfLidPackageType &&
		aDataID   == KTtpAutoConfLidPackageID )
	    {
		TRAPD( err, buffer = ConstructLidDataL() );
		if ( !err )
		    {
		    return buffer;
		    }
	    }
	    
#endif  // __SIND_AUTOCONF_LID    
    TRAPD( error, buffer = DoLoadL( fileName, aStartPosition, aEndPosition ) );
    if ( error == KErrNone )
        {
        return buffer;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::LoadLanguageData
// Load TTP language data
// -----------------------------------------------------------------------------
//	
HBufC8* CTTSDataLoader::LoadLanguageData( const TInt aPackageType, 
                                          TLanguage aLanguage,
                                          TUint32 aStartPosition, 
                                          TUint32 aEndPosition )
    {
    TFileName fileName( iPrefixOfTTPFileName );
    fileName.AppendNum( aPackageType );
    fileName.Append( _L( "_" ) );
    fileName.AppendNum( ( TInt ) aLanguage );
    fileName.Append( iPostfixOfTTPFileName );
    
    HBufC8* buffer( NULL );
    TRAPD( error, buffer = DoLoadL( fileName, aStartPosition, aEndPosition ) );
    if ( error == KErrNone )
        {
        return buffer;
        }
    else
        {
        return NULL;
        }    
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::LoadTtsData
// Load TTS general/language data
// -----------------------------------------------------------------------------
//	
HBufC8* CTTSDataLoader::LoadTtsData( const TInt aPackageType, 
                                     const TInt aDataID,
                                     TUint32 aStartPosition, 
                                     TUint32 aEndPosition )
    {
    TFileName fileName( iTTSFileName );
    fileName.AppendNum( aPackageType );
    fileName.Append( _L( "_" ) );
    fileName.AppendNum( aDataID );
    fileName.Append( iPostfixOfTTSFileName );
    
    HBufC8* buffer( NULL );
    
#ifdef __SIND_AUTOCONF_LID

	// If LID data is requested, construct it from installed LID binary files
	// Otherwise run normal data loading
	if( aPackageType == KTtpAutoConfLidPackageType &&
		aDataID   == KTtpAutoConfLidPackageID )
	    {
		TRAPD( err, buffer = ConstructLidDataL() );
		if ( !err )
		    {
		    return buffer;
		    }
	    }
#endif  // __SIND_AUTOCONF_LID

    
    TRAPD( error, buffer = DoLoadL( fileName, aStartPosition, aEndPosition ) );
    if ( error == KErrNone )
        {
        return buffer;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::GetSupportedLanguagesL
// Fill aLanguages with supported language ids
// -----------------------------------------------------------------------------
//	
void CTTSDataLoader::GetSupportedLanguagesL( RArray<TLanguage>& aLanguages )
    {
    // Get pts languages    
    TFileName mask;
    mask.Append( KDriveZ );
    mask.Append( iTTSFileName );
    mask.Append( _L( "2_*" ) );
    
    DoGetSupportedLanguagesL( aLanguages, mask);
    
    // Get ttp languages
    RArray<TLanguage> ttpLanguages;
    CleanupClosePushL( ttpLanguages );
    
    mask.Zero();
    mask.Append( KDriveZ );
    mask.Append( iTTSFileName );
    mask.Append( _L( "0_*" ) );
    
    DoGetSupportedLanguagesL( ttpLanguages, mask);
    
    // remove languages which are not supported in ttp
    for ( TInt i(0); i < aLanguages.Count(); i++ )
        {
        if ( ttpLanguages.Find( aLanguages[i] ) == KErrNotFound )
            {
            aLanguages.Remove( i );
            }
        }
    CleanupStack::PopAndDestroy( &ttpLanguages );
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::DoLoadL
// Loads the given file.
// -----------------------------------------------------------------------------
//	
HBufC8* CTTSDataLoader::DoLoadL( TFileName& iFile,
                                 TUint32 aStartPosition, 
                                 TUint32 aEndPosition )
    {
    TInt fileSize( 0 );
    RFile dataFile;
    TInt readLength;
    
    TFileName completeFile;
    completeFile.Append( KDriveZ );
    completeFile.Append( iFile );
    
    
    // Try to open file from Z drive
    TInt err = dataFile.Open( iFs, completeFile, EFileRead );

    if ( err != KErrNone )
        {
        return NULL;
        }


    // Try to resolve how much should be read
    readLength = aEndPosition - aStartPosition;
    if ( dataFile.Size( fileSize ) != KErrNone )
        {
        dataFile.Close();
        return NULL;
        }
    if ( fileSize == 0 )
        {
        dataFile.Close();
        return NULL;
        }

    if ( aEndPosition != KMaxTUint32 )
        {
        // Cannot read more than filesize
        if ( ( readLength > fileSize ) || ( readLength < 0 ) )
            {
            readLength = fileSize;
            }
        }
    else
        {
        readLength = fileSize;
        }
    
    // Seek the starting point
    // Casting needed to convert from uint to int which is needed by Seek()
    TInt startPosition = (TInt)aStartPosition;
    dataFile.Seek( ESeekStart, startPosition );

    // Reserve enough memory for the data
    HBufC8* fileBuffer = HBufC8::NewL( readLength );

    TPtr8 bufferPtr( fileBuffer->Des() );
    
    err = dataFile.Read( bufferPtr );
    
    dataFile.Close();
    
    if ( err != KErrNone )
        {
        delete fileBuffer;
        return NULL;
        }
    
    return fileBuffer;
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::DoGetSupportedLanguagesL
// Fill aLanguages with supported language ids 
// -----------------------------------------------------------------------------
//	
void CTTSDataLoader::DoGetSupportedLanguagesL( RArray<TLanguage>& aLanguages,
                                               TFileName& aMask )
    {
    CDir *directoryList( NULL );
    TInt error = iFs.GetDir( aMask, KEntryAttDir, ESortByName, directoryList );
    CleanupStack::PushL( directoryList );

    if ( !error )
        {
        TInt count = directoryList->Count();
        
        for ( TInt i( 0 ); i < count; i++ )
            {
            TPtrC tmp = (*directoryList)[i].iName;
            
            TInt64 id;
            TLex parser( tmp.Mid( KBeginningOfLanguageId , 
                                  tmp.Find( _L(".") ) - KBeginningOfLanguageId  ) );

            User::LeaveIfError( parser.Val( id) );
            
            TLanguage lang( (TLanguage)id );
            
            if ( aLanguages.Find( lang ) == KErrNotFound )
                {
                aLanguages.AppendL( lang );
                }
            }
        }
    
    CleanupStack::PopAndDestroy( directoryList );
    }
    
// -----------------------------------------------------------------------------
// CTTSDataLoader::PackageName
// Construct filename for requested package
// -----------------------------------------------------------------------------
//	
void CTTSDataLoader::PackageName (TFileName &aFileName, TUint32 aPackageType, TUint32 aPackageID) 
{
	// Resolve the name of the file
	// filename is <iPrefixOfTTPFileName>_<PackageType>_<aPackageID><iPostfixOfFileName>

	aFileName.Copy( iPrefixOfTTPFileName );          //  path/SRSF_
	aFileName.AppendNumUC( aPackageType );           //  path/SRSF_1
	aFileName.Append( _L( "_" ) );                   //  path/SRSF_1_
	aFileName.AppendNumUC( aPackageID );             //  path/SRSF_1_326
	aFileName.Append( iPostfixOfTTPFileName );       //  path/SRSF_1_326.bin
}
    
#ifdef __SIND_AUTOCONF_LID
// -----------------------------------------------------------------------------
// CTTSDataLoader::AppendLoadL
// Load language package file to the end of buffer
// -----------------------------------------------------------------------------
//	
void CTTSDataLoader::AppendLoadL( CBufBase *aBuffer,
								TUint32 aPackageType, 
								TUint32 aPackageID ) 
{
	// Resolve filename
	TFileName fileName;
	PackageName (fileName, aPackageType, aPackageID);

	// Load data
	HBufC8* buffer = NULL;
	
	TRAPD( err, buffer = DoLoadL( fileName, 0, KMaxTUint32 ));  // IMPLICIT: LID packages are loaded completely

 	if( err == KErrNone && buffer ) 
 	{
		CleanupStack::PushL( buffer );                    // ResizeL may leave below

		TInt prevSize = aBuffer->Size();                  // size before resize
		TInt newSize  = aBuffer->Size() + buffer->Size(); // new size for lid data buffer

		aBuffer->ResizeL( newSize );                      // resize lid data buffer
		aBuffer->Write( prevSize, *buffer );              // append data from read buffer

		CleanupStack::PopAndDestroy(buffer);              // buffer copied -> free
	}
}



// -----------------------------------------------------------------------------
// CTTSDataLoader::LoadSysLangPackagesL
// Load LID language data to a buffer according to installed system languages
// -----------------------------------------------------------------------------
//	
void CTTSDataLoader::SysLangPackagesToBufferL( CBufBase *aBuffer ) 
    {        

	// Get installed languages
	RArray<TLanguage> languages;
    CleanupClosePushL( languages );
	
	TFileName mask;
    mask.Append( KDriveZ );
    mask.Append( iTTSFileName );
    mask.Append( _L( "1_*" ) );
    DoGetSupportedLanguagesL( languages, mask);
	
	// Loop over installed languages
	for ( TInt i = 0 ; i < languages.Count(); i++ )
	    {
		AppendLoadL(aBuffer, KTtpAutoConfLidPackageType, languages[i] );
	    }// next language
	
	CleanupStack::PopAndDestroy( &languages );
    }

// -----------------------------------------------------------------------------
// CTTSDataLoader::ConstructLidDataL
// Load LID language data to a buffer according to installed system languages
// -----------------------------------------------------------------------------
//	
HBufC8* CTTSDataLoader::ConstructLidDataL()
    {
	CBufBase* lidDataBuffer = CBufSeg::NewL( KLidBufGranularity ) ; // accumulates the binary data from lid model files
	CleanupStack::PushL( lidDataBuffer ) ;

	SysLangPackagesToBufferL (lidDataBuffer);

	// Prepare returnable data
	HBufC8* dataBuffer = NULL;
	
	if( lidDataBuffer->Size() > 0 ) 
	    {
		// If any data was loaded, create a buffer for it
		dataBuffer = HBufC8::NewL( lidDataBuffer->Size() );     // create a returnable buffer

		TPtr8 dataBufferPtr = dataBuffer->Des();                        // get a modifiable pointer to it
		lidDataBuffer->Read( 0, dataBufferPtr, lidDataBuffer->Size() ); // put the lid data into it
    	}

	CleanupStack::PopAndDestroy( lidDataBuffer ) ;  

	return dataBuffer;
    }
#endif  // __SIND_AUTOCONF_LID

// End of File
