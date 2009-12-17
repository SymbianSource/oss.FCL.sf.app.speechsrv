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
* Description:  
*
*/


// INCLUDE FILES
#include <aknappui.h>
#include <AknUtils.h> 
#include <AknIconUtils.h> 
#include <cntdef.h>
#include <AknsConstants.h>
#include <StringLoader.h>
#include <AudioPreference.h>

#include <vasmbasepbkhandler.h>

#include <secondarydisplay/vuisecondarydisplayapi.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuicstate.h"
#include "vuicplaystate.h"
#include "vuictutorialstate.h"
#include "vuicresultsstate.h"
#include "vuicverificationinitstate.h"
#include "vuicabortstate.h"

#include "vuicdatastorage.h"

#include "vuicvoicerecogdialogimpl.h"
#include "vuicpropertyhandler.h"
#include "vuictoneplayer.h"
#include "vuicttsplayer.h"
#include "vuicglobalprogressdialog.h"
#include "vuivoiceicondefs.h"

#include "rubydebug.h"

// Constants
_LIT( KStringPosition, "%U" );

// -----------------------------------------------------------------------------
// CPlayState::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPlayState* CPlayState::NewL( CDataStorage& aDataStorage, CUiModel& aUiModel )
    {
    CPlayState* self = new (ELeave) CPlayState( aDataStorage, aUiModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }       
    
// Destructor       
CPlayState::~CPlayState()
    {
    RUBY_DEBUG0( "CPlayState::~CPlayState START" );
    
    DataStorage().TtsPlayer()->Stop();
    
    delete iProgressDialog;
    
    RUBY_DEBUG0( "CPlayState::~CPlayState EXIT" );
    }
    
// ---------------------------------------------------------
// CPlayState::HandleEventL
// ---------------------------------------------------------
//       
void CPlayState::HandleEventL( TInt aEvent )
    {
    RUBY_DEBUG_BLOCK( "CPlayState::HandleEventL" );

    CState* nextState = NULL;

    switch( aEvent )
        {
        case KErrNone:
        
            if ( iInternalState == EStarted )
                {
                iInternalState = EInitialized;
                nextState = this;
                }
            else if ( iInternalState == EInitialized )
                {
                iInternalState = ECompleted;
                nextState = this;
                }
            break;
        
        case KErrPathNotFound:
        
            // Ignore mysterious path not found error when loading confirmation tone
            if ( iInternalState == EStarted )
                {
                nextState = CTutorialState::NewL( DataStorage(), UiModel() );
                }
            break;
        
        case EVoiceTagSoftKeySelect:
        
            nextState = CTutorialState::NewL( DataStorage(), UiModel() );
            break;
            
        case EShortKeypress:
        case EVoiceTagSoftKeyOther:
        case EAknSoftkeyOther:
        case EAknSoftkeyDone:
        case EStdKeyDevice2:
        
            if ( iInternalState != EStarted )
                {
                if ( DataStorage().VerificationMode() != EVoice )
                    {
                    nextState = CResultsState::NewL( DataStorage(), UiModel() );
                    }
                else
                    {
                    if ( DataStorage().Tag()->Context()->ContextName() == KVoiceDialContext &&
                         DataStorage().Tag()->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial )
                        {
                        nextState = CVerificationInitState::NewL( DataStorage(), UiModel() );
                        }
                    }
                }
            break;
            
        case EEndCallKeypress:
        
            // Do nothing
            break;
            
        default:
        
            nextState = CAbortState::NewL( DataStorage(), UiModel() );
            break;
        }

    DataStorage().VoiceRecognitionImpl()->ChangeState( nextState );
    }

// ---------------------------------------------------------
// CPlayState::ExecuteL
// ---------------------------------------------------------
//       
void CPlayState::ExecuteL()
    {
    RUBY_DEBUG_BLOCK( "CPlayState::ExecuteL" );

    if ( iInternalState == ENotStarted )
        {
        TBuf<KMaxPhoneNumberLength> phoneNumber;        
        TFieldType fieldType;        
        TName name;
        
        iInternalState = EStarted;

        if ( DataStorage().Tag()->Context()->ContextName() == KVoiceDialContext )
            {
            if ( DataStorage().VerificationMode() == EVoice &&
                 DataStorage().Tag()->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial )
                {
                FormatDataL( name, phoneNumber, fieldType );

                ShowPlaydialogL( name, phoneNumber, fieldType );
                }
            else
                {
                TRAP_IGNORE( HandleEventL( EVoiceTagSoftKeySelect ) );
                }
            }
        else
            {
            DataStorage().TonePlayer()->InitToneL( EAvkonSIDConfirmationTone );
            }                
        }
    else if ( iInternalState == EInitialized )
        {
        if ( DataStorage().Tag()->Context()->ContextName() == KVoiceDialContext &&
             DataStorage().Tag()->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial )
            {
            iTModel.iFinalValue = DataStorage().PlayDuration().Int64() / KKRecogInterval + 4 * KRecogDelay;
            StartTimerL( *this, KRecogDelay, KKRecogInterval * iTModel.iHundreths );
            }
        else
            {
            DataStorage().TonePlayer()->PlayTone( EAvkonSIDConfirmationTone );
            }
        }
    else  if ( iInternalState == ECompleted )
        {
        if ( DataStorage().Tag()->Context()->ContextName() != KVoiceDialContext ||
             DataStorage().Tag()->RRD()->IntArray()->At( KVasExtensionRrdLocation ) != EDial )
            {
            TRAP_IGNORE( HandleEventL( EVoiceTagSoftKeySelect ) );
            }
        }
    }

// ---------------------------------------------------------
// CPlayState::CPlayState
// ---------------------------------------------------------
//              
CPlayState::CPlayState( CDataStorage& aDataStorage, CUiModel& aUiModel )
 : CState( aDataStorage, aUiModel ), iInternalState( ENotStarted ),
   iTickCount( 0 ), iPlaybackDelay( KPlaybackTimeout )
    {
    iTModel.iFinalValue = KRecogFinal;
    iTModel.iHundreths = KRecogInterval;
    iTModel.iIncrement = KRecogIncrement;
    iTModel.iRunning = EFalse;
    }

// ---------------------------------------------------------
// CPlayState::ConstructL
// ---------------------------------------------------------
//           
void CPlayState::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CPlayState::ConstructL" );
    
    CState::ConstructL();
    }
    
