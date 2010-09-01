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
* Description:  Does the application launching of the recognized voice command
*
*/


#ifndef __NSSVCAPPLAUNCHER_H__
#define __NSSVCAPPLAUNCHER_H__

//  INCLUDES
//#include "AppControllerConstants.h"
#include "nssvcapplauncher.hrh"
#include <e32base.h>
#include <e32std.h>
#include <apgcli.h>
#include <barsc.h>       // RResourceFile

#include <remconcoreapicontrollerobserver.h>

#include <nssttsutility.h>
#include <nssttscommon.h>

// FORWARD DECLARATIONS
class RFs;
class RResourceFile;

// CLASS DECLARATION

/**
* This is the implementation class for launching applications and initiating profiles.
* @lib AppController.lib
* @since 3.0
*/

class CNssVCAppLauncher : public CBase, public MTtsClientUtilityObserver, public MRemConCoreApiControllerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNssVCAppLauncher* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CNssVCAppLauncher();

    public: // New functions
        
        /**
        * Checks the id and calls the appropriate function.
        * @since Averell 2.0
        * @param aCommandId     ID of command to be executed.
        * @return TInt          KErrNone if OK, else value indicating error situation.
        */
        TInt ExecuteCommandL( TUint aCommandId );
    
    public: // Functions from base classes
        
        // From MTtsClientUtilityObserver
        void MapcPlayComplete( TInt aError );
        
        // From MTtsClientUtilityObserver
        void MapcInitComplete( TInt aError,
                               const TTimeIntervalMicroSeconds& aDuration );
        
        // Not implemented
        void MapcCustomCommandEvent( TInt /*aEvent*/, TInt /*aError*/ ) {};
                        
        /** 
	    A response has been received. 
	    @param aOperationId The operation ID. The response is to a previous 
	    command of this type.
	    @param The response error.
	    */
	    void MrccacoResponse( TRemConCoreApiOperationId aOperationId, TInt aError );

		
    private: // Constructors

        /**
        * C++ default constructor.
        */
        CNssVCAppLauncher();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

	private: // New functions
	    
	    /**
        * Launches calendars day view to a selected day.
        * @since 3.0
        * @param day Which day will be opened in the wiew:
                      today | tomorrow | monday | tuesday | ...
        * @return void
        */
        void CalendarDayViewL( const TInt aDay );
        
        /**
        * Launches calendars week view to a selected week.
        * @since 3.0
        * @param week Which week will be opened in the wiew:
                      this week | next week
        * @return void
        */
        void CalendarWeekViewL( const TInt aWeek );
        
        /**
        * Launches calendars month view to a selected month.
        * @since 3.0
        * @param month Which month will be opened in the wiew:
                       this month | next month | january | february | ...
        * @return void
        */
        void CalendarMonthViewL( const TInt aMonth );
        
        /**
        * Opens browser with the user defined homepage.
        * @since 3.0
        * @return void
        */
        void OpenHomepageL();
        
        /**
        * Activates SMS reader
        * @since 3.0
        * @param aAppUid UID for SMS reader
        * @return
        */
        TInt SMSReaderL( const TUid aAppUid );
        
        /**
        * Provides the answer to the voice command "What time is it?"
        * @since 3.0
        * @return void
        */
        void StatusTimeL();
        
        /**
        * Provides the answer to the voice command "Who is my operator?"
        * @since 3.0
        * @return void
        */
        void StatusOperatorL();
        
        /**
        * Provides the answer to the voice command "What is the battery strength?"
        * @since 3.0
        * @return void
        */
        void StatusBatteryL();
        
        /**
        * Provides the answer to the voice command "What is the signal strength?"
        * @since 3.0
        * @return void
        */
        void StatusSignalL();
        
        /**
        * Performs text to speech answer for a status query.
        * @since 3.0
        * @return void
        */
        void PlaybackStatusInfoL();
        
        /**
        * Connects to RApaLsSession.
        * @since 3.0
        * @return void
        */  
        void CreateApaConnectionL();

        /* Activates an instance of an application.
        * @since 3.0
        * @param aUid Application UID
        * @param aCommandLine Command line parameters.
        * @return void
        */
        void ActivateAppInstanceL( const TUid aUid,
                                   const TDesC16* aCmdLine = NULL );
    
        /**
        * Used to launch applications. Checks whether an instance of the launched
        * application is already running.
        * @since 3.0
        * @param aUid The Uid of the specific application.
        * @param aCommandLine Command line parameters.
        * @param aMultipleInstances If true, the application is activated
        *        in multiple instances.
        * @return void
        */
        void ActivateAppL( const TUid aUid,
                           const TDesC16* aCmdLine = NULL,
                           TBool aMultipleInstances = EFalse );
        
        /**
        * Tells whether it is am or pm.
        * @since 3.0
        * @param aTime Current time
        * @return TTimeOfDay enum value for am or pm is put here
        */
        void GetAmOrPmL( const TTime& aTime, TAmPm& aTod );
        
        /**
        * Gets network operator's name.
        * @since 3.0
        * @param aOperator The name of the operator will be put here
        *        if it is found.
        * @return void
        */
        void GetOperatorL( TDesC*& aOperator );
        
        /**
        * Gets the battery strength.
        * @since 3.0
        * @param aBattStrength The battery strength will be put here
        * return void
        */
        void GetBatteryStrengthL( TSpokenStatusInfo& aBattStrength );
        
        /**
        * Gets the signal strength.
        * @since 3.0
        * @param aSignalStrength The signal strength will be put here
        * @return void
        */
        void GetSignalStrengthL( TSpokenStatusInfo& aSignalStrength );
        
        /**
        * Gets the name of the currently selected TSY.
        * @since 3.0
        * @param aTSYName The name of the currently selected TSY will be put
        *        here.
        * @return void
        */
        void GetTSYNameL( TDes& aTSYName );
        
        /**
        * Executes a media command using Symbian Remote Control FW.
        * @since 3.0
        * @param aCommandId The media command to be executed
        * @return void
        */
        void ExecuteMediaCommandL( TUint aMediaCommandId );
        
        /*
        * Gets localized string from the resource file for text-to-speech
        * status info.
        * @since 3.0
        * @param aStringId The string which we want
        * @param aString Localized string will be put here
        */
        void GetLocalizedStringL( TSpokenStatusInfo aStringId, HBufC*& aString );   


    private:     // Data
        
        RApaLsSession*   iApaLsSession;
		
		// Text to speech stuff
		CTtsUtility*     iTts;
        CTtsParsedText*  iParsedText;
        TTtsStyle        iStyle;
        // Answer for a status query
        HBufC*           iStatusInfo;
        
        // Wait loop for TTS callbacks
        CActiveSchedulerWait iWait;
    };

#endif      // __NSSVCAPPLAUNCHER_H__   
            
// End of File