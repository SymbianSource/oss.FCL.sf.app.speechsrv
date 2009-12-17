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
* Description:  CenRep handling for DevTts
*
*/


#ifndef DEVTTSCENREP_H
#define DEVTTSCENREP_H

//  INCLUDES
#include <e32base.h>
#include <nssdevtts.h>
#include <centralrepository.h>

// CLASS DECLARATION

/**
* Playback volume CenRep handler for DevTTS
*/
NONSHARABLE_CLASS( CDevTtsCenRep ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDevTtsCenRep* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CDevTtsCenRep();
        
    public: // New functions

        /**
        * Returns the current volume setting from CenRep.
        * If nothing is found (or error occurs), maximum volume will be returned.
        * Volume setting is scaled to 0..aMaxVolume
        *
        * @param "TInt aMaxVolume" Maximum volume
        * @return Volume setting read from CenRep
        */    
        TInt Volume( TInt aMaxVolume );

    public: // Functions from base classes
       
    private:
        
        /**
        * C++ default constructor.
        */
        CDevTtsCenRep();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // Prohibit copy constructor
        CDevTtsCenRep( const CDevTtsCenRep& );
        // Prohibit assigment operator
        CDevTtsCenRep& operator=( const CDevTtsCenRep& );
        
    private: // Data

        // Central Repository
        CRepository* iRepository;

    };
    
#endif // DEVTTSCENREP_H
    
// End of File
