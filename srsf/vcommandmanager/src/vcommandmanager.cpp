/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  VCommandManager application which is started during boot. 
*
*/


// INCLUDE FILES
#include "rubydebug.h"

#include <centralrepository.h>

#include "srsfprivatecrkeys.h"
#include <menu2internalcrkeys.h>
#include <f32file.h>
#include "vcommandmanager.h"
#include "vcxmlparser.h"
#include "vcommandmerger.h"

// CONSTANTS
_LIT( KPrivateSubDirectory, "import\\" );
_LIT( KFileFilter, "*.xml" );

//static const TInt KUndefinedLanguage = -1;


// -----------------------------------------------------------------------------
// CVCommandManager::CVCommandManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVCommandManager::CVCommandManager( TBool aForceRetrain )
 : iCommands(), iMatchingIndices(), iLanguageChanged( aForceRetrain )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVCommandManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVCommandManager::ConstructL()
    {
    User::LeaveIfError( iRFs.Connect() );
    
    iParser = CVcXmlParser::NewL( iRFs, *this );
    
    // Create service object
    CVCommandHandler* service = CVCommandHandler::NewL();
    CleanupStack::PushL( service );
    
    ParseAllFilesL();
    
    // Get all existing commands from VCommandHandler
    iCommandArray = service->ListCommandsL();

    CVCommandMerger* merger = CVCommandMerger::NewLC();
    CVCommandArray* deduction = merger->ProduceDeductionByRunnablesLC( *iCommandArray, iCommands );
    CVCommandArray* addition = merger->ProduceAdditionByRunnablesLC( *iCommandArray, iCommands );
    
    service->RemoveCommandsL( deduction->PointerArray(), ETrue );
    service->AddCommandsL( addition->PointerArray(), ETrue );
    
    CleanupStack::PopAndDestroy( addition );
    CleanupStack::PopAndDestroy( deduction );
    CleanupStack::PopAndDestroy( merger );
    CleanupStack::PopAndDestroy( service );
    }

// -----------------------------------------------------------------------------
// CVCommandManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCommandManager* CVCommandManager::NewL( TBool aForceRetrain )
    {
    CVCommandManager* self = new ( ELeave ) CVCommandManager( aForceRetrain );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVCommandManager::ParseAllFilesL
// Goes through directories and parses all xml files.
// -----------------------------------------------------------------------------
//
void CVCommandManager::ParseAllFilesL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandManager::ParseAllFilesL" );
        
    TDriveList drivelist;
    User::LeaveIfError( iRFs.DriveList( drivelist ) ); 

    for ( TInt drive( EDriveA ); drive <= EDriveZ; drive++ )
        {
        if ( drivelist[drive] ) 
            {
            TChar driveLetter; 
			User::LeaveIfError( iRFs.DriveToChar( drive, driveLetter ) );

            TFileName directory;
            iRFs.PrivatePath( directory );
            directory.Insert( 0, _L(" :"));
            directory[0] = driveLetter; // replace " " with a drive letter
            
            // parse files from private directory
            ParseAllFilesFromDirectoryL( directory );
            
            // parse filer from import directory
            directory.Append( KPrivateSubDirectory );
            ParseAllFilesFromDirectoryL( directory );    
            
            }
        }
    }
    

    
