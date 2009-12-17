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
* Description:  This class handles the storage and access of user trained models
*				 and utterances into a database.  It performs checksum verification
*				 for the stored data.  It is also responsible for allocating memory
*				 when loading the models into the recognizer.
*
*/


#ifndef CSIMODELBANKDB_H
#define CSIMODELBANKDB_H

//  INCLUDES
#include <nsssispeechrecognitiondatadevasr.h>

#include "asrplugindataloader.h"
#include "sicommondb.h"

// CLASS DECLARATION

/**
*  This class implements Speaker Independent Model Database.
*
*  @lib SIControllerPlugin.lib
*  @since 2.0
*/
class CSIModelBankDB : public CSICommonDB
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSIModelBankDB* NewL( RDbNamedDatabase& aDatabase, 
                                     RDbs& aDbSession,
                                     TInt aDrive );

        /**
        * Destructor.
        */
        virtual ~CSIModelBankDB();

    public: // New functions

        /**
		* Returns a model bank object containing all speaker independent models of
		* the specified model bank.
		* @since 2.0
		* @param	aModelBankID		model bank Id
		* @return	pointer to a bank bank object
        */
        const CSIModelBank* AllAcousticModelsL( TSIModelBankID aModelBankID ); //*

		/**
        * Returns the total number of models in the system (all model banks).
        * @since	2.0
        * @param	-
        * @return	Number of models in all model banks.
        */
		TInt AllModelCountL(); 

        /**
        * Creates a new model bank ID table in the database.
        * @since 2.0
        * @param	-
        * @return	-
        */
        void CreateIDTableL(); 

        /**
        * Creates a new model bank in the database.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
        * @return	new model bank Id
        */
        TSIModelBankID CreateModelBankL( TUid aClientUid ); 

        /**
        * Returns all model bank Ids that belong to the specified client.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
		* @param	aModelBankIDs		reference where model bank Ids are stored
        * @return	-
        */
		void GetAllClientModelBankIDsL( TUid aClientUid, RArray<TSIModelBankID>& aModelBankIDs ); //*

        /**
        * Returns all model bank Ids in the database.
        * @since 2.0
		* @param	aLexiconIDs			reference where model bank Ids are stored
        * @return	-
        */
		void GetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs ); //*

        /**
        * Returns all model Ids that exist in the specified model bank.
        * @since 2.0
        * @param	aModelBankID		model bank Id
		* @param	aModelIDs			reference where model Ids are stored
        * @return	-
        */
        void GetAllModelIDsL( TSIModelBankID aModelBankID, RArray<TSIModelID>& aModelIDs ); //*

		/**
        * Returns the duration of the utterance for the specified model.
        * @since 2.0
        * @param	aModelBankID		model bank Id
		* @param	aModelID			model Id
		* @param	aDuration			reference where the duration of utterance is set
        * @return	-
        */
		void GetUtteranceDurationL( TSIModelBankID aModelBankID, TSIModelID aModelID,
					TTimeIntervalMicroSeconds32& aDuration ); //*

		/**
        * Checks if the model bank is valid or not and also verify ownership.
		* Leaves with KErrAsrDataRightViolation if not owned by the client.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
        * @param	aModelBankID		model bank Id
        * @return	ETrue if the model bank is valid
        */
        TBool IsModelBankValidL( TUid aClientUid, TSIModelBankID aModelBankID ); //*

		/**
        * Checks if the model is valid or not.
        * @since 2.0
        * @param	aModelBankID		model bank Id
		* @param	aModelID			model Id
        * @return	ETrue if the model is valid
        */
		TBool IsModelValidL( TSIModelBankID aModelBankID, TSIModelID aModelID ); //*

		/**
        * Checks if the model bank is valid and update if necessary.
        * @since 3.1
        * @param	aModelBankID		model bank Id
        */
        void UpdateModelBankIfInvalidL( TSIModelBankID aModelBankID );
		/**
        * Returns the number of models in the specified model bank.
        * @since 2.0
        * @param	aModelBankID		model bank Id
        * @return	Number of models
        */
        TInt ModelCountL( TSIModelBankID aModelBankID ); //*

		/**
        * Removes the specified model bank from the database.
		* Removing a model bank will remove all models within the model bank.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
        * @param	aModelBankID		model bank Id
        * @return	-
        */
        void RemoveModelBankL( TUid aClientUid, TSIModelBankID aModelBankID ); //*

		/**
        * Removes the specified model from the database.
        * @since 2.0
        * @param	aClientUid			client's Uid for data ownership
        * @param	aModelBankID		model bank Id
		* @param	aModelID			model Id
        * @return	-
        */
        void RemoveModelL( TUid aClientUid, TSIModelBankID aModelBankID, TSIModelID aModelID ); //*

		/**
        * Deallocates the temporary memory allocated during training.
        * @since 2.0
        * @param	-
        * @return	-
        */
        void Reset(); //*

		/**
        * Deallocates the temporary memory allocated during recognition.
        * @since 2.0
        * @param	-
        * @return	-
        */
		void ResetAndDestroy(); //*

		/**
        * Saves the trained model in the temporary memory into the database.
        * @since 2.0
        * @param	aModelBankID		model bank Id
        * @return	A new model ID
        */
        TSIModelID SaveModelL( TSIModelBankID aModelBankID ); //*

		/**
        * Saves the acoustic model into a temporary memory during training.
        * @since 2.0
        * @param	aAcousticModelBuf	buffer containing the acoustic model
        * @return	-
        */
        void SetAcousticModelL( const TPtr8& aAcousticModelBuf ); //*

		/**
        * Saves the utterance duration of a trained model in temporary memory during training.
        * @since 2.0
        * @param	aUtteranceDuration	utterace duration in microseconds
        * @return	-
        */
        void SetUtteranceDurationL( TInt aUtteranceDuration ); //*

		/**
        * Returns a reference to a pointer descriptor with the specified length where the
		* utterance will be stored.
        * @since 2.0
        * @param	aLength				length of the requested descriptor
        * @return	Reference to a pointer descriptor
        */
		// TPtr8& UtteranceBufferL( TInt aLength ); //*

		/**
        * Returns a reference to a previously trained user utterance of the specified model.
        * @since 2.0
        * @param	aModelBankID		model bank Id
		* @param	aModelID			model Id
        * @return	Reference to a user utterance pointer descriptor
        */
        // TPtr8& UtteranceL( TSIModelBankID aModelBankID, TSIModelID aModelID ); //*

  		// test
		RPointerArray<CSIModelBank>& ModelBankArray();

    private:

        /**
        * C++ default constructor.
        */
        CSIModelBankDB( RDbNamedDatabase& aDatabase, RDbs& aDbSession, TInt aDrive );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Calculates a checksum value for the given buffer.
        * @since 2.0
        * @param	aBuf				reference to a pointer descriptor containing binary data
        * @return	Checksum value
        */
        TInt32 CalculateChecksum( const TPtr8& aBuf );

        /**
        * Verifies the checksum value for the given buffer.
        * @since 2.0
        * @param	aBuf				reference to a pointer descriptor containing binary data
		* @param	aChecksum			checksum value to compare to
        * @return	ETrue if the checksum of aBuf is equal to aChecksum
        */
        TBool VerifyChecksum( const TPtr8& aBuf, TInt32 aChecksum );
		const CSIModelBank* GetAllAcousticModelsL( TSIModelBankID aModelBankID );

    private:    // Data

		// Temporary memory where loaded model banks are held during recognition session
		RPointerArray<CSIModelBank> iModelBankArray;

		// Pointer to acoustic model - used during training
		TPtr8 iAcousticModelPtr;
 		// Pointer to the user utterance
	//	TPtr8 iUtterancePtr;
		 
		CDataLoader* iDataLoader;

		// Temporary memory where loaded SI model banks are held during recognition session
		//RPointerArray<CSIModelBank> iSIModelBankArray;
		HBufC8 *iFileBuffer;
		RFs iFs;
    };

#endif      // CSIModelBankDB_H

// End of File
