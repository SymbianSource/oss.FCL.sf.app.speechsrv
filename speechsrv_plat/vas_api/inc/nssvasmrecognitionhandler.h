/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The MNssRecognitionHandler class performs the recognition function.  
*
*/


#ifndef NSSVASMRECOGNITIONHANDLER_H
#define NSSVASMRECOGNITIONHANDLER_H

// INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <nssvasmtag.h>
#include <nssvasmcontext.h>
#include <nssvasmadaptationitem.h>
#include <nssvasmrecognizeeventhandler.h>
#include <nssvasmrecognizeinitcompletehandler.h>
#include <nssvasmrejecttagcompletehandler.h>
#include <nssvasmadaptationeventhandler.h>

// CLASS DEFINITIONS

/**
*
*  The MNssRecognitionHandler class performs the recognition function.
*  CNssRecognitionHandler class.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
class MNssRecognitionHandler
{

public:

        enum TNssRecognitionResult
        {
        EVasErrorNone,
        EVasRecognitionInitFailed,
        EVasRecognitionFailed,
        EVasSelectTagFailed,
        EVasRejectTagFailed,
        EVasUnexpectedRequest,
        EVasInvalidParameter
        };

        /**
        * Initiate the Select Tag event.
        * @since 2.0
        * @param aClientTag The voice tag which was selected. The tag is owned
        *                   by the client.
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        virtual TNssRecognitionResult SelectTagL( MNssTag* aClientTag ) = 0;

        /**
        * Initiate the Select Tag event.
        * @since 2.0
        * @param aClientTag The voice tag which was selected. The tag is owned
        *                   by the client.
        * @param aShouldNotifyHandlers If this is false, then handlers which
        *                              monitor tag's context won't be notified.
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        virtual TNssRecognitionResult SelectTagL(MNssTag* aClientTag, TBool aNotifyHandlers ) = 0;

        /**
        * Perform the recognize function.
        * @since 2.0
        * @param aEventHandler The event call back to receive recognize events.
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        virtual TNssRecognitionResult Recognize( MNssRecognizeEventHandler* aEventHandler ) = 0;

        /**
        * Perform initialization for the recognize funtion.
        * @since 2.0
        * @param aInitCompleteHandler The event call back to receive recognize
        *                             init events.
        * @param aMaxResults The maximum number of voice tags to be recognized.
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        virtual TNssRecognitionResult 
                RecognizeInitL( MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
                                TInt aMaxResults ) = 0;

        /**
        * Perform initialization for the recognize function.
        * @since 2.0
        * @param aInitCompleteHandler The event call back to receive recognize
        *                             init events.
        * @param aContextArray The list of contexts to be active in
        *                      recognition, i.e. recognition vocabulary.
        * @param aMaxResults The maximum number of voice tags to be recognized.
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        virtual TNssRecognitionResult RecognizeInitL(
                MNssRecognizeInitCompleteHandler* aInitCompleteHandler,
                const CArrayPtrFlat<MNssContext>& aContextArray,
                TInt aMaxResults ) = 0;

        /**
        * Starts sampling already before RecognizeInitL and Recognize calls.
        * This is used to ensure that there is no delay between user notification
        * and sampling start.
        * Asynchronous: MNssRecognizeInitCompleteHandler::HandlePreSamplingStarted
        * will be called when ready.
        *
        * @since 3.2
        * @param aInitCompleteHandler Callback handler        
        * @return Return value indicating if request was valid 
        */
        virtual TNssRecognitionResult PreStartSampling( MNssRecognizeInitCompleteHandler* aInitCompleteHandler ) = 0;

        /**
        * Cancel the last recognition function.
        * @since 2.0
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        virtual TNssRecognitionResult Cancel() = 0;

        /**
        * Request Reject Tag, i.e. blacklisting.
        * @since 2.0
        * @param aClientTagList The list of voice tags which is to be rejected, i.e. 
        *                       blacklisted. The tag list is owned by the client.
        * @return TNssRecognitionResult Return indicating request is valid.
        */
        virtual TNssRecognitionResult RejectTagL(
                MNssRejectTagCompleteHandler* aRejectTagCompleteHandler,
                CArrayPtrFlat<MNssTag>* aClientTagList ) = 0;        
        /**
        * Get adaptation item to allow later adaptation.
        * @since 2.0
        * @return MNssAdaptationItem* The adaptation item. Client takes ownership.
        */
        virtual MNssAdaptationItem *GetAdaptationItemL() = 0;
};

#endif // NSSVASMRECOGNITIONHANDLER_H

// End of File
