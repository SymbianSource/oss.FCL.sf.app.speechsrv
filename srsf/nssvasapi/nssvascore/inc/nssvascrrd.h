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
* Description:  CNssRRD class encapsulated a RRD. It implements MNssRRD.
*
*/


#ifndef NSSVASCRRD_H
#define NSSVASCRRD_H

//  INCLUDES
#include "nssvasmrrd.h"
#include "nssvascoreconstant.h"

// CLASS DECLARATION

/**
*  CNssRRD encapsulates result recognition data. 
*  It is the implementation class for MNssRRD. In addition to methods in MNssRRD, 
*  it also provides methods for internal use by VAS components.
*  @lib NssVASApi.lib
*  @since 2.8
*/

class CNssRRD: public  CBase, public MNssRRD
{
public:

    /**
    * C++ destructor.
    */
	~CNssRRD();

    /**
    * 2 phase construction
    */
	static CNssRRD* NewL(); 

    /**
    * 2 phase construction
    */
	static CNssRRD* NewLC();

    /**
    * Set the tag id for this context
	* @param aTagId tag id for the context
    * @return 
    */
	void SetTagId(TInt aTagId);

	 /**
    * Get the tag id for this context
    * @since 2.0
    * @param 
    * @return tag id
    */  
	TInt TagId();

	/**
    * Creates a deep copy of the RRD object and returns a pointer
	* to the newly created RRD. A client calling calling
	* this method takes the responsibility of deleting the new RRD object.
	* For internal use by by VAS components.
	* @since 2.0
    * @param 
    * @return pointer to newly created rrd
    */
	CNssRRD *CopyL();

public: // from MNssRRD

	/**
    * Get the text array for this RRD
	* @since 2.0
    * @param 
    * @return pointer to text array
    */  
	 CArrayFixFlat<NssRRDText>* TextArray();

	/**
    * Get the integer array this RRD
	* @since 2.0
    * @param 
    * @return pointer to integer array
    */  
	 CArrayFixFlat<TInt>* IntArray();

	/**
    * Set the text array for this RRD
	* @since 2.0
    * @param pointer to text array
    * @return 
    */  
	 void SetTextArrayL(CArrayFixFlat<NssRRDText>* aTextArray);

	/**
    * Set the integer array this RRD
	* @since 2.0
    * @param pointer to integer array
    * @return 
    */  
	 void SetIntArrayL(CArrayFixFlat<TInt>* aIntArray);

private:

	/**
    * C++ constructor.
    */
	CNssRRD();

	/**
    * EPOC constructor.
    */
	void ConstructL();

private:

	// text array
	CArrayFixFlat<NssRRDText>* iTextArray;

	// integer array
	CArrayFixFlat<TInt>*	iIntArray;

	// tag id, for internal use by VAS DB
	TInt					iTagId; 
};


#endif // NSSVASCRRD_H  
            
// End of File
