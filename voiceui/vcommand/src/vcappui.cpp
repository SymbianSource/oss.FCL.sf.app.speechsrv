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
* Description:  Application UI class for vcommand app
*
*/


// INCLUDE FILES
#include <hlplch.h>
#include <featmgr.h>
#include <akntitle.h>  // CAknTitlePane
#include <barsread.h>  // TResourceReader
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>

#include <e32property.h>        // RProperty
#include <PSVariables.h>        // Property values
#include <ctsydomainpskeys.h>

#include <vcommand.rsg>
#include "vcappui.h"
#include "vcmodelappuiinterface.h"
#include "vcgenericview.h"
#include "vcfolderview.h"
#include "vcsettingsview.h"
#include "vcommandconstants.h"
#include "vcmodel.h"

#include "rubydebug.h"

/**
* This is an application closer class. VCommandhandler needs to finish certain
* operations once they are started. Since the application receives key events
* all the time, including the ones requesting application exit, we ask the
* system to Call() this class a bit later. From its RunL it will gracefully
* close the application
*/
NONSHARABLE_CLASS( CAppCloser ) : public CAsyncOneShot 
	{
	public:
		CAppCloser();
	protected:
	    /** Is called by the system after we asked it to Call() closer
	    */
		virtual void RunL();
	};
	
// -----------------------------------------------------------------------------
//  Constructor
// -----------------------------------------------------------------------------
//
CAppCloser::CAppCloser() :
	CAsyncOneShot( CActive::EPriorityStandard )
	{
	}
	
// -----------------------------------------------------------------------------
//  CAppCloser::RunL
//  Will be called by the system and exits the application
// -----------------------------------------------------------------------------
//
void CAppCloser::RunL() 
	{
	if( iStatus == KErrNone ) 
		{
		static_cast<CVCAppUi*> ( CEikonEnv::Static()->AppUi() )->DoExitProgram();
		}
	else 
		{
		RUBY_ERROR1( "CAppCloser::RunL Unexpected iStatus [%d]", iStatus.Int() );
		}
	}


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------------------------
// CVCAppUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CVCAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    
    iCloser = new (ELeave) CAppCloser;
 
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();
    
    iModel = CVCModel::NewL( this );

    iMainView = new (ELeave) CVCGenericView( *iModel );
    iMainView->ConstructL( R_VCOMMAND_MAIN_VIEW );
    AddViewL( iMainView );      // transfer ownership to CAknViewAppUi
    
    iFolderView = new (ELeave) CVCFolderView( *iModel );

    iFolderView->ConstructL( R_VCOMMAND_FOLDER_VIEW );
    AddViewL( iFolderView );      // transfer ownership to CAknViewAppUi

	CVCSettingsView* settingsView = new (ELeave) CVCSettingsView;

    CleanupStack::PushL( settingsView );
    settingsView->ConstructL();
    AddViewL( settingsView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( settingsView );   

    SetDefaultViewL( *iMainView );
    }

// ----------------------------------------------------------------------------
// CVCAppUi::~CVCAppUi()
// Destructor
// ----------------------------------------------------------------------------
//
CVCAppUi::~CVCAppUi() 
    {    
    delete iCloser;
    delete iModel;

    FeatureManager::UnInitializeLib();
    }


// ----------------------------------------------------------------------------
// CVCAppUi::DynInitMenuPaneL
// This function is called by the EIKON framework just before it displays
// a menu pane.
// ----------------------------------------------------------------------------
//
void CVCAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }


// ----------------------------------------------------------------------------
// CVCAppUi::HandleCommandL
// ----------------------------------------------------------------------------
//
void CVCAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        // Both exit commands should do the same thing
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            ExitProgram();
            break;
            }
            
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 AppHelpContextL() );
            break;
            }
            
        default:
            {
            CAknViewAppUi::HandleCommandL( aCommand );
            break;              
            }
        }
    }


