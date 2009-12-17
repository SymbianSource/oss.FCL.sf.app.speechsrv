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
* Description:  Voice command with the link to the VAS
*
*/



#include <e32def.h>
#include "vcommandinternalapi.h"
#include "rubydebug.h"

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
CStoredVCommand* CStoredVCommand::NewL( const TDesC& aText, const CVCRunnable& aRunnable, 
                                 const CVCCommandUi& aUi, TInt aCommandId )
    {
    CStoredVCommand* self = new (ELeave) CStoredVCommand;
    CleanupStack::PushL( self );
    self->ConstructL( aText, aRunnable, aUi, aCommandId );
    CleanupStack::Pop( self );
    return self;
    }

/** 
* Internalizes the command from stream 
* @leave KErrNotSupported if the stream data format is unsupported
*        e.g. if it has been written by newer implementation
*/
CStoredVCommand* CStoredVCommand::NewL( RReadStream &aStream, TInt aCommandId )
    {
    CStoredVCommand* self = new (ELeave) CStoredVCommand;
    CleanupStack::PushL( self );
    self->ConstructL( aStream, aCommandId );
    CleanupStack::Pop( self );
    return self;
    }
    
/**
* @see CVCommand::ConstructL
*/ 
void CStoredVCommand::ConstructL( const TDesC& aText, const CVCRunnable& aRunnable, 
                 const CVCCommandUi& aUi, TInt aCommandId ) 
    {
    CVCommand::ConstructL( aText, aRunnable, aUi );
    iCommandId = aCommandId;
    }
    
/**
* @see CVCommand::ConstructL
*/
void CStoredVCommand::ConstructL( RReadStream& aStream, TInt aCommandId )
    {
    CVCommand::ConstructL( aStream );
    iCommandId = aCommandId;
    }
    
/**
* Clone itself
* Is used for copying via NewL( CVCommand& aOriginal )
*/
CVCommand* CStoredVCommand::CloneL() const
    {
    CVCommand* clone = CStoredVCommand::NewL( SpokenText(), Runnable(), 
                                                CommandUi(), CommandId() );
    return clone;
    }

/**
* @return CommandId
*/
TInt CStoredVCommand::CommandId() const
    {
    return iCommandId;
    }

/**
* Asynchronous
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
void CStoredVCommand::PlaySpokenTextL( const CVCommandHandler& aHandler, 
                          MNssPlayEventHandler& aPlayEventHandler ) const
    {
    RUBY_DEBUG_BLOCK( "CStoredVCommand::PlaySpokenTextL" );
    aHandler.PlaySpokenTextL( *this, aPlayEventHandler );
    }

/**
* Plays back the user-specified alternative spoken text. 
* Otherwise is identical to PlaySpokenTextL
* @see PlaySpokenTextL
* @leave KErrNotFound if this command cannot be found in aHandler of if 
*        it doesn't have a user-specified text
*/     
void CStoredVCommand::PlayAlternativeSpokenTextL( const CVCommandHandler& aHandler, 
                          MNssPlayEventHandler& aPlayEventHandler ) const
    {
    RUBY_DEBUG_BLOCK( "CStoredVCommand::PlaySpokenTextL" );
    aHandler.PlayAlternativeSpokenTextL( *this, aPlayEventHandler );
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
void CStoredVCommand::CancelPlaybackL( const CVCommandHandler& aHandler ) const
    {
    aHandler.CancelPlaybackL( *this );
    }
    
//End of file
