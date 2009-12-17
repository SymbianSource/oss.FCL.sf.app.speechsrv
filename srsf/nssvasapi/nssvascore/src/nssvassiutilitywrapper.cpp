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
* Description:  Class which hides the dynamic loading of 
*               nsssispeechrecognitionutility.dll
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <nsssiutilitybase.h>
#include "rubydebug.h"
#include "nssvassiutilitywrapper.h"

// CONSTANTS
// DLL name
_LIT( KUtilityFilename, "nsssispeechrecognitionutility.dll" );
// CreateInstanceL function ordinal number
const TInt KFunctionOrdinal = 1;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssSiUtilityWrapper::NewL
// Two-phased constructor
// ---------------------------------------------------------
//
CNssSiUtilityWrapper* CNssSiUtilityWrapper::NewL( 
        MSISpeechRecognitionUtilityObserver& aObserver, TUid aClientUid )
    {
    CNssSiUtilityWrapper* self = new ( ELeave ) CNssSiUtilityWrapper( aObserver, aClientUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::~CNssSiUtilityWrapper
// Destructor
// ---------------------------------------------------------
//
CNssSiUtilityWrapper::~CNssSiUtilityWrapper()
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::~CNssSiUtilityWrapper" );

    Cancel();

    delete iUtility;
    // Close RLibrary handle
    iLib.Close();
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::CNssSiUtilityWrapper
// C++ constructor
// ---------------------------------------------------------
//
CNssSiUtilityWrapper::CNssSiUtilityWrapper( 
        MSISpeechRecognitionUtilityObserver& aObserver, TUid aClientUid ) : 
                                            CActive( EPriorityStandard ), 
                                            iClientUid( aClientUid ), 
                                            iObserver( aObserver )
    {
    // Nothing
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::ConstructL
// Second phase constructor
// ---------------------------------------------------------
//
void CNssSiUtilityWrapper::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CNssSiUtilityWrapper::ConstructL" );
    
    // Load a DLL based on name with RLibrary
    User::LeaveIfError( iLib.Load( KUtilityFilename ) );
    
    // Find the TAny* CreateInstanceL() function
    TLibraryFunction entry = iLib.Lookup( KFunctionOrdinal );
    if ( !entry )
        {
        User::Leave( KErrNotFound );
        }    
  
    // Call CreateInstanceL  
    // Cannot use static_cast since entry() call returns TInt    
    iUtility = ( MSISpeechRecognitionUtilityBase* ) entry();
     
    // Do the 2nd phase construction
    iUtility->CreateInstanceSecondPhaseL( iClientUid, KNullUid, *this );
    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::Adapt
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::Adapt( const CSIClientResultSet& aResultSet,
                                  TInt aCorrect )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::Adapt" );
    return iUtility->Adapt( aResultSet, aCorrect );
    }
    
// ---------------------------------------------------------
// CNssSiUtilityWrapper::AddVoiceTags
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::AddVoiceTags( const RPointerArray<MDesCArray>& aTextArrayArray,
                                         const RArray<TLanguage>& aLanguageArray,
                                         TSILexiconID aLexiconID,
                                         TSIGrammarID aGrammarID, 
                                         RArray<TSIRuleID>& aRuleIDArray )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::AddVoiceTags" );
    return iUtility->AddVoiceTags( aTextArrayArray, aLanguageArray, 
                                   aLexiconID, aGrammarID, aRuleIDArray );
    }
    
// ---------------------------------------------------------
// CNssSiUtilityWrapper::AddVoiceTags
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::AddVoiceTags( const RPointerArray<MDesCArray>& aTextArrayArray,
                                         const RArray<RLanguageArray>& aLanguageArray,
                                         TSILexiconID aLexiconID,
                                         TSIGrammarID aGrammarID, 
                                         RArray<TSIRuleID>& aRuleIDArray )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::AddVoiceTags" );
    return iUtility->AddVoiceTags( aTextArrayArray, aLanguageArray, 
                                   aLexiconID, aGrammarID, aRuleIDArray );
    }
    
// ---------------------------------------------------------
// CNssSiUtilityWrapper::CancelUtility
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
void CNssSiUtilityWrapper::CancelUtility()
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::Cancel" );
    return iUtility->Cancel();
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::CommitChanges
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::CommitChanges()
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::CommitChanges" );
    return iUtility->CommitChanges();
    }

    
// ---------------------------------------------------------
// CNssSiUtilityWrapper::CreateGrammar
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::CreateGrammar( TSIGrammarID& aGrammarID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::CreateGrammar" );
    return iUtility->CreateGrammar( aGrammarID );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::CreateLexicon
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::CreateLexicon( TSILexiconID& aLexiconID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::CreateLexicon" );
    return iUtility->CreateLexicon( aLexiconID );
    }
        
// ---------------------------------------------------------
// CNssSiUtilityWrapper::CreateModelBank
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::CreateModelBank( TSIModelBankID& aModelBankID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::CreateModelBank" );
    return iUtility->CreateModelBank( aModelBankID );
    }
        

// ---------------------------------------------------------
// CNssSiUtilityWrapper::EndRecSession
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::EndRecSession()
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::EndRecSession" );
    return iUtility->EndRecSession();
    }


