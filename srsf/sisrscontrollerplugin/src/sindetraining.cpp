/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for CSindeTrainer
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>
#include <numberconversion.h>
#include "sindetraining.h"
#include "rubydebug.h"
#include "sidatabase.h"
#include "sigrammardb.h"
#include "silexicondb.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSindeTrainer::CSindeTrainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSindeTrainer::CSindeTrainer( CDevASR& aDevAsr,
                              CSIControllerPluginInterface& aControllerIf,
                              CSIControllerPlugin& aPlugin,
                              CSIDatabase& aDatabase,
                              CSIGrammarDB& aGrammarDatabase,
                              CSILexiconDB& aLexiconDatabase ) :
                              CActive( EPriorityStandard ), 
                              iState( ESindeTrainerIdle ),
                              iDevAsr( aDevAsr ),
                              iControllerIf( aControllerIf ),
                              iPlugin( aPlugin ),
						      iDatabase( aDatabase ),
                              iGrammarDatabase( aGrammarDatabase ),
                              iLexiconDatabase( aLexiconDatabase )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSindeTrainer::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CSindeTrainer::ConstructL" );
    
    // Add active object to active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSindeTrainer* CSindeTrainer::NewL( CDevASR& aDevAsr,
                                    CSIControllerPluginInterface& aControllerIf,
                                    CSIControllerPlugin& aPlugin,
                                    CSIDatabase& aDatabase,
                                    CSIGrammarDB& aGrammarDatabase,
                                    CSILexiconDB& aLexiconDatabase )
    {
    RUBY_DEBUG_BLOCK( "CSindeTrainer::NewL" );
    CSindeTrainer* self = new( ELeave ) CSindeTrainer( aDevAsr, aControllerIf, aPlugin,
     										           aDatabase, aGrammarDatabase,
     										           aLexiconDatabase );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );	
    return self;
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::~CSindeTrainer
// Destructor
// -----------------------------------------------------------------------------
//
CSindeTrainer::~CSindeTrainer()
    {
    RUBY_DEBUG0( "CSindeTrainer::~CSindeTrainer" );
    
    Cancel();

    delete iTtpWordList;
    iMaxPronunsForWord.Close();
    
    iTtpDataArray.ResetAndDestroy();
    iTtpDataArray.Close();
    
    if ( iTrainArrays != NULL )
        {
        iTrainArrays->ResetAndDestroy();
        iTrainArrays->Close();
        delete iTrainArrays;
        }
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::SetReady
// Sets the object active and signals active scheduler to run it.
// -----------------------------------------------------------------------------
//
void CSindeTrainer::SetReady()
    {
    TRequestStatus* pRS = &iStatus;
    User::RequestComplete( pRS, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::RunL
// RunL of CActive
// -----------------------------------------------------------------------------
//
void CSindeTrainer::RunL()
    {
    RUBY_DEBUG0( "CSindeTrainer::RunL" );
    
    switch ( iState )
        {
        case ESindeTrainerTraining:
            {
            TRAPD( error, DoOneTrainingStepL() );
            if ( error != KErrNone )
                {
                // Move to next state
                iState = ESindeTrainerDbUpdate;
                SetReady();
                }
            break;
            }
        
        case ESindeTrainerDbUpdate:
            {
            TRAPD( error, DoDatabaseUpdateL() );
            iError = error;
            iState = ESindeGrammarCompilation;
            SetReady();
            break;
            }
        
        case ESindeGrammarCompilation:
            {
            TRAPD( error, DoGrammarCompilationL() );
            iError = error;
            if ( error != KErrNone )
                {
                // Move to next state
                iState = ESindeTrainerFinished;
                SetReady();                
                }
            break;
            }
        
        case ESindeTrainerFinished:
            {
            RUBY_DEBUG1( "CSindeTrainer::RunL sending KUidAsrEventAddVoiceTags callback with error: %d", iError );
            // All done, do callback to client thread     
            iControllerIf.SendSrsEvent( KUidAsrEventAddVoiceTags, iError );
            iState = ESindeTrainerIdle;            
            break;   
            }
        
        default:
            RUBY_ERROR0( "CSindeTrainer::RunL unexpected state" );
            break;
       
        }
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::DoCancel
// DoCancel of CActive
// -----------------------------------------------------------------------------
//
void CSindeTrainer::DoCancel()
    {
    RUBY_DEBUG0( "CSindeTrainer::DoCancel" );
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::HandleEvent
// Handles event originated from DevASR
// -----------------------------------------------------------------------------
//
void CSindeTrainer::HandleEvent( TDevASREvent aEvent, TDevASRError aError )
    {
    RUBY_DEBUG2( "CSindeTrainer::HandleEvent event: %d error: %d", aEvent, aError );
    
    if ( aError == KErrNone )
        {
        if ( aEvent == EDevASRTrainFromText )
            {
            // Store the data
            iTtpDataArray.Append( iTtpWordList );
            // Ownership transferred to array
            iTtpWordList = NULL;
            }
        else if ( aEvent == EDevASRGrammarCompile )
            {
            StoreCompiledGrammar();
            iState = ESindeTrainerFinished;
            }
        }
    
    // Do the next step in RunL
    SetReady();
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::AddSindeVoiceTagsL
// Starts training of SINDE voice tags
// -----------------------------------------------------------------------------
//
void CSindeTrainer::AddSindeVoiceTagsL( RPointerArray<MDesCArray>* aTrainArrays,
	                                    RArray<RLanguageArray>& aLanguageArray, 
                                        TSILexiconID aLexiconId,
                                        TSIGrammarID aGrammarId,
                                        TSIModelBankID aModelBankId,
                                        TUid aClientUid,
                                        RArray<TSIRuleID>& aRuleIds )
    {
    RUBY_DEBUG_BLOCK( "CSindeTrainer::AddSindeVoiceTagsL" );
    
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
        
    // Start reading the input arrays from zero index
    iLanguageIndex = 1;
    iTextIndex = 0;
    
    // Destruct previous input if it still exists
    if ( iTrainArrays != NULL )
        {
        iTrainArrays->ResetAndDestroy();
        iTrainArrays->Close();
        delete iTrainArrays;
        iTrainArrays = NULL;
        }
    
    // Store parameters for asynchronous processing
    iTrainArrays = aTrainArrays;
    iLanguageArray = &aLanguageArray;
    iLexiconId = aLexiconId;
    iGrammarId = aGrammarId;
    iClientUid = aClientUid;
    iModelBankId = aModelBankId;
    iRuleIds = &aRuleIds;
    
    iState = ESindeTrainerTraining;
    SetReady();
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::DoOneTrainingStep
// Starts one step of training using DevASR
// -----------------------------------------------------------------------------
//
void CSindeTrainer::DoOneTrainingStepL()
    {
    RUBY_DEBUG_BLOCK( "CSindeTrainer::DoOneTrainingStepL" );
    
    // Delete the previous DevASR input data
    delete iTtpWordList;
    iTtpWordList = NULL;
    iTtpWordList = CSITtpWordList::NewL();
    
    for ( TInt i = 0; i < iTrainArrays->Count(); i++ )
        {
        MDesCArray* originalText = (*iTrainArrays)[i];
            
        CDesC16ArrayFlat* wordArray = new ( ELeave ) CDesC16ArrayFlat( 1 );
        CleanupStack::PushL( wordArray );
            
        // Loop through all descriptors that are needed for this group and
        // add them to new wordArray
        for ( TInt j = 0; j < originalText->MdcaCount(); j++ )
            {
            TInt textLength = originalText->MdcaPoint( j ).Length();
            if ( textLength > 0 )
                {
                TPtrC firstChar( originalText->MdcaPoint( j ).Left( 1 ) );
                TInt length( 0 );
                TInt result( 0 );
                TDigitType type;
                result = NumberConversion::ConvertFirstNumber( firstChar, length, type );
                
                if ( result == iLanguageIndex )
                    {
                    // Take off the first character
                    TPtrC text( originalText->MdcaPoint( j ).Right( textLength - 1 ) );
                    wordArray->AppendL( text ); 
                    }
                }
            }
            
            // Append empty descriptor if nothing else was found
            if ( wordArray->Count() == 0 )
                {
                wordArray->AppendL( KNullDesC ); 
                }
                
        iTtpWordList->AddL( wordArray );
        // Ownership transferred to iTtpWordList
        CleanupStack::Pop( wordArray );
        }
        
    // Decrease 1 to get the current index, iLanguageIndex starts from 1
    TInt currentLanguageIndex( iLanguageIndex - 1 );
        
     // Check that there is legal index for languageArray available
    if ( ( currentLanguageIndex < 0 ) || ( currentLanguageIndex >= iLanguageArray->Count() ) )
        {
        RUBY_ERROR0( "CSindeTrainer::DoOneTrainingStep language array index out of bounds" );
        User::Leave( KErrNotFound );
        }
        
    iMaxPronunsForWord.Reset();	
    iMaxPronunsForWord.Append( (*iLanguageArray)[currentLanguageIndex].Count() );
        
    // Do the training call to DevASR
    iDevAsr.StartTrainingFromTextL( *iTtpWordList, (*iLanguageArray)[currentLanguageIndex], iMaxPronunsForWord );

    // Move to next group          
    iLanguageIndex++;
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::DoDatabaseUpdateL
// Stores the training results into grammar and lexicon and updates the DB
// -----------------------------------------------------------------------------
//
void CSindeTrainer::DoDatabaseUpdateL()
    {
    RUBY_DEBUG_BLOCK( "CSindeTrainer::DoDatabaseUpdateL" );
   
    iRuleIds->Reset();
   
    iDatabase.BeginTransactionL();
    
    // Get the grammar
    iGrammarDatabase.VerifyOwnershipL( iClientUid, KGrammarIdTable, KGrammarIndex, iGrammarId ); 
    CSICompiledGrammar* grammar = ( CSICompiledGrammar* ) iGrammarDatabase.GrammarL( iGrammarId );
    CleanupStack::PushL( grammar );	
    
    // Get the lexicon
    iLexiconDatabase.VerifyOwnershipL( iClientUid, KLexiconIdTable, KLexiconIndex, iLexiconId );		   
    CSILexicon* lexicon = iLexiconDatabase.LexiconL( iLexiconId );
    CleanupStack::PushL( lexicon );	
    
    iDatabase.CommitChangesL( EFalse ); 
    
    // There should be at least one element in ttp data array
    if ( iTtpDataArray.Count() > 0 )
    	{
    	// Take the first word list
        CSITtpWordList* ttpWordList = iTtpDataArray[0];
        
        for ( TInt i = 0; i < ttpWordList->Count(); i++ )
            {
            RPointerArray<CSIPronunciationInfo> pronunciations;
            RArray<TSIPronunciationID> pronunciationIDs;
            CleanupClosePushL( pronunciationIDs );
            CleanupClosePushL( pronunciations );
            // Take the pronunciations from ttp output
            ttpWordList->GetPronunciationsL( i, pronunciations );

            CSIRule* rule = iPlugin.CreateNewRuleL( grammar );
            CleanupStack::PushL( rule );	
            
            // if pronunciation generation failed, skip that word.
            if ( pronunciations.Count() == 0 )
                {
                User::LeaveIfError( iRuleIds->Append( KInvalidRuleID ) );
                CleanupStack::PopAndDestroy( rule );
                }
            else
                {
            	for ( TInt k = 0; k < pronunciations.Count(); k++ ) 
                	{
               		pronunciationIDs.Reset();
            
                    for ( TInt n = 0; n < pronunciations[k]->Count(); n++ ) 
                        {
                    	// Add Pronunciation into lexicon
                        TDesC8& pronunciation = pronunciations[k]->PronunciationL( n );
                        TSIPronunciationID pronunId = iPlugin.CreateNewPronunciationL( lexicon, pronunciation, iModelBankId );
                        User::LeaveIfError( pronunciationIDs.Append( pronunId ) );
                	    }

                    AppendPronunciationsL( i, *lexicon, pronunciationIDs );
            
            	    // Add RuleVariant into grammar
            	    iPlugin.AddNewRuleVariantL( *rule, iLexiconId, pronunciationIDs, *pronunciations[k] );
            	    }
        
        	    grammar->AddL( rule );
        	    CleanupStack::Pop( rule );
            
                // Add rule to client side array      
                User::LeaveIfError( iRuleIds->Append( rule->RuleID() ) );
                }
                   
        	// Close the arrays
        	CleanupStack::PopAndDestroy( &pronunciations ); 
        	CleanupStack::PopAndDestroy( &pronunciationIDs );

            } // for ( TInt i = 0; i < ttpWordList->Count(); i++ )

        // Do the updates into the database
        iDatabase.BeginTransactionL();
        iGrammarDatabase.UpdateGrammarL( iClientUid, grammar );
        iLexiconDatabase.UpdateLexiconL( iClientUid, lexicon );
        // Commit changes to db with compact
        iDatabase.CommitChangesL( ETrue );  
        } // if ( iTtpDataArray.Count() > 0 )

    CleanupStack::PopAndDestroy( lexicon );
    CleanupStack::PopAndDestroy( grammar );	
        
    // All data has been stored elsewhere
    iTtpDataArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::DoGrammarCompilationL
// Does grammar compilation using DevASR
// -----------------------------------------------------------------------------
//
void CSindeTrainer::DoGrammarCompilationL()
    {
    iDatabase.BeginTransactionL();
    iCompiledGrammar = ( CSICompiledGrammar* )iGrammarDatabase.LoadGrammarL( iGrammarId );
    iDatabase.CommitChangesL( EFalse );
            
    iDevAsr.CompileGrammarL( *iCompiledGrammar );
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::StoreCompiledGrammar
// Stores compiled grammar to plugin database
// -----------------------------------------------------------------------------
//
void CSindeTrainer::StoreCompiledGrammar()
    {
    TRAP_IGNORE( 
        iDatabase.BeginTransactionL();
        // save the compiled grammar
        iGrammarDatabase.UpdateGrammarL( iClientUid, iCompiledGrammar );
        iDatabase.CommitChangesL( ETrue );
        ); // TRAP_IGNORE
    }

// -----------------------------------------------------------------------------
// CSindeTrainer::AppendPronunciationsL
// Appends pronunciations from other ttp word lists than the first one
// -----------------------------------------------------------------------------
//
void CSindeTrainer::AppendPronunciationsL( TInt aIndex, CSILexicon& aLexicon, 
                                           RArray<TSIPronunciationID>& aPronunIds )
    {
    // Append pronunciations also from other word lists
    for ( TInt counter = 1; counter < iTtpDataArray.Count(); counter++ )
        {
        CSITtpWordList* ttpWordList = iTtpDataArray[counter];
        RPointerArray<CSIPronunciationInfo> pronunciations;
        CleanupClosePushL( pronunciations );
        // Take the same index as from first word list
        ttpWordList->GetPronunciationsL( aIndex, pronunciations );
                        
        for ( TInt pronunArrayCounter = 0; pronunArrayCounter < pronunciations.Count(); pronunArrayCounter++ )
            {
            for ( TInt pronunCounter = 0; pronunCounter < pronunciations[pronunArrayCounter]->Count(); pronunCounter++ )
                {
                TDesC8& pronun = pronunciations[pronunArrayCounter]->PronunciationL( pronunCounter );
                TSIPronunciationID pronunId = iPlugin.CreateNewPronunciationL( &aLexicon, pronun, iModelBankId );
                User::LeaveIfError( aPronunIds.Append( pronunId ) );
                }
            }
            CleanupStack::PopAndDestroy( &pronunciations ); 
        }
    }
    
// End of File
