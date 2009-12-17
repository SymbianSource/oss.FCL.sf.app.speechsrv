/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The MNssCoreSrsDBEventHandler provides interface to handle events
*                from Save and Delete utterance data into SRS.
*
*/


#ifndef MNSSCORESRSDBEVENTHANDLER_H
#define MNSSCORESRSDBEVENTHANDLER_H


/**
* The MNssCoreSrsDBEventHandler provides interface to handle events
* from Save and Delete utterance data into SRS.
*
*  @lib NssVasApi.lib
*  @since 2.0
*/
class MNssCoreSrsDBEventHandler
{
public:

   enum TNssSrsDBResult
   {
      EVasSuccess,
	  EVasPending,
      EVasCommitFailed,
      EVasAddPronunciationFailed,
      EVasAddRuleFailed,
      EVasRemovePronunciationFailed,
      EVasRemoveRuleFailed,
      EVasRemoveModelFailed,
	  EVasInvalidState,
	  // for Context portal
	  EVasCreateGrammarFailed,
	  EVasCreateModelBankFailed,
	  EVasCreateLexiconFailed,
	  EVasRemoveGrammarFailed,
	  EVasRemoveModelBankFailed,
	  EVasRemoveLexiconFailed,
      // for generic error
      EVasUnexpectedRequest,   // Unexpected request from client, when the state is not Idle
      EVasUnexpectedEvent,
      EVasInUse,               // VAS resource is currently in use.
      EVasDiskFull,            // SRF disk is full
      EVasInvalidParameter,
      EVasDBOperationError,
      EVasNoMemory
   };
   
   /**
   * Callback 
   * @param None
   * @return None
   */
   virtual void HandleSaveSrsDBCompleted()=0;

   /**
   * Callback 
   * @param None
   * @return None
   */
   virtual void HandleDeleteSrsDBCompleted()=0;

   /**
   * Callback
   * @param None
   * @return None
   */
   virtual void HandleResetSrsDBCompleted()=0;

   /**
   * Callback 
   * @param None
   * @return None
   */
   virtual void HandleSrsDBError(TNssSrsDBResult aResult)=0;


};

#endif
