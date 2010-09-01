/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The VasCSpeechItemSrsPortal provides the portal to the 
*               SpeechRecognitionUtility object.
*
*/



#ifndef NSSVASCSPEECHITEMSRSPORTAL_H
#define NSSVASCSPEECHITEMSRSPORTAL_H

#include <nsssispeechrecognitionutilityobserver.h>
#include "nssvassiutilitywrapper.h"

#include "nssvascoreconstant.h" 
#include "nssvastspeechitemconstant.h"     


/**
* This class handles accessing the SpeechRecognitionUtility services. 
* It allows only one transaction for each SpeechItem object to be in 
* progress at any given time. 
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssSpeechItemSrsPortal:public CBase
{
  public:
        
    /**
    * Destructor.
    */
    ~CNssSpeechItemSrsPortal();

    /**
    * Creates CNssSpeechItemSrsPortal.
    * Get the SpeechRecognitionUtility's engine status ?????
    * @since 2.0
    * @param 
    * @return 
    */      
    static CNssSpeechItemSrsPortal* NewL();

    /**
    * Creates CNssSpeechItemSrsPortal.
    * 2 phase construction
    * @since 2.0
    * @param 
    * @return 
    */      
    static CNssSpeechItemSrsPortal* NewLC();
 
    /**
    * Requests that the value of the state be changed to Termination state. 
    * If the counter is zero, the SpeechItemPortal will delete itself.
    * @since 2.0
    * @param 
    * @return 
    */ 
    void SetTerminationState();

    /**
    * Get training capabilities from SpeechRecognitionUtility object.
    * @since 2.0
    * @param 
    * @return 
    */      
    CArrayFixFlat<TNssVasCoreConstant::TNssTrainingCapability>* 
        GetTrainingCapability();

    /**
    * Increment the counter to count the number of created SpeechItem objects.
    * @since 2.0
    * @param 
    * @return 
    */      
    void Register();

    /**
    * Decrement the counter when SpeechItem objects is destroy.
    * If the counter is zero and the state is in termination state, 
    * the portal will destroy the SpeechRecognitionUtility object if exist, 
    * and it destroys itself.
    * @since 2.0
    * @param 
    * @return 
    */      
    void Deregister();



	/**
    * overloaded assignment operator
    * @param 
    * @return reference to the CNssSpeechItemSrsPortal
    */
    CNssSpeechItemSrsPortal& 
        operator=(const CNssSpeechItemSrsPortal& aCNssSpeechItemSrsPortal);


    /**
    * Return the status of whether the Engine Property Set flag
    * @param 
    * @return the status of Engine Property Set flag
    */
    TBool IsEnginePropertySet();

    /**
    * Return the Maximum Loadable Models from the SRS 
    * @param - the Model Storage capacity
    * @param - the Maximum Loadable Models
    * @return 
    */
    void SetEngineProperty(TInt aModelStorageCapacity, TInt aMaxLoadableModels);

    /**
    * Return the Model Storage Capacity from the SRS
    * @param 
    * @return the Model Storage capacity
    */
    TInt ModelStorageCapacity();

    /**
    * Return the Maximum Loadable Models from the SRS 
    * @param 
    * @return the Maximum Loadable Models
    */
    TInt MaxLoadableModels();




  private: //function


    /**
    * C++ constructor.
    */   
    CNssSpeechItemSrsPortal();


  
  private: //data


    // The Portal's state
    TNssSpeechItemConstant::TNssState iState;

    // Counter to count the total of registered SpeechItem objects 
    TInt iSpeechItemCounter; 

    // The SRS's Engine Property has been set flag
    TBool iEnginePropertySet;
    
    // The SRS's ModelStorageCapacity 
    TInt iModelStorageCapacity;

    // The SRS's MaxLoadableModels  
    TInt iMaxLoadableModels;


};

#endif // NSSVASCSPEECHITEMSRSPORTAL_H

// End of file
