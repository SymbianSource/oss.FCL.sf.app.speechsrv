/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssVASDBEventNotifier is the implementation class for interface 
*				 MNssVASDBEventNotifier.
*
*/


// INCLUDE FILES
#include    "nssvascvasdbeventnotifier.h"


// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code that
// might leave.
//
CNssVASDBEventNotifier::CNssVASDBEventNotifier(CNssVASDatabase* aDatabase): iDatabase(aDatabase)
    {
    }

// Destructor
CNssVASDBEventNotifier::~CNssVASDBEventNotifier()
    {
    
    }


// ---------------------------------------------------------
// CNssVASDBEventNotifier::AddObserverL
// adds an observer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventNotifier::AddObserverL(MNssVASDatabaseObserver* aObserver)
   {
   iDatabase->AddObserverL(aObserver);
   }


// ---------------------------------------------------------
// CNssVASDBEventNotifier::RemoveObserverL
// removes an observer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CNssVASDBEventNotifier::RemoveObserverL(MNssVASDatabaseObserver* aObserver)
   {
   User::LeaveIfError( iDatabase->RemoveObserver(aObserver) );
   }




//  End of File  
