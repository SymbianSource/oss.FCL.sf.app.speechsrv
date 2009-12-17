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
* Description:  This is the implementation of the server side for the
*               speaker independent controller.
*
*/


// INCLUDE FILES
#include "srsfbldvariant.hrh"
#include <e32std.h>
#include <nsssispeechrecognitioncustomcommandparser.h>
#include <nsssispeechrecognitioncustomcommands.h>
#include "nsssispeechrecognitioncustomcommandcommon.h"
#include "nsssicustomcommanddata.h"
#include "rubydebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::CSISpeechRecognitionCustomCommandParser
// C++ constructor.
// -----------------------------------------------------------------------------
//
CSISpeechRecognitionCustomCommandParser::CSISpeechRecognitionCustomCommandParser(
	MSISpeechRecognitionCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceSpeakerIndependent),
	iImplementor(aImplementor),
    iAdaptationSet( 0 )
	{
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::NewL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSISpeechRecognitionCustomCommandParser* 
			CSISpeechRecognitionCustomCommandParser::NewL(
	MSISpeechRecognitionCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CSISpeechRecognitionCustomCommandParser(aImplementor);
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::~CSISpeechRecognitionCustomCommandParser
// Destructor for CSISpeechRecognitionCustomCommandParser class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSISpeechRecognitionCustomCommandParser::
			~CSISpeechRecognitionCustomCommandParser()
	{
	
	iPronunciationIDs.Close();
	iRuleIDs.Close();
	iModelIDs.Close();
	iGrammarIDs.Close();
	iLexiconIDs.Close();
	iModelBankIDs.Close();
    iLanguageArray.Close();
    for ( TInt i = 0; i < iLanguageArrayArray.Count(); i++ )
        {
        iLanguageArrayArray[i].Close();
        }
    iLanguageArrayArray.Close();

	delete iResultCopyBuffer;	
	//delete iResultSet;
    delete iSIResultSet;
    delete iWordArray;
    delete iAdaptationSet;
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::HandleRequest
// Handles the client side request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::HandleRequest(
	TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == 
						KUidInterfaceSpeakerIndependent);
	TRAPD(error, DoHandleRequestL(aMessage));
	if(error)
		aMessage.Complete(error);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoHandleRequestL
// Decodes aMessage and calls the correct function. This function is trapped by
// HandleRequest() and the leave code, if any, sent to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoHandleRequestL(
	TMMFMessage& aMessage)
    {
    TPckgBuf<TInt> pckgSize;
    
    // call required function to do the requested operation
    switch( aMessage.Function() )
        {
        case ESrAddPronunciation:
            DoAddPronunciationL(aMessage);
            break;
            
        case ESrAddRule:
            DoAddRuleL(aMessage);
            break;
            
        case ESrCancel:
            DoCancel();
            break;
            
        case ESrCommitChanges:
            DoCommitChangesL();
            break;
            
        case ESrCreateGrammar:
            DoCreateGrammarL(aMessage);
            break;
            
        case ESrCreateLexicon:
            DoCreateLexiconL(aMessage);
            break;
            
        case ESrCreateModelBank:
            DoCreateModelBankL(aMessage);
            break;
            
        case ESrEndRecSession:
            DoEndRecSessionL();
            break;
            
        case ESrGetAllClientGrammarIDs:
            DoGetAllClientGrammarIDsL();
            break;
            
        case ESrGetAllClientLexiconIDs:
            DoGetAllClientLexiconIDsL();
            break;
            
        case ESrGetAllClientModelBankIDs:
            DoGetAllClientModelBankIDsL();
            break;
            
        case ESrGetAllGrammarIDs:
            DoGetAllGrammarIDsL();
            break;
            
        case ESrGetAllLexiconIDs:
            DoGetAllLexiconIDsL();
            break;
            
        case ESrGetAllModelBankIDs:
            DoGetAllModelBankIDsL();
            break;
            
        case ESrGetAllModelIDs:
            DoGetAllModelIDsL(aMessage);
            break;
            
        case ESrGetAllPronunciationIDs:
            DoGetAllPronunciationIDsL(aMessage);
            break;
            
        case ESrGetAllRuleIDs:
            DoGetAllRuleIDsL(aMessage);
            break;
            
        case ESrGetAvailableStorage:
            DoGetAvailableStorageL(aMessage);
            break;
            
        case ESrGetEngineProperties:
            DoGetEnginePropertiesL(aMessage);
            break;
            
        case ESrGetModelCount:
            DoGetModelCountL(aMessage);
            break;
            
        case ESrGetRuleValidity:
            DoGetRuleValidityL(aMessage);
            break;
            
        case ESrUtteranceDuration:
            DoGetUtteranceDurationL(aMessage);
            break;
            
        case ESrLoadGrammar:
            DoLoadGrammarL(aMessage);
            break;
            
        case ESrActivateGrammar:
            DoActivateGrammarL(aMessage);
            break;
            
        case ESrDeactivateGrammar:
            DoDeactivateGrammarL(aMessage);
            break;
            
        case ESrLoadLexicon:
            DoLoadLexiconL(aMessage);
            break;
            
        case ESrLoadModels:
            DoLoadModelsL(aMessage);
            break;
            
        case ESrPlayUtterance:
            DoPlayUtteranceL(aMessage);
            break;
            
        case ESrRecognize:
            DoRecognizeL(aMessage);
            break;
            
        case ESrRecord:
            DoRecordL(aMessage);
            break;
            
        case ESrRemoveGrammar:
            DoRemoveGrammarL(aMessage);
            break;
            
        case ESrRemoveLexicon:
            DoRemoveLexiconL(aMessage);
            break;
            
        case ESrRemoveModelBank:
            DoRemoveModelBankL(aMessage);
            break;
            
        case ESrRemoveModel:
            DoRemoveModelL(aMessage);
            break;
            
        case ESrRemovePronunciation:
            DoRemovePronunciationL(aMessage);
            break;
            
        case ESrRemoveRule:
            DoRemoveRuleL(aMessage);
            break;
            
        case ESrRemoveRules:
            RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoHandleRequestL. case ESrRemoveRules" );
            DoRemoveRulesL( aMessage );
            break;
            
        case ESrSetClientUid:
            DoSetClientUidL(aMessage);
            break;
            
        case ESrStartRecSession:
            DoStartRecSessionL(aMessage);
            break;
            
        case ESrTrain:
            DoTrainL(aMessage);
            break;
            
        case ESrUnloadRule:
            DoUnloadRuleL(aMessage);
            break;
            
        case ESrLoadEngineParameters:
            DoLoadEngineParametersL(aMessage);
            break;
            
        case ESrGetPronunciationIDArraySize:
            pckgSize() = iPronunciationIDs.Count();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetPronunciationIDArrayContents:
            DoCopyPronunciationIDArrayL(aMessage);
            break;
            
        case ESrGetRuleIDArraySize:
            pckgSize() = iRuleIDs.Count();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetRuleIDArrayContents:
            DoCopyRuleIDArrayL(aMessage);			
            break;
            
        case ESrGetModelIDArraySize:
            pckgSize() = iModelIDs.Count();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetModelIDArrayContents:
            DoCopyModelIDArrayL(aMessage);
            break;
            
        case ESrGetGrammarIDArraySize:
            pckgSize() = iGrammarIDs.Count();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetGrammarIDArrayContents:
            DoCopyGrammarIDArrayL(aMessage);
            break;
            
        case ESrGetLexiconIDArraySize:
            pckgSize() = iLexiconIDs.Count();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetLexiconIDArrayContents:
            DoCopyLexiconIDArrayL(aMessage);
            break;
            
        case ESrGetModelBankIDArraySize:
            pckgSize() = iModelBankIDs.Count();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetModelBankIDArrayContents:
            DoCopyModelBankIDArrayL(aMessage);
            break;
            
        case ESrGetClientResultSetSize:
            DoExternalizeResultSetL();
            pckgSize() = iResultCopyBuffer->Size();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetClientResultSetSizeSI:
            DoExternalizeSIResultSetL();
            pckgSize() = iResultCopyBuffer->Size();
            aMessage.WriteDataToClientL(pckgSize);
            break;
            
        case ESrGetClientResultSet:
            DoCopyResultSetL(aMessage);
            break;
            
            // SI main requests
        case ESrAdapt:
            DoAdaptL( aMessage );
            break;
            
        case ESrAddPronunciationSI:
            DoAddPronunciationSIL( aMessage );
            break;
            
        case ESrAddRuleVariant:
            DoAddRuleVariantL( aMessage );
            break;
            
        case ESrAddVoiceTag:
            DoAddVoiceTagL( aMessage );
            break;
            
        case ESrAddVoiceTags:
            DoAddVoiceTagsL( aMessage );
            break;
            
        case ESrCreateRule:
            DoCreateRuleL( aMessage );
            break;
            
        case ESrEndRecord:
            DoEndRecordL( aMessage );
            break;
            
        case ESrRecognizeSI:
            DoRecognizeSIL( aMessage );
            break;
            
        case ESrUnloadGrammar:
            DoUnloadGrammarL( aMessage );
            break;
            
        case ESrAddSindeVoiceTag:
            DoAddSindeVoiceTagL( aMessage );
            break;
            
        case ESrAddSindeVoiceTags:
            DoAddSindeVoiceTagsL( aMessage );
            break;
            
        case ESrPreStartSampling:
            DoPreStartSamplingL( aMessage );
            break;
            
        default:
            RUBY_DEBUG1( "CSISpeechRecognitionCustomCommandParser::DoHandleRequestL. Unknown function [%d]. Leaving", aMessage.Function() );
            User::Leave(KErrNotSupported);
            
        }	// end of switch
    
    
    aMessage.Complete(KErrNone);
    
    }


// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddPronunciationL
// Calls the controller plugin to add a pronunciation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddPronunciationL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoAddPronunciationL is NOT supported" );
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddRuleL
// Calls the controller plugin to add a rule.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoAddRuleL(
	TMMFMessage& aMessage)
	{

	// Async
	TSrsAddRulePckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TSrsAddRule srsAddRule = pckg();

	iImplementor.MSrAddRuleL( srsAddRule.iGrammarID, srsAddRule.iLexiconID,
							 srsAddRule.iPronunciationID, *srsAddRule.iRuleID );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCancel
// Calls the controller plugin to cancel the currect asynchronous operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoCancel()
	{

	iImplementor.MSrCancel();

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCommitChanges
// Saves the current trained model into a permanent storage.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoCommitChangesL()
	{

	iImplementor.MSrCommitChangesL();

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCreateGrammarL
// Calls the controller plugin to create a grammar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoCreateGrammarL(
	TMMFMessage& aMessage)
	{

	TSrsGrammarIDPtrPckg srsGrammarIDPtrPckg;
	aMessage.ReadData1FromClientL(srsGrammarIDPtrPckg);
	TSIGrammarID* grammarID = srsGrammarIDPtrPckg();
	iImplementor.MSrCreateGrammarL(*grammarID) ;

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCreateLexiconL
// Calls the controller plugin to create a lexicon.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoCreateLexiconL(
	TMMFMessage& aMessage)
	{

	TSrsLexiconIDPtrPckg srsLexiconIDPtrPckg;
	aMessage.ReadData1FromClientL(srsLexiconIDPtrPckg);
	TSILexiconID* lexiconID = srsLexiconIDPtrPckg();
	iImplementor.MSrCreateLexiconL(*lexiconID) ;

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCreateModelBankL
// Calls the controller plugin to create a model bank.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoCreateModelBankL(
	TMMFMessage& aMessage)
	{

	TSrsModelBankIDPtrPckg modelBankIDPtrPckg;
	aMessage.ReadData1FromClientL(modelBankIDPtrPckg);
	TSIModelBankID* modelBankID = modelBankIDPtrPckg();
	iImplementor.MSrCreateModelBankL(*modelBankID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoEndRecSessionL
// Calls the controller plugin to end the current recognition session and free
// the allocated resources.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoEndRecSessionL()
	{

	iImplementor.MSrEndRecSessionL();

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllClientGrammarIDsL
// Calls the controller plugin to get all client grammar IDs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllClientGrammarIDsL()
	{

	// Async
	iGrammarIDs.Reset();
	iImplementor.MSrGetAllClientGrammarIDsL(iGrammarIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllClientLexiconIDsL
// Calls the controller plugin to get all client lexicon IDs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllClientLexiconIDsL()
	{

	// Async
	iLexiconIDs.Reset();
	iImplementor.MSrGetAllClientLexiconIDsL(iLexiconIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllClientModelBankIDsL
// Calls the controller plugin to get all client model bank IDs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllClientModelBankIDsL()
	{

	// Async
	iModelBankIDs.Reset();
	iImplementor.MSrGetAllClientModelBankIDsL(iModelBankIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllGrammarIDsL
// Calls the controller plugin to get all grammar IDs for all clients.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllGrammarIDsL()
	{

	// Async
	iGrammarIDs.Reset();
	iImplementor.MSrGetAllGrammarIDsL(iGrammarIDs);
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllLexiconIDsL
// Calls the controller plugin to get all lexicon IDs for all clients.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllLexiconIDsL()
	{	

	// Async
	iLexiconIDs.Reset();
	iImplementor.MSrGetAllLexiconIDsL(iLexiconIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllModelBankIDsL
// Calls the controller plugin to get all model bank IDs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllModelBankIDsL()
	{

	// Async
	iModelBankIDs.Reset();
	iImplementor.MSrGetAllModelBankIDsL(iModelBankIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllModelIDsL
// Calls the controller plugin to get all model IDs from a model bank.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllModelIDsL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoGetAllModelIDsL is NOT supported" );
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllPronunciationIDsL
// Calls the controller plugin to get all pronunciation IDs from a lexicon.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllPronunciationIDsL(
	TMMFMessage& aMessage)
	{

	// Async
	TSrsLexiconIDPckg lexiconIDPckg;
	iPronunciationIDs.Reset();
	aMessage.ReadData1FromClientL(lexiconIDPckg);

	iImplementor.MSrGetAllPronunciationIDsL(lexiconIDPckg(), iPronunciationIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAllRuleIDsL
// Calls the controller plugin to get all rule IDs from a grammar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAllRuleIDsL(
	TMMFMessage& aMessage)
	{

	// Async
	TSrsGrammarIDPckg grammarIDPckg;
	iRuleIDs.Reset();
	aMessage.ReadData1FromClientL(grammarIDPckg);

	iImplementor.MSrGetAllRuleIDsL(grammarIDPckg(), iRuleIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetAvailableStorageL
// Calls the controller plugin to get the available storage for training new 
// models.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetAvailableStorageL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoGetAvailableStorageL is NOT supported" );
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetEnginePropertiesL
// Calls the controller plugin to get the engine properties.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetEnginePropertiesL(
	TMMFMessage& aMessage)
	{

	RArray<TInt> propertyIDs;
	RArray<TInt> propertyValues;
	CleanupClosePushL(propertyIDs);
	CleanupClosePushL(propertyValues);
	DoExtractIntArrayFromData1L(aMessage, propertyIDs);
	iImplementor.MSrGetEnginePropertiesL(propertyIDs,propertyValues);
	// the custom command is aware of the size of the result, so it can copy the data back directly
	DoCopyIntArrayL(aMessage, propertyValues);
	CleanupStack::PopAndDestroy(2); //propertyIDs, propertyValues

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetModelCountL
// Calls the controller plugin to get the number of models in a model bank.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetModelCountL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoGetModelCountL is NOT supported" );
    User::Leave( KErrNotSupported );
	}


// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetRuleValidityL
// Calls the controller plugin to determine if a rule is valid.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetRuleValidityL(
	TMMFMessage& aMessage)
	{

	TSrsRuleValidityPckg srsRuleValidityPckg;
	aMessage.ReadData1FromClientL(srsRuleValidityPckg);
	TSrsRuleValidity srsRuleValidity = srsRuleValidityPckg();
	iImplementor.MSrGetRuleValidityL( srsRuleValidity.iGrammarID, 
				srsRuleValidity.iRuleID, *srsRuleValidity.iRuleValid );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoGetUtteranceDurationL
// Calls the controller plugin to get the utterance duration.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoGetUtteranceDurationL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoGetUtteranceDurationL is NOT supported" );
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoLoadGrammarL
// Calls the controller plugin to load the specified grammar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoLoadGrammarL(
	TMMFMessage& aMessage)
	{

	TSrsGrammarIDPckg grammarIDPckg;
	aMessage.ReadData1FromClientL(grammarIDPckg);
	TSIGrammarID grammarID = grammarIDPckg();
	iImplementor.MSrLoadGrammarL( grammarID );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoActivateGrammarL
// Calls the controller plugin to activate the specified grammar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoActivateGrammarL(
	TMMFMessage& aMessage)
	{

	TSrsGrammarIDPckg grammarIDPckg;
	aMessage.ReadData1FromClientL(grammarIDPckg);
	TSIGrammarID grammarID = grammarIDPckg();
	iImplementor.MSrActivateGrammarL( grammarID );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoDeactivateGrammarL
// Calls the controller plugin to deactivate the specified grammar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoDeactivateGrammarL(
	TMMFMessage& aMessage)
	{

	TSrsGrammarIDPckg grammarIDPckg;
	aMessage.ReadData1FromClientL(grammarIDPckg);
	TSIGrammarID grammarID = grammarIDPckg();
	iImplementor.MSrDeactivateGrammarL( grammarID );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoLoadLexiconL
// Calls the controller plugin to load the specified lexican.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoLoadLexiconL(
	TMMFMessage& aMessage)
	{

	TSrsLexiconIDPckg lexiconIDPckg;
	aMessage.ReadData1FromClientL(lexiconIDPckg);
	TSILexiconID lexiconID = lexiconIDPckg();
	iImplementor.MSrLoadLexiconL(lexiconID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoLoadModelsL
// Calls the controller plugin to load all models from a model bank.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoLoadModelsL(
	TMMFMessage& aMessage)
	{

	TSrsModelBankIDPckg modelBankIDPckg;
	aMessage.ReadData1FromClientL(modelBankIDPckg);
	TSIModelBankID modelBankID = modelBankIDPckg();
	iImplementor.MSrLoadModelsL(modelBankID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoPlayUtteranceL
// Calls the controller plugin to play a previously recorded utterance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoPlayUtteranceL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoPlayUtteranceL is NOT supported" );
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRecognizeL
// Calls the controller plugin to recognize an utterance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRecognizeL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoRecognizeL is NOT supported" );
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRecordL
// Calls the controller plugin to start recording an utterance for either 
// training or recognition
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRecordL(
	TMMFMessage& aMessage)
	{

	TSrsTimeIntervalMicroSeconds32Pckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TTimeIntervalMicroSeconds32 recordDuration = pckg();
	iImplementor.MSrRecordL(recordDuration);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoPreStartSamplingL
// Calls the controller plugin.
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoPreStartSamplingL(
    TMMFMessage& /*aMessage*/ )
    {
    iImplementor.MSrPreStartSamplingL();
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRemoveGrammarL
// Calls the controller plugin to remove a grammar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRemoveGrammarL(
	TMMFMessage& aMessage)
	{

	TSrsGrammarIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TSIGrammarID grammarID = pckg();
	iImplementor.MSrRemoveGrammarL(grammarID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRemoveLexiconL
// Calls the controller plugin to remove a lexicon.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRemoveLexiconL(
	TMMFMessage& aMessage)
	{

	TSrsLexiconIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TSILexiconID lexiconID = pckg();
	iImplementor.MSrRemoveLexiconL(lexiconID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRemoveModelBankL
// Calls the controller plugin to remove a model bank and all associated models.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRemoveModelBankL(
	TMMFMessage& aMessage)
	{

	TSrsModelBankIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TSIModelBankID modelBankID = pckg();
	iImplementor.MSrRemoveModelBankL(modelBankID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRemoveModelL
// Calls the controller plugin to remove a model from a model bank.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRemoveModelL(
	TMMFMessage& aMessage)
	{

	TSrsModelBankIDPckg modelBankIDPckg;
	aMessage.ReadData1FromClientL(modelBankIDPckg);
	TSIModelBankID modelBankID = modelBankIDPckg();
	TSrsModelIDPckg modelIDPckg;
	aMessage.ReadData2FromClientL(modelIDPckg);
	TSIModelBankID modelID = modelIDPckg();
	iImplementor.MSrRemoveModelL(modelBankID, modelID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRemovePronunciationL
// Calls the controller plugin to remove a pronunciation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRemovePronunciationL(
	TMMFMessage& aMessage)
	{

	TSrsLexiconIDPckg lexiconIDPckg;
	aMessage.ReadData1FromClientL(lexiconIDPckg);
	TSILexiconID lexiconID = lexiconIDPckg();
	TSrsPronunciationIDPckg pronunciationIDPckg;
	aMessage.ReadData2FromClientL(pronunciationIDPckg);
	TSIPronunciationID pronunciationID = pronunciationIDPckg();
	iImplementor.MSrRemovePronunciationL( lexiconID, pronunciationID );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRemoveRuleL
// Calls the controller plugin to remove a rule.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoRemoveRuleL(
	TMMFMessage& aMessage)
	{

	TSrsGrammarIDPckg grammarIDPckg;
	aMessage.ReadData1FromClientL(grammarIDPckg);
	TSIGrammarID grammarID = grammarIDPckg();
	TSrsRuleIDPckg ruleIDPckg;
	aMessage.ReadData2FromClientL(ruleIDPckg);
	TSIRuleID ruleID = ruleIDPckg();
	iImplementor.MSrRemoveRuleL( grammarID, ruleID );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoSetClientUid
// Sets the client's UID for data ownership identification.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoSetClientUidL(TMMFMessage& aMessage)
{
    RUBY_DEBUG_BLOCK( "CSISpeechRecognitionCustomCommandParser::DoSetClientUidL" );

	TSrsUidPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TUid uid = pckg();
	iImplementor.MSrSetClientUid(uid);

}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoStartRecSessionL
// Calls the controller plugin to begin a recognition session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoStartRecSessionL(
	TMMFMessage& aMessage)
	{

	TSrsRecognitionModePckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TNSSRecognitionMode mode = pckg();
	iImplementor.MSrStartRecSessionL( mode );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoTrainL
// Calls the controller plugin to begin a recognition session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoTrainL(
	TMMFMessage& /*aMessage*/)
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoTrainL is NOT supported" );
    User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoUnloadRuleL
// Calls the controller plugin to add a rule.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionCustomCommandParser::DoUnloadRuleL(
	TMMFMessage& aMessage)
	{

	TSrsGrammarIDPckg grammarIDPckg;
	aMessage.ReadData1FromClientL(grammarIDPckg);
	TSIGrammarID grammarID = grammarIDPckg();
	TSrsRuleIDPckg ruleIDPtrPckg;
	aMessage.ReadData2FromClientL(ruleIDPtrPckg);
	TSIRuleID ruleID = ruleIDPtrPckg();

	iImplementor.MSrUnloadRuleL( grammarID, ruleID );

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoLoadEngineParametersL
// Calls the controller plugin to upload the parameters into the engine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoLoadEngineParametersL(
	TMMFMessage& aMessage)
	{

	RArray<TInt> parameters;
	RArray<TInt> values;
	CleanupClosePushL(parameters);
	CleanupClosePushL(values);
	DoExtractIntArrayFromData1L(aMessage, parameters);
	DoExtractIntArrayFromData2L(aMessage, values);
	iImplementor.MSrLoadEngineParametersL(parameters,values);
	CleanupStack::PopAndDestroy(2); // parameters, values
	
	}

// ==================== SI Extensions ==========================================

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAdaptL
// Adapts the acoustic models to a user's voice.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAdaptL(
    TMMFMessage& aMessage)
    {
    HBufC8* resultSetBuf = SICustomCommandData::ReadMessageDataLC( aMessage, 1 );

    TPckgBuf<TInt> correctPckg;
    aMessage.ReadData2FromClientL( correctPckg );

    RDesReadStream resultSetStream;
    CleanupClosePushL( resultSetStream );
    resultSetStream.Open( *resultSetBuf );

    if ( iAdaptationSet )
        {
        delete iAdaptationSet;
        iAdaptationSet = 0;
        }

    iAdaptationSet = CSIClientResultSet::NewL();
    iAdaptationSet->InternalizeL( resultSetStream );

    iImplementor.MSrAdaptL( *iAdaptationSet, correctPckg() );

    CleanupStack::PopAndDestroy( &resultSetStream );
    CleanupStack::PopAndDestroy( resultSetBuf );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddPronunciationSIL
// Adds a speaker independent pronunciation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddPronunciationSIL(
    TMMFMessage& aMessage)
    {
    TSrsAddPronunciationSIPckg pckg;
    TSrsAddPronunciationSI* addPronunData;

    aMessage.ReadData1FromClientL( pckg );
    addPronunData = &pckg();

    // Read the phrase to be trained.
    HBufC8* textData = SICustomCommandData::ReadMessageDataLC( aMessage, 2 );

    // The phrase is in 8-bit descriptor, altough its structure is 16-bit Unicode.
    // We make a 16-bit TPtr, which points to the same memory area.
    TUint16* trainTextAddress = (TUint16*)textData->Ptr();
    TInt     trainTextLength  = addPronunData->iTextLength;

    // Check against buffer overrun before making the descriptor.
    TInt trainTextSize = 2 * trainTextLength;
    if ( trainTextSize > textData->Size() )
        {
        User::Leave( KErrCorrupt );
        }

    // Make the descriptor.
    TPtrC16 trainText( trainTextAddress, trainTextLength );

    iImplementor.MSrAddPronunciationL(
        addPronunData->iLexiconID,
        trainText,
        addPronunData->iLanguage,
        *addPronunData->iPronunciationIDPtr );

    CleanupStack::PopAndDestroy( textData );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddRuleVariantL
// Adds a rule variant
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CSISpeechRecognitionCustomCommandParser::DoAddRuleVariantL(
    TMMFMessage& aMessage)
    {
    TSrsAddRuleVariantPckg pckg;
    TSrsAddRuleVariant* data;

    aMessage.ReadData1FromClientL( pckg );
    data = &pckg();

    // Read the second parameter slot of aMessage.
    HBufC8* pronunChunk = SICustomCommandData::ReadMessageDataLC( aMessage, 2 );
    // Stack: pronunChunk

    RArray<TSIPronunciationID> pronunArray;
    CleanupClosePushL( pronunArray ); // Stack: pronunChunk, pronunArray

    SICustomCommandData::InternalizePronunArrayL(
        *pronunChunk,
        data->iPronunIDCount,
        pronunArray );

    iImplementor.MSrAddRuleVariantL(
        data->iGrammarID,
        data->iLexiconID,
        pronunArray,
        data->iRuleID,
        *data->iRuleVariantID
        );

    CleanupStack::PopAndDestroy( &pronunArray );
    CleanupStack::PopAndDestroy( pronunChunk );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddVoiceTag
// Adds a voice tag
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddVoiceTagL(
    TMMFMessage& aMessage)
    {
    if ( iWordArray != 0 )
        {
        delete iWordArray;
        iWordArray = 0;
        }

    TSrsAddVoiceTagPckg pckg;
    TSrsAddVoiceTag *params = &pckg();

    aMessage.ReadData1FromClient( pckg );

    HBufC8* phraseBuf = SICustomCommandData::ReadMessageDataLC( aMessage, 2 );
     // Stack: phraseBuf

    // !!!! Is it able to call ~MDesCArray, as MDesCArray is not derived from CBase?
    iWordArray = SICustomCommandData::InternalizeDesCArrayL( *phraseBuf );

    iLanguageArray.Reset();
    params->ListLanguagesL( iLanguageArray );

    iImplementor.MSrAddVoiceTagL(
        *iWordArray, 
        iLanguageArray, 
        params->iLexiconID, 
        params->iGrammarID, 
        *params->iRuleID );

    CleanupStack::PopAndDestroy( phraseBuf );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddVoiceTags
// Adds several voice tags
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddVoiceTagsL(
    TMMFMessage& aMessage)
    {
    RPointerArray<MDesCArray>* phraseArray = 0;
    // The 2 parameter slots contain:
    //  1. Headers: Various identifiers
    //  2. Phrases: Recognition phrases in an array.

    // Header storage
    TSrsAddVoiceTagPckg pckg;
    TSrsAddVoiceTag *params = &pckg();

    // Phrase storage

    aMessage.ReadData1FromClient( pckg );
    HBufC8* phraseBuf = SICustomCommandData::ReadMessageDataLC( aMessage, 2 );
    // Stack: phraseBuf

    phraseArray = SICustomCommandData::InternalizeDesCArrayArrayL( *phraseBuf );
    CleanupStack::PushL( phraseArray );

    iLanguageArray.Reset();
    params->ListLanguagesL( iLanguageArray );

    CleanupStack::Pop( phraseArray );

    iImplementor.MSrAddVoiceTagsL(
        phraseArray,
        iLanguageArray,
        params->iLexiconID,
        params->iGrammarID,
        iRuleIDs
        );

    CleanupStack::PopAndDestroy( phraseBuf );
    }

#ifdef __SINDE_TRAINING
// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagL
// 
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagL( TMMFMessage& aMessage )
    {
    if ( iWordArray != NULL )
        {
        delete iWordArray;
        iWordArray = NULL;
        }

    TSrsAddSindeVoiceTagPckg pckg;
    TSrsAddSindeVoiceTag *params = &pckg();

    aMessage.ReadData1FromClient( pckg );

    HBufC8* phraseBuf = SICustomCommandData::ReadMessageDataLC( aMessage, 2 );
     // Stack: phraseBuf

    iWordArray = SICustomCommandData::InternalizeDesCArrayL( *phraseBuf );

    for ( TInt i = 0; i < iLanguageArrayArray.Count(); i++ )
        {
        iLanguageArrayArray[i].Close();
        }
    iLanguageArrayArray.Reset();
    params->ListLanguagesL( iLanguageArrayArray );

    iImplementor.MSrAddSindeVoiceTagL( *iWordArray, 
                                       iLanguageArrayArray, 
                                       params->iLexiconID, 
                                       params->iGrammarID, 
                                       *params->iRuleID );

    CleanupStack::PopAndDestroy( phraseBuf );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagsL
//
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagsL( TMMFMessage& aMessage )
    {
    RPointerArray<MDesCArray>* phraseArray = 0;
    // The 2 parameter slots contain:
    //  1. Headers: Various identifiers
    //  2. Phrases: Recognition phrases in an array.

    // Header storage
    TSrsAddSindeVoiceTagPckg pckg;
    TSrsAddSindeVoiceTag *params = &pckg();

    // Phrase storage

    aMessage.ReadData1FromClient( pckg );
    HBufC8* phraseBuf = SICustomCommandData::ReadMessageDataLC( aMessage, 2 );
    // Stack: phraseBuf

    phraseArray = SICustomCommandData::InternalizeDesCArrayArrayL( *phraseBuf );
    CleanupStack::PushL( phraseArray );

    for ( TInt i = 0; i < iLanguageArrayArray.Count(); i++ )
        {
        iLanguageArrayArray[i].Close();
        }
    iLanguageArrayArray.Reset();
    params->ListLanguagesL( iLanguageArrayArray );


    iImplementor.MSrAddSindeVoiceTagsL( phraseArray, iLanguageArrayArray,
                                        params->iLexiconID, params->iGrammarID,
                                        iRuleIDs );

    CleanupStack::Pop( phraseArray );
    CleanupStack::PopAndDestroy( phraseBuf );
    }

#else // __SINDE_TRAINING
// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagL
// 
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagL( TMMFMessage& /*aMessage*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagsL
//
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoAddSindeVoiceTagsL( TMMFMessage& /*aMessage*/ )
    {
    User::Leave( KErrNotSupported );
    }
#endif // __SINDE_TRAINING

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCreateRule
// Creates an empty rule
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCreateRuleL(
    TMMFMessage& aMessage)
    {
    TSrsAddRulePckg pckg;
    TSrsAddRule* params = &(pckg());

	aMessage.ReadData1FromClientL(pckg);

    iImplementor.MSrCreateRuleL( params->iGrammarID, *params->iRuleID );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoEndRecord
// Stops recognition
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoEndRecordL(
    TMMFMessage& /*aMessage*/)
    {
    iImplementor.MSrEndRecordL();
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRecognizeSI
// Starts a speaker independent recognition
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoRecognizeSIL(
    TMMFMessage& aMessage)
    {
	// remove any previous result set, if any
	if (iSIResultSet != NULL)
	{
		delete iSIResultSet;
		iSIResultSet = NULL;
	}

    // Read data chunk of CSIClientResultSet
    HBufC8* dataBuffer = SICustomCommandData::ReadMessageDataLC( aMessage, 1 );

    // Set up stream
	RDesReadStream stream( *dataBuffer );
	CleanupClosePushL(stream);

    // Convert chunk to proper type
	iSIResultSet = CSIClientResultSet::NewL();
	iSIResultSet->InternalizeL(stream);

    // Recognize
	iImplementor.MSrRecognizeL(*iSIResultSet);

	CleanupStack::PopAndDestroy(&stream);//stream
    CleanupStack::PopAndDestroy(dataBuffer);//databuffer
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoUnloadGrammar
// Deactivates a grammar from the current recognition vocabulary
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoUnloadGrammarL(
    TMMFMessage& aMessage)
    {
	TSrsGrammarIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	TSIGrammarID grammarID = pckg();

	iImplementor.MSrUnloadGrammarL(grammarID);
    }

// ==================== SI Extensions end ======================================


// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyPronunciationIDArrayL
// Copy the pronunciation IDs to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCopyPronunciationIDArrayL(
	TMMFMessage& aMessage)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iPronunciationIDs.Count();i++)
	{
		stream.WriteUint32L(iPronunciationIDs[i]);
	}

	aMessage.WriteDataToClientL(dataCopyBuffer->Ptr(0));
	
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyModelIDArrayL
// Copy the model IDs to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCopyModelIDArrayL(
	TMMFMessage& aMessage)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iModelIDs.Count();i++)
	{
		stream.WriteUint32L(iModelIDs[i]);
	}

	aMessage.WriteDataToClientL(dataCopyBuffer->Ptr(0));
	
//	iModelIDs.Close();
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyRuleIDArrayL
// Copy the rule IDs to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCopyRuleIDArrayL(
	TMMFMessage& aMessage)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iRuleIDs.Count();i++)
	{
		stream.WriteUint32L(iRuleIDs[i]);
	}

	aMessage.WriteDataToClientL(dataCopyBuffer->Ptr(0));
	
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer

	}
	
// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoRemoveRules
// Remove rules by their IDS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoRemoveRulesL( TMMFMessage& aMessage )
	{
    RUBY_DEBUG_BLOCK( "CSISpeechRecognitionCustomCommandParser::DoRemoveRulesL" );

	TSrsGrammarIDPckg grammarIDPckg;
	aMessage.ReadData1FromClientL(grammarIDPckg);
	TSIGrammarID grammarID = grammarIDPckg();
	RArray<TInt> ruleIDs;
	CleanupClosePushL( ruleIDs );
//	TMMFMessage& aMessage, 
//	RArray<TInt>& aArray)
	DoExtractIntArrayFromData2L(aMessage, ruleIDs);

	
	iRuleIDs.Reset();
	for( TInt i = 0; i < ruleIDs.Count(); i++) 
		{
		iRuleIDs.Append(ruleIDs[i]);
		}

	//iImplementor.MSrRemoveRulesL( grammarID, reinterpret_cast<RArray<TUint32>&> ( forwardedRuleIDs ) );
    iImplementor.MSrRemoveRulesL( grammarID, iRuleIDs );

	CleanupStack::PopAndDestroy();  // ruleIDs
	}


// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyGrammarIDArrayL
// Copy the grammar IDs to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCopyGrammarIDArrayL(
	TMMFMessage& aMessage)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iGrammarIDs.Count();i++)
	{
		stream.WriteUint32L(iGrammarIDs[i]);
	}

	aMessage.WriteDataToClientL(dataCopyBuffer->Ptr(0));
	
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyLexiconIDArrayL
// Copy the lexicon IDs to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCopyLexiconIDArrayL(
	TMMFMessage& aMessage)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iLexiconIDs.Count();i++)
	{
		stream.WriteUint32L(iLexiconIDs[i]);
	}

	aMessage.WriteDataToClientL(dataCopyBuffer->Ptr(0));
	
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyModelBankIDArrayL
// Copy the model bank IDs to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCopyModelBankIDArrayL(
	TMMFMessage& aMessage)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iModelBankIDs.Count();i++)
	{
		stream.WriteUint32L(iModelBankIDs[i]);
	}

	aMessage.WriteDataToClientL(dataCopyBuffer->Ptr(0));
	
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoExternalizeResultSetL
// Externalize the client result set. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoExternalizeResultSetL()
	{
    RUBY_DEBUG0( "CSISpeechRecognitionCustomCommandParser::DoExternalizeResultSetL is NOT supported" );
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyResultSetL
// Copy the recognition result set to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoCopyResultSetL(
	TMMFMessage& aMessage)
	{

	// make sure we have a copy buffer available.
	if ( iResultCopyBuffer == NULL )
	{
		User::Leave(KErrNotReady);
	}

	aMessage.WriteDataToClientL(iResultCopyBuffer->Ptr(0));
	delete iResultCopyBuffer;
	iResultCopyBuffer = NULL;

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoExternalizeSIResultSetL
// Externalize the client result set. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoExternalizeSIResultSetL()
	{

	// make sure we have a result set available.
	if ( iSIResultSet == NULL )
	{
		User::Leave(KErrNotReady);
	}
	
	// allocate the result copy buffer if needed.
	if (iResultCopyBuffer == NULL ) 
	{
		iResultCopyBuffer = CBufFlat::NewL(KExpandSize);
	}
	else
	{
		iResultCopyBuffer->Reset();	// use existing buffer, clear previous results
	}

	RBufWriteStream stream;
	stream.Open(*iResultCopyBuffer);
	CleanupClosePushL(stream);
	iSIResultSet->ExternalizeL(stream);
	delete iSIResultSet; // the controller side version is no longer needed
	iSIResultSet = NULL;
	CleanupStack::PopAndDestroy(1); //stream

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoExtractIntArrayFromData1L
// Extract an integer array from client data 1.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoExtractIntArrayFromData1L(
	TMMFMessage& aMessage, 
	RArray<TInt>& aArray)
	{
	TInt size = aMessage.SizeOfData1FromClient();
	HBufC8* buf = HBufC8::NewL(size);
	CleanupStack::PushL(buf);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData1FromClientL(ptr);
	DoExtractIntArrayL(ptr,aArray);
	CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoExtractIntArrayFromData2L
// Extract an integer array from client data 2.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSISpeechRecognitionCustomCommandParser::DoExtractIntArrayFromData2L(
	TMMFMessage& aMessage, 
	RArray<TInt>& aArray)
	{
	
	TInt size = aMessage.SizeOfData2FromClient();
	HBufC8* buf = HBufC8::NewL(size);
	CleanupStack::PushL(buf);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData2FromClientL(ptr);
	DoExtractIntArrayL(ptr,aArray);
	CleanupStack::PopAndDestroy(buf);
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoExtractIntArrayL
// Extract an integer array from a descriptor into an RArray.
// (other items were commented in a header).
// -----------------------------------------------------------------------------	
void CSISpeechRecognitionCustomCommandParser::DoExtractIntArrayL(
	TDes8& aDescriptor, 
	RArray<TInt>& aArray)
	{

	RDesReadStream stream(aDescriptor);
	CleanupClosePushL(stream);
	
	TInt numberElements;
	numberElements = stream.ReadInt32L();

    TInt i( 0 );
	for ( i = 0; i<numberElements; i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadInt32L()));
		}
	CleanupStack::PopAndDestroy(); //stream
	
#ifdef _DEBUG
        RUBY_DEBUG1( "CSISpeechRecognitionCustomCommandParser::DoExtractIntArrayL Read array of size [%d]", aArray.Count() );

		for( i = 0; i < aArray.Count(); i++) 
			{
			RUBY_DEBUG2( "::DoExtractIntArrayL aArray[%d] = [%d]", i, aArray[i] );
			}
#endif

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionCustomCommandParser::DoCopyIntArrayL
// Copy data from an RArray to the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------	
void CSISpeechRecognitionCustomCommandParser::DoCopyIntArrayL(
	TMMFMessage& aMessage, 
	const RArray<TInt>& aArray)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);

	for (TInt i=0;i<aArray.Count();i++)
	{
		stream.WriteUint32L(aArray[i]);
	}

	aMessage.WriteDataToClientL(dataCopyBuffer->Ptr(0));
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer

	}


// End of file
