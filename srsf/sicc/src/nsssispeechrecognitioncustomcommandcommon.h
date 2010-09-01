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
* Description:  This is the common header file for the speaker independent
*			    controller. It is used by both the client & server side.
*
*/


#ifndef NSSSISPEECHRECOGNITIONCUSTOMCOMMANDCOMMON_H
#define NSSSISPEECHRECOGNITIONCUSTOMCOMMANDCOMMON_H

// INCLUDE FILES
#include <nsssispeechrecognitiondataclient.h>
#include <nsssispeechrecognitiondatacommon.h>

// CONSTANTS
// (8CA6 = UID for dependent recognition)
const TUid KUidInterfaceSpeakerIndependent = {0x101F8CA7}; 

const TInt KExpandSize = 5;

// Initial assumption for buffer size. If the text is larger,
// the buffer is expanded.
const TInt KDefaultPhraseLen = 50;
// Hard upper limit on number of languages.
const TInt KMaxLanguages = 10;
// Hard upper limit on number of "segments" in the voice tag text
const TInt KMaxPartitionsInText = 5;

// FORWARD DECLARATIONS
class TSrsRecognize;
class TSrsAddPronunciation;
class TSrsAddRule;
class TSrsRuleValidity;
class TSrsGetUtteranceDuration;
class TSrsGetUtteranceDuration;
class TSrsPlayUtterance;
class TSrsAddVoiceTag;
class TSrsAddSindeVoiceTag;
class TSrsAddRuleVariant;
class TSrsAddPronunciationSI;

// DATA TYPES

// These are the custom commands messages used to communicate with
// the server.
enum TSrCustomCommandMessages
{
        ESrActivateGrammar,
		ESrAddPronunciation,
		ESrAddRule,
		ESrCancel,
		ESrCommitTrain,
		ESrCreateGrammar,
		ESrCreateLexicon,
		ESrCreateModelBank,
        ESrDeactivateGrammar,
		ESrEndRecSession,
		ESrGetAllModelIDs,
		ESrGetAllPronunciationIDs,
		ESrGetAllRuleIDs,
		ESrGetAvailableStorage,
		ESrGetEngineProperties,
		ESrGetModelCount,
		ESrGetRuleValidity,
		ESrUtteranceDuration,
		ESrLoadGrammar,
		ESrLoadLexicon,
		ESrLoadModels,
		ESrPlayUtterance,
		ESrRecognize,
		ESrRecord,
		ESrRemoveGrammar,
		ESrRemoveLexicon,
		ESrRemoveModelBank,
		ESrRemoveModel,
		ESrRemovePronunciation,
		ESrRemoveRule,
		ESrStartRecSession,
		ESrTrain,
		ESrUnloadRule,
		ESrLoadEngineParameters,
		ESrGetPronunciationIDArraySize,
		ESrGetPronunciationIDArrayContents,
		ESrGetRuleIDArraySize,
		ESrGetRuleIDArrayContents,
		ESrGetModelIDArraySize,
		ESrGetModelIDArrayContents,
		ESrGetClientResultSetSize,
		ESrGetClientResultSet,
		ESrSetClientUid,
		ESrCommitChanges,
		ESrGetAllClientGrammarIDs,
		ESrGetAllClientLexiconIDs,
		ESrGetAllClientModelBankIDs,
		ESrGetAllGrammarIDs,
		ESrGetAllLexiconIDs,
		ESrGetAllModelBankIDs,
		ESrGetGrammarIDArraySize,
		ESrGetGrammarIDArrayContents,
		ESrGetLexiconIDArraySize,
		ESrGetLexiconIDArrayContents,
		ESrGetModelBankIDArraySize,
		ESrGetModelBankIDArrayContents
// SI extensions
        ,
        ESrAdapt,
        ESrAddPronunciationSI,
        ESrAddRuleVariant,
        ESrAddVoiceTag,
        ESrAddVoiceTags,
        ESrCreateRule,
        ESrEndRecord,
        ESrRecognizeSI,
        ESrUnloadGrammar,
        ESrGetClientResultSetSizeSI,
		ESrRemoveRules,
        ESrAddSindeVoiceTag,
        ESrAddSindeVoiceTags,
        ESrPreStartSampling
};

