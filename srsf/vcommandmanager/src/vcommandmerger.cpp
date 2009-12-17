/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  VCommandManager application which is started during boot. 
*
*/


// INCLUDE FILES
#include "rubydebug.h"
#include "vcommandmerger.h"
// For CleanupResetAndDestroyPushL
//#include <mmfcontrollerpluginresolver.h> 

CVCommandMerger* CVCommandMerger::NewLC()
    {
    CVCommandMerger* self = new (ELeave) CVCommandMerger;
    CleanupStack::PushL( self );
    return self;
    }

/** 
 * @todo Move the internal member comparisons to the primitive classes
 */

// Returns the array of commands from aTarget that have counterparts
// in aInjection with the equal Runnable, but with different spoken text or UI
// Difference in user texts only does not count
CVCommandArray* CVCommandMerger::ProduceDeductionByRunnablesLC( const CVCommandArray& aTarget, 
                                                          const RVCommandArray& aInjection ) const
    {
    return aTarget.ProduceUntrainSetByRunnablesLC( aInjection );;
    }                                                                    
                                       
// Returns the array of commands from aInjection that have 
// counterparts in aTarget with the equal Runnable, but with different spoken text or UI
// Difference in user texts only does not count
CVCommandArray* CVCommandMerger::ProduceAdditionByRunnablesLC( const CVCommandArray& aTarget, 
                                                     const RVCommandArray& aInjection ) const
    {
    return aTarget.ProduceTrainSetByRunnablesLC( aInjection );
    }                                                                    

// End of File

