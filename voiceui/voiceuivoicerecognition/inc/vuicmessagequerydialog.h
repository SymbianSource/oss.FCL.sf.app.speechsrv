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


#ifndef VUICMESSAGEQUERYDIALOG_H
#define VUICMESSAGEQUERYDIALOG_H

// INCLUDES
#include <aknmessagequerydialog.h>

// CLASS DECLARATION
/**
* 
*
*/
NONSHARABLE_CLASS( CMessageQueryDialog ) : public CAknMessageQueryDialog
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CMessageQueryDialog* NewL( CMessageQueryDialog** aSelf );

        /**
        * Destructor.
        */
        virtual ~CMessageQueryDialog();
                                
    public:  // New functions
        
    protected:  // Functions from base classes
                             
    private:  // New functions
        
        /**
        * C++ default constructor.
        */
        CMessageQueryDialog( CMessageQueryDialog** aSelf );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
    private:    // Data
        
        // Address of self pointer
        CMessageQueryDialog**      iSelf;
};

#endif  // VUICMESSAGEQUERYDIALOG_H

// End of File
