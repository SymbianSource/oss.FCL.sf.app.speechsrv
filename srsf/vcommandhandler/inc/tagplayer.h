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
* Description:  Plays back voice commands
*
*/


 
#ifndef TAGPLAYER_H
#define TAGPLAYER_H

#include <nssvasmplayeventhandler.h>
#include <nssvasmtag.h>
#include <nssvasmgettagclient.h>  // for MNssTagListArray
#include "vcommandinternalapi.h"

/**
* Plays back the voice commad
*/
class CTagPlayer : public CBase, public MNssPlayEventHandler
    {
    public:
        
        static CTagPlayer* NewL();
        
        ~CTagPlayer();
        
        /**
        * Attempts to play back the text expected to be recognized. 
        * This object takes ownership on the aTag and will delete it upon
        * the playback completion.
        * This function is asynchronous
        *
        * @param aTag Tag to play. Will be deleted (not from VAS) upon the 
        *        playback completion
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
        virtual void PlayTagL( MNssTag* aTag, MNssPlayEventHandler& aPlayEventHandler );
        
        /**
        * Cancels playback. No PlayStarted/PlayComplete after it
        *
        * @leave KErrNotReady if playback has never been started
        */                                   
        virtual void CancelPlaybackL();
    
        // From MNsPlayEventHandler

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
        // Playback is asynchronous function. 
        // This variable is used to hold the tag being played back
        MNssTag*                    iPlaybackTag;
        
        // Playback is asynchronous function. 
        // This variable is used to hold the external play event
        // handler
        MNssPlayEventHandler*       iPlayEventHandler;

    };

#endif // TAGPLAYER_H