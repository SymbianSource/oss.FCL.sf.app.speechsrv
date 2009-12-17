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
* Description:     This file contains definition of the SISpeechRecognitionDataClient structure and API.
*
*/





#ifndef __SRSSIDATA_CLIENT_H__
#define __SRSSIDATA_CLIENT_H__

//  INCLUDES
#include <nsssispeechrecognitiondatacommon.h>
 

// FORWARD DECLARATION
class  CSIResultSet;


// CLASS DECLARATION

/**
*  Class to encapsulate a client recognition result.
*  The result is formulated for client consumption.
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.5
*/

class CSIClientResult : public CBase
{
public:	 // Constructors and destructor
        /**
        * Destructor.
        */
	IMPORT_C ~CSIClientResult();
        /**
        * Two-phased constructor.
        */
	IMPORT_C static CSIClientResult* NewL();
	IMPORT_C static CSIClientResult* NewLC();
	IMPORT_C static CSIClientResult* NewL(TSIGrammarID aGrammarID, TSIRuleID aRuleID);
	IMPORT_C static CSIClientResult* NewLC(TSIGrammarID aGrammarID, TSIRuleID aRuleID);

		/**
        * Use to set the grammar Id on the result.
        * @since 2.5
        * @param aGrammarID - grammar identifier.
        * @return None.
		*/
	IMPORT_C void SetGrammarID(TSIGrammarID aGrammarID);

		/**
        * Use to retrieve the grammar Id of the result.
        * @since 2.5
		* @param None.
        * @return Grammar identifier.
        */
	IMPORT_C TSIGrammarID GrammarID() const;

		/**
        * Use to set the rule Id of the result.
        * @since 2.5
        * @param aRuleID - rule identifier.
		* @return None.
        */
	IMPORT_C void SetRuleID(TSIRuleID aRuleID);

		/**
        * Use to retrieve the rule Id of the result.
        * @since 2.5
		* @param None.
        * @return Rule identifier.
        */ 
	IMPORT_C TSIRuleID RuleID() const;

private:
	void ConstructL();
	CSIClientResult();
	CSIClientResult(TSIGrammarID aGrammarID, TSIRuleID aRuleID);

private:
	TSIGrammarID iGrammarID;
	TSIRuleID iRuleID;
};


/**
*  Class to encapsulate a client recogntion result set
*  This is a lightweight result set formulated for client consumption
*
*  @lib nsssispeechrecognitiondata.lib
*  @since 2.5
*/

class CSIClientResultSet : public CBase
{
public:	// Constructors and destructor
        /**
        * Destructor.
        */
	IMPORT_C ~CSIClientResultSet();

	    /**
        * Two-phased constructor.
        */
	IMPORT_C static CSIClientResultSet* NewL();
	IMPORT_C static CSIClientResultSet* NewLC();

		/**
			* Use to add a result to the result set.
			* @since 2.5
			* @param aResult - reference to the result object to be added.
			* @return KErrNone, if the insertion is successful, otherwise one of the system wide error codes.
		*/
		IMPORT_C void AddL( const  CSIClientResult* aResult); 
		
		
		/**
			* Use to delete a result to the result set.
			* @since 2.5
			* @param aIndex Index of  .
			* @return 
			*/
		IMPORT_C void DeleteL(TInt aIndex) ;	
		/**
        * Sets the maximum number of best matches (N-best) expected in the recognition result after recognition.
        * @since 2.5
        * @param aMaxResults - the maximum number of matches expected in the recognition result.
		* @return None.
        */
	IMPORT_C void SetMaxResultsL(TInt aMaxResults);

		/**
        * Use to retrieve the current value of the max number of expected after recognition.
        * @since 2.5
        * @param None.
		* @return The maximum number of best matches (N-best) expected in the recognition result.
        */ 
	IMPORT_C TInt MaxResults() const;

		/**
        * Use to retrieve a result at a given index within the result set.
        * @since 2.5
        * @param None.
		* @return Pointer to CSIClientResult object.
        */
	IMPORT_C const CSIClientResult& AtL(TInt anIndex) const;

		/**
        * Use to set the number of results actually returned after recogntion.
        * @since 2.5
        * @param aResultCount - number of results returned by the recognizer.
		* @return None.
        */
	IMPORT_C void SetResultCount(TInt aResultCount);

		/**
        * Use to retrieve the number of results actually returned after recogntion.
        * @since 2.5
        * @param None.
		* @return Number of results actually returned after recogntion.
        */
	IMPORT_C TInt ResultCount() const;
 

	IMPORT_C  CSIResultSet& SIResultSet() const;

	
	IMPORT_C void SetSIResultSet(CSIResultSet* aSIResultSet);

		/**
        * Internalize to a data stream
        * @since 2.8
		* @param aStream Data stream
        * @return  None
        */ 
	IMPORT_C virtual void InternalizeL( RReadStream& aStream );

		/**
        * Externalize from a data stream
        * @since 2.8
		* @param None
        * @return  aStream Data stream
        */ 
	IMPORT_C virtual void ExternalizeL( RWriteStream& aStream ) const;

private:
	void ConstructL();
	CSIClientResultSet();

private:
	TInt iMaxResults;
	TInt iResultCount;
	RPointerArray<CSIClientResult> iResultArray;
	CSIResultSet* iSIResultSet;
};

#endif
 