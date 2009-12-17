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
* Description:  Definition of the recognition dialog for clients
*
*/


#ifndef VUICVOICERECOGDIALOG_H
#define VUICVOICERECOGDIALOG_H

#include <e32base.h>

//  FORWARD DECLARATIONS


// CLASS DECLARATION
/**
* The voice recognition dialog
* @lib VoiceUiRecognition.lib
* @since 3.2
*/
class CVoiceRecognitionDialog : public CBase                           
    {
    
    public:     // Constructors and Destructors

        /**
        * C++ default constructor.
        */
        IMPORT_C CVoiceRecognitionDialog();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CVoiceRecognitionDialog();

    public:     // New functions
    
        /**
        * Executes the dialog
        * @return Exit status
        */
        IMPORT_C TInt ExecuteLD();
        
        /**
        * Stops voice dialling process
        */
        IMPORT_C void Cancel();
        
   private:     // Data

   };

#endif    // VUICVOICERECOGDIALOG_H

// End of File

