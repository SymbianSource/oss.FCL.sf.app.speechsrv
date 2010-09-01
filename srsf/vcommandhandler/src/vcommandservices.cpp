/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of VCommand Handler that links the VCommand 
*                app to VAS
*
*/


#include "rubydebug.h"
#include "vcommandservices.h"
#include "contextprovider.h"
#include "tagnameset.h"
#include "taggetter.h"
#include "tagcommandconverter.h"
#include "tagplayer.h"
#include <nssvascoreconstant.h>

#include <s32mem.h>
#include <nssvascvasdbmgr.h>
#include "vcommandinternalapi.h"

// For CleanupResetAndDestroy
#include <mmfcontrollerpluginresolver.h>  


/**
* Name of the global mutex used to serialize the operations
*/
_LIT( KCommandHandlerLockName, "VCOMHNDLR" );

_LIT( KCommandHandlerPanic, "VCH" );

TAny* TVCommandTagPtrArrayKey::At( TInt aIndex ) const
    {
    MNssTag* tag ( NULL );
    TInt* key (NULL);
    if ( aIndex==KIndexPtr )
        {
        // iPtr is a pointer to the array element which is a pointer itself 
        // in case of the CArrayPtr
        tag = *(MNssTag**)iPtr;
        key = &tag->RRD()->IntArray()->At( 0 );
        }
    else 
        {
        // iBase is CBufBase* of the searched array
        // CBufBase memory buffer stores pointers to MNssTag* (i.e. MNssTag**)
        // iBase->Ptr returns TPtr8 to the desired pointer 
        // TPtr8.Ptr returns TUint8* of the element
        // This element itself is a pointer to CPerson (i.e. MNssTag**)
        tag = *(MNssTag**)iBase->Ptr( aIndex*sizeof( MNssTag** ) ).Ptr();
        key = &tag->RRD()->IntArray()->At( 0 );
        }        
        return key;        
    }
    