// ---------------------------------------------------------
// CNssSiUtilityWrapper::LoadGrammar
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::LoadGrammar( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::LoadGrammar" );
    return iUtility->LoadGrammar( aGrammarID );
    }
        

// ---------------------------------------------------------
// CNssSiUtilityWrapper::LoadModels
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::LoadModels( TSIModelBankID aModelBankID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::LoadModels" );
    return iUtility->LoadModels( aModelBankID );
    }


// ---------------------------------------------------------
// CNssSiUtilityWrapper::Record
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::Record( TTimeIntervalMicroSeconds32 aRecordTime )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::Record" );
    return iUtility->Record( aRecordTime );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::PreStartSampling
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::PreStartSampling()
    {
    RUBY_DEBUG0( "" );
    return iUtility->PreStartSampling();
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::Recognize
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::Recognize( CSIClientResultSet& aResultSet )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::Recognize" );    
    return iUtility->Recognize( aResultSet ); 
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::RemoveGrammar
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::RemoveGrammar( TSIGrammarID aGrammarID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::RemoveGrammar" );   
    return iUtility->RemoveGrammar( aGrammarID );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::RemoveLexicon
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::RemoveLexicon( TSILexiconID aLexiconID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::RemoveLexicon" );  
    return iUtility->RemoveLexicon( aLexiconID );
    }


// ---------------------------------------------------------
// CNssSiUtilityWrapper::RemoveModelBank
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::RemoveModelBank( TSIModelBankID aModelBankID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::RemoveModelBank" );      
    return iUtility->RemoveModelBank( aModelBankID );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::RemoveRule
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::RemoveRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::RemoveRule" );  
    return iUtility->RemoveRule( aGrammarID, aRuleID );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::RemoveRules
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::RemoveRules( TSIGrammarID aGrammarID, RArray<TSIRuleID>& aRuleIDs )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::RemoveRules" );
    return iUtility->RemoveRules( aGrammarID, aRuleIDs );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::SetAudioPriority
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::SetAudioPriority( TInt aPriority, TInt aTrainPreference, 
                        TInt aPlaybackPreference, TInt aRecognitionPreference )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::SetAudioPriority" );
    return iUtility->SetAudioPriority( aPriority, aTrainPreference, 
                                aPlaybackPreference, aRecognitionPreference );
    }
        
// ---------------------------------------------------------
// CNssSiUtilityWrapper::SetEventHandler
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
void CNssSiUtilityWrapper::SetEventHandler( MSISpeechRecognitionUtilityObserver*
                                            aSpeechRecognitionUtilityObserver )
    {
    iObserver = *aSpeechRecognitionUtilityObserver;
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::StartRecSession
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::StartRecSession( TNSSRecognitionMode aMode )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::StartRecSession" );
    return iUtility->StartRecSession( aMode );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::UnloadRule
// Forwards the call to Recognition Utility
// ---------------------------------------------------------
//
TInt CNssSiUtilityWrapper::UnloadRule( TSIGrammarID aGrammarID, TSIRuleID aRuleID )
    {
    RUBY_DEBUG0( "CNssSiUtilityWrapper::UnloadRule" );
    return iUtility->UnloadRule( aGrammarID, aRuleID );
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::MsruoEvent
// Callback from SI Utility
// ---------------------------------------------------------
//
void CNssSiUtilityWrapper::MsruoEvent( TUid aEvent, TInt aResult )
    {
    RUBY_DEBUG2( "CNssSiUtilityWrapper::MsruoEvent, event[%d], result[%d]", aEvent, aResult );
    iEvent = aEvent;
    iResult = aResult;
    Ready();
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::Ready
// Sets active object ready to be run
// ---------------------------------------------------------
//
void CNssSiUtilityWrapper::Ready()
    {
    if ( !IsActive() )
        {
        SetActive();
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }
    }

// ---------------------------------------------------------
// CNssSiUtilityWrapper::RunL
// From CActive
// ---------------------------------------------------------
//
void CNssSiUtilityWrapper::RunL()
    {
    // Forwards callback to other parts of VAS
    RUBY_DEBUG2( "CNssSiUtilityWrapper::RunL calling callback function, event[%d], result[%d]", iEvent, iResult );
    iObserver.MsruoEvent( iEvent, iResult );
    RUBY_DEBUG0( "CNssSiUtilityWrapper::RunL callback called" );
    }
	    
// ---------------------------------------------------------
// CNssSiUtilityWrapper::DoCancel
// From CActive
// ---------------------------------------------------------
//
void CNssSiUtilityWrapper::DoCancel()
    {
    // Nothing
    }


// End of file
