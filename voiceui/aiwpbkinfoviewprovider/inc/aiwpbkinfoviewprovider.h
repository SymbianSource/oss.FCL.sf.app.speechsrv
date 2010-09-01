/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AIW provider for pbkinfoview.
*
*/


#ifndef AIWPBKINFOVIEWPROVIDER_H
#define AIWPBKINFOVIEWPROVIDER_H

//  INCLUDES
#include <AiwServiceIfMenu.h>

// CONSTANTS
const TInt KBufSize = 128;
const TInt KSyncApplicationId = 0;

const TInt KAiwCmdSindInfoView =  0x10281C83;


// CLASS DECLARATION
class CPbkInfoViewApi;
class MVPbkContactStoreListObserver;

/**
* CAiwPbkInfoViewProvider
*  
* CAiwPbkInfoViewProvider implements CAiwServiceIfMenu interface to
* allow synchronize functionality to be called via AIW.
*
* @lib syncservice.lib
* @since Series 60 3.0
*/
class CAiwPbkInfoViewProvider : public CAiwServiceIfMenu,
                                public MVPbkContactStoreListObserver
	{
	public:
        /**
        * Two-phased constructor.
        */
		static CAiwPbkInfoViewProvider* NewL();

        /**
        * Destructor.
        */
		~CAiwPbkInfoViewProvider();

	public:		
        /**
        * From the CAiwServiceIfMenu.
		* Not implemented.
        */
		void InitialiseL(MAiwNotifyCallback& aFrameworkCallback,
			                     const RCriteriaArray& aInterest);

        /**
        * From the base class.
		* Not implemented.
        */
		void HandleServiceCmdL(const TInt& aCmdId,
                                       const CAiwGenericParamList& aInParamList,
                                       CAiwGenericParamList& aOutParamList,
                                       TUint aCmdOptions = 0,
                                       const MAiwNotifyCallback* aCallback = NULL);

     	 /**
         * Initialises menu pane by adding provider specific menu items.
	     * The AIW Framework gives the parameters to be used in addition.
	     * @param aMenuPane Menu pane handle
	     * @param aIndex position of item where to add menu items.
	     * @param aCascadeId ID of cascade menu item.
	     * @param aInParamList input parameter list for provider's parameters checking
	     */
		void InitializeMenuPaneL(CAiwMenuPane& aMenuPane,
                                         TInt aIndex,
                                         TInt aCascadeId,
                                         const CAiwGenericParamList& aInParamList);

        /**
        * Handles a menu command invoked by the Handler.
        * @param aMenuCmdId Command ID for the menu command,
        *        defined by the provider when adding the menu commands.
        * @param aInParamList Input parameters, could be empty list
        * @param aOutParamList Output parameters, could be empty list
        * @param aCmdOptions Options for the command, see KAiwCmdOpt* constants.
        * @param aCallback callback if asynchronous command handling is wanted by consumer.
        *    The provider may or may not support this, leaves with KErrNotSupported if not.
        */
		void HandleMenuCmdL(TInt aMenuCmdId,
                                    const CAiwGenericParamList& aInParamList,
                                    CAiwGenericParamList& aOutParamList,
                                    TUint aCmdOptions = 0,
                                    const MAiwNotifyCallback* aCallback = NULL);
                                    
    private:
        /*
        * From MVPbkContactStoreListObserver
        */
        void StoreReady( MVPbkContactStore& aContactStore );
        void StoreUnavailable( MVPbkContactStore& aContactStore,
                               TInt aReason );
        void HandleStoreEventL( MVPbkContactStore& aContactStore, 
                                TVPbkContactStoreEvent aStoreEvent );
        void OpenComplete();

	private:
        /**
        * C++ default constructor.
        */
		CAiwPbkInfoViewProvider();

        /**
        * By default Symbian OS constructor is private.
        */
		void ConstructL();
	
	private:
        /**
		* Gets service command id from AIW parameter list.
        * @param aInParamList AIW parameter list.
		* @return TInt Service command id.
        */
		TInt GetServiceCommandId( const CAiwGenericParamList& aInParamList );

        /**
		* Gets the contact id from AIW parameter list.
        * @param aInParamList AIW parameter list.
		* @return TInt id.
        */
	    TInt GetContactIdL( const CAiwGenericParamList& aInParamList );
        /**
		* Gets this plugin resource file name.
        * @param aText Resource file name.
		* @return None.
        */
		void GetResFileName( TDes& aText );
		
        /**
		* Copies strings.
        * @param aTarget Copied text will be put to this variable.
        * @param aSource Includes the text to be copied.
		* @return None.
        */
    	void StrCopy( TDes& aTarget, const TDesC& aSource );

	private:
		CPbkInfoViewApi* iInfoViewApi;
		// Scheduler wait object
        CActiveSchedulerWait* iWait;
		// Error code received from observer call-back methods.
        TInt iErr;
	};	

#endif  // AIWPBKINFOVIEWPROVIDER_H

// End of file
