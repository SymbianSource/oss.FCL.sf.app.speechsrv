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
* Description:  This is the main interface of the SI Controller Plugin.
*
*/


// INCLUDE FILES
#include "srsfbldvariant.hrh"
#include <mmfcontrollerframeworkbase.h>
#include <nsssispeechrecognitioncustomcommandparser.h>
#include <implementationproxy.h>
#include "sicontrollerplugininterface.h"
#include "sicontrollerplugin.h"
#include "sipluginimplementationuids.hrh"
#include "rubydebug.h"

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::CSIControllerPluginInterface
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSIControllerPluginInterface::CSIControllerPluginInterface()
    {
    RUBY_DEBUG0( "CSIControllerPluginInterface::CSIControllerPluginInterface" );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
#ifndef __CONSOLETEST__

void CSIControllerPluginInterface::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPluginInterface::ConstructL" );
    
    iController = CSIControllerPlugin::NewL(*this);
    
    CSISpeechRecognitionCustomCommandParser* speechRecognitionParser =
        CSISpeechRecognitionCustomCommandParser::NewL(*this);
    CleanupStack::PushL(speechRecognitionParser);
    AddCustomCommandParserL(*speechRecognitionParser);
    // The parser is owned by the controller framework
    CleanupStack::Pop(speechRecognitionParser);
    }

#else	// __CONSOLETEST__ defined

// Unit Test
void CSIControllerPluginInterface::ConstructL( MSrsObserver* aObserver )
    {
    iController = CSIControllerPlugin::NewL(*this);
    iObserver = aObserver;
    }

// Unit Test
void CSIControllerPluginInterface::ConstructL()
    {
    iController = CSIControllerPlugin::NewL(*this);
    iObserver = NULL;
    }

