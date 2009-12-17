/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class handles profile array changes
*
*/


#include <e32base.h>

#include <MProEngProfileObserver.h>
#include <MProEngEngine.h>
#include <MProEngNotifyHandler.h>
#include <MProEngProfileNameArrayObserver.h>

#include <vcommandapi.h>

class CDesC16ArrayFlat;


/**
 * CVCommandProfileObserver handles profile array changes 
 * { new profile, profile removed, profile name edited }
 * and syncs the voice commands database with the current profiles.
 *
 */
class CVCommandProfileObserver: public CBase, 
                                public MProEngProfileNameArrayObserver
    {
    
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        */
        static CVCommandProfileObserver* NewL();
      
        /**
        * Destructor.
        *
        */
        ~CVCommandProfileObserver();


    public: // Functions from base classes

        /**
        * From MProEngProfileNameArrayObserver
        *
        * @return void
        */        
        void HandleProfileNameArrayModificationL();
        
        /**
        * From MProEngProfileNameArrayObserver
        * 
        * @param aError error code
        * @return void
        */        
        void HandleProfileNameArrayNotificationError( TInt aError );


    private:
        
        /**
        * C++ default constructor.
        *
        */
        CVCommandProfileObserver();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @return void
        */
        void ConstructL();

        /**
        * Returns a list of voice commands.
        *
        * @return CVCommandArray& list of voice commands
        */
        const CVCommandArray& ListVCommandsL();

        /**
        * Loads profile folder name and profile tooltip text.
        *
        * @return void
        */
        void LoadProfileDataL();

        /**
        * Deletes iCommands member variable and thus forces it to be 
        * reloaded when needed next time.
        *
        * @return void
        */
        void DeleteCache();

        /**
        * Adds a profile name change to Vcommand handler.
        *
        * @param aOldName old name of the profile
        * @param aNewName new name for the profile
        * @return void
        */
        void EditProfileNameL( const TDesC& aOldName, const TDesC& aNewName );

        /**
        * Adds a new profile to VCommand handler.
        *
        * @param aNewName Name of the profile to be added
        * @param aProfileId Id of the new profile
        * @param aCommit Set as EFalse if saving to Voice Command services is done
        *                later with CommitAdditionsToVCommandServicesL
        * @return void
        */
        void AddNewProfileL( const TDesC& aNewName, TInt aProfileId, TBool aCommit = ETrue );
        
        /**
        * Adds new profiles to VCommand handler.
        *
        * @param aNames Names of the profiles to be added
        * @param aProfileNames Names of the existing profiles
        * @param aCommit Set as EFalse if saving to Voice Command services is done
        *                later with CommitAdditionsToVCommandServicesL
        * @return void
        */
        void AddNewProfilesL( const CDesC16ArrayFlat& aNames,
                              const MProEngProfileNameArray& aProfileNames,
                              TBool aCommit = ETrue );
        
        /**
        * Creates a VCommand for the given profile attributes
        */
        CVCommand* CreateProfileCommandL( const TDesC& profileName, TInt aProfileId ) const;
        
        /**
        * Removes a profile from VCommand handler.
        *
        * @param aName a name of the profile to be removed
        * @return void
        */
        void RemoveProfileL( const TDesC& aName );
        
        /**
        * Removes profiles from VCommand handler.
        *
        * @param aNames names of the profiles to be removed
        * @return void
        */
        void RemoveProfilesL( const CDesC16ArrayFlat& aNames );

        /**
        * Finds the profile index by name.
        *
        * @param aName name of a profile
        * @return TInt index to the vcommand profile, default is KErrNotFound
        */
        TInt FindProfileIndexL( const TDesC& aName );

        /** 
        * Syncronizes the voice commands at VCommand Handler to equal 
        * to the list of real profiles. This method is called when 
        * profile observing is started.
        *
        * @return void
        */
        void SyncVCommandProfilesL();
     
    private: // Data
    
        MProEngEngine*              iProfileEngine; // owned
        MProEngProfileNameArray*    iNameArray; // owned
       
        MProEngNotifyHandler*       iNotifyHandler; // owned
      
        CVCommandHandler*           iService; // owned
        CVCommandArray*             iCommands; // owned
        
        HBufC*                      iProfileFolder; // owned
        HBufC*                      iFolderTitle; // owned
        HBufC*                      iProfileTooltip; // owned
        
        // Command additions collected into an array
        RVCommandArray              iAddedCommands;
        
        TBool                       iProfileUpdateError;
        
    };
    
    