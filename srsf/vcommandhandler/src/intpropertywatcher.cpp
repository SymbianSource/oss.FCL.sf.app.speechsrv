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
* Description:  Watches for the integer P&S property and notifies observer
* 		 about the changes
*
*/


#include "rubydebug.h"
#include "intpropertywatcher.h"

CIntPropertyWatcher* CIntPropertyWatcher::NewL( const TUid aCategory, TUint aKey, 
                                    MIntChangeNotifiable& aObserver )
    {
    CIntPropertyWatcher* self = new (ELeave) CIntPropertyWatcher( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aCategory, aKey );
    CleanupStack::Pop( self );
    return self;
    }

CIntPropertyWatcher::CIntPropertyWatcher( MIntChangeNotifiable& aObserver )
    :CActive( CActive::EPriorityStandard ), iObserver( aObserver )
    {}

void CIntPropertyWatcher::ConstructL( const TUid aCategory, TUint aKey )
    {
    User::LeaveIfError( iProperty.Attach( aCategory, aKey ) );
    CActiveScheduler::Add(this);
    // initial subscription
    iProperty.Subscribe(iStatus);
    SetActive();
    }

CIntPropertyWatcher::~CIntPropertyWatcher()
    {
    Cancel();
    iProperty.Close();
    }

void CIntPropertyWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

void CIntPropertyWatcher::RunL()
    {
    RUBY_DEBUG_BLOCKL( "" );
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();    
    // property updated, get new value
    TInt newValue;
    if (iProperty.Get( newValue ) == KErrNotFound )
        {
        RUBY_DEBUG0( "Property deleted or was never defined, ignoring" );
        }
    else
        {
        iObserver.IntValueChanged( newValue );
        }
    }
    

    
//End of file
