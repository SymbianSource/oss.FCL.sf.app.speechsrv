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
* Description:  Definition of class used for the playback of a voice tag
*
*/


#ifndef VCPLAYBACKDIALOG_H
#define VCPLAYBACKDIALOG_H


//  INCLUDES
#include <e32base.h>
#include <notecontrol.h>
#include <eikprogi.h>
#include <AknProgressDialog.h>
#include <nssvasmplayeventhandler.h>
#include <vcommand.rsg>
#include "vcommand.hrh"
#include "vcommandconstants.h"

// CONSTANTS
const TInt KVoiceTagRecordLength = 32;
const TInt KVoiceTagIncrement = 1;
const TInt KInterval = 1;
const TInt KDelay = 1;
const TInt KMicroSecondsInterval = 125000;

// FORWARD DECLARATIONS
class CAknProgressDialog;
class CVCommandUiEntry;
class CVCommandHandler;
class CVCommandUiEntry;


// CLASS DECLARATION

/**
* Dialog for voice command playback using TTS
*/
class CVCPlaybackDialog : public CAknProgressDialog,
                          public MNssPlayEventHandler
    {
    struct VTimerModel
        {
        TInt  iFinalValue;
        TInt  iHundreths;
        TInt  iIncrement;
        TBool iRunning;
        };

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CVCPlaybackDialog( CVCommandHandler& iService,
                           const CVCommandUiEntry& aCommand );

        /**
        * Destructor.
        */
        virtual ~CVCPlaybackDialog();

    protected:  // Functions from base classes
        
        /**
        * From CAknProgressDialog
        * @see CAknProgressDialog for more information
        */
        void PreLayoutDynInitL();

        /**
        * From CAknProgressDialog
        * @see CAknProgressDialog for more information
        */
        void PostLayoutDynInitL();

        /**
        * From CAknProgressDialog
        * @see CAknProgressDialog for more information
        */
        TBool OkToExitL( TInt aButtonId );

    private: // functions from base classes

        /**
        * From CAknProgressDialog
        * @see CAknProgressDialog for more information
        */        
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType);

        /**
        * Called when playing of a voice tag is completed
        * @param aErrorCode EVasErrorNone if playing was successfull
        */     
        void HandlePlayComplete( TNssPlayResult aErrorCode );

        /**
        * Called when playing of a voice tag is started
        * @param aDuration - the duration of the utterance data
        */      
        void HandlePlayStarted ( TTimeIntervalMicroSeconds32 aDuration );

    private: // New functions

        /**
        * Starts the timer for the progress bar
        */
        void StartTimerL();

        /**
        * Stops the timer for the progress bar
        */
        void StopTimer();

        /**
        * Called by the timer class for every timer tick
        * @param aObject Points to same place as the this pointer
        * @return static TInt
        */
        static TInt OnTick( TAny* aObject );

        /**
        * Called by OnTick() - updates the progress bar
        */
        void DoTick();

        /**
        * Displays error note
        */
        void DisplayErrorNoteL();
      
        /**
        * Called when HandlePlayError() completes
        * @param aResult error code from TPlayResult
        * @return void
        */
        void DoHandlePlayErrorL( TNssPlayResult aResult );
      
        /**
        * Called when HandlePlayStarted() completes
        * @param aDuration Represents a microsecond time interval stored in 32 bits
        * @return void
        */
        void DoHandlePlayStartedL ( TTimeIntervalMicroSeconds32 aDuration );
        
    private: // data

        CPeriodic*                 iProgressBarTimer;
        CEikProgressInfo*          iProgressInfo;
        VTimerModel                iTModel;
        TInt                       iTickCount;
        TInt                       iExitFlag;
        TInt                       iExitWhenCalledBack;
    	TBool					   iOkToExit;
    	CVCommandHandler&          iService;
    	const CVCommandUiEntry&    iCommand;
    };

#endif  // VCPLAYBACKDIALOG_H


// End of File
