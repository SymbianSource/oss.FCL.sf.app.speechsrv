/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains definitions of utility classes used internally
*               by DevASR.
*
*/


#ifndef CDEVASRUTIL_H
#define CDEVASRUTIL_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Class to encapsulate a queue item.
*
*  @lib DevASR.lib
*  @since 2.0
*/
class CQueItem : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CQueItem* NewL( TUint8* aBuf, TInt aLength );

        /**
        * Destructor.
        */
        virtual ~CQueItem();

    private:

        /**
        * C++ default constructor.
        */
        CQueItem();

        /**
        * Construct a queue item object and initialize it with.
        *
        * @param "TUint8* aBufBuffer" Containing utterance data, this class 
        *        gains owenership.
        * @param "TInt aLength" Length of buffer
        * @param "TInt aMaxLength" Maximum length of buffer
        */
        void ConstructL( TUint8* aBuf, TInt aLength );

    public: // Data

		// Next item
        TSglQueLink* iLink;

        // Descriptor of data
		TPtrC8 iDataBlock;

		// Final block of data indicator
		TBool iFinalBlock;

        // Direct pointer to data buffer
        TUint8* iBuf;
    };

#endif // CDEVASRAPI_H

// End of File
