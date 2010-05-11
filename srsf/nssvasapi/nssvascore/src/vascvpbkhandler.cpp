/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


// INCLUDE FILES
#include <coemain.h> 
#include <centralrepository.h>
#include <featmgr.h>

#include <PbkFields.hrh>

#include <VPbkEng.rsg>

#include <CVPbkContactManager.h> 
#include <CVPbkContactStoreUriArray.h> 
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h> 
#include <MVPbkContactStoreList.h> 
#include <CVPbkContactIdConverter.h> 
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <CVPbkDefaultAttribute.h>
#include <MVPbkContactAttributeManager.h>
#include <MVPbkContactFieldTextData.h> 
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactViewBase.h> 
#include <CVPbkContactViewDefinition.h>

#include <nssvasmtag.h>
#include <nssvascoreconstant.h>

#include "vascvpbkhandler.h"
#include "vasmcontactobserver.h"

#include "srsfprivatecrkeys.h"

#include "rubydebug.h"

// CONSTANTS

// Priority list of virtual phonebook fields
const TInt KFieldPriority[] = 
    { 
    R_VPBK_FIELD_TYPE_LANDPHONEGEN, 
    R_VPBK_FIELD_TYPE_LANDPHONEHOME,
    R_VPBK_FIELD_TYPE_LANDPHONEWORK, 
    R_VPBK_FIELD_TYPE_MOBILEPHONEGEN,
    R_VPBK_FIELD_TYPE_MOBILEPHONEHOME, 
    R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
    R_VPBK_FIELD_TYPE_VIDEONUMBERGEN,
    R_VPBK_FIELD_TYPE_VIDEONUMBERHOME, 
    R_VPBK_FIELD_TYPE_VIDEONUMBERWORK,
    R_VPBK_FIELD_TYPE_FAXNUMBERGEN,
    R_VPBK_FIELD_TYPE_FAXNUMBERHOME, 
    R_VPBK_FIELD_TYPE_FAXNUMBERWORK,
    R_VPBK_FIELD_TYPE_VOIPGEN,
    R_VPBK_FIELD_TYPE_VOIPHOME, 
    R_VPBK_FIELD_TYPE_VOIPWORK,
    R_VPBK_FIELD_TYPE_IMPP,
    R_VPBK_FIELD_TYPE_EMAILGEN,
    R_VPBK_FIELD_TYPE_EMAILHOME, 
    R_VPBK_FIELD_TYPE_EMAILWORK,
    R_VPBK_FIELD_TYPE_FIRSTNAMEREADING,
    R_VPBK_FIELD_TYPE_FIRSTNAME,
    R_VPBK_FIELD_TYPE_LASTNAMEREADING,
    R_VPBK_FIELD_TYPE_LASTNAME,
    R_VPBK_FIELD_TYPE_COMPANYNAME,
    R_VPBK_FIELD_TYPE_SECONDNAME
    };
