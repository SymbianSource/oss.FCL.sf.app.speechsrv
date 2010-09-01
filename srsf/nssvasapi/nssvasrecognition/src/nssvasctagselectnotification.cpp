/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*               register and deregister for a voice tag select event. 
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "nssvasctagselectnotification.h"
#include "nssvasmtagselecthandler.h"
#include "nssvasctagselectdata.h"
#include "rubydebug.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CNssTagSelectNotification::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssTagSelectNotification* CNssTagSelectNotification::NewL()
    {
    // version of NewLC which leaves nothing on the cleanup stack
    CNssTagSelectNotification* self=NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNssTagSelectNotification* CNssTagSelectNotification::NewLC()
    {
    // NewLC with two stage construct
    CNssTagSelectNotification* self=new (ELeave) CNssTagSelectNotification();
    // get new, leave if can't
    CleanupStack::PushL(self);
    // push onto cleanup stack
    // (in case self->ConstructL leaves)
    self->ConstructL();      // use two-stage construct
    return self;
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::~CNssTagSelectNotification
// Destructor
// -----------------------------------------------------------------------------
//
CNssTagSelectNotification::~CNssTagSelectNotification()
    {
    RUBY_DEBUG0( "CNssTagSelectNotification::~CNssTagSelectNotification" );
    
    if ( iDBTagSelectNotifier ) 
        {
        iDBTagSelectNotifier->Deregister( this );
        }
    
    if ( iTagSelectDataList )
        {
        iTagSelectDataList->ResetAndDestroy();
        delete iTagSelectDataList;
        }
    CNssVASDBBuilder::RemoveInstance();  // remove iVasDBBuilder instance
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::RegisterL
// Method to register for voice tag select notification.
// -----------------------------------------------------------------------------
//
MNssTagSelectNotification::TNssSelectResult CNssTagSelectNotification::RegisterL(
                                         MNssContext* aContext,
                                         MNssTagSelectHandler* aTagSelectHandler )
    {
    RUBY_DEBUG_BLOCK( "CNssTagSelectNotification::RegisterL" );
    
    if ( iTagSelectDataList->Count() == 0 )
        {
        // register ourself
        TInt dbErrorCode;
        dbErrorCode = iDBTagSelectNotifier->RegisterForNotification( this );
        if ( dbErrorCode != KErrNone )
            {
            return EVasSelectFailed;
            }
        }
    CNssTagSelectData* tagSelectData = CNssTagSelectData::NewL( (CNssContext *) aContext,
        aTagSelectHandler );
    
    CleanupStack::PushL(tagSelectData);  
    iTagSelectDataList->AppendL( tagSelectData );
    CleanupStack::Pop(tagSelectData);  // do not destroy tagSelectData; it is appended
    return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::Deregister
// Method to deregister from voice tag select notification
// -----------------------------------------------------------------------------
//
MNssTagSelectNotification::TNssSelectResult CNssTagSelectNotification::Deregister(
                                          MNssContext* aContext,
                                          MNssTagSelectHandler* aTagSelectHandler )
    {
    RUBY_DEBUG0( "CNssTagSelectNotification::Deregister" );
   
    // if there is no registration, fail the deregister
    if ( iTagSelectDataList->Count() == 0 )
        {
        return EVasSelectFailed;
        }
    
    // loop through the TagSelectDataList entries
    // for each context match, check if the event handler also matches
    // if yes, delete the entry
    for ( TInt index=0; index < iTagSelectDataList->Count(); index++ )
        {
        
        TBool match = CompareContexts( (CNssContext *) aContext, index );
        
        MNssTagSelectHandler* tagSelectHandler;
        tagSelectHandler = iTagSelectDataList->At( index )->TagSelectHandler();
        
        if ( ( match == (TInt)ETrue ) &&
            (aTagSelectHandler == tagSelectHandler))	
            {
            CNssTagSelectData* tagSelectData = iTagSelectDataList->At( index ); 
            iTagSelectDataList->Delete( index ); 
            delete tagSelectData;
            break;  // exit the for loop
            }
        }
    if ( iTagSelectDataList->Count() == 0 )
        {
        // deregister ourself
        TInt dbErrorCode;
        dbErrorCode = iDBTagSelectNotifier->Deregister( this );
        if ( dbErrorCode != KErrNone )
            {
            RUBY_DEBUG0( "CNssTagSelectNotification, DBTagSelectNotifier->Deregister failed" );
            }
        }
    return EVasErrorNone;
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::HandleSelection
// Method called into the client by CNssDBTagSelectNotifier
// -----------------------------------------------------------------------------
//
void CNssTagSelectNotification::HandleSelection(CNssTag* aTag)
    {
    RUBY_DEBUG0( "CNssTagSelectNotification::HandleSelection" );

    // loop through the TagSelectDataList entries
    // for each context match, issue a HandleTagSelect response event
    for ( TInt index=0; index < iTagSelectDataList->Count(); index++ )
        {
        TBool result = CompareContexts((CNssContext *)(aTag->Context()), index);
        if ( result == (TInt)ETrue )
            {
            // make a copy of the tag for each client which registered for
            // the context of aTag
            // the client is responsible for deleting clientTag
            CNssTag* clientTag=NULL; 
            TRAPD( err, ( clientTag = aTag->CopyL() ) );
            if ( err != KErrNone )
                {
                RUBY_DEBUG0( "CNssTagSelectNotification: TRAPD CNssTag CopyL failed" );
                return;
                }
            
            iTagSelectDataList->At( index )->TagSelectHandler()->
                HandleTagSelect( (MNssTag*)clientTag );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::CNssTagSelectNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssTagSelectNotification::CNssTagSelectNotification()
    {
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNssTagSelectNotification::ConstructL()
    {	
    iTagSelectDataList = new (ELeave) CArrayPtrFlat<CNssTagSelectData>(1);
    
    iVasDBBuilder = CNssVASDBBuilder::InstanceL();
    
    iDBTagSelectNotifier = iVasDBBuilder->GetTagSelectNotifier();
    }

// -----------------------------------------------------------------------------
// CNssTagSelectNotification::CompareContexts
// Method to compare a context with a TagSelectDataList entry context.
// -----------------------------------------------------------------------------
//
TBool CNssTagSelectNotification::CompareContexts( CNssContext* aContext, TInt aIndex )
    {
    CNssContext* context;
    context = iTagSelectDataList->At( aIndex )->Context();
    
    if ( context->ContextName().Compare( aContext->ContextName() )==0 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// End of file
