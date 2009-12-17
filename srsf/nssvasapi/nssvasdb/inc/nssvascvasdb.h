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
* Description:  
*
*/


#ifndef CNSSVASDB_H
#define CNSSVASDB_H

#include <d32dbms.h>
#include <e32def.h>
#include "nssvascvasdbsrvdef.h"
#include "nssvasdbkonsts.h"
#include "nssvascvasdbdataexchanger.h"
#include "nssvastvasdbcreator.h"
#include "nssvascspeechitembuilder.h"
#include "nssvasccontextbuilder.h"

#include "nssvasctag.h"


class CNssVasBackupObserver;


// CLASS DECLARATION

/**
*
*  The CNssVasDb class definition. 
* 
*  @lib NssVASApi.lib
*  @since 2.8
*/
NONSHARABLE_CLASS( CNssVasDb ) : public CBase
    {
public:
	
    /**
    * Two-phased constructor.
	* @since 2.0
	* @param aClient - client thread
	* @param aServer - vasdb server
    */
	static CNssVasDb* NewL( CNssContextBuilder& aContextBuilder,
	                        CNssSpeechItemBuilder& aSpeechItemBuilder );

	/**
    * Destructor.  
    * @since 2.0
    * @param 
    * @return void
    */
	~CNssVasDb();	

public:

    /**
    * Read a context from the database. If context with this name is not present,
    * leave with KErrNotFound.
    * @since 2.8
    * @param aName the name of the context
    * @return CArrayFixFlat<CNssContext> A List of contexts.
    */
    CArrayPtrFlat<CNssContext>* GetContextByNameL( const TDesC& aName );

    /**
    * Read all global contexts from the database. Contexts have a flag, which
    * tells if they are global or not.
    * Global context = "Context, which is recognized when the user presses
    *                   the voice key"
    * Currently, SIND names and command words are global.
    * @since 2.8
    * @return CArrayFixFlat<CNssContext> A List of contexts.
    */
    CArrayPtrFlat<CNssContext>* GetGlobalContexts();

    /**
    * Read all global contexts from the database. 
    * @since 2.8
    * @return CArrayFixFlat<CNssContext> A List of contexts.
    */
    CArrayPtrFlat<CNssContext>* GetAllContexts();

    /**
    * Create a new database.
    * @since 2.8
    * @return none
    */
	void CreateDatabaseL();

    /**
    * Open database.
    * @since 2.8
    * @return none
    */
	void OpenDatabaseL();

    /**
    * Close database.
    * @since 2.8
    * @return Symbian-wide error codes
    */
	TInt  CloseDatabase();

    /**
    * Get model bank id and lexicon id from some context. All contexts
    * have the same model bank id.
    * @since 2.8
    * @param aModelBankId Place to store model bank ID.
    * @param aLexiconId   Place to store lexicon ID.
    * @return ETrue if successful (leaves, if not successful)
    */
	TBool GetModelBankIdLexiconIdL( TUint32& aModelBankId, TUint32& aLexiconId );

    /**
    * Check if a context has tags or not.
    * @since 2.8
    * @param aContextId context id
    * @return ETrue if there are tags, EFalse if not.
    */
    TBool TagExistL( TInt aContextId );

    /**
    * How many tags there are in this context?
    * @since 2.8
    * @param aContextId context id
    * @return The number of tags
    */
	TInt  TagCountL( TInt aContextId );

    /**
    * Saves a context. When a context is save, a unique Context ID
    * is assigned to it. This ID is returned in aNewContextId reference variable.
    * @since 2.8
    * @param aContext The context
    * @param aNewContextId Place to store the newly assigned context ID.
    * @return none
    */
    void SaveContextL( CNssContext& aContext, TInt& aNewContextId );

    /**
    * Updates the information of an existing context.
    * @since 2.8
    * @param aContext The context
    * @return none
    */
	void UpdateContextL( CNssContext& aContext );

    /**
    * Updates the client data of an existing context.
    * @since 2.8
    * @param aContext The context
    * @return none
    */
	void UpdateContextClientDataL( CNssContext& aContext );

    /**
    * Saves a tag. When a tag is saved, a unique ID is assigned to it
    * @since 2.8
    * @param aContext The context
    * @return none
    */
    void SaveTagL( CNssTag& aTag, TInt& aNewId );

    /**
    * Get tag reference list. A tag reference contains only the name and the ID.
    * @since 2.8
    * @param aContextId  The context
    * @return Tag references for all tags in the context.
    */
	TNssTagReferenceListArray* GetTagReferenceListL( TInt aContextId );

    /**
    * Get a tag, given the grammar ID and rule ID of the tag.
    * Used when resolving recognition result.
    * @since 2.8
    * @param aGrammarIdRuleId Dedicated struct to pass gramamr ID and rule ID.
    * @return Tag with the given rule ID.
    */
	CArrayPtrFlat<CNssTag>* GetTagL( TNssGrammarIdRuleId aGrammarIdRuleId );

    /**
    * Get a list of tags tag, given the list of grammar IDs and rule IDs of 
    * the tags.
    * Used when resolving recognition result.
    * @since 2.8
    * @param aGrammarIdRuleIds Array of gramamr ID and rule ID pairs
    * @return List of tags with the given rule ID and grammar ID.
    */
	CArrayPtrFlat<CNssTag>* GetTagsL( TNssGrammarIdRuleIdListArray& aGrammarIdRuleIds );
	
	/**
    * Get a tag, given the recognition phrase.
    * @since 2.8
    * @param aName Recognition phrase of the tag
    * @return Tag with the given rule ID.
    */
	CArrayPtrFlat<CNssTag>* GetTagL( const TDesC& aName );

    /**
    * Get a list of tags based on RRD integer data.
    * The client can save 5 integer for each tag. This function gets those tags,
    * which have value aNum in position aPosition.
    * @since 2.8
    * @param aContextId Context ID
    * @param aNum The number we search for.
    * @param aPosition RRD integer position. There are 5 slots, numbered 0-4.
    * @return List of tags
    */
	CArrayPtrFlat<CNssTag>* GetTagL( TInt aContextId, TInt aNum, TInt aPosition );

    /**
    * Get a list of tags based on RRD text data.
    * The client can save 5 strings for each tag. This function gets those tags,
    * for which the text data at slot aPosition matches to aText.
    * @since 2.8
    * @param aContextId Context ID
    * @param aText The text we search for.
    * @param aPosition RRD text position. There are 5 slots, numbered 0-4.
    * @return List of tags
    */
	CArrayPtrFlat<CNssTag>* GetTagL( TInt aContextId, const TDesC& aText, TInt aPosition );

    /**
    * Gets all tags from this context.
    * @since 2.8
    * @param aContext A context
    * @return List of tags
    */
	CArrayPtrFlat<CNssTag>* GetTagL( const CNssContext& aContext );

    /**
    * Get a tag, given a tag ID.
    * @since 2.8
    * @param aTagId tag id
    * @return List of tags
    */    
	CArrayPtrFlat<CNssTag>* GetTagL( TUint32 aTagId );

    /**
    * Method to get the default model bank id
    * @since 2.8
    * @param aId reference where to put the model bank Id
    * @return symbian-wide error code
    */
    void GetDefaultModelBankIdL( TUint32& aId );

    /**
    * Modifies the context table so that all contexts start using
    * the new id as their model bank id.
    * @since 2.8
    * @param aNewId new model bank id
    * @return symbian-wide error code
    */
    void ResetModelBankL( TUint32 aNewId );

    /**
    * Deletes a context.
    * @since 2.8
    * @param aName Name of the context to be deleted.
    * @return none
    */
	void DeleteContextL( const TDesC& aName );

    /**
    * Deletes a tag.
    * @since 2.8
    * @param aName Name of the tag to be deleted.
    * @return none
    */
	void DeleteTagL( const TDesC& aName );

    /**
    * Deletes a tag.
    * @since 2.8
    * @param aTagId The unique ID of the tag.
    * @return none
    */
	void DeleteTagL( TUint32 aTagId );

    /**
    * Deletes a tag without starting a transaction.
    * @since 2.8
    * @param aTagId Tag id
    * @return none
    */
    void DeleteTagInsideTransactionL( TUint32 aTagId );

    /**
    * Deletes a list of tags.
    * @since 2.8
    * @param aTagArray A list of tags
    */
    void DeleteTagsL( const RArray<TUint32>& aTagIdArray );

    /**
    * Updates the rule ids of some tags.
    * The rule ID changes, if an already trained tag is retrained.
    * @since 2.8
    * @param aRetrainedTags Tags which need an update for rule ID.
    * @return TInt Success status.
    */
    void UpdateTagRuleIDsL( const RArray<TNssSpeechItem>& aRetrainedTags );

    /**
    * Saves a list of tags.
    * @since 2.8
    * @param aTagArray A list of trained tags
    * @param aTagIdArray The newly assigned Tag IDs are placed here.
    */
    void SaveTagsL( CArrayPtrFlat<CNssTag>* aTagArray, RArray<TInt>& aTagIdArray );

	/**
	* Permit transactions. Used when e.g. backup or restore begins 
	* @since 3.1
	*/
	void LockTransactionsL();

	/**
	* Allow transactions. Used when e.g. backup or restore ends 
	* @since 3.1
	*/
	void UnlockTransactionsL();
	
private:

    /*
    * First phase constructor.
    */
    CNssVasDb( CNssContextBuilder& contextBuilder,
	           CNssSpeechItemBuilder& speechItemBuilder );

    /*
    * Second phase constructor.
    */
	void ConstructL();

    /*
    * Get a list of contexts.
    * The pipeline for the public GetContextL variants is:
    * (1) Turn parameters into an SQL query
    * (2) Call this function
    */
    CArrayPtrFlat<CNssContext>* GetContextL( const TDesC& aSqlStatement );

    /*
    * "workhorse" function for the the previous one. The previous function
    * takes care of locking and unlocking the database.
    */
    CArrayPtrFlat<CNssContext>* GetContextInsideTransactionL( const TDesC& aSqlQuery );

    /*
    * Reads a list of contexts after query. RDbView object is created
    * as a result of a query.
    */
	void FillContextListArrayL( RDbView& aView, CArrayPtrFlat<CNssContext>& aContextList );

    /*
    * Get the RRD data for a tag. The RRD table is separate from tag table.
    */
	void FillRRDL( TUint32 aTagId, CNssRRD& aRRD );

    /*
    * Get a speech item after a query (which created the RDbView object).
    * In addition to core tags data, it fetces the context and the RRD data
    * form separate arrays.
    */
	void FillSpeechItemL( RDbView& aView, CNssSpeechItem& aSpeechItem, TInt& aContextId );

    /*
    * Get a context after query.
    */
	void FillContextL( RDbView& aView, CNssContext &aContext );

    /*
    * (1) Makes a tag out of context, speech item and RRD data.
    * (2) Adds the tag to aTagList.
    */
	void FillTagListArrayL( CArrayPtrFlat<CNssTag>& aTagList, 
	                        CNssContext* aContext,
	                        CNssSpeechItem* aSpeechItem,
	                        CNssRRD* aRRD );

    /*
    * Saves a context.
    */
    void SaveContextL( CNssContext* aContext, TInt& aNewContextId );

    /*
    * Saves RRD data.
    */
	void SaveRRDL(CNssRRD* aRRD);	

    /*
    * Saves a tags. When a tag is saved for the first time, an ID is assigned
    * to it. This ID is stored to aTagId.
    */
	void SaveTagL( CNssTag* aTag, TInt& aTagId);

    /*
    * The public GetTagL / GetTagListL variants make an SQL query from their
    * parameters and call this function to do the rest.
    */
    CArrayPtrFlat<CNssTag>* GetTagListByQueryL(const TDesC& aSqlQuery);

    /*
    * Deletes RRD data
    */
	void DeleteRRDL( TUint32 aTagId );

    /*
    * Update an existing tag. Locks database.
    */
	void UpdateTagL( CNssTag& aTag );

    /*
    * Update an existing tag.
    */
	void UpdateTagInTransactionL( CNssTag& aTag );

    /*
    * Updates an existing context. Does not lock database.
    */
	void DoUpdateContextL( CNssContext& aContext );

    /*
    * Updates the client data of an existing context. Does not lock database.
    */
    void DoUpdateContextClientDataL( CNssContext& aContext );

    /*
    * Updates RRD data. Does not lock database.
    */
	void UpdateRRDL( CNssRRD& aRRD );

    /*
    * Updates speech item. Does not lock database.
    */
	void UpdateSpeechItemL( CNssSpeechItem& aSpeechItem );

    /*
    * Gets a tag list from view. A mid-phase between GetTagListByQuery and
    * the lower functions (those which get RRD and speech item).
    */
    void GetTagListFromViewL( RDbView& aView, 
                              CArrayPtrFlat<CNssTag>* aTagList,
                              CNssContext* aContext );

    /*
    * Locks the database with Begin().
    */
    TInt StartTransaction();

    /*
    * Commits a transaction.
    * If the client has locked the database (Lock()), keeps the database locked
    * by calling Begin() immediately again.
    * @param "TBool aCompact" ETrue if compact should be called
    */
    TInt CommitTransaction( TBool aCompact );

    /*
    * Rolls back a transaction.
    * If the client has locked the database (Lock()), keeps the database locked
    * by calling Begin() immediately again.
    */
    TInt RollbackTransaction();
		
    /*
    * Commits or rolls back according to success status, which is one of the
    * symbian-wide error codes.
    * @param aCompactIfSuccess If ETrue and commit is going to happen, compacts 
    *        the DB after the Commit
    */
    void CommitIfSuccess( RDbDatabase aDatabase, TInt aSuccess, TBool aCompactIfCommit = EFalse );

    /*
    * This method is used to create a TCleanupOperation, which rolls back
    * transaction. This way, rollback can be pushed to the cleanup stack.
    */
    static void RollbackCleanupFunction( TAny* aArg );

    /*
    * Reserves disk space. Leaves (KErrDiskFull), if not
    * enough space.
    */
    void ReserveDiskSpaceL( TInt aRequestSize );

    /*
    * Release the previously reserved disk space
    */
    void ReleaseDiskSpace();
    
    /**
    * Updates the information of an existing context inside already opened transaction.
    * @since 3.0
    * @param aContext The context
    * @return none
    */
	void UpdateContextInsideTransactionL( CNssContext& aContext );
	
	/**
	* Creates a cleanup item and pushes it to the cleanup stack. In case of a 
	* leave, PopAndDestroying this item will call RollbackCleanupFunction
	*/
	void CreatePushRollbackItemLC();
	
private:
	// Data

    // DB session
	RDbs					iDbSession;

    // DB
	RDbNamedDatabase		iDatabase;

    // Have we yet opened the DB file
	TBool					iClientHasOpenedDatabase;

    // Used to check drive space
    TInt iDrive;

    // Used to store an SQL query.
	TBuf<KNssMaxSQLLength>		iSQLStatement;

    // Used to create the database
	TNssVasDbCreator           iDbcreator;

    // Is the databas locked to us
    TBool                   iLocked;

    // Should the database be relocked after commit
    // to ensure it stays locked for us
    TBool                   iShouldBeLocked;

    // Mutex which makes sure that multiple processes don't get the exclusive
    // write lock at the same time
    RMutex iMutex;
    
    // Critical section is used in locking transactions inside one object
    RCriticalSection iCriticalSection;
    
    // backup and restore observer
    CNssVasBackupObserver* iBackupObserver;
    
    // For creating empty CNssContext-objects
    CNssContextBuilder&          iContextBuilder;
    
    // For creating empty CNssSpeechItem-objects
    CNssSpeechItemBuilder&       iSpeechItemBuilder;
    };

#endif // __CVASDB_H