const TInt KFieldPriorityCount = sizeof( KFieldPriority ) / sizeof ( TInt );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVasVPbkHandler* CVasVPbkHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CVasVPbkHandler* CVasVPbkHandler::NewL()
    {
    CVasVPbkHandler* self = new (ELeave) CVasVPbkHandler;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CVasVPbkHandler::~CVasVPbkHandler()
    {
    RUBY_DEBUG0( "CVasVPbkHandler::~CVasVPbkHandler START" );
    
    iObserver = NULL;
    
    delete iField;
    
    delete iContact;
    
    delete iOperation;
    
    delete iConverter;
    
    if ( iContactManager )
        {
        TRAP_IGNORE( iContactManager->ContactStoresL().CloseAll( *this ) );
        }
    delete iContactManager;
    
    RUBY_DEBUG0( "CVasVPbkHandler::~CVasVPbkHandler EXIT" );
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::InitializeL
// Initializes phonebook handles
// -----------------------------------------------------------------------------
//
EXPORT_C void CVasVPbkHandler::InitializeL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::InitializeL" );
    
    __ASSERT_ALWAYS( !iContactManager, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( !iConverter, User::Leave( KErrAlreadyExists ) );
    
    FeatureManager::InitializeLibL();

    iMMSSupported = FeatureManager::FeatureSupported( KFeatureIdMMS );

    FeatureManager::UnInitializeLib();

    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
    uriArray->AppendL( TVPbkContactStoreUriPtr( VPbkContactStoreUris::DefaultCntDbUri() ) );
    
    iContactManager = CVPbkContactManager::NewL( *uriArray, NULL );
    CleanupStack::PopAndDestroy( uriArray );

    MVPbkContactStoreList& storeList = iContactManager->ContactStoresL();
    storeList.OpenAllL( *this );
    
    // Synchronize the OpenAllL call
    iWait.Start();
    
    User::LeaveIfError( iError );
    
    iConverter = CVPbkContactIdConverter::NewL( iContactManager->ContactStoresL().At( 0 ) );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::CreateContactObserverL
// Creates the contact change notifier and sets observer as a listener
// -----------------------------------------------------------------------------
//
EXPORT_C void CVasVPbkHandler::CreateContactObserverL( MVasContactObserver* aObserver )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::CreateContactObserverL" );
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::CompressL
// Compresses database
// -----------------------------------------------------------------------------
//
EXPORT_C void CVasVPbkHandler::CompressL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::CompressL" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    iOperation = iContactManager->CompressStoresL( *this );
    
    iWait.Start();
    
    User::LeaveIfError( iError );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::CompressL
// Returns all contact ids from database
// -----------------------------------------------------------------------------
//
EXPORT_C CContactIdArray* CVasVPbkHandler::ContactIdArrayLC()
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactIdArrayLC START" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    CContactIdArray* array = CContactIdArray::NewLC();
    
    CVPbkContactViewDefinition* definition = CVPbkContactViewDefinition::NewL();
    CleanupStack::PushL( definition );
    
    definition->SetType( EVPbkContactsView );
    definition->SetUriL( VPbkContactStoreUris::DefaultCntDbUri() );
    
    MVPbkContactViewBase* view =
        iContactManager->CreateContactViewLC( *this, *definition, iContactManager->FieldTypes() );

    iWait.Start();
    
    User::LeaveIfError( iError );

    for( int i = 0; i < view->ContactCountL(); ++i )
        {
        MVPbkContactLink* link = view->CreateLinkLC( i );        
        
        if ( link )
            {
            array->AddL( iConverter->LinkToIdentifier( *link ) );
        
            CleanupStack::PopAndDestroy(); // Destroy link
            }
        }
        
    CleanupStack::PopAndDestroy(); // Destroy view
    CleanupStack::PopAndDestroy( definition );
        
    RUBY_DEBUG0( "CVasVPbkHandler::ContactIdArrayLC EXIT" );
    
    return array;
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::FindContactL
// Finds a contact from the phonebook that matches given tag
// -----------------------------------------------------------------------------
//    
EXPORT_C void CVasVPbkHandler::FindContactL( MNssTag* aTag )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FindContactL" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    delete iField;
    iField = NULL;
    
    delete iContact;
    iContact = NULL;
        
    MVPbkContactLink* link =
        iConverter->IdentifierToLinkLC( aTag->RRD()->IntArray()->At( KVasContactIdRrdLocation ) );
       
    iOperation = iContactManager->RetrieveContactL( *link, *this );
    
    iWait.Start();
    
    // If contact was not found, then request for immediate contact db resync
    if ( iError == KErrNotFound )
        {
        CRepository* repository = CRepository::NewLC( KCRUidSRSFSettings );
        
        repository->Set( KSRSFFullResyncNeeded, KImmediateResync );
        
        CleanupStack::PopAndDestroy( repository );
        }
    
    User::LeaveIfError( iError );
    
    CleanupStack::PopAndDestroy();  // Destroys the link
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FindContactL
// Finds a contact from the phonebook that matches given contact id
// -----------------------------------------------------------------------------
//    
EXPORT_C void CVasVPbkHandler::FindContactL( TInt32 aContactId, TBool aReadOnly )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FindContactL" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    delete iField;
    iField = NULL;
    
    delete iContact;
    iContact = NULL;
        
    MVPbkContactLink* link = iConverter->IdentifierToLinkLC( aContactId );
       
    iOperation = iContactManager->RetrieveContactL( *link, *this );
    
    iWait.Start();
    
    User::LeaveIfError( iError );
    
    if ( !aReadOnly )
        {
        iContact->LockL( *this );
        
        iWait.Start();
        
        User::LeaveIfError( iError );
        }
    
    CleanupStack::PopAndDestroy();  // Destroys the link
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::CloseContactL
// Closes contact with or without saving changes
// -----------------------------------------------------------------------------
//  
EXPORT_C void CVasVPbkHandler::CloseContactL( TBool aSaveChanges )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::CloseContactL" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( iContact, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    if ( aSaveChanges )
        {
        iContact->CommitL( *this );
        
        iWait.Start();
    
        User::LeaveIfError( iError );
        }
    
    delete iContact;
    iContact = NULL;
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FindFieldL
// Finds a contact field that matches given tag
// -----------------------------------------------------------------------------
//    
EXPORT_C void CVasVPbkHandler::FindFieldL( MNssTag* aTag )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FindFieldL" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( iContact, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    delete iField;
    iField = NULL;
    
    TRAPD( error,
        FindDefaultContactFieldL( aTag->RRD()->IntArray()->At( KVasExtensionCommandRrdLocation ) ) );
    if ( error == KErrNotFound )
        {
        FindFieldL( aTag->RRD()->IntArray()->At( KVasFieldIdRrdLocation ) );
        }
        
    if ( !iField )
        {
        User::Leave( KErrNotFound );
        }

    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FindFieldL
// Finds a contact field that matches given field id
// -----------------------------------------------------------------------------
//    
EXPORT_C void CVasVPbkHandler::FindFieldL( TInt aFieldId )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FindFieldL" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( iContact, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    delete iField;
    iField = NULL;
    
    MVPbkStoreContactFieldCollection& fields = iContact->Fields();

    TInt priority = KFieldPriorityCount;
    for ( TInt index = 0; index < fields.FieldCount(); ++index )
        {
        if ( ConvertFieldId( fields.FieldAt( index ) ) == aFieldId &&
             FieldPriority( fields.FieldAt( index ) ) < priority )
            {
            priority = FieldPriority( fields.FieldAt( index ) );
            
            delete iField;
            iField = NULL;
                            
            iField = fields.FieldAtLC( index );
            CleanupStack::Pop(); // Pops iField
            }
        }
        
    if ( !iField )
        {
        User::Leave( KErrNotFound );
        }
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FindContactFieldL
// Finds a contact and a field that mathes given tag
// -----------------------------------------------------------------------------
//    
EXPORT_C void CVasVPbkHandler::FindContactFieldL( MNssTag* aTag )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FindContactFieldL" );
    
    FindContactL( aTag );
    FindFieldL( aTag );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FindDefaultContactFieldL
// Tries to find a default field of given type from found contact
// -----------------------------------------------------------------------------
//
EXPORT_C void CVasVPbkHandler::FindDefaultContactFieldL( TInt aCommandType )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FindDefaultContactFieldL" );
    
    __ASSERT_ALWAYS( iContactManager, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( iContact, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    TInt found = KErrNotFound;
    
    TVPbkDefaultType type = SelectDefaultType( aCommandType );
    
    if ( type != EVPbkDefaultTypeUndefined )
        {
        MVPbkContactAttributeManager& attributeManager = iContactManager->ContactAttributeManagerL();
    
        CVPbkDefaultAttribute* attribute = CVPbkDefaultAttribute::NewL( type );
        CleanupStack::PushL( attribute );
        
        MVPbkStoreContactField* field =
            attributeManager.FindFieldWithAttributeL( *attribute, *iContact );

        if ( field && AcceptDefaultField( *field, aCommandType ) )
            {
            iField = field->CloneLC();
            CleanupStack::Pop(); // Pops iField
            found = KErrNone;
            }
        
        CleanupStack::PopAndDestroy( attribute );
        }
    
    User::LeaveIfError( found );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ChangeVoiceTagFieldL
// Changes current field's voice tag field status
// -----------------------------------------------------------------------------
//
EXPORT_C void CVasVPbkHandler::ChangeVoiceTagFieldL( TBool /*aSet*/ )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::ChangeVoiceTagFieldL" );
    
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::LabelL
// Returns the field label
// -----------------------------------------------------------------------------
//    
EXPORT_C TPtrC CVasVPbkHandler::LabelL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::LabelL" );
    
    __ASSERT_ALWAYS( iField, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    if ( iField->BestMatchingFieldType()->FieldTypeResId() == R_VPBK_FIELD_TYPE_IMPP && 
         iField->FieldData().DataType() == EVPbkFieldStorageTypeUri )
        {
        return MVPbkContactFieldUriData::Cast( iField->FieldData()).Scheme();
        }

    return iField->FieldLabel();
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::TextL
// Returns the field text
// -----------------------------------------------------------------------------
//    
EXPORT_C TPtrC CVasVPbkHandler::TextL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::TextL" );
    
    __ASSERT_ALWAYS( iField, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    if ( iField->FieldData().DataType() == EVPbkFieldStorageTypeText )
        {
        return MVPbkContactFieldTextData::Cast( iField->FieldData() ).Text();
        }
    else if ( iField->FieldData().DataType() == EVPbkFieldStorageTypeUri )
        {
        return MVPbkContactFieldUriData::Cast( iField->FieldData() ).Text();
        }
    else
        {
        return KNullDesC();
        }
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FieldTypeL
// Returns field type as an UID
// -----------------------------------------------------------------------------
//    
EXPORT_C TFieldType CVasVPbkHandler::FieldTypeL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FieldTypeL" );
    
    __ASSERT_ALWAYS( iField, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    TFieldType uid = KNullUid;
    
    switch( iField->BestMatchingFieldType()->FieldTypeResId() )
        {
        case R_VPBK_FIELD_TYPE_LANDPHONEHOME:
        case R_VPBK_FIELD_TYPE_LANDPHONEWORK:
        case R_VPBK_FIELD_TYPE_LANDPHONEGEN:
            {
            uid = KUidContactFieldVCardMapVOICE;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_MOBILEPHONEHOME:
        case R_VPBK_FIELD_TYPE_MOBILEPHONEWORK:
        case R_VPBK_FIELD_TYPE_MOBILEPHONEGEN:
            {
            uid = KUidContactFieldVCardMapCELL;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_VIDEONUMBERHOME:
        case R_VPBK_FIELD_TYPE_VIDEONUMBERWORK:
        case R_VPBK_FIELD_TYPE_VIDEONUMBERGEN:
            {
            uid = KUidContactFieldVCardMapVIDEO;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_FAXNUMBERHOME:
        case R_VPBK_FIELD_TYPE_FAXNUMBERWORK:
        case R_VPBK_FIELD_TYPE_FAXNUMBERGEN:
            {
            uid = KUidContactFieldVCardMapFAX;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_VOIPHOME:
        case R_VPBK_FIELD_TYPE_VOIPWORK:
        case R_VPBK_FIELD_TYPE_VOIPGEN:
        case R_VPBK_FIELD_TYPE_IMPP:
            {
            uid = KUidContactFieldVCardMapVOIP;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_EMAILHOME:
        case R_VPBK_FIELD_TYPE_EMAILWORK:
        case R_VPBK_FIELD_TYPE_EMAILGEN:
            {
            uid = KUidContactFieldVCardMapEMAILINTERNET;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_FIRSTNAME:
            {
            uid = KUidContactFieldGivenName;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_FIRSTNAMEREADING:
            {
            uid = KUidContactFieldGivenNamePronunciation;
            break;
            } 
            
        case R_VPBK_FIELD_TYPE_LASTNAME:
            {
            uid = KUidContactFieldFamilyName;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_LASTNAMEREADING:
            {
            uid = KUidContactFieldFamilyNamePronunciation;
            break;
            }
            
        case R_VPBK_FIELD_TYPE_COMPANYNAME:
            {
            uid = KUidContactFieldCompanyName;
            break;
            }            
            
        case R_VPBK_FIELD_TYPE_SECONDNAME:
            {
            uid = KUidContactFieldSecondName;
            break;
            }
        }
    
    return uid;
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FieldIdL
// Returns field id as a phonebook id
// -----------------------------------------------------------------------------
//    
EXPORT_C TInt CVasVPbkHandler::FieldIdL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::FieldIdL" );
    
    __ASSERT_ALWAYS( iField, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    return ConvertFieldId( *iField );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::IsFieldEmptyL
// Returns information if current field is empty
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CVasVPbkHandler::IsFieldEmptyL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::IsFieldEmptyL" );
    
    __ASSERT_ALWAYS( iField, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( !iOperation, User::Leave( KErrArgument ) );
    
    return iField->FieldData().IsEmpty();
    }    
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::StoreReady
// -----------------------------------------------------------------------------
//
void CVasVPbkHandler::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::StoreReady START" );

    iError = KErrNone;
    
    RUBY_DEBUG0( "CVasVPbkHandler::StoreReady EXIT" );
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::StoreUnavailable
// -----------------------------------------------------------------------------
//
void CVasVPbkHandler::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, TInt aReason )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::StoreUnavailable START" );

    iError = aReason;
    
    RUBY_DEBUG0( "CVasVPbkHandler::StoreUnavailable EXIT" );
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::HandleStoreEventL
// -----------------------------------------------------------------------------
//
void CVasVPbkHandler::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
                                         TVPbkContactStoreEvent aStoreEvent )
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::HandleStoreEventL" );
    
    if ( iObserver )
        {
        TPhonebookEvent event;
        event.iType = ConvertEvent( aStoreEvent );
        
        if ( aStoreEvent.iContactLink )
            {
            event.iContactId = iConverter->LinkToIdentifier( *aStoreEvent.iContactLink );
            }
        
        iObserver->HandleEventL( event );
        }
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::OpenComplete
// -----------------------------------------------------------------------------
//
void CVasVPbkHandler::OpenComplete()
    {
    RUBY_DEBUG0( "CVasVPbkHandler::OpenComplete START" );

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::OpenComplete EXIT" );
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::VPbkSingleContactOperationComplete
// -----------------------------------------------------------------------------
//
void CVasVPbkHandler::VPbkSingleContactOperationComplete( MVPbkContactOperationBase& /*aOperation*/,
                                                          MVPbkStoreContact* aContact )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::VPbkSingleContactOperationComplete START" );

    iError = KErrNone;
    
    delete iOperation;
    iOperation = NULL;
    
    iContact = aContact;

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::VPbkSingleContactOperationComplete EXIT" );
    }
                                                 
// -----------------------------------------------------------------------------
// CVasVPbkHandler::VPbkSingleContactOperationFailed
// -----------------------------------------------------------------------------
//                                       
 void CVasVPbkHandler::VPbkSingleContactOperationFailed( MVPbkContactOperationBase& /*aOperation*/,
                                                         TInt aError )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::VPbkSingleContactOperationFailed START" );

    iError = aError;
    
    delete iOperation;
    iOperation = NULL;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::VPbkSingleContactOperationFailed EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ContactOperationCompleted
// -----------------------------------------------------------------------------
//                                         
void CVasVPbkHandler::ContactOperationCompleted( TContactOpResult /*aResult*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactOperationCompleted START" );

    iError = KErrNone;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::ContactOperationCompleted EXIT" );
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::ContactOperationFailed
// -----------------------------------------------------------------------------
//   
void CVasVPbkHandler::ContactOperationFailed( TContactOp /*aOpCode*/, TInt aErrorCode,
                                              TBool /*aErrorNotified*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactOperationFailed START" );

    iError = aErrorCode;
        
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::ContactOperationFailed EXIT" );
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::AttributeOperationComplete
// -----------------------------------------------------------------------------
//                                        
void CVasVPbkHandler::AttributeOperationComplete( MVPbkContactOperationBase& /*aOperation*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::AttributeOperationComplete START" );

    iError = KErrNone;
    
    delete iOperation;
    iOperation = NULL;

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::AttributeOperationComplete EXIT" );
    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::AttributeOperationFailed
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::AttributeOperationFailed( MVPbkContactOperationBase& /*aOperation*/,
                                                TInt aError )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::AttributeOperationFailed START" );

    iError = aError;
    
    delete iOperation;
    iOperation = NULL;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::AttributeOperationFailed EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::StepComplete
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::StepComplete( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::StepComplete START" );

    // Do nothing
    
    RUBY_DEBUG0( "CVasVPbkHandler::StepComplete EXIT" );
    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::StepFailed
// -----------------------------------------------------------------------------
//  
TBool CVasVPbkHandler::StepFailed( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/,
                                   TInt /*aError*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::StepFailed START" );   
    RUBY_DEBUG0( "CVasVPbkHandler::StepFailed EXIT" );
    
    return EFalse;
    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::OperationComplete
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::OperationComplete( MVPbkContactOperationBase& /*aOperation*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::OperationComplete START" );
    
    iError = KErrNone;
    
    delete iOperation;
    iOperation = NULL;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::OperationComplete EXIT" );
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ContactViewReady
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::ContactViewReady( MVPbkContactViewBase& /*aView*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactViewReady START" );
    
    iError = KErrNone;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::ContactViewReady EXIT" );
    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ContactViewUnavailable
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::ContactViewUnavailable( MVPbkContactViewBase& /*aView*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactViewUnavailable" );
    // Do nothing
    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ContactAddedToView
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::ContactAddedToView( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
                                          const MVPbkContactLink& /*aContactLink*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactViewUnavailable" );
    // Do nothing
    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ContactRemovedFromView
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::ContactRemovedFromView( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
                                              const MVPbkContactLink& /*aContactLink*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactViewUnavailable" );
    // Do nothing
    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ContactViewError
// -----------------------------------------------------------------------------
//  
void CVasVPbkHandler::ContactViewError( MVPbkContactViewBase& /*aView*/, TInt aError,
                                        TBool /*aErrorNotified*/ )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ContactViewReady START" );
    
    iError = aError;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    
    RUBY_DEBUG0( "CVasVPbkHandler::ContactViewReady EXIT" );
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::CVasVPbkHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVasVPbkHandler::CVasVPbkHandler()
 : iError( KErrNone )
    {
    }

// -----------------------------------------------------------------------------
// CVasVPbkHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVasVPbkHandler::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVasVPbkHandler::ConstructL" );

    }
        
// -----------------------------------------------------------------------------
// CVasVPbkHandler::SelectDefaultType
// Returns default field type for given tag
// -----------------------------------------------------------------------------
//
TVPbkDefaultType CVasVPbkHandler::SelectDefaultType( TInt aCommandType )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::SelectDefaultType START" );
    
    TVPbkDefaultType type = EVPbkDefaultTypeUndefined;
    
    switch( aCommandType )
        {
        case EDefaultCommand:
        case EMobileCommand:
        case EGeneralCommand:
        case EWorkCommand:
        case EHomeCommand:       
            {
            type = EVPbkDefaultTypePhoneNumber;
            break;
            }
    
        case EEmailCommand:
            {
            type = EVPbkDefaultTypeEmail;
            break;
            }
            
        case EVideoCommand:
            {
            type = EVPbkDefaultTypeVideoNumber;
            break;
            }
            
        case EVoipCommand:
            {
            type = EVPbkDefaultTypeVoIP;
            break;
            }
            
        case EMessageCommand:
            {
            if ( iMMSSupported )
                {
                type = EVPbkDefaultTypeMms;
                }
            else
                {
                type = EVPbkDefaultTypeSms;
                }
            break;
            }
            
        }
        
    RUBY_DEBUG0( "CVasVPbkHandler::SelectDefaultType EXIT" );
        
    return type;
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::AcceptDefaultField
// Accepts or rejects given field based on the given tag
// -----------------------------------------------------------------------------
//
TBool CVasVPbkHandler::AcceptDefaultField( MVPbkStoreContactField& aField, TInt aCommandType )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::AcceptDefaultField START" );
    
    TBool accept = ETrue;
    
    if ( aCommandType == EMobileCommand &&
         aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_MOBILEPHONEHOME &&
         aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_MOBILEPHONEWORK &&
         aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_MOBILEPHONEGEN )
        {
        accept = EFalse;
        }
    else if ( aCommandType == EGeneralCommand &&
              aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_LANDPHONEHOME &&
              aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_LANDPHONEWORK &&
              aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_LANDPHONEGEN )
        {
        accept = EFalse;
        }
    else if ( aCommandType == EWorkCommand &&
              aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_LANDPHONEWORK )
        {
        accept = EFalse;
        }
    else if ( aCommandType == EHomeCommand &&
              aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_LANDPHONEHOME )
        {
        accept = EFalse;
        }
    else if ( aCommandType == EVoipCommand &&
              aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_MOBILEPHONEGEN &&
              aField.BestMatchingFieldType()->FieldTypeResId() != R_VPBK_FIELD_TYPE_LANDPHONEGEN )
        {
        accept = EFalse;
        }
        
    RUBY_DEBUG0( "CVasVPbkHandler::AcceptDefaultField EXIT" );
        
    return accept;
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ConvertFieldId
// Converts field id to old phonebook id
// -----------------------------------------------------------------------------
//
TInt CVasVPbkHandler::ConvertFieldId( MVPbkStoreContactField& aField )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ConvertFieldId START" );
    
    TInt id = KErrNone;
    
    if ( aField.BestMatchingFieldType() )
        {
        switch( aField.BestMatchingFieldType()->FieldTypeResId() )
            {
            case R_VPBK_FIELD_TYPE_LANDPHONEGEN:
                {
                id = EPbkFieldIdPhoneNumberGeneral;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_LANDPHONEHOME:
                {
                id = EPbkFieldIdPhoneNumberHome;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_LANDPHONEWORK:
                {
                id = EPbkFieldIdPhoneNumberWork;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_MOBILEPHONEHOME:
            case R_VPBK_FIELD_TYPE_MOBILEPHONEWORK:
            case R_VPBK_FIELD_TYPE_MOBILEPHONEGEN:
                {
                id = EPbkFieldIdPhoneNumberMobile;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_VIDEONUMBERHOME:
            case R_VPBK_FIELD_TYPE_VIDEONUMBERWORK:
            case R_VPBK_FIELD_TYPE_VIDEONUMBERGEN:
                {
                id = EPbkFieldIdPhoneNumberVideo;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_FAXNUMBERHOME:
            case R_VPBK_FIELD_TYPE_FAXNUMBERWORK:
            case R_VPBK_FIELD_TYPE_FAXNUMBERGEN:
                {
                id = EPbkFieldIdFaxNumber;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_VOIPHOME:
            case R_VPBK_FIELD_TYPE_VOIPWORK:
            case R_VPBK_FIELD_TYPE_VOIPGEN:
                {
                id = EPbkFieldIdVOIP;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_IMPP:
                {
                id = EPbkFieldIdXsp;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_EMAILHOME:
            case R_VPBK_FIELD_TYPE_EMAILWORK:
            case R_VPBK_FIELD_TYPE_EMAILGEN:
                {
                id = EPbkFieldIdEmailAddress;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_FIRSTNAMEREADING:
                {
                id = EPbkFieldIdFirstNameReading;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_FIRSTNAME:
                {
                id = EPbkFieldIdFirstName;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_LASTNAMEREADING:
                {
                id = EPbkFieldIdLastNameReading;
                break;
                }
                            
            case R_VPBK_FIELD_TYPE_LASTNAME:
                {
                id = EPbkFieldIdLastName;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_COMPANYNAME:
                {
                id = EPbkFieldIdCompanyName;
                break;
                }
                
            case R_VPBK_FIELD_TYPE_SECONDNAME:
                {
                id = EPbkFieldIdSecondName;
                break;
                }
            }
        }
    else
        {
        RUBY_DEBUG0( "aField.BestMatchingFieldType() returned NULL" );
        }
        
    RUBY_DEBUG0( "CVasVPbkHandler::ConvertFieldId EXIT" );
        
    return id;
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::ConvertEvent
// Converts contact event type
// -----------------------------------------------------------------------------
//
TVasContactEvent CVasVPbkHandler::ConvertEvent( const TVPbkContactStoreEvent& aEvent )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::ConvertEvent START" );
   
    TVasContactEvent event;
    
    switch( aEvent.iEventType )
        {
        case ENullEvent:       
            {
            event = ENullEvent;
            break;
            }
    
        case EContactChanged:
            {
            event = EContactChanged;
            break;
            }
            
        case EContactDeleted:
            {
            event = EContactDeleted;
            break;
            }
            
        case EContactAdded:
            {
            event = EContactAdded;
            break;
            }
            
        case EUnknownChanges:
            {
            event = EUnknownChanges;
            break;
            }
            
        case EGroupAdded:       
            {
            event = EGroupAdded;
            break;
            }
    
        case EGroupChanged:
            {
            event = EGroupChanged;
            break;
            }
            
        case EGroupDeleted:
            {
            event = EGroupDeleted;
            break;
            }
            
        case EStoreBackupBeginning:
            {
            event = EStoreBackupBeginning;
            break;
            }
            
        case EStoreRestoreBeginning:
            {
            event = EStoreRestoreBeginning;
            break;
            }
            
        case EStoreBackupRestoreCompleted:
            {
            event = EStoreBackupRestoreCompleted;
            break;
            }
        
        default:
            {
            event = ENotSupported;
            break;
            }
        }
    RUBY_DEBUG0( "CVasVPbkHandler::ConvertEvent EXIT" );
        
    return event;
    }
    
// -----------------------------------------------------------------------------
// CVasVPbkHandler::FieldPriority
// Gets the priority of the given field
// -----------------------------------------------------------------------------
//
TInt CVasVPbkHandler::FieldPriority( MVPbkStoreContactField& aField )
    {
    RUBY_DEBUG0( "CVasVPbkHandler::FieldPriority START" );
    
    TInt priority = KFieldPriorityCount;
    
    for ( int i = 0; i < KFieldPriorityCount; ++i )
        {
        if ( aField.BestMatchingFieldType()->FieldTypeResId() == KFieldPriority[ i ] )
            {
            priority = i;
            break;
            }
        }
    RUBY_DEBUG0( "CVasVPbkHandler::FieldPriority EXIT" );
    
    return priority;
    }

//  End of File

