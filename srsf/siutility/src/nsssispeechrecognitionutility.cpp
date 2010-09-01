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
* Description:  This is the implementation of the speech recognition utility.
*
*/


// INCLUDE FILES
#include "rubydebug.h"
#include "srsfbldvariant.hrh"
#include "nsssispeechrecognitionutility.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CSISpeechRecognitionUtility
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CSISpeechRecognitionUtility::CSISpeechRecognitionUtility(
	MSISpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver ) : 
	iSrCustomCommands(iMMFController)
	{	

	iSpeechRecognitionUtilityObserver = &aSpeechRecognitionUtilityObserver;

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CreateInstanceL
// Creates instance of CSISpeechRecognitionUtility
// Needed when dynamically loading nsssispeechrecognitionutility.dll
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CSISpeechRecognitionUtility::CreateInstanceL()
    {
    CSISpeechRecognitionUtility* utility = new (ELeave) CSISpeechRecognitionUtility();
    return static_cast<MSISpeechRecognitionUtilityBase*>( utility );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CreateInstanceSecondPhaseL
// Does second phase construction after CreateInstanceL
// Needed when dynamically loading nsssispeechrecognitionutility.dll
// -----------------------------------------------------------------------------
//
EXPORT_C void CSISpeechRecognitionUtility::CreateInstanceSecondPhaseL( TUid aClientUid, 
                                                                       TUid aPluginUid, 
                                                                       MSISpeechRecognitionUtilityObserver& aObserver )
    {
    ConstructL( aClientUid, aPluginUid, aObserver );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CSISpeechRecognitionUtility
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CSISpeechRecognitionUtility::CSISpeechRecognitionUtility(): iSrCustomCommands( iMMFController )
    {	
    // Nothing
    }

// -----------------------------------------------------------------------------
// FindSpeechPluginL
// Searches for SI Controller Plugin.
// -----------------------------------------------------------------------------
//
TUid FindSpeechPluginL()
    {
    RUBY_DEBUG_BLOCK( "CSISpeechRecognitionUtility::FindSpeechPluginL" );

	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);

	CMMFControllerPluginSelectionParameters* selectionParams = 
					CMMFControllerPluginSelectionParameters::NewLC();

	// Select the media IDs to allow
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(KUidMediaTypeSI));
	selectionParams->SetMediaIdsL(mediaIds,
			CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);
	CleanupStack::PopAndDestroy(&mediaIds);

	selectionParams->ListImplementationsL(controllers);	
	
	// make sure at least on controller has been found
	if( ! controllers.Count() )
	    {
        RUBY_DEBUG1( "No controllers found using media type %x", KUidMediaTypeSI );
		User::Leave( KErrNotFound );
    	}

    TUid result = controllers[0]->Uid();

    CleanupStack::PopAndDestroy( 2 );
    return( result );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionUtility::ConstructL( TUid aClientUid, 
                                              TUid aPluginUid, 
                                              MSISpeechRecognitionUtilityObserver& aObserver )
    {
    RUBY_DEBUG_BLOCK( "CSISpeechRecognitionUtility::ConstructL" );
    
    iSpeechRecognitionUtilityObserver = &aObserver;
    
    // dummy priority settings, they are set for real when record, recognize 
    // and playback are called
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = 0;
    prioritySettings.iPref = (TMdaPriorityPreference) 0;
    prioritySettings.iState = EMMFStateIdle; // idle
    
    if ( aPluginUid.iUid == 0 )
        {
        aPluginUid = FindSpeechPluginL();
        }
    
    // load the controller
    // coded this way for debugging
    TInt err;
    if ((err = iMMFController.Open(aPluginUid, prioritySettings))
        == KErrNone) 
        {
        // start the event monitor
        iControllerEventMonitor = 
            CMMFControllerEventMonitor::NewL(*this, iMMFController);
        iControllerEventMonitor->Start();
        }
    else
        {
        RUBY_DEBUG2( "Unable to load controller plugin, error = %d, UID = %x", err, aPluginUid.iUid );
        User::Leave(err);
        }
    
    User::LeaveIfError(iSrCustomCommands.SetClientUid(aClientUid));
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSISpeechRecognitionUtility* CSISpeechRecognitionUtility::NewL(
	MSISpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver, TUid aClientUid)
	{

	CSISpeechRecognitionUtility* self = NewLC(aSpeechRecognitionUtilityObserver, aClientUid);
	// remove the pointer from the cleanup stack
	CleanupStack::Pop( self );	
    return self;

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::NewLC
// Two-phased constructor, leaves a pointer to the object on the stack.
// -----------------------------------------------------------------------------
//
EXPORT_C CSISpeechRecognitionUtility* CSISpeechRecognitionUtility::NewLC(
	MSISpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver, TUid aClientUid)
	{

	CSISpeechRecognitionUtility* self = new (ELeave)  
		CSISpeechRecognitionUtility(aSpeechRecognitionUtilityObserver);
    // push onto cleanup stack 
    // (in case self->ConstructL leaves)
    CleanupStack::PushL(self);
	// use two-stage construct
    // Plugin UID == 0 -> List all plugins and pick any suitable
    self->ConstructL( aClientUid, KNullUid, aSpeechRecognitionUtilityObserver );
    return self;
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSISpeechRecognitionUtility* CSISpeechRecognitionUtility::NewL(
	MSISpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver,
    TUid aClientUid,
    TUid aMmfPluginUid )
    {
	CSISpeechRecognitionUtility* self = new (ELeave)  
		CSISpeechRecognitionUtility(aSpeechRecognitionUtilityObserver);
    // push onto cleanup stack 
    // (in case self->ConstructL leaves)
    CleanupStack::PushL( self );
	// use two-stage construct
    self->ConstructL(aClientUid, aMmfPluginUid, aSpeechRecognitionUtilityObserver);      
    CleanupStack::Pop( self );
    return( NULL );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::~CSISpeechRecognitionUtility
// The destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSISpeechRecognitionUtility::~CSISpeechRecognitionUtility()
	{

	// indicate we don't want any callbacks
	SetEventHandler(NULL);
	// cancel any pending requests
	if ( iControllerEventMonitor )	// make sure we have a valid pointer
	{
		iControllerEventMonitor->Cancel();
	}

	delete iControllerEventMonitor;
	iMMFController.Close();

    REComSession::FinalClose();
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::GetEngineProperties
// Retreive the properties of the underlying speech recognition engine. Returns
// an object containing the engine properties in the aProperties variable.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetEngineProperties(
	const RArray<TInt>& aPropertyId, 
	RArray<TInt>& aPropertyValue)
	{
	return iSrCustomCommands.GetEngineProperties(aPropertyId, 
									aPropertyValue);
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::Adapt
// Performs adaptation for acoustic models based on correct result.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::Adapt(
    const CSIClientResultSet& aResultSet,
    TInt aCorrect )
    {
    return iSrCustomCommands.Adapt( aResultSet, aCorrect );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::AddPronunciation
// Adds a new pronunciation into the Lexicon. Returns an the ID of the 
// pronunciation in the aPronunciationID variable.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::AddPronunciation(
    TSILexiconID aLexiconID,
    const TDesC& aTextForTraining,
    TLanguage aLanguage,
    TSIPronunciationID& aPronunciationID)
    {
    return iSrCustomCommands.AddPronunciation( aLexiconID, aTextForTraining,
                                               aLanguage, aPronunciationID );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::AddRule
// Adds a new rule into the Grammar. Returns an the ID of the pronunciation 
// in the aPronunciationID variable.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::AddRule(
	TSIGrammarID aGrammarID, 
	TSILexiconID aLexiconID, 
	TSIPronunciationID aPronunciationID, 
	TSIRuleID& aRuleID)
	{

	// async
	return iSrCustomCommands.AddRule(aGrammarID, aLexiconID, 
				aPronunciationID, aRuleID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::AddRuleVariant
// Adds a new rule variant for the given pronunciation into the 
// specified grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::AddRuleVariant(
    TSIGrammarID aGrammarID,
    TSILexiconID aLexiconID,
    RArray<TSIPronunciationID>& aPronunciationIDs,
    TSIRuleID aRuleID,
    TSIRuleVariantID& aRuleVariantID )
    {
    return iSrCustomCommands.AddRuleVariant( aGrammarID, aLexiconID, aPronunciationIDs, aRuleID, aRuleVariantID );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::AddVoiceTag
// Adds a new rule for the given phrase into the 
// specified grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::AddVoiceTag( MDesCArray& aTextArray,
                                                        const RArray<TLanguage>& aLanguageArray,
                                                        TSILexiconID aLexiconID,
                                                        TSIGrammarID aGrammarID,
                                                        TSIRuleID& aRuleID )
    {
    return iSrCustomCommands.AddVoiceTag( aTextArray, aLanguageArray,
                                          aLexiconID, aGrammarID, aRuleID );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::AddVoiceTag
// Adds a new rule for the given phrase into the 
// specified grammar.
// -----------------------------------------------------------------------------
//
#ifdef __SINDE_TRAINING
EXPORT_C TInt CSISpeechRecognitionUtility::AddVoiceTag( MDesCArray& aTextArray,
                                                        const RArray<RLanguageArray>& aLanguageArray,
                                                        TSILexiconID aLexiconID,
                                                        TSIGrammarID aGrammarID,
                                                        TSIRuleID& aRuleID )
    {
    return iSrCustomCommands.AddVoiceTag( aTextArray, aLanguageArray,
                                          aLexiconID, aGrammarID, aRuleID );
    }
#else
EXPORT_C TInt CSISpeechRecognitionUtility::AddVoiceTag( MDesCArray& /*aTextArray*/,
                                                        const RArray<RLanguageArray>& /*aLanguageArray*/,
                                                        TSILexiconID /*aLexiconID*/,
                                                        TSIGrammarID /*aGrammarID*/,
                                                        TSIRuleID& /*aRuleID*/ )
    {
    return KErrNotSupported;
    }
#endif // __SINDE_TRAINING

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::AddVoiceTags
// Adds a new rules for the given phrases into the 
// specified grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::AddVoiceTags( const RPointerArray<MDesCArray>& aTextArrayArray,
                                                         const RArray<TLanguage>& aLanguageArray,
                                                         TSILexiconID aLexiconID,
                                                         TSIGrammarID aGrammarID, 
                                                         RArray<TSIRuleID>& aRuleIDArray )
    {
    if ( iRuleIDs != 0 )
        {
        return( KErrInUse );
        }
    iRuleIDs = &aRuleIDArray;

    return iSrCustomCommands.AddVoiceTags( aTextArrayArray, aLanguageArray,
                                           aLexiconID, aGrammarID, aRuleIDArray);
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::AddVoiceTags
// Adds a new rules for the given phrases into the 
// specified grammar.
// -----------------------------------------------------------------------------
//
#ifdef __SINDE_TRAINING
EXPORT_C TInt CSISpeechRecognitionUtility::AddVoiceTags( const RPointerArray<MDesCArray>& aTextArrayArray,
                                                         const RArray<RLanguageArray>& aLanguageArray,
                                                         TSILexiconID aLexiconID,
                                                         TSIGrammarID aGrammarID, 
                                                         RArray<TSIRuleID>& aRuleIDArray )
    {
    if ( iRuleIDs != 0 )
        {
        return( KErrInUse );
        }
    iRuleIDs = &aRuleIDArray;

    return iSrCustomCommands.AddVoiceTags( aTextArrayArray, aLanguageArray,
                                           aLexiconID, aGrammarID, aRuleIDArray );
    }
#else
EXPORT_C TInt CSISpeechRecognitionUtility::AddVoiceTags( const RPointerArray<MDesCArray>& /*aTextArrayArray*/,
                                                         const RArray<RLanguageArray>& /*aLanguageArray*/,
                                                         TSILexiconID /*aLexiconID*/,
                                                         TSIGrammarID /*aGrammarID*/, 
                                                         RArray<TSIRuleID>& /*aRuleIDArray*/ )
    {
    return KErrNotSupported;
    }
#endif // __SINDE_TRAINING

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::Cancel
// Cancel the current asynchronous operation.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSISpeechRecognitionUtility::Cancel()
	{

	iSrCustomCommands.Cancel();

	// Now cancel has been called, so we can get rid of pointers
	iPronunciationIDs = NULL;
	iRuleIDs = NULL;
	iModelIDs = NULL;
	iResultSet = NULL;
	iGrammarIDs = NULL;
	iLexiconIDs = NULL;
	iModelBankIDs = NULL;

    iSIResultSet = NULL;
    iResultSet = NULL;
	}

// -----------------------------------------------------------------------------
// CSpeechRecognitionCustomCommandParser::CommitChanges
// Commit the current changes to permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::CommitChanges()
	{

	// async
	return iSrCustomCommands.CommitChanges();

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CreateGrammar
// Create a new grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::CreateGrammar(
	TSIGrammarID& aGrammarID)
	{

	// async
	return iSrCustomCommands.CreateGrammar(aGrammarID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CreateLexicon
// Create a new lexicon.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::CreateLexicon(
	TSILexiconID& aLexiconID)
	{

	// async
	return iSrCustomCommands.CreateLexicon(aLexiconID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CreateRule
// Creates a new rule.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::CreateRule(
    TSIGrammarID aGrammarID,
    TSIRuleID& aRuleID )
    {
    return iSrCustomCommands.CreateRule( aGrammarID, aRuleID );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::LoadModels
// Loads the model specified by aModelBankID into the recognizer.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::LoadModels(
	TSIModelBankID aModelBankID)
	{

	// async
	return iSrCustomCommands.LoadModels(aModelBankID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::LoadGrammar
// Loads the grammar specified by aGrammarID into the recognizer.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::LoadGrammar(
	TSIGrammarID aGrammarID)
	{

	// async
	return iSrCustomCommands.LoadGrammar(aGrammarID);
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::ActivateGrammar
// Activates the grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::ActivateGrammar(
	TSIGrammarID aGrammarID)
	{

	// async
	return iSrCustomCommands.ActivateGrammar(aGrammarID);
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::DeactivateGrammar
// Deactivates the grammar.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::DeactivateGrammar(
	TSIGrammarID aGrammarID)
	{

	// async
	return iSrCustomCommands.DeactivateGrammar(aGrammarID);
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::LoadLexicon
// Loads the lexicon specified by aLexiconID into the recognizer.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::LoadLexicon(
	TSILexiconID aLexiconID)
	{

	// async
	return iSrCustomCommands.LoadLexicon(aLexiconID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::GetModelCount
// Returns the number of models in the model bank specified by aModelBankID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetModelCount(
	TSIModelBankID aModelBankID, 
	TInt& aModelCount)
	{

	// async
	return iSrCustomCommands.GetModelCount(aModelBankID, aModelCount);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::EndRecSession
// Ends the current recognition session and frees the associated resources.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::EndRecSession()
	{

	return iSrCustomCommands.EndRecSession();

	}


// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::EndRecSession
// Start a recognition session and and allocate the required resources.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::StartRecSession(
	TNSSRecognitionMode aMode)
	{

	return iSrCustomCommands.StartRecSession(aMode);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::Recognize
// Initiates recognition.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::Recognize(
	CSIClientResultSet& aResultSet)
	{
    if ( iSIResultSet != NULL || iResultSet != NULL )
        {
        return( KErrInUse );
        }
	TInt err = iSrCustomCommands.Recognize( aResultSet );

    if ( err == KErrNone )
        {
        iSIResultSet = &aResultSet;
        }

    return( err );
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::Record
// Begins recording the utterance for aRecordTime microseconds.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::Record(
	TTimeIntervalMicroSeconds32 aRecordTime)
	{
    // setup the priority for recognition
	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPriority = iAudioPriority;
	prioritySettings.iPref = (TMdaPriorityPreference) iRecognitionPreference;
	prioritySettings.iState = EMMFStateIdle; // idle
	iMMFController.SetPrioritySettings(prioritySettings);

	// async
	return iSrCustomCommands.Record(aRecordTime);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::EndRecord
// Begins recording the utterance for aRecordTime microseconds.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::EndRecord()
	{
    return iSrCustomCommands.EndRecord();
	}
	
// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::PreStartSampling
// Begins sampling before the actual recognition.
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSISpeechRecognitionUtility::PreStartSampling()
    {
    return iSrCustomCommands.PreStartSampling();
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::RemoveGrammar
// Removes the grammer specified by aGrammarID from permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::RemoveGrammar(
	TSIGrammarID aGrammarID)
	{

	// async
	return iSrCustomCommands.RemoveGrammar(aGrammarID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::RemovePronunciation
// Removes the pronunciation specified by aPronunciationID from permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::RemovePronunciation(
	TSILexiconID aLexiconID, 
	TSIPronunciationID aPronunciationID)
	{

	// async
	return iSrCustomCommands.RemovePronunciation(aLexiconID, aPronunciationID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::RemoveLexicon
// Removes the lexicon specified by aLexiconID from permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::RemoveLexicon(
	TSILexiconID aLexiconID)
	{

	// async
	return iSrCustomCommands.RemoveLexicon(aLexiconID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::RemoveModel
// Removes the model specified by aModelID from permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::RemoveModel(
	TSIModelBankID aModelBankID, 
	TSIModelID aModelID)
	{

	// async
	return iSrCustomCommands.RemoveModel(aModelBankID, aModelID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::RemoveRule
// Removes the rule specified by aRuleID from permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::RemoveRule(
	TSIGrammarID aGrammarID, 
	TSIRuleID aRuleID)
	{

	// async
	return iSrCustomCommands.RemoveRule(aGrammarID, aRuleID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::RemoveRules
// Removes the rule specified by aRuleID from permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::RemoveRules(
	TSIGrammarID aGrammarID, 
	RArray<TSIRuleID>& aRuleIDs)
	{

	// async
	return iSrCustomCommands.RemoveRules(aGrammarID, aRuleIDs);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::UnloadRule
// Unloads a rule temporary memory. The rule in the permanent storage is not 
// removed.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::UnloadRule(
	TSIGrammarID aGrammarID, 
	TSIRuleID aRuleID)
	{

	// async
	return iSrCustomCommands.UnloadRule(aGrammarID, aRuleID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::UnloadGrammar
// Unloads the specified grammar from the list of grammars
// in temporary memory, previously loaded with LoadGrammarL.  
// The grammar in the permanent storage remains intact.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::UnloadGrammar(
    TSIGrammarID aGrammarID )
    {
    return iSrCustomCommands.UnloadGrammar( aGrammarID );
    }

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::SetEventHandler
// Sets the callback address for asynchronous functions.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSISpeechRecognitionUtility::SetEventHandler(
	MSISpeechRecognitionUtilityObserver* aSpeechRecognitionUtilityObserver)
	{

	// save the new event handler callback address
	iSpeechRecognitionUtilityObserver = aSpeechRecognitionUtilityObserver;

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::GetAllPronunciationIDs
// Returns all pronunciation IDs that exist in the aLexiconID lexicon.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllPronunciationIDs(
	TSILexiconID aLexiconID, 
	RArray <TSIPronunciationID>& aPronunciationIDs)
	{

	if (iPronunciationIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllPronunciationIDs(aLexiconID);
	
	
	if (err == KErrNone)
	{
		iPronunciationIDs = &aPronunciationIDs;
	}
	
	return err;

	}

// -----------------------------------------------------------------------------
// CSpeechRecognitionCustomCommandParser::GetAllClientGrammarIDs
// Returns all client grammar IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllClientGrammarIDs( 
	RArray <TSIGrammarID>& aGrammarIDs )
	{


	if (iGrammarIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllClientGrammarIDs();
	
	if (err == KErrNone)
	{
		iGrammarIDs = &aGrammarIDs;
	}
	
	return err;

	}

// -----------------------------------------------------------------------------
// CSpeechRecognitionCustomCommandParser::GetAllClientLexiconIDs
// Returns all client lexicon IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllClientLexiconIDs( 
	RArray <TSILexiconID>& aLexiconIDs )
	{

	if (iLexiconIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllClientLexiconIDs();
	
	if (err == KErrNone)
	{
		iLexiconIDs = &aLexiconIDs;
	}
	
	return err;

	}

// -----------------------------------------------------------------------------
// CSpeechRecognitionCustomCommandParser::GetAllClientModelBankIDs
// Returns all client model bank IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllClientModelBankIDs( 
	RArray <TSIModelBankID>& aModelBankIDs )
	{

	if (iModelBankIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllClientModelBankIDs();
	
	if (err == KErrNone)
	{
		iModelBankIDs = &aModelBankIDs;
	}
	
	return err;

	}


// -----------------------------------------------------------------------------
// CSpeechRecognitionCustomCommandParser::GetAllGrammarIDs
// Returns all grammar IDs for all clients.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllGrammarIDs( 
	RArray <TSIGrammarID>& aGrammarIDs )
	{

	if (iGrammarIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllGrammarIDs();
	
	if (err == KErrNone)
	{
		iGrammarIDs = &aGrammarIDs;
	}
	
	return err;

	}

// -----------------------------------------------------------------------------
// CSpeechRecognitionCustomCommandParser::GetAllLexiconIDs
// Returns all lexicon IDs for all clients.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllLexiconIDs( 
	RArray <TSILexiconID>& aLexiconIDs )
	{

	if (iLexiconIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllLexiconIDs();
	
	if (err == KErrNone)
	{
		iLexiconIDs = &aLexiconIDs;
	}
	
	return err;

	}

// -----------------------------------------------------------------------------
// CSpeechRecognitionCustomCommandParser::GetAllModelBankIDs
// Returns all model bank IDs for all clients.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllModelBankIDs( 
	RArray <TSIModelBankID>& aModelBankIDs )
	{

	if (iModelBankIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllModelBankIDs();
	
	if (err == KErrNone)
	{
		iModelBankIDs = &aModelBankIDs;
	}

	return err;

	}


// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::GetAllModelIDs
// Returns all model IDs that exist in the aModelBankID bank.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllModelIDs(
	TSIModelBankID aModelBankID, 
	RArray <TSIModelID>& aModelIDs)
	{

	if (iModelIDs != NULL)
	{
		return KErrInUse;
	}

	TInt err = iSrCustomCommands.GetAllModelIDs(aModelBankID);
	
	if (err == KErrNone)
	{
		iModelIDs = &aModelIDs;
	}

	return err;
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::GetRuleValidity
// Returns whether the specified rule is valid in the aValid variable.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetRuleValidity(
	TSIGrammarID aGrammarID, 
	TSIRuleID aRuleID, 
	TBool& aValid)
	{

	// async
	return iSrCustomCommands.GetRuleValidity(aGrammarID, aRuleID, aValid);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::GetAllRuleIDs
// Returns all grammar IDs that exist in the aGrammarID bank.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::GetAllRuleIDs(
	TSIGrammarID aGrammarID, 
	RArray <TSIRuleID>& aRuleIDs)
	{

	if (iRuleIDs != NULL)
	{
		return KErrInUse;
	}
		
	TInt err = iSrCustomCommands.GetAllRuleIDs(aGrammarID);
	
	if (err == KErrNone)	
	{
		iRuleIDs = &aRuleIDs;
	}
		
	return err;
	
	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::CreateModelBank
// Creates a model bank and returns ID of the new model bank in aModelBankID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::CreateModelBank(
	TSIModelBankID& aModelBankID)
	{

	// async
	return iSrCustomCommands.CreateModelBank(aModelBankID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::RemoveModelBank
// Removes the model bank specified by aModelBankID from permanent storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::RemoveModelBank(
	TSIModelBankID aModelBankID)
	{

	// async
	return iSrCustomCommands.RemoveModelBank(aModelBankID);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::LoadEngineParameters
// Sends a command to the controller to load the parameters into
// the engine.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::LoadEngineParameters(
	const RArray<TInt>& aParameterId, 
	const RArray<TInt>& aParameterValue)
	{

	return iSrCustomCommands.LoadEngineParameters(aParameterId, aParameterValue);

	}

// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::SetAudioPriority
// Set the audio priority and preference for training, recognition and
// playback.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSISpeechRecognitionUtility::SetAudioPriority(
	TInt aPriority, 
	TInt aTrainPreference, 
	TInt aPlaybackPreference, 
	TInt aRecognitionPreference)
	{
	
	iAudioPriority = aPriority;
	iTrainPreference = aTrainPreference;
	iPlaybackPreference = aPlaybackPreference;
	iRecognitionPreference = aRecognitionPreference;

	return KErrNone;

	}


// -----------------------------------------------------------------------------
// CSISpeechRecognitionUtility::HandleEvent
// Callback function for asynchrounus events. Settting the address to NULL
// insures no unexpected callback will occur.
// -----------------------------------------------------------------------------
//
void CSISpeechRecognitionUtility::HandleEvent( const TMMFEvent& aEvent )
	{

	TBool cancelled = EFalse;

	TInt event = (TInt)aEvent.iEventType.iUid;
	TInt result = (TInt)aEvent.iErrorCode;

    RUBY_DEBUG2( "CSISpeechRecognitionUtility::HandleEvent() - Event Type = %d, Event Result = %d", event, result );

	switch (event)
	    {
		case KUidAsrEventGetAllPronunciationIDsVal:
			if (iPronunciationIDs)
			{
				if ( result == KErrNone )
				{
					TRAP(result, 
						iSrCustomCommands.GetPronunciationIDArrayL(*iPronunciationIDs));
				}
				// async operation complete, so no need to store pointer anymore
				iPronunciationIDs = NULL;
			}
			else 
			{
				cancelled = ETrue;
			}
			break;
		

		case KUidAsrEventGetAllRuleIDsVal:
			if (iRuleIDs)
			{
				if ( result == KErrNone )
				{
					TRAP(result, iSrCustomCommands.GetRuleIDArrayL(*iRuleIDs));		
				}
				// async operation complete, so no need to store pointer anymore	
				iRuleIDs = NULL;
			}
			else
			{
				cancelled = ETrue;
			}
			break;

		case KUidAsrEventGetAllModelIDsVal:
			if (iModelIDs)
			{
				if ( result == KErrNone )
				{
					TRAP(result, iSrCustomCommands.GetModelIDArrayL(*iModelIDs));
				}
				// async operation complete, so no need to store pointer anymore
				iModelIDs = NULL;
			}
			else
			{
				cancelled = ETrue;
			}
			break;

		case KUidAsrEventRecordVal:
			// if the record function has an error during recognition 
			// then clear iResultSet pointer.
			if ( (iResultSet) && (result != KErrNone) )
			{
				// async operation complete, so no need to store pointer anymore
				iResultSet = NULL;
			}
			break;

		// both of these conditions can use the same functionality
		case KUidAsrEventGetAllClientGrammarIDsVal:
		case KUidAsrEventGetAllGrammarIDsVal:
			if (iGrammarIDs)
			{
				if ( result == KErrNone )
				{
					TRAP(result, iSrCustomCommands.GetGrammarIDArrayL(*iGrammarIDs));
				}
				// async operation complete, so no need to store pointer anymore
				iGrammarIDs = NULL;
			}
			else
			{
				cancelled = ETrue;
			}
			break;

		// both of these conditions can use the same functionality
		case KUidAsrEventGetAllClientLexiconIDsVal:
		case KUidAsrEventGetAllLexiconIDsVal:
			if (iLexiconIDs)
			{
				if ( result == KErrNone )
				{
					TRAP(result, iSrCustomCommands.GetLexiconIDArrayL(*iLexiconIDs));
				}
				// async operation complete, so no need to store pointer anymore
				iLexiconIDs = NULL;
			}
			else
			{
				cancelled = ETrue;
			}
			break;

		// both of these conditions can use the same functionality
		case KUidAsrEventGetAllClientModelBankIDsVal:
		case KUidAsrEventGetAllModelBankIDsVal:

			if (iModelBankIDs)
			{
				if ( result == KErrNone )
				{
					TRAP(result, iSrCustomCommands.GetModelBankIDArrayL(*iModelBankIDs));
				}
				// async operation complete, so no need to store pointer anymore
				iModelBankIDs = NULL;
			}
			else
			{
				cancelled = ETrue;
			}
			break;

        // SI functions, which require postprocessing
        case KUidAsrEventAddVoiceTagsVal:

			if (iRuleIDs)
			{
				if ( result == KErrNone )
				{
					TRAP(result, 
						iSrCustomCommands.GetRuleIDArrayL(*iRuleIDs));
				}
				// async operation complete, so no need to store pointer anymore
				iRuleIDs = NULL;
			}
			else 
			{
				cancelled = ETrue;
			}
            break;

        case KUidAsrEventRecognitionVal:

			if (iSIResultSet)
			{
				if ( result == KErrNone )
				{
					TRAP(result, iSrCustomCommands.GetSIResultSetL(*iSIResultSet));
				}
				// async operation complete, so no need to store pointer anymore
				iSIResultSet = NULL;
			}
			else
			{
				cancelled = ETrue;
			}
			break;
          
		default:
			break;
	
	    }

	if ( iSpeechRecognitionUtilityObserver && !cancelled )
        {
        iSpeechRecognitionUtilityObserver->MsruoEvent( aEvent.iEventType, result );
        }
	}

// end of file
