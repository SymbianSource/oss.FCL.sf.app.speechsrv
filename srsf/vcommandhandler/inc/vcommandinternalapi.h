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
* Description:  Voice command service interfaces, that are private for the
*              : vcommand API implementation
*
*/



#ifndef VCOMMANDINTERNALAPI_H
#define VCOMMANDINTERNALAPI_H

#include <e32def.h>
#include <e32cmn.h>
#include <s32strm.h>
#include <vcommandapi.h>

// context for voice commands tags
_LIT( KVoiceCommandContext, "COMMAND" );	
    
/**
* VCommand with a link to the VAS. This link is a command id
* Stored VCommand can be played back
*/
class CStoredVCommand : public CVCommand
    {
    private:
        // suppress inherited constructors by making them private
        /**
        * Factory function. Create a const snapshop of exe-ui-vastext collection
        * @param aText Text to be trained by VAS. Can be of any length, however
        *        only first KNssVasDbTagName characters will be trained by VAS
        * @see KNssVasDbTagName in nssvasdbkonsts.h
        * @param aRunnable Executable to be fired when the command is recognized. 
        *        VCommand takes the ownership on the aRunnable
        * @param aUi visible strings to be displayed in VC App. VCommand takes 
        *        the ownership on the aUi
        */
        static CVCommand* NewL( const TDesC& aText, const CVCRunnable& aRunnable, 
                                         const CVCCommandUi& aUi );
        
		/** 
        * Internalizes the command from stream 
        * @leave KErrNotSupported if the stream data format is unsupported
        *        e.g. if it has been written by newer implementation
        */
        static CVCommand* NewL( RReadStream &aStream );
        
    public:        
        // constructors with the commandId
        /**
        * Factory function. Create a const snapshop of exe-ui-vastext collection
        * @param aText Text to be trained by VAS. Can be of any length, however
        *        only first KNssVasDbTagName characters will be trained by VAS
        * @see KNssVasDbTagName in nssvasdbkonsts.h
        * @param aRunnable Executable to be fired when the command is recognized. 
        *        VCommand takes the ownership on the aRunnable
        * @param aUi visible strings to be displayed in VC App. VCommand takes 
        *        the ownership on the aUi
        */
        static CStoredVCommand* NewL( const TDesC& aText, const CVCRunnable& aRunnable, 
                                         const CVCCommandUi& aUi, TInt aCommandId );
        
		/** 
        * Internalizes the command from stream 
        * @leave KErrNotSupported if the stream data format is unsupported
        *        e.g. if it has been written by newer implementation
        */
        static CStoredVCommand* NewL( RReadStream &aStream, TInt aCommandId );
        
        /**
        * @return CommandId
        */
        TInt CommandId() const;
        
        /**
        * Attempts to play back the text expected to be recognized. 
        * To be playable command has to be added to CVCommandHandler AND
        * then retrieved back
        *
        * @param aHandler CVCommandHandler where the command is stored
        * @todo Consider storing link to CVCommandHandler within the CStoredVCommand
        *       Pros: No clumsy aHandler argument for the playback
        *       Pros: No need to remember in which handler the command is stored
        *             and why specifying storage is needed during the playback
        *       Cons: In case of internal link the linked handler should still
        *             exist. It cannot be e.g. destroyed and recreated later even
        *             if the latter one is using the same VAS
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
        */
        void PlaySpokenTextL( const CVCommandHandler& aHandler, 
                                     MNssPlayEventHandler& aPlayEventHandler ) const;
        
        /**
        * Plays back the user-specified alternative spoken text. 
        * Otherwise is identical to PlaySpokenTextL
        * @see PlaySpokenTextL
        * @leave KErrNotFound if this command cannot be found in aHandler of if 
        *        it doesn't have a user-specified text
        */                                      
        void PlayAlternativeSpokenTextL( const CVCommandHandler& aHandler, 
                                     MNssPlayEventHandler& aPlayEventHandler ) const;
                                     
        /**
        * Cancels playback of a spoken or alternative spoken text
        * To be playable command has to be added to CVCommandHandler AND
        * then retrieved back. After this function neither HandlePlayStarted,
        * nor HandlePlayComplete will be called
        *
        * @param aHandler CVCommandHandler where the command is stored
        *
        * @leave KErrBadHandle if the current command has not been retrieved 
        *        from CVCommandHandler (i.e. was not trained for recognition)
        * @leave KErrNotReady if playback has never been started
        *
        */                                   
        void CancelPlaybackL( const CVCommandHandler& aHandler ) const;
        
    protected:
    	/**
    	* @see CVCommand::ConstructL
    	*/ 
        void ConstructL( const TDesC& aText, const CVCRunnable& aRunnable, 
                         const CVCCommandUi& aUi, TInt aCommandId );
        /**
        * @see CVCommand::ConstructL
        */
        void ConstructL( RReadStream& aStream, TInt aCommandId );
        
        /**
        * Clone itself
        * Is used for copying via NewL( CVCommand& aOriginal )
        */
        CVCommand* CloneL() const; 
                
    private:
        TInt iCommandId;
    };


