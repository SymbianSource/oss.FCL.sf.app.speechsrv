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
* Description:  Voice command service interfaces
*
*/



#ifndef VCOMMANDAPI_H
#define VCOMMANDAPI_H

#include <e32def.h>
#include <e32cmn.h>
#include <s32strm.h>
#include <nssvasmrrd.h>  // For KNssVASMAXRRDTextLength only

/** 
* Maximal length of the exe arguments string
* @todo If longer arguments needed, reengineer the storage scheme. 
*       Currently argument string has to fit into the VAS DB rrdtext column
*       @see CTagCommandConverter
*/
const TInt KMaxVCArgumentLength = KNssVASMAXRRDTextLength;

class MNssPlayEventHandler;
class CStoredVCommand;
class MVCService;
class CVCommandHandler;
class CGulIcon;

/**
 * Container for the voice command folder information. Note that every command can
 * have its own folder information
 */
class CVCFolderInfo : public CBase
    {
    public:
        /**
         * This object does not take an ownership of the passed descriptors, 
         * but makes own copies. 
         * 
         * @param aTitle Folder title. It is shown at the top of the screen,
         *        when the folder is opened in the VCommand application
         * @param aListedName Version of a title shown when the folder is
         *        is displayed in the list of VCommands in the VCommand app
         * @param aHelpTopicId Topic to open when help is requested for the 
         *        given folder
         * @param aIconIndex Zero based index of the folder icon in the folder icons mbm file
         *        each index correspods to two images - the actual icon and its mask
         * @param aIconFile Mbm file where the icons are obtained from. If KNullDesC, 
         * 		  the default icon file is used
         * 
         */
        IMPORT_C static CVCFolderInfo* NewL( const TDesC& aTitle,
                const TDesC& aListedName, TUint32 aHelpTopicId, TUint aIconIndex,
                const TDesC& aIconFile = KNullDesC );
                
        /**
         * Constructs the folder information from stream
         * @leave KErrNotSupported if the stream data format is unsupported
         */
        IMPORT_C static CVCFolderInfo* NewL( RReadStream& aStream );
        
        /**
         * Cloning constructor
         */
        IMPORT_C static CVCFolderInfo* NewL( const CVCFolderInfo& aOriginal );
        
        IMPORT_C ~CVCFolderInfo();
        
        /** 
        * Saves the folder information to stream.
        * Descriptor components are saved as <length><descriptor> pairs, where 
        * <length> is TInt32 and <descriptor> is the default descriptor represetation
        */
        IMPORT_C void ExternalizeL( RWriteStream &aStream ) const;
        
        /**
         * Returns folder title. It is shown at the top of the screen,
         *        when the folder is opened in the VCommand application
         */
        IMPORT_C const TDesC& Title() const;
        
        /**
         * Version of a title shown when the folder is
         * is displayed in the list of VCommands in the VCommand app
         */
        IMPORT_C const TDesC& ListedName() const;
        
        IMPORT_C TUint32 HelpTopicId() const;
        
	/**
        * Creates an icon to represent this folder. Works only if CEikonEnv is available
	* @return Icon for the folder
	*/
	IMPORT_C CGulIcon* IconLC() const;
        
        IMPORT_C TBool operator==( const CVCFolderInfo& aFolderInfo ) const;
    
    protected:
        CVCFolderInfo( TUint32 aHelpTopicId, TUint aIconIndex );
        // unhiding inherited parameterless constructor
        CVCFolderInfo() {}
        
        /**
         * Zero based index of the folder icon in the icon file
         * @see IconFile
         */
        TUint IconIndex() const;
        
        /**
         * Full file name of the file name, where the folder icon is obtained from
         * Is always no longer, than KMaxFileName
         * If KNullDesC is returned, the default icon file is used
         */
        TDesC& IconFile() const;

    private:
        /** @see NewL */
        void ConstructL( const TDesC& aTitle, 
                         const TDesC& aListedName,
                         const TDesC& aIconFile );
                
        void ConstructL( RReadStream& aStream );
        
    private:
        HBufC* iTitle;
        
        HBufC* iListedName;
        
        HBufC* iIconFile;
        
        TUint32 iHelpTopicId;
        
        TUint iIconIndex;
        
    };

