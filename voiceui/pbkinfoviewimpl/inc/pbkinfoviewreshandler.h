/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class is used for reading pbkinfoview resources.
*
*/


#ifndef PBKINFOVIEWRESHANDLER_H
#define PBKINFOVIEWRESHANDLER_H


// INCLUDES
#include <e32base.h>
#include <eikenv.h>
#include <aknenv.h>

#include "pbkinfoviewdefines.h"

// FORWARD DECLARATIONS
class RResourceFile;

// CLASS DECLARATION
/**
* CPbkInfoViewResHandler
*
* This class is used for reading pbkinfoview resources.
*/
NONSHARABLE_CLASS (CPbkInfoViewResHandler) : public CBase
    {
    public:
    
        /**
        * Two-phased constructor.
        */
        static CPbkInfoViewResHandler* NewL();

	    /**
        * Destructor.
        */
        ~CPbkInfoViewResHandler();
        
        /**
        * Finds out phonebook bitmap file name.
        * @param aText File name will be put here.
		* @return None.
        */
        static void GetBitmapFileName( TDes& aText );

    private:
        /**
        * C++ default constructor.
        */
	    CPbkInfoViewResHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();
    
    	/**
        * Finds out the pbkinfoview resource file name.
        * @param aText File name will be put here.
		* @return None.
        */
        static void GetResFileName( TDes& aText );
        
    private:
		//  sync app resource file id
	    TInt iResId;
    };



#endif  // PBKINFOVIEWRESHANDLER_H

// End of File

