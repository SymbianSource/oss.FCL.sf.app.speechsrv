/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the main implementation of the SI Controller Plugin.
*               Finite State Machine is implemented here.
*
*/


// INCLUDE FILES
#include <mmfcodec.h>
#include <nsssispeechrecognitiondatadevasr.h>

#include <badesca.h>
#include <mmfbase.h>

#include "sicontrollerplugin.h"
#include "sicontrollerplugininterface.h"
#include "sidatabase.h"

#include "siresourcehandler.h"
#include "asrplugindataloader.h"

#include "sigrammardb.h"
#include "silexicondb.h"
#include "simodelbankdb.h"

#include "rubydebug.h"
 
#ifdef __SINDE_TRAINING
#include "sindetraining.h"
#endif // __SINDE_TRAINING
 
// Package types for general & language specific TTP data
 
// CONSTANTS
 
// Message types used in RunL
//const TInt KAddPronunciation			= 1; // Commented out to remove "not referenced" warning
const TInt KAddRule						= 2;
const TInt KCreateGrammar				= 3;
const TInt KCreateLexicon				= 4;
const TInt KCreateModelBank				= 5;
const TInt KGetAllClientGrammarIDs		= 6;
const TInt KGetAllClientLexiconIDs		= 7;
const TInt KGetAllClientModelBankIDs	= 8;
const TInt KGetAllGrammarIDs			= 9;
const TInt KGetAllLexiconIDs			= 10;
const TInt KGetAllModelBankIDs			= 11;
const TInt KGetAllModelIDs				= 12;
const TInt KGetAllPronunciationIDs		= 13;
const TInt KGetAllRuleIDs				= 14;
const TInt KGetAvailableStorage			= 15;
const TInt KGetModelCount				= 16;
const TInt KGetRuleValidity				= 17;
const TInt KGetUtteranceDuration		= 18;
const TInt KLoadGrammar					= 19;
const TInt KLoadLexicon					= 20;
const TInt KLoadModels					= 21;
const TInt KPlayUtterance				= 22;
const TInt KRecognize					= 23;
const TInt KRecord						= 24;
const TInt KRemoveGrammar				= 25;
const TInt KRemoveLexicon				= 26;
const TInt KRemoveModelBank				= 27;
const TInt KRemoveModel					= 28;
const TInt KRemovePronunciation			= 29;
const TInt KRemoveRule					= 30;
const TInt KTrain						= 31;
const TInt KUnloadRule					= 32;

// SI only functionalities
const TInt KAdapt					= 33;
//const TInt KSIAddPronunciation				= 34; // Commented out to remove "not referenced" warning
const TInt KAddRuleVariant				= 35;
//const TInt KAddVoiceTag					= 36; // Commented out to remove "not referenced" warning
const TInt KAddVoiceTags				= 37;
const TInt KCreateRule					= 38;
//const TInt KSIRecognize					= 39; // Commented out to remove "not referenced" warning
const TInt KEndRecord					= 40;
const TInt KUnloadGrammar 				= 41;
//const TInt KUpdateGrammarAndLexicon 				= 42; // Commented out to remove "not referenced" warning
const TInt KGetPronunciationCount				= 43;
const TInt KGetRuleCount				= 44;
const TInt KRemoveRules					= 45;
 
//const TInt KAddSIPronunciation			= 43; // Commented out to remove "not referenced" warning
//const TInt KAddOneSIPronunciation	= 44; // Commented out to remove "not referenced" warning

#ifdef __SINDE_TRAINING
// SINDE voice tags creation
const TInt KAddSindeVoiceTags = 46;
//const TInt KAddSindeVoiceTag = 47; // Commented out to remove "not referenced" warning
#endif // __SINDE_TRAINING

const TInt KPreStartSampling = 48;

// Define this if SINDE lexicon optimization is on
#define __SIND_LEXICON_OPT

// Secure ID of voice commands application process
_LIT_SECURE_ID( KVoiceCommandsAppUid, 0x101f8555 );

// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// Panic
// User panic when an unrecoverable error occurs.
// Returns: -
// -----------------------------------------------------------------------------
//
void Panic( TInt aPanicCode ) // Panic code
    {
    _LIT( KSDControllerPanicCategory, "SIControllerPlugin" );
    User::Panic( KSDControllerPanicCategory, aPanicCode );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIControllerPlugin::CSIControllerPlugin
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSIControllerPlugin::CSIControllerPlugin( CSIControllerPluginInterface& aControllerIf )
                                        : CActive( EPriorityLess ),
                                          iState( ESiPluginIdle ),
                                          iControllerIf( aControllerIf ),
                                          iClientRegistered( EFalse ),  
                                          iGrammarID( 0 ),
                                          iDataLoader( NULL )
    {
    RUBY_DEBUG0( "CSIControllerPlugin::CSIControllerPlugin" );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
// 
void CSIControllerPlugin::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::ConstructL" );
    
    iResourceHandler = CSIResourceHandler::NewL();
    
    //  Data loader
    iDataLoader = CDataLoader::NewL( *(iResourceHandler->iDataFilenamePrefix), 
        *(iResourceHandler->iDataFilenameSeperator),
        *(iResourceHandler->iDataFilenamePostfix ) );
    
    TFileName dbFileName( *(iResourceHandler->iDbFileName ));
    
    // Database instances
    iSIDatabase = CSIDatabase::NewL( dbFileName );
    RDbNamedDatabase& database = iSIDatabase->SIDatabase();
    RDbs& dbSession = iSIDatabase->DbSession();
    TInt drive = iSIDatabase->DbDrive();
    iSIGrammarDB = CSIGrammarDB::NewL( database, dbSession, drive );
    iSILexiconDB = CSILexiconDB::NewL( database, dbSession, drive );
    iSIModelBankDB = CSIModelBankDB::NewL( database, dbSession, drive );

    iSIDatabase->BeginTransactionL();    
    
    if ( !( iSIDatabase->DoesLockTableExistL() ) )
        {
        iSIDatabase->CreateLockTableL();
        }

    // Need to create all 3 ID tables at single transaction


    // Create tables if they don't already exist    
    if ( !( iSIDatabase->DoesModelBankTableExistL() ) )
        { 
        iSIModelBankDB->CreateIDTableL();
        }

    if ( !( iSIDatabase->DoesLexiconTableExistL() ) )
        { 
        iSILexiconDB->CreateIDTableL();
        }

    if ( !( iSIDatabase->DoesGrammarTableExistL() ) )
        { 
        iSIGrammarDB->CreateIDTableL();
        }

    iSIDatabase->CommitChangesL( ETrue );


    iSITtpWordList = CSITtpWordList::NewL();
    
    iDevASR = CDevASR::NewL( *this );
    
    RUBY_DEBUG2( "[%d] CSIControllerPlugin::ConstructL - DevASR[%d]", this, iDevASR );
    
#ifdef __SINDE_TRAINING 		
    iSindeTrainer = CSindeTrainer::NewL( *iDevASR, iControllerIf, *this, 
                                         *iSIDatabase, *iSIGrammarDB, *iSILexiconDB );
#endif // __SINDE_TRAINING    
    
    // Add active object to active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIControllerPlugin* CSIControllerPlugin::NewL( CSIControllerPluginInterface& aControllerIf )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::NewL" );
    CSIControllerPlugin* self = new( ELeave ) CSIControllerPlugin( aControllerIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::~CSIControllerPlugin
// Destructor
// -----------------------------------------------------------------------------
//
CSIControllerPlugin::~CSIControllerPlugin()
    {
	RUBY_DEBUG0( "CSIControllerPlugin::~CSIControllerPlugin" );

    RThread().SetPriority( EPriorityNormal );

    Cancel();
    
    // if there is any outstanding request, cancel it.
    if ( iDevASR )
        {
        iDevASR->Cancel();
        }
    
    delete iDevASR;
    delete iResourceHandler;
    delete iSIGrammarDB;
    delete iSILexiconDB; // Handled by Grcompiler
    delete iSIModelBankDB;	
    delete iDataLoader;
    
    delete iSIDatabase;
    delete iSICompiledGrammarRecompile;
    
	iMaxNPronunsForWord.Close();
    delete iSITtpWordList;
    
#ifdef __SINDE_TRAINING 		
    delete iSindeTrainer;
#endif // __SINDE_TRAINING

    if ( iTrainArrays != NULL )
        {
        iTrainArrays->ResetAndDestroy();
        iTrainArrays->Close();
        delete iTrainArrays;
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddPronunciationL
// Calls the lexicon database handler to add a new pronunciation into the lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::AddPronunciationL( TSILexiconID /*aLexiconID*/,
                                             TSIModelBankID /*aModelBankID*/,
                                             const TDesC8& /*aPronunciationPr*/,
                                             TSIPronunciationID& /*aPronunciationID*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AddPronunciationL 1" );
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddPronunciationL
// Calls the lexicon database handler to add a new pronunciation into the lexicon.
// -----------------------------------------------------------------------------
// 
void CSIControllerPlugin::AddPronunciationL( TSILexiconID /*aLexiconID*/,
                                             TSIModelBankID /*aModelBankID*/,
                                             TSIModelID /*aModelID*/, 
                                             TSIPronunciationID& /*aPronunciationID*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AddPronunciationL 2" );
    User::Leave( KErrNotSupported );	
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::SDLexiconL
// -----------------------------------------------------------------------------
//   
#ifdef DEVASR_KEEP_SD
CSDLexicon* CSIControllerPlugin::SDLexiconL( TSILexiconID anID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::SDLexiconL" );
    
    User::Leave( KErrNotSupported );
    return NULL;	
    }
#endif

// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddRuleL
// Calls the grammar database handler to add a new rule into the grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::AddRuleL( TSIGrammarID aGrammarID,
                                    TSILexiconID aLexiconID,
                                    TSIPronunciationID aPronunciationID,
                                    TSIRuleID& aRuleID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AddRuleL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KAddRule;
    iGrammarID = aGrammarID;
    iLexiconID = aLexiconID;
    iPronunciationID = aPronunciationID;
    iRuleIDPtr = &aRuleID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleAddRule
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleAddRuleL( TSIGrammarID /*aGrammarID*/,
                                          TSILexiconID /*aLexiconID*/,
                                          TSIPronunciationID /*aPronunciationID*/,
                                          TSIRuleID& /*aRuleID*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleAddRuleL" );
    
    User::Leave( KErrNotSupported ); 
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::Cancel
// Cancels the current activity and returns the plugin to Idle state.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::Cancel()
    {
    RUBY_DEBUG1( "CSIControllerPlugin::Cancel (State=%d)", iState );
    
    this->iSIDatabase->Rollback();
    switch ( iState )
        {
        case ESdPluginTrain:
            iDevASR->Cancel();
            
            TRAP_IGNORE( 
                iSIDatabase->BeginTransactionL();
                iSIModelBankDB->Reset();
                iSIDatabase->CommitChangesL( ETrue );
                ); // TRAP_IGNORE
               
            break;
            
        case ESiPluginRecognize:
            iDevASR->Cancel();
            iState = ESiPluginRecognize; 	
            break;
        case ESiPluginTrainText:
#ifdef __SINDE_TRAINING
        case ESiPluginTrainSinde:
#endif // __SINDE_TRAINING
            iDevASR->Cancel();
            iState = ESiPluginIdle; 	
            break;
        case ESiPluginPlay:
            iDevASR->Cancel();
            break;
            
        case ESiPluginIdle:
            // Nothing to cancel
            break;
            
        default:
            // Unknown state - should never be here
            break;
        }
    
    // Cancel the active object
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::CommitChangesL
// Commits (saves) all database changes since the last commit request.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::CommitChangesL( TBool aCommit )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::CommitChangesL" );
    iSIDatabase->CommitChangesL( aCommit );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::CreateGrammarL
// Calls the grammar database handler to create a new grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::CreateGrammarL( TSIGrammarID& aGrammarID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::CreateGrammarL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KCreateGrammar;
    iGrammarIDPtr = &aGrammarID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleCreateGrammarL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleCreateGrammarL( TSIGrammarID& aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleCreateGrammarL" );
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        
        // update model banks if they are not valid anymore
        RArray<TSIModelBankID> modelBankIDs;
        CleanupClosePushL( modelBankIDs );
        iSIModelBankDB->GetAllClientModelBankIDsL( iClientUid, modelBankIDs );
        for ( TInt i(0); i < modelBankIDs.Count(); i++ )
            {
            iSIModelBankDB->UpdateModelBankIfInvalidL( modelBankIDs[i] );
            }
        CleanupStack::PopAndDestroy( &modelBankIDs );
        aGrammarID = iSIGrammarDB->CreateGrammarL(iClientUid);
        iSIDatabase->CommitChangesL( ETrue );
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::CreateLexiconL
// Calls the lexicon database handler to create a new lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::CreateLexiconL( TSILexiconID& aLexiconID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::CreateLexiconL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KCreateLexicon;
    iLexiconIDPtr = &aLexiconID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleCreateLexiconL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleCreateLexiconL( TSILexiconID& aLexiconID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleCreateLexiconL");
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        aLexiconID = iSILexiconDB->CreateLexiconL(iClientUid);
        iSIDatabase->CommitChangesL( ETrue );
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::CreateModelBankL
// Calls the model database handler to create a new model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::CreateModelBankL( TSIModelBankID& aModelBankID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::CreateModelBankL");
    
    if ( IsActive() )
        {
        User::Leave(KErrServerBusy);
        }
    iRequestFunction = KCreateModelBank;
    iModelBankIDPtr = &aModelBankID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleCreateModelBankL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleCreateModelBankL( TSIModelBankID& aModelBankID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleCreateModelBankL");
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        aModelBankID = iSIModelBankDB->CreateModelBankL(iClientUid);
        iSIDatabase->CommitChangesL( ETrue ); 
        }
    else
        {
        User::Leave(KErrAsrNotRegisted);
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::EndRecSessionL
// Ends recognition session and releases resources.  If not in recognition state,
// no action is taken.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::EndRecSessionL()
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::EndRecSessionL");
    
    switch ( iState )
        {
        case ESiPluginRecognize:
            iDevASR->EndRecSession();
            // Save the changes during model adaption.
            iSIDatabase->CommitChangesL( ETrue );  
            RecognitionReset();
            break;
        case ESiPluginIdle:
            // Ignore if already Idle
            break;
        default:
            // Wrong state
            User::Leave( KErrNotReady );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::ActivateGrammarL
// Activate the grammar for the recognition
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::ActivateGrammarL( TSIGrammarID aGrammarID ) 
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::ActivateGrammarL");
    iDevASR->ActivateGrammarL(aGrammarID);
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::DeactivateGrammarL
// Deactivate the grammar for the recognition 
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::DeactivateGrammarL(TSIGrammarID aGrammarID ) 
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::DeactivateGrammarL");
    iDevASR->DeactivateGrammarL(aGrammarID);
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllClientGrammarIDsL
// Returns all grammar Ids that belong to the current client.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllClientGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetAllClientGrammarIDsL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllClientGrammarIDs;
    
    iGrammarIDs = &aGrammarIDs;
    
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllClientGrammarIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllClientGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetAllClientGrammarIDsL");
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSIGrammarDB->GetAllClientGrammarIDsL(iClientUid, aGrammarIDs);
        iSIDatabase->CommitChangesL( EFalse );
        
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllClientLexiconIDsL
// Returns all lexicon Ids that belong to the current client.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllClientLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetAllClientLexiconIDsL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllClientLexiconIDs;
    iLexiconIDs = &aLexiconIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllClientLexiconIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllClientLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleGetAllClientLexiconIDsL" );
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSILexiconDB->GetAllClientLexiconIDsL( iClientUid, aLexiconIDs );		
        iSIDatabase->CommitChangesL( EFalse );
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllClientModelBankIDsL
// Returns all model bank Ids that belong to the current client.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllClientModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetAllClientModelBankIDsL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllClientModelBankIDs;
    iModelBankIDs = &aModelBankIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllClientModelBankIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllClientModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleGetAllClientModelBankIDsL" );
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSIModelBankDB->GetAllClientModelBankIDsL( iClientUid, aModelBankIDs );
        iSIDatabase->CommitChangesL( EFalse );
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllGrammarIDsL
// Returns all grammar Ids that exist (for all clients).
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetAllGrammarIDsL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllGrammarIDs;
    iGrammarIDs = &aGrammarIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllGrammarIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetAllGrammarIDsL");
    
    iSIDatabase->BeginTransactionL();
    iSIGrammarDB->GetAllGrammarIDsL(aGrammarIDs);
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllLexiconIDsL
// Returns all lexicon Ids that exist (for all clients).
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetAllLexiconIDsL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllLexiconIDs;
    iLexiconIDs = &aLexiconIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllLexiconIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetAllLexiconIDsL");
    
    iSIDatabase->BeginTransactionL();
    iSILexiconDB->GetAllLexiconIDsL(aLexiconIDs);
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllModelBankIDsL
// Returns all model bank Ids that exist (for all clients).
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetAllModelBankIDsL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllModelBankIDs;
    iModelBankIDs = &aModelBankIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllModelBankIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetAllModelBankIDsL");
    
    iSIDatabase->BeginTransactionL();
    iSIModelBankDB->GetAllModelBankIDsL( aModelBankIDs );
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllModelIDsL
// Calls the model database handler to get all model IDs in the model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllModelIDsL( TSIModelBankID aModelBankID,
                                           RArray<TSIModelID>& aModelIDs )
    { 
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetAllModelIDsL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllModelIDs;
    iModelBankID = aModelBankID;
    iModelIDs = &aModelIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllModelIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllModelIDsL( TSIModelBankID aModelBankID,
                                                 RArray<TSIModelID>& aModelIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleGetAllModelIDsL" );
    
    iSIDatabase->BeginTransactionL();
    iSIModelBankDB->GetAllModelIDsL(aModelBankID, aModelIDs);
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllPronunciationIDsL
// Calls the lexicon database handler to get all pronunciation IDs in the lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllPronunciationIDsL( TSILexiconID aLexiconID,
                                                  RArray<TSIPronunciationID>& aPronunciationIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetAllPronunciationIDsL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllPronunciationIDs;
    iLexiconID = aLexiconID;
    iPronunciationIDs = &aPronunciationIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllPronunciationIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllPronunciationIDsL( TSILexiconID aLexiconID,
                                                         RArray<TSIPronunciationID>& aPronunciationIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetAllPronunciationIDsL");
    
    iSIDatabase->BeginTransactionL();
    iSILexiconDB->GetAllPronunciationIDsL(aLexiconID, aPronunciationIDs);
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAllRuleIDsL
// Calls the grammar database handler to get all rule IDs in the grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAllRuleIDsL( TSIGrammarID aGrammarID,
                                          RArray<TSIRuleID>& aRuleIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetAllRuleIDsL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAllRuleIDs;
    iGrammarID = aGrammarID;
    iRuleIDs = &aRuleIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAllRuleIDsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAllRuleIDsL( TSIGrammarID aGrammarID,
                                                RArray<TSIRuleID>& aRuleIDs )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetAllRuleIDsL");
    
    iSIDatabase->BeginTransactionL();
    iSIGrammarDB->GetAllRuleIDsL(aGrammarID, aRuleIDs);
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetAvailableStorageL
// Calculates the available number of models that can be trained by subtracting
// the current total from the system defined max.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetAvailableStorageL( TInt& aCount )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetAvailableStorageL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetAvailableStorage;
    iCountPtr = &aCount;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetAvailableStorageL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetAvailableStorageL( TInt& aCount )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetAvailableStorageL");
    
    iSIDatabase->BeginTransactionL();
    TInt count = iSIModelBankDB->AllModelCountL();
    iSIDatabase->CommitChangesL( EFalse );
    
    if ( count > iResourceHandler->iMaxModelsSystem )
        {
        aCount = 0;
        }
    else
        {
        aCount = iResourceHandler->iMaxModelsSystem - count;
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetEnginePropertiesL
// Returns the current recognition engine properties.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetEnginePropertiesL( const RArray<TInt>& aPropertyId,
                                                RArray<TInt>& aPropertyValue )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetEnginePropertiesL" );
    
    for ( TInt index = 0; index < aPropertyId.Count(); index++ )
        {
        switch( aPropertyId[index] )
            {
            case KModelStorageCapacity:
                aPropertyValue.Append( iResourceHandler->iMaxModelsSystem );
                break;
                
            case KMaxLoadableModels:
                aPropertyValue.Append( iResourceHandler->iMaxModelsPerBank );
                break;
                
            default:
                aPropertyValue.Append( KErrNotSupported );
                break;
            }
        }
    
    iDevASR->GetEnginePropertiesL( aPropertyId, aPropertyValue );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetModelCountL
// Calls the model database handler for the number of models in a model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetModelCountL( TSIModelBankID aModelBankID,
                                          TInt& aCount )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetModelCountL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetModelCount;
    iModelBankID = aModelBankID;
    iCountPtr = &aCount;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetModelCountL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetModelCountL( TSIModelBankID aModelBankID,
                                                TInt& aCount )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleGetModelCountL" );
    
    iSIDatabase->BeginTransactionL();
    aCount = iSIModelBankDB->ModelCountL( aModelBankID );
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetPronunciationCountL
// Calls the model database handler for the number of Pronunciation  in a lexicon
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetPronunciationCountL( TSILexiconID aLexiconID,
                                                  TInt& aCount )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetPronunciationCountL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetPronunciationCount;
    iLexiconID = aLexiconID;
    iCountPtr = &aCount;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetPronunciationCountL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetPronunciationCountL( TSIModelBankID aLexiconID,
                                                        TInt& aCount )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleGetPronunciationCountL" );
    
    iSIDatabase->BeginTransactionL();
    aCount = iSILexiconDB->PronunciationCountL( aLexiconID );
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetRuleCountL
// Calls the model database handler for the number of Pronunciation  in a lexicon
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetRuleCountL( TSIGrammarID aGrammarID,
                                         TInt& aCount )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetRuleCountL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetRuleCount;
    iGrammarID = aGrammarID;
    iCountPtr = &aCount;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetRuleCountL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetRuleCountL( TSIGrammarID aGrammarID,
                                               TInt& aCount )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetRuleCountL");
    
    iSIDatabase->BeginTransactionL();
    aCount = iSIGrammarDB->RuleCountL(aGrammarID);
    iSIDatabase->CommitChangesL( EFalse );
    }



// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetRuleValidityL
// Calls the grammar database handler to see if the rule exists in the specified
// grammar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetRuleValidityL( TSIGrammarID aGrammarID,
                                            TSIRuleID aRuleID,
                                            TBool& aValid )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::GetRuleValidityL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetRuleValidity;
    iGrammarID = aGrammarID;
    iRuleID = aRuleID;
    iValidPtr = &aValid;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetRuleValidityL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetRuleValidityL( TSIGrammarID aGrammarID,
                                                  TSIRuleID aRuleID,
                                                  TBool& aValid )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleGetRuleValidityL");
    
    iSIDatabase->BeginTransactionL();
    aValid = iSIGrammarDB->IsRuleValidL(aGrammarID, aRuleID);
    iSIDatabase->CommitChangesL( EFalse );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::GetUtteranceDurationL
// Calls the model database handler to get the duration of an utterance.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::GetUtteranceDurationL( TSIModelBankID aModelBankID,
                                                 TSIModelID aModelID,
                                                 TTimeIntervalMicroSeconds32& aDuration )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::GetUtteranceDurationL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KGetUtteranceDuration;
    iModelBankID = aModelBankID;
    iModelID = aModelID;
    iDurationPtr = &aDuration;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleGetUtteranceDurationL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleGetUtteranceDurationL( TSIModelBankID /*aModelBankID*/,
                                                       TSIModelID /*aModelID*/,
                                                       TTimeIntervalMicroSeconds32& /*aDuration*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleGetUtteranceDurationL" );
    User::Leave( KErrNotSupported );		
    }


 // -----------------------------------------------------------------------------
// CSIControllerPlugin::HandlePlayUtteranceL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandlePlayUtteranceL( TSIModelBankID /*aModelBankID*/,
                                                TSIModelID /*aModelID*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandlePlayUtteranceL" );
    User::Leave( KErrNotSupported );		 
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::LoadEngineParametersL
// Loads the specified recognizer parameter(s).  These parameters are used to
// alter the recognizer's default parameters.  The parameters are specified as
// attribute and value pairs.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::LoadEngineParametersL( const RArray<TInt>& aParameterId,
                                                 const RArray<TInt>& aParameterValue )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::LoadEngineParametersL" );
    iDevASR->LoadEngineParametersL( aParameterId, aParameterValue );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::LoadGrammarL
// Requests the grammar database handler for a grammar object to be loaded into
// the recognizer for recognition purpose.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::LoadGrammarL( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::LoadGrammarL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        } 
    
    iRequestFunction = KLoadGrammar;
    iGrammarID = aGrammarID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleLoadGrammarL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleLoadGrammarL( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleLoadGrammarL" );
    
    if ( iState == ESiPluginRecognize )
        {
        const CSICompiledGrammar* grammar = NULL;
        // iGrammarDB keeps a reference to the grammar object so we don't
        // have to push it onto a cleanupstack.
        
        iSIDatabase->BeginTransactionL();
        grammar = iSIGrammarDB->LoadGrammarL(aGrammarID);
        iSIDatabase->CommitChangesL( EFalse );
        
        iDevASR->LoadGrammar( *grammar );
        }
    else
        {
        User::Leave( KErrAsrInvalidState );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::LoadLexiconL
// Requests the lexicon database handler for a lexicon object to be loaded into
// the recognizer for recognition purpose.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::LoadLexiconL( TSILexiconID aLexiconID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::LoadLexiconL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KLoadLexicon;
    iLexiconID = aLexiconID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleLoadLexiconL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleLoadLexiconL( TSILexiconID aLexiconID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleLoadLexiconL" );
    
    if ( iState == ESiPluginRecognize )
        {
        const CSILexicon* lexicon = NULL;
        // iGrammarDB keeps a reference to the grammar object so we don't
        // have to push it onto a cleanupstack.
        
        iSIDatabase->BeginTransactionL();
        lexicon = iSILexiconDB->LexiconL( aLexiconID );
        iSIDatabase->CommitChangesL( EFalse );
        
        iDevASR->LoadLexicon( *lexicon );
        }
    else
        {
        // Wrong state
        User::Leave( KErrAsrInvalidState );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::LoadModelsL
// Requests the model database handler for a model bank object to be loaded into
// the recognizer for recognition purpose.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::LoadModelsL( TSIModelBankID aModelBankID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::LoadModelsL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KLoadModels;
    iModelBankID = aModelBankID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleLoadModelsL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleLoadModelsL( TSIModelBankID aModelBankID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleLoadModelsL" );
    
    if ( iState == ESiPluginRecognize )
        { 
        
        const CSIModelBank* modelBank = NULL;
        // iModelBankDB keeps a reference to the modelBank object so we don't
        // have to push it onto a cleanupstack.
        iSIDatabase->BeginTransactionL();
        modelBank = iSIModelBankDB->AllAcousticModelsL( aModelBankID );
        iSIDatabase->CommitChangesL( EFalse );
        
        iDevASR->LoadModels( *modelBank );
        
        }
    else
        {
        // Wrong state
        User::Leave( KErrAsrInvalidState );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::PlayUtteranceL
// Plays the user utterance.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::PlayUtteranceL( TSIModelBankID aModelBankID,
                                         TSIModelID aModelID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::PlayUtteranceL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KPlayUtterance;
    iModelBankID = aModelBankID;
    iModelID = aModelID;
    DoAsynch();
    }
 
// -----------------------------------------------------------------------------
// CSIControllerPlugin::RecognizeL
// Initiates recognition towards the recognizer.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RecognizeL( CSDClientResultSet& /*aClientResultSet*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RecognizeL(sd)" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RecognizeL
// Initiates recognition towards the recognizer.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RecognizeL( CSIClientResultSet& aResultSet )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::RecognizeL(si)");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KRecognize ;
    
    iSIClientResultSet=&aResultSet;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRecognizeL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRecognizeL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleRecognizeL" );
    
    if ( iState == ESiPluginRecognize )
        { 
        iSIDatabase->BeginTransactionL();
        
        if ( iSIGrammarDB->IsGrammarLoaded() )			
            {			 
            // ClientResultSet was already saved by RunL 	 
            iSIResultSet = &( iSIClientResultSet->SIResultSet() );
            iDevASR->InitRecognizerBE( *iSIResultSet );
            }
        else
            {
            // Either the loaded grammars are empty (contains no rule) or
            // all rules have been unloaded.  No need to proceed any further.
            iSIDatabase->CommitChangesL( ETrue );
            User::Leave( KErrNotReady );
            }
        
        iSIDatabase->CommitChangesL( ETrue );
        }
    else
        {
        // Wrong state
        User::Leave( KErrAsrInvalidState );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RecordL
// Initiates recording of user utterance.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RecordL( TTimeIntervalMicroSeconds32 aRecordTime )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RecordL" );
    
    RUBY_DEBUG0( "CSIControllerPlugin::RecognizeL Returning thread priority back to normal" );
    // the priority was lowered in the StartRecSessionL as a quick fix (voice ui 
    // tone player had too low priority to run. Toi minimize the poorly tested
    // consequences of the quick-fix, we return priority back to normal
    // as soon as possible
 
// Temporary fix that makes voiceui work in wk36-> sdks
//   RThread().SetPriority( EPriorityNormal );
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KRecord;
    iRecordTime = aRecordTime;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRecordL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRecordL( TTimeIntervalMicroSeconds32 aRecordTime )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleRecordL");
    
    if ( iState == ESdPluginTrain || iState == ESiPluginRecognize )
        {
        iDevASR->Record(aRecordTime);
        }
    else
        {
        // Wrong state
        User::Leave( KErrAsrInvalidState );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::PreStartSamplingL
// Pre-starts sampling before recognition start.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::PreStartSamplingL()
    {
    RUBY_DEBUG_BLOCK( "" );

    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KPreStartSampling;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandlePreStartSamplingL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandlePreStartSamplingL()
    {
    RUBY_DEBUG_BLOCK( "" );
    iDevASR->PreStartSamplingL();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RemoveGrammarL
// Calls the grammar database handler to remove a grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RemoveGrammarL( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RemoveGrammarL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KRemoveGrammar;
    iGrammarID = aGrammarID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRemoveGrammarL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRemoveGrammarL( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleRemoveGrammarL");
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSIGrammarDB->RemoveGrammarL( iClientUid, aGrammarID );
        iSIDatabase->CommitChangesL( ETrue ) ;  
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RemoveLexiconL
// Calls the lexicon database handler to remove a lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RemoveLexiconL( TSILexiconID aLexiconID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RemoveLexiconL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KRemoveLexicon;
    iLexiconID = aLexiconID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRemoveLexiconL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRemoveLexiconL( TSILexiconID aLexiconID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleRemoveLexiconL");
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSILexiconDB->RemoveLexiconL( iClientUid, aLexiconID );		
        iSIDatabase->CommitChangesL( ETrue ) ;  
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RemoveModelBankL
// Calls the model database handler to remove a model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RemoveModelBankL( TSIModelBankID aModelBankID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RemoveModelBankL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KRemoveModelBank;
    iModelBankID = aModelBankID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRemoveModelBankL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRemoveModelBankL( TSIModelBankID aModelBankID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleRemoveModelBankL" );
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSIModelBankDB->RemoveModelBankL( iClientUid, aModelBankID );
        iSIDatabase->CommitChangesL( ETrue ) ;  
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RemoveModelL
// Calls the model database handler to remove a model from a model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RemoveModelL( TSIModelBankID aModelBankID,
                                        TSIModelID aModelID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RemoveModelL" );
    
    if ( IsActive() )
        {
        
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KRemoveModel;
    iModelBankID = aModelBankID;
    iModelID = aModelID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRemoveModelL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRemoveModelL( TSIModelBankID /*aModelBankID*/,
                                              TSIModelID /*aModelID*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleRemoveModelL" );
    User::Leave( KErrNotSupported );  
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RemovePronunciationL
// Calls the lexicon database handler to remove a pronunciation from a lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RemovePronunciationL( TSILexiconID aLexiconID,
                                                TSIPronunciationID aPronunciationID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RemovePronunciationL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KRemovePronunciation;
    iLexiconID = aLexiconID;
    iPronunciationID = aPronunciationID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRemovePronunciationL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRemovePronunciationL( TSILexiconID aLexiconID,
                                                      TSIPronunciationID aPronunciationID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleRemovePronunciationL" );
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSILexiconDB->RemovePronunciationL( iClientUid, aLexiconID, aPronunciationID );
        iSIDatabase->CommitChangesL( ETrue ) ; 
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RemoveRuleL
// Calls the grammar database handler to remove a rule from a grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RemoveRuleL( TSIGrammarID aGrammarID,
                                       TSIRuleID aRuleID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RemoveRuleL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iState=ESiPluginRemoveRule;  // handled by database modify function 
    iRequestFunction = KRemoveRule;
    iGrammarID = aGrammarID;
    iRuleID = aRuleID;
    DoAsynch();
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRemoveRuleL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRemoveRuleL( TSIGrammarID aGrammarID,
                                             TSIRuleID aRuleID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleRemoveRuleL" );
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        
        iSICompiledGrammarRecompile = ( CSICompiledGrammar* )iSIGrammarDB->GrammarL( aGrammarID );	
        // if leave as KErrArgument
        
        CSIRule* aRule = &iSICompiledGrammarRecompile->AtL( iSICompiledGrammarRecompile->Find( aRuleID ) );
        
        
        // in a rule , all the variant have same lexicon ID, that is because 
        // the way it was added in addvoicetags()
        CSIRuleVariant* rv = &aRule->AtL( 0 );	
        TSILexiconID lexiconId = rv->LexiconID();	
        
        TSIPronunciationIDSequence pronunciationIdSequence;	
        CleanupClosePushL( pronunciationIdSequence );
        
        CSILexicon* lexicon = iSILexiconDB->LexiconL( lexiconId );	
        CleanupStack::PushL( lexicon ); 
        
        for ( TInt i = 0; i < aRule->Count(); i++ )
            {
            pronunciationIdSequence.Reset();
            CSIRuleVariant* rv = &aRule->AtL( i );
            rv->GetPronunciationIDsL( pronunciationIdSequence );
            for ( TInt k = 0; k < pronunciationIdSequence.Count(); k++ )
                {
                lexicon->DeleteL( pronunciationIdSequence[k] );		
                }
            }
            
        // Update the lexicon
        iSILexiconDB->UpdateLexiconL( iClientUid, lexicon );
        
        CleanupStack::PopAndDestroy( lexicon );
        CleanupStack::PopAndDestroy( &pronunciationIdSequence ); 
        
        
        // Recompile the grammar after a deletion
        // Async call, callback handled in handleeventtp
        
        iSICompiledGrammarRecompile->DeleteL( aRuleID );
        if ( iSICompiledGrammarRecompile->Count() == 0 )
            {
            // Reset compiled data to null
            iSICompiledGrammarRecompile->SetCompiledData( NULL );
            delete iSICompiledGrammarRecompile;
            iSICompiledGrammarRecompile = NULL;
            // Special case, do the callback now since we're not expecting
            // anything from the lower layers
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveRule, iResult );
            }
        else
            {
            iDevASR->CompileGrammarL( *iSICompiledGrammarRecompile );
            }
        
        iSIDatabase->CommitChangesL( ETrue );
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }



// -----------------------------------------------------------------------------
// CSIControllerPlugin::RemoveRulesL
// Calls the grammar database handler to remove a set of rules from a grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RemoveRulesL( TSIGrammarID aGrammarID,
                                        RArray<TSIRuleID>& aRuleIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RemoveRulesL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iState=ESiPluginRemoveRules;  // handled by database modify function 
    iRequestFunction = KRemoveRules;
    iGrammarID = aGrammarID;
    iRuleIDs = &aRuleIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleRemoveRulesL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// (other items were commented in a header).
// Note, that request to remove a non-existing rule completes successfully
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleRemoveRulesL( TSIGrammarID aGrammarID,
                                              RArray<TSIRuleID>& aRuleIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleRemoveRulesL" );
    TInt i( 0 );

    RUBY_DEBUG1( "CSIControllerPlugin::HandleRemoveRulesL grammarId [%d]", aGrammarID );
    RUBY_DEBUG1( "CSIControllerPlugin::HandleRemoveRulesL aRuleIDs.Count() = [%d]", aRuleIDs.Count() );
    
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        
        // 1.Get the lexion id 
        // 2. For each rule, delete it's varant's prounication from lexicon and then delete the rule from grammar
        // 3. Update lexicon DB.
        // 4. Recompile the grammar
        // Get the grammar ,use all rules, so it clean up when iSIGrammarDB is deleted	
        RUBY_DEBUG1( "CSIControllerPlugin::HandleRemoveRulesL Getting All rules for grammarID [%d]", aGrammarID );
        
        iSICompiledGrammarRecompile = ( CSICompiledGrammar* )iSIGrammarDB->GrammarL( aGrammarID );	
        __UHEAP_MARK;		
        RPointerArray<CSILexicon> aLexiconArray;
        CleanupClosePushL( aLexiconArray );
        RArray<TSILexiconID> aLexiconIDs;
        CleanupClosePushL( aLexiconIDs );
        CSILexicon* aLexicon;
        
        // if leave as KErrArgument
        for ( i = 0; i < aRuleIDs.Count(); i++ ) 
            {		
            TSIRuleID aRuleID = aRuleIDs[i];
            TInt removeRuleIndex = iSICompiledGrammarRecompile->Find( aRuleID );
            CSIRule* aRule = NULL;  // rule to remove
            if ( removeRuleIndex == KErrNotFound ) 
                {
                RUBY_DEBUG1("CSIControllerPlugin::HandleRemoveRulesL RuleID [%d] not found. Probably already deleted", aRuleID );
                // Nothing to delete in this round.
                // Proceed to the next one
                continue;
                }
            else if ( removeRuleIndex >= 0 ) 
                {
                aRule = &iSICompiledGrammarRecompile->AtL( removeRuleIndex );
                }
            else 
                {
                // Unknown error
                User::Leave( removeRuleIndex );
                }
            // in a rule , all the variant have same lexicon ID, 
            // and all the rule's rule variant have same lexicon ID,
            // one lexicon corresponding to one grammar
            // that is because  the way it was added in addvoicetags()			
            CSIRuleVariant* aRv = &aRule->AtL( 0 );	
            TSILexiconID aLexiconID = aRv->LexiconID();	
            
            // This hack ensures 4 byte alignment in winscw
            TSILexiconID* lexID = new ( ELeave ) TSILexiconID;
            CleanupStack::PushL( lexID );
            *lexID = aRv->LexiconID();       
            // Uniq ids array
            aLexiconIDs.InsertInSignedKeyOrder( *lexID );
            CleanupStack::PopAndDestroy( lexID );
            }	
        // collect the lexicon according to the uniq lexicon array
        for ( i = 0; i < aLexiconIDs.Count(); i++ ) 
            {	
            aLexicon = iSILexiconDB->LexiconL( aLexiconIDs[i] );				
            aLexiconArray.Append( aLexicon );
            }
        
        for ( i = 0; i < aRuleIDs.Count(); i++ ) 
            {
            TSIRuleID aRuleID = aRuleIDs[i];
            
            TInt removeRuleIndex = iSICompiledGrammarRecompile->Find( aRuleID );
            CSIRule* aRule = NULL;  // rule to remove
            if ( removeRuleIndex == KErrNotFound ) 
                {
                RUBY_DEBUG1("CSIControllerPlugin::HandleRemoveRulesL RuleID [%d] not found. Probably already deleted", aRuleID );
                // Nothing to delete in this round.
                // Proceed to the next one
                continue;
                }
            else if ( removeRuleIndex >= 0 ) 
                {
                aRule = &iSICompiledGrammarRecompile->AtL( removeRuleIndex );
                }
            else 
                {
                // Unknown error
                User::Leave( removeRuleIndex );
                }
            
            // in a rule , all the variant have same lexicon ID, that is because 
            // the way it was added in addvoicetags()
            
            TSIPronunciationIDSequence aIPronunciationIDSequence;	
            CleanupClosePushL( aIPronunciationIDSequence );
            
            for ( TInt i = 0 ; i < aRule->Count() ; i++ ) 
                {
                aIPronunciationIDSequence.Reset();
                CSIRuleVariant* aRv = &aRule->AtL( i );
                aRv->GetPronunciationIDsL( aIPronunciationIDSequence );
                TSILexiconID aLexiconID = aRv->LexiconID();
                TInt aLocation = 0;
                for ( TInt s = 0 ; s < aLexiconIDs.Count() ; s++ ) 
                    {
                    if ( aLexiconIDs[s] == aLexiconID ) 
                        {
                        aLocation = s;
                        break;
                        }
                    }
                
                CSILexicon* aLexicon =aLexiconArray[aLocation];
                for ( TInt k = 0 ; k < aIPronunciationIDSequence.Count() ; k++ ) 
                    {
                    aLexicon->DeleteL( aIPronunciationIDSequence[k] );
                    }
                }
            
            // delete aIPronunciationIDSequence
            CleanupStack::PopAndDestroy( &aIPronunciationIDSequence ); 
            
            // Recompile the grammar after a deletion
            // Async call, callback handled in handleeventtp
            //delete iSICompiledGrammarRecompile;			
            iSICompiledGrammarRecompile->DeleteL( aRuleID );
            }
        
        // Update the lexicon
        for ( i = 0; i < aLexiconArray.Count(); i++ )
            {
            iSILexiconDB->UpdateLexiconL( iClientUid, aLexiconArray[i] );
            }	
        
        // clean up 					
        // Close the arrays
        //aLexiconIDs.Close();
        CleanupStack::PopAndDestroy( &aLexiconIDs ); // CleanupClosePushL aLexiconIDs
        aLexiconArray.ResetAndDestroy();
        CleanupStack::PopAndDestroy( &aLexiconArray ); //aLexiconArray
        __UHEAP_MARKEND;
        
        if ( iSICompiledGrammarRecompile->Count() == 0 )
            {
            // Reset compiled data to null
            iSICompiledGrammarRecompile->SetCompiledData( NULL );
            
            iSIDatabase->BeginTransactionL();
            // save the compiled grammar
            TRAPD( error, iSIGrammarDB->UpdateGrammarL( iClientUid,iSICompiledGrammarRecompile ) );
            iSIDatabase->CommitChangesL( ETrue );
            if ( error )
                {
                iControllerIf.SendSrsEvent( KUidAsrEventRemoveRules, error );
                delete iSICompiledGrammarRecompile;
                iState = ESiPluginIdle;
                return;
                }	
            TRAP( error, iSIDatabase->CommitChangesL( ETrue ) );  
            
            delete iSICompiledGrammarRecompile;
            iSICompiledGrammarRecompile = NULL;
            iState = ESiPluginIdle;
            
            // Special case, do the callback now since we're not expecting
            // anything from the lower layers
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveRules, error );
            } // if ( iSICompiledGrammarRecompile->Count() == 0 )
        else
            {
            iDevASR->CompileGrammarL( *iSICompiledGrammarRecompile );
            }
        
        } // if ( iClientRegistered )
        else
            {
            User::Leave( KErrAsrNotRegisted );
            }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::SetClientUid
// Sets the client's UID for data ownership identification.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::SetClientUid( TUid aClientUid )
    {
    RUBY_DEBUG0( "CSIControllerPlugin::SetClientUid") ;
    iClientUid = aClientUid;
    iClientRegistered = ETrue;
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::SetPrioritySettings
// Set the priority settings for this controller.  This is used during recording
// and playback.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings )
    {
    RUBY_DEBUG0( "CSIControllerPlugin::SetPrioritySettings" );
    iDevASR->SetPrioritySettings( aPrioritySettings );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::StartRecSessionL
// Initiates a recognition session.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::StartRecSessionL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::StartRecSessionL" );
    RThread().SetPriority( EPriorityAbsoluteLow );
    RUBY_DEBUG0( "CSIControllerPlugin::StartRecSessionL Lowered thread priority to absolute low" );
    //RUBY_DEBUG1( "CSIControllerPlugin::StartRecSessionL Thread id is [%x]", RThread().Id() );
    
    if ( iState == ESiPluginIdle )
        {
        
        User::LeaveIfError( iDevASR->StartRecSession( ESiRecognition ) );		
        iState = ESiPluginRecognize;
        }
    else
        {
        // Wrong state
        User::Leave( KErrAsrInvalidState );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::TrainL
// Initiates a speaker depedent training session.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::TrainL( TSIModelBankID aModelBankID,
                                  TSIModelID& aModelID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::TrainL" );
    
    if ( IsActive() )
        {	
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KTrain;
    iModelBankID = aModelBankID;
    iModelIDPtr = &aModelID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleTrainL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleTrainL( TSIModelBankID /*aModelBankID*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleTrainL" );
    User::Leave( KErrNotSupported );  
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::UnloadRuleL
// Unloads a rule from the recognizer for this recognition session.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::UnloadRuleL( TSIGrammarID aGrammarID,
                                       TSIRuleID aRuleID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::UnloadRuleL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KUnloadRule;
    iGrammarID = aGrammarID;
    iRuleID = aRuleID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleUnloadRuleL
// This is the asynchronous handle called from the CSIAsyncHandler object.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleUnloadRuleL( TSIGrammarID aGrammarID,
                                             TSIRuleID aRuleID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleUnloadRuleL" );
    iDevASR->UnloadRule( aGrammarID, aRuleID ); // for blacklisting only
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleUnloadRuleL
// Notification from DevASR when feature vectors are available.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::FeatureVectorDataRcvd( const TDesC8& /*aFV*/, 
                                                 TInt32 /*aSNR*/, 
                                                 TInt32 /*aPosition*/ ) 
    {
    RUBY_DEBUG0( "CSIControllerPlugin::FeatureVectorDataRcvd" ); 
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::DevASREvent
// Event from DevASR interface.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::DevASREvent( TDevASREvent aEvent,
                                       TDevASRError aError )
    {
    RUBY_DEBUG2( "CSIControllerPlugin::DevASREvent (Event=%d, Error=%d)", aEvent, aError );
    
    switch ( iState )
        {
        case ESdPluginTrain:
            TRAP_IGNORE( HandleEventTrainL( aEvent, aError ) );
            break;
        case ESiPluginRecognize:
            HandleEventRecognize( aEvent, aError );
            break;
        case ESiPluginPlay:
            TRAP_IGNORE( HandleEventPlayL( aEvent, aError ) );
            break;
        case ESiPluginTrainText:
            HandleEventTTP( aEvent, aError );
            break;
#ifdef __SINDE_TRAINING
        case ESiPluginTrainSinde:
            iSindeTrainer->HandleEvent( aEvent, aError );
            break;
#endif // __SINDE_TRAINING
        case ESiPluginRemoveRule:
            HandleEventRemoveRule( aEvent, aError );
            break;
        case ESiPluginRemoveRules:
            RUBY_DEBUG0( "CSIControllerPlugin::DevASREvent case iState ESiPluginRemoveRules" );
            HandleEventRemoveRules( aEvent, aError );
            break;
        case ESiPluginIdle:
            break;
        default:
            // Unexpected or cancelled message
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RequestSpeechData
// Get speech data from audio buffer
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RequestSpeechData()
    {
    const TInt KBufferMaxLength( 4000 );
    TInt sizeLeft( iAudioBuffer->Size() - iNSamplesSent * 2 );
    if ( sizeLeft == 0 )
        {
        // nothing
        }
    else if ( sizeLeft < KBufferMaxLength*2 )
        {
        // last buffer
        iCurrentAudioBuffer.Set( iAudioBuffer->Right( sizeLeft ) );
        iDevASR->SendSpeechData( iCurrentAudioBuffer, ETrue   );
        
        iNSamplesSent += sizeLeft/2;
        }
    else
        {
        iCurrentAudioBuffer.Set( iAudioBuffer->Mid( 2*iNSamplesSent, 2*KBufferMaxLength ) );
        
        iDevASR->SendSpeechData( iCurrentAudioBuffer, EFalse  );
        iNSamplesSent += KBufferMaxLength;
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::SILexiconL
// Load lexicon by this call back functionDevAsr Take the owner ship of the lexicon 
// -----------------------------------------------------------------------------
//
CSILexicon* CSIControllerPlugin::SILexiconL( TSILexiconID anID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::SILexiconL");
    
    iSIDatabase->BeginTransactionL();
    CSILexicon* lexicon = iSILexiconDB->LexiconL(anID);
    iSIDatabase->CommitChangesL( EFalse );
    
    return lexicon;
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::ConfigurationData
// DevASR calls this method to get configuration data.
// -----------------------------------------------------------------------------
//
HBufC8* CSIControllerPlugin::ConfigurationData( TUint32 aPackageType, 
                                                TUint32 aPackageID,
                                                TUint32 aStartPosition ,
                                                TUint32 aEndPosition )
    {
    RUBY_DEBUG0( "ConfigurationData::MdtoConfigurationData" );
    return ( iDataLoader->LoadData( aPackageType, aPackageID, 
                                    aStartPosition, aEndPosition ) );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::DevASRMessage
// A message in response to a custom command.  This controller never sends a
// custom command.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::DevASRMessage( TDesC8& /*aMsg*/ )
    {
    // SI Controller Plugin does not implement this event.
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::PlayL
// Plays the trained user utterance.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::PlayL( TSIModelBankID /*aModelBankID*/,
                                 TSIModelID /*aModelID*/ )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::PlayL");
    User::Leave(KErrNotSupported);  
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleEventPlayL
// Handling of DevASR event in Play state.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleEventPlayL( TDevASREvent/* aEvent*/,
                                            TDevASRError/* aError*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleEventPlayL" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleEventPlayL
// Handling of DevASR event in RemoveRule state.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleEventRemoveRule( TDevASREvent aEvent, 
                                                 TDevASRError aError ) 
    {
    iState = ESiPluginIdle;
    
    if ( aError )
        {
        iControllerIf.SendSrsEvent(KUidAsrEventRemoveRule, aError);
        delete iSICompiledGrammarRecompile;
        iSICompiledGrammarRecompile = NULL;
        return;
        }
    
    switch (aEvent)
        {
        case EDevASRGrammarCompile:
            {
            TRAPD( error, 
                iSIDatabase->BeginTransactionL();
                // save the compiled grammar
                iSIGrammarDB->UpdateGrammarL( iClientUid, iSICompiledGrammarRecompile );
                iSIDatabase->CommitChangesL( ETrue );
                ); // TRAPD
            if ( error )
                {
                iControllerIf.SendSrsEvent( KUidAsrEventRemoveRule, error );
                delete iSICompiledGrammarRecompile;
                iSICompiledGrammarRecompile = NULL;
                iState = ESiPluginIdle;
                return;
                }	
            TRAP( error, iSIDatabase->CommitChangesL( ETrue ) );  
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveRule, error );
            
            delete iSICompiledGrammarRecompile;
            iSICompiledGrammarRecompile = NULL;
            iState = ESiPluginIdle;
            break;
            
            }
        default:
            // Unexpected or cancelled message
            RUBY_DEBUG0( "CSIControllerPlugin::HandleEventRemoveRule. Unexpected.") ;
            break;
        }
    
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleEventRemoveRules
// Handling of DevASR event in RemoveRules state.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleEventRemoveRules( TDevASREvent aEvent, 
                                                  TDevASRError aError ) 
    {
    RUBY_DEBUG2( "CSIControllerPlugin::HandleEventRemoveRules. Event [%d], Error [%d]", aEvent, aError );
    
    iState = ESiPluginIdle;
    
    if ( aError )
        {
        iControllerIf.SendSrsEvent(KUidAsrEventRemoveRules, aError);
        delete iSICompiledGrammarRecompile;
        iSICompiledGrammarRecompile = NULL;
        return;
        }
    
    switch ( aEvent )
        {
        case EDevASRGrammarCompile:
            {
            TRAPD( error, 
                iSIDatabase->BeginTransactionL();
                // save the compiled grammar
                iSIGrammarDB->UpdateGrammarL( iClientUid, iSICompiledGrammarRecompile );
                iSIDatabase->CommitChangesL( ETrue );
                ); // TRAPD
            
            if ( error )
                {
                iControllerIf.SendSrsEvent( KUidAsrEventRemoveRules, error );
                delete iSICompiledGrammarRecompile;
                iSICompiledGrammarRecompile = NULL;
                iState = ESiPluginIdle;
                return;
                }	
            TRAP( error, iSIDatabase->CommitChangesL( ETrue ) );  
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveRules, error );
            
            delete iSICompiledGrammarRecompile;
            iSICompiledGrammarRecompile = NULL;
            iState = ESiPluginIdle;
            break;
            
            }
        default:
            // Unexpected or cancelled message
            RUBY_DEBUG0( "CSIControllerPlugin::HandleEventRemoveRules. Unexpected." );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleEventTTP
// Handling of DevASR event in TTP state.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleEventTTP( TDevASREvent aEvent,
                                         TDevASRError aError ) 
    {
    if ( aError )
        {
        iControllerIf.SendSrsEvent(KUidAsrEventAddVoiceTags, aError);
        iState = ESiPluginIdle;
        return;
        }
    
    switch ( aEvent )
        {
        case EDevASRTrainFromText:
            //case EDevASRTrainFromTextFinished:
            {
            
            TRAPD( error, 
                UpdateGrammarAndLexiconDBL( iSITtpWordList, iLexiconID, iGrammarID,*iRuleIDs ); 
                iSIDatabase->BeginTransactionL();
                iSICompiledGrammar = ( CSICompiledGrammar* )iSIGrammarDB->LoadGrammarL( iGrammarID );
                iSIDatabase->CommitChangesL( EFalse );
                iDevASR->CompileGrammarL( *iSICompiledGrammar );              
                );
             if ( error )
                {
                iControllerIf.SendSrsEvent( KUidAsrEventAddVoiceTags, error );
                iState = ESiPluginIdle;
                return;
                }
            
            break;
            }
        case EDevASRGrammarCompile:
            {
            TInt error = KErrNone;
            TRAP( error,
                iSIDatabase->BeginTransactionL();
                // save the compiled grammar
                iSIGrammarDB->UpdateGrammarL( iClientUid, iSICompiledGrammar );
                iSIDatabase->CommitChangesL( ETrue );
                ); // TRAP
            iControllerIf.SendSrsEvent( KUidAsrEventAddVoiceTags, error );
            iState = ESiPluginIdle;
            break;
            }
        default:
            // Unexpected or cancelled message
            RUBY_DEBUG0( "CSIControllerPlugin::HandleEventTTP. Unexpected." );
            break;
            
        }
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleEventRecognizeL
// Handling of DevASR event in Recognition state.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleEventRecognize( TDevASREvent aEvent,
											    TDevASRError aError )
    {
    RUBY_DEBUG2( "CSIControllerPlugin::HandleEventRecognize - Event=%d, Error=%d", aEvent, aError);
    
    switch ( aEvent )
        {
        case EDevASRLoadModels:
            //	case EDevASRModelsLoaded:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRLoadModels failed with error [%d]", aError );
               
                TRAP_IGNORE( 
                    iSIDatabase->BeginTransactionL();
                    iSIModelBankDB->ResetAndDestroy();
                    iSIDatabase->CommitChangesL( ETrue );
                    ); // TRAP_IGNORE
                
                iState = ESiPluginIdle;
                }
            
            iControllerIf.SendSrsEvent( KUidAsrEventLoadModels, aError );
            break;
        case EDevASRLoadLexicon:
            //	case EDevASRLexiconLoaded:
            
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRLoadLexicon failed with error [%d]", aError );
                iSIModelBankDB->ResetAndDestroy();
                iSILexiconDB->ResetAndDestroy();			
                iState = ESiPluginIdle;
                }
            iControllerIf.SendSrsEvent( KUidAsrEventLoadLexicon, aError );
            break;
            
        case EDevASRLoadGrammar:
            //	case EDevASRGrammarLoaded:		
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRLoadGrammar failed with error [%d]", aError );

                iSIModelBankDB->ResetAndDestroy();
                iSILexiconDB->ResetAndDestroy();
                iSIGrammarDB->ResetAndDestroy();
                iState = ESiPluginIdle;
                }
            iControllerIf.SendSrsEvent( KUidAsrEventLoadGrammar, aError );
            break;
            
        case EDevASRInitRecognitionBackend:
            //case EDevASRRecBEInitialized:		
            if ( aError == KErrNone )
                {
                iDevASR->InitFrontEnd( ESiRecognition );
                }
            else
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRInitRecognitionBackend failed with error [%d]", aError );

                RecognitionReset();
                iControllerIf.SendSrsEvent( KUidAsrEventRecognition, KErrAsrInitializationFailure );
                }
            break;
            
        case EDevASRInitFrontend:
            //case EDevASRFEInitialized:
            
            if ( aError == KErrNone )
                {	
                iControllerIf.SendSrsEvent( KUidAsrEventRecognitionReady, KErrNone );
                }
            else
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRInitFrontend failed with error [%d]", aError );

                RecognitionReset();
                iControllerIf.SendSrsEvent( KUidAsrEventRecognition, KErrAsrInitializationFailure );
                }
            break;
            
        case EDevASRRecordStarted:
            //case EDevASRRecordStarted:
            
            iControllerIf.SendSrsEvent( KUidAsrEventRecordStarted, aError );
            break;
            
        case EDevASRRecord:
            //case	EDevASRRecordFinished:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRRecord failed with error [%d]", aError );

                RecognitionReset();
                }
            iControllerIf.SendSrsEvent( KUidAsrEventRecord, aError );
            break;
            
        case EDevASRAdapt: // adaptation finished
            //		case  EDevASRAdaptFinished: // adaptation finished
            
            if ( aError != KErrNone ) 
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRAdapt failed with error [%d]", aError );

                iControllerIf.SendSrsEvent( KUidAsrEventAdapt, aError );
                }
            else 
                {
                // save the models , iModelBankID decided in loadmodel()
                TRAPD( error, 
                    iSIDatabase->BeginTransactionL();
                    iSIModelBankDB->SaveModelL( iModelBankID );
                    iSIDatabase->CommitChangesL( ETrue );
                    ); // TRAPD
                iControllerIf.SendSrsEvent( KUidAsrEventAdapt, error );
                }
            break;
        case EDevASREouDetected:
            //case	EDevASRRecordFinished:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASREouDetected failed with error [%d]", aError );

                RecognitionReset();
                }
            iControllerIf.SendSrsEvent( KUidAsrEventEouDetected, aError );
            break;
            
            //	case EDevASREouDetected:
            
        case EDevASRRecognize:
            
            {
            if ( aError == KErrNone )
                {
                
                // Copy result into client result
                if ( ProcessRecognitionComplete() > 0 )
                    {
                    iResult = KErrNone;		 
                    }
                else
                    {
                    iResult = KErrAsrNoMatch;
                    }
                }
            else if (aError ==KErrOverflow ||aError ==KErrArgument  )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRRecognize failed with error [%d]", aError );

                iResult = aError;	 
                }
            else 
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRRecognize failed with error [%d]", aError );

                if ( aError != KErrAsrNoSpeech &&
                    aError != KErrAsrSpeechTooEarly &&
                    aError != KErrAsrSpeechTooLong &&
                    aError != KErrAsrSpeechTooShort &&
                    aError != KErrTimedOut &&
                    aError !=KErrOverflow &&
                    aError !=KErrArgument 
                    )
                    {
                    RecognitionReset();
                    }
                iResult = aError;
                }
            
            iControllerIf.SendSrsEvent(KUidAsrEventRecognition, iResult);
            
            //	iControllerIf.SendSrsEvent(KUidAsrEventRecognition, iResult);
            break;
            }
        case EDevASRPlayStarted:
            iControllerIf.SendSrsEvent(KUidAsrEventPlayStarted, aError);
            break;
            
        case EDevASRPlay:
            //case EDevASRPlayFinished:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRPlay failed with error [%d]", aError );

                RecognitionReset();
                }
            iControllerIf.SendSrsEvent(KUidAsrEventPlay, aError);
            break;
        case EDevASRActivateGrammar:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRActivateGrammar failed with error [%d]", aError );

                RecognitionReset();
                }
            iControllerIf.SendSrsEvent(KUidAsrEventActivateGrammar, aError);
            break;
        case EDevASRDeactivateGrammar:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRDeactivateGrammar failed with error [%d]", aError );

                RecognitionReset();
                }
            iControllerIf.SendSrsEvent(KUidAsrEventDeactivateGrammar, aError);
            break;
        case EDevASRUnloadGrammar:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRUnloadGrammar failed with error [%d]", aError );

                RecognitionReset();
                }
            else 
                { 	
                TRAP_IGNORE( 
                    iSIDatabase->BeginTransactionL();
                    iSIGrammarDB->UnloadGrammarL( iGrammarID );
                    iSIDatabase->CommitChangesL( ETrue );
                    ); // TRAP_IGNORE
                }
            
            iControllerIf.SendSrsEvent(KUidAsrEventUnloadGrammar, aError);
            break;
        case EDevASRUnloadRule:
            if ( aError != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::HandleEventRecognize. EDevASRUnloadRule failed with error [%d]", aError );

                //RecognitionReset();
                }
            else 
                {
                TRAP_IGNORE( 
                    iSIDatabase->BeginTransactionL();
                    iSIGrammarDB->UnloadRuleL( iGrammarID, iRuleID );
                    iSIDatabase->CommitChangesL( ETrue );
                    ); // TRAPD
                }
            iControllerIf.SendSrsEvent( KUidAsrEventUnloadRule, aError );
            break;
            
        default:
            // Unexpected or cancelled message
            RUBY_DEBUG0( "CSIControllerPlugin::HandleEventRecognize. Unexpected." );

            break;
        }
        
}


// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleEventTrainL
// Handling of DevASR event in Train state.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleEventTrainL( TDevASREvent /*aEvent*/,
										     TDevASRError /*aError*/ )
{
	RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleEventTrainL" );
	User::Leave( KErrNotSupported );  
}


// -----------------------------------------------------------------------------
// CSIControllerPlugin::CreateNewRuleL
// Create a empty rule into a given grammar
// -----------------------------------------------------------------------------
//                                 
CSIRule* CSIControllerPlugin::CreateNewRuleL( CSICompiledGrammar* aGrammar ) 
    {
    RUBY_DEBUG_BLOCKL("CSIControllerPlugin::CreateNewRuleL");
    TInt aID=0;
    TInt Count=aGrammar->Count();
    // existing myRuleVariantID
    RArray<TSIRuleID> myIDs;
    myIDs.Reset();
    for( TInt i = 0; i < Count; i++ ) 
        {
        CSIRule* aRule=&(aGrammar->AtL(i));
        myIDs.Append(aRule->RuleID());
        } 
    // Find a uniq new id 
    iSIDatabase->BeginTransactionL();
    aID=iSIGrammarDB->GetNewID(myIDs);
    iSIDatabase->CommitChangesL( EFalse );
    
    myIDs.Close(); 
    CSIRule* aRule = CSIRule::NewL(aID); 
    return( aRule );
    } 

// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddNewRuleVariantL
// Create new rule variant within a rule
// -----------------------------------------------------------------------------
//      
void CSIControllerPlugin::AddNewRuleVariantL( CSIRule& aRule, 
                                              TSILexiconID aLexiconID, 
                                              RArray<TSIPronunciationID>& aPronunciationIDs, 
                                              CSIParameters& aParameters ) 	
    {
    RUBY_DEBUG_BLOCKL( "CSIControllerPlugin::AddNewRuleVariantL" );
    // existing myRuleVariantID
    RArray<TSIRuleID> myID;
    myID.Reset();
    TInt i( 0 );
    for( i = 0; i < aRule.Count(); i++ ) {
        CSIRuleVariant* aRuleVariant=&(aRule.AtL(i));
        myID.Append(aRuleVariant->RuleVariantID());
        }
    
    iSIDatabase->BeginTransactionL();
    
    // Find a uniq new id 
    TSIRuleVariantID aRuleVariantID=STATIC_CAST(TSIRuleVariantID,iSIGrammarDB->GetNewID(myID));
    myID.Close();	
    
    iSIDatabase->CommitChangesL( ETrue );
    
    // add the rule variant to the rule
    CSIRuleVariant* ruleVariant= CSIRuleVariant::NewL(aRuleVariantID,aLexiconID);
    CleanupStack::PushL( ruleVariant );	
    ruleVariant->SetPronunciationIDsL( aPronunciationIDs );
    RArray<TInt> parameterIDs;
    CleanupClosePushL( parameterIDs );
    RArray<TInt> parameterValues;
    CleanupClosePushL( parameterValues );
    
    // copy parameters
    aParameters.ListParametersL( parameterIDs, parameterValues );
    
    if ( parameterIDs.Count() != parameterValues.Count() )
        {
        User::Leave( KErrCorrupt );
        }
    for ( i = 0; i < parameterIDs.Count(); i++ )
        {
        ruleVariant->SetParameterL( parameterIDs[i], parameterValues[i] );
        }
    
    CleanupStack::PopAndDestroy(); // parameterValues
    CleanupStack::PopAndDestroy(); // parameterIDs
    aRule.AddL( ruleVariant );
    CleanupStack::Pop( ruleVariant );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::CreateNewPronunciationL
// Create a empty pronunciation into a given lexicon
// -----------------------------------------------------------------------------
//                                 
TSIPronunciationID CSIControllerPlugin::CreateNewPronunciationL( CSILexicon* aLexicon, 
                                                                 TDesC8& aPronunciationPr,
												                 TSIModelBankID aModelBankID) 	
    {
    RUBY_DEBUG_BLOCKL( "CSIControllerPlugin::CreateNewPronunciationL" );
    
    TSIPronunciationID pronunciationID( 0 );
    
    if ( aLexicon->Count() )
        {
        pronunciationID = aLexicon->AtL( aLexicon->Count() - 1 ).PronunciationID() + 1;
        }
    
    //    RUBY_DEBUG1("pronunID = %i", pronunciationID );
    // add the Pronunciation  to the Pronunciation
    CSIPronunciation* pronunciation = CSIPronunciation::NewL( pronunciationID, aModelBankID );
    
    CleanupStack::PushL( pronunciation );	
    pronunciation->SetPhonemeSequenceL( aPronunciationPr );
    TRAPD( error, aLexicon->AddL( pronunciation ) );
    if ( error == KErrAlreadyExists )
        {
#ifdef __SIND_LEXICON_OPT
        // Take the existing pronunciation ID
        TInt index = aLexicon->Find( aPronunciationPr );
        pronunciationID = aLexicon->AtL( index ).PronunciationID();
        
        CleanupStack::PopAndDestroy( pronunciation ); 
#else       
        // try to find non-existing id in the middle of range
        RUBY_DEBUG0( "pronunID already exists" );
        RArray<TSIPronunciationID> myPronunciationID;
        myPronunciationID.Reset();
        for ( TInt i = 0; i < aLexicon->Count(); i++ )
            {
            CSIPronunciation* tmpPronunciation=&( aLexicon->AtL( i ) );
            myPronunciationID.Append( tmpPronunciation->PronunciationID() );
            }
        
        // Find a uniq new id 
        iSIDatabase->BeginTransactionL();
        pronunciationID = iSILexiconDB->GetNewID( myPronunciationID );
        iSIDatabase->CommitChangesL( ETrue );
        
        pronunciation->SetPronunciationID( pronunciationID );
        
        myPronunciationID.Close();
        aLexicon->AddL( pronunciation );
#endif // __SIND_LEXICON_OPT
        }
    else
        {
        User::LeaveIfError( error );
#ifdef __SIND_LEXICON_OPT        
        CleanupStack::Pop( pronunciation );     
#endif // __SIND_LEXICON_OPT
        }

#ifndef __SIND_LEXICON_OPT
    CleanupStack::Pop( pronunciation );
#endif // __SIND_LEXICON_OPT
    
    RUBY_DEBUG1( "CSIControllerPlugin::CreateNewPronunciationL pronunciationID=%x", pronunciationID );
    return pronunciationID;    
    }	


// -----------------------------------------------------------------------------
// CSIControllerPlugin::UpdateGrammarAndLexiconDBL
// Save TTP result into the given grammar and lexicon of the plugin database
// -----------------------------------------------------------------------------
//                                 
void CSIControllerPlugin::UpdateGrammarAndLexiconDBL( CSITtpWordList* aSITtpWordList, 
                                                      TSILexiconID aLexiconID, 
                                                      TSIGrammarID aGrammarID, 
                                                      RArray<TSIRuleID>& aRuleIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::UpdateGrammarAndLexiconDBL" );
    iSITtpWordList = aSITtpWordList;
    iLexiconID = aLexiconID;
    iGrammarID = aGrammarID;
    iRuleIDs = &aRuleIDs;
    
    aRuleIDs.Reset();
    
    iSIDatabase->BeginTransactionL();
    
    // is grammar already in database?
    iSIGrammarDB->VerifyOwnershipL( iClientUid, KGrammarIdTable, KGrammarIndex, iGrammarID ); 
    CSICompiledGrammar*  aGrammar = (CSICompiledGrammar* )iSIGrammarDB->GrammarL( iGrammarID );
    CleanupStack::PushL( aGrammar );	
    
    //is lexicion already in database?
    iSILexiconDB->VerifyOwnershipL( iClientUid, KLexiconIdTable, KLexiconIndex, iLexiconID );		   
    CSILexicon* aLexicon = iSILexiconDB->LexiconL( iLexiconID );
    CleanupStack::PushL( aLexicon );	
    
    iSIDatabase->CommitChangesL( EFalse );  
    
    
    // unpack the iSITtpWordList;
    for ( TInt i = 0; i < iSITtpWordList->Count(); i++ )
        {
        //iSIDatabase->BeginTransactionL();  
        // Get pronunciations
        RPointerArray<CSIPronunciationInfo> pronunciations;
        RArray<TSIPronunciationID> pronunciationIDs;
        CleanupClosePushL( pronunciationIDs );
        CleanupClosePushL( pronunciations );
        iSITtpWordList->GetPronunciationsL( i, pronunciations );
   
        RUBY_DEBUG1( "CSIControllerPlugin::UpdateGrammarAndLexiconDBL pronunciations.Count=%d", pronunciations.Count() );
        
        // if pronunciation generation failed, skip that word.
        if ( pronunciations.Count() == 0 )
            {
            CleanupStack::PopAndDestroy( &pronunciations ); 
            CleanupStack::PopAndDestroy( &pronunciationIDs );
            User::LeaveIfError( aRuleIDs.Append( KInvalidRuleID ) );
            continue;
            }
        
        // Create a empty rule into grammar, one name -> one rule; 
        TSIRuleID ruleID;
        
        
        CSIRule* rule=CreateNewRuleL(aGrammar);
        CleanupStack::PushL(  rule );	
        ruleID=rule->RuleID();
        User::LeaveIfError( aRuleIDs.Append( ruleID ) );
        
        for(TInt k=0;k<pronunciations.Count();k++) 
            {
            pronunciationIDs.Reset();
            
            for ( TInt n = 0; n < pronunciations[k]->Count(); n++ ) 
                {	
                // Add Prounication into lexicon
                
                TSIPronunciationID aPronunciationID = CreateNewPronunciationL( aLexicon,pronunciations[k]->PronunciationL( n ),
                    iModelBankID );
                User::LeaveIfError( pronunciationIDs.Append( aPronunciationID ) ); 
                }
            
            // Add RuleVariant into grammar
            AddNewRuleVariantL(*rule, aLexiconID, pronunciationIDs, *pronunciations[k] );                          
            }
        
        
        aGrammar->AddL( rule );
        CleanupStack::Pop( rule );
        
        // Close the arrays
        CleanupStack::PopAndDestroy( &pronunciations ); 
        CleanupStack::PopAndDestroy( &pronunciationIDs );
        }
    
    RUBY_DEBUG0( "CSIControllerPlugin::UpdateGrammarAndLexiconDBL" );
    
    iSIDatabase->BeginTransactionL();
    iSIGrammarDB->UpdateGrammarL( iClientUid,aGrammar );
    iSILexiconDB->UpdateLexiconL( iClientUid,aLexicon );
    iSIDatabase->CommitChangesL( ETrue );  
    CleanupStack::PopAndDestroy( 2 ); // aGrammar aLexicon	
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RecognitionReset
// Terminate recognition session.  Free up resources and return to IDLE state.
// -----------------------------------------------------------------------------
//                                 
void CSIControllerPlugin::RecognitionReset()
    {
    iSIModelBankDB->ResetAndDestroy();
    iSILexiconDB->ResetAndDestroy();  // Grcompiler takes the ownership of it
    iSIGrammarDB->ResetAndDestroy();
    iState = ESiPluginIdle;
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::ProcessTrainCompleteL
// Training is complete.  Store the acoustic model and user utterance into the
// model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::ProcessTrainCompleteL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::ProcessTrainCompleteL" );
    // No need to train it at all  
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::ProcessRecognitionCompleteL
// Recogntion is complete.  Transfer the result from the recognizer into the result
// object sent by the client.
// -----------------------------------------------------------------------------
//
TInt CSIControllerPlugin::ProcessRecognitionComplete()
    {
    RUBY_DEBUG0( "CSIControllerPlugin::ProcessRecognitionComplete" );
    
    const CSIResult* result = 0;
    CSIClientResult* clientResult = 0;
    
    TInt resultsAvailable = iSIResultSet->Count();
    TInt resultsWanted    = iSIClientResultSet->MaxResults();
    TInt count            = Min( resultsAvailable, resultsWanted );
    
    // Copy the results from the recognizer into client's result set object
    TInt resCount = 0;
    
    
    for ( TInt i = 0; i < count; i++ )
        {
        TRAPD( err, result = &( iSIResultSet->AtL( i ) ) );
        if ( err )
            {
            return 0; 	 	
            }
        RUBY_DEBUG2( "CSIControllerPlugin::ProcessRecognitionComplete rank(%d): grammarId(%d)", i+1, result->GrammarID() );
        RUBY_DEBUG2( "CSIControllerPlugin::ProcessRecognitionComplete ruleId(%d), score(%d)", result->RuleID(), result->Score() );

        if ( result->Score() > 0 )
            {
            TRAP( err, clientResult = CSIClientResult::NewL( result->GrammarID(), 
                                                             result->RuleID() ) );
            if ( err ) 
                {
                return 0; 	 	
                }			
            TRAP( err, iSIClientResultSet->AddL( clientResult ) );
            if ( err ) 
                {
                return 0; 	 	
                }
            resCount++;
            }
        else
            {
            break;
            }
        }
    
    iSIClientResultSet->SetResultCount( resCount );
    return resCount;
    } 

// -----------------------------------------------------------------------------
// CSIControllerPlugin::DoCancel
// Cancel handle from CActive class.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::RunL
// RunL from CActive class.  Check which message got saved and call the
// appropriate handle function.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::RunL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::RunL" );
    switch ( iRequestFunction )
        {
        case KAddRule:
            TRAP( iResult, HandleAddRuleL( iGrammarID, iLexiconID, iPronunciationID, *iRuleIDPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventAddRule, iResult );
            break;
            
        case KCreateGrammar:
            TRAP( iResult, HandleCreateGrammarL( *iGrammarIDPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventCreateGrammar, iResult );
            break;
            
        case KCreateLexicon: 
            TRAP( iResult, HandleCreateLexiconL( *iLexiconIDPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventCreateLexicon, iResult );
            break;
            
        case KCreateModelBank:
            TRAP( iResult, HandleCreateModelBankL( *iModelBankIDPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventCreateModelBank, iResult );
            break;
            
        case KGetAllClientGrammarIDs:
            TRAP( iResult, HandleGetAllClientGrammarIDsL( *iGrammarIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllClientGrammarIDs, iResult );
            break;
            
        case KGetAllClientLexiconIDs:
            TRAP( iResult, HandleGetAllClientLexiconIDsL( *iLexiconIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllClientLexiconIDs, iResult );
            break;
            
        case KGetAllClientModelBankIDs:
            TRAP( iResult, HandleGetAllClientModelBankIDsL( *iModelBankIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllClientModelBankIDs, iResult );
            break;
            
        case KGetAllGrammarIDs:
            TRAP( iResult, HandleGetAllGrammarIDsL( *iGrammarIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllGrammarIDs, iResult );
            break;
            
        case KGetAllLexiconIDs:
            TRAP( iResult, HandleGetAllLexiconIDsL( *iLexiconIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllLexiconIDs, iResult );
            break;
            
        case KGetAllModelBankIDs:
            TRAP( iResult, HandleGetAllModelBankIDsL( *iModelBankIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllModelBankIDs, iResult );
            break;
            
        case KGetAllModelIDs:
            TRAP( iResult, HandleGetAllModelIDsL( iModelBankID, *iModelIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllModelIDs, iResult );
            break;
            
        case KGetAllPronunciationIDs:
            TRAP( iResult, HandleGetAllPronunciationIDsL( iLexiconID, *iPronunciationIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllPronunciationIDs, iResult );
            break;
            
        case KGetAllRuleIDs:
            TRAP( iResult, HandleGetAllRuleIDsL( iGrammarID, *iRuleIDs ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAllRuleIDs, iResult );
            break;
            
        case KGetAvailableStorage:
            TRAP( iResult, HandleGetAvailableStorageL( *iCountPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetAvailableStorage, iResult );
            break;
            
        case KGetModelCount:
            TRAP( iResult, HandleGetModelCountL( iModelBankID, *iCountPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetModelCount, iResult );
            break;
            
        case KGetPronunciationCount:
            TRAP( iResult, HandleGetPronunciationCountL( iLexiconID, *iCountPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetPronunciationCount, iResult );
            break;
            
        case KGetRuleCount:
            TRAP( iResult, HandleGetRuleCountL( iGrammarID, *iCountPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetRuleCount, iResult );
            break;	
        case KGetRuleValidity:
            TRAP( iResult, HandleGetRuleValidityL( iGrammarID, iRuleID, *iValidPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetRuleValidity, iResult );
            break;
            
        case KGetUtteranceDuration:
            TRAP( iResult, HandleGetUtteranceDurationL( iModelBankID, iModelID, *iDurationPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventGetUtteranceDuration, iResult );
            break;
            
        case KLoadGrammar:
            TRAP( iResult, HandleLoadGrammarL( iGrammarID ) );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KLoadGrammar. HandleLoadGrammarL Left with error [%d]", iResult );
                // iGrammarDB needs to be reset also since multiple grammars can
                // be loaded and this may not be the first grammar.
                iSIModelBankDB->ResetAndDestroy();
                iSILexiconDB->ResetAndDestroy();
                iSIGrammarDB->ResetAndDestroy();
                iState = ESiPluginIdle;
                iControllerIf.SendSrsEvent( KUidAsrEventLoadGrammar, iResult );
                }
            break;
            
        case KLoadLexicon:
            TRAP(iResult, HandleLoadLexiconL( iLexiconID ) );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KLoadLexicon. HandleLoadGrammarL Left with error [%d]", iResult );

                // iLexiconDB needs to be reset also since multiple lexicons can
                // be loaded and this may not be the first lexicon.
                iSIModelBankDB->ResetAndDestroy();
                iSILexiconDB->ResetAndDestroy();
                iState = ESiPluginIdle;
                iControllerIf.SendSrsEvent( KUidAsrEventLoadLexicon, iResult );
                }
            break;
            
        case KLoadModels:
            TRAP(iResult, HandleLoadModelsL( iModelBankID ) );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KLoadModels. HandleLoadGrammarL Left with error [%d]", iResult );
                
                if ( iResult == KErrCorrupt )
                    {
                    RUBY_DEBUG0( "CSIControllerPlugin::RunL. KLoadModels. Try to update models" );
                    TRAP( iResult,
                        iSIModelBankDB->UpdateModelBankIfInvalidL( iModelBankID );
                        HandleLoadModelsL( iModelBankID );
                    );
                    }

                if ( iResult != KErrNone )
                    {
                    // iModelBankDB needs to be reset also since multiple model banks can
                    // be loaded and this may not be the first model bank.
                    iSIModelBankDB->ResetAndDestroy();
                    iState = ESiPluginIdle;
                    
                    iControllerIf.SendSrsEvent( KUidAsrEventLoadModels, iResult );
                    }
                }
            break;
            
        case KRecognize:
            TRAP( iResult, HandleRecognizeL() );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KRecognize. HandleLoadGrammarL Left with error [%d]", iResult );

                if ( iResult != KErrAsrNoMatch )
                    {
                    RecognitionReset();
                    }
                iControllerIf.SendSrsEvent( KUidAsrEventRecognition, iResult );
                }
            break;
            
        case KRecord:
            TRAP( iResult, HandleRecordL( iRecordTime ) );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KRecord. HandleLoadGrammarL Left with error [%d]", iResult );

                iControllerIf.SendSrsEvent( KUidAsrEventRecord, iResult );
                }
            break;
            
        case KRemoveGrammar:
            TRAP( iResult, HandleRemoveGrammarL( iGrammarID ) );
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveGrammar, iResult );
            break;
            
        case KRemoveLexicon:
            TRAP( iResult, HandleRemoveLexiconL( iLexiconID ) );
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveLexicon, iResult );
            break;
            
        case KRemoveModelBank:
            TRAP( iResult, HandleRemoveModelBankL( iModelBankID ) );
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveModelBank, iResult );
            break;
            
        case KRemoveModel:
            TRAP( iResult, HandleRemoveModelL( iModelBankID, iModelID ) );
            iControllerIf.SendSrsEvent( KUidAsrEventRemoveModel, iResult );
            break;
            
        case KRemovePronunciation:
            TRAP( iResult, HandleRemovePronunciationL( iLexiconID, iPronunciationID ) );
            iControllerIf.SendSrsEvent( KUidAsrEventRemovePronunciation, iResult );
            break;
            
        case KRemoveRule:
            TRAP( iResult, HandleRemoveRuleL( iGrammarID, iRuleID ) );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KRemoveRule. HandleLoadGrammarL Left with error [%d]", iResult );

                iControllerIf.SendSrsEvent( KUidAsrEventRemoveRule, iResult );
                }
            break;
        case KRemoveRules:
            TRAP( iResult, HandleRemoveRulesL( iGrammarID, *iRuleIDs ) );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KRemoveRules. HandleLoadGrammarL Left with error [%d]", iResult );

                iControllerIf.SendSrsEvent( KUidAsrEventRemoveRules, iResult );
                }
            break;
            
        case KTrain:
            TRAP( iResult, HandleTrainL( iModelBankID ) );
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KTrain. HandleLoadGrammarL Left with error [%d]", iResult );

                iControllerIf.SendSrsEvent( KUidAsrEventTrain, iResult );
                }
            break;
            
        case KUnloadRule:
            TRAP( iResult, HandleUnloadRuleL(iGrammarID, iRuleID));
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KUnloadRule. HandleLoadGrammarL Left with error [%d]", iResult );

                iControllerIf.SendSrsEvent( KUidAsrEventUnloadRule, iResult );
                }
            break;
            
            
            // SI Component
        case KAdapt:
            TRAP( iResult, HandleAdaptL(*iSIClientResultSet ,iCorrect)) ;
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KAdapt. HandleLoadGrammarL Left with error [%d]", iResult );

                iControllerIf.SendSrsEvent( KUidAsrEventAdapt , iResult );
                }
            break;
            /*	case KSIAddPronunciation:
            TRAP(iResult, HandleAddPronunciationL(iLexiconID, iTrainTextPtr,
            iLanguage, iPronunciationIDPtr)) ;
            iControllerIf.SendSrsEvent(KUidAsrEventAddPronunciation, iResult);
            break;
            */		
        case KAddRuleVariant:
            TRAP( iResult, HandleAddRuleVariantL( iGrammarID, iLexiconID, *iPronunciationIDs, iRuleID, *iRuleVariantIDPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventAddRuleVariant, iResult );				
            break;
            /*	
            case KAddVoiceTag:
            TRAP(iResult, HandleAddVoiceTagL( *iTrainArray, *iLanguageArray, 
            iLexiconID, iGrammarID, iRuleIDPtr)) ;
            iControllerIf.SendSrsEvent(KUidAsrEventAddVoiceTag, iResult);
            break;
            */	
        case KAddVoiceTags:
            TRAP( iResult, HandleAddVoiceTagsL( iTrainArrays, *iLanguageArray, iLexiconID,
                iGrammarID /* ,*iRuleIDs */) );
            // AddVoiceTags contain several async functions,
            // KUidAsrEventAddVoiceTags will be sent when the AddVoiceTags complete
            if ( iResult != KErrNone )
                {
                RUBY_DEBUG1( "CSIControllerPlugin::RunL. KAddVoiceTags. HandleLoadGrammarL Left with error [%d]", iResult );

                // Clean up the iTrainArrays
                if ( iTrainArrays )
                    {
                    iTrainArrays->ResetAndDestroy();
                    iTrainArrays->Close();
                    delete iTrainArrays;
                    iTrainArrays = NULL;
                    }
                // Send error message
                iControllerIf.SendSrsEvent( KUidAsrEventAddVoiceTags, iResult );
                }
            iTrainArrays = NULL;                
            break;

#ifdef __SINDE_TRAINING
        case KAddSindeVoiceTags:
            RUBY_DEBUG0( "CSIControllerPlugin::RunL KAddSindeVoiceTags" );
            
#ifdef _DEBUG
            for ( TInt i = 0; i < iLanguageArrayArray->Count(); i++ )
                {
                RUBY_DEBUG1( "Contents of element %d:", i );
                RLanguageArray langArr = (*iLanguageArrayArray)[i];
                for ( TInt j = 0; j < langArr.Count(); j++ )
                    {
                    RUBY_DEBUG2( "Index: %d Language: %d", j, langArr[j] );
                   }
                }
#endif // _DEBUG

            TRAP( iResult, HandleAddSindeVoiceTagsL( iTrainArrays, *iLanguageArrayArray, iLexiconID, iGrammarID ) );

            if ( iResult != KErrNone ) 
                {
                iTrainArrays->ResetAndDestroy();
                iTrainArrays->Close();
                delete iTrainArrays;
                iTrainArrays = NULL;
                
                // Send error message
                iControllerIf.SendSrsEvent( KUidAsrEventAddVoiceTags, iResult );
                }
            iTrainArrays = NULL;
            break;
#endif
        case KCreateRule:
            TRAP(iResult, HandleCreateRuleL( iGrammarID,*iRuleIDPtr ) );
            iControllerIf.SendSrsEvent( KUidAsrEventCreateRule, iResult );
            break;
            /*	case KSIRecognize:
            TRAP(iResult, HandleRecognizeL(*(CSIClientResultSet*)iSIClientResultSet)) ;
            //        iControllerIf.SendSrsEvent(KUidAsrEventSIRecognize, iResult);
            break;
            */
        case KEndRecord:
            TRAP( iResult, HandleEndRecordL( ) );
            iControllerIf.SendSrsEvent( KUidAsrEventEndRecord, iResult );
            break;
        case KUnloadGrammar:
            TRAP( iResult, HandleUnloadGrammarL( iGrammarID ) ) ;
            // UnloadGrammar contain several async functions,
            // KUidAsrEventUnloadGrammar will be sent when the UnloadGrammar complete		
            //iControllerIf.SendSrsEvent(KUidAsrEventUnloadGrammar, iResult);
            break;		
            /*case KUpdateGrammarAndLexicon:
            TRAP(iResult, HandleUpdateGrammarAndLexiconL(iSITtpWordList,iLexiconID,iGrammarID,*iRuleIDs));
            iControllerIf.SendSrsEvent(KUidAsrEventUpdateGrammarAndLexicon, iResult);				
            
              break;
            */
            
        case KPreStartSampling:
            TRAP( iResult, HandlePreStartSamplingL() );
            iControllerIf.SendSrsEvent( KUidAsrEventPreStartSampling, iResult );
            break;
            
        default:
            // No action
            break;
    }
}

// -----------------------------------------------------------------------------
// CSIControllerPlugin::DoAsynch
// This method completes the request status and set the object active
// to provide asynchronous behavior.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::DoAsynch()
    {
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete( pRS, KErrNone );
    SetActive();
    }

// ============================SI MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIControllerPlugin::AdaptL
// Calls the devasr for adaptation
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::AdaptL( CSIClientResultSet& aResultSet ,TInt aCorrect ) 
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AdaptL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KAdapt;
    
    iSIClientResultSet  = &aResultSet;
    iCorrect = aCorrect;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleAdaptL
// Calls the devasr for adaptation
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleAdaptL( CSIClientResultSet& aResultSet, 
                                        TInt aCorrect ) 
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleAdaptL" );
    if ( iState == ESiPluginRecognize )
        {
        iSIDatabase->BeginTransactionL();
        if ( iSIGrammarDB->IsGrammarLoaded() )
            
            {
            iDevASR->AdaptL( aResultSet.SIResultSet(), aCorrect );
            }
        else
            {
            // Either the loaded grammars are empty (contains no rule) or
            // all rules have been unloaded.  No need to proceed any further.
            iSIDatabase->CommitChangesL( EFalse );
            
            User::Leave( KErrAsrNoMatch );
            }
        
        iSIDatabase->CommitChangesL( EFalse );
        }
    else
        {
        // Wrong state
        User::Leave( KErrAsrInvalidState );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddPronunciationL
// Adds a new pronunciation for the given model into the specified lexicon.
// -----------------------------------------------------------------------------
//                                 
void CSIControllerPlugin::AddPronunciationL( TSILexiconID /*aLexiconID*/,
                                             const TDesC& /*aTrainText*/,
                                             TLanguage /*aLanguage*/,
                                             TSIPronunciationID& /*aPronunciationID*/)  
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AddPronunciationL" );
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddRuleVariantLtionL
// Adds a new rule variant for the given pronunciation into the specified grammar.
// -----------------------------------------------------------------------------
//                                 
void CSIControllerPlugin::AddRuleVariantL( TSIGrammarID aGrammarID,
                                           TSILexiconID aLexiconID,
                                           RArray<TSIPronunciationID>& aPronunciationIDs, 
                                           TSIRuleID aRuleID,										  
                                           TSIRuleVariantID& aRuleVariantID )
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::AddRuleVariantL");
    
    if ( IsActive() )
        {
        User::Leave(KErrServerBusy);
        }
    iRequestFunction = KAddRuleVariant;
    iGrammarID = aGrammarID;
    iRuleID = aRuleID;
    iLexiconID = aLexiconID;
    iRuleVariantIDPtr = &aRuleVariantID;	
    iPronunciationIDs = &aPronunciationIDs;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleAddRuleVariantL
// Adds a new rule variant for the given pronunciation into the specified grammar.
// -----------------------------------------------------------------------------
//                                 
void CSIControllerPlugin::HandleAddRuleVariantL( TSIGrammarID aGrammarID, 
                                                 TSILexiconID aLexiconID,
                                                 RArray<TSIPronunciationID>& aPronunciationIDs, 
                                                 TSIRuleID aRuleID,
                                                 TSIRuleVariantID& aRuleVariantID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleAddRuleVariantL" );
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSIGrammarDB->AddRuleVariantL( iClientUid, aGrammarID, aLexiconID, 
                                       aPronunciationIDs, aRuleID, aRuleVariantID ); 
        iSIDatabase->CommitChangesL( ETrue );
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddVoiceTagL
// Trains a new voice tag.
// -----------------------------------------------------------------------------
//  
void CSIControllerPlugin::AddVoiceTagL( MDesCArray& /*aTrainArray*/, 
                                        RArray<TLanguage>& /*aLanguageArray*/, 
                                        TSILexiconID /*aLexiconID*/,
                                        TSIGrammarID /*aGrammarID*/, 
                                        TSIRuleID& /*aRuleID*/)
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AddVoiceTagL" );
    User::Leave( KErrNotSupported );
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddVoiceTagsL
// Adds several new voice tags.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::AddVoiceTagsL( RPointerArray<MDesCArray>& aTrainArrays,
                                         RArray<TLanguage>& aLanguageArray, 
                                         TSILexiconID aLexiconID,
                                         TSIGrammarID aGrammarID, 
                                         RArray<TSIRuleID>& aRuleIDs)
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::AddVoiceTagsL");
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
        
    // Use low priority for training (if training is not done from Voice Commands app)        
    if ( RProcess().SecureId() != KVoiceCommandsAppUid )
        {
        RThread().SetPriority( EPriorityAbsoluteLow );
        }
    
    iState = ESiPluginTrainText;
    //	iTtpState=EAddVoiceTags;
    
    iRequestFunction = KAddVoiceTags ;
    iLexiconID= aLexiconID;
    iGrammarID = aGrammarID;
    
    iRuleIDs = &aRuleIDs;
    iLanguageArray = &aLanguageArray;
    iTrainArrays = &aTrainArrays;
    
    DoAsynch();
    }

#ifdef __SINDE_TRAINING
// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddSindeVoiceTagL
// Trains a new voice tag.
// -----------------------------------------------------------------------------
//  
void CSIControllerPlugin::AddSindeVoiceTagL( MDesCArray& /*aTrainArray*/, 
                                             RArray<RLanguageArray>& /*aLanguageArray*/, 
                                             TSILexiconID /*aLexiconID*/,
                                             TSIGrammarID /*aGrammarID*/, 
                                             TSIRuleID& /*aRuleID*/ )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AddSindeVoiceTagL" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::AddSindeVoiceTagsL
// Adds several new voice tags.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::AddSindeVoiceTagsL( RPointerArray<MDesCArray>& aTrainArrays,
                                              RArray<RLanguageArray>& aLanguageArray, 
                                              TSILexiconID aLexiconID,
                                              TSIGrammarID aGrammarID, 
                                              RArray<TSIRuleID>& aRuleIDs )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::AddSindeVoiceTagsL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    // Use low priority for training (if training is not done from Voice Commands app)
    if ( RProcess().SecureId() != KVoiceCommandsAppUid )
        {
        RThread().SetPriority( EPriorityAbsoluteLow );
        }
    
    iState = ESiPluginTrainSinde;
    
    iRequestFunction = KAddSindeVoiceTags;
    iLexiconID = aLexiconID;
    iGrammarID = aGrammarID;
    
    // Store parameters for asynchronous processing
    iRuleIDs = &aRuleIDs;
    iLanguageArrayArray = &aLanguageArray;
    iTrainArrays = &aTrainArrays;
    
    DoAsynch();
    }
#endif // __SINDE_TRAINING

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleAddVoiceTagsL
// Adds several new voice tags.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleAddVoiceTagsL( RPointerArray<MDesCArray>* aTrainArrays,
                                               RArray<TLanguage>& aLanguageArray, 
                                               TSILexiconID aLexiconID,
											   TSIGrammarID aGrammarID ) 
    {
    RUBY_DEBUG_BLOCK("CSIControllerPlugin::HandleAddVoiceTagsL");
    // 
    // first test if both KLexiconID lexicon and KGrammarID are created
    if ( IsLexiconIDInvalid( aLexiconID ) || IsGrammarIDInvalid( aGrammarID ) )
        {
        User::Leave( KErrNotFound ); // no such grammar exist in Database
        }
    // Need to clean the iSITtpWordList before adding new arrays
    delete iSITtpWordList;
    iSITtpWordList = NULL;
    iSITtpWordList = CSITtpWordList::NewL();
    if ( iState == ESiPluginTrainText )
        { 
        for ( TInt i=0; i < aTrainArrays->Count(); i++ )
            {            
            iSITtpWordList->AddL( ( *aTrainArrays )[i] );
            // If next AddL fails, we must not delete the objects twice - once in
            // aTrainArray->ResetAndDestroy() and second time in ~CSITtpWordList.
            ( *aTrainArrays )[i] = NULL;
            }
        aTrainArrays->ResetAndDestroy();
        aTrainArrays->Close();
        delete aTrainArrays;
        aTrainArrays = NULL;
        
        // Set Max prnounication per word 
        iMaxNPronunsForWord.Reset();	
        iMaxNPronunsForWord.Append( aLanguageArray.Count() );
        
        RUBY_DEBUG1( "CSIControllerPlugin::AddVoiceTagsL(aLanguageArray.Count=%d)", aLanguageArray.Count() );
        
        iDevASR->StartTrainingFromTextL(*iSITtpWordList,aLanguageArray,iMaxNPronunsForWord);
        }
    else
        {   
        // Wrong state
        User::Leave( KErrAsrInvalidState );
        }
    }

#ifdef __SINDE_TRAINING
// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleAddVoiceTagsL
// Adds several new voice tags.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleAddSindeVoiceTagsL( RPointerArray<MDesCArray>* aTrainArrays,
                                                    RArray<RLanguageArray>& aLanguageArray, 
                                                    TSILexiconID aLexiconID,
											        TSIGrammarID aGrammarID ) 
    {
    // First test if both KLexiconID lexicon and KGrammarID are created
    if ( IsLexiconIDInvalid( aLexiconID ) || IsGrammarIDInvalid( aGrammarID ) )
        {
        User::Leave( KErrNotFound ); // no such grammar exist in Database
        }
  
    iSindeTrainer->AddSindeVoiceTagsL( aTrainArrays, aLanguageArray, 
                                       aLexiconID, aGrammarID, iModelBankID, 
                                       iClientUid, *iRuleIDs );

    }
#endif // __SINDE_TRAINING

// -----------------------------------------------------------------------------
// CSIControllerPlugin::IsGrammarIDInvalid
// is the grammar id valid in the database
// -----------------------------------------------------------------------------
//
TBool CSIControllerPlugin::IsGrammarIDInvalid( TSIGrammarID aGrammarID )
    {
    TInt i(0);
    RArray<TSIGrammarID> aGrammarIDs;
    
    TRAPD( error, 
        iSIDatabase->BeginTransactionL();
        iSIGrammarDB->GetAllGrammarIDsL( aGrammarIDs );
        iSIDatabase->CommitChangesL( EFalse );
        ); // TRAPD
    
    if ( error == KErrNone )
        {
        for( i = 0; i < aGrammarIDs.Count(); i++ ) 
            {
            if ( aGrammarIDs[i] == aGrammarID )
                {
                break;
                }
            if ( i == aGrammarIDs.Count() )
                {
                aGrammarIDs.Close();
                return ETrue; // no such Lexicon exist in Database
                }
            }
        }
    aGrammarIDs.Close();
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::IsLexiconIDInvalid
// is the lexicon id valid in the database
// -----------------------------------------------------------------------------
//
TBool CSIControllerPlugin::IsLexiconIDInvalid( TSILexiconID aLexiconID )
    {
    // first test if both KLexiconID lexicon and KLexiconID are created
    TInt i(0);
    RArray<TSILexiconID> aLexiconIDs;

    TRAPD( error,     
        iSIDatabase->BeginTransactionL();
        iSILexiconDB->GetAllLexiconIDsL( aLexiconIDs );
        iSIDatabase->CommitChangesL( EFalse );
        ); //TRAPD
        
    if ( error == KErrNone )
        {
        for( i = 0; i < aLexiconIDs.Count(); i++ ) 
            {
            if ( aLexiconIDs[i] == aLexiconID ) 
                {
                break;
                }
            if ( i == aLexiconIDs.Count() ) 
                {
                aLexiconIDs.Close();
                return ETrue; // no such Lexicon exist in Database
                }
            }
        }

    aLexiconIDs.Close();
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CSIControllerPlugin::CreateRuleL
// Creates a new empty rule.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::CreateRuleL( TSIGrammarID aGrammarID, 
                                       TSIRuleID& aRuleID ) 
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::CreateRuleL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KCreateRule ;
    iGrammarID = aGrammarID; 
    iRuleIDPtr= &aRuleID;
    
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleCreateRuleL
// Creates a new empty rule.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleCreateRuleL( TSIGrammarID aGrammarID, 
                                             TSIRuleID& aRuleID ) 
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleCreateRuleL" );
    if ( iClientRegistered )
        {
        iSIDatabase->BeginTransactionL();
        iSIGrammarDB->CreateRuleL( iClientUid, aGrammarID, aRuleID ); 
        iSIDatabase->CommitChangesL( EFalse );
        }
    else
        {
        User::Leave( KErrAsrNotRegisted );
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::EndRecordL
// End recordingEnds recording. Unlike Cancel(), this function may return a 
// recognition result if adequate number of samples was already recorded.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::EndRecordL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::EndRecordL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KEndRecord ; 
    DoAsynch();
}

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleEndRecordL
// End recordingEnds recording. Unlike Cancel(), this function may return a 
// recognition result if adequate number of samples was already recorded.
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleEndRecordL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleEndRecordL" );
    iDevASR->EndRecord();	
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::UnloadGrammarL
// Unload a grammar from memory
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::UnloadGrammarL( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::UnloadGrammarL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    iRequestFunction = KUnloadGrammar ;
    iGrammarID=aGrammarID;
    DoAsynch();
    }

// -----------------------------------------------------------------------------
// CSIControllerPlugin::HandleUnloadGrammarL
// Unload a grammar from memory
// -----------------------------------------------------------------------------
//
void CSIControllerPlugin::HandleUnloadGrammarL( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPlugin::HandleUnloadGrammarL" );
    
    iSIDatabase->BeginTransactionL();
    CSICompiledGrammar* LoadedGrammar =iSIGrammarDB->FindGrammarL( aGrammarID );	 
    iSIDatabase->CommitChangesL( EFalse );
        
    iDevASR->UnloadGrammar( *LoadedGrammar );	// async call to unload grammar in devasr
    }

//  End of File
