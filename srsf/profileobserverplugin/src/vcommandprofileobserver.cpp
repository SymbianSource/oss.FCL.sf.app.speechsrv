/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  implements profile changes handling
*
*/


#include <aknlists.h>
#include <bautils.h> 

#include <ProEngFactory.h>
#include <MProEngProfileNameArray.h>

#include <vcprofileobserver.rsg>
#include <mmfcontrollerpluginresolver.h> // For CleanupResetAndDestroyPushL

#include "vcommandprofileobserver.h"
#include "rubydebug.h"


// ============================ CONSTANTS ===============================

_LIT( KResourceFile, "z:\\resource\\vcprofileobserver.rsc" );

// UID of the profiles application
const TUid KUidAppProfiles = { 0x100058F8 };

// Executable for changing profiles.
const TUid KProfileChangerUid = { 0x10281D15 };

// Starting id for profile command arguments
const TInt KProfileCommandArgumentBase = 200;

// File name to read the icon from
_LIT( KIconFile, "Z:\\resource\\apps\\vcommand.mif" );

// Index of the profile folder icon in the default folder icon file
// @see CVCFolderIcon::NewL
const TInt KProfileFolderIconIndex = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCommandProfileObserver::CVCommandProfileObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVCommandProfileObserver::CVCommandProfileObserver()
 : iProfileUpdateError( EFalse )
    {
    }
 

// -----------------------------------------------------------------------------
// CVCommandProfileObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::ConstructL()
    {  
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::ConstructL()" );
    
    iService = CVCommandHandler::NewL();  

    iProfileEngine = ProEngFactory::NewEngineL();
    iNameArray = iProfileEngine->ProfileNameArrayLC();     
    CleanupStack::Pop(); // we can't popup C-class with M-class argument
     
    LoadProfileDataL();
    SyncVCommandProfilesL();
    
    // Create the profile engine notify handler only after construction of
    // other parts have been completed successfully. This way, should
    // construction fail, we avoid the situation where a callback is executed
    // on a partially contructed object
    iNotifyHandler = ProEngFactory::NewNotifyHandlerL();
    iNotifyHandler->RequestProfileNameArrayNotificationsL( *this );
    }
 
   
// -----------------------------------------------------------------------------
// CVCommandProfileObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCommandProfileObserver* CVCommandProfileObserver::NewL() 
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::NewL()" );
    
    CVCommandProfileObserver* self = new( ELeave ) CVCommandProfileObserver();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }
 

// -----------------------------------------------------------------------------
// CVCommandProfileObserver::~CVCommandProfileObserver
// Destructor.
// -----------------------------------------------------------------------------
//
CVCommandProfileObserver::~CVCommandProfileObserver()
    {
    RUBY_DEBUG0( "CVCommandProfileObserver::~CVCommandProfileObserver()" );
    
    if( iProfileEngine ) 
        {
        iProfileEngine->Release();    
        }
    iProfileEngine = NULL;
     
    if( iNameArray ) 
        {
        delete iNameArray;
        iNameArray = NULL;
        }
       
    DeleteCache();
        
    delete iService;
    
    if ( iNotifyHandler )
        {
        iNotifyHandler->CancelProfileNameArrayNotifications();
        }
    delete iNotifyHandler;
    delete iProfileFolder;
    delete iFolderTitle;
    delete iProfileTooltip;
    
    iAddedCommands.ResetAndDestroy();
    iAddedCommands.Close();    
    } 
  
 
// -----------------------------------------------------------------------------
// CVCommandProfileObserver::SyncVCommandProfilesL
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::SyncVCommandProfilesL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::SyncVCommandProfilesL()" );
    
    if( !iNameArray || iNameArray->MdcaCount() <= 0 ) 
        {
        return;
        }
    
    // Form the set of commands that should be in
    RVCommandArray profileEngineCommands;
    CleanupResetAndDestroyPushL( profileEngineCommands );
    for( TInt i = 0; i < iNameArray->MdcaCount(); i++ )
        {
        TInt index = iNameArray->FindByName( iNameArray->MdcaPoint( i ) );
        TInt profileId = iNameArray->ProfileId( index );
        CVCommand* command = CreateProfileCommandL( iNameArray->MdcaPoint( i ), profileId );
        profileEngineCommands.AppendL( command );
        }
    
    // delete cache and force iCommands to be reloaded
    //
    DeleteCache();

    const CVCommandArray& storedCommands = ListVCommandsL();
    CVCommandArray* deduction = storedCommands.ProduceUntrainSetByRunnablesLC( profileEngineCommands );
    CVCommandArray* addition = storedCommands.ProduceTrainSetByRunnablesLC( profileEngineCommands );
    
    iService->RemoveCommandsL( deduction->PointerArray(), ETrue );
    iService->AddCommandsL( addition->PointerArray(), ETrue );
    CleanupStack::PopAndDestroy( addition );
    CleanupStack::PopAndDestroy( deduction );
    
    CleanupStack::PopAndDestroy( &profileEngineCommands );
    
    }


