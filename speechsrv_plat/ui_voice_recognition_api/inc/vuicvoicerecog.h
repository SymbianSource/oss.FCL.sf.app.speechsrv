/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef VUICVOICERECOG_H
#define VUICVOICERECOG_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Loads the resource file for voice recognition
*  This class must be instantiated before performing recognition
* @lib VoiceUiRecognition.lib
* @since 2.8
*/
class CVoiceRecog : public CBase
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CVoiceRecog* NewLC();
        IMPORT_C static CVoiceRecog* NewL(); 

        /**
        * Destructor.
        */
        virtual ~CVoiceRecog();


    private:

        /**
        * By default constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CVoiceRecog();

    private: // Data
    
    };

#endif  // VUICVOICERECOG_H

// End of File