// ----------------------------------------------------------------------------
// CVCAppUi::ExitProgram
// ----------------------------------------------------------------------------
//
void CVCAppUi::ExitProgram() 
    {
    if( iOperationInProgress ) 
        {
        iExitRequested = ETrue;
        }
    else 
        {
        DoExitProgram();
        }
    }

// ----------------------------------------------------------------------------
// CVCAppUi::DoExitProgram
// ----------------------------------------------------------------------------
//    
void CVCAppUi::DoExitProgram() 
    {
    Exit();
    }

// ----------------------------------------------------------------------------
// CVCAppUi::ExitProgramDeferred
// ----------------------------------------------------------------------------
//
void CVCAppUi::ExitProgramDeferred()
    {
    iCloser->Call();
    }
    
// ----------------------------------------------------------------------------
// CVCAppUi::StartAtomicOperationLC
// ----------------------------------------------------------------------------
//  
void CVCAppUi::StartAtomicOperationLC()
    {
    iOperationInProgress = ETrue;
    
    TCleanupItem item( CleanupEndAtomicOperation, this );
    CleanupStack::PushL( item );
    }

// ----------------------------------------------------------------------------
// CVCAppUi::NotifyEndOfAtomicOperationL
// ----------------------------------------------------------------------------
//   
void CVCAppUi::EndAtomicOperation()
    {
    iOperationInProgress = EFalse;
    if( iExitRequested ) 
        {
        RUBY_DEBUG0( "NotifyEndOfAtomicOperationL exit requested -> DoExit" );
        ExitProgramDeferred();
        
        // A function with a NotifyStartOfAtomicOperation -
        // NotifyEndOfAtomicOperation pair might be called from a function
        // with the same pair, so we make sure ExitProgramDeferred is called
        // only once in such a situation.
        iExitRequested = EFalse;
        }
    }
    
// ----------------------------------------------------------------------------
// CVCAppUi::CleanupEndAtomicOperation
// ----------------------------------------------------------------------------
//   
void CVCAppUi::CleanupEndAtomicOperation( TAny* aService )
    {
    CVCAppUi* service = static_cast<CVCAppUi*>(aService);
    service->EndAtomicOperation();
    }


void CVCAppUi::CreateAndPushEndAtomicCleanupItemL()
    {
    TCleanupItem item( CleanupEndAtomicOperation, this );
    CleanupStack::PushL( item );
    }


// ----------------------------------------------------------------------------
// CVCAppUi::SetTitleL
// Set title pane text from a resource.
// ----------------------------------------------------------------------------
//
void CVCAppUi::SetTitleL( TInt aResourceId )
    {
    CAknTitlePane* title = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceId );
    title->SetFromResourceL( reader );
    CleanupStack::PopAndDestroy();  // resource reader
    }
    
// ----------------------------------------------------------------------------
// CVCAppUi::InvalidTextNoteL
// ----------------------------------------------------------------------------
//
void CVCAppUi::ShowErrorNoteL( TInt aResourceId )
    {
    HBufC* text = StringLoader::LoadLC( aResourceId );
    CAknErrorNote* note = new( ELeave ) CAknErrorNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }


// ----------------------------------------------------------------------------
// CVCAppUi::NameAlreadyInUseNoteL
// ----------------------------------------------------------------------------
//
void CVCAppUi::NameAlreadyInUseNoteL( const TDesC& aName )
    {
    if ( aName.Length() > 0 )
        {
        // Not using the version of LoadLC where a parameter is inserted to
        // the returned string because for some reason that causes some weird
        // characters to appear when using localized strings
        TDesC* stringholder = StringLoader::LoadLC( R_QTN_VC_ERROR_IN_USE,
                                                    iEikonEnv );
        TName noteText;
        StringLoader::Format( noteText, *stringholder, -1, aName );
        CleanupStack::PopAndDestroy( stringholder );
        
        CAknNoteWrapper* note = new ( ELeave ) CAknNoteWrapper();
        note->ExecuteLD( R_VC_ALREADY_IN_USE, noteText );
        }
    }
    
    
