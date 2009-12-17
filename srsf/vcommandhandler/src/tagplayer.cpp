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
* Description:  Plays voice commands
*
*/


#include "rubydebug.h"
#include "tagplayer.h"

_LIT( KStoredVCommandPlayerPanic, "SVCP" );

CTagPlayer* CTagPlayer::NewL()
    {
    CTagPlayer* self = new (ELeave) CTagPlayer;
    return self;
    }
    
CTagPlayer::~CTagPlayer()
    {
    // if any
    delete iPlaybackTag;  
    // to panic in HandlePlays if they are called after the object destruction
    iPlaybackTag = NULL;  
    }

/**
* Attempts to play back the text expected to be recognized. 
* To be playable command has to be added to CVCommandHandler AND
* then retrieved back. This function is asynchronous
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
void CTagPlayer::PlayTagL( MNssTag* aTag, MNssPlayEventHandler& aPlayEventHandler )
    {
    RUBY_DEBUG_BLOCK( "CTagPlayer::PlayTagL" );
    CleanupDeletePushL( aTag );
    __ASSERT_ALWAYS( !iPlaybackTag, User::Leave( KErrLocked ) );
    CleanupStack::Pop( aTag );
    iPlaybackTag = aTag;
    iPlayEventHandler = &aPlayEventHandler;

    MNssSpeechItem::TNssSpeechItemResult err = iPlaybackTag->SpeechItem()->PlayL( this );
    switch( err ) 
        {
        case MNssSpeechItem::EVasErrorNone:
            break;
        case MNssSpeechItem::EVasUnexpectedRequest:
            User::Leave( KErrNotReady );
        case MNssSpeechItem::EVasInUse:
            User::Leave( KErrInUse );
        case MNssSpeechItem::EVasInvalidParameter:
            User::Leave( KErrArgument );
        case MNssSpeechItem::EVasPlayFailed:
            User::Leave( KErrGeneral );
        case MNssSpeechItem::EVasTrainFailed:
        default:
            RUBY_ERROR1( "CTagPlayer::PlaySpokenTextL Unexpected error code [%d]", err );
            User::Leave( KErrGeneral );
        }
    }
    
/**
* Cancels playback. No PlayStarted/PlayComplete after it
*
* @leave KErrNotReady if playback has never been started
*/                                   
void CTagPlayer::CancelPlaybackL()
    {
    RUBY_DEBUG_BLOCK( "CTagPlayer::CancelPlaybackL" );
    __ASSERT_ALWAYS( iPlaybackTag, User::Leave( KErrNotReady ) );
    TInt err = iPlaybackTag->SpeechItem()->CancelL();
    RUBY_DEBUG1( "CTagPlayer::CancelPlaybackL Canceled with the error code [%d]", err );
    delete iPlaybackTag;
    iPlaybackTag = NULL;
    iPlayEventHandler = NULL;
    }

// From MNsPlayEventHandler

/**
* The HandlePlayStarted method is a virtual method implemented by the
* client and is called when play is started
* @param aDuration - the duration of the utterance data
*/       
void CTagPlayer::HandlePlayStarted( TTimeIntervalMicroSeconds32 aDuration )
    {
    __ASSERT_ALWAYS( iPlaybackTag, User::Panic( KStoredVCommandPlayerPanic, KErrNotReady ) );
    iPlayEventHandler->HandlePlayStarted( aDuration );
    }

/**
* The HandlePlayComplete method is a virtual method implemented by the
* client and is called when play is completed
* @param aErrorCode EVasErrorNone if playing was successfull
*/       
void CTagPlayer::HandlePlayComplete( TNssPlayResult aErrorCode )
    {
    __ASSERT_ALWAYS( iPlaybackTag, User::Panic( KStoredVCommandPlayerPanic, KErrNotReady ) );
    delete iPlaybackTag;
    iPlaybackTag = NULL;
    iPlayEventHandler->HandlePlayComplete( aErrorCode );
    iPlayEventHandler = NULL;
    }

//End of file
