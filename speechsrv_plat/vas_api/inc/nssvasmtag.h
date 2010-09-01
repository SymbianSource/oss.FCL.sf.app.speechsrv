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
* Description:  MTag is an abstraction for a voice tag. MTag memebrs consist of 
*               a context, speech item, and RRD. 
*
*/


#ifndef NSSVASMTAG_H
#define NSSVASMTAG_H

// include 

#include <nssvasmcontext.h>
#include <nssvasmrrd.h>
#include <nssvasmspeechitem.h>

/**
* MTag is the interface class for tag processing.
* A voice tag consists of a context, a RRD, and a speech item. A context can have
* multiple voice tags. Context represents the category (application) to which the tag
* belongs. RRD represents the result data for a tag. Speech item represents the speech
* speech data and speech activities for a tag. A client will not create a tag diectly, 
* but use the methods in tag manager to get a tag. After getting a tag, the client can
* get the RRD and speech item objects using the following methods in MTag. A context needs
* to be created before creating a tag. After a tag is used, it needs to be deleted. Deleting
* a tag deletes its RRD and speech item, and thus they donot have to be deleted expicitly,
* @lib NssVASApi.lib
* @since 2.0
*/
class MNssTag
    {
    public:
    
        /**
        * Get the context 
    	* @since 2.0
        * @return context object to which this tag belongs
        */	
        virtual MNssContext* Context() = 0;

        /**
        * Get the RRD
    	* @since 2.0
        * @return RRD object for this tag
        */
    	virtual MNssRRD* RRD() = 0;

        /**
        * Get the speech item
    	* @since 2.0
        * @return speech item for this tag
        */
    	virtual MNssSpeechItem* SpeechItem() = 0;

        /**
        * virtual destructor for proper clean up
        */
    	virtual ~MNssTag(){}
    	
    };

#endif // NSSVASMTAG_H
            
// End of File
