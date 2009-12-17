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
* Description:  CNssAdaptationItem handles speaker adaptation.
*
*/


// INCLUDE FILES
#include "nssvascadaptationitem.h"
#include "rubydebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNssAdaptationItem::CNssAdaptationItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssAdaptationItem::CNssAdaptationItem(
    CNssRecognitionHandler* aRecHandlerHost,
    CSIClientResultSet*  aRecResult)

 :  iRecHandler( aRecHandlerHost ),
    iIsActive  ( ETrue ),
    iAdaptationData( aRecResult )

    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CNssAdaptationItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssAdaptationItem* CNssAdaptationItem::NewL( CNssRecognitionHandler* aHandler, 
                                                       CSIClientResultSet* aResultSet )
    {
    if ( !aHandler || !aResultSet )
        {
        User::Leave( KErrArgument );
        }

    CNssAdaptationItem* self = new( ELeave ) CNssAdaptationItem( aHandler, aResultSet );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CNssAdaptationItem::~CNssAdaptationItem
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssAdaptationItem::~CNssAdaptationItem()
    {
    if ( iAdaptationData )
        {
        delete iAdaptationData;
        iAdaptationData = 0;
        }

    // Announce for the host recognition handler, that we no longer exist.
    if ( iRecHandler )
        {
        iRecHandler->RemoveAdaptationItem( this );
        }
    }

// -----------------------------------------------------------------------------
// CNssAdaptationItem::AdaptL
// Starts adaptation.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNssAdaptationItem::AdaptL( MNssAdaptationEventHandler* aHandler,
                                          TInt aCorrect )
    {
    // This can happen, if the client has deleted the recognition handler.
    if ( !iRecHandler )
        {
        User::Leave( KErrNotReady );
        }

    iRecHandler->AdaptL( aHandler, iAdaptationData, aCorrect );
    }

// -----------------------------------------------------------------------------
// CNssAdaptationItem::AdaptL
// Starts adaptation based on tag
// -----------------------------------------------------------------------------
//
EXPORT_C void CNssAdaptationItem::AdaptL( MNssAdaptationEventHandler* aHandler,
                                          MNssTag& aTag )
    {
    TInt correctIndex( KErrNotFound );
    
    CNssSpeechItem* speechItem = static_cast<CNssSpeechItem*>( aTag.SpeechItem() );
    CNssContext* context = static_cast<CNssContext*>( aTag.Context() );
   
    // Find the index for the correct result
    for ( TInt iCounter = 0; iCounter < iAdaptationData->ResultCount(); iCounter++ )
        {
        const CSIClientResult& result = iAdaptationData->AtL( iCounter );
        if ( ( result.GrammarID() == context->GrammarId() ) && ( result.RuleID() == speechItem->RuleID() ) )
            {
            correctIndex = iCounter;   
            }
        }
    
    // Leave with KErrNotFound if no match in grammar id and rule id
    User::LeaveIfError( correctIndex );
    
    AdaptL( aHandler, correctIndex );
    }

// -----------------------------------------------------------------------------
// CNssAdaptationItem::Disable
// Host recognition handler calls this function, when it is destroyed.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNssAdaptationItem::Disable()
    {
    if ( iIsActive )
        {
        RUBY_DEBUG0( "CNssAdaptationItem::Disable() is active" );

        delete iAdaptationData;
        iAdaptationData = 0;
       
        iRecHandler = 0;

        iIsActive = EFalse;
        }
    }

// End of File  
