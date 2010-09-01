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
* Description:  Saves training parameters, which include pronunciation language
*
*/


// INCLUDE FILES
#include "nssvasctrainingparameters.h"
#include "nssvasdbkonsts.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNssTrainingParameters::NewL
// Two-phased constructor
// ---------------------------------------------------------
//
EXPORT_C CNssTrainingParameters* CNssTrainingParameters::NewL()
    {
    CNssTrainingParameters *me = CNssTrainingParameters::NewLC();
    CleanupStack::Pop( me );
    return( me );
    }

// ---------------------------------------------------------
// CNssTrainingParameters::NewLC
// Two-phased constructor
// ---------------------------------------------------------
//
EXPORT_C CNssTrainingParameters* CNssTrainingParameters::NewLC()
    {
    CNssTrainingParameters *me = new (ELeave) CNssTrainingParameters();
    CleanupStack::PushL( me );

    me->ConstructL();

    return( me );
    }

// ---------------------------------------------------------
// CNssTrainingParameters::CNssTrainingParameters
// C++ constructor
// ---------------------------------------------------------
//
CNssTrainingParameters::CNssTrainingParameters()
 : iSeparator(' ')
    {
    // Nothing
    }

// ---------------------------------------------------------
// CNssTrainingParameters::ConstructL
// Second-phase constructor
// ---------------------------------------------------------
//
void CNssTrainingParameters::ConstructL()
    {
    iLanguages = new (ELeave) RArray<TLanguage>;
    }

// ---------------------------------------------------------
// CNssTrainingParameters::~CNssTrainingParameters
// Destructor
// ---------------------------------------------------------
//
EXPORT_C CNssTrainingParameters::~CNssTrainingParameters()
    {
    if ( iLanguages )
        {
        iLanguages->Close();
        delete iLanguages;
        }
    ResetSindeLanguageArray();
    
    }

// ---------------------------------------------------------
// CNssTrainingParameters::SetLanguages
// Sets the language list
// ---------------------------------------------------------
//
EXPORT_C void CNssTrainingParameters::SetLanguages( RArray<TLanguage>* aLanguageArray )
    {
    if ( iLanguages )
        {
        iLanguages->Close();
        delete iLanguages;
        }
    iLanguages = aLanguageArray;
    }

// ---------------------------------------------------------
// CNssTrainingParameters::Languages
// Returns the language list
// ---------------------------------------------------------
//
EXPORT_C const RArray<TLanguage>& CNssTrainingParameters::Languages() const
    {
    return( *iLanguages );
    }

// ---------------------------------------------------------
// CNssTrainingParameters::SetSeparator
// Sets the separator char
// ---------------------------------------------------------
//
EXPORT_C void CNssTrainingParameters::SetSeparator( TChar aSeparator )
    {
    iSeparator = aSeparator;
    }

// ---------------------------------------------------------
// CNssTrainingParameters::Separator
// Returns the separator char
// ---------------------------------------------------------
//
EXPORT_C TChar CNssTrainingParameters::Separator() const
    {
    return( iSeparator );
    }

// ---------------------------------------------------------
// CNssTrainingParameters::SetLanguages
// Sets the SINDE language list
// ---------------------------------------------------------
//
EXPORT_C void CNssTrainingParameters::SetSindeLanguages( 
                const RArray<RTrainingLanguageArray>& aLanguageArray )
    {
    ResetSindeLanguageArray();
    
    for ( TInt i = 0; i < aLanguageArray.Count(); i++ )
        {
        const RTrainingLanguageArray& array = aLanguageArray[i];
        RTrainingLanguageArray copiedArray;
        for ( TInt j = 0; j < array.Count(); j++ )
            {
            copiedArray.Append( array[j] );
            }
        iSindeLanguages.Append( copiedArray );
        }
    
    }
    
// ---------------------------------------------------------
// CNssTrainingParameters::Languages
// Returns the SINDE language list
// ---------------------------------------------------------
//
EXPORT_C const RArray<RTrainingLanguageArray>& CNssTrainingParameters::SindeLanguagesL() const
    {
    if ( iSindeLanguages.Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
    
    return iSindeLanguages;
    }
    
// ---------------------------------------------------------
// CNssTrainingParameters::ResetSindeLanguageArray
// Resets SINDE language array
// ---------------------------------------------------------
//    
void CNssTrainingParameters::ResetSindeLanguageArray()
    {
    for ( TInt i = 0; i < iSindeLanguages.Count(); i++ )
        {
        RTrainingLanguageArray& array = iSindeLanguages[i];
        array.Close();
        }
    iSindeLanguages.Reset();
    iSindeLanguages.Close();
    }

// End of File
