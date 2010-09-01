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
* Description:  MRRD class encapsulated a Recognition Result Data (RRD). RRD serves 
*               as a container for client defined data (text and integers) that 
*               needs to be associated with a voice tag.
*
*/


#ifndef NSSVASMRRD_H
#define NSSVASMRRD_H

//  INCLUDES
#include <e32base.h>

// maximum length for RRD Text
const TInt KNssVASMAXRRDTextLength = 64;

// typedef
typedef TBuf<KNssVASMAXRRDTextLength> NssRRDText;

// CLASS DECLARATION

/**
* MRRD is the interface for Recognition Result Data (RRD) processing. It provides methods 
* for setting and getting the text and integer array that comprise the RRD. A RRD
* is part of Tag. RRD is not created by the client, it is created when a tag is created
* and destroyed when the tag is destroyed. Thus, a client doesnot delete an RRD.
* @lib NssVASApi.lib
* @since 2.0
*/
class MNssRRD
{
public:

    /**
    * Get the text array in RRD
	* @since 2.0
    * @param 
    * @return an array of text
    */
	virtual CArrayFixFlat<NssRRDText>* TextArray() = 0;

    /**
    * Get the integer array in RRD
	* @since 2.0
    * @param 
    * @return an array of integers
    */
	virtual CArrayFixFlat<TInt>* IntArray() = 0;

    /**
    * Set the text array in RRD
	* @since 2.0
    * @param aTextArray pointer to an array of text
    * @return
    */
	virtual void SetTextArrayL(CArrayFixFlat<NssRRDText>* aTextArray) = 0;

    /**
    * Set the integer array in RRD
	* @since 2.0
    * @param aIntArray pointer to an array of integers
    * @return 
    */
	virtual void SetIntArrayL(CArrayFixFlat<TInt>* aIntArray) = 0;

};

#endif // NSSVASMRRD_H  
            
// End of File