#endif	// __CONSOLETEST__

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIControllerPluginInterface* CSIControllerPluginInterface::NewL()
    {
    RUBY_DEBUG_BLOCK( "CSIControllerPluginInterface::NewL" );
    
    CSIControllerPluginInterface* self = new( ELeave ) CSIControllerPluginInterface;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::~CSIControllerPluginInterface
// Destructor.
// -----------------------------------------------------------------------------
//
CSIControllerPluginInterface::~CSIControllerPluginInterface()
    {
    RUBY_DEBUG0( "CSIControllerPluginInterface::~CSIControllerPluginInterface" );
    delete iController;
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::AddDataSourceL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::AddDataSourceL( MDataSource& /*aDataSource*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::AddDataSinkL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::AddDataSinkL( MDataSink& /*aDataSink*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::RemoveDataSourceL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::RemoveDataSourceL( MDataSource& /*aDataSource*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::RemoveDataSinkL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::RemoveDataSinkL( MDataSink& /*aDataSink*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::ResetL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::ResetL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::PrimeL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::PrimeL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::PlayL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::PlayL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::PauseL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::PauseL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::StopL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::StopL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::PositionL
// This function is not supported.
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CSIControllerPluginInterface::PositionL() const
    {
    User::Leave( KErrNotSupported );
    return TTimeIntervalMicroSeconds( 0 ); // Just to make the compiler happy
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::SetPositionL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::SetPositionL( const TTimeIntervalMicroSeconds& /*aPosition*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::DurationL
// This function is not supported.
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CSIControllerPluginInterface::DurationL() const
    {
    User::Leave( KErrNotSupported );
    return TTimeIntervalMicroSeconds( 0 ); // Just to make the compiler happy
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::SetPrioritySettings
// Set the priority settings for this controller.  This is used during recording
// and playback.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::SetPrioritySettings( const TMMFPrioritySettings& aPrioritySettings )
    {
    iController->SetPrioritySettings( aPrioritySettings );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::GetNumberOfMetaDataEntriesL
// This function is not supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::GetNumberOfMetaDataEntriesL( TInt& /*aNumberOfEntries*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::GetMetaDataEntryL
// This function is not supported.
// -----------------------------------------------------------------------------
//
CMMFMetaDataEntry* CSIControllerPluginInterface::GetMetaDataEntryL( TInt /*aIndex*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL; // Just to make the compiler happy
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddPronunciationL
// Adds a new pronunciation into the lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddPronunciationL( TSILexiconID aLexiconID,
                                                         TSIModelBankID aModelBankID,
                                                         TSIModelID aModelID,
                                                         TSIPronunciationID& aPronunciationID )
    {
    iController->AddPronunciationL(aLexiconID, aModelBankID, aModelID, aPronunciationID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddRuleL
// Adds a new rule into the grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddRuleL( TSIGrammarID /*aGrammarID*/,
                                                TSILexiconID /*aLexiconID*/,
                                                TSIPronunciationID /*aPronunciationID*/,
                                                TSIRuleID& /*aRuleID*/ )
    {
    User::Leave( KErrNotSupported );	
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrCancel
// Cancels the current activity and returns the plugin to Idle state.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrCancel()
    {
    iController->Cancel();
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrCommitTrainL
// Training is complete.  Call model database handler to save the model into the
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrCommitChangesL()
    {
    iController->CommitChangesL( ETrue );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrCreateGrammarL
// Creates a new grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrCreateGrammarL( TSIGrammarID& aGrammarID )
    {
    iController->CreateGrammarL( aGrammarID );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrCreateLexiconL
// Creates a new lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrCreateLexiconL( TSILexiconID& aLexiconID )
    {
    iController->CreateLexiconL( aLexiconID );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrCreateModelBankL
// Creates a new model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrCreateModelBankL( TSIModelBankID& aModelBankID )
    {
    iController->CreateModelBankL( aModelBankID );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrEndRecSessionL
// Ends recognition session and releases resources.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrEndRecSessionL()
    {
    iController->EndRecSessionL();
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllClientGrammarIDsL
// Returns all grammar Ids that belong to the current client.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllClientGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs )
    {
    iController->GetAllClientGrammarIDsL( aGrammarIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllClientLexiconIDsL
// Returns all lexicon Ids that belong to the current client.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllClientLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs )
    {
    iController->GetAllClientLexiconIDsL( aLexiconIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllClientModelBankIDsL
// Returns all model bank Ids that belong to the current client.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllClientModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs )
    {
    iController->GetAllClientModelBankIDsL( aModelBankIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllGrammarIDsL
// Returns all grammar Ids that exist (for all clients).
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllGrammarIDsL( RArray<TSIGrammarID>& aGrammarIDs )
    {
    iController->GetAllGrammarIDsL( aGrammarIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllLexiconIDsL
// Returns all lexicon Ids that exist (for all clients).
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllLexiconIDsL( RArray<TSILexiconID>& aLexiconIDs )
    {
    iController->GetAllLexiconIDsL( aLexiconIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllModelBankIDsL
// Returns all model bank Ids that exist (for all clients).
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs )
    {
    iController->GetAllModelBankIDsL( aModelBankIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllModelIDsL
// Gets all model IDs in the model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllModelIDsL( TSIModelBankID aModelBankID,
                                                       RArray<TSIModelID>& aModelIDs )
    {
    iController->GetAllModelIDsL( aModelBankID, aModelIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllPronunciationIDsL
// Gets all pronunciation IDs in the lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllPronunciationIDsL( TSILexiconID aLexiconID,
                                                               RArray<TSIPronunciationID>& aPronunciationIDs )
    {
    iController->GetAllPronunciationIDsL( aLexiconID, aPronunciationIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAllRuleIDsL
// Gets all rule IDs in the grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAllRuleIDsL( TSIGrammarID aGrammarID,
                                                      RArray<TSIRuleID>& aRuleIDs )
    {
    iController->GetAllRuleIDsL( aGrammarID, aRuleIDs );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetAvailableStorageL
// Calculates the available number of models that can be trained by subtracting
// the current total from the system defined max.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetAvailableStorageL( TInt& /*aCount*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetEnginePropertiesL
// Returns the current recognition engine properties.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetEnginePropertiesL( const RArray<TInt>& aPropertyId,
                                                            RArray<TInt>& aPropertyValue )
    {
    iController->GetEnginePropertiesL( aPropertyId, aPropertyValue );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetModelCountL
// Gets the number of models in a model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetModelCountL( TSIModelBankID aModelBankID,
													  TInt& aCount )
    {
    iController->GetModelCountL( aModelBankID, aCount );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetPronunciationCountL
//  
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetPronunciationCountL( TSILexiconID aLexiconID, 
                                                              TInt& aCount )
    {
    iController->GetPronunciationCountL( aLexiconID, aCount );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetRuleCountL
//  
// -----------------------------------------------------------------------------
//	
void CSIControllerPluginInterface::MSrGetRuleCountL( TSIGrammarID aGrammarID, TInt& aCount )
    {
    iController->GetRuleCountL(aGrammarID, aCount);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetRuleValidityL
// Checks to see if a rule exists in the specified grammar or not.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetRuleValidityL( TSIGrammarID aGrammarID,
                                                        TSIRuleID aRuleID,
                                                        TBool& aValid )
    {
    iController->GetRuleValidityL(aGrammarID, aRuleID, aValid);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrGetUtteranceDurationL
// Gets the duration of an utterance.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrGetUtteranceDurationL( TSIModelBankID /*aModelBankID*/,
                                                             TSIModelID /*aModelID*/,
                                                             TTimeIntervalMicroSeconds32& /*aDuration*/ )
    {
    User::Leave(KErrNotSupported);	
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrLoadEngineParametersL
// Loads the specified recognizer parameter(s).  These parameters are used to
// alter the recognizer's default parameters.  The parameters are specified as
// attribute and value pairs.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrLoadEngineParametersL( const RArray<TInt>& aParameterId,
                                                             const RArray<TInt>& aParameterValue )
    {
    iController->LoadEngineParametersL(aParameterId, aParameterValue);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrLoadGrammarL
// Loads the specified grammar into the recognizer.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrLoadGrammarL( TSIGrammarID aGrammarID )
    {
    iController->LoadGrammarL(aGrammarID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrLoadLexiconL
// Loads the specified lexicon into the recognizer.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrLoadLexiconL( TSILexiconID /*aLexiconID*/ )
    {
    User::Leave(KErrNotSupported);	
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrLoadModelsL
// Loads the specified model bank into the recognizer.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrLoadModelsL( TSIModelBankID aModelBankID)
    {
    iController->LoadModelsL(aModelBankID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrActivateGrammarL
//  
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrActivateGrammarL( TSIGrammarID aGrammarID )
    {
    iController->ActivateGrammarL(aGrammarID );
    } 
// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrDeactivateGrammarL
//  
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrDeactivateGrammarL( TSIGrammarID aGrammarID )
    {
    iController->DeactivateGrammarL(aGrammarID);
    } 

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrPlayUtteranceL
// Plays the user utterance.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrPlayUtteranceL( TSIModelBankID /*aModelBankID*/,
                                                      TSIModelID /*aModelID*/ )
    {
    // Not supported in SI 
    User::Leave(KErrNotSupported);	
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRecognizeL
// Initiates recognition towards the recognizer.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRecognizeL( CSDClientResultSet& /*aResultSet*/ )
    {
    // Not supported in SI 
    User::Leave(KErrNotSupported);	
    //	iController->RecognizeL(aResultSet);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRecordL
// Initiates recording of user utterance.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRecordL( TTimeIntervalMicroSeconds32 aRecordTime )
    {
    iController->RecordL(aRecordTime);
    }
    
// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrPreStartSamplingL
// Starts sampling
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrPreStartSamplingL()
    {
    iController->PreStartSamplingL();
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRemoveGrammarL
// Removes the specified grammar from the permanent storage.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRemoveGrammarL( TSIGrammarID aGrammarID )
    {
    iController->RemoveGrammarL(aGrammarID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRemoveLexiconL
// Removes the specified lexicon from the permanent storage.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRemoveLexiconL( TSILexiconID aLexiconID )
    {
    iController->RemoveLexiconL(aLexiconID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRemoveModelBankL
// Removes the specified model bank from the permanent storage.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRemoveModelBankL( TSIModelBankID aModelBankID )
    {
    iController->RemoveModelBankL(aModelBankID);
    }


// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRemoveModelL
// Removes the specified model from the specified model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRemoveModelL( TSIModelBankID /*aModelBankID*/,
                                                    TSIModelID /*aModelID*/ )
    {
    User::Leave(KErrNotSupported);	
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRemovePronunciationL
// Removes the specified pronunciation from the specified lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRemovePronunciationL( TSILexiconID aLexiconID,
                                                            TSIPronunciationID aPronunciationID )
    {
    iController->RemovePronunciationL(aLexiconID, aPronunciationID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRemoveRuleL
// Removes the specified rule from the specified grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRemoveRuleL( TSIGrammarID aGrammarID,
                                                   TSIRuleID aRuleID )
    {
    iController->RemoveRuleL(aGrammarID, aRuleID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRemoveRulesL
// Removes the specified rules from the specified grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRemoveRulesL( TSIGrammarID aGrammarID,
                                                    RArray<TSIRuleID>& aRuleIDs )
    {
    //   User::Leave( KErrNotSupported );
    iController->RemoveRulesL(aGrammarID, aRuleIDs);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrSetClientUid
// Sets the client's UID for data ownership identification.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrSetClientUid( TUid aClientUid )
    {
    iController->SetClientUid(aClientUid);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrStartRecSessionL
// Starts a new recognition session.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrStartRecSessionL( TNSSRecognitionMode aMode )
    { 
    if ( aMode == ENSSSiMode)
        {
        iController->StartRecSessionL();
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrTrainL
// Trains a new model into the specified model bank.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrTrainL( TSIModelBankID aModelBankID,
                                              TSIModelID& aModelID )
    {
    iController->TrainL(aModelBankID, aModelID);
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrUnloadRuleL
// Unloads a rule from the recognizer for this recognition session.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrUnloadRuleL( TSIGrammarID aGrammarID,
                                                   TSIRuleID aRuleID )
    {
    iController->UnloadRuleL(aGrammarID, aRuleID);
    }


///SI only functionalities
// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAdaptL
// Does speaker adapation to speaker independent models
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAdaptL( CSIClientResultSet& aResultSet,
                                              TInt aCorrect)
    {
    iController->AdaptL( aResultSet, aCorrect );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddPronunciationL
// AAdds a new pronunciation for the given model into the specified lexicon.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddPronunciationL( TSILexiconID aLexiconID, 
                                                         const TDesC& aTrainText,
                                                         TLanguage aLanguage, 
                                                         TSIPronunciationID& aPronunciationID )
    {
    iController->AddPronunciationL( aLexiconID, aTrainText,aLanguage,aPronunciationID );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddRuleVariantL
// Adds a new rule variant for the given pronunciation into the specified grammar.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddRuleVariantL( TSIGrammarID aGrammarID, 
                                                       TSILexiconID aLexiconID,
                                                       RArray<TSIPronunciationID>& aPronunciationIDs,
                                                       TSIRuleID aRuleID,
                                                       TSIRuleVariantID& aRuleVariantID ) 
    {
    iController->AddRuleVariantL( aGrammarID, aLexiconID, aPronunciationIDs,
                                  aRuleID, aRuleVariantID ) ;
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddVoiceTagsL
// Trains a new voice tag. The recognition phrase is the concatenation of words in the aTrainArray. 
// Pronunciations are created with all given languages, which are supported.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddVoiceTagsL( RPointerArray<MDesCArray>* aTrainArray,
                                                     RArray<TLanguage>& aLanguageArray, 
                                                     TSILexiconID aLexiconID,
                                                     TSIGrammarID aGrammarID,
                                                     RArray<TSIRuleID>& aRuleID ) 
    {
    iController->AddVoiceTagsL( *aTrainArray, aLanguageArray,   aLexiconID,
                                 aGrammarID, aRuleID );
    }


// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddVoiceTagL
// Trains a new voice tag.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddVoiceTagL( MDesCArray& aTrainArray, 
                                                    RArray<TLanguage>& aLanguageArray, 
                                                    TSILexiconID aLexiconID,
                                                    TSIGrammarID aGrammarID, 
                                                    TSIRuleID& aRuleID) 
    {
    iController->AddVoiceTagL( aTrainArray, aLanguageArray,   aLexiconID,
                               aGrammarID,  aRuleID );
    }

#ifdef __SINDE_TRAINING
// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddSindeVoiceTagL
// Trains a new SINDE voice tag.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddSindeVoiceTagL( MDesCArray& aTrainArray,
                                                         RArray<RLanguageArray>& aLanguageArray,
                                                         TSILexiconID aLexiconID,
                                                         TSIGrammarID aGrammarID,
                                                         TSIRuleID& aRuleID )
    {
    iController->AddSindeVoiceTagL( aTrainArray, aLanguageArray,
                                    aLexiconID, aGrammarID, aRuleID );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddSindeVoiceTagL
// Trains a new SINDE voice tag.
// -----------------------------------------------------------------------------
//    
void CSIControllerPluginInterface::CSIControllerPluginInterface::MSrAddSindeVoiceTagsL( RPointerArray<MDesCArray>* aTrainArray,
                                                                                        RArray<RLanguageArray>& aLanguageArray,
                                                                                        TSILexiconID aLexiconID,
                                                                                        TSIGrammarID aGrammarID,
                                                                                        RArray<TSIRuleID>& aRuleID )
    {
    iController->AddSindeVoiceTagsL( *aTrainArray, aLanguageArray,
                                     aLexiconID, aGrammarID, aRuleID );
    }

#else // __SINDE_TRAINING

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddSindeVoiceTagL
// Trains a new SINDE voice tag.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrAddSindeVoiceTagL( MDesCArray& /*aTrainArray*/,
                                                         RArray<RLanguageArray>& /*aLanguageArray*/,
                                                         TSILexiconID /*aLexiconID*/,
                                                         TSIGrammarID /*aGrammarID*/,
                                                         TSIRuleID& /*aRuleID*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrAddSindeVoiceTagL
// Trains a new SINDE voice tag.
// -----------------------------------------------------------------------------
//    
void CSIControllerPluginInterface::CSIControllerPluginInterface::MSrAddSindeVoiceTagsL( RPointerArray<MDesCArray>* /*aTrainArray*/,
                                                                                        RArray<RLanguageArray>& /*aLanguageArray*/,
                                                                                        TSILexiconID /*aLexiconID*/,
                                                                                        TSIGrammarID /*aGrammarID*/,
                                                                                        RArray<TSIRuleID>& /*aRuleID*/ )
    {
    User::Leave( KErrNotSupported );
    }

#endif // __SINDE_TRAINING
    


// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrCreateRuleL
// Creates a new empty rule.
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrCreateRuleL( TSIGrammarID aGrammarID, 
                                                   TSIRuleID& aRuleID )  
    {
    iController->CreateRuleL( aGrammarID, aRuleID );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrRecognizeL
// Initiates recognition; performed following loading of model bank, lexicon, and grammar
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrRecognizeL( CSIClientResultSet& aResultSet ) 
    {
    iController->RecognizeL( aResultSet );
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrEndRecordL
// Ends recording. Unlike Cancel(), this function may return a recognition
// result if adequate number of samples was already recorded. 
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrEndRecordL() 
    {
    iController->EndRecordL();
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::MSrUnloadGrammarL
// Unloads the specified grammar from the temporary memory, 
// previously loaded with MSrLoadGrammarL. The grammar in the permanent storage remains intact. 
// result if adequate number of samples was already recorded.		        
// -----------------------------------------------------------------------------
//
void CSIControllerPluginInterface::MSrUnloadGrammarL( TSIGrammarID aGrammarID ) 
    {
    iController->UnloadGrammarL( aGrammarID ) ;
    }

// -----------------------------------------------------------------------------
// CSIControllerPluginInterface::SendSrsEvent
// Sends an event to the client.
// -----------------------------------------------------------------------------
//
#ifndef __CONSOLETEST__

void CSIControllerPluginInterface::SendSrsEvent( TUid aEvent,
                                                 TInt aResult )
    {
    RUBY_DEBUG2( "CSIControllerPluginInterface::SendSrsEvent: %d, result [%d]", aEvent, aResult );
    TMMFEvent event( aEvent, aResult );
    DoSendEventToClient( event );
    }

#else

// Unit Test
void CSIControllerPluginInterface::SendSrsEvent(
                                                TUid aEvent,
                                                TInt aResult )
    {
    iObserver->SrsEvent(aEvent, aResult);
    }

#endif

// ========================== OTHER EXPORTED FUNCTIONS =========================


#ifdef __CONSOLETEST__
// Unit Test

// Two-phased constructor
CSIControllerPluginInterface* CSIControllerPluginInterface::NewL(
                                                                 MSrsObserver* aObserver )
    {
    CSIControllerPluginInterface* self = new( ELeave ) CSIControllerPluginInterface();
    
    CleanupStack::PushL( self );
    self->ConstructL(aObserver);
    CleanupStack::Pop();
    
    return self;
    }

// Returns reference to a database
RDbNamedDatabase& CSIControllerPluginInterface::GetDb()
    {
    return iController->GetDb();
    }


// Returns reference to model bank array
RPointerArray<CSIModelBank>& CSIControllerPluginInterface::ModelBankArray()
    {
    return iController->ModelBankArray();
    }

// Returns current plugin state
TInt CSIControllerPluginInterface::PluginState()
    {
    return iController->PluginState();
    }

#endif


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

#ifndef __CONSOLETEST__
/**
*
* ImplementationTable
*
*/
const TImplementationProxy ImplementationTable[] =
    {
        {{KSIUidController}, ( TProxyNewLPtr ) CSIControllerPluginInterface::NewL}		// defined in SIPluginImplementationUIDs.hrh
    };

/**
* ImplementationGroupProxy
* @param aTableCount
* @returns "TImplementationProxy*"
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    
    return ImplementationTable;
    }

#endif // __CONSOLETEST__

//  End of File
