/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssTagSelectNotification class allows the client to
*                register and deregister for a voice tag select event. 
*
*/


#ifndef CNSSTAGSELECTNOTIFICATION_H
#define CNSSTAGSELECTNOTIFICATION_H

// INCLUDES

#include <e32std.h>
#include "nssvasmtagselectnotification.h"
#include "nssvasmdbtagselectnotifierclient.h"
#include "nssvasccontext.h"
#include "nssvascvasdbbuilder.h"

// FORWARD DECL
class CNssTag;
class CNssVASDBBuilder;
class MNssTagSelectHandler;
class CNssTagSelectData;


// CLASS DEFINITIONS

/**
*
*  The CNssTagSelectNotification class performs the recognition function.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/

class CNssTagSelectNotification : public CBase,
							   public MNssTagSelectNotification,
							   public MNssDBTagSelectNotifierClient
{

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */	
        static CNssTagSelectNotification* NewL();

		/**
        * Two-phased constructor.
        */	
		static CNssTagSelectNotification* NewLC();

        /**
        * Destructor.
        */	
		~CNssTagSelectNotification();

    public: // Functions from base classes

        /**
		* From MNssTagSelectNotification
        * Register for voice tag select notification
        * @since 2.0
        * @param aContext The voice tag context which determines notification.
        * @param aTagSelectHandler The event handler for tag select notification.
        * @return TSelectError Return code to indicate request was valid.
        */
        TNssSelectResult RegisterL(MNssContext* aContext,
					   MNssTagSelectHandler* aTagSelectHandler);

        /**
	    * From MNssTagSelectNotification
        * Deegister from voice tag select notification
        * @since 2.0
        * @param aContext The voice tag context which determines notification.
        * @param aTagSelectHandler The event handler for tag select notification,
		*                          used to deregister the correct client.
        * @return TSelectError Return code to indicate request was valid.
        */
        TNssSelectResult Deregister(MNssContext* aContext,
			            MNssTagSelectHandler* aTagSelectHandler);

        /**
	    * From MNssDBTagSelectNotifierClient
        * HandleSelection is called into the client by CNssDBTagSelectNotifier
        * @since 2.0
        * @param aTag  The voice tag which was selected.
        * @return -
        */
		void HandleSelection(CNssTag* aTag);

    private:

        /**
        * Constructor.
        */	
		CNssTagSelectNotification();

       /**
        * By default Symbian 2nd phase constructor is private.
        */		
        void ConstructL();

        /**
        * CompareContexts compare a context with a Tag Select Data List context
        * @since 2.0
        * @param aContext  The context of the voice tag which is selected.
		* @param aIndex The index of the Tag Select Data List entry to compare
		*               the aContext with
        * @return - ETrue if contexts match, EFalse if they do not compare
        */
		TBool CompareContexts(CNssContext* aContext, TInt aIndex);

	private:

		// Pointer to VAS Database Builder object.
		CNssVASDBBuilder* iVasDBBuilder;

		// Pointer to Database Tag Select Notifier object.
		CNssDBTagSelectNotifier* iDBTagSelectNotifier;

		// Pointer to Tag Select Data List object.
		CArrayPtrFlat<CNssTagSelectData>* iTagSelectDataList;

};

#endif // CNSSTAGSELECTNOTIFICATION_H

// End of File
