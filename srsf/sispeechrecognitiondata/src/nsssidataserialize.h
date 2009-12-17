/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Serialization functions for CSIGrammar and CSILexicon
*
*/



#ifndef SIDATASERIALIZE_H
#define SIDATASERIALIZE_H

//  INCLUDES
//#include <?include_file>
#include <nsssispeechrecognitiondatadevasr.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;


// The grammar and the lexicon have a binary format.
// These IDs identify the format.
const TInt32 KBinaryGrammarID   = 0xAACF1234;
const TInt32 KBinaryCompiledGrammarID = 0xAACF1235;
const TInt32 KBinaryLexiconID   = 0xAACF1236;
const TInt32 KBinaryResultSetID = 0xAACF1237;
const TInt32 KBinaryParameterID = 0xAACF1238;

const TInt32 KBinaryNibble16ID  = 0xAACF1239;
const TInt32 KBinaryNibble4ID   = 0xAACF123A;
const TInt32 KBinaryRLEID       = 0xAACF123B;
const TInt32 KBinaryYesNoID     = 0xAACF123C;

const TInt KShortPronunLimit   = 0xffff;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
struct TLexiconPhoneme // comment
    {
    TBuf8<KMaxPhonemeLength+1> iPhoneme;
    TInt                       iIndex;
    };

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION



/**
*  Serialization of CSILexicon
*  Lexicon is a container of pronunciations. It contains 5 types of information.
*  This class puts the informaiton to 5 piles, and uses primitive encoding
*  methods to reduce the space needed. The methods used are:
*
*   * Nibble coding: Suppose you have k 32-bit values, but most of them fit to
*                    16 bit. This method creates a k-length array of 16-bit
*                    values. If the original value fits, it is placed there.
*                    If it is too large, the value in this compact array
*                    signals that the real value is in a 32-bit 'overflow' array.
*   * RLE coding: For values, which are almost always the same. For example,
*                 model bank for pronunciations, and lexicon for rule variants.
*
*  @lib ?library
*  @since ?Series60_version
*/



/**
* Codes 32-bit unsigned integers to 16 bits.
* Sorts the values to two classes: those, which fit to 16 bits, and those,
* which require 32 bits. Then those, which require 32 bits are placed to an
* overflow array.
*
* Space is saved, if over 50% of values fit to 16 bits.
*
* @lib SISpeechRecognitionData.lib
* @since 2.8
*/
class CNibble16Coder : public CBase
    {
public: // Constructors and destructors
    /**
    * Codes an array.
    * @since 2.8
    * @param aPlainArray Array to be encoded
    * @return none
    */
    static CNibble16Coder* NewL( const RArray<TUint32>& aPlainArray );

    /**
    * Reads an array from stream.
    * @since 2.8
    * @param aStream a stream, where a CNibble16Coder was previously stored.
    * @return none
    */
    static CNibble16Coder* NewL( RReadStream& aStrem );

    /**
    * Destructor..
    */
    ~CNibble16Coder();

public: // New functions
    /**
    * Initializes the iterator.
    * @since 2.8
    * @param aIterator The iterator
    * @return none
    */
    void DecodeReset();

    /**
    * Decodes one value. Leaves with KErrOverflow, if all values in the array
    * have been read.
    * @since 2.8
    * @return The decoded value.
    */
    TUint32 NextL();

    /**
    * Externalizes the array to the stream.
    * @since 2.8
    * @param aStream Stream to store into.
    * @return none
    */
    void ExternalizeL(RWriteStream& aStream);

private:
    /**
    * C++ default constructor.
    */
    CNibble16Coder();

private:
    // Those values, which fit to 16 bits
    RArray<TUint32> iMainArray;

    // The king size values requiring 32 bits
    RArray<TUint32> iOverflowArray;

    /**************** Iterator variables ***************/
    // Intex to the main array
    TInt iPosition;

    // Index to the overflow array
    TInt iOverflowPos;
    /************** Iterator variables end *************/
    };

