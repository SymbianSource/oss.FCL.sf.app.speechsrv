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
* Description:  CNssContextSrsPortal provides SRS related functionalities for 
*               context processing. A context is saved in the SRS DB by creating 
*               model bank, lexicon, and grammar into the SRS.
*
*/


// INCLUDE FILES
#include "nssvasccontextsrsportal.h"
#include "rubydebug.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssContextSrsPortal::CNssContextSrsPortal
// C++ default constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
//
CNssContextSrsPortal::CNssContextSrsPortal()
    {
    RUBY_DEBUG0( "CNssContextSrsPortal::CNssContextSrsPortal" );
    
    iTerminationState = CNssContextSrsPortal::EVASTERMINATION_NOT_PENDING;
    iState = EVASNone;
    iRecovery = EFalse;
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::~CNssContextSrsPortal
// Destructor
// ---------------------------------------------------------
//
CNssContextSrsPortal::~CNssContextSrsPortal()
    {
    RUBY_DEBUG0( "CNssContextSrsPortal::~CNssContextSrsPortal" );
    
    DeleteSRS();
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::MsruoEvent
// interface implementation from MSpeechRecognitionUtilityObserver
// ---------------------------------------------------------
//
void CNssContextSrsPortal::MsruoEvent( TUid aEvent, TInt aResult )
    {
    RUBY_DEBUG2( "CNssContextSrsPortal::MsruoEvent - Event=0x%x, Result=%d", aEvent.iUid, aResult );
    
    TInt err = KErrNone;
    switch ( aEvent.iUid )
        {
        case KUidAsrEventCreateGrammarVal:
            RUBY_DEBUG0( "CNssContextSrsPortal::MsruoEvent - CreateGrammar" );

            if ( aResult == KErrNone )
                {
                iContext->SetGrammarId( iGrammarId );
                iState = EVasWaitForCommitChanges;  // wait for commit
                iSrsDBEventHandler->HandleSaveSrsDBCompleted();
                }
            else
                {   
                CleanUp( aResult );
                }
            break;
            
            
        case KUidAsrEventCreateLexiconVal:
            RUBY_DEBUG0( "CNssContextSrsPortal::MsruoEvent - CreateLexicon" );
                 
            if (aResult == KErrNone)
                {
                iContext->SetLexiconId(iLexiconId);
                iState = EVASCreateGrammar;
                err = iSpeechRecognitionUtility->CreateGrammar(iGrammarId);
                if(err != KErrNone)
                    {
                    CleanUp(aResult);
                    }
                }
            else
                {
                CleanUp(aResult);
                }
            break;
            
            
        case KUidAsrEventCreateModelBankVal:
            RUBY_DEBUG0( "CNssContextSrsPortal::MsruoEvent - CreateModelBank" );

            // Two states may trigger this message:
            //  * When creating the first context
            //  * When resetting to factory settings
            
            // "Reset factory settings"
            if ( iState == EVASResetCreateModelBank )
                {
                if ( aResult == KErrNone )
                    {
                    iState = EVasWaitForCommitChanges;      // wait for commit
                    iSrsDBEventHandler->HandleResetSrsDBCompleted();
                    }
                else
                    {
                    CleanUp( aResult );
                    }
                }
            else
                {
                
                if ( aResult == KErrNone )
                    {
                    iContext->SetModelBankId(iModelBankId);
                    iState = EVASCreateLexicon;
                    err = iSpeechRecognitionUtility->CreateLexicon(iLexiconId);
                    if ( err != KErrNone )
                        {
                        CleanUp( aResult );
                        }
                    }
                else
                    {
                    CleanUp( aResult );
                    }
                }
            break;
            
            
        case KUidAsrEventRemoveGrammarVal:
            RUBY_DEBUG0( "CNssContextSrsPortal::MsruoEvent - RemoveGrammar" );
            
            if ( aResult == KErrNone || aResult == KErrNotFound )
                {
                iState = EVASRemoveLexicon;
                err = iSpeechRecognitionUtility->RemoveLexicon( 
                                (TSILexiconID) iContext->LexiconId() );
                if ( err != KErrNone )
                    {
                    CleanUp( aResult );
                    }
                }
            else
                {
                CleanUp( aResult );
                }
            break;
            
            // This case may never being called.
        case KUidAsrEventRemoveLexiconVal:
            RUBY_DEBUG0( "CNssContextSrsPortal::MsruoEvent - RemoveLexicon" );
             
            if ( aResult == KErrNone || aResult == KErrNotFound )
                {
                iState = EVasWaitForCommitChanges;      // wait for commit
                iSrsDBEventHandler->HandleDeleteSrsDBCompleted();
                }
            else
                {
                CleanUp( aResult );
                }
            break;
            
            
            // This case may never being called.
        case KUidAsrEventRemoveModelBankVal:
            RUBY_DEBUG0( "CNssContextSrsPortal::MsruoEvent - RemoveModelBank" );
             
            // "Reset factory settings": Remove speaker adapted model bank
            //                           and create a new one.
            if ( iState == EVASResetRemoveModelBank )
                {
                TInt createErr = KErrNone;
                
                if ( aResult == KErrNone )
                    {
                    iState = EVASResetCreateModelBank;
                    
                    createErr = iSpeechRecognitionUtility->CreateModelBank( *iResetModelBankId );
                    }
                
                // If RemoveModelBank failed, or CreateModelBank 5 lines age failed.
                if ( aResult != KErrNone || createErr != KErrNone )
                    {
                    DeleteSRS();
                    iSrsDBEventHandler->HandleSrsDBError( 
                            MNssCoreSrsDBEventHandler::EVasRemoveModelBankFailed );
                    }
                }
            else
                {
                // Legacy code from SDND - I don't know when this is invoked
                if ( aResult == KErrNone || aResult == KErrNotFound )
                    {
                    iState = EVasWaitForCommitChanges;      // wait for commit
                    
                    DeleteSRS();
                    iSrsDBEventHandler->HandleDeleteSrsDBCompleted();	
                    }
                else
                    {
                    CleanUp( aResult );
                    }
                }
            break;
            
            
            
        default:
            RUBY_DEBUG0( "CNssContextSrsPortal::MsruoEvent - default" );
            CleanUp( aResult );
            break;
    }
}

// -----------------------------------------------------------------------------
// CNssContextSrsPortal::DoSrsDBResult
// Method to process the SRS DB result.
// -----------------------------------------------------------------------------
//
MNssCoreSrsDBEventHandler::TNssSrsDBResult 
CNssContextSrsPortal::DoSrsDBResult(TInt aResult)
    {
    RUBY_DEBUG1( "CNssContextSrsPortal::DoSrsDBResult() aResult = %d", aResult );
    
    MNssCoreSrsDBEventHandler::TNssSrsDBResult errorCode;
    
    switch (aResult)
        {
        case KErrDiskFull:
            RUBY_DEBUG0( "CNssContextSrsPortal::DoSrsDBResult - KErrDiskFull" );
            
            errorCode = MNssCoreSrsDBEventHandler::EVasDiskFull;
            break;
        case KErrNoMemory:
            RUBY_DEBUG0( "CNssContextSrsPortal::DoSrsDBResult - KErrNoMemory" );
            
            errorCode = MNssCoreSrsDBEventHandler::EVasNoMemory;
            break;
        default:
            {
            RUBY_DEBUG1( "CNssContextSrsPortal::DoSrsDBResult - default, state = %d", iState );
            
            switch ( iState )
                {
                case EVASCreateModelBank:
                    errorCode = MNssCoreSrsDBEventHandler::EVasCreateModelBankFailed;
                    break;
                case EVASCreateLexicon:
                    errorCode = MNssCoreSrsDBEventHandler::EVasCreateLexiconFailed;
                    break;
                case EVASCreateGrammar:
                    errorCode = MNssCoreSrsDBEventHandler::EVasCreateGrammarFailed;
                    break;
                case EVASRemoveModelBank:
                    errorCode = MNssCoreSrsDBEventHandler::EVasRemoveModelBankFailed;
                    break;
                case EVASRemoveLexicon:
                    errorCode = MNssCoreSrsDBEventHandler::EVasRemoveLexiconFailed;
                    break;
                case EVASRemoveGrammar:
                    errorCode = MNssCoreSrsDBEventHandler::EVasRemoveGrammarFailed;
                    break;
                default:
                    errorCode = MNssCoreSrsDBEventHandler::EVasDBOperationError;
                    break;
                }
            }
        }
    
    return errorCode;
    }



// ---------------------------------------------------------
// CNssContextSrsPortal::BeginSaveContextL
// save context to srs database
// ---------------------------------------------------------
//
void CNssContextSrsPortal::BeginSaveContextL( CNssContext* aContext, 
                                              MNssCoreSrsDBEventHandler* aSrsDBEventHandler )
    {
    RUBY_DEBUG_BLOCK( "CNssContextSrsPortal::BeginSaveContextL" );
    
    iSrsDBEventHandler = aSrsDBEventHandler;
    iContext = aContext;
    
    // verify the SRS is NULL, if not return failure
    if ( iSpeechRecognitionUtility )
        {
        User::Leave( KErrGeneral );
        }
    
    RUBY_DEBUG0( "CNssContextSrsPortal::CREATE SRS" );
    
    iSpeechRecognitionUtility = CNssSiUtilityWrapper::NewL( *this, KNssVASApiUid );
    
    TInt error;
    iRecovery = EFalse;
    
    // if creating context first time, create model bank and lexicon
    if ( !aContext->ModelBankAndLexiconExist() ) 
        {
        iState = EVASCreateModelBank;
        error = iSpeechRecognitionUtility->CreateModelBank(iModelBankId);
        if ( error != KErrNone )
            {
            DeleteSRS();
            User::Leave( error );
            }
        }
    else // Create grammar and lexicon
        {
        iState = EVASCreateLexicon;
        error = iSpeechRecognitionUtility->CreateLexicon(iLexiconId);
        if ( error != KErrNone )
            {
            DeleteSRS();
            User::Leave( error );
            }
        }
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::BeginDeleteContextL
// deletes context from SRS. 
// ---------------------------------------------------------
//
void CNssContextSrsPortal::BeginDeleteContextL(CNssContext* aContext, 
                                MNssCoreSrsDBEventHandler* aSrsDBEventHandler)
    {
    RUBY_DEBUG_BLOCK( "CNssContextSrsPortal::BeginDeleteContextL" );
    
    iSrsDBEventHandler = aSrsDBEventHandler;
    iContext = aContext;
    iRecovery = EFalse;
    
    // verify the SRS is NULL, if not return failure
    if ( iSpeechRecognitionUtility )
        {
        User::Leave( KErrGeneral );
        }

    RUBY_DEBUG0( "CNssContextSrsPortal::CREATE SRS" );
     
    iSpeechRecognitionUtility = CNssSiUtilityWrapper::NewL( *this, KNssVASApiUid );
    iState = EVASRemoveGrammar;
    
    TInt error = iSpeechRecognitionUtility->RemoveGrammar( (TSIGrammarID)aContext->GrammarId() );
    if ( error != KErrNone )
        {
        DeleteSRS();
        User::Leave( error );
        }
    
    // Lexicon will be deleted when grammar deletion callback arrives
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::CommitSaveContext
// commit context changes from SRS. 
// ---------------------------------------------------------
//
TInt CNssContextSrsPortal::CommitSaveContext()
    {
    RUBY_DEBUG0( "CNssContextSrsPortal::CommitSaveContext" );
    
    TInt ret( KErrNone );
    
    if ( iState == EVasWaitForCommitChanges )
        {
        TInt error = iSpeechRecognitionUtility->CommitChanges();
        
        if ( error != KErrNone )
            {
            DeleteSRS();
            ret = error;
            }
        }
    else
        {
        DeleteSRS();
        ret = KErrGeneral;
        }
    
    DeleteSRS();
    iState = EVASNone;      // reset the state
    
    return ret;	
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::RollbackSaveContext
// commit context changes from SRS. 
// ---------------------------------------------------------
//
TInt CNssContextSrsPortal::RollbackSaveContext()
    {
    RUBY_DEBUG0( "CNssContextSrsPortal::RollbackSaveContext" );

    if (iState != EVasWaitForCommitChanges)
        {
        DeleteSRS();
        return KErrGeneral;
        }
    
    DeleteSRS();
    iState = EVASNone;      // reset the state
    return KErrNone;	
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::DeleteSRS
// Delete the SRS, and set it to NULL
// ---------------------------------------------------------
//
void CNssContextSrsPortal::DeleteSRS()
    {
    RUBY_DEBUG0( "CNssContextSrsPortal::DeleteSRS" );
    
    if ( iSpeechRecognitionUtility )
        {
        RUBY_DEBUG0( "CNssContextSrsPortal::deleted SRS........" );        
        
        delete iSpeechRecognitionUtility; 
        iSpeechRecognitionUtility = NULL;
        
        RUBY_DEBUG0( "CNssContextSrsPortal::.....SRS just deleted" );
        }      
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::CleanUp
// Clean up after SRS operation fail
// ---------------------------------------------------------
//
void CNssContextSrsPortal::CleanUp( TInt aResult )
    {
    RUBY_DEBUG0( "CNssContextSrsPortal::CleanUp" );

    DeleteSRS();
    
    MNssCoreSrsDBEventHandler::TNssSrsDBResult errorCode = DoSrsDBResult( aResult );
    iSrsDBEventHandler->HandleSrsDBError( errorCode );            
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::Register
// context portal keeps a count of number of contexts created
// increment count
// ---------------------------------------------------------
//
void CNssContextSrsPortal::Register()
    {
    iContextCount++;
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::Deregister
// decrement count
// id count is zero, and context builder is deleted, delete itself (portal),
// ---------------------------------------------------------
//
void CNssContextSrsPortal::Deregister()
    {
    iContextCount--;
    if ( iContextCount == 0 && iTerminationState == EVASTERMINATION_PENDING )
        {
        delete this;
        }
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::ContextCount
// returns context count
// ---------------------------------------------------------
//
TInt CNssContextSrsPortal::ContextCount()
    {
    return iContextCount;
    }

// ---------------------------------------------------------
// CNssContextSrsPortal::SetTerminationState
// sets termination state
// ---------------------------------------------------------
//
void CNssContextSrsPortal::SetTerminationState( TNssTerminationState aState )
   {
	iTerminationState = aState;
   }

// ---------------------------------------------------------
// CNssContextSrsPortal::BeginResetModelsL
// sets termination state
// ---------------------------------------------------------
//
void CNssContextSrsPortal::BeginResetModelsL( TSIModelBankID aAdaptedModels,
                                              TSIModelBankID& aNewModels,
                                              MNssCoreSrsDBEventHandler* aSrsDBEventHandler )
    {
    RUBY_DEBUG_BLOCK( "CNssContextSrsPortal::BeginSaveContextL" );
    
    iSrsDBEventHandler = aSrsDBEventHandler;
    
    // verify the SRS is NULL, if not return failure
    if ( iSpeechRecognitionUtility )
        {
        User::Leave( KErrGeneral );
        }
    
    RUBY_DEBUG0( "CNssContextSrsPortal::CREATE SRS" );
    
    iSpeechRecognitionUtility = CNssSiUtilityWrapper::NewL( *this, KNssVASApiUid );
    TInt error = KErrNone;
    iRecovery = EFalse;
    
    iState = EVASResetRemoveModelBank;
    error = iSpeechRecognitionUtility->RemoveModelBank( aAdaptedModels );
    
    if ( error != KErrNone )
        {
        DeleteSRS();
        User::Leave( error );
        }
    
    iResetModelBankId = &aNewModels;
    }

//  End of File  
