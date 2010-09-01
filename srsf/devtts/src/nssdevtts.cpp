/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CDevTTS method implementations.
*
*/


// INCLUDE FILES
#include "srsfbldvariant.hrh"
#include "nssdevtts.h"
#include "devttsalgorithmmanager.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDevTTS::CDevTTS
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevTTS::CDevTTS() : iImpl(NULL)
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CDevTTS::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevTTS::ConstructL( MDevTTSObserver& aObserver/*, CConsoleBase* console*/ )
    {
    iImpl = CTTSAlgorithmManager::NewL( aObserver/*, console*/ );
    }

// -----------------------------------------------------------------------------
// CDevTTS::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDevTTS* CDevTTS::NewL( MDevTTSObserver& aObserver/*, CConsoleBase* console*/ )
    {
    CDevTTS* self = new( ELeave ) CDevTTS;
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver/*, console*/ );
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CDevTTS::~CDevTTS
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDevTTS::~CDevTTS()
    {
    if (iImpl != NULL) 
        {
        delete iImpl;
        iImpl = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CDevTTS::AddStyleL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyleID CDevTTS::AddStyleL( const TTtsStyle& aStyle )
    {
    return iImpl->AddStyleL( aStyle );
    }

// -----------------------------------------------------------------------------
// CDevTTS::Balance
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDevTTS::Balance()
    {
    return iImpl->Balance();
    }

// -----------------------------------------------------------------------------
// CDevTTS::BufferProcessed
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::BufferProcessed( const TDesC8& aBuffer )
    {
    iImpl->BufferProcessed( aBuffer );
    }

// -----------------------------------------------------------------------------
// CDevTTS::CustomInterface
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CDevTTS::CustomInterface( TUid aInterfaceID )
    {
    return iImpl->CustomInterface( aInterfaceID );
    }

// -----------------------------------------------------------------------------
// CDevTTS::DeleteStyleL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::DeleteStyleL( TTtsStyleID aStyleID )
    {
    iImpl->DeleteStyleL( aStyleID );
    }

// -----------------------------------------------------------------------------
// CDevTTS::GetPositionL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::GetPositionL( TTimeIntervalMicroSeconds& aTime ) const
    {
    iImpl->GetPositionL( aTime );
    }

// -----------------------------------------------------------------------------
// CDevTTS::GetPositionL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::GetPositionL( TTtsSegment& aSegment, TInt& aWordIndex ) const
    {
    iImpl->GetPositionL( aSegment, aWordIndex );
    }

// -----------------------------------------------------------------------------
// CDevTTS::IsLanguageSupported
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CDevTTS::IsLanguageSupported( TLanguage aLanguage ) const
    {
    return iImpl->IsLanguageSupported( aLanguage );
    }

// -----------------------------------------------------------------------------
// CDevTTS::MaxVolume
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDevTTS::MaxVolume()
    {
    return iImpl->MaxVolume();
    }

// -----------------------------------------------------------------------------
// CDevTTS::StyleL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyle& CDevTTS::StyleL( TTtsStyleID aStyleID ) const
    {
    return iImpl->StyleL( aStyleID );
    }

// -----------------------------------------------------------------------------
// CDevTTS::StyleL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TTtsStyle& CDevTTS::StyleL( TUint16 aIndex ) const
    {
    return iImpl->StyleL( aIndex );
    }

// -----------------------------------------------------------------------------
// CDevTTS::NumberOfStyles
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint16 CDevTTS::NumberOfStyles() const
    {
    return iImpl->NumberOfStyles();
    }

// -----------------------------------------------------------------------------
// CDevTTS::Pause
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::Pause()
    {
    iImpl->Pause();
    }

// -----------------------------------------------------------------------------
// CDevTTS::PrimeSynthesisL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::PrimeSynthesisL( MTtsSegmentStream& aStream ) 
    {
    iImpl->PrimeSynthesisL( aStream );
    }

// -----------------------------------------------------------------------------
// CDevTTS::PrimeSynthesisL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::PrimeSynthesisL( CTtsParsedText& aText )
    {
    iImpl->PrimeSynthesisL( aText );
    }

// -----------------------------------------------------------------------------
// CDevTTS::SetAudioPriority
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SetAudioPriority( TInt aPriority, TDevTTSAudioPreference aPref )
    {
    iImpl->SetAudioPriority( aPriority, aPref );
    }

// -----------------------------------------------------------------------------
// CDevTTS::SetAudioOutputL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SetAudioOutputL( TInt aAudioOutput )
    {
    iImpl->SetAudioOutputL( aAudioOutput );
    }
    
// -----------------------------------------------------------------------------
// CDevTTS::SetBalance
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SetBalance( TInt aBalance )
    {
    iImpl->SetBalance( aBalance );
    }

// -----------------------------------------------------------------------------
// CDevTTS::SetPositionL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SetPositionL( const TTimeIntervalMicroSeconds& aTime )
    {
    iImpl->SetPositionL( aTime );
    }

// -----------------------------------------------------------------------------
// CDevTTS::SetPositionL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SetPositionL( const TTtsSegment& aSegment, TInt aWordIndex )
    {
    iImpl->SetPositionL( aSegment, aWordIndex );
    }

// -----------------------------------------------------------------------------
// CDevTTS::SetVolume
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SetVolume( TInt aVolume )
    {
    iImpl->SetVolume( aVolume );
    }

// -----------------------------------------------------------------------------
// CDevTTS::SetVolumeRamp
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration )
    {
    iImpl->SetVolumeRamp( aRampDuration );
    }

// -----------------------------------------------------------------------------
// CDevTTS::Stop
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::Stop()
    {
    iImpl->Stop();
    }

// -----------------------------------------------------------------------------
// CDevTTS::SynthesizeL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::SynthesizeL( TDevTTSOutputMode aOutputMode ) 
    {
    iImpl->SynthesizeL( aOutputMode );
    }

// -----------------------------------------------------------------------------
// CDevTTS::Volume
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CDevTTS::Volume()
    {
    return iImpl->Volume();
    }


// -----------------------------------------------------------------------------
// CDevTTS::LanguageIdentificationL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::LanguageIdentificationL( CTtsParsedText& aText, 
                                                TInt aNumberOfGuesses, 
                                                RArray<TLanguage>& aLanguages,
                                                RArray<TInt>& aScores )
    {
    iImpl->LanguageIdentificationL( aText, aNumberOfGuesses, aLanguages, aScores );
    }

// -----------------------------------------------------------------------------
// CDevTTS::NormalizeTextL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::NormalizeTextL( CTtsParsedText& aText )
    {
    iImpl->NormalizeTextL( aText );
    }

// -----------------------------------------------------------------------------
// CDevTTS::NormalizeAndSegmentTextL
// Forwards call to the actual implementation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDevTTS::NormalizeAndSegmentTextL( CTtsParsedText& aText )
    {
    iImpl->NormalizeAndSegmentTextL( aText );
    }


// End of File  
