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
* Description:  DOS Server Monitor Class is used to monitor Audio Accessorries,
*                and encapsulate the interfaces with DOS Server.
*
*/



#ifndef CHeadsetButtonMonitor_H
#define CHeadsetButtonMonitor_H

#include <e32base.h>
#include <dossvrservices.h>
#include <psvariables.h>		// Property values


/**
* DOS Server Monitor Class is used to monitor Audio Accessorries,
* and encapsulate the interfaces with DOS Server.
*/
class CVoiceProgressDialog;
class CPlaybackVoiceDialog;
class CHeadsetButtonMonitor : public CDosEventListenerBase
    {
    public: // Constructors and destructor

        /**
        * The CVoiceTagHeadsetButtonMonitor destructor
        * @param 
        * @return 
        */   
        virtual ~CHeadsetButtonMonitor();


        /**
        * 2nd phase CVoiceTagHeadsetButtonMonitor constructor
        * @param aObserver - an observer to be notified when the disruption occurred.
        * @return a pointer to the new CActiveClass0SMSMonitor object
        */   
        static CHeadsetButtonMonitor* NewLC();


        /**
        * 2nd phase CVoiceTagHeadsetButtonMonitor constructor
        * @param aObserver - an observer to be notified when the disruption occurred.
        * @return a pointer to the new CActiveClass0SMSMonitor object
        */   
        static CHeadsetButtonMonitor* NewL();

    public:
    
        /**
        * Method to handle the registration for Audio Accessories disruption monitor.
        * @param aDisruptionType - a disruption type to be monitored
        * @return - Symbian standard error response
        */   
        TInt ActivateMonitorL();


        /**
        * Method to handle the deregistration of monitoring class 0 SMS
        * @param aDisruptionType - a disruption type to be stopped monitoring
        * @return 
        */  
        TInt DeActivateMonitor();

        /**
        * Method to handle the deregistration of monitoring class 0 SMS
        * @param aDisruptionType - a disruption type to be stopped monitoring
        * @return 
        */  
        void Register(CVoiceProgressDialog* aProgressObserver,CPlaybackVoiceCommandDialog* aPlaybackObserver);

        /**
        * Method to handle the deregistration of monitoring class 0 SMS
        * @param aDisruptionType - a disruption type to be stopped monitoring
        * @return 
        */  
        void DeRegister(CVoiceProgressDialog* aProgressObserver,CPlaybackVoiceCommandDialog* aPlaybackObserver);


        /**
        * Method to cancel the monitoring operation from Dos Server
        * @param 
        * @return 
        */   
        void CancelMonitor();

    protected:

        /**
        * It notifies about a change in the status of the headset.
        * @param aStatus The new status.
        * @return void
        */ 
		void HeadsetButtonChangedL(EPSButtonState aState);

        /**
        * Handles headset button down operation
        * @return void
        */
        void HandleButtonDown();
        
        /**
        * Handles head set button up operation
        * return void
        */
        void HandleButtonUp();
        
        /**
        * Handles head set button long press down operation
        * return void
        */
        void HandleButtonDownLong();

      private:

        /**
        * 2nd phase CVoiceTagHeadsetButtonMonitor constructor
        * @param 
        * @return 
        */   
        void ConstructL();

       /**
        * The CVoiceTagHeadsetButtonMonitor constructor
        * @param aObserver - an observer to be notified when the disruption occurred.
        * @return 
        */   
        CHeadsetButtonMonitor ();

    private: // data

        RDosServer                   iDosServerSession;
        CVoiceProgressDialog*        iProgressObserver;
        CPlaybackVoiceCommandDialog* iPlaybackObserver;
        TBool                        iButtonDown;
    };


#endif // CHeadsetButtonMonitor_H


// End of File
