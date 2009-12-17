/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUICUIMODEL_H
#define VUICUIMODEL_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CGlobalNote;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CUiModel ) : public CBase
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CUiModel* NewL();

        /**
        * Destructor.
        */
        virtual ~CUiModel();
                                
    public:     // New functions
    
        /**
        * Returns note
        * @return Reference to note object
        */
        CGlobalNote& Note() const;
             
    private:    // Functions
        
        /**
        * C++ default constructor.
        */
        CUiModel();
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
                
    private:    // Data
    
        // Note
        CGlobalNote*                iNote;
};

#endif  // VUICUIMODEL_H

// End of File
