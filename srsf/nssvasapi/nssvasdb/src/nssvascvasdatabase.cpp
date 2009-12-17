/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssVasDatabase is responsible for the issuing of requests  to
*               to the VasDb server and returning the results to the VasDb Managers.
*
*/


#include "srsfbldvariant.hrh"
#include <e32base.h>
#include <e32test.h>
#include "nssvascvasdatabase.h"
#include "nssvascspeechitem.h"
#include "nssvascvasdbsrvdef.h"
#include "nssvastspeechitemconstant.h"
#include <f32file.h>
#include "rubydebug.h"

// Complementary macro for TRAPD:
// TRAPD( err, doSomething() );
// REACT( err, return( err ) );
#define REACT(a,b) if ( a < 0 ) {b;}

const TInt KTagRefListGranularity = 2;

const TInt KFetchedTagRefListGranularity = 50;

#ifdef _DEBUG
// used in UDEB macros only
_LIT( KVasDatabasePanic, "CNssVasDatabase.cpp" );
#endif // _DEBUG

// ============================ Methods ===============================

//-------------------------------------------------------- 
//C++ default constructor can NOT contain any code that
// might leave.
//--------------------------------------------------------    
CNssVASDatabase::CNssVASDatabase( TInt /*aPriority*/ )
: iIsLocked( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssVASDatabase* CNssVASDatabase::NewLC( TInt aPriority )
    {
	CNssVASDatabase* self = new(ELeave)CNssVASDatabase( aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssVASDatabase* CNssVASDatabase::NewL( TInt aPriority )
    {
	CNssVASDatabase* self = NewLC( aPriority );
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNssVASDatabase::ConstructL()
    {
    iContextBuilder         = CNssContextBuilder::NewL();
    iSpeechItemBuilder      = CNssSpeechItemBuilder::NewL( this );
    iVasDb                  = CNssVasDb::NewL( *iContextBuilder, 
                                               *iSpeechItemBuilder);
    iTNssTagRefList         = new(ELeave) TNssTagReferenceListArray( KTagRefListGranularity );
    }

//------------------------------------------
// CNssVASDatabase::~CNssVASDatabase
// Destructor
//------------------------------------------
CNssVASDatabase::~CNssVASDatabase()
    {
    RUBY_DEBUG0( "CNssVASDatabase::~CNssVASDatabase" ); 

    if ( iGrammarIdRuleIdPackage )
	    {
	    delete iGrammarIdRuleIdPackage;
        }

	if ( iSpeechItemBuilder )
	    {
	    delete iSpeechItemBuilder;
	    }

	if ( iVASDBEventMonitor )
	    {
	    delete iVASDBEventMonitor;
	    }

	if ( iTNssTagRefList )
	    {
        iTNssTagRefList->Reset();
	    delete iTNssTagRefList;
	    }

	if ( iVasDb )
	    {
	    delete iVasDb;
	    }
	if ( iContextBuilder )
	    {
	    delete iContextBuilder;
        }	    
    }
    
// -----------------------------------------------------------------------------
// RNssVasDbSession::OpenDatabase
// Method to open the database.
// -----------------------------------------------------------------------------
//
TInt CNssVASDatabase::OpenDatabase()
    {
    if ( iConnected )
        {
        iConnected++;
        return KErrDbAlreadyOpen;
        }

    TRAPD( error, iVasDb->OpenDatabaseL() ); 

    if ( error == KErrNone )
        {
        iConnected++;
        }

    return error;
    }
	
// -----------------------------------------------------------------------------
// RNssVasDbSession::CloseDatabase
// Method to close the database.
// -----------------------------------------------------------------------------
//
void CNssVASDatabase::CloseDatabase()
    {
    // Check if already closed (or never opened)
    if ( iConnected > 0 )
        {
        // Some other VAS instance in the same thread may be using the database.
        iConnected--;

        // If not, close it.
        if ( iConnected <= 0 )
            {
            iVasDb->CloseDatabase();
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::CreateDb
// Method to create vas database
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::CreateDb()
    {
    TRAPD( error, iVasDb->CreateDatabaseL() );
    
    return error;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::StartMonitoringDatabaseL
// Method to start monitoring changes to vas database
// -----------------------------------------------------------------------------
void CNssVASDatabase::StartMonitoringDatabaseL()
    {
    //iVASDBEventMonitor = CNssVASDBEventMonitor::NewL();
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::AddObserverL
// Method to add an observer of the vas database
// -----------------------------------------------------------------------------
void CNssVASDatabase::AddObserverL( MNssVASDatabaseObserver* /*aObserver*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::RemoveObserverL
// Method to remove an observer of the vas database
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::RemoveObserver( MNssVASDatabaseObserver* /*aObserver*/ )
    {
	return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::ModelBankAndLexiconExist
// Method to determine if modelbank and lexicon ids exist.
// -----------------------------------------------------------------------------
TBool CNssVASDatabase::ModelBankAndLexiconExist( TUint32 &aModelBankId, TUint32 &aLexiconId )
    {
    TBool ret = EFalse;

    TRAPD( err, ret = iVasDb->GetModelBankIdLexiconIdL( aModelBankId, aLexiconId ) );

    // To remove warning
    if ( err != KErrNone )
        {
        }

    return ret;
    }

//-----------------------------------------------------------------------------
//CNssVasDatabase::TagExist
//Method to determine if tags exist for a context.
//-----------------------------------------------------------------------------
TBool CNssVASDatabase::TagExist( CNssContext* aContext )
    {
    TBool ret( EFalse );

    if ( aContext->ContextId() != KNssVASDbDefaultValue )
        {
        TRAPD( error, ret = iVasDb->TagExistL( aContext->ContextId() ) );
        REACT( error, ret = EFalse );
        } 
    else
        {
        RUBY_DEBUG0( "CNssVASDatabase::TagExist ERROR:Unsaved context" );
        }

    return ret;
    }
//-----------------------------------------------------------------------------
//CNssVasDatabase::TagCount
//Method to get the number of tags for a context.
//-----------------------------------------------------------------------------
TInt CNssVASDatabase::TagCount( CNssContext* aContext )
    {
    TInt ret( KErrNotFound );
   
    if ( aContext->ContextId() != KNssVASDbDefaultValue )
        {
        TRAPD( error, ret = iVasDb->TagCountL( aContext->ContextId() ) );

        // To remove warning
        if ( error != KErrNone )
            {
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::SaveContext
// Method to save a context 
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::SaveContext( CNssContext* aContext, TInt& aContextId )
    {
    TInt ret = KErrNone;

    aContextId = aContext->ContextId();
        
    if ( aContext->ContextId() == KNssVASDbDefaultValue )
        {
        TRAP( ret, iVasDb->SaveContextL( *aContext, aContextId ) );
        }
    else
        {
        TRAP( ret, iVasDb->UpdateContextL( *aContext ) );
        }
        
    return ret;  
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::SaveContextClientData
// Method to save the clientdata of a context 
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::SaveContextClientData( CNssContext* aContext )
    {
    TRAPD( error, iVasDb->UpdateContextClientDataL( *aContext ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetContext
// Method to get a context by name
// -----------------------------------------------------------------------------
CArrayPtrFlat<CNssContext>* CNssVASDatabase::GetContext( const TDesC& aName )
    {
    CArrayPtrFlat<CNssContext>* result = NULL;

    TRAPD( error, result = iVasDb->GetContextByNameL( aName ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetGlobalContexts
// Method to get all global contexts 
// -----------------------------------------------------------------------------
CArrayPtrFlat<CNssContext>* CNssVASDatabase::GetGlobalContexts()
    {
    CArrayPtrFlat<CNssContext>* result = NULL;

    TRAPD( error, result = iVasDb->GetGlobalContexts() );

    // To remove warning
    if ( error != KErrNone )
        {
        }

    return( result );
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetAllContexts
// Method to get all contexts 
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CNssContext>* CNssVASDatabase::GetAllContexts()
    {
    CArrayPtrFlat<CNssContext>* result = NULL;

    TRAPD( error, result = iVasDb->GetAllContexts() );

    // To remove warning
    if ( error != KErrNone )
        {
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::SaveTag
// Method to save a tag 
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::SaveTag( CNssTag* aTag, TInt& aNewId )
    {
    TInt ret = KErrNone;
        
    if ( aTag->TagId() == KNssVASDbDefaultValue )
        {
        TRAP( ret, iVasDb->SaveTagL( *aTag, aNewId ) );
        }
    else
        {
        // Used to call RNssVasDbSession::UpdateTag
        // which returned KErrNotSupported
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::DeleteTag
// Method to delete several tags 
// -----------------------------------------------------------------------------
//
TInt CNssVASDatabase::DeleteTags( const RArray<TUint32>& aTagIdArray )
    {
    TRAPD( error, iVasDb->DeleteTagsL( aTagIdArray ) );

    return error;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::SaveTag
// Method to save several tags 
// -----------------------------------------------------------------------------
//
TInt CNssVASDatabase::SaveTags( RPointerArray<CNssTag>* aTagArray )
    {
    RUBY_DEBUG0("CNssVASDatabase::SaveTags");
        
    RArray<TInt> tagIdArray;
    
    TInt arrayCount = aTagArray ? aTagArray->Count() : 0;
    if( arrayCount == 0 )
        {
        arrayCount++;
        }
        
    CArrayPtrFlat<CNssTag>* ctagArray = new CArrayPtrFlat<CNssTag>( arrayCount );
    
    for( TInt i( 0 ); i < aTagArray->Count(); i++ )
        {
        TRAPD( error, ctagArray->AppendL( (*aTagArray)[i] ) );
        if( error != KErrNone )
            {
            delete ctagArray;
            return error;
            }
        }

    TRAPD( error, iVasDb->SaveTagsL( ctagArray, tagIdArray ) );
    
    if( error == KErrNone ) 
    	{
    	RUBY_ASSERT_DEBUG( ctagArray->Count() == tagIdArray.Count(), 
    	                   User::Panic( KVasDatabasePanic, __LINE__ ) );

    	for ( TInt k( 0 ); k < ctagArray->Count(); k++ )
        	{
        	(*aTagArray)[k]->SetTagId( tagIdArray[k] );
        	}
    	}
    else 
    	{
    	// iSess->SaveTags failed
    	/** @todo Higher level handler should display some "No memory" label if possible 
    	Or should we do it here? */
    	}
    	
    delete ctagArray;
    
    tagIdArray.Close();
    	
    return error;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTag
// Method to get a tag by name
// -----------------------------------------------------------------------------
MNssTagListArray* CNssVASDatabase::GetTag( const TDesC& aName )
    {
    CArrayPtrFlat<CNssTag>* result = NULL;

    TRAPD( error, result = iVasDb->GetTagL( aName ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }
    
    CArrayPtrFlat<MNssTag>* array = NULL;
    
    TInt arrayCount = result ? result->Count() : 0;
    if( arrayCount == 0 )
        {
        arrayCount++;
        }
    
    array = new CArrayPtrFlat<MNssTag>( arrayCount );
    if( !array )
        {
        return NULL;
        }
    
    if( result )
        {
        for( TInt i( 0 ); i < result->Count(); i++ )
            {
            TRAP( error, array->AppendL( result->At( i ) ) );
            if( error != KErrNone )
                {
                return NULL;
                }
            }
            
        result->Reset();
        delete result;   
        }
        
    return array;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTag
// Method to get a tag by context and name
// Not fully implemented.
// -----------------------------------------------------------------------------
 
MNssTagListArray* CNssVASDatabase::GetTag(CNssContext* /*aContext*/,const TDesC& aName)
    {
    return GetTag(aName);
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTag
// Method to get a tag by context.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<MNssTag>* CNssVASDatabase::GetTag( CNssContext* aContext )
    {
    CArrayPtrFlat<CNssTag>* result = NULL;

    TRAPD( error, result =  iVasDb->GetTagL( *aContext ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }
    
    CArrayPtrFlat<MNssTag>* array = NULL;
    
    if( result )
        {
        array = new CArrayPtrFlat<MNssTag>( result->Count() );
        if( !array )
            {
            return NULL;
            }
        
        for( TInt i( 0 ); i < result->Count(); i++ )
            {
            CNssTag* tag = result->At( i );
            TRAP( error, array->AppendL( result->At( i ) ) );
            if( error != KErrNone )
                {
                return NULL;
                }
            }
            
        result->Reset();
        delete result;        
        }
    
    return array;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTag
// Method to get a tag by grammarid and ruleid.
// -----------------------------------------------------------------------------
//
MNssTagListArray* CNssVASDatabase::GetTag( const TInt aGrammarID, const TInt aRuleID )
    {
    iTNssGrammarIdRuleId.iGrammarId = aGrammarID;
    iTNssGrammarIdRuleId.iRuleId    = aRuleID;

    CArrayPtrFlat<CNssTag>* result = NULL;

    TRAPD( error, result =  iVasDb->GetTagL( iTNssGrammarIdRuleId ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }
    
    CArrayPtrFlat<MNssTag>* array = NULL;
    
    TInt arrayCount = result ? result->Count() : 0;
    if( arrayCount == 0 )
        {
        arrayCount++;
        }
    
    array = new CArrayPtrFlat<MNssTag>( arrayCount );
    if( !array )
        {
        return NULL;
        }
    
    if( result )
        {
        for( TInt i( 0 ); i < result->Count(); i++ )
            {
            TRAP( error, array->AppendL( result->At( i ) ) );
            if( error != KErrNone )
                {
                return NULL;
                }
            }
            
        result->Reset();
        delete result;    
        }
    
    return array;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTags
// Method to get tags by their grammarid and ruleid.
// -----------------------------------------------------------------------------
//
MNssTagListArray* CNssVASDatabase::GetTags( TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds)
	{
	CArrayPtrFlat<CNssTag>* result = NULL;

    TRAPD( error, result =  iVasDb->GetTagsL( aGrammarIdRuleIds ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }
    
    CArrayPtrFlat<MNssTag>* array = NULL;
    
    TInt arrayCount = result ? result->Count() : 0;
    if( arrayCount == 0 )
        {
        arrayCount++;
        }
    
    array = new CArrayPtrFlat<MNssTag>( arrayCount );
    if( !array )
        {
        return NULL;
        }
    
    if( result )
        {
        for( TInt i( 0 ); i < result->Count(); i++ )
            {
            TRAP( error, array->AppendL( result->At( i ) ) );
            if( error != KErrNone )
                {
                return NULL;
                }
            }
            
        result->Reset();
        delete result;        
        }

    return array;
	}

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTag
// Method to get a tag by tagid
// -----------------------------------------------------------------------------
MNssTagListArray* CNssVASDatabase::GetTag( const TUint32 aTagId )
    {
    CArrayPtrFlat<CNssTag>* result = NULL;

    TRAPD( error, result =  iVasDb->GetTagL( aTagId ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }
    
    CArrayPtrFlat<MNssTag>* array = NULL;
    
    TInt arrayCount = result ? result->Count() : 0;
    if( arrayCount == 0 )
        {
        arrayCount++;
        }
    
    array = new CArrayPtrFlat<MNssTag>( arrayCount );
    if( !array )
        {
        return NULL;
        }
    
    if( result )
        {
        for( TInt i( 0 ); i < result->Count(); i++ )
            {
            TRAP( error, array->AppendL( result->At( i ) ) );
            if( error != KErrNone )
                {
                return NULL;
                }
            }
            
        result->Reset();
        delete result;
        }
        
    return array;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTagList
// Method to get tags by rrd int position.
// -----------------------------------------------------------------------------
MNssTagListArray* CNssVASDatabase::GetTagList( CNssContext* aContext, TInt aNum, TInt aPosition )
    {    
    CArrayPtrFlat<CNssTag>* result = NULL;

    TRAPD( error, result = iVasDb->GetTagL( aContext->ContextId(),
                                            aNum, aPosition ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }
    
    CArrayPtrFlat<MNssTag>* array = NULL;
    
    TInt arrayCount = result ? result->Count() : 0;
    if( arrayCount == 0 )
        {
        arrayCount++;
        }
    
    array = new CArrayPtrFlat<MNssTag>( arrayCount );
    if( !array )
        {
        return NULL;
        }
    
    if( result )
        {
        for( TInt i( 0 ); i < result->Count(); i++ )
            {
            TRAP( error, array->AppendL( result->At( i ) ) );
            if( error != KErrNone )
                {
                return NULL;
                }
            }
            
        result->Reset();
        delete result;        
        }
    
    return array;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::GetTagList
// Method to get tags by rrd text value and position.
// -----------------------------------------------------------------------------
MNssTagListArray* CNssVASDatabase::GetTagList( CNssContext* aContext, 
                                               TDesC& aText, TInt aPosition )
    {
    CArrayPtrFlat<CNssTag>* result = NULL;

    TRAPD( error, result = iVasDb->GetTagL( aContext->ContextId(),
                                            aText, aPosition ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }
    
    CArrayPtrFlat<MNssTag>* array = NULL;
    
    TInt arrayCount = result ? result->Count() : 0;
    if( arrayCount == 0 )
        {
        arrayCount++;
        }
    
    array = new CArrayPtrFlat<MNssTag>( arrayCount );
    if( !array )
        {
        return NULL;
        }
    
    if( result )
        {
        for( TInt i( 0 ); i < result->Count(); i++ )
            {
            TRAP( error, array->AppendL( result->At( i ) ) );
            if( error != KErrNone )
                {
                return NULL;
                }
            }
            
        result->Reset();
        delete result;        
        }
    
    return array;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::DeleteTag
// Method to delete a tag.
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::DeleteTag( CNssTag *aTag )
    {
    TInt ret( KErrNotFound );
   
    if ( aTag->TagId() != KNssVASDbDefaultValue )
        {
        ret = DeleteTag( aTag->TagId() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::DeleteTag
// Method to delete a tag by name.
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::DeleteTag( const TDesC& aName )
    {
    TRAPD( error, iVasDb->DeleteTagL( aName ) );

    return( error );
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::DeleteTag
// Method to delete a tag by key/id.
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::DeleteTag( const TUint32 aTagId )
    {
    TRAPD( error, iVasDb->DeleteTagL( aTagId ) );

    return( error );
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::DeleteContext
// Method to delete context.
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::DeleteContext( CNssContext* aContext )
    {
    return( DeleteContext( aContext->ContextName() ) );
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::DeleteContext
// Method to delete context by name.
// -----------------------------------------------------------------------------
TInt CNssVASDatabase::DeleteContext( const TDesC& aName )
    {
    TRAPD( error, iVasDb->DeleteContextL( aName ) );

    return( error );
    }

// -----------------------------------------------------------------------------
// CNssVasDatabase::DeleteContext
// Method to all tag references of a context.
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<MNssTagReference>* CNssVASDatabase::GetTagReferenceList( CNssContext* aContext )
    {
    TNssTagReferenceListArray* ttagRefArray = NULL;
   
    TRAPD( error,
           ttagRefArray = iVasDb->GetTagReferenceListL( aContext->ContextId() ) );

    // To remove warning
    if ( error != KErrNone )
        {
        }

    if ( !ttagRefArray )
        {
        return( NULL );
        }

    // Convert TNssTagReferences to CNssTagReferences
    CArrayPtrFlat<MNssTagReference>* result = 
            new CArrayPtrFlat<MNssTagReference>( KFetchedTagRefListGranularity );

    if ( !result )
        {
        delete ttagRefArray;
        ttagRefArray = 0;
        return( NULL );
        }

    TRAPD( err, ConvertTNssTagRefToMNssTagRefL( ttagRefArray, result ) );

    delete ttagRefArray;
    ttagRefArray = 0;

    if ( err != KErrNone )
        {
        result->ResetAndDestroy();
        delete result;
        result = 0;
        }

    return( result );
    }


// ------------------------
// Private Utility Methods  
// -----------------------

// -----------------------------------------------------------------------------
// CNssVASDatabase::ConvertTNssTagRefToMNssTagRefL
//
// -----------------------------------------------------------------------------
//
void CNssVASDatabase::ConvertTNssTagRefToMNssTagRefL( 
                            TNssTagReferenceListArray* aTNssTagRef, 
                            CArrayPtrFlat<MNssTagReference>* aCNssTagRef )
    {
    CNssTagReference* tagRefPtr=NULL;

    for ( TInt i( 0 ); i < aTNssTagRef->Count(); i++ )
        {
        tagRefPtr = new(ELeave)CNssTagReference();
        CleanupStack::PushL( tagRefPtr );
        tagRefPtr->SetTagNameL( (*aTNssTagRef)[i].iTagName );
        tagRefPtr->SetTagId( (*aTNssTagRef)[i].iTagId );
	    aCNssTagRef->AppendL( tagRefPtr );
	    CleanupStack::Pop( tagRefPtr );
        }
    }

// -----------------------------------------------------------------------------
// CNssVASDatabase::ConvertDBError
//
// -----------------------------------------------------------------------------
//
MNssVASDatabaseClient::TNssVASDBClientReturnCode CNssVASDatabase::ConvertDBError( TInt aError )
    {
    MNssVASDatabaseClient::TNssVASDBClientReturnCode failcode = 
                                        MNssVASDatabaseClient::EVASDBFailure;

    switch ( aError )
        {
        case KErrNone:
		    failcode = MNssVASDatabaseClient::EVASDBSuccess;
        break;
        case KErrNotFound:
		    failcode = MNssVASDatabaseClient::EVASDBItemNotFound;
	    break;
        case KErrNoMemory:
            failcode = MNssVASDatabaseClient::EVASDBNoMemory;
        break;
        case KErrDiskFull:
		    failcode = MNssVASDatabaseClient::EVASDBDiskFull;
	    break;
        default:
            failcode = MNssVASDatabaseClient::EVASDBFailure;//just in case
        break;
        }

    return failcode;
    }

// -----------------------------------------------------------------------------
// CNssVASDatabase::GetDefaultModelBankId
//
// -----------------------------------------------------------------------------
//
TInt CNssVASDatabase::GetDefaultModelBankId( TUint32& aId )
    {
    TRAPD( error, iVasDb->GetDefaultModelBankIdL( aId ) );
    
    return error;
    }

// -----------------------------------------------------------------------------
// CNssVASDatabase::ResetModelBank
//
// -----------------------------------------------------------------------------
//
TInt CNssVASDatabase::ResetModelBank( TUint32 aNewId )
    {
    TRAPD( error, iVasDb->ResetModelBankL( aNewId ) );
    
    return error;
    }

// -----------------------------------------------------------------------------
// CNssVASDatabase::UpdateTagRuleIDs
//
// -----------------------------------------------------------------------------
//
TInt CNssVASDatabase::UpdateTagRuleIDs(
    const RArray<TNssSpeechItem>& aRetrainedTags )
    {
    TRAPD( error, iVasDb->UpdateTagRuleIDsL( aRetrainedTags ) );
    
    return( error );
    }
