/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of VoiceUIPluginHandler
*
*/


// INCLUDE FILES
#include <ecom.h>
#include <nssvuipf.hrh>
#include "nssvoiceuipluginhandler.h" 
#include "rubydebug.h"
#include <nssvoicestatus.h>


// CONSTANTS
const TUid KEventPluginUid =  { KVuipfEventInterfaceUid };
const TUid KActionPluginUid = { KVuipfExecutionInterfaceUid };



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNssVoiceUIPluginHandler::CNssVoiceUIPluginHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNssVoiceUIPluginHandler::CNssVoiceUIPluginHandler()
    {
    // nothing
    }

// -----------------------------------------------------------------------------
// CNssVoiceUIPluginHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNssVoiceUIPluginHandler::ConstructL()
    {
    LoadPluginsL();
    }

// -----------------------------------------------------------------------------
// CNssVoiceUIPluginHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssVoiceUIPluginHandler* CNssVoiceUIPluginHandler::NewL()
    {
    RUBY_DEBUG_BLOCK( "CNssVoiceUIPluginHandler::NewL()" );
    CNssVoiceUIPluginHandler* self = new( ELeave ) CNssVoiceUIPluginHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// -----------------------------------------------------------------------------
// CNssVoiceUIPluginHandler::~CNssVoiceUIPluginHandler
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNssVoiceUIPluginHandler::~CNssVoiceUIPluginHandler()
    {
    UnloadPlugins();
    RUBY_DEBUG0( "CNssVoiceUIPluginHandler::~CNssVoiceUIPluginHandler() end" );
    }


// -----------------------------------------------------------------------------
// CNssVoiceUIPluginHandler::VoiceEvent
// Handle voice event
// -----------------------------------------------------------------------------
//
TInt CNssVoiceUIPluginHandler::VoiceEvent( TUid aEventUid ) 
    {
    RUBY_DEBUG1( "CNssVoiceUIPluginHandler::VoiceEvent(0x%x)", aEventUid.iUid );
    CVoiceUiPluginBase* plugin = NULL;
    CVoiceUiPluginBase* previousPlugin = NULL;
    TInt highestPriority( KMinTInt );

    RPointerArray<CVoiceUiPluginBase> usedPlugins;
    // Select the suitable plug-in with highest priority
    // In case of multiple plug-ins with same priority the first one is selected. 
    
    TInt status( KErrNoneEventNotConsumed );
    
    // execute plug-ins until the event has been successfully processed
    while ( status != KErrNone )
        {
        
        // Loop all plugins
        for( TInt i( 0 ); i < iActionPlugins.Count(); i++ ) 
            {
            CVoiceUiPluginBase* tmpPlugin = iActionPlugins[i];
     
            // Loop all supported events in the plugin
            for ( TInt j( 0 ); j < tmpPlugin->EventUids().Count(); j++ ) 
                {
                RUBY_DEBUG2( "VoiceEvent Comparing event uid [0x%x] against plugin's [0x%x]", 
                             aEventUid.iUid, tmpPlugin->EventUids()[j].iUid );
                             
                TVuipPriority priority = tmpPlugin->Priority();
                if ( tmpPlugin->EventUids()[j] == aEventUid 
                    && priority > highestPriority )
                    {
                    
                    // find the next plug-in in the order of priority
                    TBool alreadyUsed( EFalse );
                    TInt k( 0 );
                    while( k < usedPlugins.Count() && !alreadyUsed )
                        {
                        if ( usedPlugins[k] == tmpPlugin )
                            {
                            alreadyUsed = ETrue;
                            }
                        k++;
                        }

                    if ( !alreadyUsed )
                        {
                        plugin = tmpPlugin;
                        highestPriority = priority; 
                        }
                    }
                }
            }

        if ( plugin && plugin != previousPlugin )
            {
            // execute a plug-in
            status = plugin->Execute( aEventUid );
            previousPlugin = plugin;
            }
        else
            {
            // all plug-ins checked -> exit loop
            RUBY_DEBUG0( "No plugin for the event");
            break;    
            }
        }
        
    
    if ( status == KErrNoneForwardEvent )
        {
        status = KErrNoneEventNotConsumed;
        }
        
    usedPlugins.Reset();
    usedPlugins.Close();
    return status;
    }
    

// -----------------------------------------------------------------------------
// CNssVoiceUIPluginHandler::LoadPluginsL
// Loads plug-ins
// -----------------------------------------------------------------------------
//
void CNssVoiceUIPluginHandler::LoadPluginsL() 
    {
    RUBY_DEBUG_BLOCK( "CNssVoiceUIPluginHandler::LoadPluginsL()" );
    TInt i( 0 );
    RImplInfoPtrArray eventPluginArray;
    CleanupClosePushL( eventPluginArray );

    REComSession::ListImplementationsL( KEventPluginUid, eventPluginArray );

    RUBY_DEBUG1( "CNssVoiceUIPluginHandler::LoadPluginsL() found %i event plugins", 
                 eventPluginArray.Count() );
    // load event plug-ins
    for ( i = 0; i < eventPluginArray.Count(); i++ )
        {
        // Get Uid of plug-in
        TUid id = eventPluginArray[i]->ImplementationUid();
      
        // create plug-in
        TUid dtorID;
        CVoiceEventObserverPluginBase* tmpPlugin = NULL;
        
        TRAPD( error, 
            tmpPlugin = 
                reinterpret_cast<CVoiceEventObserverPluginBase*> 
                ( REComSession::CreateImplementationL( id, dtorID ) ) );

        if ( error == KErrNone )
            {           
            // initialize plug-in
            TRAP( error, tmpPlugin->InitializeL( *this ) );
            }
            
        if ( error == KErrNone )
            {
            error = iEventPlugins.Append( tmpPlugin );   
            }
            
        if ( error == KErrNone )
            {
            error = iEventPluginDtorIds.Append( dtorID );
            }
           
        if ( error != KErrNone )
            {
            REComSession::DestroyedImplementation( dtorID );
            delete tmpPlugin;
            tmpPlugin = NULL;
            }

        }

    eventPluginArray.ResetAndDestroy();

    CleanupStack::PopAndDestroy( &eventPluginArray );

    RImplInfoPtrArray actionPluginArray;
    CleanupClosePushL( actionPluginArray );    
    REComSession::ListImplementationsL( KActionPluginUid, actionPluginArray );

    RUBY_DEBUG1( "CNssVoiceUIPluginHandler::LoadPluginsL() found %i VUI plugins", 
                 actionPluginArray.Count() );

    // load action plug-ins
    for ( i = 0; i < actionPluginArray.Count(); i++ )
        {
        // Get Uid of plug-in
        TUid id = actionPluginArray[i]->ImplementationUid();
      
        // create plug-in
        TUid dtorID;
        CVoiceUiPluginBase* tmpPlugin = NULL;
        
        TRAPD( error, 
            tmpPlugin = 
                reinterpret_cast<CVoiceUiPluginBase*> 
                ( REComSession::CreateImplementationL( id,dtorID ) ) );

        if ( error == KErrNone )
            {
            // initialize plug-in
            TRAP( error, tmpPlugin->InitializeL() );
            }
            
        if ( error == KErrNone )
            {
            error = iActionPlugins.Append( tmpPlugin );
            }
        
        if ( error == KErrNone )
            {    
            error = iActionPluginDtorIds.Append( dtorID );
            }

        if ( error != KErrNone )
            {
            REComSession::DestroyedImplementation( dtorID );
            delete tmpPlugin;
            tmpPlugin = NULL;
            }
        }

    actionPluginArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &actionPluginArray );

    }

// -----------------------------------------------------------------------------
// CNssVoiceUIPluginHandler::UnloadPlugins
// Unloads plug-ins
// -----------------------------------------------------------------------------
//
void CNssVoiceUIPluginHandler::UnloadPlugins() 
    {
    TInt i( 0 );
    iEventPlugins.ResetAndDestroy();
    iEventPlugins.Close();

    // notify ecom
    for ( i = 0; i < iEventPluginDtorIds.Count(); i++ )
        {
        REComSession::DestroyedImplementation( iEventPluginDtorIds[i] );
        }

    iEventPluginDtorIds.Close();
    iActionPlugins.ResetAndDestroy();
    iActionPlugins.Close();
    
    // notify ecom
    for ( i = 0; i < iActionPluginDtorIds.Count(); i++ )
        {
        REComSession::DestroyedImplementation( iActionPluginDtorIds[i] );
        }
    iActionPluginDtorIds.Close();

    // Release REComSession allocations.
    REComSession::FinalClose(); 
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// For DLL goodness.
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
    {
    return KErrNone;
    }
#endif

// End of File
