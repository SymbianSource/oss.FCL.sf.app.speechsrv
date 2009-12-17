/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


// INCLUDE FILES
#include "TestAppContainer.h"

#include <eiklabel.h>  // for example label control

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTestAppContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CTestAppContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    iLabelStatus = new (ELeave) CEikLabel;
    iLabelStatus->SetContainerWindowL( *this );
    iLabelStatus->SetTextL( _L("Open menu to connect!") );

    iLabelVoice = new (ELeave) CEikLabel;
    iLabelVoice->SetContainerWindowL( *this );
    iLabelVoice->SetTextL( _L(" ") );
    
    iLabelVolume = new (ELeave) CEikLabel;
    iLabelVolume->SetContainerWindowL( *this );
    iLabelVolume->SetTextL( _L(" ") );
    
    iLabelSpekingRate = new (ELeave) CEikLabel;
    iLabelSpekingRate->SetContainerWindowL( *this );
    iLabelSpekingRate->SetTextL( _L(" ") );
    
    iLabelDuration = new (ELeave) CEikLabel;
    iLabelDuration->SetContainerWindowL( *this );
    iLabelDuration->SetTextL( _L(" ") );

    iLabelGeneral = new (ELeave) CEikLabel;
    iLabelGeneral->SetContainerWindowL( *this );
    iLabelGeneral->SetTextL( _L(" ") );
    
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CTestAppContainer::~CTestAppContainer()
    {
    delete iLabelStatus;
    delete iLabelVoice;
    delete iLabelVolume;
    delete iLabelSpekingRate;
    delete iLabelDuration;
    delete iLabelGeneral;
    }

// ---------------------------------------------------------
// CTestAppContainer::ChangeTextL()
// 
// ---------------------------------------------------------
//
void CTestAppContainer::ChangeTextL( TInt aIndex, const TDesC& aText )
    {
    if ( aIndex == 0 )
        {
        iLabelStatus->SetTextL( aText );
        }
    else if (aIndex == 1 )
        {
        iLabelVoice->SetTextL( aText );
        }
    else if (aIndex == 2 )
        {
        iLabelVolume->SetTextL( aText );
        }
    else if (aIndex == 3 )
        {
        iLabelSpekingRate->SetTextL( aText );
        }
    else if (aIndex == 4 )
        {
        iLabelDuration->SetTextL( aText );
        }        
    else if (aIndex == 5 )
        {
        iLabelGeneral->SetTextL( aText );
        }        
        
    SizeChanged();
    DrawNow();
    }
    
// ---------------------------------------------------------
// CTestAppContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CTestAppContainer::SizeChanged()
    {
    iLabelStatus->SetExtent( TPoint(10,10), iLabelStatus->MinimumSize() );
    iLabelVoice->SetExtent( TPoint(10,30), iLabelVoice->MinimumSize() );
    iLabelVolume->SetExtent( TPoint(10,50), iLabelVolume->MinimumSize() );
    iLabelSpekingRate->SetExtent( TPoint(10,70), iLabelSpekingRate->MinimumSize() );
    iLabelDuration->SetExtent( TPoint(10,90), iLabelDuration->MinimumSize() );
    iLabelGeneral->SetExtent( TPoint(10,130), iLabelGeneral->MinimumSize() );
    }

// ---------------------------------------------------------
// CTestAppContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CTestAppContainer::CountComponentControls() const
    {
    return 6; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CTestAppContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CTestAppContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iLabelStatus;
        case 1:
            return iLabelVoice;
        case 2:
            return iLabelVolume;
        case 3:
            return iLabelSpekingRate;
        case 4:
            return iLabelDuration;
        case 5:
            return iLabelGeneral;           
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CTestAppContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CTestAppContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

// ---------------------------------------------------------
// CTestAppContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CTestAppContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // Nothing
    }

// End of File  