typedef TPckgBuf<TSrsRecognize> TSrsRecognizePckg;
typedef TPckgBuf<TSrsAddPronunciation> TSrsAddPronunciationPckg;
typedef TPckgBuf<TSrsAddRule> TSrsAddRulePckg;
typedef TPckgBuf<TSrsRuleValidity> TSrsRuleValidityPckg;
typedef TPckgBuf<TSrsGetUtteranceDuration> TSrsGetUtteranceDurationPckg;
typedef TPckgBuf<TSrsPlayUtterance> TSrsPlayUtterancePckg;

typedef TPckgBuf<TInt*> TSrsIntPtrPckg;
typedef TPckgBuf<TNSSRecognitionMode> TSrsRecognitionModePckg;
typedef TPckgBuf<TTimeIntervalMicroSeconds32> TSrsTimeIntervalMicroSeconds32Pckg;
typedef TPckgBuf<TUid> TSrsUidPckg;

typedef TPckgBuf<TSIGrammarID > TSrsGrammarIDPckg;
typedef TPckgBuf<TSIGrammarID*> TSrsGrammarIDPtrPckg;
typedef TPckgBuf<TSILexiconID> TSrsLexiconIDPckg;
typedef TPckgBuf<TSILexiconID*> TSrsLexiconIDPtrPckg;
typedef TPckgBuf<TSIModelID> TSrsModelIDPckg;
typedef TPckgBuf<TSIModelID*> TSrsModelIDPtrPckg;
typedef TPckgBuf<TSIModelBankID> TSrsModelBankIDPckg;
typedef TPckgBuf<TSIModelBankID*> TSrsModelBankIDPtrPckg;
typedef TPckgBuf<TSIPronunciationID> TSrsPronunciationIDPckg;
typedef TPckgBuf<TSIRuleID> TSrsRuleIDPckg;
typedef TPckgBuf<TSIRuleID*> TSrsRuleIDPtrPckg;

typedef TPckgBuf<TSrsAddVoiceTag> TSrsAddVoiceTagPckg;
typedef TPckgBuf<TSrsAddSindeVoiceTag> TSrsAddSindeVoiceTagPckg;
typedef TPckgBuf<TSrsAddRuleVariant> TSrsAddRuleVariantPckg;
typedef TPckgBuf<TSrsAddPronunciationSI> TSrsAddPronunciationSIPckg;

// CLASS DECLARATION

/**
*  This is the TSrsRecognize class declaration.
*
*  @lib SpeechRecognitionCustomCommands.lib
*  @since 2.0
*/
#if(0)
class TSrsRecognize
{
public:

	/**
	*	C++ constructor for this class.
    * @since	2.0
    * @param	-
    * @return	-
    */
	TSrsRecognize() {}

	/**
	*	C++ constructor for this class.
    * @since	2.0
    * @param	aClientResultSet	Pointer to client result set object.
    * @return	-
    */
	TSrsRecognize(CSDClientResultSet* aClientResultSet) : 
				iClientResultSet(aClientResultSet){}
		
	// member variable pointer to result set
	CSDClientResultSet* iClientResultSet;

};
#endif
/**
*  This is the TSrsAddPronunciationSI class declaration.
*
*  @lib SpeechRecognitionCustomCommands.lib
*  @since 2.0
*/
class TSrsAddPronunciation
{
public:

	/**
	*	C++ constructor for this class.
    * @since	2.0
    * @param	-
    * @return	-
    */
	TSrsAddPronunciation() {}

	/**
	*	C++ constructor for this class.
    * @since	2.0
	* @param	aLexiconID			lexicon ID
	* @param	aModelBankID		model bank ID
	* @param    aModelID			model ID
	* @param	aPronunciationID	pronunciation ID 
    * @return	-
    */
	TSrsAddPronunciation(TSILexiconID aLexiconID, TSIModelBankID aModelBankID,
		TSIModelID aModelID, TSIPronunciationID* aPronunciationID) : 
		iLexiconID(aLexiconID), iModelBankID(aModelBankID), 
		iModelID(aModelID), iPronunciationID(aPronunciationID)
	{}

	// lexicon ID member variable 
	TSILexiconID iLexiconID;
	// model bank ID member variable 
	TSIModelBankID iModelBankID;
	// model ID member variable 
	TSIModelID iModelID;
	// pronunciation ID pointer member variable
	TSIPronunciationID* iPronunciationID;
};


