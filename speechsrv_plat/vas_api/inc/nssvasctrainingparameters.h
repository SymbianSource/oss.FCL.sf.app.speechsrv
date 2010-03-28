/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CTag implements MTag. In addition, it provides members for use
*               by VAS internal components.
*
*/


#ifndef NSSVASCTRAININGPARAMETERS_H
#define NSSVASCTRAININGPARAMETERS_H

// includes
#include <nssvasmspeechitem.h>
#include "nssvasmtag.h"
#include "nssvasmcontext.h"
#include "nssvasmrrd.h"
#include "nssvascoreconstant.h"

// TYPEDEFS
typedef RArray<TLanguage> RTrainingLanguageArray;

// CLASS DECLARATION

/**
*  CTag is the implementation class for MTag interface. 
*  It encapsulates tag data. A tag cannot be directly created by a client. 
*  A client has to own an instance of Tag Manager which has methods for creating 
*  tag objects. The CTag class also provides additional members for internal 
*  use by VAS.
*  @lib VASApi.lib
*  @since 2.8
*/
class CNssTrainingParameters: public CBase
    {
    public:

        /**
        * Two-phased constructor
        * @since 2.8
        * @return Created instance
        */
        IMPORT_C static CNssTrainingParameters* NewL();

        /**
        * Two-phased constructor
        * @since 2.8
        * @return Created instance
        */
        IMPORT_C static CNssTrainingParameters* NewLC();

        /**
        * Virtuous destructor. Inherited from CBase.
        * @since 2.8
        */
        IMPORT_C virtual ~CNssTrainingParameters();

        /**
        * When text is transformed into pronunciation, the outcome depends on
        * language. List the languages, for which pronunciations should be
        * generated.
        * @since 2.8
        * @param aLanguageArray The list of languages
        */
        IMPORT_C void SetLanguages( RArray<TLanguage>* aLanguageArray );

        /**
        * When text is transformed into pronunciation, the outcome depends on
        * language. List the languages, for which pronunciations should be
        * generated.
        * @since 2.8
        * @return Array of languages.
        */
        IMPORT_C const RArray<TLanguage>& Languages() const;

        /**
        * Set the separator between the first name and the last name.
        * @since 2.8
        * @param aSeparator The separator
        */
        IMPORT_C void SetSeparator( TChar aSeparator );

        /**
        * Returns the separator character.
        * @since 2.8
        * @return The separator between the first name and the last name.
        */
        IMPORT_C TChar Separator() const;
        
        /**
        * Sets languages for SINDE type of training
        * @since 3.1
        * @param aLanguageArray Two dimensional language array
        */
        IMPORT_C void SetSindeLanguages( const RArray<RTrainingLanguageArray>& aLanguageArray );
        
        /**
        * Returns the list of SIND training languages.
        * @since 3.1
        * @return Array of language arrays
        */
        IMPORT_C const RArray<RTrainingLanguageArray>& SindeLanguagesL() const;

    private:
    
        /**
        * C++ constructor.
        */
        CNssTrainingParameters();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Destroys SIND language array.
        */
        void ResetSindeLanguageArray();

    private:

        // Word separator
        TChar iSeparator;

        // Language array, owned
        RArray<TLanguage>* iLanguages;
        
        // Array of language arrays for SINDE training, owned
        RArray<RTrainingLanguageArray> iSindeLanguages;
};

#endif // NSSVASCTRAININGPARAMETERS_H  
            
// End of File
