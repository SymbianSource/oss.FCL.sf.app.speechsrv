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
* Description:  Listbox for commands
*
*/


// INCLUDE FILES
#include "vcdoublelargestylelistbox.h"

// -----------------------------------------------------------------------------
// CVCDoubleLargeStyleListBox::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVCDoubleLargeStyleListBox* CVCDoubleLargeStyleListBox::NewL()
    {
    CVCDoubleLargeStyleListBox* self = new (ELeave) CVCDoubleLargeStyleListBox();
    return self;
    }   
    
// Destructor       
CVCDoubleLargeStyleListBox::~CVCDoubleLargeStyleListBox()
    {
    }

// ---------------------------------------------------------
// CVCDoubleLargeStyleListBox::AddScrollBarObserver
// ---------------------------------------------------------
//     
void CVCDoubleLargeStyleListBox::AddScrollBarObserver( MEikScrollBarObserver* aObserver )
    {
    iObserver = aObserver;
    }
    
// ---------------------------------------------------------
// CVCDoubleLargeStyleListBox::HandleScrollEventL
// ---------------------------------------------------------
//    
void CVCDoubleLargeStyleListBox::HandleScrollEventL( CEikScrollBar* aScrollBar,
                                                     TEikScrollEvent aEventType )
    {
    if ( iObserver )
        {
        iObserver->HandleScrollEventL( aScrollBar, aEventType );
        }
        
    CAknDoubleLargeStyleListBox::HandleScrollEventL( aScrollBar, aEventType );
    }

// ---------------------------------------------------------
// CVCDoubleLargeStyleListBox::CVCDoubleLargeStyleListBox
// ---------------------------------------------------------
//              
CVCDoubleLargeStyleListBox::CVCDoubleLargeStyleListBox()
 : CAknDoubleLargeStyleListBox()
    {
    }
    
// End of File

