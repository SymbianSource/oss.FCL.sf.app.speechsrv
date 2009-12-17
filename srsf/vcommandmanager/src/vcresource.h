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
* Description:  Class which contains localized voice command strings. Read 
*               from a resource file.
*
*/


#ifndef VCRESOURCE_H
#define VCRESOURCE_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// CLASS DECLARATION

/**
*   Class which contains localized voice command strings. Read 
*   from a resource file.
*
*/
NONSHARABLE_CLASS( CVcResource ) : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor. Leaves if resource file cannot be found.
        * 
        * @param aRFs File system handle
        * @param aFileName Resource file name without any path info
        *        and without any filename extension.
        */
        static CVcResource* NewL( RFs& aRFs, const TDesC& aFileName );
        
        /**
        * Destructor.
        */
        virtual ~CVcResource();
        
    public: // New functions
        
        /**
        * Gets the localized string based on an integer key value.
        * Ownership of the returned string is not transferred
        *
        * @param aKey Key integer
        * @return Reference to localized voice command string
        */
        HBufC& GetCommandL( TInt aKey );
        
        /**
        * Gets the localized string based on a descriptor key value.
        * Descriptor needs to contain numerical information.
        * Ownership of the returned string is not transferred
        *
        * @param aKey Key containing numerical information
        * @return Reference to localized voice command string
        */
        HBufC& GetCommandL( const TDesC& aKey );

        /**
        * Gets integer value based on descriptor key.
        *
        * @param aKey Key containing numerical information
        * @return Value for the key
        */
        TInt GetValueL( const TDesC& aKey );
       
        /**
        * Gets integer value based on integer key.
        *
        * @param aKey Key integer
        * @return Value for the key
        */
        TInt GetValueL( TInt aKey );
       
    private:
        
        /**
        * C++ default constructor.
        *
        * @param aRFs File system handle
        */
        CVcResource( RFs& aRFs );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aFileName Resource file name
        */
        void ConstructL( const TDesC& aFileName );
        
        /**
        * Resolves the correct localized resource file name.
        * 
        * @param aFileName Resource file name
        */
        void ResolveResourceFileL( const TDesC& aFileName );
        
        /**
        * Loads the resource file content
        *
        * @param aRfs File system handle
        * @param aFileName Resource file name
        */
        void ReadResourceFileL( RFs& aRfs, const TDesC& aFileName );
        
    private: // Data

        // List of keys
        RArray<TInt>            iKeys;
        
        // List of localized strings
        RPointerArray<HBufC>    iCommands;
                
        // List of keys to the integer array
        RArray<TInt>            iIntegerKeys;
        
        // List of integer values
        RArray<TInt>            iIntegerValues;
        
        // File system handle
        RFs&                    iRFs;        
    };

#endif // VCRESOURCE_H

// End of File
