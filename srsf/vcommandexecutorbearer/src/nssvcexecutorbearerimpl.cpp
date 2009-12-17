/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Bearer plugin API Implementation file
*
*/


// INCLUDE FILES
#include <e32def.h>
#include <remcon/remconbearerobserver.h>
#include <remcon/remconconverterplugin.h>
#include <remconaddress.h>
#include <remconcoreapi.h>

// Include this once it is exported
// #include <RemConExtensionApi.h>

#include "nssvcexecutorbearerimpl.h"                                     

//Constants
_LIT8( KFormatString, "%c" );

// ---------------------------------------------------------
// NssVCExecutorBearer::NewL()
// Description: Factory function.
// return: Ownership of a new NssVCExecutorBearer.
// ---------------------------------------------------------
//
NssVCExecutorBearer* NssVCExecutorBearer::NewL( TBearerParams& aParams )
    {
    NssVCExecutorBearer* self = new(ELeave) NssVCExecutorBearer( aParams );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// NssVCExecutorBearer::~NssVCExecutorBearer()
// Destructor.
// ---------------------------------------------------------
//
NssVCExecutorBearer::~NssVCExecutorBearer()
    {    
    }

// ---------------------------------------------------------
// NssVCExecutorBearer::NssVCExecutorBearer()
// construction.
// ---------------------------------------------------------
//
NssVCExecutorBearer::NssVCExecutorBearer( TBearerParams& aParams )
:   CRemConBearerPlugin( aParams )
    {
    //Pass
    }

// ---------------------------------------------------------
// NssVCExecutorBearer::ConstructL()
// 2nd-phase construction.
// ---------------------------------------------------------
//
void NssVCExecutorBearer::ConstructL()
    {
    // throw an indication up to RemCon.
    TRemConAddress addr;
    addr.BearerUid() = Uid();
    TInt err = Observer().ConnectIndicate( addr );
    }
    
// ---------------------------------------------------------
// NssVCExecutorBearer::ConnectRequest()
// Rem Con server send connect request to Bearer Plugin
// ---------------------------------------------------------
//
void NssVCExecutorBearer::ConnectRequest(const TRemConAddress& /*aAddr*/)
    {
    // Pass
    }
// ---------------------------------------------------------
// NssVCExecutorBearer::DisconnectRequest()
// Rem Con server send disconnect request to Bearer Plugin
// ---------------------------------------------------------
//
void NssVCExecutorBearer::DisconnectRequest(const TRemConAddress& /*aAddr*/)
    {
    // Pass
    }
// ---------------------------------------------------------
// NssVCExecutorBearer::SendResponse()
// @see MRemConBearerInterface::SendResponse()
// ---------------------------------------------------------
//
TInt NssVCExecutorBearer::SendResponse(TUid /* aInterfaceUid */, 
        TUint /*aOperationId*/, 
        TUint /*aTransactionId*/, // we don't care about this transaction ID
        RBuf8& /* aData */, 
        const TRemConAddress& /*aAddr*/)
    {
    return KErrNone;
    }
// ---------------------------------------------------------
// NssVCExecutorBearer::SendCommand()
// @see MRemConBearerInterface::SendCommand()
// ---------------------------------------------------------
//
TInt NssVCExecutorBearer::SendCommand( TUid  /*aInterfaceUid */ , 
        TUint  aOperationId , 
        TUint /* aTransactionId */,  
        RBuf8& /* aData */, 
        const TRemConAddress& /*aAddr*/)
    {
    // 1. Button click
    iData.Format( KFormatString, ERemConCoreApiButtonClick );
    
    // 2. Operation id. No conversion is done.
    iOperationId = aOperationId;
    
    // 3. Interface ID is required when GetCommand is called
    iInterfaceUid = TUid::Uid( KRemConCoreApiUid ); // from Remconcoreapi.h
    
    // 4. Transaction ID
    iTransactionId = Observer().NewTransactionId();
    
    // 5. Addr 
    TRemConAddress addr;
    addr.BearerUid() = Uid();
    addr.Addr() = KNullDesC8();
    
    // Msgtype  is ERemConCommand
    TInt aError = Observer().NewCommand( addr );
    
    return KErrNone;
    }
// ---------------------------------------------------------
// NssVCExecutorBearer::GetResponse()
// @see MRemConBearerInterface::GetResponse()
// ---------------------------------------------------------
//
TInt NssVCExecutorBearer::GetResponse( TUid& /* aInterfaceUid */, 
        TUint& /* aTransactionId*/, 
        TUint& /* aOperationId */, 
        RBuf8& /* aData */, 
        TRemConAddress& /*aAddr */)
    {
    return KErrNone;
    }
// ---------------------------------------------------------
// NssVCExecutorBearer::GetCommand()
// @see MRemConBearerInterface::GetCommand()
// ---------------------------------------------------------
//
TInt NssVCExecutorBearer::GetCommand( TUid& aInterfaceUid, 
                                      TUint& aTransactionId, 
                                      TUint& aOperationId, 
                                      RBuf8& aData,
                                      TRemConAddress& aAddr )
    {
    TInt error( KErrNone );
    
    aInterfaceUid = iInterfaceUid;
    aOperationId = iOperationId;

    // Pass ownership of this to RemCon.
    error = aData.Create( iData );
    
    if ( error == KErrNone )
        {
        aAddr.BearerUid() = Uid();
        aAddr.Addr() = KNullDesC8();
    
        aTransactionId = iTransactionId;
        }
    
    return error;
    }


// ---------------------------------------------------------
// NssVCExecutorBearer::SecurityPolicy()
// @see MRemConBearerInterface::SecurityPolicy()
// ---------------------------------------------------------
//
TSecurityPolicy NssVCExecutorBearer::SecurityPolicy() const
    {
    return TSecurityPolicy(ECapabilityLocalServices);
    }
// ---------------------------------------------------------
// NssVCExecutorBearer::ClientStatus()
// @see MRemConBearerInterface::ClientStatus()
// ---------------------------------------------------------
//
void NssVCExecutorBearer::ClientStatus(TBool /*aControllerPresent*/, 
                                TBool /*aTargetPresent*/)
    {
    //Pass
    }
// ---------------------------------------------------------
// NssVCExecutorBearer::GetInterface()
// @see CRemConBearerPlugin::GetInterface()
// ---------------------------------------------------------
//
TAny* NssVCExecutorBearer::GetInterface(TUid aUid)
    {
    TAny* ret = NULL;
    if ( aUid == TUid::Uid( KRemConBearerInterface1 ) )
        {
        ret = reinterpret_cast<TAny*>(
                    static_cast<MRemConBearerInterface*>(this)
                    );
        }
    return ret;

    }


// End of file
