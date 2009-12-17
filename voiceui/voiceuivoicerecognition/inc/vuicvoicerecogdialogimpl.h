/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the dialog displayed when a recognition is in progress     :
*
*/


#ifndef VUICVOICERECOGDIALOGIMPL_H
#define VUICVOICERECOGDIALOGIMPL_H

//  INCLUDES
#include <nssvasapi.h>
#include <nssttsutility.h> 

#include "vuimkeycallback.h"

#include "voiceuibldvariant.hrh"

// FORWARD DECLARATIONS
class CState;
class CDataStorage;
class CUiModel;
class MVoiceRecognitionDialogCallback;

// CLASS DECLARATION
/**
* The voice recognition dialog
* @lib VoiceUiRecognition.lib
*/
NONSHARABLE_CLASS( CVoiceRecognitionDialogImpl ) : public CActive,
                                                   public MKeyCallback,
                                                   public MNssRecognizeInitCompleteHandler,
                                                   public MNssRecognizeEventHandler,
                                                   public MNssAdaptationEventHandler,
                                                   public MTtsClientUtilityObserver,
                                                   public MNssGetContextClient,
                                                   public MNssGetTagClient,
                                                   public MNssSaveContextClient,
                                                   public MNssTrainTextEventHandler,
                                                   public MNssSaveTagClient,
                                                   public MNssDeleteContextClient
    {

    public:  // Constructors and Destructors

        /**
        * Symbian two-phased constructor.
        */
        static CVoiceRecognitionDialogImpl* NewL( MVoiceRecognitionDialogCallback* aObserver );

        /**
        * Destructor.
        */
        virtual ~CVoiceRecognitionDialogImpl();

    public:  // New functions
    
        /**
        * Executes the dialog
        */
        void ExecuteL();

        /**
        * Only calls will be allowed if device is locked
        */
        void SetOnlyCallsAllowed();
        
        /**
        * Handles property value change event.
        * @param aValue New property value
        */
        void HandlePropertyValueChange( TInt aValue );
        
        /**
        * Sets the next state
        * @param aNextState State which will be activated. Has no effect if value is NULL
        */
        void ChangeState( CState* aNextState );
        
        /**
        * Stops voice recognition
        */
        void Exit();

    protected:  // Functions from base classes
      
        /**
        * From MNssRecognizeInitCompleteHandler Called when recognition initialization is complete
        * @see MNssRecognizeInitCompleteHandler
        */
        void HandleRecognizeInitComplete( TNssRecognizeInitError aErrorCode );

#ifndef __WINS__          
#ifdef __FULLDUPLEX_CHANGE
        /**
        * From MNssRecognizeInitCompleteHandler Called when presampling startup is complete
        * @see MNssRecognizeInitCompleteHandler
        */
        void HandlePreSamplingStarted( TInt aErrorCode );
#endif // __FULLDUPLEX_CHANGE
#endif // __WINS__  

        /**
        * From MNssRecognizeEventHandler Called when recording has started
        * @see MNssRecognizeEventHandler
        */
        void HandleRecordStarted();

        /**
        * From MNssRecognizeEventHandler Called when end-of-utterance is detected
        * @see MNssRecognizeEventHandler
        */
        void HandleEouDetected();

        /**
        * From MNssRecognizeEventHandler Called when recognition is completed
        * @see MNssRecognizeEventHandler
        */
        void HandleRecognizeComplete( CArrayPtrFlat<MNssTag>* aTagList,
                                      TNssRecognizeError aErrorCode  );

        /**
        * From MKeyCallback Called when a softkey is pressed
        * @see MKeyCallback
        */
        void HandleKeypressL( TInt aSoftkey );

        /**
        * From MNssAdaptationEventHandler Called when
        * Adaptation is successful
        * @see MNssAdaptationEventHandler for more information
        */
        void HandleAdaptComplete( TInt aErrorCode );
        
        /**
        * From MMdaAudioPlayerCallback.
        * @see MMdaAudioPlayerCallback for more information.
        */
        void MapcInitComplete( TInt aError, 
                               const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback
        * @see MMdaAudioPlayerCallback for more information.
        */
        void MapcPlayComplete( TInt aError );

        /**
        * From MTtsClientUtilityObserver
        * @see MTtsClientUtilityObserver for more information.
        */
        void MapcCustomCommandEvent( TInt aEvent, TInt aError );

        /**
        * From MGetContextClient
        * @see MGetContextClient for more information
        */        
        void GetContextCompleted( MNssContext* aContext, TInt aErrorCode );

        /**
        * From MGetContextClient
        * @see MGetContextClient for more information
        */        
        void GetContextListCompleted( MNssContextListArray *aContextList, TInt aErrorCode  );
        
        /**
        * From MNssGetTagClient
        * @see MNssGetTagClient for more information
        */  
        void GetTagListCompleted( MNssTagListArray* aTagList, TInt aErrorCode );
        
        /**
        * From MNssSaveContextClient
        * @see MNssSaveContextClient for more information
        */
        void SaveContextCompleted( TInt aErrorCode );

        /**
        * From MNssTrainTextEventHandler
        * @see MNssTrainTextEventHandler for more information
        */       
        void HandleTrainComplete( TInt aErrorCode );
        
        /**
        * From MNssSaveTagClient
        * @see MNssSaveTagClient for more information
        */  
        void SaveTagCompleted( TInt aErrorCode );
        
        /**
        * From MNssDeleteContextClient
        * @see MNssDeleteContextClient for more information
        */ 
        void DeleteContextCompleted( TInt aErrorCode );

    private: // Functions from base classes

        /**
        * From CActive.
        * Handle a change event.
        */
        void RunL();

        /**
        * From CActive.
        * Cancel the request to receive events.
        */
        void DoCancel();

    private:  // New functions
    
        /**
        * C++ default constructor.
        */
        CVoiceRecognitionDialogImpl( MVoiceRecognitionDialogCallback* aObserver );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Checks if the state can be changed and waits until possible.
        */
        void CheckState();

    private:    // Data

        // For storing various things needed in different states
        CDataStorage*                       iDataStorage;
        
        // For storing global UI things
        CUiModel*                           iUiModel;
        
        // Current state
        CState*                             iState;
        
        // Next active state
        CState*                             iNextState;
        
        // Mutex
        RMutex                              iMutex;
        
        // Dialog observer
        MVoiceRecognitionDialogCallback*    iObserver;
   };

#endif    // VUICVOICERECOGDIALOGIMPL_H

// End of File