// Construction/destruction
CVCommandService* CVCommandService::NewL( MVCommandHandlerObserver* aObserver )
    {
    CVCommandService* self = new (ELeave) CVCommandService( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CVCommandService::~CVCommandService()
    {
    RUBY_DEBUG0( "CVCommandService::~CVCommandService" );
    delete iGlobalTickWatcher;
    iGlobalTickProperty.Close();
    delete iVasDbManager;
    iCommands.ResetAndDestroy();
    delete iTagPlayer;
    if( iLock.IsHeld() )
        {
        RUBY_DEBUG0( "CVCommandService::~CVCommandService. Mutex is still held.\
                     Signaling it" );
        iLock.Signal();
        }
    iLock.Close();
    }
   
// From the MVCService

/**
* Synchronous. Service doesn't take the ownership, but makes an own copy
* Duplicates can be added
* @todo should we check for duplicates and leave with KErrAlreadyExists?
*/
void CVCommandService::AddCommandL( const CVCommand& aCommand )
    {
    RUBY_DEBUG_BLOCK( "CVCommandService::AddCommandL" );
    StartAtomicOperationLC();
    
    RVCommandArray commands;
    CleanupClosePushL( commands );
    commands.Append( &aCommand );
    DoAddCommandsL( commands );
    
    CleanupStack::PopAndDestroy( &commands );
    CleanupStack::PopAndDestroy();  // Calls EndAtomicOperation
    }

/**
* Synchronous. Service doesn't take the ownership, but makes an own copy
* Duplicates can be added
* @todo Should we check for duplicates and leave with KErrAlreadyExists?
* @param aIgnoreErrors If ETrue, even if some commands fail to be trained,
*        handler adds all that are trainable
*/
void CVCommandService::AddCommandsL( const RVCommandArray& aCommands, TBool aIgnoreErrors )
    {
    RUBY_DEBUG_BLOCK( "CVCommandService::AddCommandsL" );
    StartAtomicOperationLC();
    DoAddCommandsL( aCommands, aIgnoreErrors );
    CleanupStack::PopAndDestroy();  // Calls EndAtomicOperation
    }
    
void CVCommandService::DoAddCommandsL( const RVCommandArray& aCommands, TBool aIgnoreErrors )
    {
    RUBY_DEBUG_BLOCKL( "CVCommandService::DoAddCommandsL" );
    MNssContext* context = GetVCommandContextLC();
    CTagNameSet* trainingPack = CTagNameSet::NewLC();
    
    for( TInt i = 0; i < aCommands.Count(); i++ )
        {
        CArrayPtr<MNssTag>* tags = CreateTagsLC2( *aCommands[i], *context); 
        // tags array contains 1 tag if UserText is not defined
        // tags array contains 2==KMaxTagsPerCommand tags if UserText is defined
        __ASSERT_ALWAYS( tags->Count() > 0, User::Leave( KErrCorrupt ) );
        __ASSERT_ALWAYS( tags->Count() <= KMaxTagsPerCommand, User::Leave( KErrCorrupt ) );
        
        // All the tags corresponding to the same VCommand get the same command id
        SetVCommandIdL( *tags, NewCommandIdL() );
        trainingPack->AppendL( tags->At( 0 ), aCommands[i]->SpokenText() );
        if( tags->Count() == KMaxTagsPerCommand )
            {
            // UserText tag is present and this command has been converted into two tags
            trainingPack->AppendL( tags->At( 1 ), aCommands[i]->AlternativeSpokenText() );
            }    
            
        CleanupStack::Pop( tags );  // ResetAndDestroy
        CleanupStack::PopAndDestroy( tags );  // delete the CArrayPtr itself
        }  // for aCommands
    
    
    CNssTrainingParameters* trainingParams = ConstructTrainingParametersLC();    
    trainingPack->TrainAndSaveL( *trainingParams, *iTagManager, aIgnoreErrors );
    CleanupStack::PopAndDestroy( trainingParams );
    
    CleanupStack::PopAndDestroy( trainingPack );
    CleanupStack::PopAndDestroy( context );
    RUBY_DEBUG0( "CVCH::DoAddCommandsL Context destroyed" );        
    InvalidateCacheL();
    }


/**
* Synchronous. Removes the command from the system
* @param aCommand Reference to the command to be removed. Existing commands are
*		 compared against aCommand. All the matches are removed from VAS
* @leave KErrNotFound No such command
*/
void CVCommandService::RemoveCommandL( const CVCommand& aCommand )
    {
    RUBY_DEBUG_BLOCK( "CVCommandService::RemoveCommandL" );
    StartAtomicOperationLC();
	RVCommandArray array;
	CleanupClosePushL( array );
	array.AppendL( &aCommand );
	DoRemoveCommandsL( array );
	CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy();  // Calls EndAtomicOperation
    }
    
/**
* Synchronous. 
* @param aCommands Reference to the list of commands to be removed. Existing commands are
*		 compared against aCommands items. All the matches are removed from VAS
* @param aIgnoreErrors If ETrue, even if some commands fail to be removed,
*        handler will remove as many as possible
*/
void CVCommandService::RemoveCommandsL( const RVCommandArray& aCommands, TBool aIgnoreErrors )
	{
    RUBY_DEBUG_BLOCK( "RemoveCommandsL" );
    StartAtomicOperationLC();
    DoRemoveCommandsL( aCommands, aIgnoreErrors );
    CleanupStack::PopAndDestroy();  // Calls EndAtomicOperation
	
	}
    
void CVCommandService::DoRemoveCommandsL( const RVCommandArray& aCommands,
        						TBool aIgnoreErrors )
	{
	// for tracking the not found situations
	RArray<TBool> foundCommands;
	CleanupClosePushL( foundCommands );
	for( TInt i=0; i < aCommands.Count(); i++ )
	    {
	    foundCommands.AppendL( EFalse );
	    }
	
	// Iterate over all the tags in the system. Whenever a tag can be converted
	// to one of the aCommands, move it to the deletion bunch
	CTagNameSet* removeTags = CTagNameSet::NewLC();
	MNssContext* context = GetVCommandContextLC();    
	MNssTagListArray* tags = CVCommandService::GetTagListLC2( *context );
	for( TInt i = 0; i < tags->Count(); i++ )
		{
		CVCommand* command = CTagCommandConverter::TagToCommandLC( *tags->At( i ) );
		for( TInt j = 0; j < aCommands.Count(); j++ )
			{
			if( *command == *aCommands[j] )
				{
				RUBY_DEBUG1( "CVCH::RemoveCommandsL Tag [%d] should be deleted", i );
				foundCommands[j] = ETrue;
				removeTags->AppendL( tags->At( i ) );  
				// ownership to removeTags, current tag to be removed from tags => index--
				tags->Delete( i );
				i--;
				break;
				}
			}
		CleanupStack::PopAndDestroy( command );
		}

    // Handle not founds
    if( !aIgnoreErrors )
        {
        for( TInt i=0; i < foundCommands.Count(); i++ )
            {
            if( foundCommands[i] == EFalse )
                {
                RUBY_ERROR1( "Command [%d] not found", i );
                User::Leave( KErrNotFound );
                }
            }
        }
	// Delete all the discovered tags
	removeTags->UntrainL( *iTagManager, aIgnoreErrors );
	
	CleanupStack::PopAndDestroy( tags );  // ResetAndDestroy
	CleanupStack::PopAndDestroy( tags );  // delete	
    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( removeTags );
    CleanupStack::PopAndDestroy( &foundCommands );
    InvalidateCacheL();
	}
        						
/**
* Synchronous
* @return an array of the commands in the system
*         Ownership of the array is transfered to the client
*         The returned CVCommandArray contains copies of all the 
*         commands currently stored in this handler
*/
CVCommandArray* CVCommandService::ListCommandsL()
    {
    RUBY_DEBUG_BLOCK( "CVCommandService::ListCommandsL" );
    StartAtomicOperationLC();
    if( !IsCacheValidL() )
    	{
    	RUBY_DEBUG0( "CVCommandService::ListCommandsL Cache is invalid. \
    					Refresh the command list" );
    	RefreshCommandListL();
    	}
    
    CVCommandArray* result = CVCommandArray::NewL( iCommands );
    CleanupStack::PopAndDestroy();  // Calls EndAtomicOperation
    return result;
    }

// Private methods

CVCommandService::CVCommandService( MVCommandHandlerObserver* aObserver ) :
    iObserver( aObserver )
    {
    // nothing
    }

void CVCommandService::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CVCommandService::ConstructL" );
    TInt err = iLock.OpenGlobal( KCommandHandlerLockName );
    if( err != KErrNone )
        {
        RUBY_DEBUG1( "CVCommandService::ConstructL failed to open mutex with [%d].\
                      Creating a new one", err );
        err = iLock.CreateGlobal( KCommandHandlerLockName );
        RUBY_DEBUG1( "CVCommandService::ConstructL. Created mutex. Err [%d]", err );
        User::LeaveIfError( err );
        }
    iTagPlayer = CTagPlayer::NewL();
    iVasDbManager = CNssVASDBMgr::NewL();
    iVasDbManager->InitializeL();
    iTagManager = iVasDbManager->GetTagMgr();
    TInt propertyErr = RProperty::Define( KSINDUID, ECommandHandlerTickKey, RProperty::EInt );
    if( ( propertyErr != KErrNone ) && ( propertyErr != KErrAlreadyExists ) )
        {
        RUBY_ERROR1( "Attempt to define the ECommandHandlerTickKey flag failed with [%d]", 
                     propertyErr )
        User::Leave( propertyErr );
        }
    
    RUBY_DEBUG0( "Attaching to the global tick property" );
    User::LeaveIfError( iGlobalTickProperty.Attach( KSINDUID, ECommandHandlerTickKey ) );
    // right after creation, local cache cannot be valid
    TInt globalTickCount;
    User::LeaveIfError( iGlobalTickProperty.Get( globalTickCount ) );
    iLocalTickCount = globalTickCount - 1;
    
    iLastSetGlobalTickCount = iLocalTickCount;
    iGlobalTickWatcher = CIntPropertyWatcher::NewL( KSINDUID, ECommandHandlerTickKey, *this );
    }
    
/**
* Returns the VCommand context. Creates one on demand if none exists yet
*/
MNssContext* CVCommandService::GetVCommandContextLC() const
    {
    RUBY_DEBUG0( "CVCommandService::GetVCommandContextL start" );
    CContextProvider* contextProvider = CContextProvider::NewLC( *iVasDbManager );
    MNssContext* result = contextProvider->GetVCommandContextLC();
    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( contextProvider );
    CleanupDeletePushL( result );
    RUBY_DEBUG0( "CVCommandService::GetVCommandContextL end" );
    return result;    
    }
    
    
/**
* Constructs a set of new VAS tags from a command and a context. 
* Does not save to VAS, just creates. Ownership on the created tag is 
* passed to the client
*/
CArrayPtr<MNssTag>* CVCommandService::CreateTagsLC2( const CVCommand& aCommand, 
                                                     const MNssContext& aContext )
    {
    return CTagCommandConverter::CommandToTagsLC2( aCommand, aContext, *iTagManager );
    }
    
/**
* Constructs training related parameters. Like language to be used
* @leave negated TNssSpeechItemResult
* @todo Is it ok to mix system-wide codes with the TNssSpeechItemResult codes
*/
CNssTrainingParameters* CVCommandService::ConstructTrainingParametersLC() const
    {
    RUBY_DEBUG0( "CVCommandService::SetLanguageParamersL start" );
    RArray<TLanguage>* languageArray = new (ELeave) RArray<TLanguage>;
    CleanupDeletePushL ( languageArray ); // Protect [allocated on the heap] array itself
    CleanupClosePushL( *languageArray );  // Protect the array elements
    
    // Always generate a pronunciation in UI language
    User::LeaveIfError( languageArray->Append( User::Language() ) );
    
    // Add an extra language, which can be determined by language identification
    User::LeaveIfError( languageArray->Append( ELangOther ) );

    CNssTrainingParameters* trainingParams = CNssTrainingParameters::NewL();
    // No L-functions for some time, delay pushing parameters to the cleanup stack
    
    trainingParams->SetLanguages( languageArray );
    // No need to protect the language array anymore. 
    // From now on it is managed by the parameters 
    
    // For array elements pushed via CleanupClosePushL
    CleanupStack::Pop( languageArray );
    CleanupStack::Pop( languageArray );
    CleanupStack::PushL( trainingParams );
    RUBY_DEBUG0( "CVCommandService::SetLanguageParamersL end" );
    return trainingParams;    
    }

/**
* Retrieves the list of tags for a given context. Synchronous.
* Leaves two objects on the cleanup stack!
* First PopAndDestroy will ResetAndDestroy content
* Second one will destroy the MNsstagListArray itself
*/
MNssTagListArray* CVCommandService::GetTagListLC2( const MNssContext& aContext ) const
    {
    RUBY_DEBUG0( "CVCommandService::GetTagListLC start" );
    CTagGetter* tagGetter = CTagGetter::NewLC();
    MNssTagListArray* result = tagGetter->GetTagListLC2( *iTagManager, aContext );

    CleanupStack::Pop( result );
    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( tagGetter );
    CleanupDeletePushL( result );
    CleanupResetAndDestroy<MNssTagListArray>::PushL( *result );
    RUBY_DEBUG0( "CVCommandService::GetTagListLC end" );

    return result;
    }
    
/**
* Retrieves the list of tags for a given context and voice command id. 
* Synchronous. Leaves two objects on the cleanup stack!
* First PopAndDestroy will ResetAndDestroy content
* Second one will destroy the MNsstagListArray itself
*/
MNssTagListArray* CVCommandService::GetTagListLC2( const MNssContext& aContext, 
                                                   TInt aCommandId ) const
    {
    RUBY_DEBUG0( "CVCommandService::GetTagListLC start" );
    CTagGetter* tagGetter = CTagGetter::NewLC();
    MNssTagListArray* result = tagGetter->GetTagListLC2( *iTagManager, aContext, aCommandId );

    CleanupStack::Pop( result );
    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( tagGetter );
    CleanupDeletePushL( result );
    CleanupResetAndDestroy<MNssTagListArray>::PushL( *result );
    RUBY_DEBUG0( "CVCommandService::GetTagListLC end" );

    return result;
    }

/**
* Resets iCommands and fills them with the commands from VAS
*/
void CVCommandService::RefreshCommandListL()
	{
	RUBY_DEBUG_BLOCK( "CVCommandService::RefreshCommandListL" );
	const TInt KMinimalCommandId = 0;
	iCommands.ResetAndDestroy();
	iMaxCommandId = KMinimalCommandId - 1;
    MNssContext* context = GetVCommandContextLC();
    MNssTagListArray* tagList = GetTagListLC2( *context );
    RUBY_DEBUG1( "CVCommandService::RefreshCommandListL Found [%d] tags", tagList->Count() );
    
    // Sort array by command ids
    TVCommandTagPtrArrayKey key( ECmpTInt );
    tagList->Sort( key );
    
    // Array of tags related to the same VCommand
    CArrayPtrFlat<MNssTag>* commandArray = 
                new ( ELeave) CArrayPtrFlat<MNssTag>( KMaxTagsPerCommand ); 
    CleanupStack::PushL( commandArray );
    // No need for CleanupResetAndDestroyPushL. 
    // MNssTag objects are not copied to the commandArray
    
    TInt currentId = KMinimalCommandId - 1;
    
    for( TInt i = 0; i < tagList->Count(); i++ ) 
        {
        RUBY_DEBUG1( "CVCommandService::RefreshCommandListL Processing tag [%d]", i );
        TInt commandId = tagList->At( i )->RRD()->IntArray()->At( 0 );
        if( currentId != commandId )  // new or first command
            {
            if( commandArray->Count() > 0  )
                {
                RUBY_DEBUG1( "CVCommandService::RefreshCommandListL Converting [%d] tags to command", commandArray->Count() );
                CStoredVCommand* command = CTagCommandConverter::TagsToCommandLC( *commandArray );
                RUBY_DEBUG2( "CVCommandService::RefreshCommandListL Converted successfully to command id [%d], command text [%S]", command->CommandId(), &(command->SpokenText() ) );
                iCommands.AppendL( command );
                CleanupStack::Pop( command );
                }
            commandArray->Reset();
            currentId = commandId;
            }
        commandArray->AppendL( tagList->At( i ) );
        iMaxCommandId = Max( iMaxCommandId, commandId );
        }
        
    // Flush the last buffer
    if( commandArray->Count() > 0  )
        {
        RUBY_DEBUG1( "CVCommandService::RefreshCommandListL Converting [%d] tags to command", commandArray->Count() );
        CStoredVCommand* command = CTagCommandConverter::TagsToCommandLC( *commandArray );
        RUBY_DEBUG2( "CVCommandService::RefreshCommandListL Converted successfully to command id [%d], command text [%S]", command->CommandId(), &(command->SpokenText() ) );
        iCommands.AppendL( command );
        CleanupStack::Pop( command );
        }
    CleanupStack::PopAndDestroy( commandArray );
    CleanupStack::PopAndDestroy( tagList );  // ResetAndDestroy
    CleanupStack::PopAndDestroy( tagList );  // delete
    CleanupStack::PopAndDestroy( context );
    MarkCacheValidL();
	}
	
/**
* Marks the iCommands as an invalid reflection of the VAS content.
* To make iCommands reflect the real VAS content a call to 
* RefreshCommandsL is needed
*/
void CVCommandService::InvalidateCacheL()
	{
	// Invalidating own cache means unvalidating the other instances' caches as well
	TInt globalTickCount;
	User::LeaveIfError( iGlobalTickProperty.Get( globalTickCount ) );
	
	iLocalTickCount = globalTickCount;
    iLastSetGlobalTickCount = globalTickCount + 1;
	User::LeaveIfError( iGlobalTickProperty.Set( iLastSetGlobalTickCount ) );
	}
	
/**
* Marks the iCommands as the valid reflection of the VAS content.
*/
void CVCommandService::MarkCacheValidL()
	{
	// Since all the cache comparison/update functions are executed under the
	// StartAtomicOperation/EndAtomicOperation, we can be sure that nobody
	// touched VAS and global tick count since the last IsCacheValidL
	TInt globalTickCount;
	User::LeaveIfError( iGlobalTickProperty.Get( globalTickCount ) );
	
	iLocalTickCount = globalTickCount;
	}

/**
* Tells if the iCommands reflect the content of the VAS DB correctly
* @return ETrue if the cache is valid, EFalse otherwise
*/
TBool CVCommandService::IsCacheValidL()
	{
	RUBY_DEBUG_BLOCKL( "CVCommandService::IsCacheValidL" );
	TInt globalTickCount;
	User::LeaveIfError( iGlobalTickProperty.Get( globalTickCount ) );
	RUBY_DEBUG2( "CVCommandService::IsCacheValidL Local tick count [%d], \
	              globalTickCount [%d]", iLocalTickCount, globalTickCount );
	return iLocalTickCount == globalTickCount;
	}

/**
* Generates new command id to be used for identifying voice tags, that
* belong to the same VCommand
*/
TInt CVCommandService::NewCommandIdL()
    {
    if( !IsCacheValidL() )
        {
        // Fills the max used command id
        RefreshCommandListL();
        }
    return ++iMaxCommandId;
    }
    
/** 
* Sets the given aId as a VCommand id for all the tags in the given list
* This id is set as a first RRD int element of all the given MNssTags.
* If the RRD int array has no elements yet, one element is added
*/
void CVCommandService::SetVCommandIdL( CArrayPtr<MNssTag>& aTags, TInt aId ) const
    {
    // All the tags corresponding to the same VCommand get the same command id
    
    for( TInt i = 0; i < aTags.Count(); i++ )
        {
        if( aTags[i]->RRD()->IntArray()->Count() > 0 ) 
            {
            aTags[i]->RRD()->IntArray()->At( i ) = aId;
            }
        else 
            {
            aTags[i]->RRD()->IntArray()->AppendL( aId );
            }
        }
     }

/**
* Asynchronous
* Attempts to play back the text expected to be recognized. 
* To be playable command has to be added to CVCommandHandler AND
* then retrieved back
*
* @param aHandler CVCommandHandler where the command is stored
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
void CVCommandService::PlaySpokenTextL( const CStoredVCommand& aCommand, 
                                        MNssPlayEventHandler& aPlayEventHandler )
    {
    RUBY_DEBUG_BLOCK( "CVCommandService::PlaySpokenTextL" );
    __ASSERT_ALWAYS( !iPlaybackHandler, User::Leave( KErrInUse  ) );
    StartAtomicOperationLC();
    iPlaybackHandler = &aPlayEventHandler;
    MNssTag* playbackTag = TagByCommandIdTextL( aCommand.CommandId(), 
                                    aCommand.SpokenText() );
    if( !playbackTag )
        {
        User::Leave( KErrNotFound );
        }
    iTagPlayer->PlayTagL( playbackTag, *this ); 
    CleanupStack::Pop(); // EndOfAtomicOperation
    }

/**
* Asynchronous
* Plays back the user-specified alternative spoken text. 
* Otherwise is identical to PlaySpokenTextL
* @see PlaySpokenTextL
* @leave KErrNotFound if this command cannot be found in aHandler of if 
*        it doesn't have a user-specified text
*/     
void CVCommandService::PlayAlternativeSpokenTextL( const CStoredVCommand& aCommand, 
                                        MNssPlayEventHandler& aPlayEventHandler )
    {
    RUBY_DEBUG_BLOCK( "CVCommandService::PlayAlternativeSpokenTextL" );
    __ASSERT_ALWAYS( !iPlaybackHandler, User::Leave( KErrInUse  ) );
    StartAtomicOperationLC();
    iPlaybackHandler = &aPlayEventHandler;
    MNssTag* playbackTag = TagByCommandIdTextL( aCommand.CommandId(), 
                                    aCommand.AlternativeSpokenText() );
    if( !playbackTag )
        {
        User::Leave( KErrNotFound );
        }
    iTagPlayer->PlayTagL( playbackTag, *this ); 
    CleanupStack::Pop(); // EndOfAtomicOperation
    }
    
/**
* This method is not intended to be called directly. 
* Use CVCommand::CancelPlaybackL instead
* @see CVCommand::CancelPlaybackL
* @see CStoredVCommand::CancelPlaybackL
*/
void CVCommandService::CancelPlaybackL( const CStoredVCommand& /*aCommand*/ )
    {
    // For calling EndOfAtomicOperation if a leave occurs 
    CreateAndPushEndAtomicCleanupItemL();

    __ASSERT_ALWAYS( iPlaybackHandler, User::Leave( KErrNotReady ) );
    iTagPlayer->CancelPlaybackL();
    iPlaybackHandler = NULL;
    CleanupStack::PopAndDestroy(); // Calls EndAtomicOperation
    }
    
/**
* The HandlePlayStarted method is a virtual method implemented by the
* client and is called when play is started
* @param aDuration - the duration of the utterance data
*/       
void CVCommandService::HandlePlayStarted( TTimeIntervalMicroSeconds32 aDuration )
    {
    __ASSERT_ALWAYS( iPlaybackHandler, User::Panic( KCommandHandlerPanic, KErrNotReady ) );
    iPlaybackHandler->HandlePlayStarted( aDuration );
    }

/**
* The HandlePlayComplete method is a virtual method implemented by the
* client and is called when play is completed
* @param aErrorCode EVasErrorNone if playing was successfull
*/       
void CVCommandService::HandlePlayComplete( TNssPlayResult aErrorCode )
    {
    __ASSERT_ALWAYS( iPlaybackHandler, User::Panic( KCommandHandlerPanic, KErrNotReady ) );
    
    // End atomic operation before calling the observer not to block its potential
    // VCommandHandler usage from the HandlePlayComplete
    
    // Before ending atomic operation, store the current event handler to the 
    // temporary variable to protect it against the potential change by the new
    // PlaySpokenTextL call
    MNssPlayEventHandler* handler = iPlaybackHandler;
    iPlaybackHandler = NULL;
    EndAtomicOperation();
    handler->HandlePlayComplete( aErrorCode );    
    }

    
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
MNssTag* CVCommandService::TagByCommandIdTextL( TInt aCommandId, const TDesC& aText )
    {
    RUBY_DEBUG_BLOCKL( "CVCommandService::TagByCommandIdTextL" );
    MNssContext* context = GetVCommandContextLC();
    MNssTagListArray* tagList = GetTagListLC2( *context, aCommandId );
    // A command can have up to two voice tags. We need to locate the one, 
    // that has been trained against a given text
    MNssTag* tag (NULL);
    for( TInt i = 0; i < tagList->Count(); i++ )
        {
        if( tagList->At(i)->SpeechItem()->RawText() == aText )
            {
            tag = tagList->At(i);
            // deletes pointer only. tag will be returned to the calling party
            tagList->Delete( i );  
            break;
            }  // if
        }  // for
    CleanupStack::PopAndDestroy( tagList );  // ResetAndDestroy
    CleanupStack::PopAndDestroy( tagList );  // delete
    CleanupStack::PopAndDestroy( context );
    
    return tag;
    }

/**
* Starts the section of the code, that can be simultaneously executed
* only by a single instance of this class. Other instances have to wait
* Same as starting the Java "synchronized" method. Puts the function
* EndAtomicOperation to cleanup stack to make the use leave safe. Subsequent
* call to CleanupStack::PopAndDestroy will run EndAtomicOperation.
*/
void CVCommandService::StartAtomicOperationLC()
    {
    RUBY_DEBUG0( "CVCommandService::StartAtomicOperation start" );
    if ( iLock.IsHeld() )
        {
        User::Leave( KErrLocked );
        }
        
    iLock.Wait();
    
    CreateAndPushEndAtomicCleanupItemL();
    
    RUBY_DEBUG0( "CVCommandService::StartAtomicOperation end" );
    }

/**
* Ends the section of the code, that can be simultaneously executed
* only by a single instance of this class. Other instances have to wait
* Same as exiting the Java "synchronized" method
* @leave KErrNotReady if atomic operation hasn't been started
*/
void CVCommandService::EndAtomicOperation()
    {
    RUBY_DEBUG0( "CVCommandService::EndAtomicOperation start" );
    __ASSERT_ALWAYS( iLock.IsHeld(), User::Panic( KCommandHandlerPanic, KErrNotReady ) );
    iLock.Signal();
    RUBY_DEBUG0( "CVCommandService::EndAtomicOperation end" );
    }
    
/**
* Runs the EndAtomicOperation when calling CleanupStack::PopAndDestroy after
* a call to StartAtomicOperationLC
*/    
void CVCommandService::CleanupEndAtomicOperation( TAny* aService )
    {
    CVCommandService* service = static_cast<CVCommandService*>(aService);
    service->EndAtomicOperation();
    }

/**
* Creates and pushes to cleanup stack a TCleanupItem that calls
* EndAtomicOperation when CleanupStack::PopAndDestroy is called
*/   
void CVCommandService::CreateAndPushEndAtomicCleanupItemL()
    {
    TCleanupItem item( CleanupEndAtomicOperation, this );
    CleanupStack::PushL( item );
    }
    
void CVCommandService::IntValueChanged( TInt aNewValue )
    {
    RUBY_DEBUG2( "iLocalTickCount [%d], aNewValue [%d]", iLocalTickCount, aNewValue );
    if( aNewValue != iLastSetGlobalTickCount )
        {
        if( iObserver )
            {
            iObserver->CommandSetChanged();
            }  // if iObserver
        }  // if local cache is out of date
    }
    
//End of file
