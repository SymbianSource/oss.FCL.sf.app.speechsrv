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
* Description:  CNssDBTagSelectNotifier provides tag select notification to 
*               its clients (observers). It itslef receives tag select notification
*               from Recognition Handler.
*
*/


#ifndef NSSVASCDBTAGSELECTNOTIFIER_H
#define NSSVASCDBTAGSELECTNOTIFIER_H

#include <e32base.h>
#include "nssvascvasdatabase.h"
#include "nssvasmdbtagselectnotifierclient.h"


// CLASS DECLARATIONS
/**
*  CNssDBTagSelectNotifier provides tag select notification to its clients.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssDBTagSelectNotifier : public CActive
{
public:

   /**
   * 2 Phase construction
   * @param aVasDatabase - a pointer to the datatbase
   * @return a pointer to CNssDBTagSelectNotifier
   */
	static CNssDBTagSelectNotifier* NewL( CNssVASDatabase* aVasDatabase );
   
   /**
   * 2 Phase construction
   * @param aVasDatabase - a pointer to the datatbase
   * @return a pointer to CNssDBTagSelectNotifier
   */
	static CNssDBTagSelectNotifier* NewLC( CNssVASDatabase* aVasDatabase );
   
	/**
   * Destructor.
   */	
	~CNssDBTagSelectNotifier();
   
   /**
   * Register a client (observer) for tag select notification
   * @since 2.0
   * @param aObserver - MNssDBTagSelectNotifierClient 
   * @return error code
   */
   TInt RegisterForNotification( MNssDBTagSelectNotifierClient* aObserver );

   /**
   * Deregister a client (observer) from tag select notification
   * @since 2.0
   * @param aObserver - MNssDBTagSelectNotifierClient 
   * @return error code
   */
   TInt Deregister( MNssDBTagSelectNotifierClient* aObserver );

   /**
   * Handle selection, called by regconition when tag is selected
   * @since 2.0
   * @param None
   * @return None
   */
	void HandleSelection( CNssTag* aTag );

private:

    /**
    * C++ constructor
    */
    CNssDBTagSelectNotifier();

    /**
    * Overloaded C++constructor
    */
    CNssDBTagSelectNotifier( CNssVASDatabase* aVasDatabase );

    /**
    * Symbian construtor
    */
    void ConstructL ();

    /**
    * Handles selection of a tag based on tag and context ids
    *
    * @param "TInt aTagId" Tag identifier
    * @param "TInt aContextId" Context identifier
    */
    void DoHandleSelectionCallback( TInt aTagId, TInt aContextId );

private: // From CActive

    void RunL();
    void DoCancel();

private: 
	// pointer to the VAS datatbase
	CNssVASDatabase* iVasDatabase;

	// list of observers (clients)
	CArrayPtrFlat<MNssDBTagSelectNotifierClient> *iObserverList;
	
	// P&S handle
	RProperty iProperty;
	
	// Values read from P&S
	TInt iTagId;
	TInt iContextId;
};

#endif // NSSVASCDBTAGSELECTNOTIFIER_H

// End of file
