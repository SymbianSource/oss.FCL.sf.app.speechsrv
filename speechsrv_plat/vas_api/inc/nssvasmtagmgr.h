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
* Description:  MNssTagMgr provides services for tag management. Clients obtain a
*				 tag manager from the VAS DB Manager. VAS DB Manager owns the tag 
*				 manager and is responsible for deleting it when the VAS DB manager is
*				 deleted. Clienst should not directly delete the tag manager. Through
*				 the tag manager, a client can create a tag, save it to the VAS DB, 
*				 retreive a tag (or tag list) from the VAS DB, and delete tag from VAS DB.
*
*/


#ifndef MNSSTAGMGR_H
#define MNSSTAGMGR_H

// INCLUDES
#include <e32std.h>
#include <nssvascoreconstant.h>


//FORWARD DECLARATIONS
class MNssGetTagClient;
class MNssDeleteTagClient;
class MNssSaveTagClient;
class MNssTag;
class MNssRRD;
class MNssContext;
class MNssGetTagReferenceClient;
class MNssTagReference;

// CLASS DECLARATIONS
/**
*  MNssTagMgr is the interface class for tag management. 
*  @lib NssVASApi.lib
*  @since 2.0
*/
class MNssTagMgr
{
public:
   /**
   * Creates a new tag. A context needs to be created prior to creating a tag.
   * @since 2.0
   * @param aContext - context for the tag
   * @return newly created tag pointer
   */
   virtual MNssTag* CreateTagL(MNssContext* aContext)=0;
   
   /**
   * Gets tag list associated with a context. Asynchronous.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aContext - context for which to get the tags
   * @return synchronous return error code
   */   
   virtual TInt GetTagList(MNssGetTagClient* aTagClient, MNssContext* aContext)=0;

   /**
   * Gets tag list with a given name. Asynchronous.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aContext - context for which to get the tags
   * @param aName - name for which to get the tags
   * @return synchronous return error code
   */   
   virtual TInt GetTagList( MNssGetTagClient* aTagClient, MNssContext* aContext,
                            const TDesC& aName)=0;
      
   /**
   * Delete tag from VAS DB. Asynchronous.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTag - tag to delete
   * @return synchronous return error code
   */   
   virtual TInt DeleteTag(MNssDeleteTagClient* aTagClient,MNssTag* aTag)=0;

   /**
   * Save tag to VAS DB. Asynchronous.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTag - tag to save
   * @return synchronous return error code
   */   
   virtual TInt SaveTag(MNssSaveTagClient* aTagClient,MNssTag* aTag)=0;
   
   /**
   * Get tag count from VAS DB. Synchronous.
   * @since 2.0
   * @param aContext - context for which to get tag count.
   * @return synchronous return error code or tagCount
   */   
   virtual TInt TagCount(MNssContext* aContext)=0;

   /**
   * Get the tag(s) based on partial rrd int data. Asynchronous.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTInt - a rrd int value
   * @param aPosition - the rrd int value position in the rrd intarray 
   * @return asynchronous return error code
   */
   virtual TInt GetTagList( MNssGetTagClient* aTagClient,MNssContext* aContext,
                            TInt aNum,TInt aPosition )=0;
   /**
   * Get the tag(s) based on partial rrd text data. Asynchronous.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTInt - a rrd text value
   * @param aPosition - the rrd text value position in the rrd textarray
   * @return asynchronous return error code
   */
   virtual TInt GetTagList( MNssGetTagClient* aTagClient,MNssContext* aContext,
                            TDesC& aText,TInt aPosition )=0;
                            
   /**
   * Get a list of tag references based on context. Asynchronous.
   * @since 2.0
   * @param aTagReferenceClient - call back address
   * @param aContext - a context
   * @return asynchronous return error code
   */
   virtual TInt GetTagReferenceList( MNssGetTagReferenceClient* aTagRefClient,
                                     MNssContext* aContext )=0;
   
   /**
   * Get a tag based on tagreference. Asynchronous.
   * @since 2.0
   * @param aTagClient - call back address
   * @param aTagReference - a tagreferenc
   * @return asynchronous return error code
   */
   virtual TInt GetTag(MNssGetTagClient* aTagClient,MNssTagReference* aTagReference)=0;
};
#endif
