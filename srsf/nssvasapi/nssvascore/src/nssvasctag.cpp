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
* Description:  CNssTag implements MNssTag. In addition, it provides members for use
*               by VAS internal components.
*
*/


// INCLUDE FILES
#include "nssvasctag.h"
#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssTag::CNssTag
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//
CNssTag::CNssTag( CNssContext* aContext, CNssRRD* aRRD, 
                  CNssSpeechItem* aSpeechItem )
    {
    iContext = aContext;
    iRRD = aRRD;
    iSpeechItem = aSpeechItem;
    iTagId = KNssVASDbDefaultValue;
    }

// ---------------------------------------------------------
// CNssTag::~CNssTag
// Destructor
// ---------------------------------------------------------
//
CNssTag::~CNssTag()
    {
    delete iContext;
    iContext = NULL;
    
    delete iRRD;
    iRRD = NULL;
    
    delete iSpeechItem;
    iSpeechItem = NULL;
    }

// ---------------------------------------------------------
// CNssTag::Context
// get the context object for this tag
// ---------------------------------------------------------
//
MNssContext* CNssTag::Context()
    {
    return  iContext;
    }

// ---------------------------------------------------------
// CNssTag::RRD
// get the RRD object for this tag
// ---------------------------------------------------------
//
MNssRRD* CNssTag::RRD()
    {
    return  iRRD;
    }

// ---------------------------------------------------------
// CNssTag::SpeechItem
// get the speech item for this tag
// ---------------------------------------------------------
//
MNssSpeechItem* CNssTag::SpeechItem()
    {
    return iSpeechItem;
    }

// ---------------------------------------------------------
// CNssTag::TagId
// get the tag id for this tag
// ---------------------------------------------------------
//
TInt CNssTag::TagId()
    {
    return iTagId;
    }

// ---------------------------------------------------------
// CNssTag::SetTagId
// Set the tag id for this tag
// ---------------------------------------------------------
//
void CNssTag::SetTagId(TInt aId)
    {
    iTagId = aId;
    
    if ( iSpeechItem )
        {
        iSpeechItem->SetTagId( aId );
        }
    }

// ---------------------------------------------------------
// CNssTag::CopyL
// Make a deep copy of the tag
// ---------------------------------------------------------
//
CNssTag* CNssTag::CopyL()
    {
    CNssContext *context = iContext->CopyL();
    CleanupStack::PushL( context );
    CNssRRD *rrd = iRRD->CopyL();
    CleanupStack::PushL( rrd );
    CNssSpeechItem *speechItem = iSpeechItem->CopyL( context );
    CleanupStack::PushL( speechItem );
    CNssTag* copy = new (ELeave) CNssTag( context, rrd, speechItem );
    CleanupStack::Pop( speechItem );
    CleanupStack::Pop( rrd );
    CleanupStack::Pop( context );
    copy->iTagId = iTagId;
    return copy;
    }

//  End of File  
