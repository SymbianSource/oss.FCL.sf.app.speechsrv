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
* Description:  Language package data file loader 
*
*/


// INCLUDE FILES
#include "asrplugindataloader.h"
#include "sysconfigprovider.h"


#ifdef __SIND_AUTOCONF_LID
#include <syslangutil.h>
/* MSysConfigProvider is a wrapper for get syslangutil. This makes it easier to test without */
/* dependency to the actual language configuration in the emulator environment. The test  */
/* class implements this interface. */
class TDefaultConfig : public MSysConfigProvider {
public:
	TDefaultConfig() {};
	~TDefaultConfig() {};
	TInt GetInstalledLanguages( CArrayFixFlat<TInt>*& aLanguages, RFs* aFileServerSession = NULL ) 
	{
		return SysLangUtil::GetInstalledLanguages(aLanguages, aFileServerSession);		
	};
};
#endif  // __SIND_AUTOCONF_LID

// Local constants

_LIT( KZDriveLetter, "z" ); 
 

#ifdef __SIND_AUTOCONF_LID
const TUint32 KTtpAutoConfLidPackageType = 1;
const TUint32 KTtpAutoConfLidPackageID   = 1;
const TInt KLidBufGranularity = 1000;
#endif  // __SIND_AUTOCONF_LID


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataLoaderLoader::CDataLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDataLoader::CDataLoader() 
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CDataLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDataLoader::ConstructL( const TDesC& aPrefixOfFileName,
							  const TDesC& aSeparator,                                     
                              const TDesC& aPostfixOfFileName,
                              MSysConfigProvider *aProvider )
    {
    iPrefixOfFileName = HBufC::NewL( aPrefixOfFileName.Length() );
    (*iPrefixOfFileName) = aPrefixOfFileName;
    iSeparator = HBufC::NewL( aSeparator.Length() );
    (*iSeparator) = aSeparator;
    iPostfixOfFileName = HBufC::NewL( aPostfixOfFileName.Length() );
    (*iPostfixOfFileName) = aPostfixOfFileName;
    
	iProvider = aProvider;

    // Open file session
    User::LeaveIfError( iFs.Connect() );
    }

