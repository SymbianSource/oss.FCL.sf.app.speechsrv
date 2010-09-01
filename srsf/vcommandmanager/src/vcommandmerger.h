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
* Description:  
*
*/


#ifndef VCOMMANDMERGER_H
#define VCOMMANDMERGER_H

//  INCLUDES
#include <e32base.h>

#include "vcommandapi.h"

class CVCommandMerger : public CBase
	{
	public:
		static CVCommandMerger* NewLC();
		
		/**
         * Compares the runnable component of the aTarget commands with the one of
         * aInjection commands. Returns the array of commands from aTarget that have counterparts
         * in aInjection with the equal Runnable, but with different spoken text or UI.
         * Difference in user texts only does not count
         * 
         * This is to detect the commands that should be removed when the system language 
         * is changed
         * 
         * @param aTarget Set of commands "already existing in the system".
         * @param aInjection Set of new commands to replace the aTarget commands with the equal
         *        runnables
         * 
         * @return Array of commands from aTarget that have counterparts
         *         in aInjection with the equal Runnable, but with different spoken text or UI.
		 */
		CVCommandArray* ProduceDeductionByRunnablesLC( const CVCommandArray& aTarget, 
											   	       const RVCommandArray& aInjection ) const;
											   
        /**
         * Compares the runnable component of the aTarget commands with the one of
         * aInjection commands. Returns the array of commands consisting of everything
         * in aInjection, without the commands fully equal to any command in aTarget
         * Difference in user texts only does not count
         * 
         * This is to detect the commands that should be (re)trained when the system language 
         * is changed
         * 
         * @param aTarget Set of commands "already existing in the system".
         * @param aInjection Set of new commands to replace the aTarget commands with the equal
         *        runnables
         * 
         * @return the array of commands consisting of everything
         * in aInjection, without the commands fully equal to any command in aTarget
         */
         CVCommandArray* ProduceAdditionByRunnablesLC( const CVCommandArray& aTarget, 
											           const RVCommandArray& aInjection ) const;
	};

#endif // VCOMMANDMERGER_H

// End of File
