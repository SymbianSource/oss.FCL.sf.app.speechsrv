/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssVASDBMgr is the starting point to begin interaction with VAS. 
*               The client must first create and Initialize the CNssVASDBMgr; then 
*               request the ContextMgr, TagMgr, VASDBEventNotifier, or 
*               TagSelectNotifier to begin using VAS services. 
*
*/


#ifndef CNSSVASDBMGR_H
#define CNSSVASDBMGR_H


// includes
#include <e32def.h>
#include <e32std.h>
#include <e32base.h>

#include "nssvascoreconstant.h"
#include "nssvasmcontextmgr.h"
#include "nssvasmtagmgr.h"
#include "nssvasmvasdbeventnotifier.h"

//FORWARD DECLARATIONS
class CNssVASDBBuilder;
class CNssVASDBEventNotifier;
class CNssDBTagSelectNotifier;
class MNssResetFactoryModelsClient;

// CLASS DECLARATIONS
/**
*  CNssVASDBMgr is the entry point for VAS services. 
*  It provides methods for creation of managers that
*  provide the pertinent services.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssVASDBMgr : public CBase
{
public:
   /**
   * 2 phase constructor
   * @param 
   * @return pointer to created CNssVASDBMgr
   */
   IMPORT_C static CNssVASDBMgr* NewL ();

   /**
   * 2 phase constructor
   * @param 
   * @return pointer to created CNssVASDBMgr
   */
   IMPORT_C static CNssVASDBMgr* NewLC ();
   
	/**
   * Destructor.
   * @param 
   * @return
   */	
   ~CNssVASDBMgr();
   
#ifdef __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY   
	/**
   * Initializes VAS DB class attributes.
   * @since 3.2
   */
   IMPORT_C void InitializeL(); 
#else
	/**
   * Initializes VAS DB class attributes.
   * @since 2.0
   * @return Error code
   */
   IMPORT_C TInt InitializeL();
#endif // __SIND_RD_BREAK_PHONEBOOK_COMPATIBILITY 
   
   /**
   * Gets the Context Manager.
   * @since 2.0
   * @param None
   * @return pointer to MNssContextMgr
   */	
   IMPORT_C MNssContextMgr* GetContextMgr();
   
   /**
   * Gets the Tag Manager.
   * @since 2.0
   * @param None
   * @return pointer to MNssTagMgr
   */	
   IMPORT_C MNssTagMgr* GetTagMgr();
   
   /**
   * Gets the Vas Db event notifier.
   * @since 2.0
   * @param None
   * @return pointer to MNssVASDBEventNotifier
   */    
   IMPORT_C MNssVASDBEventNotifier* GetVASDBEventNotifier();

	/**
   * Gets the tag select notifier.
   * @since 2.0
   * @param None
   * @return pointer to MNssVASDBEventNotifier
   */    
   IMPORT_C CNssDBTagSelectNotifier* GetTagSelectNotifier();

   /**
   * Reset speaker-adapted models to speaker independent ones.
   * @since 2.0
   * @param None
   * @return None
   */
   IMPORT_C void ResetFactoryModelsL( MNssResetFactoryModelsClient* aClient );

private:

	/**
   * C++ constructor
   * @param 
   * @return 
   */    
	CNssVASDBMgr();

	/**
   * EPOC constructor
   * @param 
   * @return 
   */    
	void ConstructL ();

private:
	// owns the VAS DB Builder
	CNssVASDBBuilder *iVasDBBuilder;


};
#endif