// -----------------------------------------------------------------------------
// CDataLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDataLoader* CDataLoader::NewL( const TDesC& aPrefixOfFileName,
								const TDesC& aSeparator,                                     
                                const TDesC& aPostfixOfFileName,
                                MSysConfigProvider *aProvider )
    {
    CDataLoader* self = new ( ELeave ) CDataLoader();
    
    CleanupStack::PushL( self );
    self->ConstructL( aPrefixOfFileName,
					  aSeparator,                                     
                      aPostfixOfFileName,
                      aProvider);
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CDataLoader::~CDataLoader
// Destructor.
// -----------------------------------------------------------------------------
//
CDataLoader::~CDataLoader()
    {
    delete iPrefixOfFileName,
	delete iSeparator,                                     
    delete iPostfixOfFileName;    
    
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CDataLoader::PackageName
// Construct filename for requested package
// -----------------------------------------------------------------------------
//	
void CDataLoader::PackageName (TFileName &aFileName, TUint32 aPackageType, TUint32 aPackageID) 
{
	// Resolve the name of the file
	// filename is <iPrefixOfFileName>_<PackageType>_<aPackageID><iPostfixOfFileName>

	aFileName.Copy( iPrefixOfFileName->Des() );          //  path/SRSF_
	aFileName.AppendNumUC( aPackageType );               //  path/SRSF_1
	aFileName.Append( iSeparator->Des() );               //  path/SRSF_1_
	aFileName.AppendNumUC( aPackageID );                 //  path/SRSF_1_326
	aFileName.Append( iPostfixOfFileName->Des() );       //  path/SRSF_1_326.bin
}

#ifdef __SIND_AUTOCONF_LID
// -----------------------------------------------------------------------------
// CDataLoader::AppendLoadL
// Load language package file to the end of buffer
// -----------------------------------------------------------------------------
//	
void CDataLoader::AppendLoadL( CBufBase *aBuffer,
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
// CDataLoader::LoadSysLangPackagesL
// Load LID language data to a buffer according to installed system languages
// -----------------------------------------------------------------------------
//	
void CDataLoader::SysLangPackagesToBufferL( CBufBase *aBuffer ) 
{        
	MSysConfigProvider *config = iProvider;
	TDefaultConfig defaultProvider;
	if (!config) config = &defaultProvider;

	// Get installed languages
	CArrayFixFlat<TInt>* languages( NULL );

	//User::LeaveIfError (SysLangUtil::GetInstalledLanguages(languages, &iFs));
	User::LeaveIfError (config->GetInstalledLanguages(languages, &iFs));
	CleanupStack::PushL( languages ) ;

	// Loop over installed languages
	for ( TInt i = 0 ; i < languages->Count(); i++ )
	{
		AppendLoadL(aBuffer, KTtpAutoConfLidPackageType, languages->At(i));
	}// next language
	
	CleanupStack::PopAndDestroy( languages );
}

// -----------------------------------------------------------------------------
// CDataLoader::ConstructLidDataL
// Load LID language data to a buffer according to installed system languages
// -----------------------------------------------------------------------------
//	
HBufC8* CDataLoader::ConstructLidDataL()
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


// -----------------------------------------------------------------------------
// CDataLoader::LoadData
// Load language package file. It resolves the name of the language package
// -----------------------------------------------------------------------------
//	
HBufC8* CDataLoader::LoadData( TUint32 aPackageType, 
								TUint32 aPackageID,
								TUint32 aStartPosition,
								TUint32 aEndPosition ) {

	TInt err;
	HBufC8* buffer = NULL;

#ifdef __SIND_AUTOCONF_LID
	// If LID data is requested, construct it from installed LID binary files
	// Otherwise run normal data loading

	if( aPackageType == KTtpAutoConfLidPackageType &&
		aPackageID   == KTtpAutoConfLidPackageID )
	{
		TRAP( err, buffer = ConstructLidDataL() );
	}
#endif  // __SIND_AUTOCONF_LID

	if (!buffer)	{
		TFileName fileName;
		PackageName (fileName, aPackageType, aPackageID);
	
		// Load data    
		TRAP( err, buffer = DoLoadL( fileName, aStartPosition, aEndPosition ) );
	}

	if ( err == KErrNone )
	{
		return buffer;
  }
	else
	{
		return NULL;
	}    
}
                                         


// -----------------------------------------------------------------------------
// CDataLoader::DoLoadL
// Actual file loading function
// -----------------------------------------------------------------------------
//	
HBufC8* CDataLoader::DoLoadL( TFileName& aFileName,
                              TUint32 aStartPosition, 
                              TUint32 aEndPosition )
    {
    TInt fileSize( 0 );
    RFile dataFile;
    TInt readLength;

    // Open file	
    TInt err = dataFile.Open( iFs, aFileName, EFileRead);
	 
		if ( err != KErrNone )	 
		{		
			aFileName.Replace(0,1,KZDriveLetter); // "z:\"
			TInt err = dataFile.Open( iFs, aFileName, EFileRead);		 
			if ( err != KErrNone )
          {
    			return NULL;
          }
		}

	CleanupClosePushL( dataFile );
    // Try to resolve how much should be read
    readLength = aEndPosition - aStartPosition;
    User::LeaveIfError( dataFile.Size( fileSize ) );

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
    TInt startPosition = ( TInt ) aStartPosition;
    dataFile.Seek( ESeekStart, startPosition );

    // Reserve enough memory for the data
    HBufC8* fileBuffer = HBufC8::NewLC( readLength );
    TPtr8 bufferPtr( fileBuffer->Des() );
    
    User::LeaveIfError( dataFile.Read( bufferPtr ) );

    CleanupStack::Pop( fileBuffer );
    CleanupStack::PopAndDestroy( &dataFile );
    return fileBuffer;
    }

// End of File