/** 
* Container for VC app specific "human-visible" parts of the voice command:
* tooltip, displayable text, folder, icon, etc
*/ 
class CVCCommandUi : public CBase
    {
    public:
        /**
        * This object does not take an ownership of the passed descriptors, 
        * but makes own copies. 
        * 
        * @param aWrittenText Text to display
        * @param aTooltip Second line of the text if ant
        * @param aFolderInfo Folder-related details
        * @param aModifiable If user can modify the command
        * @param aConfirmationNeeded If EFalse, when the command is recognized,
        *        it is immediately executed
        *        If ETrue, when the command is recognized, it is played back and
        *        user is asked to confirm the recognition results
        * @see
        * @todo Add "see" reference to the recognition-side function that uses the
        *       "confirmation needed information"
        * @todo aModifiable param is "hanging in the air" and never actually used
        *       Clarify its existence and responsibilities
        * @param aUserText Optional user-specified shortcut for the command
        */
        IMPORT_C static CVCCommandUi* NewL( const TDesC& aWrittenText, 
                const CVCFolderInfo& aFolderInfo, TBool aModifiable, 
                const TDesC& aTooltip, const TUid& aIconUid, 
                const TDesC& aUserText = KNullDesC,
                TBool aConfirmationNeeded = ETrue );
                
        /**
        * Factory function that constructs a CVCCommandUi from the stream
        * @leave KErrNotSupported if the stream data format is unsupported
        *        e.g. if it has been written by newer implementation
        */
        IMPORT_C static CVCCommandUi* NewL( RReadStream& aStream );
        
        /**
        * Copy the existing CVCCommandUi
        */
        IMPORT_C static CVCCommandUi* NewL( const CVCCommandUi& aOriginal );
                
        /** Destructor */
        IMPORT_C ~CVCCommandUi();

        /** 
		* Saves the command to stream.
		* Descriptor components are saved as <length><descriptor> pairs, where 
		* <length> is TInt32 and <descriptor> is the default descriptor represetation
		* TBools are saved as TInt32 either
		*/
		IMPORT_C void ExternalizeL( RWriteStream &aStream ) const;

        // Accessor methods
        
        /**
        * @return Unmodifiable written text
        */
        IMPORT_C const TDesC& WrittenText() const;
        
        /**
        * @return Unmodifiable user-specified shortcut for the command
        *         KNullDesC if none
        */
        IMPORT_C const TDesC& UserText() const;
        
        /**
        * @return Unmodifiable folder name
        */
        IMPORT_C const CVCFolderInfo& FolderInfo() const;
        
        /**
        * @return ETrue if the command is modifiable by user
        *         EFalse otherwise
        */
        IMPORT_C TBool Modifiable() const;
        
        /**
        * @return EFalse, if when the command is recognized,
        *         it is immediately executed
        *         ETrue, if when the command is recognized, it is played back and
        *         user is asked to confirm the recognition results
        * @see
        * @todo Add "see" reference to the recognition-side function that uses the
        *       "confirmation needed information"
        **/
        IMPORT_C TBool ConfirmationNeeded() const;
        
        /**
        * @return Unmodifiable extra text
        */
        IMPORT_C const TDesC& Tooltip() const;
        

        /**
        * @return global icon id
        * @todo AVKON-global? What kind of global?
        */ 
        IMPORT_C const TUid& IconUid() const;

	    /**
        * Creates the command icon. Takes into account the skin-specifics
	    * Works only if CEikonEnv is available
        * @return The command icon pushed to the cleanup stack
        */
	    IMPORT_C CGulIcon* IconLC() const;
        
        IMPORT_C TBool operator==( const CVCCommandUi& aCommandUi ) const;
        
        /**
         * Tells if all non user-changeable parts of aCommandUi are equal to this object's
         * @return ETrue if equal, EFalse otherwise
         */
        IMPORT_C TBool EqualNonUserChangeableData( const CVCCommandUi& aCommandUi ) const;
        
        
    private:
        /** 
        * Second-phase constructor 
        * @see NewL for guard conditions
        */
        void ConstructL( const TDesC& aWrittenText, 
                const CVCFolderInfo& aFolderInfo, TBool aModifiable, 
                const TDesC& aTooltip, const TUid& aIconUid, 
                const TDesC& aUserText, TBool aConfirmationNeeded );
                
        void ConstructL( RReadStream& aStream );
                
    private: // Data

        // Text shown on screen
        HBufC* iWrittenText;
        
        // User-specified alternative command text
        HBufC* iUserText;

        // Text shown on screen
        HBufC* iTooltip;

        // Folder-related details
        CVCFolderInfo* iFolderInfo;

        // Tells if this command is user modifiable
        TBool iUserCanModify;
        
        // Ask user to confirm the recognition result
        TBool iConfirmationNeeded;
        
        // global icon id
        /** @todo AVKON-global? What kind of global? */
        TUid iIconUid;
    };

