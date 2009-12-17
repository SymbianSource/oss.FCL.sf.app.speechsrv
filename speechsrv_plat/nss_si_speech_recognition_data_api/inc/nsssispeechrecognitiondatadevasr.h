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
* Description:     This file contains definition of the SISpeechRecognitionDataDevASR structure and API.
*
*/





#ifndef __SRSIDATA_DEVASR_H__
#define __SRSIDATA_DEVASR_H__

//  INCLUDES
#include <nsssispeechrecognitiondatacommon.h>
 
// FORWARD DECLARATION
class CSILexicon;
class CSICompiledGrammar;


// CLASS DECLARATION

/**
*  Class to encapsulate a user defined parameters
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIParameters : public CBase
	{
	public: // Constructors and destructor
		/**
		* Destructor.
        */
		IMPORT_C virtual ~CSIParameters();

		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CSIParameters* NewL();

		IMPORT_C static CSIParameters* NewLC();
	public: // New functions
		
		/**
        * Use to set an internal parameter.
        * @since 2.8
		* @param aParameterIndex Parameter index
		* @param aParameter Parameter value
        * @return None.
        */ 
		IMPORT_C virtual void SetParameterL( const TInt aParameterID, const TInt aParameterValue );
		
		/**
        * Use to retrieve an internal parameter.
        * @since 2.8
		* @param aParameterIndex Parameter index
        * @return Parameter value.
        */ 
		IMPORT_C virtual TInt ParameterL( const TInt aParameterID ) const;
	
		/**
        * Internalize to a data stream
        * @since 2.8
		* @param aStream Data stream
        * @return  None
        */ 
		IMPORT_C virtual void InternalizeL(RReadStream& aStream );

		/**
        * Externalize from a data stream
        * @since 2.8
		* @param None
        * @return  aStream Data stream
        */ 
		IMPORT_C virtual void ExternalizeL(RWriteStream& aStream ) const;

        /**
        * Returns all set parameters and their values.
        * @since 2.8
        * @param "RArray<TInt>& aParameterIDs" This array will contain parameter IDs after call.
        * @param "RArray<TInt>& aParameterValues" This array will contain parameter values after call.
        */
        IMPORT_C virtual void ListParametersL( RArray<TInt>& aParameterIDs, RArray<TInt>& aParameterValues ) const;

	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIParameters();
		
	private:
		RArray<TInt> iParameterIDs;
		RArray<TInt> iParameterValues;	
	};

/**
*  Class to encapsulate a Speaker Independent Rule variant
*
*  @lib nsssispeechRecognitionData.lib
*  @since 2.8
*/
class CSIRuleVariant : public CSIParameters
	{
	public: // Constructors and destructor
		/**
		* Destructor.
        */
		IMPORT_C virtual ~CSIRuleVariant();
		
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIRuleVariant* NewL( const TSIRuleVariantID aRuleVariantID, 
											  const TSILexiconID aLexiconID);
	    IMPORT_C static CSIRuleVariant* NewLC( const TSIRuleVariantID aRuleVariantID, 
											  const TSILexiconID aLexiconID);
     	 
	public: // New functions
		
		/**
        * Use to get the pronunciation Ids.
        * @since 2.8
		* @param None.
        * @return Pronunciation identifier.
        */ 
		IMPORT_C virtual void GetPronunciationIDsL( TSIPronunciationIDSequence& aPronunciationIDs ) const;

		/**
        * Use to set the pronunciation Ids.
        * @since 2.8
		* @param Pronunciation identifier .
        */ 
		IMPORT_C virtual void SetPronunciationIDsL( const TSIPronunciationIDSequence& aPronunciationIDs );

		/**
        * Get Language of the rule variant
        * @since 2.8
        * @return Language of the rule variant
        */ 
		IMPORT_C virtual TLanguage Language() const;  
		
		/**
        * Set Language of the rule variant
        * @since 2.8
        * @param Language of the rule variant
        * @return None
        */ 
		IMPORT_C virtual void SetLanguage(TLanguage aLanguage);
		
		
		/**
        * Use to retrieve the lexicon Id.
        * @since 2.8
		* @param None.
        * @return Lexicon identifier.
        */ 
		IMPORT_C virtual TSILexiconID LexiconID() const;
		
		/**
        * Use to retrieve the rule variant Id.
        * @since 2.8
        * @param None.
        * @return Rule identifier.
        */ 
		IMPORT_C virtual TSIRuleVariantID RuleVariantID() const;
		
		
		
		
	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIRuleVariant( const TSIRuleVariantID aRuleVariantID, 
			const TSILexiconID aLexiconID );
		
	private:
		TSIRuleVariantID iRuleVariantID;
		TSIPronunciationIDSequence iPronunciationIDs;
		TSILexiconID iLexiconID;
		TLanguage iLanguage;
		TInt* iParameterArray;
		TInt iNParameters;
		TAny* iReserved;
	};

