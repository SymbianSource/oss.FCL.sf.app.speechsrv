/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  VCommand control panel plugin implementation.
*
*/


// GS Framework and this plugin.
#include    "vccontrolpanelplugin.h"
#include    <vccontrolpanelplugin.mbg>
#include    <GSParentPlugin.h>
#include    <GSCommon.hrh>
#include    <vccontrolpanelpluginrsc.rsg>
#include    <GSPrivatePluginProviderIds.h>
#include    <GSFWViewUIDs.h>
#include    <AknsUtils.h> 

// General services.
#include    <AknNullService.h>
#include    <bautils.h>
#include    <StringLoader.h>

// This application's resource file.
_LIT( KVcControlPanelPluginResourceFileName, "z:vccontrolpanelpluginrsc.rsc" );
_LIT( KIconFileName, "z:vccontrolpanelplugin.mif");

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcControlPanelPlugin::CVcControlPanelPlugin() : iResources( *iCoeEnv ), iNullService( NULL )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcControlPanelPlugin::~CVcControlPanelPlugin()
    {
    iResources.Close();

    if ( iNullService )
        {
        delete iNullService;
        }
    }

// ---------------------------------------------------------------------------
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
void CVcControlPanelPlugin::ConstructL()
    {
    OpenLocalizedResourceFileL( KVcControlPanelPluginResourceFileName, iResources );    
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CVcControlPanelPlugin* CVcControlPanelPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CVcControlPanelPlugin* self = new( ELeave ) CVcControlPanelPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::Id
// -----------------------------------------------------------------------------
//
TUid CVcControlPanelPlugin::Id() const
    {
    return KDllTUid;
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::DoActivateL
// -----------------------------------------------------------------------------
//
void CVcControlPanelPlugin::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::DoDeactivate
// -----------------------------------------------------------------------------
//
void CVcControlPanelPlugin::DoDeactivate()
    {
    }

// ========================= From CGSPluginInterface ==================
// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::GetCaptionL
// -----------------------------------------------------------------------------
//
void CVcControlPanelPlugin::GetCaptionL( TDes& aCaption ) const
    {
    HBufC* result = StringLoader::LoadL( R_VCOMMAND_CONTROL_PANEL_PLUGIN_CAPTION );
    aCaption.Copy( *result );
    delete result;
    }
    
// ---------------------------------------------------------
// CVcControlPanelPlugin::CreateIconL
// ---------------------------------------------------------
//    
CGulIcon* CVcControlPanelPlugin::CreateIconL( const TUid aIconType )
    {
    CGulIcon* icon;
    
    TParse parse;
    parse.Set( KIconFileName, &KDC_BITMAP_DIR, NULL );
    TFileName fileName( parse.FullName() );

    if ( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(), KAknsIIDQgnPropCpDevVc, fileName,
                                          EMbmVccontrolpanelpluginQgn_prop_cp_dev_vc,
                                          EMbmVccontrolpanelpluginQgn_prop_cp_dev_vc_mask );
        }
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    return icon;
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::PluginProviderCategory
// -----------------------------------------------------------------------------
//
TInt CVcControlPanelPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::ItemType()
// -----------------------------------------------------------------------------
//
TGSListboxItemTypes CVcControlPanelPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::GetValue()
// -----------------------------------------------------------------------------
//
void CVcControlPanelPlugin::GetValue( const TGSPluginValueKeys /*aKey*/,
                                      TDes& /*aValue*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::HandleSelection()
// -----------------------------------------------------------------------------
//
void CVcControlPanelPlugin::HandleSelection(
    const TGSSelectionTypes /*aSelectionType*/ )
    {
    TRAP_IGNORE( LaunchVCommandAppL() );
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::OpenLocalizedResourceFileL()
// -----------------------------------------------------------------------------
//
void CVcControlPanelPlugin::OpenLocalizedResourceFileL(
    const TDesC& aResourceFileName,
    RConeResourceLoader& aResourceLoader )
    {
    // Find the resource file:
    TParse* parse = new (ELeave) TParse;
    CleanupStack::PushL( parse );
    parse->Set( aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName* fileName = new (ELeave) TFileName( parse->FullName() );
    CleanupStack::PushL( fileName );

    // Open resource file:
    aResourceLoader.OpenL( *fileName );
    
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PopAndDestroy( parse );
    }

// -----------------------------------------------------------------------------
// CVcControlPanelPlugin::LaunchVCommandAppL()
// -----------------------------------------------------------------------------
//
void CVcControlPanelPlugin::LaunchVCommandAppL()
    {
    // Get the correct application data
    RWsSession ws;
    User::LeaveIfError( ws.Connect() );
    CleanupClosePushL( ws );

    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KVCommandAppTUid );

    if ( task.Exists() )
        {
        task.BringToForeground();
        }
    // Launch VCommand as embedded.
    else
        {
        if ( iNullService )
            {
            delete iNullService;
            iNullService = NULL;
            }
        // Catch the pointer to prevent a memory leak
        iNullService = CAknNullService::NewL( KVCommandAppTUid, this );
        }
    CleanupStack::PopAndDestroy();  // ws
    }

// End of file