/**
* Container for the information on how the command should be executed:
* 1) app uid OR exe filename; 2) optional command line arguments
*/    
class CVCRunnable : public CBase
    {
    public:
        /**
        * Factory function
        * @param aAppUid UID of the application to start. Cannot be KNullUidValue
        * @param aArguments Command line to pass to the application. This object makes
        *        a copy of the passed string
        * @leave KErrOverflow if aArguments is longer, than KMaxVCArgumentLength
        * @leave KErrArgument if aAppUid is KNullUidValue
        */
        IMPORT_C static CVCRunnable* NewL( TUid aAppUid, const TDesC8& aArguments = KNullDesC8 );
        
        /**
        * Factory function
        * @param aExeName Exe-file to start. Can be full or partical name according to 
        *        the RProcess::Create rules. Cannot be KNullDesC
        * @param aArguments Command line to pass to the application. This object makes
        *        a copy of the passed string
        * @leave KErrOverflow if aArguments is longer, than KMaxVCArgumentLength or 
        *		 aExeName is longer, than KMaxFileName
        * @leave KErrUndeflow if aExeName is empty
        */
        IMPORT_C static CVCRunnable* NewL( const TDesC& aExeName, 
        								   const TDesC8& aArguments = KNullDesC8 );
        
        /** 
        * Internalizes the object from stream 
        * @leave KErrNotSupported if the stream data format is unsupported
        *        e.g. if it has been written by newer implementation of CVCommandUi
        */
        IMPORT_C static CVCRunnable* NewL( RReadStream &aStream );
        
        /**
        * Copy the existing CVRunnable
        */
        IMPORT_C static CVCRunnable* NewL( const CVCRunnable& aOriginal );
        
        
        /** Destructor */
        IMPORT_C ~CVCRunnable();
        
        /** 
        * Run the command. In case of aAppUid specified during the construction
        * application is started via the RApaLsSession::StartApp, otherwise
        * via the RProcess::Create
        * @leave System-wide error code. In particular KErrNotFound if there is
        *        no such app in the system
        */
        IMPORT_C void ExecuteL() const;
        
        /** Saves the object to stream */
        IMPORT_C void ExternalizeL( RWriteStream &aStream ) const;

        /**
        * @return The uid of the application to be started. KNullUid if there is none
        */
        IMPORT_C const TUid Uid() const;
        
        /**
        * @return The filename of the application to be started. KNullDesC if there is none
        */
        IMPORT_C const TDesC& ExeName() const;
        
        /**
        * The arguments to be passed to the application. This object keeps
        * the ownership of the returned value
        */
        IMPORT_C const TDesC8& Arguments() const;
        
        IMPORT_C TBool operator==( const CVCRunnable& aRunnable ) const;
        
    private:
        void ConstructL( TUid aAppUid, const TDesC8& aArguments );
        void ConstructL( const TDesC& aExeName, const TDesC8& aArguments );
        void ConstructL( RReadStream& aStream );
        
    private:
        // UID of the application to be run, when the vcommand is recognized
        // KNullUidValue if none specified
        TUid iExeUid;
        
        // Filename to pass to RProcess::Create or NULL
        HBufC* iExeName;
        
        // Arguments string
        HBufC8* iArguments;
    };