/**
*  Class to encapsulate a Speaker Independent Rule
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIRule : public CBase
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
        */
		IMPORT_C virtual ~CSIRule();
		
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIRule* NewL( const TSIRuleID aRuleID );
	    IMPORT_C static CSIRule* NewLC( const TSIRuleID aRuleID );
       	
	public: // New functions
		/**
			* Use to retrieve the rule Id.
			* @since 2.8
			* @param None.
			* @return Rule identifier.
        */ 
		IMPORT_C virtual TSIRuleID RuleID() const;
	/**
		* Use to retrieve number of rule variants.
		* @since 2.8
		* @param None.
		* @return Number of rule variants.
       */
		IMPORT_C virtual TInt Count() const;
		
 		/**
		* Use to retrieve the rule variant based on index.
		* @since 2.8
		* @param aIndex Index of rule variant.
		* @return Rule variant.
        */
		IMPORT_C virtual CSIRuleVariant& AtL(TInt aIndex) const;	
		
	
			/**
		* Use to add new rule variant.
		* @since 2.8
		* @param aRuleVariant New rule variant.
		* @return None.
        */
		IMPORT_C virtual void AddL(CSIRuleVariant* aRuleVariant);
	
	/**
		* Use to retrieve the rule variant index based on Id.
		* @since 2.8
			* @param aRuleVariantID Identifier of rule variant.
		* @return Rule variant.
        */
		IMPORT_C virtual TInt Find(TSIRuleVariantID aRuleVariantID) const;
	

		IMPORT_C virtual CSIRuleVariant&  RuleVariantL(TSIRuleVariantID aRuleVariantID) const;
	
		/**
		* Use to delete rule variant.
		* @since 2.8
		* @param aRuleVariant Rule variant identifier.
			* @return None.
        */
		IMPORT_C virtual void DeleteL(TSIRuleVariantID aRuleVariantID);
	 	 
        /**
        * Compares two CSIRule objects.
        * Returns 0 if rule ids are equal.
        * Returns -1 if first id is smaller.
        * Returns 1 if second id is smaller.
        *
        * @param "const CSIRule& aFirst" First rule to compare
        * @param "const CSIRule& aSecond" Second rule to compare
        * @return 0, -1 or 1
        */
		static TInt Compare( const CSIRule& aFirst, const CSIRule& aSecond );

	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIRule( const TSIRuleID aRuleID );
		
	private:
		TSIRuleID iRuleID;
		RPointerArray<CSIRuleVariant> iRuleVariantArray;
		TAny* iReserved;
	};


