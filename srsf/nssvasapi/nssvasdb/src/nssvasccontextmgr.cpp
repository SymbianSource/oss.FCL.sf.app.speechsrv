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
* Description:  CNssContextManager provides context processing for the client. CNssContextManager
*               implements the MNssContextMgr interface, and also provides additional methods for
*               use by internal VAS components. 
*
*/


// include files
#include "nssvasccontextmgr.h"
#include "nssvascvasdatabase.h"
#include "nssvasccontextbuilder.h"
#include "nssvasmresetfactorymodelsclient.h"
#include "rubydebug.h"

const TInt KContextArrayGranularity = 5;

_LIT( KContextMgrPanic, "contextmgr.cpp");

// ---------------------------------------------------------
// CNssContextMgr::CNssContextMgr
// C++ constructor
// ---------------------------------------------------------
//
CNssContextMgr::CNssContextMgr()
: CActive( EPriorityStandard )
    {
    }

// ---------------------------------------------------------
// CNssContextMgr::CNssContextMgr
// overloaded C++ constructor
// ---------------------------------------------------------
//
CNssContextMgr::CNssContextMgr( CNssVASDatabase* aVASDatabase )
: CActive( EPriorityStandard )
    {
	iVasDatabase = aVASDatabase;
	iState = ENone;
    }

// ---------------------------------------------------------
// CNssContextMgr::~CNssContextMgr
// destructor
// ---------------------------------------------------------
//
CNssContextMgr::~CNssContextMgr()
    {
	if ( iContextBuilder )
	    {
		delete iContextBuilder;
		iContextBuilder = NULL;
	    }

	// Cancel any oustanding requests before destroying self.
	if( IsActive() )
	    {
	    Cancel(); // -> in base class -> goes on to call DoCancel in this class...
	    }
	
	// Delete context list if it has not been delivered to the client yet
    if ( iContextList )
        {
        iContextList->ResetAndDestroy();
        delete iContextList;
        }	    
    }