// ---------------------------------------------------------
// CPlayState::DoTimedEventL
// ---------------------------------------------------------
//
void CPlayState::DoTimedEventL()
    {
    if ( ( ++iTickCount < iTModel.iFinalValue + iPlaybackDelay ) && iProgressDialog )
        {
        iProgressDialog->UpdateProgressDialogL( iTickCount, iTModel.iFinalValue ); 
        }
    else 
        {
        HandleEventL( EVoiceTagSoftKeySelect );
        } 
    }
    
// ---------------------------------------------------------
// CPlayState::FormatName
// ---------------------------------------------------------
//           
void CPlayState::FormatName( TDes& aName )
    {
    RUBY_DEBUG0( "CPlayState::FormatName START" );
    
    const CFont* font = CEikonEnv::Static()->NormalFont();
    if ( AknLayoutUtils::Variant() == EApacVariant )
        {
        font = ApacPlain16();
        }

    TRect rect = iAvkonAppUi->ClientRect();
    TAknLayoutText layoutText;
    layoutText.LayoutText( rect, AKN_LAYOUT_TEXT_Wait_or_progress_note_pop_up_window_texts_Line_1(0,0,0) );
    TInt maxWidth = layoutText.TextRect().Width();

    AknTextUtils::ClipToFit( aName, *font, maxWidth, AknTextUtils::EClipFromEnd );
    
    RUBY_DEBUG0( "CPlayState::FormatName EXIT" );
    }     
    
