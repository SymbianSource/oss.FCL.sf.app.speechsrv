/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A utility class for converting arrays to descriptors and vise versa
*
*/


 
#ifndef DESARRAYCONVERTER_H
#define DESARRAYCONVERTER_H

#include <nssvasmrrd.h> // NssRRDText

/**
* A utility class for converting arrays to descriptors and vise versa
*/
class CDesArrayConverter : public CBase
	{
	public:
		/**
        * Converts the stream data into the array of fixed size descriptors
        * @param aSource 8-bit descriptor to convert
        */
		static CArrayFixFlat<NssRRDText>* DesC8ToArrayLC( const TDesC8& aSource );
    	
    	/**
        * Constructs DesC8 descriptor from the given array slice. Almost a 
        * counterpart of the DesC8ToArrayLC
        * @param aArray Array to convert
        * @param aStartIndex First element to convert. Zero-based
        * @param aCount number of elements to convert. 
        * @leave KErrArgument if aStartIndex or aCount are out of the array bounds
        */
    	static TDesC8* ArrayToDesC8LC( const CArrayFixFlat<NssRRDText>& aArray, 
    											TInt aStartIndex, TInt aCount );
	};

#endif // DESARRAYCONVERTER_H