/**
* Codes 32-bit unsigned integers to 4 bits.
* Sorts the values to two classes: those, which fit to 4 bits, and those,
* which require 32 bits. Then those, which require 32 bits are placed to an
* overflow array.
*
* @lib SISpeechRecognitionData.lib
* @since 2.8
*/
class CNibble4Coder : public CBase
    {
public: // Constructors and destructors
    /**
    * Codes an array of 32-bit variables to 2 arrays,
    * one 4-bit and the other 32-bit.
    * @since 2.8
    * @param aMainArray Populated during the function
    * @param aOverflowArray Populated during the function
    * @param aPlainArray Array to be encoded
    * @return none
    */
    static CNibble4Coder* NewL( const RArray<TUint32>& aPlainArray );

    /**
    * Reads an array from stream.
    * @since 2.8
    * @param aStream a stream, where a CNibble4Coder was previously stored.
    * @return none
    */
    static CNibble4Coder* NewL( RReadStream& aStrem );

    /**
    * Destructor..
    */
    ~CNibble4Coder();

public: // New functions

    /**
    * Initializes the iterator.
    * @since 2.8
    * @param aIterator The iterator
    * @return none
    */
    void DecodeReset();

    /**
    * Decodes one value. Leaves with KErrOverflow, if all values in the array
    * have been read.
    * @since 2.8
    * @param aResult Result is placed to this variable.
    * @param aIterator The iterator
    * @param aMainArray Contains coded data.
    * @param aOverflowArray Contains coded data.
    * @return none
    */
    TUint32 NextL();

    /**
    * Externalizes the array to the stream.
    * @since 2.8
    * @param aStream Stream to store into.
    * @return none
    */
    void ExternalizeL(RWriteStream& aStream);

private:
    /**
    * C++ default constructor.
    */
    CNibble4Coder();

private:
    // Those values, which fit to 4 bits
    RArray<TUint32> iMainArray;

    // The values requiring more
    RArray<TUint32> iOverflowArray;

    /**************** Iterator variables ***************/
    // Intex to the main array
    TInt iPosition;

    // 4-bit slot in the 32-bit entry
    TInt iSlot;

    // Index to the overflow array
    TInt iOverflowPos;
    /************** Iterator variables end *************/
    };

/**
* Codes 32-bit unsigned integers using Run Length Encoding.
*
* @lib SISpeechRecognitionData.lib
* @since 2.8
*/
class CRLECoder : public CBase
    {
public: // Constructors and destructors
    /**
    * Codes an array of 32-bit variables using Run Length Encoding.
    * @since 2.8
    * @param aPlainArray Array to be coded
    * @return none
    */
    static CRLECoder* NewL(const RArray<TUint16>& aPlainArray);

    /**
    * Internalizes an array from stream.
    * @since 2.8
    * @param aStream A stream containing the object.
    * @return none
    */
    static CRLECoder* NewL( RReadStream& aStream );

    /**
    * Destructor..
    */
    ~CRLECoder();

public: // New functions

    /**
    * Initializes the iterator.
    * @since 2.8
    * @return none
    */
    void DecodeReset();

    /**
    * Decodes one value. Leaves with KErrOverflow, if all values in the array
    * have been read.
    * @since 2.8
    * @param aResult Result is placed to this variable.
    * @param aIterator The iterator
    * @param aRleArray Contains coded data.
    * @return none
    */
    TUint16 NextL();

    /**
    * Externalizes the array to the stream.
    * @since 2.8
    * @param aStream Stream to store into.
    * @return none
    */
    void ExternalizeL(RWriteStream& aStream);

private:
    /**
    * C++ default constructor.
    */
    CRLECoder();

private: // Class-internal data types

    typedef struct
        {
        // How many times the value is repeated
        TUint16 iCount;
        // The value
        TUint16 iValue;

        } TValueAndCount;
private:

    // The values, stored using run length encoding.
    RArray<TValueAndCount> iRleArray;

    /**************** Iterator variables ***************/
    // Intex to the main array
    TInt iPosition;

    // RLE repetition counter
    TUint16 iRepetition;
    /************** Iterator variables end *************/
    };

class CYesNoCoder : public CBase
    {
public: // Constructors and destructors

    /**
    * 2-phase constructor.
    */
    static CYesNoCoder* NewL();

    /**
    * Internalizes a bit array from stream.
    * @since 2.8
    * @param aStream A stream containing the object.
    * @return none
    */
    static CYesNoCoder* NewL( RReadStream& aStream );

    /**
    * Destructor..
    */
    ~CYesNoCoder();

public: // New functions

    /**
    * Encodes a single bit. Increases the iterator. 
    * @since 2.8
    * @param aValue The bit to be coded (true or false)
    * @return none
    */
    void EncodeL( TBool aValue );

    /**
    * Initializes the iterator.
    * @since 2.8
    * @return none
    */
    void DecodeReset();

    /**
    * Decodes a single bit. Increases the iterator. Leaves, if the storage ends.
    * @since 2.8
    * @return Yes or no.
    */
    TBool NextL();

    /**
    * Externalizes the array to the stream.
    * @since 2.8
    * @param aStream Stream to store into.
    * @return none
    */
    void ExternalizeL(RWriteStream& aStream);

private:
    /**
    * C++ default constructor.
    */
    CYesNoCoder();

    /**
    * Makes the iterator point to the next bit.
    */
    void NextBit();

private:
    // Storage for the bits
    RArray<TUint32> iStore;

    /**************** Iterator variables ***************/
    // Points to the correct TUint32 in the array.
    TInt iPosition;

    // Bit mask for getting the next bit.
    TUint32 iMask;
    /************** Iterator variables end *************/
    };

