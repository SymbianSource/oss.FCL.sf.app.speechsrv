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
* Description:  The CNssSpeechItemSrsPortal provides the portal to the 
*               SpeechRecognitionUtility object.
*
*/


// INCLUDE FILES
#include "nssvascspeechitemsrsportal.h"
#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::CNssSpeechItemSrsPortal
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssSpeechItemSrsPortal::CNssSpeechItemSrsPortal()
    {
    //Initialize data members
    iState = TNssSpeechItemConstant::EIdle;
    
    iEnginePropertySet = EFalse;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::~CNssSpeechItemSrsPortal
// Destructor
// -----------------------------------------------------------------------------
//
CNssSpeechItemSrsPortal::~CNssSpeechItemSrsPortal()
    {
    RUBY_DEBUG0( "CNssSpeechItemSrsPortal::~CNssSpeechItemSrsPortal" );
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssSpeechItemSrsPortal* CNssSpeechItemSrsPortal::NewL()
    {
    RUBY_DEBUG0( "CNssSpeechItemSrsPortal::NewL" );

    CNssSpeechItemSrsPortal* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssSpeechItemSrsPortal* CNssSpeechItemSrsPortal::NewLC()
    {
    CNssSpeechItemSrsPortal* self = new (ELeave) CNssSpeechItemSrsPortal();
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::SetTerminationState
// If there is no registered SpeechItem object, then delete myself,
// else set the state to Termination State
// -----------------------------------------------------------------------------
//
void CNssSpeechItemSrsPortal::SetTerminationState()
    {
    RUBY_DEBUG0( "CNssSpeechItemSrsPortal::SetTerminationState" );
    
    if ( !iSpeechItemCounter )
        {
        // delete myself
        delete this;
        }
    else
        {
        iState = TNssSpeechItemConstant::ETerminate;
        }
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::Register
// Increment the counter for SpeechItem object
// -----------------------------------------------------------------------------
//
void CNssSpeechItemSrsPortal::Register()
    {
    //increment the counter
    iSpeechItemCounter++;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::Deregister
// Decrement the counter for SpeechItem, and check for Termination state
// -----------------------------------------------------------------------------
//
void CNssSpeechItemSrsPortal::Deregister()
    {
    //decrement the counter
    iSpeechItemCounter--;
    
    if (iState == TNssSpeechItemConstant::ETerminate)
        {
        SetTerminationState();
        }
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::GetTrainingCapability
// Return the Training Capabilities
// -----------------------------------------------------------------------------
//
CArrayFixFlat<TNssVasCoreConstant::TNssTrainingCapability>*  
CNssSpeechItemSrsPortal::GetTrainingCapability()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::IsEnginePropertySet
// Return ETrue or EFalse
// -----------------------------------------------------------------------------
//
TBool CNssSpeechItemSrsPortal::IsEnginePropertySet()
    {
    return iEnginePropertySet;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::SetEngineProperty
// Set the Engine Property data members
// -----------------------------------------------------------------------------
//
void CNssSpeechItemSrsPortal::SetEngineProperty( TInt aModelStorageCapacity, 
                                                 TInt aMaxLoadableModels )
    {
    iModelStorageCapacity = aModelStorageCapacity;
    iMaxLoadableModels = aMaxLoadableModels;
    
    // Set the EnginePropertySet flag
    iEnginePropertySet = ETrue;     
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::ModelStorageCapacity
// Return the Model Storage capacity 
// -----------------------------------------------------------------------------
//
TInt CNssSpeechItemSrsPortal::ModelStorageCapacity()
    {
    return iModelStorageCapacity;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::MaxLoadableModels
// Return the Maximum Loadable Models 
// -----------------------------------------------------------------------------
//
TInt CNssSpeechItemSrsPortal::MaxLoadableModels()
    {
    return iMaxLoadableModels;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemSrsPortal::operator=
// Overloaded operator =
// -----------------------------------------------------------------------------
//
CNssSpeechItemSrsPortal& CNssSpeechItemSrsPortal::operator=( 
                            const CNssSpeechItemSrsPortal& aPortal )
    {
    if ( this != &aPortal )
        {
        iState = aPortal.iState;
        iSpeechItemCounter = aPortal.iSpeechItemCounter;
        iEnginePropertySet = aPortal.iEnginePropertySet;
        iModelStorageCapacity = aPortal.iModelStorageCapacity;
        iMaxLoadableModels = aPortal.iMaxLoadableModels;
        }
    
    return *this;
    }

// End of file
