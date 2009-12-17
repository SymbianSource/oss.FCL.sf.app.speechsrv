/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CNssRRD class encapsulated a RRD. It implements MNssRRD.
*
*/


// INCLUDE FILES
#include <e32svr.h>

#include "rubydebug.h"
#include "nssvascrrd.h"
#include "nssvascvasdbdataexchanger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssRRD::CNssRRD
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CNssRRD::CNssRRD()
    {
    iTagId = KNssVASDbDefaultValue;
    }

// ---------------------------------------------------------
// CNssRRD::ConstructL
// EPOC constructor can leave.
// ---------------------------------------------------------
//
void CNssRRD::ConstructL()
    {
    iTextArray = new (ELeave) CArrayFixFlat<NssRRDText>(1);
    iIntArray = new (ELeave) CArrayFixFlat<TInt>(1);
    }

// ---------------------------------------------------------
// CNssRRD::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CNssRRD* CNssRRD::NewL()
    {
    CNssRRD* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CNssRRD::NewLC
// Two-phased constructor.
// ---------------------------------------------------------
//
CNssRRD* CNssRRD::NewLC()
    {
    CNssRRD* self = new (ELeave) CNssRRD;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CNssRRD::~CNssRRD
// Destructor
// ---------------------------------------------------------
//
CNssRRD::~CNssRRD()
    {
    if ( iTextArray )
        {
        iTextArray->Reset(); 
        delete iTextArray;
        }
    if ( iIntArray )
        {
        iIntArray->Reset();
        delete iIntArray;
        }
    }

// ---------------------------------------------------------
// CNssRRD::TextArray
// returns the text array with this RRD
// ---------------------------------------------------------
//
CArrayFixFlat<NssRRDText>* CNssRRD::TextArray()
    {
	return iTextArray; 
    }

// ---------------------------------------------------------
// CNssRRD::IntArray
// returns the int array with this rrd
// ---------------------------------------------------------
//
CArrayFixFlat<TInt>* CNssRRD::IntArray()
    {
    return iIntArray;
    }

// ---------------------------------------------------------
// CNssRRD::SetTextArrayL
// sets text array for this RRD
// ---------------------------------------------------------
//
void CNssRRD::SetTextArrayL(CArrayFixFlat<NssRRDText>* aTextArray)
    {
    iTextArray->Reset();
    TInt count = aTextArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( (*aTextArray)[i].MaxLength() > KNssVASMAXRRDTextLength )
            {
            RUBY_DEBUG2("CNssRRD::SetTextArrayL text too long %i, max %i",
                 (*aTextArray)[i].MaxLength(), KNssVASMAXRRDTextLength );
            User::Leave( KErrOverflow );
            }
         
        iTextArray->AppendL( (*aTextArray) [i] );
        }
    }

// ---------------------------------------------------------
// CNssRRD::SetIntArrayL
// sets int array for this rrd
// ---------------------------------------------------------
//
 void CNssRRD::SetIntArrayL(CArrayFixFlat<TInt>* aIntArray)
     {
     iIntArray->Reset();
     TInt count = aIntArray->Count();
     for ( TInt i = 0; i < count; i++ )
         {
         iIntArray->AppendL( (*aIntArray) [i] );
         }
     }

// ---------------------------------------------------------
// CNssRRD::SetTagId
// sets the tag id associated with a RRD
// for use by the VAS Database
// ---------------------------------------------------------
//
void CNssRRD::SetTagId( TInt aTagId )
    {
    iTagId = aTagId;
    }		

// ---------------------------------------------------------
// CNssRRD::TagId
// gets the tag id associated with the RRD
// for use by the VAS Database
// ---------------------------------------------------------
//
TInt CNssRRD::TagId()
    {
    return iTagId;
    }		

// ---------------------------------------------------------
// CNssRRD::CopyL
// Make a deep copy of RRD
// ---------------------------------------------------------
//
CNssRRD* CNssRRD::CopyL()
    {
    RUBY_DEBUG_BLOCK( "CNssRRD::CopyL" );
    
    CNssRRD *copy = CNssRRD::NewL();
    copy->SetTextArrayL( iTextArray );
    copy->SetIntArrayL( iIntArray );
    copy->iTagId = iTagId;
    return copy;
    }
	
//  End of File  
