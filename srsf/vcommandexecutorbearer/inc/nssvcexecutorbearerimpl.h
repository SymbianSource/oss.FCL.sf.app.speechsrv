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
* Description: 
*       Declaration of class NssVCExecutorBearer.
*       
*
*/


#ifndef __NSSVCEXECUTORBEARERIMPL_H__
#define __NSSVCEXECUTORBEARERIMPL_H__

// INCLUDE
#include <e32base.h>
#include <c32comm.h>
#include <remcon/remconbearerplugin.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/messagetype.h>

//CONSTANTS
const TInt KDataBufferSize = 10;

/**
*  NssVCExecutorBearer
*     Implementation of the Rem Con Bearer Plugin
*  @lib Nssvcexecutorbearer.dll
*  @since 3.0
*/
class NssVCExecutorBearer : public CRemConBearerPlugin,
                     public MRemConBearerInterface
    {
    public:
        /**
        * Creates instance of the NssVCExecutorBearer class.
        *
        * @param  TBearerParams: required for the CRemConBearerPlugin
        * @return : Instance of the NssVCExecutorBearer
        */
        static NssVCExecutorBearer* NewL(TBearerParams& aParams);

        /**
        * Destructor.
        */    
        virtual ~NssVCExecutorBearer();

    private:  // constructors
        /**
        * default constructor.
        * @param aParams: BearerPlugin Params
        */
        NssVCExecutorBearer(TBearerParams& aParams);
        
        /**
        * Two - phase construction.
        */
        void ConstructL();

    private: // from CRemConBearerPlugin
        /** @see CRemConBearerPlugin::GetInterface() */
    	TAny* GetInterface(TUid aUid);

    private: // from MRemConBearerInterface
    
        /** @see MRemConBearerInterface::GetResponse() */
        TInt GetResponse(TUid& aInterfaceUid, 
            TUint& aTransactionId, 
            TUint& aOperationId, 
            RBuf8& aData, 
            TRemConAddress& aAddr);
            
        /** @see MRemConBearerInterface::SendCommand() */            
        TInt SendCommand(TUid aInterfaceUid, 
            TUint aCommand, 
            TUint aTransactionId,  
            RBuf8& aData, 
            const TRemConAddress& aAddr);
            
        /** @see MRemConBearerInterface::GetCommand() */            
        TInt GetCommand(TUid& aInterfaceUid, 
            TUint& aTransactionId, 
            TUint& aCommand, 
            RBuf8& aData, 
            TRemConAddress& aAddr);
            
        /** @see MRemConBearerInterface::SendResponse() */
        TInt SendResponse(TUid aInterfaceUid, 
            TUint aOperationId, 
            TUint aTransactionId, 
            RBuf8& aData, 
            const TRemConAddress& aAddr);

        /** @see MRemConBearerInterface::ConnectRequest() */
        void ConnectRequest(const TRemConAddress& aAddr);
        
        /** @see MRemConBearerInterface::DisconnectRequest() */        
        void DisconnectRequest(const TRemConAddress& aAddr);
        
        /** @see MRemConBearerInterface::ClientStatus() */        
        void ClientStatus(TBool aControllerPresent, TBool aTargetPresent);
        
        /** @see MRemConBearerInterface::SecurityPolicy() */        
        TSecurityPolicy SecurityPolicy() const;


    private: // Data

        // Last received and corrected-decoded message.
        TUid iInterfaceUid;
        TUint iOperationId;

        TBuf8<KDataBufferSize> iData;
        
        TUint iTransactionId;
    };

#endif // __NSSVCEXECUTORBEARERIMPL_H__
