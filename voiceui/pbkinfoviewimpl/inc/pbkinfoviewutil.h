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
* Description:  Utility functions for PbkInfoView.
*
*/



#ifndef PBKINFOVIEWUTIL_H
#define PBKINFOVIEWUTIL_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* Utility functions for PbkInfoView. 
*/
NONSHARABLE_CLASS (TPbkInfoViewUtil)
	{
    public:
        static void Panic( TInt aReason) ;
        
        /** 
        * String copy with length check.
        * @param TDes8& aTarget Copied text will be put to this variable.
        * @param TDesC& aSource Includes the text to be copied.
        * @return None
        */
		static void StrCopy( TDes8& aTarget, const TDesC& aSource );
		
		/** 
        * String copy with length check.
        * @param TDes& aTarget Copied text will be put to this variable.
        * @param TDesC8& aSource Includes the text to be copied.
        * @return None
        */
		static void StrCopy( TDes& aTarget, const TDesC8& aSource );
		
		/** 
        * String copy with length check.
        * @param TDes& aTarget Copied text will be put to this variable.
        * @param TDesC& aSource Includes the text to be copied.
        * @return None
        */
		static void StrCopy( TDes& aTarget, const TDesC& aSource );
		
		/** 
        * Adds resource file to control environment.
        * @param aFile File location.
        * @return TInt Error code.
        */
		static TInt AddResFileL(const TDesC& aFile);
	};

#endif  // PBKINFOVIEWUTIL_H

// End of file