// ---------------------------------------------------------------------------
// CPlayState::PlayVoiceTagL
// ---------------------------------------------------------------------------
//
void CPlayState::PlayVoiceTagL()
    {
    RUBY_DEBUG_BLOCK( "CPlayState::PlayVoiceTagL" );
    
    // if TTS is used
    if ( DataStorage().SynthesizerMode() != KErrNone ||
         DataStorage().DeviceLockMode() )
        {            
        // Add text "dialling" in front of the name if using voice dialing
        if ( DataStorage().Tag()->Context()->ContextName() == KVoiceDialContext &&
             DataStorage().Tag()->RRD()->IntArray()->At( KVasExtensionRrdLocation ) == EDial )
            {
            HBufC* ttsText = StringLoader::LoadLC( R_QAN_VC_TTS_DIALLING );
            
            TInt position = ttsText->FindC( KStringPosition );
            User::LeaveIfError( position );            
            
            CleanupStack::PopAndDestroy( ttsText );
                                                   
            ttsText = StringLoader::LoadLC( R_QAN_VC_TTS_DIALLING,
                                            DataStorage().Tag()->SpeechItem()->Text() );
                                                   
            HBufC* text = DataStorage().Tag()->SpeechItem()->PartialTextL( KNameTrainingIndex );        
            CleanupStack::PushL( text ); 
            
            DataStorage().TtsPlayer()->PlayL( *ttsText, position,
                                              DataStorage().Tag()->SpeechItem()->Text().Length(),
                                              *text );
            
            CleanupStack::PopAndDestroy( text );
            CleanupStack::PopAndDestroy( ttsText );
            }
        else if ( DataStorage().Tag()->Context()->ContextName() == KVoiceDialContext )
            {           
            HBufC* text = DataStorage().Tag()->SpeechItem()->PartialTextL( KNameTrainingIndex );        
            CleanupStack::PushL( text ); 
        
            DataStorage().TtsPlayer()->PlayL( DataStorage().Tag()->SpeechItem()->Text(), 0,
                                              DataStorage().Tag()->SpeechItem()->Text().Length(),
                                              *text );

            CleanupStack::PopAndDestroy( text );
            }
        else
            {            
            DataStorage().TtsPlayer()->PlayL( DataStorage().Tag()->SpeechItem()->Text() );
            }
        }
    else
        {
        // Set playback delay so that dialog will be dismissed immediately when
        // progess bar reaches final value
        iPlaybackDelay = 1;
                
        // Show playback dialog without TTS.
        DataStorage().SetPlayDuration( KTimeoutMicroseconds );
        HandleEventL( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CPlayState::FormatDataL
// ---------------------------------------------------------------------------
//
void CPlayState::FormatDataL( TDes& aName, TDes& aPhoneNumber, TFieldType& aFieldType )
    {
    RUBY_DEBUG_BLOCK( "CPlayState::FormatDataL" );
    
    TPtrC phoneNumber( KNullDesC );
    aPhoneNumber = (TDes&) phoneNumber;
        
    aFieldType = KNullUid;
    
    aName.Zero();
        
    if ( DataStorage().Tag()->Context()->ContextName() == KVoiceDialContext )
        {
        HBufC* name = DataStorage().Tag()->SpeechItem()->PartialTextL( KNameTrainingIndex );
        CleanupStack::PushL( name );
        
        aName.Copy( *name );
            
        CleanupStack::PopAndDestroy( name );
        
        // Format name so that it fits on one line
        FormatName( aName );
 
        DataStorage().PbkHandler()->FindContactFieldL( DataStorage().Tag() );

        // Format phone number
        phoneNumber.Set( DataStorage().PbkHandler()->TextL() );
        aPhoneNumber = (TDes&) phoneNumber;
        AknTextUtils::LanguageSpecificNumberConversion( aPhoneNumber );
 
        aFieldType = DataStorage().PbkHandler()->FieldTypeL();      
        }
    else
        {
        aName.Copy( DataStorage().Tag()->SpeechItem()->Text() );
        }
    }  

// ---------------------------------------------------------------------------
// CPlayState::ShowPlaydialogL
// ---------------------------------------------------------------------------
//
void CPlayState::ShowPlaydialogL( const TDesC& aName, const TDesC& aPhoneNumber,
                                  const TFieldType aFieldType )
    {
    RUBY_DEBUG_BLOCK( "CPlayState::ShowPlaydialogL" );
    
    // Get icon information
    TInt iconId;
    TInt iconMaskId;
    TAknsItemID iconSkinId;
    GetDialogIcon( aFieldType, iconId, iconMaskId, iconSkinId );

    iProgressDialog = CGlobalProgressDialog::NewL();

    // Set icon, image and skin for the playback dialog
    iProgressDialog->SetIconL( AknIconUtils::AvkonIconFileName(),
                               iconId, iconMaskId );
    
    iProgressDialog->SetImageL( AknIconUtils::AvkonIconFileName(),
                                EMbmAvkonQgn_note_voice_found,
                                EMbmAvkonQgn_note_voice_found_mask );

    iProgressDialog->SetSkinIds( KAknsIIDQgnNoteVoiceFound, EMbmAvkonQgn_note_voice_found,
                                 EMbmAvkonQgn_note_voice_found_mask, iconSkinId );
   
    iProgressDialog->RegisterForKeyCallback( DataStorage().VoiceRecognitionImpl() );
        
    // Package dialog data for coverui
    SecondaryDisplay::TMatchData matchData;
    matchData.iType = aFieldType;
    matchData.iName.Append( aName );
    matchData.iNumber.Append( aPhoneNumber );

    // Publish the data for the secondary display
    iProgressDialog->SetSecondaryDisplayDataL( SecondaryDisplay::KCatVoiceUi,
                                               SecondaryDisplay::ECmdShowMatchedItemNote,
                                               &matchData );
                                               
    BringToForeground();

    iProgressDialog->ShowProgressDialogL( aName, aPhoneNumber, R_AVKON_SOFTKEYS_OTHER_QUIT );
           
    // Play selected name using TTS
    PlayVoiceTagL();
    }    
    
// ---------------------------------------------------------
// CPlayState::GetDialogIcon
// Chooses appropriate dialog icon
// ---------------------------------------------------------
//
void CPlayState::GetDialogIcon( const TFieldType& aType, TInt& aIcon,
                                TInt& aIconMask, TAknsItemID& aIconId )
   {   
   // select a phone type bitmap
   if ( aType == KUidContactFieldVCardMapVOICE )
      {
      aIcon = KBitmapId[ EIconPhone ];
      aIconMask = KMaskId[ EIconPhone ];
      aIconId = *KIconId[ EIconPhone ];
      }
   else if ( aType == KUidContactFieldVCardMapCELL )
      {
      aIcon = KBitmapId[ EIconMobile ];
      aIconMask = KMaskId[ EIconMobile ];
      aIconId = *KIconId[ EIconMobile ];
      }
   else if ( aType == KUidContactFieldVCardMapFAX )
      {
      aIcon = KBitmapId[ EIconFax ];
      aIconMask = KMaskId[ EIconFax ];
      aIconId = *KIconId[ EIconFax ];
      }
   else if ( aType == KUidContactFieldVCardMapEMAILINTERNET )
      {
      aIcon = KBitmapId[ EIconEmail ];
      aIconMask = KMaskId[ EIconEmail ];
      aIconId = *KIconId[ EIconEmail ];
      }
   else if ( aType == KUidContactFieldVCardMapVIDEO )
      {
      aIcon = KBitmapId[ EIconVideo ];
      aIconMask = KMaskId[ EIconVideo ];
      aIconId = *KIconId[ EIconVideo ];
      }
   else if ( aType == KUidContactFieldVCardMapVOIP )
      {
      aIcon = KBitmapId[ EIconVoip ];
      aIconMask = KMaskId[ EIconVoip ];
      aIconId = *KIconId[ EIconVoip ];
      }   
   else
      {
      aIcon = KBitmapId[ EIconBlank ];
      aIconMask = KMaskId[ EIconBlank ];
      aIconId = *KIconId[ EIconBlank ];
      }
   }

// End of File

