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
* Description:  Definition of the CVCSettingsEngine
*
*/


#ifndef CVCSETTINGSENGINE_H
#define CVCSETTINGSENGINE_H


//  INCLUDES
#include <e32base.h>
#include <nssvasapi.h>
#include <nssvasmresetfactorymodelsclient.h>
#include <aknnotewrappers.h>

// FORWARD DECLARATIONS
class CAknKeySoundSystem;

// CLASS DECLARATION

/**
* Definition of CVCSettingsEngine
*/
class CVCSettingsEngine : public CBase,
                              public MNssResetFactoryModelsClient

    {
    
    public:  // Constructors and destructor

         /**
         * Two-phased constructor.
         */
         static CVCSettingsEngine* NewL();
        
         /**
         * Destructor.
         */
         virtual ~CVCSettingsEngine();

    public:  // New Functions

        /**
        * Resets the adapted models
        * @return void
        */
        void ResetAdaptationL();

    protected: // Functions from base classes

        /**
        * From MNssResetFactoryModelsClient
        * @see MNssResetFactoryModelsClient for more information.
        */       
        void HandleResetComplete( TInt aErrorCode );
        
    private:	// Constructors

        /**
        * By default Symbian constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CVCSettingsEngine();

    private: // New functions

        /*
        * Displays error note.
        * @return void
        */
        void DisplayErrorNoteL();

        /**
        * Returns sound system
        * @return CAknKeySoundSystem
        */
        CAknKeySoundSystem* GetSoundSystem() const;

        /**
        * Plays the error sound
        * @param aSound the sound type
        * @return void
        */
        void PlaySound(TAvkonSystemSID aSound) const;

		/**
		* Called when HandleResetComplete() completes
		* @return void
		*/
        void DoHandleResetCompleteL();

        /**
        * Called when HandleResetComplete has an error in its parameter.
        *
        */
        void HandleResetFailed();
        

    private: // Data
    
        CNssVASDBMgr*           iVasDbManager;		// Owned
        MNssContextMgr*         iContextManager;    // Not owned
        CActiveSchedulerWait    iShedulerWait;
    };

#endif  // CVCSETTINGSENGINE_H

// End of File