/**
*  Class to encapsulate a Speaker Independent Grammar
*  A grammar consist of a list of rules
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIGrammar : public CBase
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
        */
		IMPORT_C virtual ~CSIGrammar();
		
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIGrammar* NewL(const TSIGrammarID aGrammarID);
		IMPORT_C static CSIGrammar* NewLC(const TSIGrammarID aGrammarID);
 		
	public: // New functions

		/**
        * Use to add a rule to the grammar.
        * @since 2.8
		* @param aRule - reference to CSIRule object.
        * @return KErrNone, if the insertion is successful, otherwise one of the system wide error codes.
        */
		IMPORT_C virtual void AddL(CSIRule* aRule);
		
		
		/**
        * Use to return a rule based at an index.
        * @since 2.8
		* @param aIndex: an index. Value must not be negative and must not be greater than the number of rules currently in the grammar, otherwise it raises a USER-130 panic.
        * @return Reference to CSIRule object.
        */
		IMPORT_C virtual CSIRule& AtL(TInt aIndex) const;

		/**
        * Use to delete a rule to the grammar.
        * @since 2.8
		* @param aRuleID - rule identifier.
        * @return None.
        */
		IMPORT_C virtual void DeleteL(const TSIRuleID aRuleID);
 		
		/**
        * Use to get the number of rules in the grammar
        * @since 2.8
		* @param None.
        * @return Number of rules.
        */  
		IMPORT_C virtual TInt Count() const;
		
		/**
			* Use to retrieve the grammar Id of the result.
			* @since 2.8
			* @param None.
			* @return Grammar identifier.
        */
		IMPORT_C virtual TSIGrammarID GrammarID() const;

		/**
        * Use to find a rule in the grammar based on rule Id.
        * @since 2.8
		* @param aRuleID - rule identifier.
        * @return The index of the first matching rule within the grammar.  KErrNotFound, if no matching rule can be found.
        */ 
		
		IMPORT_C virtual TInt Find(const TSIRuleID aRuleID) const;

		IMPORT_C virtual CSIRule&  RuleL(TSIRuleID aRuleID) const;

		/**
        * Internalize to a data stream
        * @since 2.8
		* @param aStream Data stream
        * @return  None
        */ 
		IMPORT_C virtual void InternalizeL(RReadStream& aStream );

		/**
        * Externalize from a data stream
        * @since 2.8
		* @param None
        * @return  aStream Data stream
        */ 
		IMPORT_C virtual void ExternalizeL(RWriteStream& aStream ) const;

 	protected:
		
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIGrammar(const TSIGrammarID aGrammarID);
		
	private:
		TSIGrammarID iGrammarID;
		RPointerArray<CSIRule> iRuleArray;
		TAny* iReserved;
	};

/**
*  Class to encapsulate a compiled grammar (ready for recognition)
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/ 
class CSICompiledGrammar : public CSIGrammar
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
        */
		IMPORT_C virtual ~CSICompiledGrammar();
		
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSICompiledGrammar* NewL(const TSIGrammarID aGrammarID); 
		
		IMPORT_C static CSICompiledGrammar* NewLC(const TSIGrammarID aGrammarID); 
	public: // New functions
 		

				/**
        * Use to delete a rule to the grammar.
        * @since 2.8
		* @param aRuleID - rule identifier.
        * @return None.
        */
		IMPORT_C virtual void DeleteL(const TSIRuleID aRuleID);
		/**
        * Use to add a rule to the grammar.
        * @since 2.8
		* @param aRule - reference to CSIRule object.
        * @return KErrNone, if the insertion is successful, otherwise one of the system wide error codes.
        */
		IMPORT_C virtual void AddL(CSIRule* aRule);
 		

		/**
        * Use to retrieve the recognition data.
        * @since 2.8
		* @param None.
        * @return Reference to the recognition data.
        */ 
 		IMPORT_C virtual void SetCompiledData( HBufC8* aCompilerData);
		
		/**
        * Function to get compiler data
        * @since 2.8
        * @param None
        * @return Pointer to data
        */	
		IMPORT_C virtual TDesC8&  CompiledData() const;
		
		/**
        * Internalize to a data stream
        * @since 2.8
		* @param aStream Data stream
        * @return  None
        */ 
		IMPORT_C virtual void InternalizeL(RReadStream& aStream );

		/**
        * Externalize from a data stream
        * @since 2.8
		* @param None
        * @return  aStream Data stream
        */ 
		IMPORT_C virtual void ExternalizeL(RWriteStream& aStream ) const;

	protected:
		IMPORT_C virtual void ConstructL();
		
		IMPORT_C CSICompiledGrammar(const TSIGrammarID aGrammarID);
		
	private:
		// is compiled data valid
		TBool iValidData;
		 
		// data of grammar compiler
		HBufC8*  iGrammarCompilerData;
		TAny* iReserved;
		
	protected:
		
	};