// ---------------------------------------------------------
// CNssContextMgr::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CNssContextMgr* CNssContextMgr::NewL( CNssVASDatabase* aVASDatabase )
    {
    CNssContextMgr* self = NewLC( aVASDatabase );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CNssContextMgr::NewLC
// Two-phased constructor.
// ---------------------------------------------------------
//
CNssContextMgr* CNssContextMgr::NewLC( CNssVASDatabase* aVASDatabase )
    {
    CNssContextMgr* self = new (ELeave) CNssContextMgr( aVASDatabase );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CNssContextMgr::ConstructL
// EPOC constructor can leave.
// ---------------------------------------------------------
//
void CNssContextMgr::ConstructL()
    {
	iContextBuilder = CNssContextBuilder::NewL();

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CNssContextMgr::CreateContextL
// creates an empty context object
// ---------------------------------------------------------
//
MNssContext* CNssContextMgr::CreateContextL()
    {
    return iContextBuilder->CreateContextL(); 
    }

// ---------------------------------------------------------
// CNssContextMgr::GetContext
// retrives a context fromt he VAS DB based on the name of the context
// ---------------------------------------------------------
//
TInt CNssContextMgr::GetContext( MNssGetContextClient* aContextClient, const TDesC& aName )
    {
    if ( IsActive() || !aContextClient || aName.Length() == 0 )
        {
        return KErrGeneral;
        }

    if ( iContextList )
        {
        iContextList->ResetAndDestroy();
        delete iContextList;
        iContextList = NULL;
        }
        
    TInt ret( KErrNone );

    iContextList = iVasDatabase->GetContext( aName );

    if ( !iContextList )
        {
        ret = KErrGeneral;
        }
    
    else if ( iContextList->Count() == 0 )
        {
        delete iContextList;
        iContextList = NULL;
        ret = KErrGeneral;
        }
    
    else
        {
    	iState = EGetContextClientGetContext;
        iGetContextClient = aContextClient;

        TRequestStatus* pRS = &iStatus;
    	User::RequestComplete( pRS, KErrNone );
    	SetActive();        
        }
    
    return ret;
    }

// ---------------------------------------------------------
// CNssContextMgr::GetContextList
// retreives a list of contexts from the VAS DB
// ---------------------------------------------------------
//
TInt CNssContextMgr::GetContextList( MNssGetContextClient* aContextClient )
    {
    if ( IsActive() || !aContextClient )
        {
        return KErrGeneral;
        }

    if ( iContextList )
        {
        iContextList->ResetAndDestroy();
        delete iContextList;
        iContextList = 0;
        }

    TInt ret( KErrNone );

    iContextList = iVasDatabase->GetAllContexts();

    if ( !iContextList )
        {
        ret = KErrGeneral;
        }

    else if ( iContextList->Count() == 0 )
        {
        delete iContextList;
        iContextList = NULL;
        ret = KErrGeneral;
        }
        
    else
        {
        iState = EGetContextClientGetContextList;
        iGetContextClient = aContextClient;

        TRequestStatus* pRS = &iStatus;
    	User::RequestComplete( pRS, KErrNone );
    	SetActive();
        }

    return ret;
    }

// ---------------------------------------------------------
// CNssContextMgr::DeleteContext
// deletes a context from VAS DB
// ---------------------------------------------------------
//
TInt CNssContextMgr::DeleteContext( MNssDeleteContextClient* aContextClient,
                                    MNssContext* aContext )
    {
	iState = EDeleteContextClient;
	iDeleteContextClient = aContextClient;
	iLocalContext = (CNssContext*)(aContext);

	// will fail if 2 requests come in together
	TRAPD( error, iLocalContext->BeginDeleteFromSrsL(this) );
	
	return error;
    }

// ---------------------------------------------------------
// CNssContextMgr::SaveContext
// saves a context in the vas db
// ---------------------------------------------------------
//
TInt CNssContextMgr::SaveContext( MNssSaveContextClient* aContextClient,MNssContext* aContext )
    {
    if ( IsActive() || !aContextClient || !aContext )
        {
        return KErrGeneral;
        }

    if ( !( &aContext->ContextName() ) )
        {
        return KErrGeneral;
        }

	iState = ESaveContextClient;
	iSaveContextClient = aContextClient;
	iLocalContext = (CNssContext*)(aContext);

	TUint32 modelBankId;
	TUint32 lexiconId;
	if ( iVasDatabase->ModelBankAndLexiconExist( modelBankId, lexiconId ) )
	    {
		iLocalContext->SetModelBankId( modelBankId );
	    iLocalContext->SetLexiconId( lexiconId );
	    iLocalContext->SetModelBankAndLexiconExist( ETrue );
	    }

	// will fail if 2 requests come in together
	TRAPD( err, iLocalContext->BeginSaveToSrsL(this) );
	
	return err;
    }

// ---------------------------------------------------------
// CNssContextMgr::SaveClientData
// Saves to the VAS DB the client data associated with a context
// ---------------------------------------------------------
//
TInt CNssContextMgr::SaveClientData(
    MNssContext* aContext )
    {
    if ( !aContext || !( &aContext->ContextName() ) )
        {
        return KErrArgument;
        }

    return iVasDatabase->SaveContextClientData( (CNssContext*)aContext );
    }

// ---------------------------------------------------------
// CNssContextMgr::GetGlobalContexts
// retrives a list of context from the VAS DB, that have
// their global flag ETrue
// ---------------------------------------------------------
//
TMNssContextList* CNssContextMgr::GetGlobalContexts()
    {
    return iVasDatabase->GetGlobalContexts();
    }

// ---------------------------------------------------------
// CNssContextMgr::VASDatabaseComplete
// call back implementation from interface MNssVASDatabaseClient
// ---------------------------------------------------------
//
void CNssContextMgr::VASDatabaseComplete(CArrayPtrFlat<CNssContext> *aContextList, 
									  CArrayPtrFlat<CNssTag>* /*aTagList*/ ,
									  CArrayPtrFlat<CNssTagReference>* /*aTagRefList*/,
									  TNssVASDBClientReturnCode aCode)
    {
    switch(iState)
        {
        case EGetContextClientGetContext:
            iState = ENone;
            if ( aCode == EVASDBSuccess )
                {
                
                if ( iGetContextClient )
                    {
                    // make a copy of the context to return, this copy needs to
                    // be deleted by the client.
                    MNssContext *context = NULL;
                    TRAPD( err, (context = ((*aContextList)[0])->CopyL()) );
                    aContextList->ResetAndDestroy();
                    delete aContextList;
                    if( err == KErrNone )
                        {
                        // call back to client with copied context.
                        iGetContextClient->GetContextCompleted( context, KErrNone );
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                        iGetContextClient->GetContextCompleted( context );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY 
                        }
                    else
                        {
                        // call back to client with error code.
                        iGetContextClient->GetContextCompleted( NULL, KErrGeneral ); 
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                        iGetContextClient->GetContextFailed( MNssGetContextClient::EVASDBFailure );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                         
                        }
                    
                    }
                else
                    {
                    aContextList->ResetAndDestroy();
                    delete aContextList;
                    }
                }
            else 
                {
                // if get context list from VAS DB failed, there is no list returned back,
                // thus no need to delete any list.
                if ( iGetContextClient )
                    {
                    iGetContextClient->GetContextCompleted( NULL, KErrGeneral ); 
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                    iGetContextClient->GetContextFailed( MNssGetContextClient::EVASDBFailure );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                    }
                }
            break;
            
        case EGetContextClientGetContextList:
            iState = ENone;
            if ( aCode == EVASDBSuccess )
                {
                TRAPD( err, CNssContextListToMNssContextListConvertorL( aContextList ) );
                if ( err != KErrNone && iGetContextClient )
                    {
                    iGetContextClient->GetContextCompleted( NULL, err );
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                    iGetContextClient->GetContextFailed( MNssGetContextClient::EVASDBFailure );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY 
                    
                    }
                }
            else 
                {
                if ( iGetContextClient )
                    {
                    iGetContextClient->GetContextCompleted( NULL, KErrGeneral );
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                    iGetContextClient->GetContextFailed( MNssGetContextClient::EVASDBFailure );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                     
                    }
                }
            break;
            
        case EDeleteContextClient:
            iState = ENone;
            if ( aCode == EVASDBSuccess )
                {
                if ( iDeleteContextClient )
                    {		
                    if ( iLocalContext->CommitSrsChanges() == KErrNone )
                        {
                        iDeleteContextClient->DeleteContextCompleted( KErrNone );
                        }
                    else
                        {
                        iDeleteContextClient->DeleteContextCompleted( KErrGeneral );
                        }
                    }
                }
            else 
                {
                iLocalContext->CommitSrsChanges();

                iDeleteContextClient->DeleteContextCompleted( KErrGeneral );	 
                }
            break;
            
        case ESaveContextClient:
            iState = ENone;

            if ( aCode == EVASDBSuccess )
                {
                // context has been saved in VAS DB and SRS DB.
                // model bank and lexicon have been created or existed from before
                iLocalContext->SetModelBankAndLexiconExist( ETrue );
                
                if ( iSaveContextClient )
                    {
                    if ( iLocalContext->CommitSrsChanges() == KErrNone )
                        {
                        iSaveContextClient->SaveContextCompleted( KErrNone );
                        }
                    else
                        {
                        iSaveContextClient->SaveContextCompleted( KErrGeneral );
                        }
                    }
                }
            else // failed to save context in vas db
                {
                iLocalContext->RollbackSrsChanges();
                iSaveContextClient->SaveContextCompleted( KErrGeneral );
                }
            break;
            
        case ESaveClientDataClient:
            iState = ENone;
            if ( aCode == EVASDBSuccess )
                {
                iSaveContextClient->SaveContextCompleted( KErrNone );
                }
            else{
                iSaveContextClient->SaveContextCompleted( KErrGeneral );
                }
            break;
            
        case EInterCompContextMgrClientGetContext:
            iState = ENone;
            if ( aCode == EVASDBSuccess )
                {
                if ( iInterCompContextMgrClient )
                    {
                    // make a copy of the context to return, this copy needs to
                    // be deleted by the client.
                    CNssContext *context = NULL;
                    TRAPD( err, (context = ((*aContextList)[0])->CopyL()) );
                    // delete the context list got from VAS DB.
                    aContextList->ResetAndDestroy();
                    delete aContextList;
                    if ( err == KErrNone )
                        {
                        // call back to client with copied context.
                        iInterCompContextMgrClient->GetContextCompleted( context, KErrNone );
                        }
                    else
                        {
                        // call back to client with error code.
                        iInterCompContextMgrClient->GetContextCompleted( 
                            NULL, err ); 
                        }
                    }
                else 
                    {
                    aContextList->ResetAndDestroy();
                    delete aContextList;
                    }
                }
            else 
                {
                if ( iInterCompContextMgrClient)
                    {
                    iInterCompContextMgrClient->GetContextCompleted( NULL, KErrGeneral );
                    }
                }
            break;
            
        case EInterCompContextMgrClientGetGlobalContexts:
            iState = ENone;
            if ( aCode == EVASDBSuccess )
                {
                if ( iInterCompContextMgrClient )
                    {
                    iInterCompContextMgrClient->GetGlobalContextsCompleted(
                        aContextList, KErrNone );
                    }
                else
                    {
                    aContextList->ResetAndDestroy();
                    delete aContextList;
                    }
                }
            else 
                {
                if ( iInterCompContextMgrClient )
                    {
                    iInterCompContextMgrClient->GetContextCompleted(
                        NULL, KErrGeneral );
                    }
                }
            break;
            
        case EInterCompContextMgrClientGetContextList:
            iState = ENone;
            if ( aCode == EVASDBSuccess )
                {
                if ( iInterCompContextMgrClient )
                    {
                    iInterCompContextMgrClient->GetContextListCompleted(
                        aContextList, NULL );
                    }
                else
                    {
                    aContextList->ResetAndDestroy();
                    delete aContextList;
                    }
                }
            else
                {
                if ( iInterCompContextMgrClient )
                    {
                    iInterCompContextMgrClient->GetContextCompleted( 
                        NULL, KErrNone );
                    }
                }
            break;
            
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CNssContextMgr::HandleSaveSrsDBCompleted
// call back implementation from MNssCoreSrsDBEventHandler
// ---------------------------------------------------------
//
void CNssContextMgr::HandleSaveSrsDBCompleted()
    {
    TInt contextId = KNssVASDbDefaultValue;

    // Save context to VAS DB
    TInt ret = iVasDatabase->SaveContext( iLocalContext, contextId );

    // Commit SRS DB
    if ( ret == KErrNone )
        {
        iLocalContext->CommitSrsChanges();
        }
    else
        {
        iLocalContext->RollbackSrsChanges();
        }

    // When a context is saved for the first time,
    // a Context ID is assigned for it.
    if ( iLocalContext->ContextId() == KNssVASDbDefaultValue )
        {
        iLocalContext->SetContextId( contextId );
        }

    // Send callback
    if ( iSaveContextClient )
        {
        if ( ret == KErrNone )
            {
            iSaveContextClient->SaveContextCompleted( KErrNone );
            }
        else
            {
            iSaveContextClient->SaveContextCompleted( KErrGeneral );
            }
        }
    }

// ---------------------------------------------------------
// CNssContextMgr::HandleDeleteSrsDBCompleted
// call back implementation from MNssCoreSrsDBEventHandler
// NO LONGER USED AFTER SERIALIZATION OF DELETETAG.
// CNssSpeechItemTrainer TAKES CARE OF DELETING NOW.
// ---------------------------------------------------------
//
void CNssContextMgr::HandleDeleteSrsDBCompleted()
    {
    TInt ret = iVasDatabase->DeleteContext( iLocalContext->ContextName() );

    // If VAS DB removal failed, roll back Plugin delete.
    if ( ret != KErrNone )
        {
        iLocalContext->RollbackSrsChanges();

        iDeleteContextClient->DeleteContextCompleted( KErrGeneral );
        return;
        }

    // If VAS DB removal was successful, commit the Plugin removal.
    TInt commitErr;
    commitErr = iLocalContext->CommitSrsChanges();

    // DB becomes corrupted: VAS DB removal was successful, SRS DB wasn't.
    if ( commitErr != KErrNone )
        {
        iDeleteContextClient->DeleteContextCompleted( KErrGeneral );
        return;
        }

    // Both removals were successful.
    iDeleteContextClient->DeleteContextCompleted( KErrNone );
    }

// -----------------------------------------------------------------------------
// CNssContextMgr::HandleResetSrsDBCompleted
//  call back from SRS DB 
// -----------------------------------------------------------------------------
//
void CNssContextMgr::HandleResetSrsDBCompleted()
    {
    if ( iVasDatabase->ResetModelBank( iModelBankId ) == KErrNone )
        {
        iContextBuilder->GetContextPortal()->CommitSaveContext();
        iResetClient->HandleResetComplete( KErrNone );
        }
    else{
        iContextBuilder->GetContextPortal()->RollbackSaveContext();
        iResetClient->HandleResetComplete( KErrGeneral );
        }
    }

// ---------------------------------------------------------
// CNssContextMgr::HandleSrsDBError
// call back implementation from MNssCoreSrsDBEventHandler
// ---------------------------------------------------------
//
void CNssContextMgr::HandleSrsDBError( MNssCoreSrsDBEventHandler::TNssSrsDBResult /*aResult*/)
    {
	switch(iState)
	    {
		case ESaveContextClient:
			iState = ENone;
			if ( iSaveContextClient )
			    {
			    iSaveContextClient->SaveContextCompleted( KErrGeneral );
			    }
			break;

		case EDeleteContextClient:
		    iState = ENone;
		    if ( iDeleteContextClient)
		        {
		        iDeleteContextClient->DeleteContextCompleted( KErrGeneral );
		        }	
		    break;

		default:
			break;
	    }
    }

// ---------------------------------------------------------
// CNssContextMgr::CNssContextListToMNssContextListConvertorL
// private method to convert CNssTag list to MNssTag list
// ---------------------------------------------------------
//
void CNssContextMgr::CNssContextListToMNssContextListConvertorL( 
                        CArrayPtrFlat<CNssContext>* aSourceList )
    {
	CArrayPtrFlat<MNssContext>* destinationList =  new (ELeave) CArrayPtrFlat<MNssContext>(1);
	CleanupStack::PushL( destinationList );
	CleanupResetAndDestroyPushL( *destinationList );
	for ( TInt i( 0 ); i < aSourceList->Count(); i++ )
	    {
		destinationList->AppendL((MNssContext*) (*aSourceList)[i]);
	    }
	aSourceList->Reset();
	delete aSourceList; 
	CleanupStack::Pop( destinationList );  // ResetAndDestroy
	CleanupStack::Pop( destinationList );  // list itself
	iGetContextClient->GetContextListCompleted( destinationList, KErrNone );
    }

// ---------------------------------------------------------
// CNssContextMgr::TagExist
// determine if a tags exists for a context in the VAS Database.
// ---------------------------------------------------------
//
TBool CNssContextMgr::TagExist( CNssContext* aContext )
    {
	return iVasDatabase->TagExist( aContext );
    }

// ---------------------------------------------------------
// CNssContextMgr::CancelGetContext
// cancel get context /get context list / get global contexts
// ---------------------------------------------------------
//
void CNssContextMgr::CancelGetContext()
    {
	iInterCompContextMgrClient = NULL;
	
	iGetContextClient = NULL;
    }

// ---------------------------------------------------------
// CNssContextMgr::DoCancel
// A function required by CActive
// ---------------------------------------------------------
//
void CNssContextMgr::DoCancel()
    {
    CancelGetContext();
    }

// ---------------------------------------------------------
// CNssContextMgr::RunL
// CActive calls this function when higher-priority tasks
// have been finished.
// ---------------------------------------------------------
//
void CNssContextMgr::RunL()
    {
 	TInt err( iStatus.Int() );

    RUBY_DEBUG1( "CNssContextMgr::RunL, iStatus = %d", err );
   
	switch( iState )
	    {
        case EGetContextClientGetContext:
            {
            iState = ENone;

            MNssContext* context = 0;

        	TRAP( err, (context = ((*iContextList)[0])->CopyL()) );
		    iContextList->ResetAndDestroy();

            delete iContextList;
            iContextList = 0;

            if ( iGetContextClient )
                {
                if ( context )
                    {
                    iGetContextClient->GetContextCompleted( context, KErrNone );
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                    iGetContextClient->GetContextCompleted( context );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                  
                    }
                else
                    {
                    iGetContextClient->GetContextCompleted( NULL, KErrGeneral );
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                    iGetContextClient->GetContextFailed( MNssGetContextClient::EVASDBFailure );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                  
                    }
                }
            else
                {
                delete context;
                }
            }
            break;

        case EGetContextClientGetContextList:
            {
            CArrayPtrFlat<MNssContext>* contextList = 
                    new CArrayPtrFlat<MNssContext>( KContextArrayGranularity );

            if ( contextList )
                {
                err = KErrNone;
                for( TInt k = 0; k < iContextList->Count(); k++ )
                    {
                    TRAP( err, contextList->AppendL( (*iContextList)[k] ) );
                    }

                delete iContextList;
                iContextList = 0;

                if ( err != KErrNone )
                    {
                    contextList->ResetAndDestroy();
                    delete contextList;
                    contextList = 0;
                    }
                }

            if ( iGetContextClient )
                {
                if ( contextList )
                    {
                    iGetContextClient->GetContextListCompleted( contextList, KErrNone );
                    }
                else{
                    iGetContextClient->GetContextCompleted( NULL, KErrGeneral );
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                        
                    iGetContextClient->GetContextFailed( MNssGetContextClient::EVASDBFailure );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY                  
                    }
                }
            else
                {
                contextList->ResetAndDestroy();
                delete contextList;
                contextList = NULL;
                }
            }
            break;

        case EInterCompContextMgrClientGetContext:
            {
            CArrayPtrFlat<CNssContext>* contextList = iContextList;
            iContextList = 0;

            if ( contextList )
                {
                iInterCompContextMgrClient->GetContextListCompleted(
                    contextList, NULL );
                }
            else{
                iInterCompContextMgrClient->GetContextCompleted(
                    NULL, KErrGeneral );
                }
            }
            break;

        default:
            {
            User::Panic( KContextMgrPanic, __LINE__ );
            }
            break;
	    }
    }

// ---------------------------------------------------------
// CNssContextMgr::RunError
// Cleanup function required by CActive
// ---------------------------------------------------------
//
TInt CNssContextMgr::RunError(TInt /*aError*/)
    {
    if ( iContextList )
        {
	    iContextList->ResetAndDestroy();
        delete iContextList;
        iContextList = 0;
        }

	Cancel();

    return KErrNone;
    }

// ---------------------------------------------------------
// CNssContextMgr::ResetFactoryModels
// Resets factory models
// ---------------------------------------------------------
//
TInt CNssContextMgr::ResetFactoryModels( MNssResetFactoryModelsClient* aClient )
    {
    if ( !aClient )
        {
        return KErrArgument;
        }

    if ( IsActive() )
        {
        return KErrNotReady;
        }

    iResetClient = aClient;

    // Old ID: We destroy the old (speaker adapted) model bank with this ID.
    TUint32 oldId;

    // New ID: When we create a new (speaker independent) model bank,
    //         we save the ID here.
    iModelBankId = KInvalidModelBankID;

    TInt err = iVasDatabase->GetDefaultModelBankId( oldId );

    if ( err != KErrNone )
        {
        return( err );
        }

    TRAP( err, iContextBuilder->GetContextPortal()->BeginResetModelsL( 
                        (TSIModelBankID)oldId, iModelBankId, this ) );

    if ( err != KErrNone )
        {
        return( err );
        }   

    return( KErrNone );
    }
