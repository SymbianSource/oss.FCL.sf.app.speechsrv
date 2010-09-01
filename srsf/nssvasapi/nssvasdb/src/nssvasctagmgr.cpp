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
* Description:  CNssTagMgr implements the MNssTagMgr interface. In addition, it also provides
*               methods for internal use by VAS components.
*
*/


// includes
#include "nssvasctagmgr.h"
#include "nssvasccontextmgr.h"
#include "nssvascvasdatabase.h"
#include "nssvascspeechitembuilder.h"
#include "rubydebug.h"

_LIT( KTagMgrPanic, "tagmgr.cpp" );

// -----------------------------------------------------------------------------
// CNssTagMgr::CNssTagMgr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssTagMgr::CNssTagMgr()
: CActive( CActive::EPriorityStandard )
    {
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::CNssTagMgr
// C++ overloaded constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssTagMgr::CNssTagMgr(CNssVASDatabase* aVasDatabase, CNssContextMgr *aContextManager)
: CActive( CActive::EPriorityStandard ), iTagList( 0 ), iTagRefList( 0 )
    {
    iVasDatabase = aVasDatabase;
    iContextManager = aContextManager;
    iState = ENone;
    }

// destructor
CNssTagMgr::~CNssTagMgr()
    {
    Cancel();

    if( iTagList ) 
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        }
    if ( iSpeechItemBuilder )
        {
        delete iSpeechItemBuilder;
        iSpeechItemBuilder = NULL;
        }
    
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssTagMgr* CNssTagMgr::NewL( CNssVASDatabase* aVasDatabase, CNssContextMgr *aContextManager )
    {
    CNssTagMgr* self = NewLC(aVasDatabase, aContextManager);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssTagMgr* CNssTagMgr::NewLC( CNssVASDatabase* aVasDatabase, CNssContextMgr *aContextManager )
    {
    CNssTagMgr* self = new(ELeave) CNssTagMgr(aVasDatabase, aContextManager);
    CleanupStack::PushL( self );
    self->ConstructL();
    return ( self );
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::ConstructL
// EPOC constructor.
// -----------------------------------------------------------------------------
//
void CNssTagMgr::ConstructL()
    {
    iSpeechItemBuilder = CNssSpeechItemBuilder::NewL( iVasDatabase );
    
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::CreateTagL
// creates a new tag, taking in the context for the tag
// -----------------------------------------------------------------------------
//
MNssTag* CNssTagMgr::CreateTagL( MNssContext* aContext )
    {
    if ( !aContext )
        {
        User::Leave( KErrArgument );
        }
    
    CNssContext *context = ((CNssContext*)(aContext))->CopyL();
    CleanupStack::PushL(context);
    CNssSpeechItem *speechItem = iSpeechItemBuilder->CreateEmptySpeechItemL(*context);
    CleanupStack::PushL(speechItem);
    CNssRRD* rrd = CNssRRD::NewL(); 
    CleanupStack::PushL(rrd);
    CNssTag *tag = new (ELeave) CNssTag(context, rrd, speechItem);
    CleanupStack::Pop(rrd);
    CleanupStack::Pop(speechItem);
    CleanupStack::Pop(context);
    return tag;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::CreateTagL
// gets a tag list from VAS DB for a given context
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::GetTagList( MNssGetTagClient* aTagClient,MNssContext* aContext )
    {
    if ( IsActive() || !aTagClient || !aContext )
        {
        return KErrGeneral;
        }
    
    iState = EGetTagClient;
    iGetTagClient = aTagClient;
    
    if( iTagList ) 
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = NULL;
        }
    
    iTagList = iVasDatabase->GetTag((CNssContext*)(aContext));
    
    if ( !iTagList || iTagList->Count() == 0 )
        {
        return KErrGeneral;
        }
    
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete(pRS, KErrNone);
    SetActive();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::GetTagList
// gets a tag list from VAS DB for a given name
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::GetTagList( MNssGetTagClient* aTagClient, 
                             MNssContext* aContext, const TDesC& aName )
    {
    if ( IsActive() || !aTagClient || 
         !aContext || aName.Length() == 0 )
        {
        return KErrGeneral;
        }
    
    iState = EGetTagClient;
    iGetTagClient = aTagClient;
    
    if( iTagList ) 
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = NULL;
        }
    
    iTagList = iVasDatabase->GetTag((CNssContext*)(aContext), aName);
    
    if ( !iTagList || iTagList->Count() == 0 )
        {
        return KErrGeneral;
        }
    
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete(pRS, KErrNone);
    SetActive();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::DeleteTag
// deletes a tag from VAS 
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::DeleteTag( MNssDeleteTagClient* aTagClient, MNssTag* aTag )
    {
    if ( IsActive() || !aTagClient || !aTag )
        {
        return KErrGeneral;
        }

    iState = EDeleteTagClient;
	iDeleteTagClient = aTagClient;
	iLocalTag = (CNssTag*) (aTag);
	iLocalSpeechItem = (CNssSpeechItem*) (iLocalTag->SpeechItem());

    TInt ret( KErrNone );

   	MNssCoreSrsDBEventHandler::TNssSrsDBResult srsDBResult = 
   	                    MNssCoreSrsDBEventHandler::EVasSuccess;
    TRAPD( err, (srsDBResult = 
           iLocalSpeechItem->NSSBeginDeleteFromSrs( iDeleteTagClient, iLocalTag )) );

	if(err != KErrNone || srsDBResult != MNssCoreSrsDBEventHandler::EVasSuccess)
        {
		ret = KErrGeneral;
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::SaveTag
// saves a tag to VAS 
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::SaveTag( MNssSaveTagClient* aTagClient, MNssTag* aTag )
    {
    if ( IsActive() || !aTagClient || !aTag )
        {
        return KErrGeneral;
        }
    
    // Client can save only trained tags.
    if ( aTag->SpeechItem()->TrainedType() == TNssVasCoreConstant::EVasNotTrained )
        {
        return KErrNotReady;
        }
    
    iSaveTagClient = aTagClient;
    
    iState = ESaveTagClient;
    
    /********************* Debug rule ID and grammar ID ***********************/
    /*
    RFs session;
    RFile file;
    _LIT( aFileName, "c:\\documents\\savetag.txt" );
    
      session.Connect();
      if ( file.Open( session, aFileName, EFileWrite ) != KErrNone )
      {
      if ( file.Create( session, aFileName, EFileWrite ) != KErrNone )
      {
      User::Panic( _L("SDContrlDebug"), __LINE__ );
      }
      }
      
        TInt size = 0;
        file.Seek( ESeekEnd, size );
        
          TBuf8<100> debugBuf;
          debugBuf.AppendFormat( _L8("Saving tag with RID:%d, GID:%d\n"), ((CNssSpeechItem*)aTag->SpeechItem())->RuleID(),((CNssContext*)aTag->Context())->GrammarId() );
          file.Write( debugBuf );
          
            file.Close();
            session.Close();
    */
    /**************************************************************************/
    TInt ret( KErrNone );
    CNssTag* tag = (CNssTag*)aTag;
    TInt err;
    
    iLocalSpeechItem = (CNssSpeechItem*)tag->SpeechItem();
    err = iLocalSpeechItem->NSSBeginSaveToSrs( aTagClient, tag );
    
    if ( err != KErrNone )
        {
        ret = KErrGeneral;
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::GetTag
// get tag from VAS DB based on grammar id and rule id
// -----------------------------------------------------------------------------
//
CNssTag* CNssTagMgr::GetTag( TUint32 aGrammarId, TUint32 aRuleId )
    {
    MNssTag* result = 0;
    
    if( iTagList ) 
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = NULL;
        }
    
    iTagList = iVasDatabase->GetTag( aGrammarId, aRuleId );
    
    // If there are tags in the list, return the first one.
    if ( iTagList != 0 )
        {
        if ( iTagList->Count() > 0 )
            {
            result = (*iTagList)[0];
            
            iTagList->Delete( 0 );
            
            __ASSERT_DEBUG( iTagList->Count() == 0, User::Panic( KTagMgrPanic, __LINE__ ) );
            }
        
        // Release the memory
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = 0;
        }
    
    // Cast is safe, since only CNssTag implements MNssTag.
    return( STATIC_CAST( CNssTag*, result ) );
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::GetTags
// get tags from VAS DB based on theirgrammar id and rule id
// -----------------------------------------------------------------------------
MNssTagListArray* CNssTagMgr::GetTags( TNssGrammarIdRuleIdListArray&  aGrammarIdRuleIds)
    {
    MNssTagListArray* result = iVasDatabase->GetTags( aGrammarIdRuleIds );
    return result;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::VASDatabaseComplete
// call back from CNssVASDatabase 
// -----------------------------------------------------------------------------
//
void CNssTagMgr::VASDatabaseComplete( CArrayPtrFlat<CNssContext>* /*aContextList*/, 
                                      CArrayPtrFlat<CNssTag>* /*aTagList*/, 
                                      CArrayPtrFlat<CNssTagReference>* /*aTagRefList*/,
                                      MNssVASDatabaseClient::TNssVASDBClientReturnCode /*aCode*/ )
    {
    User::Panic( KTagMgrPanic, __LINE__ );
    }
 
// -----------------------------------------------------------------------------
// CNssTagMgr::HandleSaveSrsDBCompleted
//  call back from SRS DB 
// -----------------------------------------------------------------------------
//
void CNssTagMgr::HandleSaveSrsDBCompleted()
    {
    TInt newTagId;
    
    CNssSpeechItem* item = (CNssSpeechItem*)iLocalTag->SpeechItem();
    
    // Save tag to VAS.
    TInt ret = iVasDatabase->SaveTag( iLocalTag, newTagId );
    
    // If VAS DB saving failed, roll back Plugin DB changes.
    if ( ret != KErrNone )
        {
        CNssSpeechItem* item = (CNssSpeechItem*)iLocalTag->SpeechItem();
        item->RollbackSrsChanges();
        
        if(iSaveTagClient)
            {
            iSaveTagClient->SaveTagCompleted( ret );
            }
        return;
        }
    
    MNssCoreSrsDBEventHandler::TNssSrsDBResult srsret = item->CommitSrsChanges();
    
    if ( srsret == EVasSuccess )
        {
        if(iSaveTagClient)
            {
            iSaveTagClient->SaveTagCompleted( KErrNone );
            }
        }
    else{
        if(iSaveTagClient)
            {
            iSaveTagClient->SaveTagCompleted( KErrGeneral );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::HandleDeleteSrsDBCompleted
//  call back from SRS DB 
// -----------------------------------------------------------------------------
//
void CNssTagMgr::HandleDeleteSrsDBCompleted()
    {
    iState = ENone;

    TInt ret = iVasDatabase->DeleteTag( iLocalTag );

    //                      trained, not saved -case
    if ( ret != KErrNone && ret != KErrNotFound )
        {
        iLocalSpeechItem->RollbackSrsChanges();

        iDeleteTagClient->DeleteTagCompleted( ret );
        return;
        }

    if ( iLocalSpeechItem->CommitSrsChanges() != MNssCoreSrsDBEventHandler::EVasSuccess )
        {
        iDeleteTagClient->DeleteTagCompleted( KErrGeneral );
        return;
        }

    iDeleteTagClient->DeleteTagCompleted( KErrNone );
    return;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::HandleResetSrsDBCompleted
//  call back from SRS DB 
// -----------------------------------------------------------------------------
//
void CNssTagMgr::HandleResetSrsDBCompleted()
    {
    // Should never be called
    RUBY_DEBUG0( "CNssTagMgr::HandleResetSrsDBCompleted - ERROR" );
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::HandleSrsDBError
//  call back from SRS DB 
// -----------------------------------------------------------------------------
//
void CNssTagMgr::HandleSrsDBError( MNssCoreSrsDBEventHandler::TNssSrsDBResult /*aResult*/ )
    {
    
    switch(iState)
        {
        case EDeleteTagClient:
            iState = ENone;
            iDeleteTagClient->DeleteTagCompleted( KErrGeneral );
            break;
        case ESaveTagClient:
            iState = ENone;
            iSaveTagClient->SaveTagCompleted( KErrGeneral );
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::CNssTagListToMNssTagListConvertorL
//  utility function for CNssTag list to MNssTag list conversion, and call back to client
// -----------------------------------------------------------------------------
//
void CNssTagMgr::CNssTagListToMNssTagListConvertorL(CArrayPtrFlat<CNssTag>* aSourceList)
    {
    CArrayPtrFlat<MNssTag> *destinationList = new (ELeave) CArrayPtrFlat<MNssTag>(1);
    CleanupStack::PushL(destinationList);
    for(TInt i = 0; i < aSourceList->Count(); i++)
        {
        destinationList->AppendL( (MNssTag*) ((*aSourceList)[i]) );
        }
    aSourceList->Reset();
    delete aSourceList; 
    CleanupStack::Pop(destinationList);
    iGetTagClient->GetTagListCompleted(destinationList, KErrNone);
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY    
    iGetTagClient->GetTagListCompleted( destinationList );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY  
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::CancelGetTag
// cancel get tag for inter-comp clients
// -----------------------------------------------------------------------------
//
void CNssTagMgr::CancelGetTag()
    {
    iInterCompTagMgrClient = NULL;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::TagCount
// gets a tag count from VAS DB for a given context
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::TagCount( MNssContext* aContext )
    {
    return iVasDatabase->TagCount( (CNssContext*)aContext );
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::Tag
// gets a tag from VAS DB based on rrd int value and position
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::GetTagList(MNssGetTagClient* aTagClient,MNssContext* aContext,
                            TInt aNum,TInt aPosition)
    {
    if ( IsActive() || !aTagClient || !aContext
         || aPosition < 0 )
        {
        return KErrGeneral;
        }
    iState = EGetTagClient;
    iGetTagClient = aTagClient;
    
    if( iTagList ) 
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = NULL;
        }
    
    iTagList = iVasDatabase->GetTagList( (CNssContext*)aContext, aNum, aPosition );
    
    if ( !iTagList || iTagList->Count() == 0 )
        {
        return KErrGeneral;
        }
    
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete(pRS, KErrNone);
    SetActive();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::GetTagList
// gets a tag from VAS DB based on rrd text value and position
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::GetTagList( MNssGetTagClient* aTagClient,MNssContext* aContext,
                             TDesC& aText,TInt aPosition )
    {
    if ( IsActive() || !aTagClient || !aContext ||
        aText.Length() == 0 || aPosition < 0 )
        {
        return KErrGeneral;
        }
    
    iState = EGetTagClient;
    iGetTagClient = aTagClient;
    
    if( iTagList ) 
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = NULL;
        }
    
    iTagList = iVasDatabase->GetTagList( (CNssContext*)aContext, aText, aPosition );
    
    if ( !iTagList || iTagList->Count() == 0 )
        {
        return KErrGeneral;
        }
    
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete(pRS, KErrNone);
    SetActive();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::GetTagReferenceList
// get a list of tag references from VAS DB based on a context
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::GetTagReferenceList( MNssGetTagReferenceClient* aTagRefClient,
                                      MNssContext* aContext)
    {
    if ( IsActive() || !aTagRefClient || !aContext )
        {
        return KErrGeneral;
        }
    
    iState = EGetTagReferenceClient;
    iGetTagReferenceClient = aTagRefClient;
    
    if ( iTagRefList )
        {
        iTagRefList->ResetAndDestroy();
        delete iTagRefList;
        iTagRefList = NULL;
        }
    
    iTagRefList = iVasDatabase->GetTagReferenceList( (CNssContext*)aContext );
    
    if ( !iTagRefList || iTagRefList->Count() == 0 )
        {
        return KErrGeneral;
        }
    
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete(pRS, KErrNone);
    SetActive();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::GetTag
// get a tag from VAS DB based on tagreference (using the TagId of the tagreference)
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::GetTag(MNssGetTagClient* aTagClient,MNssTagReference* aTagReference)
    {
    CNssTagReference* ctagRef = (CNssTagReference*) aTagReference;
    
    if ( IsActive() || !aTagClient || !aTagReference )
        {
        return KErrGeneral;
        }
    
    iState = EGetTagClient;
    iGetTagClient = aTagClient;
    
    if( iTagList ) 
        {
        iTagList->ResetAndDestroy();
        delete iTagList;
        iTagList = NULL;
        }
    
    iTagList = iVasDatabase->GetTag( ctagRef->TagId() );
    
    if ( !iTagList || iTagList->Count() == 0 )
        {
        return KErrGeneral;
        }
    
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete(pRS, KErrNone);
    SetActive();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::CNssTagRefListToMNssTagRefListConvertorL
// utility function for CNssTagReference list to MNssTagReference list conversion, 
// and call back to client
// -----------------------------------------------------------------------------
//
void CNssTagMgr::CNssTagRefListToMNssTagRefListConvertorL(
                    CArrayPtrFlat<CNssTagReference>* aSourceList )
    {
    
    CArrayPtrFlat<MNssTagReference> *destinationList = 
                            new (ELeave) CArrayPtrFlat<MNssTagReference>(1);
    CleanupStack::PushL(destinationList);
    for(TInt i = 0; i < aSourceList->Count(); i++)
        {
        destinationList->AppendL( (MNssTagReference*) ((*aSourceList)[i]) );
        }
    aSourceList->Reset();
    delete aSourceList; 
    CleanupStack::Pop(destinationList);
    iGetTagReferenceClient->GetTagReferenceListCompleted( destinationList,
                                                          KErrNone );
    }


/************ ACTIVE OBJECT *************/

// -----------------------------------------------------------------------------
// CNssTagMgr::DoCancel
//
// -----------------------------------------------------------------------------
//
void CNssTagMgr::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::RunL
//
// -----------------------------------------------------------------------------
//
void CNssTagMgr::RunL()
    {
	switch (iState)
	    {
        case ESaveTagClient:
            iSaveTagClient->SaveTagCompleted( KErrNone );
            break;

        case EGetTagClient:
            {
            MNssTagListArray* ret = iTagList;
            iTagList = 0;

            iGetTagClient->GetTagListCompleted( ret, KErrNone );
#ifndef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY    
            iGetTagClient->GetTagListCompleted( ret );
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY              
            }
            break;

        case EGetTagReferenceClient:
            {
            CArrayPtrFlat<MNssTagReference>* ret = iTagRefList;
            iTagRefList = 0;

            iGetTagReferenceClient->GetTagReferenceListCompleted( ret, KErrNone );
            }
            break;

        default:
            User::Panic( KTagMgrPanic, __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// CNssTagMgr::RunError
//
// -----------------------------------------------------------------------------
//
TInt CNssTagMgr::RunError(TInt /*aError*/)
{
	Cancel();

    return KErrNone;
}