/**
*  Class to define rule variant info
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class TSIRuleVariantInfo
	{
	public:  // Constructors and destructor
		
        /**
        * C++ default constructor.
        */
        IMPORT_C TSIRuleVariantInfo( const TSIGrammarID aGrammarID, 
                                     const TSIRuleID aRuleID, 
                                     const TSIRuleVariantID aRuleVariantID );
		
        /**
        * Destructor.
        */
        IMPORT_C virtual ~TSIRuleVariantInfo();
		
	public: // New functions
		
			/**
			* Use to get Grammar ID
			* @since 2.8
			* @return Identifier of Grammar
		*/
		IMPORT_C virtual TSIGrammarID GrammarID() const;
		

			/**
			* Use to get rule ID
			* @since 2.8
			* @return Identifier of rule
		*/
		IMPORT_C virtual TSIRuleID RuleID() const;
		
		/**
		* Use to get rule variant ID
		* @since 2.8
		* @return Identifier of rule variant
		*/
		IMPORT_C virtual TSIRuleVariantID RuleVariantID() const;
		
	protected:    // Data
		
		
		// rule identifier
		const TSIGrammarID iGrammarID;
		// rule identifier
		const TSIRuleID iRuleID;
		
		// rule variant identifier
		const TSIRuleVariantID iRuleVariantID;
		
	};
	
/**
*  Class to encapsulate a Speaker Independent Pronunciation
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIPronunciation : public CSIParameters
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
		*/
		IMPORT_C virtual ~CSIPronunciation();
		
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CSIPronunciation* NewL(const TSIPronunciationID aPronunciationID, 
											   const TSIModelBankID aModelBankID );
		IMPORT_C static CSIPronunciation* NewLC(const TSIPronunciationID aPronunciationID, 
												const TSIModelBankID aModelBankID );
		
	public: // New functions
		/**
		* Use to retrieve the model bank Id of the model bank.
		* @since 2.8
		* @param None.
		* @return Model bank identifier.
		*/ 
		IMPORT_C virtual TSIModelBankID ModelBankID() const;
 			/**
			* Use to set the pronunciation Id of this lexicon entry.
			* @since 2.8
			* @param Pronunciation identifier.
			* @return None.
		*/ 
		 IMPORT_C virtual void SetPronunciationID(TSIPronunciationID aPronunciationID); 
		
		
			/**
			* Use to retrieve the pronunciation Id of this lexicon entry.
			* @since 2.8
			* @param None.
			* @return Pronunciation identifier.
		*/ 
		IMPORT_C virtual TSIPronunciationID PronunciationID() const;
		/**
		* Use to set phoneme sequence.
		* @since 2.8
		* @param aPhonemeSequence Phoneme sequence.
		* @return None.
		*/  
		IMPORT_C virtual void SetPhonemeSequenceL( const TDesC8& aPhonemeSequence );
		

		/**
		* Use to retrieve the phoneme sequence.
		* @since 2.8
		* @param None.
		* @return Phoneme sequence.
		*/  
		IMPORT_C virtual const TDesC8& PhonemeSequence() const;

        /**
        * Compares two CSIPronunciation objects.
        * Returns 0 if prununciation ids are equal.
        * Returns -1 if first id is smaller.
        * Returns 1 if second id is smaller.
        *
        * @param "const CSIPronunciation& aFirst" First pronunciation to compare
        * @param "const CSIPronunciation& aSecond" Second pronunciation to compare
        * @return 0, -1 or 1
        */
		static TInt Compare( const CSIPronunciation& aFirst, const CSIPronunciation& aSecond );

        /**
        * Compares two CSIPronunciation objects.
        * Returns 0 if phoneme sequences are equal.
        * Returns -1 if first phoneme sequence is smaller.
        * Returns 1 if second phoneme sequence is smaller.
        *
        * @param "const CSIPronunciation& aFirst" First pronunciation to compare
        * @param "const CSIPronunciation& aSecond" Second pronunciation to compare
        * @return 0, -1 or 1
        */
		static TInt ComparePhonemes( const CSIPronunciation& aFirst, const CSIPronunciation& aSecond );


	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIPronunciation(const TSIPronunciationID aPronunciationID, 
								  const TSIModelBankID aModelBankID );
		
	private:
		TSIPronunciationID iPronunciationID;
		TSIModelBankID iModelBankID;
		HBufC8* iPhonemeSequence;
		TAny* iReserved;
	};