// -----------------------------------------------------------------------------
// CVCommandManager::ParseAllFilesFromDirectoryL
// Goes through directory and parses all xml files.
// -----------------------------------------------------------------------------
//
void CVCommandManager::ParseAllFilesFromDirectoryL( const TDesC& aDirectory )
    {
    RUBY_DEBUG_BLOCK( "CVCommandManager::ParseAllFilesFromDirectoryL" );
    
    CDir* dirContents = NULL;

    TFileName searchPath( aDirectory );
    searchPath.Append( KFileFilter );
    
    RUBY_DEBUG1( "CVCommandManager::ParseAllFilesFromDirectoryL - Search path: [%S]", &searchPath );
    
    TInt error = iRFs.GetDir( searchPath, KEntryAttMatchMask, ESortNone, dirContents );   
    if ( error == KErrNone )
        {        
        CleanupStack::PushL( dirContents );
        
        RUBY_DEBUG1( "CVCommandManager::ParseAllFilesFromDirectoryL - Found %d xml-file(s)", dirContents->Count() );

        // Go through all found xml files
        for ( TInt iCounter = 0; iCounter < dirContents->Count(); iCounter++ )
            {
            TEntry entry = (*dirContents)[iCounter];
            if ( !entry.IsDir() )
                {
                TFileName filename( aDirectory );
                filename.Append( entry.iName );                
                
                TRAPD( error, iParser->ParseFileL( filename ) );
                if ( error )
                    {
                    // Reset parser if an error occurs
                    delete iParser;
                    iParser = NULL;
                    
                    iParser = CVcXmlParser::NewL( iRFs, *this );
                    }
                }
            }
         
        CleanupStack::PopAndDestroy( dirContents );    
        }
    else
        {
        RUBY_DEBUG1( "CVCommandManager::ParseAllFilesFromDirectoryL - GetDir returns [%d]", error );
        }
    }
    
// -----------------------------------------------------------------------------
// CVCommandManager::CompareCommands
// Compares if two commands are equal
// -----------------------------------------------------------------------------
//  
TBool CVCommandManager::CompareCommands( const CVCommand* aFirst, const CVCommand* aSecond )
    {
    TBool result = EFalse;
        
    if ( aFirst && aSecond )
        {
        result = aFirst->Runnable() == aSecond->Runnable();
        }
    
    return result;
    }
    
// -----------------------------------------------------------------------------
// CVCommandManager::~CVCommandManager
// Destructor.
// -----------------------------------------------------------------------------
//
CVCommandManager::~CVCommandManager()
    {
    delete iParser;
//    delete iClient;
    delete iCommandArray;
    
    iCommands.ResetAndDestroy();
    iMatchingIndices.Close();
    
    iRFs.Close();
    }

// -----------------------------------------------------------------------------
// CVCommandManager::VCMThreadFunction
// Static main function for Voice Command Manager thread
// -----------------------------------------------------------------------------
//    
TInt CVCommandManager::VCMThreadFunction( TAny* /*aParams*/ )
    {
    __UHEAP_MARK;
    
    // CleanupStack creation
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    CActiveScheduler* scheduler = NULL;
    
    TInt error = KErrNone;
    
    TRAP( error, 

        // ActiveScheduler creation
        scheduler = new ( ELeave ) CActiveScheduler();
        
        CleanupStack::PushL( scheduler );

        CActiveScheduler::Install( scheduler );
        
        TSecureId sid = RThread().SecureId();
        RUBY_DEBUG1( "VCommandManager secure id [%x]", sid.iId );
        
        // Create VCommandManager
        CVCommandManager* vcmanager = CVCommandManager::NewL( EFalse );
        delete vcmanager;

        CleanupStack::PopAndDestroy( scheduler ); 
    ); // TRAP
    
    delete cleanupStack;
    
    __UHEAP_MARKEND;
    
    RUBY_DEBUG0( "CVCommandManager::VCMThreadFunction finished ok" );
    
    return error;
    }

