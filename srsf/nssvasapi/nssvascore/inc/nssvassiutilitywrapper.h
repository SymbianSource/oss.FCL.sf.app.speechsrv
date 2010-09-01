/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/


#ifndef NSSVASSIUTILITYWRAPPER_H
#define NSSVASSIUTILITYWRAPPER_H

// INCLUDES
#include <e32base.h>
#include <nsssispeechrecognitiondataclient.h>
#include <nsssispeechrecognitionutilityobserver.h>
#include <nsssiutilitybase.h>

// CLASS DECLARATION

/**
* CNssSiUtilityWrapper
* @since 3.1
* @see nsssispeechrecognitionutility.h
* @see nsssispeechrecognitionutilityobserver.h
* @see nsssiutilitybase.h
*/
NONSHARABLE_CLASS ( CNssSiUtilityWrapper ) : public CActive, 
                                             public MSISpeechRecognitionUtilityObserver
    {
    public: // Constructors & destructor
     
        /**
        * Two-phased constructor
        * 
        * @since 3.1
        */
        static CNssSiUtilityWrapper* NewL( MSISpeechRecognitionUtilityObserver& aObserver,
                                           TUid aClientUid );

        /**
        * Destructor.
        */
	    virtual ~CNssSiUtilityWrapper();

    public: // New functions
    
        /**
        *
        *    Adapts the speaker independent models to a specific speaker's voice.
        *
        * @since  2.8
        * @param  aResultSet     Recognition result N-Best
        * @param  aCorrect       Index to N-Best pointing the correct result.
        * @return system wide error code
        */
        TInt Adapt( const CSIClientResultSet& aResultSet,
                             TInt aCorrect );    
    
        /**
        * Adds a new rules for the given phrases into the 
        * specified grammar.
        *
        * @param    aTextArrayArray     Array of text arrays. The phrase k is the concatenation
        *                               of texts in array k.
        * @param    aLanguageArray      pronunciations will be generated for these languages
        * @param    aLexiconID          lexicon, where to add pronunciations
        * @param    aGrammarID          grammar, where to add rules
        * @param    aRuleID             the ID of the rule will be placed on this variable.
        *                               If some of the names fail while others succeed,
        *                               KInvalidRuleID will be placed to the array.
        * @return    system wide error code
        */
        TInt AddVoiceTags( const RPointerArray<MDesCArray> &aTextArrayArray,
                                    const RArray<TLanguage> &aLanguageArray,
                                    TSILexiconID aLexiconID,
                                    TSIGrammarID aGrammarID, 
                                    RArray<TSIRuleID>& aRuleIDArray );

        /**
        * Adds a new rules for the given phrases into the 
        * specified grammar.
        *
        * @param aTextArrayArray     Array of text arrays. The phrase k is the concatenation
        *                            of texts in array k.
        * @param aLanguageArray      pronunciations will be generated for these languages
        * @param aLexiconID          lexicon, where to add pronunciations
        * @param aGrammarID          grammar, where to add rules
        * @param aRuleID             the ID of the rule will be placed on this variable.
        *                            If some of the names fail while others succeed,
        *                            KInvalidRuleID will be placed to the array.
        * @return System wide error code
        */
        TInt AddVoiceTags( const RPointerArray<MDesCArray>& aTextArrayArray,
                                    const RArray<RLanguageArray>& aLanguageArray,
                                    TSILexiconID aLexiconID,
                                    TSIGrammarID aGrammarID, 
                                    RArray<TSIRuleID>& aRuleIDArray );  

        /**
        * Cancels the current operation
        */
        void CancelUtility();

        /**
        * Saves the changes into a permanent storage.
        *
        * @return    System-wide errors
        */
        TInt CommitChanges();
    
        /**
        * Creates a new grammar.
        */
        TInt CreateGrammar( TSIGrammarID& aGrammarID );

        /**
        * Creates a new lexicon.
        */
        TInt CreateLexicon( TSILexiconID& aLexiconID );
        
        /**
        *    Creates a new model bank.
        * @param    aModelBankID        reference where a new model 
        *                                bank Id is assigned to
        * @return    system wide error codes
        */
        TInt CreateModelBank( TSIModelBankID& aModelBankID );

        /**
        *    Ends the current recognition session. Resources allocated 
        *    for recognition are freed.
        * @param    aModelBankID        reference where a new model bank Id is 
        *                                assigned to
        * @return    system wide error codes
        */
        TInt EndRecSession();

        /**
        * 
        *    Loads the specified grammar into the recognizer;
        *    done prior to recognition.
        * @param     aGrammarID            grammar ID
        * @return    system wide error codes
        */
        TInt LoadGrammar( TSIGrammarID aGrammarID );
        
        /**
        * 
        *    Loads the specified model bank into the recognizer;
        *    done prior to recognition..
        * @param     ModelBankID            model bank Id
        * @return    system wide error codes
        */
        TInt LoadModels( TSIModelBankID aModelBankID );


        /**
        *    Records uder utterance for training and recognition.
        * @param    aRecordTime            recording time in microseconds
        * @return    system wide error codes
        */
        TInt Record( TTimeIntervalMicroSeconds32 aRecordTime );

        /**
        * Pre-starts samping before Record call.
        * 
        * @since 3.2
        * @return System wide error code
        */
        TInt PreStartSampling();

        /**
        *    Initiates speaker independent recognition; performed
        *   following loading of model bank, lexicon, and grammar.
        *
        * @param    aResultSet            reference where the recognition
        *                                result is set
        * @return    system wide error codes
        */
        TInt Recognize( CSIClientResultSet& aResultSet );

        /**
        *    Removes the specified grammar from the permanent storage.
        *    Removing a grammar will remove all rules within the grammar.
        *
        * @param    aGrammarID            grammar Id
        * @return    system wide error codes
        */
        TInt RemoveGrammar( TSIGrammarID aGrammarID );

        /**
        *    Removes the specified lexicon from the permanent storage.
        *    Removing a lexicon will remove all pronunciations within the 
        *    lexicon.
        * 
        * @param    aLexiconID            lexicon Id
        * @return    system wide error codes
        */
        TInt RemoveLexicon( TSILexiconID aLexiconID );


        /**
        *    Removes the specified model bank from the permanent storage.
        *    Removing a model bank will remove all models within the model bank.
        * 
        * @param    aModelBankID        model bank Id
        * @return    system wide error codes
        */
        TInt RemoveModelBank( TSIModelBankID aModelBankID );

        /**
        *    Removes the specified rule from the specified grammar permanently.
        * 
        * @param    aGrammarID            grammar Id
        * @param    aRuleID                rule Id
        * @return    system wide error codes
        */
        TInt RemoveRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID );

        /**
        *    Removes multiple rules from the specified grammar permanently.
        * 
        * @param    aGrammarID            grammar Id
        * @param    aRuleID                rule Id
        * @return    system wide error codes
        */
        TInt RemoveRules( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs );

        /**
        *    Set the audio priority and preference for train, playback 
        *    and recognition
        * 
        * @param    aPriority                priority for voice recognition
        * @param    aTrainPreference        preference for training
        * @param    aPlaybackPreference        preference for playback
        * @param    aRecognitionPreference    preference for recognition
        * @return    system wide error codes
        */
        TInt SetAudioPriority( TInt aPriority, TInt aTrainPreference, 
                        TInt aPlaybackPreference, TInt aRecognitionPreference );
        
        /**
        *    Set the event handler for asynchronous events
        * 
        * @param    aSpeechRecognitionUtilityObserver        pointer to observer
        */
        void SetEventHandler( MSISpeechRecognitionUtilityObserver*
                        aSpeechRecognitionUtilityObserver );


        /**
        *    Starts a new recognition session.
        * 
        * @param    aMode                recognition mode
        * @return    system wide error codes
        */
        TInt StartRecSession( TNSSRecognitionMode aMode );


        /**
        *    Unloads the specified rule from the specified grammar
        *    in temporary memory, previously loaded with LoadGrammarL.  
        *    The rule in the permanent storage remains intact.
        * 
        * @param    aGrammarID            grammar Id
        * @param    aRuleID               rule Id
        * @return   system wide error codes
        */
        TInt UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID );


    private: // Functions

	    /**
        * C++ constructor.
        */
	    CNssSiUtilityWrapper( MSISpeechRecognitionUtilityObserver& iObserver, TUid aClientUid );

	    /**
        * EPOC constructor.
        */
	    void ConstructL();
	    
	    /**
	    * From CActive
	    */
	    void RunL();
	    
	    /**
	    * From CActive
	    */
	    void DoCancel();
	    
	    /**
	    * Sets the active object ready to be run
	    */
	    void Ready();
	    
	    /**
	    * From MSISpeechRecognitionUtilityObserver
	    */
	    void MsruoEvent( TUid aEvent, TInt aResult );
	    
	private: // Member variables
	
	    // Handle to library loader
	    RLibrary iLib;
	    
	    // UID of the client
	    TUid iClientUid;
	    
	    // Utility API
	    MSISpeechRecognitionUtilityBase* iUtility;
	    
	    // Reference to callback handler
	    MSISpeechRecognitionUtilityObserver& iObserver;
	    
	    // Stored callback parameters
	    TUid iEvent;
	    TInt iResult;
    };

#endif // NSSVASSIUTILITYWRAPPER_H

// End of file
