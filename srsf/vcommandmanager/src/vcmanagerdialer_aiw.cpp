/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the implementation of the class for global dialer.
*
*/


// INCLUDE FILES
#include "vcmanagerdialer_aiw.h"
#include <AiwCommon.hrh>    //KAiwCmdCall
#include <aiwdialdataext.h>
#include <AiwDialDataTypes.h>
#include <AiwServiceHandler.h> 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVcManagerDialer::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVcManagerDialer* CVcManagerDialer::NewL()
    { 
    CVcManagerDialer* self = new (ELeave) CVcManagerDialer;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CVcManagerDialer::~CVcManagerDialer
// Destructor
// ---------------------------------------------------------
// 
CVcManagerDialer::~CVcManagerDialer() 
    {
    delete iServiceHandler;
    }

// ---------------------------------------------------------
// CVcManagerDialer::ExecuteLD
// Execute dialer
// ---------------------------------------------------------
//
void CVcManagerDialer::ExecuteL( TPtrC aName, TPtrC aNumber, TCallType aCallType ) 
    {
    if( aNumber.Length() )
        {
        CAiwDialDataExt* dialData = CAiwDialDataExt::NewLC();   
        dialData->SetNameL( aName.Left( AIWDialDataExt::KMaximumNameLength ) );
        dialData->SetPhoneNumberL( aNumber );
            
        // convert TPbkFielId to TAiwCallType
        CAiwDialData::TCallType callType( CAiwDialData::EAIWForcedCS );
        
        if ( aCallType == EVideo )
            {
            callType = CAiwDialData::EAIWForcedVideo;
            }
        else if ( aCallType == EVoIP )
            {
            callType = CAiwDialData::EAIWVoiP;
            }
            
        dialData->SetCallType( callType );
        
        CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
        dialData->FillInParamListL(paramList);
        
        iServiceHandler->ExecuteServiceCmdL( KAiwCmdCall,
                                             paramList,
                                             iServiceHandler->OutParamListL(),
                                             0,
                                             NULL ); //Callback is NULL as no dial results wanted.
                                            
        CleanupStack::PopAndDestroy( dialData ); 
        }
    }
    
// ---------------------------------------------------------
// CVcManagerDialer::CVcManagerDialer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CVcManagerDialer::CVcManagerDialer()
    { 
    // nothing
    }
    
// ---------------------------------------------------------
// CVcManagerDialer::ConstructL
// Second phase constructor.
// ---------------------------------------------------------
//
void CVcManagerDialer::ConstructL()
    {
    iServiceHandler = CAiwServiceHandler::NewL();

    // Create AIW interest
    RCriteriaArray interest;
    CleanupClosePushL( interest );
    CAiwCriteriaItem* criteria = CAiwCriteriaItem::NewLC( KAiwCmdCall, KAiwCmdCall,
                                                          _L8( "*" ) );
    TUid base;
    base.iUid = KAiwClassBase;
    criteria->SetServiceClass( base );
    User::LeaveIfError( interest.Append( criteria ) );

    // attach to AIW interest
    iServiceHandler->AttachL( interest );
    
    CleanupStack::PopAndDestroy( criteria ); 
    CleanupStack::PopAndDestroy( &interest );
    }


// End of File
