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
* Description:  CNssContextSrsPortal provides SRS related functionalities for 
*               context processing. A context is saved in the SRS DB by creating 
*               model bank, lexicon, and grammar into the SRS.
*
*/


#ifndef NSSVASCCONTEXTSRSPORTAL_H
#define NSSVASCCONTEXTSRSPORTAL_H

//  INCLUDES
#include "nssvassiutilitywrapper.h"
#include <nsssispeechrecognitionutilityobserver.h>
#include "nssvasccontext.h"
#include "nssvasmcoresrsdbeventhandler.h"

// FORWARD DECLARATIONS
class CNssContext;
 
//FORWARD DECLARATION  
class MNssSpeechRecognitionEvent;

// CLASS DECLARATION

/**
*  CNssContextSrsPortal is the interface to SRS.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssContextSrsPortal: public CBase, public MSISpeechRecognitionUtilityObserver
{

public:

	// ENUMERATIONS
	enum TNssTerminationState
	{
		EVASTERMINATION_PENDING,
		EVASTERMINATION_NOT_PENDING
	};

public:

	/**
    * C++ constructor
    */
	CNssContextSrsPortal();

	/**
    * C++ destructor
    */
	~CNssContextSrsPortal();

public: // from MSpeechRecognitionEvent

	/**
    * overriding interface from MSpeechRecognitionUtilityObserver
	* @since 2.0
    */
	void MsruoEvent(TUid aEvent, TInt aResult);


public: // for inter component use

	/**
    * Save a context to SRS. Asyncronous function.
	* @since 2.0
    * @param aContext - context to be saved
	* @param aSrsDBEventHandler - call back address
    * @return none
    */
	void BeginSaveContextL(CNssContext* aContext, MNssCoreSrsDBEventHandler* aSrsDBEventHandler);

	/**
    * Delete a context from SRS. Asyncronous function
	* @since 2.0
    * @param aContext - context to be deleted
	* @param aSrsDBEventHandler - call back address
    * @return none
    */
	void BeginDeleteContextL(CNssContext* aContext, MNssCoreSrsDBEventHandler* aSrsDBEventHandler);

    /**
    * Reset speaker adapted models: Remove a model bank and create a new one.
    * @since 2.8
    * @param 
    * @return none
    */
    void BeginResetModelsL( TSIModelBankID aAdaptedModels, 
            TSIModelBankID& aNewModels, MNssCoreSrsDBEventHandler* aSrsDBEventHandler );

	/**
    * Commit a context from SRS. Syncronous function
	* @since 2.0
    * @param 
    * @return error code
    */
	TInt CommitSaveContext();

	/**
    * Don't commit a context from SRS. Syncronous function
	* @since 2.8
    * @param 
    * @return error code
    */
	TInt RollbackSaveContext();

	/**
    * Register context to the portal
	* @since 2.0
    * @param 
    * @return
    */
	void Register();

	/**Deregister context from portal 
	* @since 2.0
    * @param 
    * @return 
    */
	void Deregister();

	/**
    * Set portal state to termination pending, or not pending
	* @since 2.0
    * @param aState - termination pending, or not.
    * @return 
    */
	void SetTerminationState(TNssTerminationState aState);

	/**
    * Returns context count, number of contexts registered with the portal
	* @since 2.0
    * @param 
    * @return count
    */
	TInt ContextCount();

	/**
    * Clean up the SRS after the SRS operation fail
	* @since 2.0
    * @param 
    * @return
    */
	void CleanUp(TInt aResult);

	/**
    * Delete the SRS 
	* @since 2.0
    * @param 
    * @return
    */
	void DeleteSRS();

    /**
    * Method to process the SRS DB error 
    * @since 2.0
    * @param aResult - the result value from SRS to indicate an error
    * @return - an error code to identify the DB error
    */   
    MNssCoreSrsDBEventHandler::TNssSrsDBResult DoSrsDBResult(TInt aResult);



private:
	
	// a count of the number of contexts registered to the portal
	TInt					    iContextCount; 

	// state denoting if the builder is destroyed or not
	TNssTerminationState			iTerminationState;

	// instance of the SRS utility class
	CNssSiUtilityWrapper*	iSpeechRecognitionUtility;

	// Pointer to SRS DB Event Handler object.
    MNssCoreSrsDBEventHandler* iSrsDBEventHandler; 
	
	// model bank id, used internally for error handling
	TSIModelBankID iModelBankId;

	// lexicon id, used internally for error handling
	TSILexiconID iLexiconId;

	// grammar id, used internally for error handling
	TSIGrammarID iGrammarId;

	// local pointer to context, used internally for error handling
	CNssContext *iContext;

	// flag for recovery during error cases
	TBool iRecovery;

    // when resetting speaker adaptation, the new model bank ID is stored here.
    TSIModelBankID* iResetModelBankId;

	// states required for EDiskFull errors from SRS
	enum TState
	{
		EVASNone,
		EVASCreateModelBank,
		EVASCreateLexicon,
		EVASCreateGrammar,
		EVASRemoveModelBank,
		EVASRemoveLexicon,
		EVASRemoveGrammar,
        EVASResetRemoveModelBank,
        EVASResetCreateModelBank,
        EVasWaitForCommitChanges
	} iState;
};


#endif // NSSVASCCONTEXTSRSPORTAL_H
            
// End of File