/**
*  This is the TSrsAddRule class declaration.
*
*  @lib SpeechRecognitionCustomCommands.lib
*  @since 2.0
*/
class TSrsAddRule
{
public:
	/**
	*	C++ constructor for this class.
    * @since	2.0
    * @param	-
    * @return	-
    */
	TSrsAddRule() {}

	/**
	*	C++ constructor for this class.
    * @since	2.0
	* @param	aGrammarID			grammar ID
	* @param	aLexiconID			lexicon ID
	* @param	aPronunciationID	pronunciation ID 
	* @param	aRuleID				rule ID
    * @return	-
    */
	TSrsAddRule(TSIGrammarID aGrammarID, TSILexiconID aLexiconID,
		TSIPronunciationID aPronunciationID, TSIRuleID* aRuleID) : 
		iGrammarID(aGrammarID), iLexiconID(aLexiconID), 
		iPronunciationID(aPronunciationID), iRuleID(aRuleID) {}

	// grammar ID member variable
	TSIGrammarID iGrammarID;
	// lexicon ID member variable
	TSILexiconID iLexiconID;
	// pronunciation ID member variable
	TSIPronunciationID iPronunciationID;
	// rule ID pointer member variable
	TSIRuleID* iRuleID;

};


/**
*  This is the TSrsRuleValidity class declaration.
*
*  @lib SpeechRecognitionCustomCommands.lib
*  @since 2.0
*/
class TSrsRuleValidity
{
public:
	/**
	*	C++ constructor for this class.
    * @since	2.0
    * @param	-
    * @return	-
    */
	TSrsRuleValidity() {}

	/**
	*	C++ constructor for this class.
    * @since	2.0
	* @param	aGrammarID			grammar ID
	* @param	aRuleID				rule ID
	& @param	aRuldValid			indicates if the rule is valid
    * @return	-
    */
	TSrsRuleValidity(TSIGrammarID aGrammarID, TSIRuleID aRuleID, TBool* aRuleValid) : 
		iGrammarID(aGrammarID), iRuleID(aRuleID), iRuleValid(aRuleValid) {}

	// grammar ID member variable
	TSIGrammarID iGrammarID;
	// rule ID pointer member variable
	TSIRuleID iRuleID;
	// rule valid member variable
	TBool* iRuleValid; 
};

/**
*  This is the TSrsGetUtteranceDuration class declaration.
*
*  @lib SpeechRecognitionCustomCommands.lib
*  @since 2.0
*/
class TSrsGetUtteranceDuration
{
public:
	/**
	*	C++ constructor for this class.
    * @since	2.0
    * @param	-
    * @return	-
    */
	TSrsGetUtteranceDuration() {}

	/**
	*	C++ constructor for this class.
    * @since	2.0
	* @param	aModelBankID		model bank ID
	* @param	aModelID			model ID
    * @return	-
    */
	TSrsGetUtteranceDuration(TSIModelBankID aModelBankID, TSIModelID aModelID,
		TTimeIntervalMicroSeconds32* aDuration) : 
		 iModelBankID(aModelBankID), iModelID(aModelID),
			iDuration(aDuration) {}

	// model bank ID member variable
	TSIModelBankID iModelBankID;
	// model ID member variable
	TSIModelID iModelID;
	// utterance duration
	TTimeIntervalMicroSeconds32* iDuration;
};

/**
*  This is the TSrsPlayUtterance class declaration.
*
*  @lib SpeechRecognitionCustomCommands.lib
*  @since 2.0
*/
class TSrsPlayUtterance
{
public:
	/**
	*	C++ constructor for this class.
    * @since	2.0
    * @param	-
    * @return	-
    */
	TSrsPlayUtterance() {}

	/**
	*	C++ constructor for this class.
    * @since	2.0
	* @param	aModelBankID		model bank ID
	* @param	aModelID			model ID
    * @return	-
    */
	TSrsPlayUtterance(TSIModelBankID aModelBankID, TSIModelID aModelID) : 
		iModelBankID(aModelBankID), iModelID(aModelID) {}

	// model bank ID member variable
	TSIModelBankID iModelBankID;
	// model ID member variable
	TSIModelID iModelID;
};

