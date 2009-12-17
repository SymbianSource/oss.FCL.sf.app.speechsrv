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
* Description:  Implementation of CVCSettingsEngine
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <aknsoundsystem.h>
#include <aknappui.h>

#include <vcommand.rsg>
#include "vcsettingsengine.h"
#include "vcommandconstants.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCSettingsEngine::CVCSettingsEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVCSettingsEngine::CVCSettingsEngine()
    {
    }

// -----------------------------------------------------------------------------
// CVCSettingsEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVCSettingsEngine::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("[Voice commands] -- CVCSettingsEngine::ConstructL"));
#endif

    iVasDbManager = CNssVASDBMgr::NewL();
    iVasDbManager->InitializeL();
    }

// -----------------------------------------------------------------------------
// CVCSettingsEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCSettingsEngine* CVCSettingsEngine::NewL()
    {
    CVCSettingsEngine* self = new( ELeave ) CVCSettingsEngine;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CVCSettingsEngine::~CVCSettingsEngine()
    {
    delete iVasDbManager;
	iContextManager = NULL;
    }


// -----------------------------------------------------------------------------
// CVCSettingsEngine::ResetAdaptationL
// -----------------------------------------------------------------------------
//
void CVCSettingsEngine::ResetAdaptationL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("[Voice commands] -- CVCSettingsEngine::ResetAdaptationL"));
#endif

    iVasDbManager->ResetFactoryModelsL( this );
	iShedulerWait.Start();
    }


// -----------------------------------------------------------------------------
// CVCSettingsEngine::HandleResetComplete
// -----------------------------------------------------------------------------
//
void CVCSettingsEngine::HandleResetComplete( TInt aErrorCode )
    {
#ifdef _DEBUG
    RDebug::Print(_L("[Voice commands] -- CVCSettingsEngine::HandleResetComplete"));
#endif

    if( aErrorCode != KErrNone ) 
        {
        return HandleResetFailed();
        }

    TRAP_IGNORE( DoHandleResetCompleteL());

	if ( iShedulerWait.IsStarted() )
		{
		iShedulerWait.AsyncStop();
		}
	}

 
// -----------------------------------------------------------------------------
// CVCSettingsEngine::HandleResetComplete
// -----------------------------------------------------------------------------
//
void CVCSettingsEngine::DoHandleResetCompleteL()
    {
    HBufC* text = StringLoader::LoadLC( R_QTN_VC_CONF_NOTE_RESET );
    
    CAknConfirmationNote* dlg = new (ELeave) CAknConfirmationNote( ETrue );
    dlg->ExecuteLD( *text );
    
    CleanupStack::PopAndDestroy( text );  // text
    }

// -----------------------------------------------------------------------------
// CVCSettingsEngine::HandleResetFailed
// -----------------------------------------------------------------------------
//
void CVCSettingsEngine::HandleResetFailed()
    {
#ifdef _DEBUG
    RDebug::Print(_L("[Voice commands] -- CVCSettingsEngine::HandleResetFailed"));
#endif

    TRAP_IGNORE( DisplayErrorNoteL());

	if ( iShedulerWait.IsStarted() )
		{
		iShedulerWait.AsyncStop();
		}
    }


// -----------------------------------------------------------------------------
// CVCSettingsEngine::DisplayErrorNoteL
// -----------------------------------------------------------------------------
//
void CVCSettingsEngine::DisplayErrorNoteL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("[Voice commands] -- CVCSettingsEngine::DisplayErrorNoteL"));
#endif

    HBufC* text = StringLoader::LoadLC( R_TEXT_VOICE_SYSTEM_ERROR );
    CAknErrorNote* dlg = new (ELeave) CAknErrorNote( ETrue );
    dlg->ExecuteLD( *text );
    PlaySound( EAvkonSIDNameDiallerErrorTone );
    CleanupStack::PopAndDestroy( text ); // text
    }


// ---------------------------------------------------------
// CVCSettingsEngine::PlaySound
// ---------------------------------------------------------
//
void CVCSettingsEngine::PlaySound(TAvkonSystemSID aSound) const
    {
    if (GetSoundSystem())
        {
        GetSoundSystem()->PlaySound(aSound);
        }
    }


// ---------------------------------------------------------
// CVCSettingsEngine::SoundSystem
// ---------------------------------------------------------
//
CAknKeySoundSystem* CVCSettingsEngine::GetSoundSystem() const
    {
	if (iAvkonAppUiBase)
		{
		return iAvkonAppUiBase->KeySounds();	// Static access
		}

	return NULL;
    }

//  End of File
