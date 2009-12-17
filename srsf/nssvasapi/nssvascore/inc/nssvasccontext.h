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
* Description:  CContext class provides context processing. It implements 
*               MNssContext interface.
*
*/


#ifndef NSSVASCCONTEXT_H
#define NSSVASCCONTEXT_H

//  INCLUDES
#include <nsssispeechrecognitiondatacommon.h>
#include "nssvascoreconstant.h"
#include "nssvasmcontext.h"
#include "nssvasccontextsrsportal.h"
#include "nssvasmcoresrsdbeventhandler.h"
#include "nssvascoreconstant.h"


class CNssContextSrsPortal;

// CLASS DECLARATION

/**
*  CContext is the implementation class for MNssContext. 
*  It encapsulates context data. A context cannot be directly created by a 
*  client. A client has to own an instance of Context Manager which has methods 
*  for creating context objects. The CContext class also provides additional 
*  members for internal use by NssVAS.
*  @lib NssVASApi.lib
*  @since 2.0
*/

class CNssContext: public CBase, public MNssContext
{
public:
    /**
    * C++ constructor.
    */
	CNssContext(CNssContextSrsPortal* aContextSrsPortal);

    /**
    * Destructor.
    */
	~CNssContext();

public: // functions for internal VAS use

    /**
    * Get the context id
	* for internal use
	* @since 2.0
    * @param 
    * @return context id
    */
	TInt ContextId() const;

    /**
    * Get the grammar id
	* for internal use
	* @since 2.0
    * @param 
    * @return grammar id
    */
	TUint32 GrammarId() const;

	/**
    * Get the lexicon id
	* for internal use
	* @since 2.0
    * @param 
    * @return lexicon id
    */
	TUint32 LexiconId() const;

	/**
    * Get the model bank id
	* for internal use
	* @since 2.0
    * @param 
    * @return model bank id
    */
	TUint32 ModelBankId() const;

	/**
    * Get the recognition mode
	* for internal use
	* @since 2.8
    * @param none
    * @return recognition technology (SI / SD)
    */
	TNSSRecognitionMode RecognitionMode() const;

    /**
    * Set the context id 
	* for internal use
	* @since 2.0
    * @param aContextId - the context id 
    * @return 
    */
	void SetContextId(TInt aContextId);

	/**
    * Set the grammar id 
	* for internal use
	* @since 2.0
    * @param aGrammarId - the grammar id
    * @return 
    */
	void SetGrammarId(TUint32 aGrammarId);

	/**
    * Set the lexicon id
	* for internal use
	* @since 2.0
    * @param aLexiconId - the lexicon id
    * @return id
    */
	void SetLexiconId(TUint32 aLexiconId);

	/**
    * Set the model bank id
	* for internal use
	* @since 2.0
    * @param aModelBankId - the model bank id
    * @return id
    */
	void SetModelBankId(TUint32 aModelBankId);

    /**
    * Set the speech technology (Speaker independent or dependent)
    * for internal use
    * @since 2.0
    * @param aRecognitionMode - the recognition mode
    * @return none
    */
    void SetRecognitionMode(TNSSRecognitionMode aRecognitionMode);

	/**
    * Save the context to the SRS. 
	* for internal use
	* This method is used to save the context to the SRS.
	* This creates the model bank, 
	* lexicon, and grammar for a context. Model bank, and lexicon 
	* are created only for the first time, and are thus, fixed.
	* For every context, there is one grammar id. After, model bank,
	* lexicon, and grammar are created, the corresponding Ids are 
	* set in the context object.
	* @since 2.0
    * @param
    * @return none
    */
	void BeginSaveToSrsL(MNssCoreSrsDBEventHandler* aSrsDBEventHandler);

	/**
    * Delete the context from the SRS. 
	* for internal use
	* deletes grammar from SRS
	* @since 2.0
    * @param 
    * @return none
    */
	void BeginDeleteFromSrsL(MNssCoreSrsDBEventHandler* aSrsDBEventHandler);

	/**
    * Commit the context from the SRS. 
	* for internal use
	* Commit the changes from SRS
	* @since 2.0
    * @param 
    * @return 
    */
	TInt CommitSrsChanges();

	/**
    * Don't commit the context from the SRS. 
	* for internal use
	* Doesn't commit the changes from SRS
	* @since 2.8
    * @param 
    * @return 
    */
	TInt RollbackSrsChanges();

    /**
    * Set/Reset the iModelBankAndLexiconExist flag.
	* for internal use
	* @since 2.0
    * @param aVal, ETrue to set, EFalse to reset
    * @return 
    */
	void SetModelBankAndLexiconExist(TBool aVal);

	/**
    * Get the iModelBankAndLexiconExist flag
	* for internal use
	* @since 2.0
    * @param 
    * @return ETrue if set, else EFalse
    */
	TBool ModelBankAndLexiconExist();

	/**
    * overloaded assignment operator
	* for internal use
	* @since 2.0
    * @param reference to context
    * @return reference to the context
    */
	CNssContext& operator=(const CNssContext& aContext);

	/**
    * Create a copy of the context. 
	* for internal use
	* This function creates a new context by allocating memory for
	* a new context, and deep copying the members of the context on which
	* this fucntion is called. Old context and new context exist. Client need
	* to deallocate memory.
	* @since 2.0
    * @param 
    * @return pointer the newly created context.
    */
	CNssContext* CopyL();

    /**
    * Sets/Resets the client data. The data must be saved with
    * MNssContextMgr::SaveClientData after this call.
    * @since 2.8
    * @param aData At most 100 bytes of serialized client data.
    */
    void SetClientData(const TDesC8& aData);

    /**
    * Gets the client data.
    * @since 2.8
    * @return At most 100 bytes of data.
    */
    const TDesC8& ClientData();

public: // functions derived from MNssContext

   /**
   * Get the name of the context
   * @since 2.0
   * @param 
   * @return reference to name of context
   */  
   TDesC& ContextName();

	/**
   * Is the context global
   * @since 2.0
   * @param 
   * @return ETrue if context is global, EFalse otherwise
   */  
   TBool IsGlobal();

	/**
    * Sets the name of the context
	* @since 2.0
    * @param aName name of the context as a descriptor
    * @return 
    */  
    void SetNameL(const TDesC& aName);

   /**
   * Sets/Resets the global flag for the context
   * @since 2.0
   * @param aGlobal ETrue to set the context as global, EFalse as not global 
   * @return 
   */  
   void SetGlobal(TBool aGlobal);

private: // data

	// name of the context
	HBufC*				iName;

	// context id, used by the VAS DB
	TInt				iContextId;

	// global flag
	TBool				iGlobal;

	// grammar id
	TUint32		        iGrammarId;

	// lexicon id
	TUint32             iLexiconId;

	// model bank id
	TUint32             iModelBankId;

	// association to context srs portal 
	CNssContextSrsPortal*	iContextSrsPortal;
	
	// flag to denote if Model Bank and Lexicon already created
	TBool				iModelBankAndLexiconExist;

    // differentiates between SI and SD contexts
    TNSSRecognitionMode iRecognitionMode;

    // client payload data
    TBuf8<100>          iClientData;
};


#endif // NSSVASCCONTEXT  
            
// End of File
