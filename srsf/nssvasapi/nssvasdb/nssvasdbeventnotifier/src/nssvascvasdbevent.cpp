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
* Description:  The CNssVASDBEvent encapsulates events occuring on the VAS DB.
*
*/


// INCLUDE FILES
#include  "nssvascvasdbevent.h"
#include  "nssvasctag.h"


// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code that
// might leave.
//
CNssVASDBEvent::CNssVASDBEvent()
{
}

// 2 Phase constructor
//
CNssVASDBEvent* CNssVASDBEvent::NewL(TVASDBEventType& aType, MNssTag* aTag)
{
	CNssVASDBEvent* self = NewLC(aType, aTag);
	CleanupStack::Pop(self);
	return self;
}

// 2 Phase constructor
//
CNssVASDBEvent* CNssVASDBEvent::NewLC(TVASDBEventType& aType, MNssTag* aTag)
{
	CNssVASDBEvent* self = new (ELeave) CNssVASDBEvent;
	CleanupStack::PushL(self);
	self->ConstructL(aType, aTag);
	return self;
}

// EPOC Constructor
//
void CNssVASDBEvent::ConstructL(TVASDBEventType& aType, MNssTag *aTag)
{
	iType = aType;
	if(aTag)
		iTag = ((CNssTag*)aTag)->CopyL();
}


    
// Destructor
//
CNssVASDBEvent::~CNssVASDBEvent()
{
	 if(iTag)
	 {
		 delete iTag;
	 }
}


// returns the type of event
//
EXPORT_C CNssVASDBEvent::TVASDBEventType CNssVASDBEvent:: Type()
{
    return iType;
}


// returns the tag on which the event occured,
//
EXPORT_C MNssTag* CNssVASDBEvent::Tag()
{
	return iTag;
}




//  End of File  