/**
*  Class to encapsulate a Speaker Independent Lexicon
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSILexicon : public CBase
	{
	public:// Constructors and destructor
	
        /**
        * Destructor.
		*/
		IMPORT_C virtual ~CSILexicon();
		
		/**
		* Two-phased constructor.
		* @param aLexiconID Lexicon ID
		* @return Created instance
		*/
		IMPORT_C static CSILexicon* NewL( const TSILexiconID aLexiconID );
		IMPORT_C static CSILexicon* NewLC( const TSILexiconID aLexiconID );
		
	public: // New functions
		
		/**
		* Use to add a lexicon entry to the lexicon.
		* Leaves with KErrNoMemory if maximal allowed amount of pronunciations
		* is already stored in this lexicon
		* @since 2.8
		* @param aPronunciation pointer to CSIPronunciation object.
		*/
    	IMPORT_C virtual void AddL( CSIPronunciation* aPronunciation );
		
		/**
		* Use to return a pronunciation based at an index.
		* @since 2.8
		* @param aIndex Value must not be negative and must not be greater than
		*               the number of pronunciations currently in the lexicon.
		* @return Reference to CSIPronunciation object.
		*/
		IMPORT_C virtual CSIPronunciation& AtL( TInt aIndex ) const;

        /**
		* Deletes a pronunciation based on identifier.
		* @since 2.8
		* @param aPronunciationID pronunciation variant identifier.
        */
		IMPORT_C virtual void DeleteL( TSIPronunciationID aPronunciationID );
		 
		/**
		* Use to find a pronunciation based on pronunciation Id.
		* @since 2.8
		* @param aPronunciationID: pronunciation identifier.
		* @return The index of the first matching pronunciation within the lexicon.
		*         KErrNotFound, if no matching pronunciation can be found.
		*/ 
		IMPORT_C virtual TInt Find( const TSIPronunciationID aPronunciationID ) const;
		
		/**
		* Use to find a pronunciation based on phoneme sequence.
		* @since 2.8
		* @param aPronunciation: phoneme sequence.
		* @return The index of the first matching pronunciation within the lexicon.
		*         KErrNotFound, if no matching pronunciation can be found.
		*/ 
		IMPORT_C virtual TInt Find( const TDesC8& aPhonemeSequence ) const;
		
		/**
		* Use to count the number of lexicon entries in the lexicon.
		* @since 2.8
		* @return Number of pronunciations.
		*/
		IMPORT_C virtual TInt Count() const;

		/**
		* Use to retrieve the model bank Id of the model bank.
		* @since 2.8
		* @return Model bank identifier.
		*/  
		IMPORT_C virtual TSILexiconID LexiconID() const;
 		
		/**
        * Internalize to a data stream
        * @since 2.8
		* @param aStream Data stream
        */ 
		IMPORT_C virtual void InternalizeL( RReadStream& aStream );

		/**
        * Externalize from a data stream
        * @since 2.8
		* @param aStream Data stream
        */ 
		IMPORT_C virtual void ExternalizeL( RWriteStream& aStream ) const;

	protected:
		IMPORT_C void ConstructL();
		IMPORT_C CSILexicon( const TSILexiconID aLexiconID );
		
    private:
    
        /**
        * Utility function to add pronunciation to the end of ID-ordered array
        *
        * @param "CSIPronunciation* aPronunciation" Pronunciation which should be added.
        */
        void AddPronunciationToEndL( CSIPronunciation* aPronunciation );
		
	private:
        // ID of the lexicon
        TSILexiconID iLexiconID;
        // Pronunciations in the order of IDs
		RPointerArray<CSIPronunciation> iPronunciationArray;
		// Pronunciations in the order of the phoneme sequence
		RPointerArray<CSIPronunciation> iPronunOrder;
		// For future extensions
		TAny* iReserved;
	};


