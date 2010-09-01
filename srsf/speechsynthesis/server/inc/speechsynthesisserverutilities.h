/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility classes for speech synthesis server
*
*/


#ifndef SPEECHSYNTHESISSERVERUTILITIES_H
#define SPEECHSYNTHESISSERVERUTILITIES_H

// INCLUDES

#include <e32base.h>
#include <e32property.h>
#include <speechsynthesis.h>


// CONSTANTS

_LIT( KPanic, "SpeechSynthesisServer" );

// Time after server is closed if nobody is connected to it
const TTimeIntervalMicroSeconds32 KSpeechSynthesisShutdownDelay = 0x200000; // 2 seconds


// ----------------------------------------------------------------------------
// TSpeechSynthesisPanic
// Server's panic codes 
// ----------------------------------------------------------------------------
//
enum TSpeechSynthesisPanic
    {
    EPanicGeneral,
    EPanicIllegalFunction
    };

// ----------------------------------------------------------------------------
// PanicClient
//
// ----------------------------------------------------------------------------
//
void PanicClient( const RMessagePtr2& aMessage, TSpeechSynthesisPanic aPanic );


// ----------------------------------------------------------------------------
// MSpeechSynthesisServerObserver
// Observer to receive callbacks from server
// ----------------------------------------------------------------------------
//
class MSpeechSynthesisServerObserver
	{
    
    public:

        /**
        * Initialisation has finished
        * @param aError KErrNone if initialisation succeeded
        * @param aDuration Estimated duration of synthesis
        */
    	virtual void MsssInitComplete( TInt aError, 
	                                   const TTimeIntervalMicroSeconds& aDuration ) = 0;

        /**
        * Synthesis complete
        * @param aError KErrNone synthesis has finalised without errors
        */
	    virtual void MsssPlayComplete( TInt aError ) = 0;
	    
	    /**
	    * Initialisation or synthesis has been aborted. 
	    */
	    virtual void MsssOperationAborted() = 0; 
	
	};


// ----------------------------------------------------------------------------
// MConfigurationObserver
// Observer to receive callbacks from CConfigurationListener
// ----------------------------------------------------------------------------
//
class MConfigurationObserver
    {
        
    public:
    
        /**
        * Change in configuration detected
        */ 
        virtual void McoConfigurationChanged() = 0;
        
    };
    

// ----------------------------------------------------------------------------
// TVoiceConfiguration 
// 
// ----------------------------------------------------------------------------
//
class TVoiceConfiguration : public TVoice
    {
    public:
    
        TUid        iPluginUid; 
        
        TVoiceConfiguration():
            TVoice(),
            iPluginUid( KNullUid ) {};
    };
    


// ----------------------------------------------------------------------------
// Auto shutdown class 
//
// ----------------------------------------------------------------------------
//
class CShutdown : public CTimer
    {
    
    public:
        
        static CShutdown* NewL(); 
        
        void Start();
    
    private:
    
        inline CShutdown();
        
        inline void ConstructL();
        
        void RunL();
        
    };


// ----------------------------------------------------------------------------
// Class used to detect installation and removal of SIS package
//
// ----------------------------------------------------------------------------
// 
class CConfigurationListener : public CActive
    {
    
    public:
        
        static CConfigurationListener* NewL( MConfigurationObserver& aHost );
        
        virtual ~CConfigurationListener();
        
    private:
        
        CConfigurationListener( MConfigurationObserver& aHost );
        
        void ConstructL();
        
        void RunL();
        
        void DoCancel();
        
    private:
        
        RProperty               iProperty;
        
        MConfigurationObserver& iHost; 
    };

// ----------------------------------------------------------------------------
// Class used to detect insertion and removal of MMC card
// @todo Changes needed due to multiple drive support? 
// ----------------------------------------------------------------------------
// 
class CMMCListener : public CActive
    {
    
    public:
        
        static CMMCListener* NewL( MConfigurationObserver& aHost );
        
        virtual ~CMMCListener();
        
    private:
        
        CMMCListener( MConfigurationObserver& aHost );
        
        void ConstructL();
        
        void RunL();
        
        void DoCancel();
        
    private:
        
        RProperty                   iProperty;
        
        TInt                        iPrevValue;
        
        MConfigurationObserver&     iHost; 
    };

#endif // SPEECHSYNTHESISSERVERUTILITIES_H
