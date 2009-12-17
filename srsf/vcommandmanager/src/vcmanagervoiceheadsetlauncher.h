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
* Description:  Headset key observer
*
*/


#ifndef VCMANAGERVOICEHEADSETLAUNCHER_H
#define VCMANAGERVOICEHEADSETLAUNCHER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>
#include <w32std.h> 
#include <eikenv.h>
#include <RemConCallHandlingTargetObserver.h>


// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CRemConInterfaceSelector;
class CRemConCallHandlingTarget;

/**
* Class for starting Voice UI
* @since 5.0
*/
NONSHARABLE_CLASS( CVCommandManagerVoiceHeadSetLauncher ) : public CBase,
                                                            MRemConCallHandlingTargetObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVCommandManagerVoiceHeadSetLauncher* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CVCommandManagerVoiceHeadSetLauncher();

    public: // New functions
     

    public: // Functions from base classes
        
         
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void AnswerCall();
        
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void AnswerEndCall();
         
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void EndCall();
         
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void VoiceDial( const TBool aActivate );
         
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void LastNumberRedial();
         
         /**
        * @see MRemConCallHandlingTargetObserver
        */
        void DialCall( const TDesC8& aTelNumber );
    
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void MultipartyCalling( const TDesC8& aData );
    
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void GenerateDTMF( const TChar aChar );
    
        /**
        * @see MRemConCallHandlingTargetObserver
        */
        void SpeedDial( const TInt aIndex );
 
    private:

        /**
        * C++ default constructor.
        */
        CVCommandManagerVoiceHeadSetLauncher();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Start voice ui
        * @param aDeviceLockMode ETrue if started in device lock mode else EFalse
        */        
        void LaunchVoiceUiL( TBool aDeviceLockMode );
        
        /**
        * Checks if call is active
        * @return Call state or KErrNotFound
        */
        TInt CheckCallState();
        
        /**
        * Checks if video call is active
        * @return ETrue if video call else EFalse
        */
        TBool IsVideoCall();
        
        /**
        * Checks if auto lock is active
        * @return Auto lock value or KErrNotFound
        */
        TInt GetAutoLockValue();

    private:    // Data
    
        CRemConInterfaceSelector*      iSelector;             // Owned
        CRemConCallHandlingTarget*     iCallHandlingTarget;   // Not owned

        RProperty                      iProperty;
    };

#endif // VCMANAGERVOICEHEADSETLAUNCHER_H
            
// End of File