/**
*  This class is used to moves data required by AddPronunciation method
*  over the MMF client-server barrier.
*
*  @lib SpekaerIndependentCustomCommands.lib
*  @since 2.8
*/
class TSrsAddPronunciationSI
    {
    public:
	/**
	*	C++ constructor for this class.
    * @since	2.8
    * @param	-
    * @return	-
    */
	TSrsAddPronunciationSI() {}

    /**
    * C++ constructor for this class.
    * @since 2.8
    * @param aLexiconID Lexicon identifier
    * @param aTextLength Length of the text to be trained.
    * @param aLanguage  Pronunciation language
    * @param aPronunciation New pronunciation identifier
    *                       will be assigned to this variable.
    * @return -
    */

    TSrsAddPronunciationSI(
        TSILexiconID aLexiconID,
        TInt aTextLength,
        TLanguage aLanguage,
        TSIPronunciationID& aPronunciationID )
        {
        iLexiconID = aLexiconID;
        iLanguage  = aLanguage;
        iPronunciationIDPtr = &aPronunciationID;
        iTextLength = aTextLength;
        }

    TSILexiconID iLexiconID;
    TLanguage  iLanguage;
    TInt iTextLength;
    TSIPronunciationID *iPronunciationIDPtr;
    };


/**
*  This class is used to moves data required by AddRuleVariant method
*  over the MMF client-server barrier.
*
*  @lib SpekaerIndependentCustomCommands.lib
*  @since 2.8
*/
class TSrsAddRuleVariant
    {
    public:
	/**
	*	C++ constructor for this class.
    * @since	2.8
    * @param	-
    * @return	-
    */
	TSrsAddRuleVariant() {}

    /**
    * C++ constructor for this class.
    * @since 2.8
    * @param aLexiconID Lexicon identifier
    * @param aTrainText Text to be trained. LESS THAN KMaxPhraseLen CHARACTERS.
    * @param aLanguage  Pronunciation language
    * @param aPronunciation New pronunciation identifier
    *                       will be assigned to this variable.
    * @return -
    */

    TSrsAddRuleVariant(
        TSIGrammarID aGrammarID,
        TSILexiconID aLexiconID,
        TInt         aPronunIDCount,
        TSIRuleID    aRuleID,
        TSIRuleVariantID* aRuleVariantID
        ) :
        iGrammarID( aGrammarID ),
        iLexiconID( aLexiconID ),
        iPronunIDCount( aPronunIDCount ),
        iRuleID( aRuleID ),
        iRuleVariantID( aRuleVariantID )
        {
        // Nothing
        }

    TSIGrammarID      iGrammarID;
    TSILexiconID      iLexiconID;
    TInt              iPronunIDCount;
    TSIRuleID         iRuleID;
    TSIRuleVariantID* iRuleVariantID;
    };

/**
*  This class is used to moves data required by AddVoiceTags method
*  over the MMF client-server barrier.
*
*  @lib NssSpeakerIndependentCustomCommands.lib
*  @since 2.8
*/
class TSrsAddVoiceTag
    {
    public:
        /**
        * C++ constructor for this class.
        * @since 2.8
        */
        TSrsAddVoiceTag() {}
        
        /**
        * C++ constructor for this class.
        * @since 2.8
        *
        * @param aLexiconID Lexicon identifier
        * @param aGrammarID Grammar identifier
        * @param aRuleID
        * @param aLanguageTable Pronunciation languages
        */
        TSrsAddVoiceTag( TSILexiconID aLexiconID,
                         TSIGrammarID aGrammarID,
                         TSIRuleID* aRuleID,
                         const RArray<TLanguage>& aLanguageTable ) :
                         iLexiconID( aLexiconID ),
                         iGrammarID( aGrammarID ),
                         iRuleID( aRuleID )
            {
            TInt languageCount = 0;
            
            if ( &aLanguageTable != NULL )
                {
                languageCount = aLanguageTable.Count();
                }
            
            for ( TInt k = 0; k < KMaxLanguages; k++ )
                {
                if ( k < languageCount )
                    {
                    // If aLanguageTable == NULL, languageCount == 0 and
                    // this is never called.
                    iLanguages[ k ] = aLanguageTable[ k ];
                    }
                else
                    {
                    iLanguages[ k ] = ELangNone;
                    }
                }
            }
        
        /**
        * Returns a copy of language ids in RArray
        *
        * @since 2.8
        * @param aLangArray Array which is filled
        */
        void ListLanguagesL( RArray<TLanguage>& aLangArray )
            {
            for ( TInt k = 0; iLanguages[ k ] != ELangNone; k++ )
                {
                User::LeaveIfError( aLangArray.Append( iLanguages[ k ] ) );
                }
            }
        
        TSILexiconID iLexiconID;
        TSIGrammarID iGrammarID;
        TSIRuleID* iRuleID;
        TLanguage iLanguages[ KMaxLanguages ];
    };

