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
* Description:  Contains functions that are exported from PbkInfoView for 
*                PbkInfoView AIW provider.
*
*/


#ifndef PBKINFOVIEWAPI_H
#define PBKINFOVIEWAPI_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class CPbkInfoViewDialog;

/**
*  Public API for executing info view. 
* 
*  @lib pbkinfoview.lib
*  
*/
class CPbkInfoViewApi : public CBase
    {

    public:
        /**
        * Two-phased constructor.
        */
	    IMPORT_C static CPbkInfoViewApi* NewL();

	    /**
        * Destructor.
        */
	    virtual ~CPbkInfoViewApi();
	    
        /**
        * Shows info view displaying voice commands related to the contact.
        * @param aContactId Contact id from contacts db.
	    * @return None.
        */
		IMPORT_C void ShowInfoViewL( TInt aContactId );

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

        /**
        * C++ default constructor.
        */
        CPbkInfoViewApi();
    };


#endif  // PBKINFOVIEWAPI_H

// End of file
