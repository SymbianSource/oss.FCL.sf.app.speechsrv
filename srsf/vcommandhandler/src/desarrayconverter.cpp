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
* Description:  A utility object to convert descriptors to 
*                VAS savable arrays and vise versa
*
*/


#include "rubydebug.h"
#include "desarrayconverter.h"

/**
* Converts the stream data into the array of fixed size descriptors
* @param aSource 8-bit descriptor to convert
*/
CArrayFixFlat<NssRRDText>* CDesArrayConverter::DesC8ToArrayLC( 
											  const TDesC8& aSource )
	{
    NssRRDText buf;
    
    const TInt fieldSize = buf.MaxLength(); 
    TInt textFieldCount = aSource.Length() / fieldSize;
    if( aSource.Length() % fieldSize != 0 )
        {
        textFieldCount++;
        }
    RUBY_DEBUG1( "CDesArrayConverter::DesC8ToArrayLC [%d] slots are needed \
    			to store the streamed command", textFieldCount );
    
    
    CArrayFixFlat<NssRRDText>* array = new (ELeave) CArrayFixFlat<NssRRDText>( textFieldCount );
    CleanupStack::PushL( array );
    
    TInt appendedSize = fieldSize;
    for( TInt i(0); i < textFieldCount; i++ )
        {
        // the last piece
        if( i == textFieldCount - 1 ) 
            {
            appendedSize = aSource.Length() - fieldSize * i;
            }
        buf.Copy( aSource.Mid( fieldSize*i, appendedSize ) );
        array->AppendL( buf );
        }
    return array;
	}

/**
* Constructs Des8 descriptor from the given array slice. Almost a 
* counterpart of the DesC8ToArrayLC
* @param aArray Array to convert
* @param aStartIndex First element to convert. Zero-based
* @param aCount number of elements to convert. 
* @leave KErrArgument if aStartIndex or aCount are out of the array bounds
*/
TDesC8* CDesArrayConverter::ArrayToDesC8LC( const CArrayFixFlat<NssRRDText>& aArray, 
													TInt aStartIndex, TInt aCount )
	{
    __ASSERT_ALWAYS( aStartIndex < aArray.Count(), User::Leave( KErrArgument) );
    __ASSERT_ALWAYS( aStartIndex + aCount <= aArray.Count(), User::Leave( KErrArgument) );
    // Calculate the required descriptor length
    TInt storageLength (0);
    for( TInt i = aStartIndex; i < aStartIndex + aCount; i++ )
        {
        storageLength += aArray[i].Length();
        }
    HBufC8* storage = HBufC8::NewLC( storageLength );
    
    for( TInt i = aStartIndex; i < aStartIndex + aCount; i++ ) 
        {
        // 16 bit chars are converted to 8-bit chars
        storage->Des().Append( aArray[i] );
        }
    return storage;
	}

			
//End of file