// -----------------------------------------------------------------------------
// CVCommandProfileObserver::HandleProfileNameArrayModificationL
// -----------------------------------------------------------------------------
//
// Assume that there will be only one change each time this function is called.
//
void CVCommandProfileObserver::HandleProfileNameArrayModificationL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::HandleProfileNameArrayModificationL()" );
    
    // delete cache and force iCommands to be reloaded
    //
    DeleteCache();
    
    // new array of profile names
    MProEngProfileNameArray* nameArray = iProfileEngine->ProfileNameArrayLC();
          
    // copy profile names to two descriptor arrays
    //
    CDesC16ArrayFlat* oldNames = new ( ELeave ) CDesCArrayFlat( iNameArray->MdcaCount() );   
    CleanupStack::PushL( oldNames );
    
    CDesC16ArrayFlat* newNames = new ( ELeave ) CDesCArrayFlat( nameArray->MdcaCount() );
    CleanupStack::PushL( newNames );
        
    for( TInt i = 0; i < iNameArray->MdcaCount(); i++ )
        {
        oldNames->AppendL( iNameArray->MdcaPoint( i ) );
        }
    for( TInt j = 0; j < nameArray->MdcaCount(); j++ )
        {
        newNames->AppendL( nameArray->MdcaPoint( j ) );
        }
 
         
    // Find changes by deleting equivalent commands from both descriptor arrays.
    //
    for( TInt i = 0; i < newNames->MdcaCount(); i++ ) // go through the new names
        {
        TName searchStr = newNames->MdcaPoint( i );
        TInt index;
        
        oldNames->Find( searchStr, index, ECmpNormal );
        
        if( index != oldNames->MdcaCount() ) // String was found
            {
            oldNames->Delete( index );
            newNames->Delete( i );
            i--;
            }
        }
      
      
    // profile name has been edited
    //
    if( newNames->MdcaCount() == oldNames->MdcaCount() ) 
        {
    
        // now we should have a pair of original and changed name
        //
        if( newNames->MdcaCount() == 1 ) // let's edit the profile name (old name, new name)
            {
            TRAPD( error, EditProfileNameL( oldNames->MdcaPoint( 0 ), newNames->MdcaPoint( 0 ) ) );
            if ( error == KErrGeneral )
                {
                CleanupStack::PopAndDestroy( newNames );    
                CleanupStack::PopAndDestroy( oldNames );
                CleanupStack::Pop(); // nameArray ( M-class pointer )
        
                delete iNameArray;        
                iNameArray = nameArray;
        
                User::Leave( error );
                }
            }
        }
    // new profile(s) has been added
    //
    else if( newNames->MdcaCount() > oldNames->MdcaCount() ) 
        {
        AddNewProfilesL( *newNames, *nameArray );
        }
    // profile(s) has been deleted
    //
    else 
        {
        RemoveProfilesL( *oldNames );
        }
    
    // update old profile names
    //
    CleanupStack::PopAndDestroy( newNames );    
    CleanupStack::PopAndDestroy( oldNames );    
    CleanupStack::Pop(); // nameArray ( M-class pointer )
    
    delete iNameArray;
    iNameArray = nameArray;
    
    if ( iProfileUpdateError )
        {
        iProfileUpdateError = EFalse;
        
        HandleProfileNameArrayModificationL();
        }
    }


// -----------------------------------------------------------------------------
// CVCommandProfileObserver::HandleProfileNameArrayNotificationError
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::HandleProfileNameArrayNotificationError( TInt aError ) 
    {
    RUBY_ERROR1( "HandleProfileNameArrayNotificationError: %d", aError );
    
    if ( aError == KErrLocked )
        {
        iProfileUpdateError = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CVCommandProfileObserver::LoadProfileDataL
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::LoadProfileDataL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::LoadProfileDataL()" );
    
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    
    RResourceFile resourceFile;
    TResourceReader theReader;
    
    TFileName name;
    name.Append( KResourceFile );

    BaflUtils::NearestLanguageFile( fs, name );

    if ( !BaflUtils::FileExists( fs, name ) )
        {
        User::Leave( KErrNotFound );
        }
    
    resourceFile.OpenL( fs, name );
    CleanupClosePushL( resourceFile );

    // Leaves 'res' to cleanupstack
    HBufC8* res = resourceFile.AllocReadLC( VCPROFILEOBSERVERINFO );
    theReader.SetBuffer( res );

    iProfileFolder = theReader.ReadHBufCL();
    iFolderTitle = theReader.ReadHBufCL();
    iProfileTooltip = theReader.ReadHBufCL();
    
    // Cleanup res 
    CleanupStack::PopAndDestroy( res );
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( &fs );
    }
    
    
// -----------------------------------------------------------------------------
// CVCommandProfileObserver::ListVCommandsL
// -----------------------------------------------------------------------------
//
const CVCommandArray& CVCommandProfileObserver::ListVCommandsL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::ListVCommandsL()" );
    
    if( !iCommands )
        {
        iCommands = iService->ListCommandsL();
        }
        
    return *iCommands;
    }
 