/**
*  This class is used to moves data required by AddSindeVoiceTags method
*  over the MMF client-server barrier.
*
*  @lib NssSpeakerIndependentCustomCommands.lib
*  @since 3.1
*/
class TSrsAddSindeVoiceTag
    {
    public:
        /**
        * C++ constructor
        * @since 3.1
        */
        TSrsAddSindeVoiceTag() {}
        
        /**
        * C++ constructor.
        * Makes a copy LanguageArray.
        *
        * @since 3.1
        * @param aLexiconID Lexicon identifier
        * @param aGrammarID Grammar identifier
        * @param aRuleID
        * @param aLanguageTable Pronunciation languages
        */
        TSrsAddSindeVoiceTag( TSILexiconID aLexiconID,
                              TSIGrammarID aGrammarID,
                              TSIRuleID* aRuleID,
                              const RArray<RLanguageArray>& aLanguageTable ) :
                              iLexiconID( aLexiconID ),
                              iGrammarID( aGrammarID ),
                              iRuleID( aRuleID )
            {
            // Find out how many arrays there are inside the language array
            TInt languageArrayCount = aLanguageTable.Count();
            
            for ( TInt partsInText = 0; partsInText < KMaxPartitionsInText; partsInText++ )
                {
                RLanguageArray languages;
                TInt languageCount = 0;
                if ( partsInText < languageArrayCount )
                    {
                    languages = aLanguageTable[ partsInText ];
                    languageCount = languages.Count();
                    }
         
                for ( TInt languageCounter = 0; languageCounter < KMaxLanguages; languageCounter++ )
                    {
                    if ( languageCounter < languageCount )
                        {
                        // Store real value
                        iLanguages[ KMaxLanguages * partsInText + languageCounter ] = languages[ languageCounter ];
                        iGrouping[ KMaxLanguages * partsInText + languageCounter ] = partsInText;
                        }
                    else
                        {
                        // Fill in ELangNone & KErrNotFound to empty spaces
                        iLanguages[ KMaxLanguages * partsInText + languageCounter ] = ELangNone;
                        iGrouping[ KMaxLanguages * partsInText + languageCounter ] = KErrNotFound;
                        }
                    }
                }
            }
        
        /**
        * Returns a copy of language ids in RArray<RLanguageArray>
        *
        * @since 3.1
        * @param aLangArray Array which is filled
        */
        void ListLanguagesL( RArray<RLanguageArray>& aLangArray )
            {
            aLangArray.Reset();
            RLanguageArray langArr;
            CleanupClosePushL( langArr );
            TInt previousGroup( KErrNotFound );
            for ( TInt k = 0; k < KMaxLanguages * KMaxPartitionsInText; k++ )
                {
                if ( iLanguages[k] != ELangNone )
                    {
                    if ( ( previousGroup != iGrouping[k] ) && ( previousGroup != KErrNotFound ) )
                        {
                        User::LeaveIfError( aLangArray.Append( langArr ) );
                        // Ownership transferred
                        CleanupStack::Pop( &langArr );
                        // Create new array for next bunch of languages
                        langArr = RLanguageArray();
                        CleanupClosePushL( langArr );
                        }
                    User::LeaveIfError( langArr.Append( iLanguages[k] ) );
                    previousGroup = iGrouping[k];
                    }
                } // for
            // Append the last one if there just is something
            if ( langArr.Count() > 0 )
                {
                User::LeaveIfError( aLangArray.Append( langArr ) );
                }
            CleanupStack::Pop( &langArr );
            }
        
        // Lexicon ID
        TSILexiconID iLexiconID;

        // Grammar ID
        TSIGrammarID iGrammarID;

        // Rule ID pointer, used as a return value to client thread, not owned
        TSIRuleID* iRuleID;

    private:
        // Linear array of language ids
        TLanguage iLanguages[ KMaxLanguages * KMaxPartitionsInText ];

        // Array which maps languages in iLanguages to groups
        TInt iGrouping[ KMaxLanguages * KMaxPartitionsInText ];
    };

#endif	// NSSSISPEECHRECOGNITIONCUSTOMCOMMANDCOMMON_H

// End of file