/**
* The packaged VCommand storable/retriavable to/from VAS
*/
class CVCommand : public CBase
    {
    public:
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
        IMPORT_C static CVCommand* NewL( const TDesC& aText, const CVCRunnable& aRunnable, 
                                         const CVCCommandUi& aUi );
        
		/** 
        * Internalizes the command from stream 
        * @leave KErrNotSupported if the stream data format is unsupported
        *        e.g. if it has been written by newer implementation
        */
        IMPORT_C static CVCommand* NewL( RReadStream &aStream );
        
        /**
        * Copy the existing CVCommand
        */
        IMPORT_C static CVCommand* NewL( const CVCommand& aOriginal );
        
        /** Destructor */
        IMPORT_C ~CVCommand();
        
        /**
		* Saves the command to stream 
		* Format: <protocol version: int32><SpokenTextLength: int32><SpokenText descriptor>
		*         <runnable><commandui>
		*/
        IMPORT_C void ExternalizeL( RWriteStream &aStream ) const;
        
        /**
        * @return the Runnable component of the command
        */
        IMPORT_C const CVCRunnable& Runnable() const;

        /**
        * @return the UI-related component of the command
        */
        IMPORT_C const CVCCommandUi& CommandUi() const;
        
        /**
        * @return the text, that user is expected to pronounce
        *         It is different from the CVCommandUi::WrittenText
        */
        IMPORT_C const TDesC& SpokenText() const;
        
        /**
        * @return the user specified text, that user can pronounce to
        *         recognize the command. Is always identical to the 
        *         CVCommandUi::UserText()
        *         Can be KNullDesC
        *
        * @todo Responsibilities of SpokenText, AlternativeSpokenText,
        *       WrittenText and UserText are mixed. The source of problems
        *       is the fact that WrittenText can really be different from the
        *       default spoken text while User supplied text is always what's
        *       expected to be [alternatively] recognized
        *       A better structuring would be very welcome
        */
        IMPORT_C const TDesC& AlternativeSpokenText() const;
        
        /**
        * Asynchronous
        * Attempts to play back the text expected to be recognized. 
        * To be playable command has to be added to CVCommandHandler AND
        * then retrieved back
        *
        * @param aHandler CVCommandHandler where the command is stored
        * @todo Consider storing link to CVCommandHandler within the CVCommand (CStoredVCommand)
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
        IMPORT_C virtual void PlaySpokenTextL( const CVCommandHandler& aHandler, 
                                   MNssPlayEventHandler& aPlayEventHandler ) const;
        
        /**
        * Asynchronous
        * Plays back the user-specified alternative spoken text. 
        * Otherwise is identical to PlaySpokenTextL
        * @see PlaySpokenTextL
        * @leave KErrNotFound if this command cannot be found in aHandler of if 
        *        it doesn't have a user-specified text
        */ 
        IMPORT_C virtual void PlayAlternativeSpokenTextL( const CVCommandHandler& aHandler, 
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
        IMPORT_C virtual void CancelPlaybackL( const CVCommandHandler& aHandler ) const;
        
        IMPORT_C TBool operator==( const CVCommand& aCommand ) const;
        
        /**
         * Tells if all non user-changeable parts of aCommand are equal to this command
         * @return ETrue if equal, EFalse otherwise
         */
        IMPORT_C TBool EqualNonUserChangeableData( const CVCommand& aCommand ) const;
        
    protected:
    	/**
    	* @leave KErrOverflow if aText is longer, than KMaxVCTextLength
    	*/ 
        void ConstructL( const TDesC& aText, const CVCRunnable& aRunnable, 
                         const CVCCommandUi& aUi );
        /**
        * @see NewL( RReadStream& aStream )
        */
        void ConstructL( RReadStream& aStream );
        
        /**
        * Clone itself
        * Is used for copying via NewL( CVCommand& aOriginal )
        * To be overridden by descendants
        */
        virtual CVCommand* CloneL() const; 

    private:
    	// Text to be recognized
        HBufC* iSpokenText;
        CVCRunnable* iRunnable;
        CVCCommandUi* iCommandUi;
        
    };
    
/** Array of pointers to CVCommands */
typedef RPointerArray<CVCommand> RVCommandArray;

/**
* Non-modifiable list of VCommands
*/
class CVCommandArray : public CBase
	{
	public:
		/** 
		* Constructs the non-modifiable CVCommandArray
		* @param aSource Commands to store. CVCommandArray makes
		*        copies of them
		*/
		IMPORT_C static CVCommandArray* NewL( const RVCommandArray& aSource );
		
		/**
		* Destructor
		*/
		IMPORT_C ~CVCommandArray();
		
		/**
		* Returns the reference to the command stored in this
		* CVCommandArray
		* @param aIndex Zero-based index of the command. If aIndex is out of
		*               bounds, this method will panic with USER 130
		* @return Unmodifiable reference to CVCommand
		*/
		IMPORT_C const CVCommand& At( TInt aIndex ) const;
		
		/**
		* Equivalent to the operator At
		* @see At()
		*/
		IMPORT_C const CVCommand& operator[]( TInt aIndex ) const;

		/** 
		* Returns the number of commands
		*/
		IMPORT_C TInt Count() const;
		
		/**
		* For the compatibility with routines, requiring RVCommandArray
		* @return Unmodifiable array of pointers to VCommands
		*/
		IMPORT_C const RVCommandArray& PointerArray() const;
        
        /**
         * Figures out which commands have to untrained (removed from the system)
         * if this command set is merged with aUpdates.
         * Merging is performed on the basis of comapring the commands' Runnable component
         * If runnables are equal, the commands are considered being equal.
         * It is useful e.g. when figuring out what to (un)train after the language change
         * 
         * @param aUpdates Set of commands to merge in. 
         * @return Set of commands to be removed from the system if the merging takes place
         * @see ProduceTrainSetByRunnables
         */
        IMPORT_C CVCommandArray* ProduceUntrainSetByRunnablesLC( 
                                    const RVCommandArray& aUpdates ) const;
        
        /**
         * Figures out which commands have to trained (removed added to the system)
         * if this command set is merged with aUpdates.
         * Merging is performed on the basis of comapring the commands' Runnable component
         * If runnables are equal, the commands are considered being equal.
         * It is useful e.g. when figuring out what to (un)train after the language change
         * 
         * @param aUpdates Set of commands to merge in. 
         * @return Set of commands to be added to the system if the merging takes place
         * @see ProduceUntrainSetByRunnables
         */
        IMPORT_C CVCommandArray* ProduceTrainSetByRunnablesLC( 
                                    const RVCommandArray& aUpdates ) const;
		
	private:
		void ConstructL( const RVCommandArray& aSource );

	private:
		RVCommandArray iCommands;
	};

/**
 * Allows the CVCommandHandler clients be notified about the vcommand set
 * changes by another instances of the CVCommandHandler
 */
class MVCommandHandlerObserver 
	{
	public:
		/**
		* Is called whenever the VCommand set is changed
    	* by *another* instance of CVCommandHandler. I.e. lets the client know if
    	* his understanding of the VCommand set is up to date
    	*/
		virtual void CommandSetChanged() = 0;
	};

/**
* Provider of the voice command services
*/
class CVCommandHandler : public CBase
    {
    public:
    	/** 
    	* Factory function
    	* @param aObserver Listener to be notified whenever the VCommand set is changed
    	* 		 by *another* instance of CVCommandHandler. I.e. lets the client know if
    	* 		 his understanding of the VCommand set is up to date
    	*/ 
    	IMPORT_C static CVCommandHandler* NewL(  MVCommandHandlerObserver* aObserver = NULL );
    	
        IMPORT_C virtual ~CVCommandHandler();

        /**
        * Synchronous. Service doesn't take the ownership, but makes an own copy
        * Duplicates can be added
        * @todo should we check for duplicates and leave with KErrAlreadyExists?
        */
        IMPORT_C virtual void AddCommandL( const CVCommand& aCommand );
        
        /**
        * Synchronous. Service doesn't take the ownership, but makes an own copies
        * Duplicates can be added
        * @todo should we check for duplicates and leave with KErrAlreadyExists?
        * @param aCommands Commands to add
        * @param aIgnoreErrors If ETrue, even if some commands fail to be trained,
        *        handler adds all that are trainable
        */
        IMPORT_C virtual void AddCommandsL( const RVCommandArray& aCommands, 
                                            TBool aIgnoreErrors = EFalse );
        
        /**
        * Synchronous. Removes the command from the system
        * @param aCommand Reference to the command to be removed. Existing commands are
        *		 compared against aCommand. All the matches are removed from VAS
        * @leave KErrNotFound No such command
        */
        IMPORT_C virtual void RemoveCommandL( const CVCommand& aCommand );

        /**
        * Synchronous. 
        * @param aCommands Reference to the list of commands to be removed. Existing commands are
        *		 compared against aCommands items. All the matches are removed from VAS
        * @param aIgnoreErrors If ETrue, even if some commands fail to be removed,
        *        handler will remove as many as possible
        */
        IMPORT_C virtual void RemoveCommandsL( const RVCommandArray& aCommands, 
				      TBool aIgnoreErrors = EFalse );

        
        /**
        * Synchronous
        * @return an array of the commands in the system
        *         Ownership of the array is transfered to the client
        *         The returned CVCommandArray contains copies of all the 
        *         commands currently stored in this handler
        */
        IMPORT_C virtual CVCommandArray* ListCommandsL();
        
        /**
        * Not intented to be called directly.
        * @see CVCommand::PlaySpokenTextL
        */
        virtual void PlaySpokenTextL( const CStoredVCommand& aCommand, 
                                   MNssPlayEventHandler& aPlayEventHandler ) const;
        
        /**
        * Not intented to be called directly.
        * @see CVCommand::PlayAlternativeSpokenTextL
        */                                   
        virtual void PlayAlternativeSpokenTextL( const CStoredVCommand& aCommand, 
                                   MNssPlayEventHandler& aPlayEventHandler ) const;
                                   
        /**
        * Not intented to be called directly.
        * @see CVCommand::CancelPlaybackL
        */                                   
        virtual void CancelPlaybackL( const CStoredVCommand& aCommand ) const;
        
    private:
    	void ConstructL( MVCommandHandlerObserver* aObserver );
    	// Implementation of the VCommand API
    	MVCService* iImpl;
    };
    
#endif // VCOMMANDAPI_H