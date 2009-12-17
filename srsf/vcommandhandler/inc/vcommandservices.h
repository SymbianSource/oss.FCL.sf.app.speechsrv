/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Engine, that links the VCommand application to VAS
*
*/


 
#ifndef VCOMMANDSERVICES_H
#define VCOMMANDSERVICES_H

#include <nssvasmrrd.h>
#include <nssvasctrainingparameters.h>
#include <vcommandapi.h>
#include <nssvasmgettagclient.h>  // For MNssTagListArray
#include <nssvasmplayeventhandler.h>
#include "vcommandinternalapi.h"
#include "intpropertywatcher.h"

// Forward declarations
class CNssVASDBMgr;
class MNssContextMgr;
class MNssTagMgr;
class MNssTag;
class CTagPlayer;

/**
* Key for sorting CArrayPtr<MNssTag> by the value of the
* first element of the RRD IntArray
*/
class TVCommandTagPtrArrayKey : public TKeyArrayFix
    {
    public:
        inline TVCommandTagPtrArrayKey( TKeyCmpNumeric aType)
            :TKeyArrayFix( 0, aType ) {}
            virtual TAny* At( TInt aIndex ) const;
        };

/**
* Engine, that links the VCommand application to VAS
* @todo isolate the MNssPlayEventHandler dependency
*/
class CVCommandService : public CBase, public MVCService, public MNssPlayEventHandler, 
                         private MIntChangeNotifiable
                        
    {
    public:
        // Construction/destruction
		/**
		 * @param aObserver Listener to be notified whenever the VCommand set is changed
         *	 by *another* instance of CVCommandHandler. I.e. lets the client know if
         *	 his understanding of the VCommand set is up to date
	     */
        static CVCommandService* NewL( MVCommandHandlerObserver* aObserver = NULL );
        
        virtual ~CVCommandService();

        // From the MVCService
        
        /**
        * Synchronous. Service doesn't take the ownership, but makes an own copy
        * Duplicates can be added
        * @todo Should we check for duplicates and leave with KErrAlreadyExists?
        * @leave KErrLocked if there is an ongoing operation started by the same thread
        */
        void AddCommandL( const CVCommand& aCommand );
        
        /**
        * Synchronous. Service doesn't take the ownership, but makes an own copy
        * Duplicates can be added
        * @todo Should we check for duplicates and leave with KErrAlreadyExists?
        * @param aIgnoreErrors If ETrue, even if some commands fail to be trained,
        *        handler adds all that are trainable
        * @leave KErrLocked if there is an ongoing operation started by the same thread
        */
        void AddCommandsL( const RVCommandArray& aCommands, TBool aIgnoreErrors = EFalse );
        
        /**
        * Synchronous. Removes the command from the system
        * @param aCommand Reference to the command to be removed. Existing commands are
        *		 compared against aCommand. All the matches are removed from VAS
        * @leave KErrNotFound No such command
        * @leave KErrLocked if there is an ongoing operation started by the same thread
        */
        void RemoveCommandL( const CVCommand& aCommand );
        
        /**
        * Synchronous. 
        * @param aCommands Reference to the list of commands to be removed. Existing commands are
        *		 compared against aCommands items. All the matches are removed from VAS
        * @param aIgnoreErrors If ETrue, even if some commands fail to be removed,
        *        handler will remove as many as possible
        * @leave KErrLocked if there is an ongoing operation started by the same thread
        */
        void RemoveCommandsL( const RVCommandArray& aCommands, TBool aIgnoreErrors = EFalse );

        /**
        * Synchronous
        * @leave KErrLocked if there is an ongoing operation started by the same thread
        * @return an array of the commands in the system
        *         Ownership of the array is transfered to the client
        *         The returned CVCommandArray contains copies of all the 
        *         commands currently stored in this handler
        */
        CVCommandArray* ListCommandsL();
        
        /**
        * Asynchronous
        * Attempts to play back the text expected to be recognized. 
        * To be playable command has to be added to CVCommandHandler AND
        * then retrieved back. This function is asynchronous
        *
        * @param aPlayEventHandler Entity that handles the playback callbacks
        * @see NssVasMPlayEventHandler.h
        *
        * @leave KErrBadHandle if the current command has not been retrieved 
        *        from CVCommandHandler (i.e. was not trained for recognition)
        * @leave KErrNotFound if this command cannot be found in aHandler
        * @leave KErrNotReady @see nssvasmspeechitem.h MNssSpeechItem::TNssSpeechItemResult 
        *                                              EVasUnexpectedRequest
        * @leave KErrInUse @see nssvasmspeechitem.h MNssSpeechItem::TNssSpeechItemResult 
        *                                              EVasInUse
        * @leave KErrArgument @see nssvasmspeechitem.h MNssSpeechItem::TNssSpeechItemResult 
        *                                              EVasInvalidParameter
        * @leave KErrGeneral @see nssvasmspeechitem.h MNssSpeechItem::TNssSpeechItemResult 
        *                                             EVasPlayFailed
        * @leave KErrLocked if there is an ongoing operation started by the same thread
        */
        void PlaySpokenTextL( const CStoredVCommand& aCommand, 
                                      MNssPlayEventHandler& aPlayEventHandler );
        
        /**
        * Asynchronous
        * Plays back the user-specified alternative spoken text. 
        * Otherwise is identical to PlaySpokenTextL
        * @see PlaySpokenTextL
        * @leave KErrNotFound if this command cannot be found in aHandler of if 
        *        it doesn't have a user-specified text
        * @leave KErrLocked if there is an ongoing operation started by the same thread
        */ 
        void PlayAlternativeSpokenTextL( const CStoredVCommand& aCommand, 
                                      MNssPlayEventHandler& aPlayEventHandler );
        
        /**
        * This method is not intended to be called directly. 
        * Use CVCommand::CancelPlaybackL instead
        * @see CVCommand::CancelPlaybackL
        * @see CStoredVCommand::CancelPlaybackL
        */
        void CancelPlaybackL( const CStoredVCommand& aCommand );
        
    public:
    
        // From MNssPlayEventHandler                                      
        /**
        * The HandlePlayStarted method is a virtual method implemented by the
        * client and is called when play is started
        * @param aDuration - the duration of the utterance data
        */       
        virtual void HandlePlayStarted( TTimeIntervalMicroSeconds32 aDuration );

        /**
        * The HandlePlayComplete method is a virtual method implemented by the
        * client and is called when play is completed
        * @param aErrorCode EVasErrorNone if playing was successfull
        */       
        virtual void HandlePlayComplete( TNssPlayResult aErrorCode );
                                      
    private:
        CVCommandService( MVCommandHandlerObserver* aObserver );
        
        void ConstructL();
        
        /**
        * Returns the VCommand context. Creates one on demand if none exists yet
        * @return VCommand context. Ownership is transfered to the client
        */
        MNssContext* GetVCommandContextLC() const;
        
        /**
        * Constructs a set of new VAS tags from a command and a context. 
        * Does not save to VAS, just creates. Ownership on the created tag is 
        * passed to the client
        */
        CArrayPtr<MNssTag>* CreateTagsLC2( const CVCommand& aCommand, 
                                           const MNssContext& aContext );
        
        /**
        * Constructs training related parameters. Like language to be used
        * @leave negated TNssSpeechItemResult
        * @todo Is it ok to mix system-wide codes with the TNssSpeechItemResult codes
        */
        CNssTrainingParameters* ConstructTrainingParametersLC() const;

        /**
		* Retrieves the list of tags for a given context. Synchronous.
		* Leaves two objects on the cleanup stack!
		* First PopAndDestroy will ResetAndDestroy content
		* Second one will destroy the MNsstagListArray itself
		*/
        MNssTagListArray* GetTagListLC2( const MNssContext& aContext ) const;
        
        /**
		* Retrieves the list of tags for a given context and voice command id. 
		* Synchronous. Leaves two objects on the cleanup stack!
		* First PopAndDestroy will ResetAndDestroy content
		* Second one will destroy the MNsstagListArray itself
		*/
        MNssTagListArray* GetTagListLC2( const MNssContext& aContext, TInt aCommandId ) const;
        
        /**
        * Resets iCommands and fills them with the commands from VAS
        */
        void RefreshCommandListL();
        
        /**
        * Marks the iCommands as an invalid reflection of the VAS content.
        * To make iCommands reflect the real VAS content a call to 
        * RefreshCommandsL is needed
        */
        void InvalidateCacheL();
        
        /**
        * Marks the iCommands as the valid reflection of the VAS content.
        */
        void MarkCacheValidL();
        
        /**
        * Tells if the iCommands reflect the content of the VAS DB correctly
        * @return ETrue if the cache is valid, EFalse otherwise
        */
        TBool IsCacheValidL();
        
        /**
        * Generates new command id to be used for identifying voice tags, that
        * belong to the same VCommand
        */
        TInt NewCommandIdL();
        
        /** 
        * Sets the given aId as a VCommand id for all the tags in the given list
        * This id is set as a first RRD int element of all the given MNssTags.
        * If the RRD int array has no elements yet, one element is added
        */
        void SetVCommandIdL( CArrayPtr<MNssTag>& aTags, TInt aId ) const;
        
        /**
        * Searches vcommand tags for the tag, that corresponds to the given
        * commandId and is trained against the given text
        * @param aCommandId Command id to search for
        * @param aText
        * @return ANY of the voice tags corresponding to the given aCommandId
        *         and aText. Typically there will be one such tag only. However,
        *         if there are several it is not specified which one will be 
        *         returned
        * @return NULL if no satisfying tag is found
        */
        MNssTag* TagByCommandIdTextL( TInt aCommandId, const TDesC& aText );
        
        /**
        * Synchronous. Service doesn't take the ownership, but makes an own copy
        * Duplicates can be added
        * Is not the atomic operation
        * @see AddCommandsL
        */
        void DoAddCommandsL( const RVCommandArray& aCommands, 
                             TBool aIgnoreErrors = EFalse );

		/**
		 * Synchronous
		 * @see RemoveCommandsL
		 */                             
        void DoRemoveCommandsL( const RVCommandArray& aCommands,
        						TBool aIgnoreErrors = EFalse );
        
        /**
        * Starts the section of the code, that can be simultaneously executed
        * only by a single instance of this class. Other instances have to wait
        * Same as starting the Java "synchronized" method. Puts the function
        * EndAtomicOperation to cleanup stack to make the use leave safe. Subsequent
        * call to CleanupStack::PopAndDestroy will run EndAtomicOperation.
        */
        void StartAtomicOperationLC();
        
        /**
        * Ends the section of the code, that can be simultaneously executed
        * only by a single instance of this class. Other instances have to wait
        * Same as exiting the Java "synchronized" method
        * @leave KErrNotReady if atomic operation hasn't been started
        */
        void EndAtomicOperation();
        
        /**
        * Runs the EndAtomicOperation when calling CleanupStack::PopAndDestroy after
        * a call to StartAtomicOperationLC
        */ 
        static void CleanupEndAtomicOperation( TAny* aService );
        
        /**
        * Creates and pushes to cleanup stack a TCleanupItem that calls
        * EndAtomicOperation when CleanupStack::PopAndDestroy is called
        */
        void CreateAndPushEndAtomicCleanupItemL();
        
    private:
        // From MIntChangeNotifiable
        /**
         * Is called by the P&S property watcher to let this object know that
         * the global tick property has been changed
         */
        void IntValueChanged( TInt aNewValue );
        
    private:
        // Main access point to VAS
        CNssVASDBMgr*               iVasDbManager;      // Owned
        MNssTagMgr*                 iTagManager;        // Not owned.
        
        // Stores the local tick count. When local tick count
        // is equal to the global tick count, this object is aware
        // of all the VAS updates
        TInt						iLocalTickCount;
        
        // Commands in the system
        RVCommandArray              iCommands;        
        
        // Maximal used command id
        TInt                        iMaxCommandId;
        
        // Utility that plays back the voice commands
        CTagPlayer*                 iTagPlayer;
        
        RMutex                      iLock;
        
        // Client that should be notified about the playback events
        MNssPlayEventHandler*       iPlaybackHandler;
        
        // Is used to check the global "tick count", i.e.
        // number of operations performed by this
        // and another instances of VCommand handler
        RProperty                   iGlobalTickProperty;
        
        MVCommandHandlerObserver*	iObserver;
        
        // Watches for the global tick property to let the services observer
        // know when the local cache is out of date
        CIntPropertyWatcher*        iGlobalTickWatcher;
        
        // Last time when this instance set the global tick count
        // it was this value.
        // Is used to distinguish own global tick changes from external global tick changes
        TInt                        iLastSetGlobalTickCount;
    };

#endif // VCOMMANDSERVICES_H