// ----------------------------------------------------------------------------
// CVCSindAppUi::ShowWaitNoteL
// ----------------------------------------------------------------------------
//
void CVCAppUi::ShowWaitNoteL( const TDesC& aText )
    {
    if ( iWaitNote )
        {
        delete iWaitNote;
        iWaitNote = NULL;
        }
    iWaitNote = new(ELeave) CAknWaitDialog(
        ( reinterpret_cast<CEikDialog**>( &iWaitNote ) ), ETrue );
    iWaitNote->PrepareLC( R_VC_WAIT_NOTE );
    iWaitNote->SetTextL( aText );
    
    iWaitNote->RunLD();
    }
    
    
// ----------------------------------------------------------------------------
// CVCSindAppUi::ShowWaitNoteL
// ----------------------------------------------------------------------------
//
void CVCAppUi::DeleteWaitNoteL()
    {
    if (iWaitNote)
    	{
    	iWaitNote->ProcessFinishedL(); // deletes the dialog    	
    	}
    delete iWaitNote;
    iWaitNote = NULL;
    }


// ----------------------------------------------------------------------------
// CVCAppUi::CheckCallStateL
// ----------------------------------------------------------------------------
//
TInt CVCAppUi::CheckCallStateL()
    {
    TInt callState;
    TInt err = RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallState,
                               callState );
        
    User::LeaveIfError( err );
        
    return callState;
    }


// ----------------------------------------------------------------------------
// CVCAppUi::IsVideoCallL
// ----------------------------------------------------------------------------
//
TBool CVCAppUi::IsVideoCallL() 
    { 
    TInt callType;
    TInt err =  RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType,
                                callType );
                                
    User::LeaveIfError( err );

    return callType == EPSCTsyCallTypeH324Multimedia;
    }


// ----------------------------------------------------------------------------
// CVCAppUi::CallInProgressNoteL
// ----------------------------------------------------------------------------
//
void CVCAppUi::CallInProgressNoteL()
    {
    HBufC* text = StringLoader::LoadLC( R_TEXT_CALL_IN_PROGRESS );
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }


// ----------------------------------------------------------------------------
// CVCAppUi::HandleResourceChangeL
// Handle skin change event.
// ----------------------------------------------------------------------------
//
void CVCAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    
    if ( aType == KEikDynamicLayoutVariantSwitch ) 
        {
        CAknView* view = View( KUidVCMainView );        
        if ( view )
            {
        	static_cast< CVCGenericView* >( view )->HandleSizeChange(); 
            }
        
        view = View( KUidVCFolderView );        
        if ( view )
            {
            static_cast< CVCGenericView* >( view )->HandleSizeChange(); 
            }
            
        view = View( KUidVCSettingsView );        
        if ( view )
            {
            static_cast< CVCSettingsView* >( view )->HandleClientRectChange();
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CVCAppUi::CommandSetChanged
// ----------------------------------------------------------------------------
//
void CVCAppUi::CommandSetChanged()
    {
    TUid focusedView = FocusedView();
    
    TRAPD( err,
        // Shouldn't all views be updated?
/*        if( focusedView == KUidVCMainView )
            {
            iMainView->RefreshViewL();
            }
        else
            {
            iFolderView->RefreshViewL();
            }*/
        iMainView->RefreshViewL();
        iFolderView->RefreshViewL();
        );
    if( err != KErrNone )
        {
        RUBY_ERROR1( "RefreshViewL failed with [%d]", err );
        }
    }
    
// ----------------------------------------------------------------------------
// CVCAppUi::CommandSetChanged
// ----------------------------------------------------------------------------
//
TBool CVCAppUi::IsDisplayingMenuOrDialog()
    {
    return EFalse;
    }
    
// End of File  

