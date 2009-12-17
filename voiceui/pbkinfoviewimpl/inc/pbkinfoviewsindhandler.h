/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used to fetch voice tags from vas db
*
*/


#ifndef PBKINFOVIEWSINDHANDLER_H
#define PBKINFOVIEWSINDHANDLER_H

// INCLUDES
#include <nssvasapi.h>
#include <nssttsutility.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class TPbkContactItemField;
class MVasBasePbkHandler;

// CONSTANTS
const TInt KMaxSegments = 6;

// CLASS DECLARATION

/**
 * Extension for Phonebook engine.
 */
class CPbkInfoViewSindHandler : public CBase,
                                public MNssGetContextClient,
                                public MNssGetTagClient,
                                public MNssPlayEventHandler,
                                public MTtsClientUtilityObserver
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of CPbkSindHandler.
         * @param aContactId Contact id for the contact whose voice tags are to
         *        be fetched from vas db.
         */
        static CPbkInfoViewSindHandler* NewL( TInt aContactId );
        /**
         * Creates an instance that doesn't require CCoeEnv for STIF testing
         * purposes.
         * @param aContactId Contact id for the contact whose voice tags are to
         *        be fetched from vas db.
         */
        static CPbkInfoViewSindHandler* TestNewL( TInt aContactId );
        /**
         * Destructor.
         */
        ~CPbkInfoViewSindHandler();
        /**
         * Returns the number of voice tags for the contact.
         * @return TInt voice tag count.
         */
         TInt VoiceTagCount();
         /**
         * Returns a voice tag.
         * @param aIndex index for the voice tag.
         *        0 <= aIndex <= VoiceTagCount() - 1
         * @return TDesC& the voice tag
         */
         const TDesC& VoiceTagL( TInt aIndex );
         
         /**
         * Returns the label associated with a voice tag (mobile, home, etc).
         * @param aIndex index for the voice tag.
         *        0 <= aIndex <= VoiceTagCount() - 1
         * @return HBufC* the label 
         */
         HBufC* VoiceTagLabelLC( TInt aIndex );
         /**
         * Returns the phone number, e-mail address, etc. associated with a
         * voice tag.
         * @param aIndex index for the voice tag.
         *        0 <= aIndex <= VoiceTagCount() - 1
         * @return TPtrC the value
         */
         TPtrC VoiceTagValueL( TInt aIndex );
         /**
         * Returns the popup text associated with a voice tag.
         * @param aIndex index for the voice tag.
         *        0 <= aIndex <= VoiceTagCount() - 1
         * @return TDesC& popup text
         */
         TDesC& PopupTextL( TInt aIndex );
         /**
         * Plays a voice tag.
         * @param aIndex index for the voice tag.
         *        0 <= aIndex <= VoiceTagCount() - 1
         */
         void PlayVoiceCommandL( TInt aIndex );
         /**
         * Cancels voice tag playback.
         * @return TBool boolean value
         */
         void CancelPlaybackL();
         /**
         * Is voice tag playing ongoing.
         * @return TBool boolean value
         */
         TBool IsPlaying();
         /**
         * Returns an icon id for the contact field the voice tag is associated
         * with.
         * @param aIndex index for the voice tag.
         *        0 <= aIndex <= VoiceTagCount() - 1
         * @return TInt the icon id
         */
         TInt IconIdL( TInt aIndex );
         
         /**
         * Calls CompareL and catches possible leaves
         */
         static TInt Compare( const MNssTag& aTag1, const MNssTag& aTag2 );
         
         /**
         * Compares two voice tags and decides which one should be displayed
         * first in info view list. Used in the sorting of voice tags.
         * @param aTag1 A voice tag
         * @param aTag2 A voice tag
         * @return TInt -1 if aTag1 should be displayed first, 1 if aTag2 should be 
                   displayed first, 0 otherwise.
         */
         static TInt CompareL( const MNssTag& aTag1, const MNssTag& aTag2 );
        
    public: // From MNssGetContextClient
    
        void GetContextCompleted( MNssContext* aContext, TInt aErrorCode );
        void GetContextListCompleted( MNssContextListArray* aContextList,
                                      TInt aErrorCode );
    
    public: // From MNssGetTagClient
    
        void GetTagListCompleted( MNssTagListArray* aTagList, TInt aErrorCode );
        
    public: // From MNssPlayEventHandler
        void HandlePlayComplete( TNssPlayResult aErrorCode );
        // Not implemented
        void HandlePlayStarted( TTimeIntervalMicroSeconds32 /*aDuration*/ ) {};

    public: // From MTtsClientUtilityObserver
        // Not implemented
        void MapcCustomCommandEvent( TInt /*aEvent*/, TInt /*aError*/ ) {};
        
        void MapcInitComplete(TInt /*aError*/, const TTimeIntervalMicroSeconds& /*aDuration*/) {};
        
        void MapcPlayComplete(TInt /*aError*/) {};
        
    private: // own methods:
        /**
        * Initialization.
        * @param aContactId Contact id for the contact whose voice tags are
        *         fetched from vas db.
        */
        void InitializeL( TInt aContactId );
        
        /**
        * Fetches voice tag list from vas db.
        * @param aContactId Contact id for the contact whose voice tags are
        *         fetched from vas db.
        */
        void CreateVoiceTagListL( TInt aContactId );
        
        /**
        * Creates an array for popups in info view list.
        */
        void CreatePopupArrayL();
        
        /**
        * Tries to find default contact item field.
        * @param aContactItem A contact item
        * @param aFieldId A field to search
        * @return TPbkContactItemField* Default field if found, otherwise NULL
        */
        static TPbkContactItemField* FindDefaultField( CPbkContactItem* aContactItem,
                                                       TInt aFieldId );

    private: // Implementation
        CPbkInfoViewSindHandler();
        void ConstructL( TInt aContactId );
        void TestConstructL( TInt aContactId );
        
    private: // Data
       
        // Vas databse manager
        CNssVASDBMgr* iVasDbManager;        
        // Nss context
        MNssContext* iContext;
        // Scheduler wait object
        CActiveSchedulerWait* iWait;
        // Context manager, does not take ownership
        MNssContextMgr* iContextManager;        
        // Tag manager, does not take ownership
        MNssTagMgr* iTagManager;
        // Error code received from observer call-back methods.
        TInt iErr;
        // Voice tag array
        RPointerArray<MNssTag> iTagArray;
        // Phonebook contact item
        CPbkContactItem* iPbkContactItem;
        // Phonebook engine
        CPbkContactEngine* iPbkEngine;
        // Phonebook contact item field for phone numbers etc, and icons.
        // Does not take ownership
        TPbkContactItemField* iPbkContactItemField;
        // Array for popups in info view list.
        RPointerArray<TDesC> iPopupArray;
        // Current voice tag form iTagArray being played
        TInt iPlaybackIndex;
         // For getting the phone numbers etc used in voice tags
        MVasBasePbkHandler* iPbkHandler;
        // TTS Utility
        CTtsUtility* iTts;

        //static TInt iContactId;
    };

#endif  // __PBINFOVIEWKSINDHANDLER_H__

// End of file
