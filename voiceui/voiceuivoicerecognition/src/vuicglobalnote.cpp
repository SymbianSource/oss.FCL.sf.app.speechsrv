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
* Description:  Handles tutorial mode resources and dialog showing
*
*/


// INCLUDE FILES
#include <featmgr.h>

#include <avkon.hrh>

#include <aknSDData.h>

#include <StringLoader.h>

#include <vuivoicerecognition.rsg>

#include "vuivoicerecognition.hrh"

#include "vuicglobalnote.h"

#include "rubydebug.h"

// -----------------------------------------------------------------------------
// CGlobalNote::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlobalNote* CGlobalNote::NewL()
    {
    CGlobalNote* self = new (ELeave) CGlobalNote();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor       
CGlobalNote::~CGlobalNote()
    {
    RUBY_DEBUG0( "CGlobalNote::~CGlobalNote START" );
    
    TRAP_IGNORE( DismissGlobalNoteL() );
    
    delete iNote;
    
    RUBY_DEBUG0( "CGlobalNote::~CGlobalNote EXIT" );
    }

// -----------------------------------------------------------------------------
// CGlobalNote::DisplayGlobalNoteL
// -----------------------------------------------------------------------------
//
void CGlobalNote::DisplayGlobalNoteL( TGlobalNoteState aState )
    {
    RUBY_DEBUG_BLOCK( "CGlobalNote::DisplayGlobalNoteL" );
    
    switch( aState )
        {
        case ECallInProgress:
            DisplayGlobalNoteL( EAknGlobalInformationNote, R_TEXT_CALL_IN_PROGRESS,
                                SecondaryDisplay::ECmdShowCallInProgressNote );
            break;
            
        case ESystemError:
            DisplayGlobalNoteL( EAknGlobalErrorNote, R_TEXT_VOICE_SYSTEM_ERROR,
                                SecondaryDisplay::ECmdShowVoiceSystemErrorNote );
            break;

        case ENoTag:
            DisplayGlobalNoteL( EAknGlobalInformationNote, R_TEXT_VOICE_NO_TAG,
                                SecondaryDisplay::ECmdShowVoiceNoTagNote );
            break;

        case ENoMatchPermanent:
            DisplayGlobalNoteL( EAknGlobalPermanentNote, R_TEXT_VOICE_NO_MATCHES,
                                SecondaryDisplay::ECmdShowVoiceNoMatchesNote,
                                EMbmAvkonQgn_note_voice,
                                EMbmAvkonQgn_note_voice_mask );
            break;

        case ENotRecognizedPermanent:
            DisplayGlobalNoteL( EAknGlobalPermanentNote, R_TEXT_VOICE_SYSTEM_ERROR,
                                SecondaryDisplay::ECmdShowVoiceNotRecognizedNote,
                                EMbmAvkonQgn_note_error,
                                EMbmAvkonQgn_note_error_mask );
            break;
            
        case ENoMemory:
            DisplayGlobalNoteL( EAknGlobalErrorNote, R_QTN_VC_MEMORY_LOW,
                                SecondaryDisplay::ECmdVoiceNoNote );
            break;
        default:
            RUBY_ERROR0( "CGlobalNote::DisplayGlobalNoteL - Invalid state" );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CGlobalNote::DismissGlobalNoteL
// -----------------------------------------------------------------------------
//
void CGlobalNote::DismissGlobalNoteL()
    {
    RUBY_DEBUG_BLOCK( "CGlobalNote::DismissGlobalNoteL" );
                
    // Dismiss message dialog
    iNote->CancelNoteL( iNoteId );
    }

// -----------------------------------------------------------------------------
// CGlobalNote::CGlobalNote
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGlobalNote::CGlobalNote()
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CGlobalNote::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlobalNote::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CGlobalNote::ConstructL" );

    iNote = CAknGlobalNote::NewL();
    }
    
// ---------------------------------------------------------
// CGlobalNote::DisplayGlobalNoteL
// Displays a global note
// ---------------------------------------------------------
//
void CGlobalNote::DisplayGlobalNoteL( TAknGlobalNoteType aNoteType,
                                      TInt aMsgId,
                                      SecondaryDisplay::TVUISecondaryDisplayDialogs aDialog,
                                      TMifAvkon aIcon,
                                      TMifAvkon aIconMask )
    {
    RUBY_DEBUG_BLOCK( "CGlobalNote::DisplayGlobalNoteL" );
    
    HBufC* noteText = StringLoader::LoadLC( aMsgId, CCoeEnv::Static() );    
    DisplayGlobalNoteL( aNoteType, noteText, aDialog, aIcon, aIconMask );
    CleanupStack::PopAndDestroy( noteText );
    }


// ---------------------------------------------------------
// CGlobalNote::DisplayGlobalNoteL
// Displays a global note
// ---------------------------------------------------------
//
void CGlobalNote::DisplayGlobalNoteL( TAknGlobalNoteType aNoteType,
                                      HBufC* aNoteText,
                                      SecondaryDisplay::TVUISecondaryDisplayDialogs aDialog,
                                      TMifAvkon aIcon,
                                      TMifAvkon aIconMask )
    {
    RUBY_DEBUG_BLOCK( "CGlobalNote::DisplayGlobalNoteL" );

    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) &&
         aDialog != SecondaryDisplay::ECmdVoiceNoNote )
        {
        CAknSDData* sddata = CAknSDData::NewL( SecondaryDisplay::KCatVoiceUi,
                                               aDialog,
                                               KNullDesC8 );
        iNote->SetSecondaryDisplayData( sddata );
        }
    
    if ( aIcon && aIconMask )
        {
        iNote->SetGraphic( aIcon, aIconMask );
        }
    
    iNote->SetTone( EAvkonSIDNoSound );
    iNoteId = iNote->ShowNoteL( aNoteType, *aNoteText );
    }

// End of File

