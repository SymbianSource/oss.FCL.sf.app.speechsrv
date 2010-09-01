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
* Description:  Watches for the integer P&S property and notifies the observer
*                about the changes
*
*/



#ifndef INTPROPERTYWATCHER_H
#define INTPROPERTYWATCHER_H

#include <e32property.h>
#include <e32base.h>

/**
 * Interface for those who want to be notified about the property change
 */
class MIntChangeNotifiable
    {
    public:
        virtual void IntValueChanged(TInt aNewValue) = 0;
    };

/**
 * After the creation watches for the given Publish & Subscribe property
 * changes and notifies the observer about the changes
 */
class CIntPropertyWatcher : public CActive
    {
    public:
        static CIntPropertyWatcher* NewL( const TUid aCategory, TUint aKey, 
                                        MIntChangeNotifiable& aObserver );
        
        virtual ~CIntPropertyWatcher();
    
    private:
    
        CIntPropertyWatcher( MIntChangeNotifiable& aObserver );
        
        void ConstructL( const TUid aCategory, TUint aKey );
        
        void RunL();
        
        void DoCancel();
        
    private:
    
        RProperty iProperty;
        
        MIntChangeNotifiable& iObserver;
        
    };

    
#endif // INPROPERTYWATCHER_H