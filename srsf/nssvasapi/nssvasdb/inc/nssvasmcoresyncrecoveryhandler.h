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
* Description:  The MCoreDbSyncRecoveryHandler provides means to restore the database
*                synchronisation between Speech Recognition DB (SR DB) and VAS DB.
*
*                Training a new name saves stuff to SR DB. The SaveTag() call saves
*                stuff to VAS DB. If the client decides after training not to save, or
*                battery is removed between training the name and saving it, then the
*                two databases are out of sync.
*                These functions add to VAS DB enough information for proper rollback.
*                If they are called before changing SR DB, the databases will always
*                stay in sync.
*
*
*/


#ifndef MNSSCORERECOVERYHANDLER_H
#define MNSSCORERECOVERYHANDLER_H

class CNssTag;

/**
* The MNssCoreSrsDBEventHandler provides interface to handle events
* from Save and Delete utterance data into SRS.
*
*  @lib NssVasApi.lib
*  @since 2.0
*/
class MNssCoreSyncRecoveryHandler
{
public:

   /**
   * Recovery functions.
   * These are called, before changing the speech recognition database.
   * They save enough information to clean up and fall back, if:
   * (a) The client changes SR DB, but doesn't save the tag.
   * (b) A crash happens during operation.
   */

   /**
   * Callback
   * @return None
   */
   virtual void PrepareRecoveryInTraining( const RPointerArray<CNssSpeechItem>& anArray,
                                           const CNssContext* aContext )=0;

   /**
   * Callback
   * @return None
   */
   virtual void PrepareRecoveryInChanging(const RPointerArray<CNssSpeechItem>& anArray)=0;

   /**
   * Callback
   * @return None
   */
   virtual void PrepareRecoveryInDeleting(const RPointerArray<CNssSpeechItem>& anArray)=0;


};

#endif
