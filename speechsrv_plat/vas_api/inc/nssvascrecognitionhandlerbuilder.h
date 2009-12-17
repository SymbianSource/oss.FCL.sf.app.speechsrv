/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The CNssRecognitionHandlerBuilder class performs initialization 
*                for CNssRecognitionHandler the  class. 
*
*/


#ifndef CNSSRECOGNITIONHANDLERBUILDER_H
#define CNSSRECOGNITIONHANDLERBUILDER_H

//  INCLUDES
#include <e32base.h>
#include <e32def.h>
#include "nssvasmrecognitionhandler.h"

// FORWARD DECLARATIONS

class CNssRecognitionHandler;


// CLASS DECLARATION

/**
*
*  The CNssRecognitionHandlerBuilder class performs initialization for the 
*  CNssRecognitionHandler class.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssRecognitionHandlerBuilder :public CBase
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */		
		IMPORT_C static CNssRecognitionHandlerBuilder* NewL();

        /**
        * Two-phased constructor.
        */			
		IMPORT_C static CNssRecognitionHandlerBuilder* NewLC();
        
        /**
        * Destructor.
        */				
		~CNssRecognitionHandlerBuilder();

    public: // New functions
        
         /**
        * Initialize RecognitionHandler object 
        * @since 2.0
        * @return void
        */ 
        IMPORT_C void InitializeL();

        /**
        * Get a Pointer to Recognition Handler object.
        * @since 2.0
        * @return MNssRecognitionHandler* Pointer to the MNssRecognitionHandler
		*                              object.
        */
        IMPORT_C MNssRecognitionHandler* GetRecognitionHandler();

    private:

		/**
        * Constructor.
        */	
		CNssRecognitionHandlerBuilder();

    private:    // Data

		// Pointer to the Recognition Handler object.
		CNssRecognitionHandler  * iRecognitionHandler;

    };

#endif      // CRECOGNITIONHANDLERBUILDER_H   
            
// End of File
