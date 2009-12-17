/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "pbkinfoviewapi.h"
#include "rubydebug.h"

#include <AiwMenu.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactIdConverter.h>
#include <MVPbkContactStoreListObserver.h>
#include <VPbkContactStoreUris.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>

#include <implementationproxy.h>

#include <data_caging_path_literals.hrh>  // for resource and bitmap directories

#include <aiwpbkinfoviewprovider.rsg>
#include "aiwpbkinfoviewprovider.h"
#include "aiwpbkinfoviewprovideruids.hrh"
#include "aiwpbkinfoviewprovider.rh"

_LIT( KResFileName, "z:aiwpbkinfoviewprovider.rsc" );
const TInt32 KVoiceTaglessContactId = -2;

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::NewL
//
// -----------------------------------------------------------------------------
//
CAiwPbkInfoViewProvider* CAiwPbkInfoViewProvider::NewL()
	{
	CAiwPbkInfoViewProvider* self = new (ELeave) CAiwPbkInfoViewProvider();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
	return self;
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::Constructor
//
// -----------------------------------------------------------------------------
//
CAiwPbkInfoViewProvider::CAiwPbkInfoViewProvider()
	{
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::Destructor
//
// -----------------------------------------------------------------------------
//
CAiwPbkInfoViewProvider::~CAiwPbkInfoViewProvider()
	{
	delete iInfoViewApi;
	iInfoViewApi = NULL;
	delete iWait;
	iWait = NULL;
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::ConstructL
//
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::ConstructL()
    {
	iInfoViewApi = CPbkInfoViewApi::NewL();
	iWait = new (ELeave) CActiveSchedulerWait();
    } 

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::InitialiseL (from CAiwServiceIfBase)
//
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::InitialiseL( 
                                  MAiwNotifyCallback& /*aFrameworkCallback*/,
	                              const RCriteriaArray& /*aInterest*/ )
	{
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::HandleServiceCmdL (from CAiwServiceIfBase)
//
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::HandleServiceCmdL(
                                  const TInt& /*aCmdId*/,
                                  const CAiwGenericParamList& /*aInParamList*/,
                                  CAiwGenericParamList& /*aOutParamList*/,
                                  TUint /*aCmdOptions*/,
                                  const MAiwNotifyCallback* /*aCallback*/ )
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::InitializeMenuPaneL (from CAiwServiceIfMenu)
// Initialises menu pane by adding provider specific menu items. The AIW
// framework gives the parameters to be used in addition.
// @param aMenuPane Menu pane handle
// @param aIndex position of item where to add menu items.
// @param aCascadeId ID of cascade menu item.
// @param aInParamList input parameter list for provider's parameters checking
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::InitializeMenuPaneL(
                                  CAiwMenuPane& aMenuPane,
			                      TInt aIndex,
								  TInt /*aCascadeId*/,
                                  const CAiwGenericParamList& aInParamList )
	{
	TFileName resFile;
	GetResFileName( resFile );

    TInt serviceCommand = GetServiceCommandId( aInParamList );
    
    if ( serviceCommand == KErrNotFound )
        {
        User::Leave( KErrArgument );
    	}
       	
    if ( serviceCommand == KAiwCmdSindInfoView )
    	{
        aMenuPane.AddMenuItemsL( resFile, R_PBKINFOVIEWPROVIDER_SUBMENU,
    	                         KAiwCmdSindInfoView, aIndex );
    	}
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::HandleMenuCmdL (from CAiwServiceIfMenu)
// Handles a menu command invoked by the Handler.
// @param aMenuCmdId Command ID for the menu command,
//        defined by the provider when adding the menu commands.
// @param aInParamList Input parameters, could be empty list
// @param aOutParamList Output parameters, could be empty list
// @param aCmdOptions Options for the command, see KAiwCmdOpt* constants.
// @param aCallback callback if asynchronous command handling is wanted by consumer.
//        The provider may or may not support this, leaves with KErrNotSupported
//        if not.
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::HandleMenuCmdL( 
                                  TInt aMenuCmdId, 
                                  const CAiwGenericParamList& aInParamList,
                                  CAiwGenericParamList& /*aOutParamList*/,
                                  TUint /*aCmdOptions*/,
                                  const MAiwNotifyCallback* /*aCallback*/ )
	{
	RUBY_DEBUG_BLOCK( "CAiwPbkInfoViewProvider::HandleMenuCmdL" );
	
	if( aMenuCmdId == EPbkInfoViewProviderCmd )
		{
		TInt contactId = GetContactIdL( aInParamList );
		
        iInfoViewApi->ShowInfoViewL( contactId );
		}
	}
	

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::OpenComplete (from MVPbkContactStoreObserver)
//
// -----------------------------------------------------------------------------
//	
void CAiwPbkInfoViewProvider::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {
    }

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::OpenComplete (from MVPbkContactStoreObserver)
//
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
    {
    }

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::OpenComplete (from MVPbkContactStoreObserver)
//
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    }
// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::OpenComplete (from MVPbkContactStoreListObserver)
//
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::OpenComplete()
	{
    // Continue with GetContactIdL-method
    if ( iWait && iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::GetContactIdL
// Gets the contact id from AIW parameter list.
// @param aInParamList AIW parameter list.
// @return Contact id.
// -----------------------------------------------------------------------------
//
TInt CAiwPbkInfoViewProvider::GetContactIdL(
                                  const CAiwGenericParamList& aInParamList )
	{
	RUBY_DEBUG_BLOCK( "CAiwPbkInfoViewProvider::GetContactIdL" );
	
	TInt index( 0 );
	TInt32 contactId( KVoiceTaglessContactId );
	
    const TAiwGenericParam* param
        = aInParamList.FindFirst( index, EGenericParamContactLinkArray );
        
    if( param )
        {
        // Get the externalized contact link array
        TPtrC8 packedLinkArray = param->Value().AsData();
        
        CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();

        uriArray->AppendL( TVPbkContactStoreUriPtr( VPbkContactStoreUris::DefaultCntDbUri() ) );

        CVPbkContactManager* contactManager = CVPbkContactManager::NewL( *uriArray );
        CleanupStack::PushL( contactManager );
        
        // Open contact stores
        MVPbkContactStoreList& storeList = contactManager->ContactStoresL();
        storeList.OpenAllL( *this );
        
        // Let's synchronize the OpenAllL call
        if ( iWait && !iWait->IsStarted() )
            {
            iWait->Start();
            }
        
        // Internalize contact link array
        CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC(
            packedLinkArray, contactManager->ContactStoresL() );
        
        if( links->Count() > 0 )
            {
            // There should only be one link in the array
            const MVPbkContactLink& link = links->At( 0 );
            
            MVPbkContactStore& store = link.ContactStore();
            
            // Get the contact id    
            CVPbkContactIdConverter* idConverter
                = CVPbkContactIdConverter::NewL( store );
            CleanupStack::PushL( idConverter );
            
            contactId = idConverter->LinkToIdentifier( link ); 
            
            CleanupStack::PopAndDestroy( idConverter );  
            }

        CleanupStack::PopAndDestroy( links );
        CleanupStack::PopAndDestroy( contactManager );
        CleanupStack::PopAndDestroy( uriArray );
        }

    RUBY_DEBUG1( "Contact id: %d", contactId );
	return contactId;
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::GetServiceCommandId
// Gets service command id from AIW parameter list.
// @param aInParamList AIW parameter list.
// @return TInt Service command id.
// -----------------------------------------------------------------------------
//
TInt CAiwPbkInfoViewProvider::GetServiceCommandId(
                                  const CAiwGenericParamList& aInParamList)
	{
	TInt index( 0 );
	TInt32 num( 0 );
	
	const TAiwGenericParam* param
	    = aInParamList.FindFirst( index, EGenericParamServiceCommand );

	const TAiwVariant& val = param->Value();
	if ( !val.Get( num ) )
		{
		num = KErrNotFound;
		}
	return num;
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::GetResFileName
// Gets this plugin resource file name.
// @param aText Resource file name.
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::GetResFileName( TDes& aText )
	{
	TParse parse;
	parse.Set( KResFileName, &KDC_RESOURCE_FILES_DIR, NULL );
	StrCopy( aText, parse.FullName() );
	}

// -----------------------------------------------------------------------------
// CAiwPbkInfoViewProvider::StrCopy
//
// String copy with lenght check.
// @param TDes& aTarget Copied text will be put to this variable.
// @param TDesC8& aSource Includes the text to be copied.
// -----------------------------------------------------------------------------
//
void CAiwPbkInfoViewProvider::StrCopy( TDes& aTarget, const TDesC& aSource )
    {
	TInt len = aTarget.MaxLength();
    if( len < aSource.Length() ) 
	    {
		aTarget.Copy( aSource.Left( len ) );
		return;
	    }
	aTarget.Copy( aSource );
    }

//
// Rest of the file is for ECom initialization. 
//

// -----------------------------------------------------------------------------
// Map the interface UIDs to implementation factory functions
//
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KAspInfoViewProviderImplUid,
                                CAiwPbkInfoViewProvider::NewL )
    };

// -----------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
}

//
// DLL entry point
//
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}
#endif

// End of file
