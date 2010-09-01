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
* Description:  Some data serialization functions for Custom Commands
*
*/



#ifndef SICUSTOMCOMMANDDATA_H
#define SICUSTOMCOMMANDDATA_H

//  INCLUDES
#include <e32std.h>
#include <badesca.h>
#include <nsssispeechrecognitiondataclient.h>
#include <mmfcontrollerframework.h>
#include "nsssispeechrecognitioncustomcommandcommon.h"


/**
*  Static functions to internalize and externalize some data types.
*
*  @lib nsssispeechrecognitioncustomcommands.lib
*  @since 2.8
*/
class SICustomCommandData // Static classes don't have Symbian prefix
    {
    public: // New functions
        
 		/**
		*	Internalize a string array.
        * @since	2.8
        * @param	aBuffer             Buffer to be internalized
        * @return	Internalized string array
        */
        static MDesCArray* InternalizeDesCArrayL(const TDesC8& aBuffer);

		/**
		*	Externalize a string array.
        * @since	2.8
        * @param    aArray              Array to be externalized
        * @return	The buffer containing the string array
        */
        static CBufFlat* ExternalizeDesCArrayL(const MDesCArray& aArray);

		/**
		*	Internalize a string array.
        * @since	2.8
        * @param    aBuffer              Buffer to be internalized
        * @return	The array of string arrays
        */
        static RPointerArray<MDesCArray> *InternalizeDesCArrayArrayL(
            const TDesC8& aBuffer);

		/**
		*	Externalize a string array.
        * @since	2.8
        * @param    aArray              Array of string arras to be externalized.
        * @return	The buffer containing the array of string arrays.
        */
        static CBufFlat* ExternalizeDesCArrayArrayL(
	        const RPointerArray<MDesCArray>& aArrayArray);

		/**
		*	Reads parameter data from TMMFMessage. TMMFMessage has 2 param slots.
        * @since	2.8
        * @param    aMessage The message to be read.
        * @param    aSlot The parameter slot to be read. 1 or 2.
        * @return	The data from slot X.
        */
        static HBufC8* ReadMessageDataLC( TMMFMessage& aMessage, TInt aSlot );

		/**
		*	Internalizes a pronunciatoin array.
        * @since	2.8
        * @param    aData The serialized buffer
        * @param    aCount How many pronunciations was saved to aData.
        * @param    aArray Array to be populated.
        */
        static void InternalizePronunArrayL(
            const TDesC8& aData,
            TInt aCount,
            RArray<TSIPronunciationID>& aArray );
   
    };

#endif // SICUSTOMCOMMANDDATA_H

// End of File
