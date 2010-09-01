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
* Description:  CenRep handling for DevASR
*
*/


#ifndef DEVASRCENREP_H
#define DEVASRCENREP_H

//  INCLUDES
#include <e32base.h>
#include <centralrepository.h>

// CLASS DECLARATION

/**
* Playback volume CenRep handler for DevAsr
*/
NONSHARABLE_CLASS( CDevAsrCenRep ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDevAsrCenRep* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CDevAsrCenRep();
        
    public: // New functions

        /**
        * Returns the current rejection setting from CenRep.
        * If nothing is found (or error occurs), maximum volume will be returned.
        * Volume setting is scaled to 0..aMaxVolume
        *
        * @param "TInt aMaxVolume" Maximum volume
        * @return Volume setting read from CenRep
        */    
        TInt RejectionValue();

    public: // Functions from base classes
       
    private:
        
        /**
        * C++ default constructor.
        */
        CDevAsrCenRep();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // Prohibit copy constructor
        CDevAsrCenRep( const CDevAsrCenRep& );
        // Prohibit assigment operator
        CDevAsrCenRep& operator=( const CDevAsrCenRep& );
        
    private: // Data

        // Central Repository
        CRepository* iRepository;

    };
    
#endif // DEVASRCENREP_H
    
// End of File
