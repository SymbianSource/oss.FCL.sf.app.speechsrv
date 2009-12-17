/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssVasDbBuilder is owned by CNssVASDBMgr. It is responsible for creating
*               the context manaher, tag manager, vas db event notifier, and tag select 
*               notifier. This class is a singleton, and is created using the InstanceL(),
*               and deleted using the RemoveInstance() methods. 
*
*/


#ifndef CNSSVASDBBUILDER_H
#define CNSSVASDBBUILDER_H

#include "nssvascvasdatabase.h"
#include "nssvasccontextmgr.h"
#include "nssvasctagmgr.h"
#include "nssvascvasdbeventnotifier.h"
#include "nssvascdbtagselectnotifier.h"
#include "nssvascvasdbsrvdef.h" 


//FORWARD DECLARATIONS

// CLASS DECLARATIONS
/**
*  Singleton builder class for creating the VAS DB objects.
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssVASDBBuilder : public CBase
{
public:
   /**
   * This function returns an instance of CNssVASDBBuilder.
   * Must be called one to one with RemoveInstance(). 
   * @since 2.0
   * @param
   * @return pointer to CNssVASDBBuilder
   */
	static CNssVASDBBuilder* InstanceL();

	/**
   * This method removes one instance of this class by decrementing
   * the number of instances. When the number of instances hit 0,
   * the object is deleted. Must be called one to one with InstanceL().
   * @since 2.0
   * @param
   * @return
   */
	static void RemoveInstance();
   
	/**
   * Destructor.
   * @param
   * @return
   */	
	~CNssVASDBBuilder();
   
   /**
   * Initializes VAS DB 
   * Only one initialization per object lifetime is doing a real job
   * All the subsequent calls to InitializeL are ignored
   * @since 2.0
   * @param 
   * @return none 
   */
   void InitializeL();

   /**
   * Gets the ContextMgr.
   * @since 2.0
   * @param 
   * @return pointer to CNssContextMgr
   */	
   CNssContextMgr* GetContextMgr();
   
   /**
   * Gets the TagMgr.
   * @since 2.0
   * @param 
   * @return pointer to CNssTagMgr
   */	
   CNssTagMgr* GetTagMgr();
   
   /**
   * Gets the Vas Db event notifier.
   * @since 2.0
   * @param 
   * @return pointer to CNssVASDBEventNotifier
   */    
   CNssVASDBEventNotifier* GetVASDBEventNotifier();


   /**
   * Gets the Vas Db event notifier.
   * @since 2.0
   * @param 
   * @return pointer to CNssVASDBEventNotifier
   */    
   CNssDBTagSelectNotifier* GetTagSelectNotifier();


private:
	/**
   * C++ constructor
   * @param 
   * @return 
   */    
   CNssVASDBBuilder();

	/**
   * 2 phase construction
   * @param
   * @return
   */
	static CNssVASDBBuilder* NewL ();

	/**
   * 2 phase construction
   * @param
   * @return
   */
	static CNssVASDBBuilder* NewLC ();

   /**
   * EPOC constructor
   * @param 
   * @return 
   */    
   void ConstructL ();

	/**
   * closes the VAS database
   * @since 2.0
   * @param -
   * @return 
   */    
   void Close();

   // owns the VAS Database
   CNssVASDatabase*			iVasDatabase;
   
   // owns the context manager
   CNssContextMgr*				iContextMgr;

   // owns the tag manager
   CNssTagMgr*					iTagMgr;

   // owns the tag select notifier
   CNssDBTagSelectNotifier*	iDBTagSelectNotifier;

   // owns the event notifier
   CNssVASDBEventNotifier*		iVASDBEventNotifier;
   
   // Only one initialization per lifetime is permitted
   // All the subsequent calls to InitializeL are ignored
   TBool					iInitialized;
   
};

// structure for singleton implementation
// 
struct TThreadLocalStorage 
{
	CNssVASDBBuilder* iInstance;
    TInt16 iInstanceCount;
};

#endif // __CVASDBBUILDER_H