// -----------------------------------------------------------------------------
// CVCommandProfileObserver::DeleteCache
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::DeleteCache()
    {
    RUBY_DEBUG0( "CVCommandProfileObserver::DeleteCache()" );
    
    // delete cache and force iCommands to be reloaded
    //
    delete iCommands;
    iCommands = NULL;
    }
 
 
// -----------------------------------------------------------------------------
// CVCommandProfileObserver::AddNewProfileL
// Adds new profile to VCommandhandler.
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::AddNewProfileL( const TDesC& aNewName,
                                               TInt aProfileId,
                                               TBool aCommit )
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::AddNewProfileL()" );
    
    // if profile name doesn't exists
    //
    if( FindProfileIndexL( aNewName ) == KErrNotFound )
        {
        CVCommand* initialCommand = CreateProfileCommandL( aNewName, aProfileId );
        CleanupStack::PushL( initialCommand );

        if ( aCommit )
            {
            iService->AddCommandL( *initialCommand );
            CleanupStack::PopAndDestroy( initialCommand );

            // delete cache and force iCommands to be reloaded
            //
            DeleteCache();
            }
        else
            {
            iAddedCommands.AppendL( initialCommand );
        
            CleanupStack::Pop( initialCommand );            
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CVCommandProfileObserver::AddNewProfilesL
// Adds new profiles to VCommandhandler.
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::AddNewProfilesL( const CDesC16ArrayFlat& aNames,
                                                const MProEngProfileNameArray& aProfileNames,
                                                TBool aCommit )
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::AddNewProfilesL()" );
    
    RVCommandArray commands;
    CleanupClosePushL( commands );
    CleanupResetAndDestroyPushL( commands );
    
    for( TInt i = 0; i < aNames.MdcaCount(); i++ )
        {
        TInt index = aProfileNames.FindByName( aNames.MdcaPoint(i) );
        TInt profileId = aProfileNames.ProfileId( index );
        
        // if profile name doesn't exists
        //
        if( FindProfileIndexL( aNames.MdcaPoint(i) ) == KErrNotFound )
            {
            CVCommand* initialCommand = CreateProfileCommandL( aNames.MdcaPoint(i), profileId );
            CleanupStack::PushL( initialCommand );

            if ( aCommit )
                {
                commands.Append( initialCommand );
                }
            else
                {
                iAddedCommands.AppendL( initialCommand );
                }
                
            CleanupStack::Pop( initialCommand );  
            }
        }
    
    if ( aCommit )
        {
        iService->AddCommandsL( commands );
                
        // delete cache and force iCommands to be reloaded
        //
        DeleteCache();
        }

    CleanupStack::PopAndDestroy( &commands );
    CleanupStack::PopAndDestroy( &commands );
    }
    
// -----------------------------------------------------------------------------
// CVCommandProfileObserver::CreateProfileCommandL
// Creates a VCommand for the given profile attributes
// -----------------------------------------------------------------------------
CVCommand* CVCommandProfileObserver::CreateProfileCommandL( const TDesC& profileName, TInt aProfileId ) const
        {
        CVCFolderInfo* folderInfo = CVCFolderInfo::NewL( iFolderTitle->Des(), 
                                                         iProfileFolder->Des(), 0, 
                                                         KProfileFolderIconIndex,
                                                         KIconFile );
        CleanupStack::PushL( folderInfo );
        CVCCommandUi* commandUi = CVCCommandUi::NewL( profileName,
                                                      *folderInfo, ETrue,
                                                      iProfileTooltip->Des(),
                                                      KUidAppProfiles );
        CleanupStack::PopAndDestroy( folderInfo );
        CleanupStack::PushL( commandUi );

        TBuf8<10>  idDescriptor;
        idDescriptor.Num( KProfileCommandArgumentBase + aProfileId );
        CVCRunnable* runnable = CVCRunnable::NewL( KProfileChangerUid, idDescriptor.Left( idDescriptor.Length() ) );
        CleanupStack::PushL( runnable );
    
        CVCommand* command = CVCommand::NewL( profileName,
                                              *runnable,
                                              *commandUi );
        CleanupStack::PopAndDestroy( runnable );
        CleanupStack::PopAndDestroy( commandUi );
        return command;
        }

// -----------------------------------------------------------------------------
// CVCommandProfileObserver::RemoveProfileL
// Removes profile from VCommandhandler.
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::RemoveProfileL( const TDesC& aName )
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::RemoveProfileL()" );
    
    TInt idx = 0;
    while( ( idx = FindProfileIndexL( aName ) ) != KErrNotFound )  
        {
        iService->RemoveCommandL( ListVCommandsL()[ idx ] );
        
        // delete cache and force iCommands to be reloaded
        //
        DeleteCache(); 
        }
    }
    