/**
* Interface for storing, listing and removing voice commands to/from VAS
*/
class MVCService 
    {
    public:
        /**
        * Synchronous. Service doesn't take the ownership, but makes an own copy
        * Duplicates can be added
        * @todo should we check for duplicates and leave with KErrAlreadyExists?
        */
        virtual void AddCommandL( const CVCommand& aCommand ) = 0;
        
        /**
        * Synchronous. Service doesn't take the ownership, but makes an own copy
        * Duplicates can be added
        * @todo Should we check for duplicates and leave with KErrAlreadyExists?
        * @param aIgnoreErrors If ETrue, even if some commands fail to be trained,
        *        handler adds all that are trainable
        */
        virtual void AddCommandsL( const RVCommandArray& aCommands, 
                                   TBool aIgnoreErrors = EFalse ) = 0;
        
        /**
        * Synchronous. Removes the command from the system
        * @param aCommand Reference to the command to be removed. Existing commands are
        *		 compared against aCommand. All the matches are removed from VAS
        * @leave KErrNotFound No such command
        */
        virtual void RemoveCommandL( const CVCommand& aCommand ) = 0;

        /**
        * Synchronous. 
        * @param aCommands Reference to the list of commands to be removed. Existing commands are
        *		 compared against aCommands items. All the matches are removed from VAS
        * @param aIgnoreErrors If ETrue, even if some commands fail to be removed,
        *        handler will remove as many as possible
        */
        virtual void RemoveCommandsL( const RVCommandArray& aCommands, 
                                      TBool aIgnoreErrors = EFalse ) = 0;

        
        /**
        * Synchronous
        * @return an array of the commands in the system
        *         Ownership of the array is transfered to the client
        *         The returned CVCommandArray contains copies of all the 
        *         commands currently stored in this handler
        */
        virtual CVCommandArray* ListCommandsL() = 0;
        
        /**
        * This method is not intended to be called directly. 
        * Use CVCommand::PlaySpokenTextL instead
        * @see CVCommand::PlaySpokenTextL
        * @see CStoredVCommand::PlaySpokenTextL
        */
        virtual void PlaySpokenTextL( const CStoredVCommand& aCommand, 
                                      MNssPlayEventHandler& aPlayEventHandler ) = 0;
                                      
        /**
        * This method is not intended to be called directly. 
        * Use CVCommand::PlayAlternativeSpokenTextL instead
        * @see CVCommand::PlayAlternativeSpokenTextL 
        * @see CStoredVCommand::PlayAlternativeSpokenTextL 
        */
        virtual void PlayAlternativeSpokenTextL( const CStoredVCommand& aCommand, 
                                      MNssPlayEventHandler& aPlayEventHandler ) = 0;
                                      
        /**
        * This method is not intended to be called directly. 
        * Use CVCommand::CancelPlaybackL instead
        * @see CVCommand::CancelPlaybackL
        * @see CStoredVCommand::CancelPlaybackL
        */
        virtual void CancelPlaybackL( const CStoredVCommand& aCommand ) = 0;
        
        virtual ~MVCService() {};
    };
    
#endif // VCOMMANDINTERNALAPI_H