/**
*  Class to encapsulate a Speaker Independent Model
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIModel : public CSIParameters
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
		*/
		IMPORT_C virtual ~CSIModel();
		
		/**
		* Two-phased constructors.
		*/
		//IMPORT_C static CSIModel* NewL();
		//IMPORT_C static CSIModel* NewLC();
		IMPORT_C static CSIModel* NewL( const TSIModelID aModelID );
		IMPORT_C static CSIModel* NewLC( const TSIModelID aModelID );
		
	public: // New functions
			/**
			* Use to set the phoneme Id of this model.
			* @since 2.8
			* @param aModelID: phoneme identifier.
			* @return None.
		*/ 
		IMPORT_C virtual void SetModelID( const TSIModelID aModelID);
		
		/**
		* Use to retreive the phoneme Id of this model.
		* @since 2.8
		* @param None.
		* @return Phoneme identifier.
		*/ 
		IMPORT_C virtual TSIModelID ModelID() const;
		
		/**
		* Use to get a pointer to the acoustic model of this model.
		* @since 2.8
		* @param None.
		* @return A descriptor to the acoustic model data.
		*/
		IMPORT_C virtual   TDesC8& AcousticModel() const;
	
		/**
		* Use to set the acoustic model of the model.
		* @since 2.8
		* @param aAcousticModel: pointer descriptor containing the acoustic model (binary).
		* @return None.
		*/ 
		IMPORT_C virtual void SetAcousticModel(  HBufC8* aAcousticModel );
		
		
	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIModel( );
		IMPORT_C CSIModel( const TSIModelID aModelID );
		
	private:
		TSIModelID iModelID;
		HBufC8* iAcousticModel;
		TAny* iReserved;
	};

/**
*  Class to encapsulate a Speaker Independent Model Bank
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIModelBank : public CBase
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
		*/
		IMPORT_C virtual ~CSIModelBank();
		
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CSIModelBank* NewL(const TSIModelBankID aModelBankID);
		IMPORT_C static CSIModelBank* NewLC(const TSIModelBankID aModelBankID);
		
	public: // New functions	
		 


		/**
		* Use to add a model to the model bank.
		* @since 2.8
		* @param aModel: reference to CSIModel object.
		* @return None
		*/ 
		IMPORT_C virtual void AddL(const CSIModel* aModel);
		
		/**
		* Use to return a model at the given index.
		* @since 2.8
		* @param Reference to CSIModel object.
		* @return None
		 */ 
		IMPORT_C virtual CSIModel& AtL(TInt aIndex) const;
		
		/**
		* Deletes  a model from model bank.
		* @since 2.8
		* @param ID of the model 
		* @return None 
		*/ 
		IMPORT_C virtual void DeleteL(TSIModelID aModelID); 

		/**
		* Use to find a model based on phoneme Id.
		* @since 2.8
		* @param None.
		* @return Index of the first matching model within the model bank.  KErrNotFound, if no matching model can be found..
		*/ 
		IMPORT_C virtual TInt Find(TSIModelID aModelID) const;
		
		/**
		* Use to retreive the number of models in the model bank.
		* @since 2.8
		* @param None.
		* @return Number of models in the model bank.
		*/
		IMPORT_C virtual TInt Count() const;
		
			/**
			* Use to return the model bank Id of the model bank.
			* @since 2.8
			* @param None.
			* @return Model bank identifier.
		*/
		IMPORT_C virtual TSIModelBankID ModelBankID() const;
		
	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIModelBank(const TSIModelBankID aModelBankID);
		
	private:
		TSIModelBankID iModelBankID;
		RPointerArray<CSIModel> iModelArray;
		TAny* iReserved;
	};

/**
*  Class to define pronunciation type.
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIPronunciationInfo : public CSIParameters
	{
	public:  // Constructors and destructor

		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CSIPronunciationInfo* NewL(RPointerArray<HBufC8>  aPronunciationArray,
			TLanguage aLanguage );
		IMPORT_C static CSIPronunciationInfo* NewLC(RPointerArray<HBufC8>  aPronunciationArray,
			TLanguage aLanguage );
		
		/**
		* Destructor.
		*/
		IMPORT_C virtual ~CSIPronunciationInfo();
		
	public: // New functions
		
			/**
			* Use to get phoneme sequence data
			* @since 2.8
			* @return Phoneme sequence
		*/
		IMPORT_C virtual TDesC8& PronunciationL(TInt aIndex) const;

			/**
			* Use to set phoneme sequence data
			* @since 2.8
			* @return Phoneme sequence
		*/
		
		IMPORT_C virtual void SetPronunciationL( TInt aIndex, HBufC8* aPronunciation);

		IMPORT_C virtual TLanguage Language() const;

		IMPORT_C virtual TInt Count() const;
		

	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIPronunciationInfo( RPointerArray<HBufC8> aPronunciationArray,TLanguage aLanguage);	

	private:    // Data
		
		// phoneme sequence
		//HBufC8* iPronunciation;
		RPointerArray<HBufC8> iPronunciationArray;
			
		TLanguage iLanguage;
		TAny* iReserved;
	};

