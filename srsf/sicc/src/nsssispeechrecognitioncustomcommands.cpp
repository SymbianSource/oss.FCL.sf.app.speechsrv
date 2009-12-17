/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the implementation of the client interface for the
*               speaker independent custom commands.
*
*/


// INCLUDE FILES
#include "srsfbldvariant.hrh"
#include "nsssispeechrecognitioncustomcommands.h"
#include <nsssispeechrecognitiondatacommon.h>
#include <badesca.h>
#include "nsssispeechrecognitioncustomcommandcommon.h"
#include "nsssicustomcommanddata.h"
#include "rubydebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RSISpeechRecognitionCustomCommands
// C++ constructor.
// -----------------------------------------------------------------------------

EXPORT_C RSISpeechRecognitionCustomCommands::RSISpeechRecognitionCustomCommands(
	RMMFController& aController) :
	RMMFCustomCommandsBase(aController, 
	KUidInterfaceSpeakerIndependent)
	{

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::AddRule
// Sends a command to the controller to add a rule.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::AddRule(
	TSIGrammarID aGrammarID, 
	TSILexiconID aLexiconID, 
	TSIPronunciationID aPronunciationID, 
	TSIRuleID& aRuleID)
	{

	TSrsAddRule srsAddRule(aGrammarID, aLexiconID, aPronunciationID, &aRuleID);
	TSrsAddRulePckg pckg(srsAddRule);
	return iController.CustomCommandSync(iDestinationPckg, ESrAddRule,
											pckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::Cancel
// Sends a command to the controller to cancel the currect asynchronous 
// operation.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSISpeechRecognitionCustomCommands::Cancel()
	{

	iController.CustomCommandSync(iDestinationPckg, ESrCancel,
											KNullDesC8, KNullDesC8);

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::CommitChanges
// Sends a command to the controller to commit the current changes.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::CommitChanges()
	{

	return iController.CustomCommandSync(iDestinationPckg, ESrCommitChanges,
											KNullDesC8, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::CreateGrammar
// Sends a command to the controller to create a grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::CreateGrammar(
	TSIGrammarID& aGrammarID)
	{

	TSrsGrammarIDPtrPckg srsGrammarIDPtrPckg = &aGrammarID;
	return iController.CustomCommandSync(iDestinationPckg, ESrCreateGrammar,
											srsGrammarIDPtrPckg, KNullDesC8);

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::CreateLexicon
// Sends a command to the controller to create a lexicon.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::CreateLexicon(
	TSILexiconID& aLexiconID)
	{

	TSrsLexiconIDPtrPckg srsLexiconIDPtrPckg = &aLexiconID;
	return iController.CustomCommandSync(iDestinationPckg, ESrCreateLexicon,
											srsLexiconIDPtrPckg, KNullDesC8);

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::CreateModelBank
// Sends a command to the controller to create a model bank.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::CreateModelBank(
	TSIModelBankID& aModelBankID)
	{

	TSrsModelBankIDPtrPckg modelBankIDPtrPckg = &aModelBankID;
	return iController.CustomCommandSync(iDestinationPckg, ESrCreateModelBank,
											modelBankIDPtrPckg, KNullDesC8);
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::EndRecSession
// Sends a command to the controller to end the current recognition session and 
// free the allocated resources.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::EndRecSession()
	{

	TInt err = iController.CustomCommandSync(iDestinationPckg, ESrEndRecSession,
											KNullDesC8, KNullDesC8);
	return err;

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllClientGrammarIDs
// Calls the controller plugin to get all client grammar IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllClientGrammarIDs()
	{

	return iController.CustomCommandSync(iDestinationPckg, 
			ESrGetAllClientGrammarIDs, KNullDesC8, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllClientLexiconIDs
// Calls the controller plugin to get all client lexicon IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllClientLexiconIDs()
	{

	return iController.CustomCommandSync(iDestinationPckg, 
			ESrGetAllClientLexiconIDs, KNullDesC8, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllClientModelBankIDs
// Calls the controller plugin to get all client model bank IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllClientModelBankIDs()
	{

	return iController.CustomCommandSync(iDestinationPckg, ESrGetAllClientModelBankIDs,
							KNullDesC8, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllGrammarIDs
// Calls the controller plugin to get all grammar IDs for all clients.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllGrammarIDs()
	{

	return iController.CustomCommandSync(iDestinationPckg, ESrGetAllGrammarIDs,
							KNullDesC8, KNullDesC8); // Async
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllLexiconIDs
// Calls the controller plugin to get all lexicon IDs for all clients.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllLexiconIDs()
	{	

	return iController.CustomCommandSync(iDestinationPckg, ESrGetAllLexiconIDs,
							KNullDesC8, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllModelBankIDs
// Calls the controller plugin to get all model bank IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllModelBankIDs()
	{

	return iController.CustomCommandSync(iDestinationPckg, ESrGetAllModelBankIDs,
							KNullDesC8, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllModelIDs
// Sends a command to the controller to get all model IDs from a model bank.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllModelIDs(
	TSIModelBankID aModelBankID)
	{

	TSrsModelBankIDPckg modelBankIDPckg = aModelBankID;

	return iController.CustomCommandSync(iDestinationPckg, ESrGetAllModelIDs,
							modelBankIDPckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllPronunciationIDs
// Sends a command to the controller to get all pronunciation IDs from a lexicon.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllPronunciationIDs(
	TSILexiconID aLexiconID)
	{

	TSrsLexiconIDPckg lexiconIDPckg = aLexiconID;
	return iController.CustomCommandSync(iDestinationPckg, 
						ESrGetAllPronunciationIDs,
						lexiconIDPckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetAllRuleIDs
// Sends a command to the controller to get all rule IDs from a grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetAllRuleIDs(
	TSIGrammarID aGrammarID)
	{

	TSrsGrammarIDPckg grammarIDPckg = aGrammarID;

	return iController.CustomCommandSync(iDestinationPckg, ESrGetAllRuleIDs,
							grammarIDPckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetEngineProperties
// Sends a command to the controller to get the engine properties.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetEngineProperties(
	const RArray<TInt>& aPropertyId, 
	RArray<TInt>& aPropertyValue)
	{

	TRAPD(err, DoGetEnginePropertiesL(aPropertyId, aPropertyValue));
	return err;	

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetModelCount
// Sends a command to the controller to get the number of models in a model bank.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetModelCount(
	TSIModelBankID aModelBankID, TInt& aModelCount)
	{

	TSrsModelBankIDPckg modelBankPckg = aModelBankID;
	TSrsIntPtrPckg modelCountPtrPckg = &aModelCount;

	return iController.CustomCommandSync(iDestinationPckg, ESrGetModelCount,
											modelBankPckg, modelCountPtrPckg);

	}


// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetRuleValidity
// Sends a command to the controller to determine if a rule is valid.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::GetRuleValidity(
	TSIGrammarID aGrammarID, 
	TSIRuleID aRuleID, TBool& aValid)
	{

	TSrsRuleValidity srsRuleValidity(aGrammarID, aRuleID, &aValid);
	TSrsRuleValidityPckg srsRuleValidityPckg(srsRuleValidity);

	return iController.CustomCommandSync(iDestinationPckg, ESrGetRuleValidity,
											srsRuleValidityPckg, KNullDesC8);

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::LoadGrammar
// Sends a command to the controller to load the specified grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::LoadGrammar(
	TSIGrammarID aGrammarID)
	{

	TSrsGrammarIDPckg pckg = aGrammarID;
	return iController.CustomCommandSync(iDestinationPckg, ESrLoadGrammar,
											pckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::ActivateGrammar
// Sends a command to the controller to activate a grammar
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::ActivateGrammar(
	TSIGrammarID aGrammarID)
	{
	TSrsGrammarIDPckg pckg = aGrammarID;
	return iController.CustomCommandSync( iDestinationPckg, ESrActivateGrammar,
                                          pckg, KNullDesC8 ); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::DeactivateGrammar
// Sends a command to the controller to deactivate a gramamr
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::DeactivateGrammar(
	TSIGrammarID aGrammarID)
	{
	TSrsGrammarIDPckg pckg = aGrammarID;
	return iController.CustomCommandSync( iDestinationPckg, ESrDeactivateGrammar,
                                          pckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::LoadLexicon
// Sends a command to the controller to load the specified lexican.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::LoadLexicon(
	TSILexiconID aLexiconID)
	{

	TSrsLexiconIDPckg pckg = aLexiconID;
	return iController.CustomCommandSync(iDestinationPckg, ESrLoadLexicon,
											pckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::LoadModels
// Sends a command to the controller to load all models from a model bank.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::LoadModels(
	TSIModelBankID aModelBankID)
	{

	TSrsModelIDPckg pckg = aModelBankID;
	return iController.CustomCommandSync(iDestinationPckg, ESrLoadModels,
											pckg, KNullDesC8);  // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::Record
// Sends a command to the controller to start recording an utterance for either 
// training or recognition
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::Record(
	TTimeIntervalMicroSeconds32 aRecordTime)
	{

	TSrsTimeIntervalMicroSeconds32Pckg pckg = aRecordTime;
	return iController.CustomCommandSync(iDestinationPckg, ESrRecord,
											pckg, KNullDesC8); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RemoveGrammar
// Sends a command to the controller to remove a grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::RemoveGrammar(
	TSIGrammarID aGrammarID)
	{

	TSrsGrammarIDPckg pckg = aGrammarID;
	return iController.CustomCommandSync(iDestinationPckg, ESrRemoveGrammar,
											pckg, KNullDesC8);

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RemoveLexicon
// Sends a command to the controller to remove a lexicon.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::RemoveLexicon(
	TSILexiconID aLexiconID)
	{

	TSrsLexiconIDPckg pckg = aLexiconID;
	return iController.CustomCommandSync(iDestinationPckg, ESrRemoveLexicon,
											pckg, KNullDesC8);

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RemoveModelBank
// Sends a command to the controller to remove a model bank and all associated 
// models.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::RemoveModelBank(
	TSIModelBankID aModelBankID)
	{

	TSrsModelBankIDPckg modelBankIDPckg = aModelBankID;
	return iController.CustomCommandSync(iDestinationPckg, ESrRemoveModelBank,
											modelBankIDPckg, KNullDesC8);

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RemoveModel
// Sends a command to the controller to remove a model from a model bank.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::RemoveModel(
	TSIModelBankID aModelBankID, 
	TSIModelID aModelID)
	{

	TSrsModelBankIDPckg modelBankIDPckg = aModelBankID;
	TSrsModelIDPckg modelIDPckg = aModelID;

	return iController.CustomCommandSync(iDestinationPckg, ESrRemoveModel,
										modelBankIDPckg, modelIDPckg); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RemovePronunciation
// Sends a command to the controller to remove a pronunciation.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::RemovePronunciation(
	TSILexiconID aLexiconID, 
	TSIPronunciationID aPronunciationID)
	{

	TSrsLexiconIDPckg lexiconIDPckg = aLexiconID;
	TSrsPronunciationIDPckg pronunciationIDPckg = aPronunciationID;

	return iController.CustomCommandSync(iDestinationPckg, ESrRemovePronunciation,
										lexiconIDPckg, pronunciationIDPckg); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RemoveRule
// Sends a command to the controller to remove a rule.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::RemoveRule(
	TSIGrammarID aGrammarID, 
	TSIRuleID aRuleID)
	{

	TSrsGrammarIDPckg grammarIDPckg = aGrammarID;
	TSrsRuleIDPckg ruleIDPckg = aRuleID;

	return iController.CustomCommandSync(iDestinationPckg, ESrRemoveRule,
											grammarIDPckg, ruleIDPckg); // Async

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::RemoveRules
// Sends a command to the controller to remove a rule.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::RemoveRules(
	TSIGrammarID aGrammarID, 
	RArray<TSIRuleID>& aRuleIDs)
	{
    RUBY_DEBUG1( "RSISpeechRecognitionCustomCommands::RemoveRules GrammarID [%d]", aGrammarID );
	
	TSrsGrammarIDPckg grammarIDPckg = aGrammarID;

    CBufFlat* flatBuf = 0;
    
    /** @todo Implement externalize, that takes arrays of Uint32? */
    TRAPD( err, flatBuf
        = ExternalizeIntArrayL( reinterpret_cast<RArray<TInt>&> ( aRuleIDs ) ) );
	
    if ( err != KErrNone )
       	{
       	delete flatBuf;
       	return err;
       	}
	// SRS will signal, when the rule IDs are ready.
	TInt ret = iController.CustomCommandSync(iDestinationPckg, ESrRemoveRules,
                                             grammarIDPckg, flatBuf->Ptr(0) ); // Async
    delete flatBuf;

    return( ret );
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::SetClientUid
// Sends a command to the controller to set the client UID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::SetClientUid( TUid aClientUid )
{

	TSrsUidPckg uidPckg = aClientUid;
	return iController.CustomCommandSync(iDestinationPckg, 
						ESrSetClientUid, uidPckg, KNullDesC8);

}


// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::StartRecSession
// Sends a command to the controller to begin a recognition session.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::StartRecSession(
	TNSSRecognitionMode aMode)
	{

	TSrsRecognitionModePckg recognitionModePckg = aMode;
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
						ESrStartRecSession, recognitionModePckg, KNullDesC8);

	return err;

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::UnloadRule
// Sends a command to the controller to add a rule.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::UnloadRule(
	TSIGrammarID aGrammarID, 
	TSIRuleID aRuleID)
	{

	TSrsGrammarIDPckg grammarIDPckg = aGrammarID;
	TSrsRuleIDPckg ruleIDPtrPckg = aRuleID;

	return iController.CustomCommandSync(iDestinationPckg, ESrUnloadRule,
											grammarIDPckg, ruleIDPtrPckg);
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::LoadEngineParameters
// Sends a command to the controller to load the parameters into
// the engine.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::LoadEngineParameters(
	const RArray<TInt>& aParameterId, 
	const RArray<TInt>& aParameterValue)
	{

	TRAPD(error, DoLoadEngineParametersL(aParameterId, aParameterValue));
	return error;	

	}


// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetPronunciationIDArrayL
// Sends a command to get the pronunciation ID array.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSISpeechRecognitionCustomCommands::GetPronunciationIDArrayL(
	RArray<TSIPronunciationID>& aPronunciationIDs)
	{
	
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetPronunciationIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
	

	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TUint32));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetPronunciationIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aPronunciationIDs.Append(stream.ReadUint32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
		
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetRuleIDArrayL
// Sends a command to get the rule ID array.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSISpeechRecognitionCustomCommands::GetRuleIDArrayL(
	RArray<TSIRuleID>& aRuleIDs)
	{

	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetRuleIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TUint32));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetRuleIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aRuleIDs.Append(stream.ReadUint32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetModelIDArrayL
// Sends a command to get the model ID array.
// -----------------------------------------------------------------------------
//	
EXPORT_C void RSISpeechRecognitionCustomCommands::GetModelIDArrayL(
	RArray<TSIModelID>& aModelIDs)
	{

	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetModelIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TUint32));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetModelIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError( aModelIDs.Append( (TSIModelID)stream.ReadUint32L() ) );
		}

	CleanupStack::PopAndDestroy( 2 );//stream, buf
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetGrammarIDArrayL
// Sends a command to get the grammar ID array.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSISpeechRecognitionCustomCommands::GetGrammarIDArrayL(
	RArray<TSIGrammarID>& aGrammarIDs)
	{

	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetGrammarIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TUint32));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetGrammarIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for ( TInt i = 0; i < pckgSize(); i++ )
		{
		User::LeaveIfError( aGrammarIDs.Append( (TSIGrammarID)stream.ReadUint32L() ) );
		}

	CleanupStack::PopAndDestroy( 2 );//stream, buf
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetLexiconIDArrayL
// Sends a command to get the lexicon ID array.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSISpeechRecognitionCustomCommands::GetLexiconIDArrayL(
	RArray<TSILexiconID>& aLexiconIDs)
	{

	TPckgBuf<TInt> pckgSize;

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetLexiconIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));

    TInt size = pckgSize();
	HBufC8* buf = HBufC8::NewLC(size*sizeof(TUint32));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetLexiconIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));

	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for ( TInt i = 0; i < pckgSize(); i++ )
		{
		User::LeaveIfError( aLexiconIDs.Append( (TSILexiconID)stream.ReadUint32L() ) );
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetLexiconIDArrayL
// Sends a command to get the lexicon ID array.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSISpeechRecognitionCustomCommands::GetModelBankIDArrayL(
	RArray<TSIModelBankID>& aModelBankIDs)
	{

	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetModelBankIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TUint32));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetModelBankIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for ( TInt i = 0; i < pckgSize(); i++ )
		{
		User::LeaveIfError( aModelBankIDs.Append( (TSIModelBankID)stream.ReadUint32L() ) );
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::GetSIResultSetL
// Sends a command to get the speaker independent recognition result set.
// -----------------------------------------------------------------------------
//	
EXPORT_C void RSISpeechRecognitionCustomCommands::GetSIResultSetL(
	CSIClientResultSet& aResultSet)
	{
	
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetClientResultSetSizeSI, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));

	HBufC8* buf = HBufC8::NewLC( pckgSize() );
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetClientResultSet,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));

	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	aResultSet.InternalizeL(stream);
	CleanupStack::PopAndDestroy(2);//stream, buf

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::DoRecognizeL
// Sends a command to get the recognition result set.
// -----------------------------------------------------------------------------
//	
void RSISpeechRecognitionCustomCommands::DoRecognizeL(CSDClientResultSet& /*aResultSet*/)
	{
    RUBY_DEBUG0( "RSISpeechRecognitionCustomCommands::DoRecognizeL is NOT supported" );
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::DoRecognizeSIL
// Sends a command to get the recognition result set.
// -----------------------------------------------------------------------------
//	
void RSISpeechRecognitionCustomCommands::DoRecognizeSIL(CSIClientResultSet& aResultSet)
	{
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);

	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);

	aResultSet.ExternalizeL(stream);

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
												ESrRecognizeSI,
												dataCopyBuffer->Ptr(0),
												KNullDesC8));

	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::DoGetEnginePropertiesL
// Sends a command to get the engine properties.
// -----------------------------------------------------------------------------
//	
void RSISpeechRecognitionCustomCommands::DoGetEnginePropertiesL(
	const RArray<TInt>& aPropertyId, 
	RArray<TInt>& aPropertyValue)
	{
	
	CBufFlat* param1 = ExternalizeIntArrayL(aPropertyId);
	CleanupStack::PushL(param1);
	
	TInt aNumberResults = aPropertyId.Count(); // same number of values as properties
	
	HBufC8* buf = HBufC8::NewLC(aNumberResults *sizeof(TInt));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetEngineProperties, 
												param1->Ptr(0),
												KNullDesC8,
												ptr));

	
	InternalizeIntArrayL(ptr,aNumberResults,aPropertyValue);
	CleanupStack::PopAndDestroy(2);//buf,param1

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::DoLoadEngineParametersL
// Sends a command to load the engine parameters.
// -----------------------------------------------------------------------------
//	
void RSISpeechRecognitionCustomCommands::DoLoadEngineParametersL(
	const RArray<TInt>& aParameterId, 
	const RArray<TInt>& aParameterValue)
	{

	CBufFlat* param1 = ExternalizeIntArrayL(aParameterId);
	CleanupStack::PushL(param1);
	CBufFlat* param2 = ExternalizeIntArrayL(aParameterValue);
	CleanupStack::PushL(param2);
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrLoadEngineParameters, 
												param1->Ptr(0),
												param2->Ptr(0)));
												
	CleanupStack::PopAndDestroy(2); //param2, param1	

	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::ExternalizeIntArrayL
// Externalizes an interger array.
// -----------------------------------------------------------------------------
//	
CBufFlat* RSISpeechRecognitionCustomCommands::ExternalizeIntArrayL(
	const RArray<TInt>& aArray)
	{

	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	
	stream.WriteInt32L(aArray.Count());
	
	for (TInt i=0;i<aArray.Count();i++)
		stream.WriteUint32L(aArray[i]);

	CleanupStack::PopAndDestroy(1); //stream
	CleanupStack::Pop(); //dataCopyBuffer;
	return dataCopyBuffer;	
	
	}

// -----------------------------------------------------------------------------
// RSISpeechRecognitionCustomCommands::InternalizeIntArrayL
// Internalizes an interger array.
// -----------------------------------------------------------------------------
//
void RSISpeechRecognitionCustomCommands::InternalizeIntArrayL(
	TDes8& aDes, 
	TInt aNumberElements, 
	RArray<TInt>& aArray)
	{

	RDesReadStream stream(aDes);
	CleanupClosePushL(stream);

	for (TInt i=0; i<aNumberElements; i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy();//stream

	}

/************************** New SI functions start here ***********************/

// -----------------------------------------------------------------------------
// Adapts speaker independent models according using a correct recognition result.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::Adapt(const CSIClientResultSet& aResultSet, TInt aCorrect)
    {
    CBufFlat* dataBuf = 0;
    RBufWriteStream dataStream;
    TInt ret = KErrNone;
    TBool forever = ETrue;

    TPckgBuf<TInt> correctPckg( aCorrect );

    while(forever)
        {
// Poor man's exception handling: while and break
#define BREAK( error ) if ( error != KErrNone ) { break; }

        // Allocate buffer for storing CSIClientResultSet
        TRAP ( ret, dataBuf = CBufFlat::NewL( KExpandSize ) );
        BREAK( ret );

        // Open a stream to the buffer
        dataStream.Open( *dataBuf );

        // Pack the data
        TRAP ( ret, aResultSet.ExternalizeL( dataStream ) );
        BREAK( ret );

        // Send through MMF
        ret = iController.CustomCommandSync(
            iDestinationPckg,
            ESrAdapt,
            dataBuf->Ptr( 0 ),
            correctPckg );

        break;
        }

    dataStream.Close();
    delete dataBuf;

    return ret;
    }

// -----------------------------------------------------------------------------
// Adds a pronunciation to the given lexicon.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::AddPronunciation(
    TSILexiconID aLexiconID,
    const TDesC& aTrainText,
    TLanguage aLanguage,
    TSIPronunciationID& aPronunciationID)
    {
    TSrsAddPronunciationSI srsAddPronunciation( aLexiconID, aTrainText.Length(),
                                                aLanguage, aPronunciationID );
    TSrsAddPronunciationSIPckg pckg( srsAddPronunciation );

    TInt    trainTextSize( aTrainText.Size() );
    TUint8* trainTextPtr = (TUint8*)aTrainText.Ptr();
    TPtrC8  trainTextDes( trainTextPtr, trainTextSize );

    return iController.CustomCommandSync(iDestinationPckg, ESrAddPronunciationSI,
                                         pckg, trainTextDes );
    }

// -----------------------------------------------------------------------------
// Adds a rule variant to a rule in a grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::AddRuleVariant(
    TSIGrammarID aGrammarID,
    TSILexiconID aLexiconID,
    const RArray<TSIPronunciationID>& aPronunciationIDs,
    TSIRuleID aRuleID,
    TSIRuleVariantID& aRuleVariantID )
    {
    TSrsAddRuleVariant srsAddRuleVariant( aGrammarID,
                                          aLexiconID,
                                          aPronunciationIDs.Count(),
                                          aRuleID,
                                          &aRuleVariantID );
    TSrsAddRuleVariantPckg pckg( srsAddRuleVariant );

    TUint8* pronunIdPtr = (TUint8*)&(aPronunciationIDs[0]);
    TPtrC8  pronunIdDes( pronunIdPtr, 
                         sizeof( TSIPronunciationID ) * aPronunciationIDs.Count()
                         );

    return iController.CustomCommandSync(iDestinationPckg, ESrAddRuleVariant,
                                         pckg, pronunIdDes );
    }

// -----------------------------------------------------------------------------
// Trains a voice tag. Adds the text to the lexicon, and creates the required
// rules and rule variants.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::AddVoiceTag(
    const MDesCArray& aTrainArray,
    const RArray<TLanguage>& aLanguageArray,
    TSILexiconID aLexiconID,
    TSIGrammarID aGrammarID,
    TSIRuleID& aRuleID)
    {
    TSrsAddVoiceTag srsAddVoiceTagData( aLexiconID, aGrammarID, &aRuleID, aLanguageArray );
    TSrsAddVoiceTagPckg pckg( srsAddVoiceTagData );

    CBufFlat* flatBuf = 0;
    TRAPD( err, flatBuf
        = SICustomCommandData::ExternalizeDesCArrayL( aTrainArray ) );

    if ( err != KErrNone )
        return( err );

    // SRS will signal, when the rule IDs are ready.
	TInt ret = iController.CustomCommandSync( iDestinationPckg, ESrAddVoiceTag,
                                              pckg, flatBuf->Ptr( 0 ) ); // Async

    delete flatBuf;

    return( ret );
    }

// -----------------------------------------------------------------------------
// Trains voice tags. If the training fails for some of the names (but not all),
// it is signaled by setting the Rule ID to KInvalidRuleID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::AddVoiceTags(
    const RPointerArray<MDesCArray>& aTrainArray,
    const RArray<TLanguage>& aLanguageArray,
    TSILexiconID aLexiconID,
    TSIGrammarID aGrammarID,
    RArray<TSIRuleID>& /*aRuleID*/)
    {
    TSrsAddVoiceTag srsAddVoiceTagData( aLexiconID, aGrammarID, NULL, aLanguageArray );
    TSrsAddVoiceTagPckg pckg( srsAddVoiceTagData );

    CBufFlat* flatBuf = 0;
    TRAPD( err, flatBuf
        = SICustomCommandData::ExternalizeDesCArrayArrayL( aTrainArray ) );

    if ( err != KErrNone )
        {
        delete flatBuf;
        return err;
        }

    // SRS will signal, when the rule IDs are ready.
	TInt ret = iController.CustomCommandSync( iDestinationPckg, ESrAddVoiceTags,
                                              pckg, flatBuf->Ptr( 0 ) ); // Async
    delete flatBuf;

    return( ret );
    }

#ifdef __SINDE_TRAINING
// -----------------------------------------------------------------------------
// Trains a voice tag. Adds the text to the lexicon, and creates the required
// rules and rule variants.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::AddVoiceTag( const MDesCArray& aTrainArray,
                                                               const RArray<RLanguageArray>& aLanguageArray,
                                                               TSILexiconID aLexiconID,
                                                               TSIGrammarID aGrammarID,
                                                               TSIRuleID& aRuleID )
    {
    TSrsAddSindeVoiceTag srsAddVoiceTagData( aLexiconID, aGrammarID, &aRuleID, aLanguageArray );
    TSrsAddSindeVoiceTagPckg pckg( srsAddVoiceTagData );

    CBufFlat* flatBuf( NULL );
    TRAPD( err, flatBuf = SICustomCommandData::ExternalizeDesCArrayL( aTrainArray ) );

    if ( err != KErrNone )
        {
        return err;
        }

    // SRS plugin will signal, when the rule IDs are ready.
	TInt ret = iController.CustomCommandSync( iDestinationPckg, ESrAddSindeVoiceTag,
                                              pckg, flatBuf->Ptr( 0 ) ); // Async

    delete flatBuf;

    return ret;
    }

// -----------------------------------------------------------------------------
// Trains voice tags. If the training fails for some of the names (but not all),
// it is signaled by setting the Rule ID to KInvalidRuleID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::AddVoiceTags( const RPointerArray<MDesCArray>& aTrainArray,
                                                                const RArray<RLanguageArray>& aLanguageArray,
                                                                TSILexiconID aLexiconID,
                                                                TSIGrammarID aGrammarID,
                                                                RArray<TSIRuleID>& /*aRuleID*/)
    {
    TSrsAddSindeVoiceTag srsAddVoiceTagData( aLexiconID, aGrammarID, NULL, aLanguageArray );
    TSrsAddSindeVoiceTagPckg pckg( srsAddVoiceTagData );

    CBufFlat* flatBuf( NULL );
    TRAPD( err, flatBuf = SICustomCommandData::ExternalizeDesCArrayArrayL( aTrainArray ) );

    if ( err != KErrNone )
        {
        delete flatBuf;
        return err;
        }

    // SRS plugin will signal, when the rule IDs are ready.
	TInt ret = iController.CustomCommandSync( iDestinationPckg, ESrAddSindeVoiceTags,
                                              pckg, flatBuf->Ptr( 0 ) ); // Async
    delete flatBuf;

    return ret;
    }

// -----------------------------------------------------------------------------
// Pre-starts sampling before Record() call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::PreStartSampling()
    {
    return iController.CustomCommandSync( iDestinationPckg, ESrPreStartSampling,
                                          KNullDesC8, KNullDesC8 ); // Async      
    }
    
#endif // SINDE_TRAINING

// -----------------------------------------------------------------------------
// Creates a new rule.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::CreateRule(TSIGrammarID aGrammarID, TSIRuleID& aRuleID)
    {
    TSrsAddRule srsCreateRule( aGrammarID, KInvalidLexiconID, KInvalidPronunciationID, &aRuleID );
    TSrsAddRulePckg pckg( srsCreateRule );

	return iController.CustomCommandSync(iDestinationPckg, ESrCreateRule,
                                         pckg, KNullDesC8 ); // Async    
    }

// -----------------------------------------------------------------------------
// Starts recognition
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::Recognize( CSIClientResultSet& aResultSet )
    {
    TRAPD( err, DoRecognizeSIL( aResultSet ) );
    return err;
    }

// -----------------------------------------------------------------------------
// Stops recognition. Unlike Cancel(), this function gives
// the recognition result, if it can be guessed.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::EndRecord()
    {
    return iController.CustomCommandSync( iDestinationPckg, ESrEndRecord,
                                          KNullDesC8, KNullDesC8 ); // Async
    }

// -----------------------------------------------------------------------------
// Deactivates a grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSISpeechRecognitionCustomCommands::UnloadGrammar( TSIGrammarID aGrammarID )
    {
    TSrsGrammarIDPckg pckg( aGrammarID );

	return iController.CustomCommandSync(iDestinationPckg, ESrUnloadGrammar,
                                         pckg, KNullDesC8 ); // Async
    }


/************************** New SI functions stop here ************************/

// End of file