// -----------------------------------------------------------------------------
// CVCommandProfileObserver::RemoveProfilesL
// Removes profiles from VCommandhandler.
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::RemoveProfilesL( const CDesC16ArrayFlat& aNames )
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::RemoveProfilesL()" );

    RVCommandArray commands;
    CleanupClosePushL( commands );
    
    for( TInt i = 0; i < aNames.MdcaCount(); i++ )
        {
        TInt idx = 0;
        while( ( idx = FindProfileIndexL( aNames.MdcaPoint(i) ) ) != KErrNotFound )  
            {
            commands.Append( &ListVCommandsL()[ idx ] );
            break;
            }
        }

    iService->RemoveCommandsL( commands );

    // delete cache and force iCommands to be reloaded
    //
    DeleteCache();
    
    CleanupStack::PopAndDestroy( &commands );
    }


// -----------------------------------------------------------------------------
// CVCommandProfileObserver::EditProfileNameL
// Edits the profile name.
// -----------------------------------------------------------------------------
//
void CVCommandProfileObserver::EditProfileNameL( const TDesC& aOldName, const TDesC& aNewName )
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::EditProfileNameL()" );
    
    // Get profile id
    TInt idx = iNameArray->FindByName( aOldName );
    TInt profileId = iNameArray->ProfileId( idx );
    
    idx = FindProfileIndexL( aOldName );
    
    // this finds always user edited one, if one exists
    //
    if( idx == KErrNotFound )
        {
        // for some reason profile doesn't exist in voice command list
        // so let's add it
        //
        return AddNewProfileL( aNewName, profileId ); 
        }
    
    const CVCommand& oldCmd = ListVCommandsL()[ idx ];
        
    CVCCommandUi* commandUi = CVCCommandUi::NewL( aNewName, 
                                                         oldCmd.CommandUi().FolderInfo(), ETrue, 
                                                         oldCmd.CommandUi().Tooltip(), 
                                                         oldCmd.CommandUi().IconUid() );
    CleanupStack::PushL( commandUi );
                                                      
    TBuf8<10>  idDescriptor;
    idDescriptor.Num( profileId );
    CVCRunnable* runnable = CVCRunnable::NewL( oldCmd.Runnable() );
    CleanupStack::PushL( runnable );
    
    CVCommand* command = CVCommand::NewL( aNewName, *runnable, *commandUi );
    CleanupStack::PopAndDestroy( runnable );
    CleanupStack::PopAndDestroy( commandUi );
    CleanupStack::PushL( command );
        
    // Delete old command
    iService->RemoveCommandL( oldCmd );
      
    // delete cache and force iCommands to be reloaded
    //
    DeleteCache(); 
      
    while( ( idx = FindProfileIndexL( aOldName ) ) != KErrNotFound ) 
        {
        iService->RemoveCommandL( ListVCommandsL()[ idx ] );
        DeleteCache(); 
        }
        
    // Add new command
    //    
    iService->AddCommandL( *command );
    CleanupStack::PopAndDestroy( command ); 
    
    // delete cache and force iCommands to be reloaded
    //
    DeleteCache(); 
    }


// ---------------------------------------------------------
// CVCommandProfileObserver::FindProfileIndexL
// ---------------------------------------------------------
//
// Assumes that profiles are in a folder called GetProfileFolderName().
//
// Finds always the user edited command if it exists.
//
TInt CVCommandProfileObserver::FindProfileIndexL( const TDesC& aName )
    {
    RUBY_DEBUG_BLOCK( "CVCommandProfileObserver::FindProfileIndexL()" );
    
    TInt retval = KErrNotFound;
    
    for( TInt i = 0; i < ListVCommandsL().Count(); i++ ) 
        {
        if( ListVCommandsL()[ i ].CommandUi().FolderInfo().Title() == iFolderTitle->Des() && 
            aName == ListVCommandsL()[ i ].CommandUi().WrittenText() ) 
            {
            retval = i;
            
            // user edited command
            //
            if( ListVCommandsL()[ i ].SpokenText() != 
                ListVCommandsL()[ i ].CommandUi().WrittenText() ) 
                {
                break;
                }
            }
        }
    return retval;    
    }


// End of File

