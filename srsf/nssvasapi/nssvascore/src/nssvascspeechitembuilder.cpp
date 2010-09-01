/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssSpeechItemBuilder provides methods to create SpeechItem object.
*
*/


#include "nssvascspeechitembuilder.h"
#include "rubydebug.h"

// ---------------------------------------------------------
// CNssSpeechItemBuilder::CNssSpeechItemBuilder
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CNssSpeechItemBuilder::CNssSpeechItemBuilder()
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemBuilder::ConstructL
// Symbian 2nd phase constructor can leave.
// This is overloaded function for SpeechItemBuilder from database
// -----------------------------------------------------------------------------
//
void CNssSpeechItemBuilder::ConstructL( CNssVASDatabase *aDatabase )
    {
    // create the Srs Potal object
    iPortal  = CNssSpeechItemSrsPortal::NewL();
    iTrainer = CNssSpeechItemTrainer::NewL( aDatabase );
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemBuilder::NewL
// Two-phased constructor.
// This is for new SpeechItemBuilder
// -----------------------------------------------------------------------------
//
CNssSpeechItemBuilder* CNssSpeechItemBuilder::NewL( CNssVASDatabase *aDatabase )
    {
    RUBY_DEBUG_BLOCK( "CNssSpeechItemBuilder::NewL" );
    
    CNssSpeechItemBuilder* self = NewLC( aDatabase );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssSpeechItemBuilder::NewLC
// Two-phased constructor.
// This is overloaded function for SpeechItemBuilder from database
// -----------------------------------------------------------------------------
//
CNssSpeechItemBuilder* CNssSpeechItemBuilder::NewLC( CNssVASDatabase *aDatabase )
    {
    CNssSpeechItemBuilder* self = new (ELeave) CNssSpeechItemBuilder();
    CleanupStack::PushL( self );
    self->ConstructL( aDatabase );
    return self;
    }

// ---------------------------------------------------------
// CNssSpeechItemBuilder::CreateEmptySpeechItem
// Creates a SpeechItem given a Context and a Portal
// ---------------------------------------------------------
//
CNssSpeechItem* CNssSpeechItemBuilder::CreateEmptySpeechItemL( CNssContext& aContext )
    {
    CNssSpeechItem *speechItem = CNssSpeechItem::NewL( aContext, *iPortal, *iTrainer );
    speechItem->SetTagId( KNssVASDbDefaultValue );   //Identify as a new item
    return speechItem;
    }

// ---------------------------------------------------------
// CNssSpeechItemBuilder::CreateSpeechItem
// Creates a SpeechItem given all data member values.
// ---------------------------------------------------------
//
CNssSpeechItem* CNssSpeechItemBuilder::CreateSpeechItemL( CNssContext& aContext, 
                                                          TUint32 aRuleID, 
                                                          const TDesC& aText, 
                                                          TInt aTagID,
                               TNssVasCoreConstant::TNssTrainedType aTrainedType )
    {
    CNssSpeechItem *speechItem = CNssSpeechItem::NewL( aContext, *iPortal,
                                                       *iTrainer, aRuleID, aText );
    speechItem->SetTrainedType( aTrainedType );
    speechItem->SetTagId( aTagID );   
    return speechItem;
    }


// ---------------------------------------------------------
// CNssSpeechItemBuilder::~CNssSpeechItemBuilder
// Delete the SpeechItemBuilder object and set the Portal's 
// state to Terminate.
// The Builder does not delete the Portal, the Portal  
// deletes itself.
// ---------------------------------------------------------
//
CNssSpeechItemBuilder::~CNssSpeechItemBuilder()
    {
    RUBY_DEBUG0( "CNssSpeechItemBuilder::~CNssSpeechItemBuilder" );

    if ( iPortal )
        {
        //set the SRS Portal's state to termination 
        iPortal->SetTerminationState();
        }
    delete iTrainer;
    }

// End of file