/**
* Serialization helper class for CSILexicon.
*
* The native form of CSILexicon is an array of pronunciations. This class
* piles each data type to its own pile - model bank IDs, pronunciations,
* etc. - and then serializes these piles.
*
* @lib SISpeechRecognitionData.lib
* @since 2.8
*/
class CSILexiconSerializer : public CBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor. For externalization.
    */
    static CSILexiconSerializer* NewLC( const CSILexicon& aLexicon );

    /**
    * Two-phased constructor. For internalization.
    */
    static CSILexiconSerializer* NewLC( RReadStream& aStream );

    /**
    * Destructor.
    */
    virtual ~CSILexiconSerializer();
        
public: // New functions
        
    /**
    * Stores the object to a stream.
    * @since 2.8
    * @param aStream Stream to serialize to
    * @return none
    */
    void ExternalizeL( RWriteStream& aStream );
        
    /**
    * Populates a lexicon.
    * @since 2.8
    * @param aLexicon The lexicon to internalize to
    * @param aLexiconID CSILexicon does not contain a function to set
    *                   Lexicon ID, so we can't return it with aLexicon.
    * @return none
    */
    void RestoreL( CSILexicon& aLexicon, TSILexiconID& aLexiconID );

private:
        
    /**
    * C++ default constructor.
    */
    CSILexiconSerializer();
        
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(const CSILexicon& aLexicon);
        
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(RReadStream& aStream);

    void SavePronunciationL(CSIPronunciation* aPronun);
    bool NextPhonemeL( TDes8& phonemeBuf, const TDesC8& phonemeSeq,
                       TInt&  aReadIterator );
    HBufC8* Index2PhonemeLC(const TDesC8& aIndexSeq);

    void InternalizeL( RReadStream& aStream );
    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );
        
public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        
protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        
private:    // Data

    // !!!! Check the pronunciation storing once more.
    RPointerArray<CSIPronunciation> iPronunciations;
    RPointerArray<HBufC8>           iIndexPronuns;

    RArray<TLexiconPhoneme> iConversionTable;

    // Pronunciation IDs as 16-bit nibble-coded numbers
    CNibble16Coder* iPronunIDs;

    // Model bank IDs as RLEd 32-bit numbers
    CRLECoder* iModelBankIDs;

    // Parameters are serialized as such, since we know nothing about them.
    CBufFlat* iParamBuf;

    // Total number of pronunciations
    TInt iCount; 

    // Lexicon ID
    TSILexiconID iLexiconID;

    };

/**
* Serialization helper class for CSIGrammar.
*
* The native form of CSIGrammar is an array of rules and rule variants.
* This class piles each data type to its own pile - model bank IDs,
* pronunciation IDs, etc. - and then serializes these piles.
*
* @lib SISpeechRecognitionData.lib
* @since 2.8
*/
class CSIGrammarSerializer : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor for externalization.
        */
        static CSIGrammarSerializer* NewLC( const CSIGrammar& aGrammar );
        
        /**
        * Two-phased constructor for internalization.
        */
        static CSIGrammarSerializer* NewLC( RReadStream& aStream );

        /**
        * Destructor.
        */
        virtual ~CSIGrammarSerializer();
        
    public: // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

        /**
        * Stores the object to a stream.
        * @since 2.8
        * @param aStream Stream to serialize to
        * @return none
        */
        void ExternalizeL( RWriteStream& aStream );

        /**
        * Populates the grammar.
        * @since 2.8
        * @param aGrammar The grammar to be filled.
        * @param aGrammarID Grammar ID is returned in this variable, as
        *                   CSIGrammar does not contain SetGrammarID() method.
        * @return none
        */
        void RestoreL( CSIGrammar& aGrammar, TSIGrammarID& aGrammarID );

    public: // Functions from base classes
        
        /**
        * From ?base_class ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();
        
    private:
        
        /**
        * C++ default constructor.
        */
        CSIGrammarSerializer();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CSIGrammar& aGrammar );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RReadStream& aStream );

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

private:    // Data

    // Grammar ID
    TSIGrammarID iGrammarID;

    // Rule count
    TInt       iCount;

    // Rule IDs
    CNibble16Coder* iRuleIDs;

    // Numbers of rule variant in rules
    CNibble4Coder* iRuleVariantCounts;

    // Rule variant IDs
    CNibble4Coder* iRuleVariantIDs;

    // Lexicon IDs of variants
    CRLECoder* iRuleVariantLexiconIDs;

    // Languages of variants
    CNibble4Coder* iRuleVariantLanguages;

    // Lengths of the pronunciation sequences
    CNibble4Coder* iPronunIDSeqLengths;

    // Pronunciation IDs of the pronunciation seuqences
    CNibble16Coder* iPronunIDSequences;

    CBufFlat* iParamBuf;
    // Reserved pointer for future extension
    //TAny* iReserved;
    };

#endif      // SIDATASERIALIZE_H

// End of File
