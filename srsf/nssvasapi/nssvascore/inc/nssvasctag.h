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
* Description:  CNssTag implements MNssTag. In addition, it provides members for use
*               by VAS internal components.
*
*/


#ifndef NSSVASCTAG_H
#define NSSVASCTAG_H

// includes
#include "nssvasmtag.h"
#include "nssvasccontext.h"
#include "nssvascrrd.h"
#include "nssvascspeechitem.h"
#include "nssvascoreconstant.h"

// CLASS DECLARATION

/**
*  CNssTag is the implementation class for MNssTag interface. 
*  It encapsulates tag data. A tag cannot be directly created by a client. 
*  A client has to own an instance of Tag Manager which has methods for creating 
*  tag objects. The CNssTag class also provides additional members for internal 
*  use by VAS.
*  @lib NssVASApi.lib
*  @since 2.8
*/

class CNssTag: public CBase, public MNssTag
{

public: // functions for internal VAS use
	
    /**
    * Two-phased constructor. Creates an empty tag object
    * for internal use.
    */
    static CNssTag* NewLC();
    
    /**
    * Two-phased constructor. Creates an empty tag object
    * for internal use.
    */
    static CNssTag* NewL();

public:   

    /**
    * C++ constructor.
	* @param aContext - pointer to a previously created context
	* @param aRRD - pointer to a previously created rrd
	* @param aSpeechItem - pointer to a previously created speech item
    * @return 
    */
	CNssTag(CNssContext* aContext, CNssRRD* aRRD, CNssSpeechItem* aSpeechItem);

    /**
    * Destructor.
	* @param 
    * @return 
    */
	 ~CNssTag();

    /**
    * Get the context for this tag.
	* @since 2.0
	* @param 
    * @return pointer to the context for this tag
    */
     MNssContext* Context();

    /**
    * Get the RRD for this tag.
	* @since 2.0
	* @param 
    * @return pointer to the RRD for this tag
    */
	 MNssRRD* RRD();

    /**
    * Get the Speech Item for this tag.
	* @since 2.0
	* @param 
    * @return pointer to the RRD for this tag
    */
	 MNssSpeechItem* SpeechItem();

	/**
    * Get the Tag Id for this tag.
	* @since 2.0
	* @param 
    * @return id for this tag
    */
	 TInt TagId();

	/**
    * Set the Tag Id for this tag.
	* @since 2.0
	* @param aId - id for this tag
    * @return 
    */
	 void SetTagId(TInt aId);

	/**
    * Creates a copy of the tag object and returns a pointer
	* to the newly created tag. CopyL creates a new rrd object and 
	* a new speech item for the new tag. CopyL uses the same context 
	* object from the source tag into the new tag. A client calling calling
	* this method takes the responsibility of deleting the new tag object.
	* For internal use by by VAS components.
	* @since 2.0
	* @param 
    * @return pointer to the tag object newly created 
    */
	 CNssTag *CopyL();
	 
private:
    /**
    * Constructor used in two-phased construction
    */
    CNssTag();
    
    /**
    * Two-phase construction
    */
	void ConstructL();

private:

	// context object
	CNssContext* iContext;
	
	// RRD object
	CNssRRD* iRRD;

	// speech item object
	CNssSpeechItem* iSpeechItem;

	// tag id, for internal use
	TInt iTagId;

};

#endif // NSSVASCTAG_H   
            
// End of File