/**
*  Class to define word list. The list is used as input parameter for TTP.
*
*  @lib SiSpeechRecognitionData.lib
*  @since 2.8
*/
class CSITtpWordList : public CBase
	{
	public:  // Constructors and destructor
		
			 /**
			 * Two-phased constructor.
		*/
		IMPORT_C static CSITtpWordList* NewL();
		IMPORT_C static CSITtpWordList* NewLC();
		
		/**
		* Destructor.
		*/
		IMPORT_C virtual ~CSITtpWordList();
		
	public: // New functions
		
			/**
			* Use to add a new word
			* @since 2.8
			* @param aWord Word to be added
		*/
		IMPORT_C virtual void AddL(  MDesCArray* aWords );
		
		/**
		* Use to get number of words
		* @since 2.8
		* @return number of words
		*/
		IMPORT_C virtual TInt Count() const;
		
		/**
		* Use to return word at an index
		* @since 2.8
		* @param aIndex Index of word
		* @return reference to word sequence
		*/
		IMPORT_C virtual  MDesCArray& AtL( const TInt aIndex ) const;
		
		
		/**
		* Use to delete word at an index
		* @since 2.8
		* @param aIndex Index of word 		
		* @return None
		*/
		IMPORT_C virtual void DeleteL(TInt aIndex);
		
 

		/**
		* Use to set pronunciation for a word
		* @since 2.8
		* @param aIndex Index of word seqence
		* @param aPronunciation Pronunciation to be added
		*/
		IMPORT_C virtual void AppendPronunciationL(TInt aIndex, 
			CSIPronunciationInfo* aPronunciation);

		
			/**
			* Use to get pronunciations for a word
			* @since 2.8
			* @param aIndex Index of word sequence
			* @param aPronunciations Array where to put pronunciations
		*/
		IMPORT_C virtual void GetPronunciationsL( const TInt aIndex, 
			RPointerArray<CSIPronunciationInfo>& aPronunciations ) const; 

	protected:
		
	/**
	* C++ default constructor.
		*/
		IMPORT_C CSITtpWordList();
		
		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		IMPORT_C void ConstructL();
		
	private:    // Data
		
		// word array
	//	RArray<TPtrC> iWordArray;
 		RPointerArray<MDesCArray> iWordArray;
		// index array for pronunciations
		RArray<TInt> iIndexArray; 
		// pronunciation array
		RPointerArray<CSIPronunciationInfo> iPronunciationArray;
		
		// Reserved pointer for future extension
		TAny* iReserved;
    };

/**
*  Class to encapsulate a Recognition Result
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIResult : public CSIParameters
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
		*/
		IMPORT_C virtual ~CSIResult();
		
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CSIResult* NewL();
		IMPORT_C static CSIResult* NewLC();
		
	public: // New functions	
			/**
			* Use to set the grammar Id on the result.
			* @since 2.8
			* @param aGrammarID - grammar identifier.
			* @return None.
		*/
		IMPORT_C virtual void SetGrammarID(const TSIGrammarID aGrammarID);
		
		/**
		* Use to retrieve the grammar Id of the result.
		* @since 2.8
		* @param None.
		* @return Grammar identifier.
		*/
		IMPORT_C virtual TSIGrammarID GrammarID() const;
		
		/**
		* Use to set the rule Id of the result.
		* @since 2.8
		* @param aRuleID - rule identifier.
		* @return None.
		*/
		IMPORT_C virtual void SetRuleID(const TSIRuleID aRuleID);
		
		/**
		* Use to retrieve the rule Id of the result.
		* @since 2.8
		* @param None.
		* @return Rule identifier.
		*/ 
		IMPORT_C virtual TSIRuleID RuleID() const;
		
		/**
		* Use to set the rule variant Id of the result.
		* @since 2.8
		* @param aRuleID - rule variant identifier.
		* @return None.
		*/
		IMPORT_C virtual void SetRuleVariantID(const TSIRuleVariantID aRuleVariantID);
		
		/**
		* Use to retrieve the rule variant Id of the result.
		* @since 2.8
		* @param None.
		* @return Rule variant identifier.
		*/ 
		IMPORT_C virtual TSIRuleVariantID RuleVariantID() const;
		
		/**
		* Use to set the score for this result.
		* @since 2.8
		* @param aScore: score of this result.
		* @return None.
		*/ 
		IMPORT_C virtual void SetScore(const TInt32 aScore);
		
		/**
		* Use to retreive the score of this result.
		* @since 2.8
		* @param None.
		* @return Score of this result.
		*/ 
		IMPORT_C virtual TInt32 Score() const;
		
		/**
		* Sets the pronunciation information needed for speaker adaptation and playback.  
		* @since 2.8
		* @param aSIPronunciationInfo  Pronunciation information,  needed for speaker adaptation and playback  
		* @return  None.
		*/ 
		IMPORT_C virtual void SetPronunciation(  CSIPronunciation* aSIPronunciation);

		/**
		* Gets the pronunciation information needed for speaker adaptation and playback.  
		* @since 2.8
		* @param None
		* @return aSIPronunciationInfo  Pronunciation information,  needed for speaker adaptation and playback  
		*/ 
 		IMPORT_C virtual CSIPronunciation& Pronunciation() const ;
	protected:
		IMPORT_C virtual void ConstructL();
		IMPORT_C CSIResult();
		
	private:
		TSIGrammarID iGrammarID;
		TSIRuleID iRuleID;
		TSIRuleVariantID iRuleVariantID;
		TInt32 iScore;
		CSIPronunciation* iSIPronunciation;
		TAny* iReserved;
	};


