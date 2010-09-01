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
* Description:  Implementation of the VCommand class
*
*/



#include <e32def.h>
#include <vcommandapi.h>
#include "rubydebug.h"

/**
* The version of the streamed data protocol
* It is rather likely, that the format of the data streamed changes in the future.
* Using the protocol version, the code can check if it knows how to parse the 
* input stream
*/
const TInt32 KVCommandProtocolVersion = 1;

/**
* Create a const snapshop of exe-ui-vastext collection
* @param aText Text to be trained by VAS. No longer, than KMaxVCTextLength. 
*        CVCommand doesn't take the ownership
* @param aRunnable Executable to be fired when the command is recognized. 
*        VCommand doesn't take the ownership
* @param aUi visible strings to be displayed in VC App. 
*        VCommand doesn't take the ownership
*/
EXPORT_C CVCommand* CVCommand::NewL( const TDesC& aText, const CVCRunnable& aRunnable, 
                            const CVCCommandUi& aUi )
    {
    RUBY_DEBUG_BLOCKL( "CVCCommand::NewL with args" );
    CVCommand* self = new (ELeave) CVCommand;
    CleanupStack::PushL( self );
    self->ConstructL( aText, aRunnable, aUi );
    CleanupStack::Pop( self );
    return self;
    }
    
/** 
* Internalizes the command from stream 
*/
EXPORT_C CVCommand* CVCommand::NewL( RReadStream &aStream )
    {
    CVCommand* self = new (ELeave) CVCommand;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;    
    }

EXPORT_C CVCommand* CVCommand::NewL( const CVCommand& aOriginal )
    {
    // Original knows better how to copy itself
    return aOriginal.CloneL();    
    }

void CVCommand::ConstructL( RReadStream &aStream )
	{
	TInt32 ver;
    aStream >> ver;
    __ASSERT_ALWAYS( ver == KVCommandProtocolVersion, User::Leave( KErrNotSupported  ) );
    iSpokenText = HBufC::NewL( aStream.ReadInt32L() );
    TPtr pSpokenText = iSpokenText->Des();
    aStream >> pSpokenText;
    iRunnable = CVCRunnable::NewL( aStream );
    iCommandUi = CVCCommandUi::NewL( aStream );
	}
	
void CVCommand::ConstructL( const TDesC& aText, const CVCRunnable& aRunnable, 
                            const CVCCommandUi& aUi )
    {
    __ASSERT_ALWAYS( aText.Length() > 0, User::Leave( KErrUnderflow ) );
    iSpokenText = HBufC::NewL( aText.Length() );
    *iSpokenText = aText;
  	iRunnable = CVCRunnable::NewL( aRunnable );
    iCommandUi = CVCCommandUi::NewL( aUi );    
    }
	
	
EXPORT_C CVCommand::~CVCommand()
    {
    delete iCommandUi;
    delete iRunnable;
    delete iSpokenText;
    }

CVCommand* CVCommand::CloneL() const
    {
    CVCommand* clone = CVCommand::NewL( SpokenText(), Runnable(), CommandUi() );
    return clone;
    }
    
/**
* Saves the command to stream 
* Format: <protocol version: int32><SpokenTextLength: int32><SpokenText descriptor>
*         <runnable><commandui>
*/
EXPORT_C void CVCommand::ExternalizeL( RWriteStream &aStream ) const
    {
    aStream << KVCommandProtocolVersion;
    aStream << static_cast<TInt32>( iSpokenText->Length() );
    aStream << *iSpokenText;
    aStream << *iRunnable;
    aStream << *iCommandUi;
    }
    
EXPORT_C TBool CVCommand::operator==( const CVCommand& aCommand ) const
    {
    return (
            ( *iSpokenText == aCommand.SpokenText() ) &&
            ( *iRunnable == aCommand.Runnable() ) &&
            ( *iCommandUi == aCommand.CommandUi() ) 
           );
    
    }	


/**
* @return the Runnable component of the command
*/
EXPORT_C const CVCRunnable& CVCommand::Runnable() const
    {
    return *iRunnable;
    }

/**
* @return the UI-related component of the command
*/
EXPORT_C const CVCCommandUi& CVCommand::CommandUi() const
    {
    return *iCommandUi;
    }

/**
* @return the text, that user is expected to pronounce
*/
EXPORT_C const TDesC& CVCommand::SpokenText() const
    {
    return *iSpokenText;
    }
    
/**
* @return the user specified text, that user can pronounce to
*         recognize the command. Is always identical to the 
*         CVCommandUi::UserText()
*         Can be KNullDesC
*/
EXPORT_C const TDesC& CVCommand::AlternativeSpokenText() const
    {
    return iCommandUi->UserText();
    }
    
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
EXPORT_C void CVCommand::PlaySpokenTextL( const CVCommandHandler& /*aHandler*/, 
                          MNssPlayEventHandler& /*aPlayEventHandler*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CVCommand::PlaySpokenTextL" );
    RUBY_ERROR0( "Attempt to play never added command" );
    User::Leave( KErrBadHandle );
    }

/**
* Asynchronous
* Plays back the user-specified alternative spoken text. 
* Otherwise is identical to PlaySpokenTextL
* @see PlaySpokenTextL
* @leave KErrNotFound if this command cannot be found in aHandler of if 
*        it doesn't have a user-specified text
*/     
EXPORT_C void CVCommand::PlayAlternativeSpokenTextL( const CVCommandHandler& /*aHandler*/, 
                          MNssPlayEventHandler& /*aPlayEventHandler*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CVCommand::PlaySpokenTextL" );
    RUBY_ERROR0( "Attempt to play never added command" );
    User::Leave( KErrBadHandle );
    }
    
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
EXPORT_C void CVCommand::CancelPlaybackL( const CVCommandHandler& /*aHandler*/ ) const
    {
    RUBY_DEBUG_BLOCK( "CVCommand::CancelPlaybackL" );
    RUBY_ERROR0( "CVCommand::CancelPlaybackL Attempt to cancel playback of the command\
                  never added to the VCommandHandler" );
    User::Leave( KErrBadHandle );
    }

/**
 * Tells if all non user-changeable parts of aCommand are equal to this command
 * @return ETrue if equal, EFalse otherwise
 */
EXPORT_C TBool CVCommand::EqualNonUserChangeableData( const CVCommand& aCommand ) const
    {
    if( SpokenText() != aCommand.SpokenText() ) return EFalse;
    if( !(Runnable() == aCommand.Runnable() ) ) return EFalse;
    
    return CommandUi().EqualNonUserChangeableData( aCommand.CommandUi() );
    }
    
//End of file
