/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The MNssAdaptationItem interface class handles speaker adaptation,
*               when the client wants to decide herself, when to adapt.
*
*/


#ifndef NSSVASMADAPTATIONITEM_H
#define NSSVASMADAPTATIONITEM_H

//  INCLUDES
#include <e32std.h>
#include <nssvasmadaptationeventhandler.h>
#include <nssvasmtag.h>

// CLASS DECLARATION

/**
*  MNssAdaptationItem event handler class.
*  Contains adaptation data.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
class MNssAdaptationItem
    {

    public:  //enum

	  enum TNssRejectTagError
        {
        EVasErrorNone,
        EVasRejectTagFailed,
        EVasNoTagInContexts,
        EVasRecognizeUndefinedError10,   // for place holder
        EVasRecognizeUndefinedError9,
        EVasRecognizeUndefinedError8,
        EVasRecognizeUndefinedError7,
        EVasRecognizeUndefinedError6,
        EVasRecognizeUndefinedError5,
        EVasRecognizeUndefinedError4,
        EVasRecognizeUndefinedError3,
        EVasRecognizeUndefinedError2,
        EVasRecognizeUndefinedError1
        };
	
    public: // New functions

        /**
        * AdaptL - Adapts the models using utterance and result, from which the ad
        *
        * @since 2.0
        * @param aHandler Event handler for this adaptation
        * @param aCorrect Index of correct result in the list of results
        */      		
        virtual void AdaptL( MNssAdaptationEventHandler *aHandler, TInt aCorrect ) = 0;

        /**
        * Adapts the models using the tag information
        * 
        * @since 3.2
        * @param aHandler Event handler for this adaptation
        * @param aTag Selected tag
        */ 
        virtual void AdaptL( MNssAdaptationEventHandler* aHandler,
                             MNssTag& aTag ) = 0;
                             
        /**
        * ~MNssAdaptationItem - Destructor  
        * @since 2.0
        */
        virtual ~MNssAdaptationItem() {};

    };

#endif // NSSVASMADAPTATIONITEM_H   
            
// End of File