// -----------------------------------------------------------------------------
// CVCommandManager::ConstructCommandL
// Constructs new command with given data
// -----------------------------------------------------------------------------
//      
void CVCommandManager::ConstructCommandL( const TDesC& aWrittenText,
                                          const TDesC& aSpokenText,
                                          const TDesC& aTooltipText,
                                          const TDesC& aFolder,
                                          const TDesC& aFolderTitle,
                                          const TDesC& aParameters, 
                                          TUid aIcon,
                                          TUid aAppId,
                                          const TDesC& aAppName,
                                          TBool aStartImmediately,
                                          TBool aUserCanModify,
                                          const TDesC& aFolderIconFile,
                                      	  TInt aFolderIconIndex
                                           )
    {
    const TInt KCenRepBufferSize = 255;

    RUBY_DEBUG_BLOCKL( "CVCommandManager::ConstructCommandL" );
    
    RUBY_DEBUG1( "CVCommandManager::ConstructCommandL - Creating command: [%S]", &aWrittenText );    
    
    // First check that if the application is hidden so voice command is not created then
    if ( aAppId != KNullUid )
    	{
    	
	    TBuf<KCenRepBufferSize> buf;
	    CRepository* cenRepSession = CRepository::NewLC( KCRUidMenu );
	    cenRepSession->Get( KMenuHideApplication, buf );
	    CleanupStack::PopAndDestroy( cenRepSession );

		buf.LowerCase();
	    TBuf<8> textUidHex;
	    textUidHex.AppendNumFixedWidth( aAppId.iUid, EHex, 8 );    
	    if ( buf.Find( textUidHex ) != KErrNotFound )
	    	{
	    	// hidden application: no voice tag needed
	    	RUBY_DEBUG1( "%S hidden application", &aWrittenText );  
	    	return;
	    	}
        }
        	    
	HBufC8* arguments = HBufC8::NewL( aParameters.Length() );
	CleanupStack::PushL( arguments );
	TPtr8 argumentsPtr = arguments->Des();
	argumentsPtr.Copy( aParameters );        

            
    // Create runnable object
    CVCRunnable* runnable = NULL;        
    if ( aAppId != KNullUid )
        {
        runnable = CVCRunnable::NewL( aAppId, argumentsPtr );
        }
    else
        {
        runnable = CVCRunnable::NewL( aAppName, argumentsPtr );
        }
    CleanupStack::PushL( runnable );
    
    CVCFolderInfo* folderInfo = CVCFolderInfo::NewL( aFolderTitle, aFolder, 0, 
    												 aFolderIconIndex, aFolderIconFile );
    CleanupStack::PushL( folderInfo );
    // Create commandUi object
    CVCCommandUi* commandUi = CVCCommandUi::NewL( aWrittenText,
                                                  *folderInfo,
                                                  aUserCanModify,
                                                  aTooltipText,
                                                  aIcon,
                                                  KNullDesC,
                                                  !aStartImmediately );
    CleanupStack::PopAndDestroy( folderInfo );
    CleanupStack::PushL( commandUi );
       
    // Create command object
    CVCommand* command = CVCommand::NewL( aSpokenText, *runnable, *commandUi );
        
    TBool found = EFalse;
      
    // Check that identical command has not already been found from some xml-file
    for ( int i = 0; i < iCommands.Count(); ++i )
        {
        if ( CompareCommands( iCommands[ i ], command ) )
            {
            found = ETrue;
            break;
            }            
        }
        
    // Add command if not found already
    if ( !found )
        {
        iCommands.Append( command );
        iMatchingIndices.Append( KErrNotFound );
        }
    else
        {
        delete command;
        }
        
    CleanupStack::PopAndDestroy( commandUi ); 
    CleanupStack::PopAndDestroy( runnable ); 
    CleanupStack::PopAndDestroy( arguments );    
    }
    
// -----------------------------------------------------------------------------
// CVCommandManager::MvcxpoVoiceCommandFound
// Callback from VC XML parser
// -----------------------------------------------------------------------------
//        
void CVCommandManager::MvcxpoVoiceCommandFound( const TDesC& aWrittenText,
                                                const TDesC& aSpokenText,
                                                const TDesC& aTooltipText,
                                                const TDesC& aFolder,
                                                const TDesC& aFolderTitle,
                                                const TDesC& aParameters, 
                                                TUid aIcon,
                                                TUid aAppId,
                                                const TDesC& aAppName,
                                                TBool aStartImmediately,
                                                TBool aUserCanModify,
                                                const TDesC& aFolderIconFile,
		                                        TInt aFolderIconIndex
                                                 )
    {
    RUBY_DEBUG0( "CVCommandManager::MvcxpoVoiceCommandFound START" );
    
    TRAP_IGNORE( ConstructCommandL( aWrittenText,
                                    aSpokenText,
                                    aTooltipText,
                                    aFolder,
                                    aFolderTitle,
                                    aParameters,
                                    aIcon,
                                    aAppId,
                                    aAppName,
                                    aStartImmediately,
                                    aUserCanModify,
                                    aFolderIconFile,
                                    aFolderIconIndex ) );
      
    RUBY_DEBUG0( "CVCommandManager::MvcxpoVoiceCommandFound EXIT" );
    }
                                      
// End of File