/**
*  Class to encapsulate a a set of Recognition Result
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.8
*/
class CSIResultSet : public CBase
	{
	public:// Constructors and destructor
		   /**
		   * Destructor.
		*/        
		IMPORT_C virtual ~CSIResultSet();
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CSIResultSet* NewL();
		IMPORT_C static CSIResultSet* NewLC();
		
	public: // New functions		
			/**
			* Use to add a result to the rule set.
			* @since 2.8
			* @param aResult - reference to the result object to be added.
			* @return KErrNone, if the insertion is successful, otherwise one of the system wide error codes.
		*/
		IMPORT_C virtual void AddL( const CSIResult* aResult); 

		/**
		* Use to return a result at a given index within the result set.
		* @since 2.8
		* @param aIndex - index in the result set.
		* @return  A reference to CSIResult object.
		*/
		IMPORT_C virtual  CSIResult& AtL(const TInt aIndex);

        /**
		* Use to return a result at a given index within the result set.
		* @since 2.8
		* @param aIndex - index in the result set.
		* @return  A constant reference to CSIResult object.
		*/
		IMPORT_C virtual const CSIResult& AtL(const TInt aIndex) const;
		
		/**
		* Use to retreive the number of result in the result set.
		* @since 2.8
		* @param None.
		* @return Number of results in the result set.
		*/ 
		IMPORT_C virtual TInt Count() const;
	
		
		/**
		* Delete a result in the result set.
		* @since 2.8
		* @param  index of a result
		* @return None
		*/ 
		IMPORT_C virtual void DeleteL(TInt aIndex);
		
		/**
		* Sets the data needed for speaker adaptation. CSIResultSet takes ownership of the data.		
		* @since 2.8
		* @param   Adaptation data
		* @return None
		*/  
		IMPORT_C virtual void SetAdaptationData( HBufC8* aAdaptationData );


		/**
		* Gets the data needed for speaker adaptation. CSIResultSet takes ownership of the data.		
		* @since 2.8
		* @param None  		
		* @return   Adaptation data
		*/  
		IMPORT_C virtual TDesC8& AdaptationData();

        /**
        * Stores the object to the stream.
        * @since 2.8
        * @param aWriteStream The stream to write to.
        * @return None
        */
        IMPORT_C virtual void ExternalizeL( RWriteStream& aWriteStream ) const;

        /**
        * Resurrects an object from the stream.
        * @since 2.8
        * @param aReadStream The stream to read from.
        * @return None
        */
        IMPORT_C virtual void InternalizeL( RReadStream& aReadStream );

	private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
		IMPORT_C virtual void ConstructL();

        /**
        * C++ constructor
        */
		IMPORT_C CSIResultSet();
		
	private:
        // The results
		RPointerArray<CSIResult> iResultArray;

        // Speaker adaptation data
		HBufC8* iAdaptationData;

        // Reserved for future extensions
		TAny* iReserved;
	};

#endif

// End of file
