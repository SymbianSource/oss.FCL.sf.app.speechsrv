/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Declares container control for application.
*
*/


#ifndef TESTAPPCONTAINER_H
#define TESTAPPCONTAINER_H

// INCLUDES
#include <coecntrl.h>
   
// FORWARD DECLARATIONS
class CEikLabel; 

// CLASS DECLARATION

/**
*  CTestAppContainer  container control class.
*  
*/
class CTestAppContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
        
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CTestAppContainer();
        
        void ChangeTextL( TInt aIndex, const TDesC& aText );

    private: // Functions from base classes

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

       /**
        * From CCoeControl,Draw.
        */
        void Draw(const TRect& aRect) const;

       /**
        * 
        */
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
        
    private: //data
        
        CEikLabel* iLabelStatus;
        CEikLabel* iLabelVoice;
        CEikLabel* iLabelVolume;
        CEikLabel* iLabelSpekingRate;
        CEikLabel* iLabelDuration;
        CEikLabel* iLabelGeneral;
        
    };

#endif

// End of File
