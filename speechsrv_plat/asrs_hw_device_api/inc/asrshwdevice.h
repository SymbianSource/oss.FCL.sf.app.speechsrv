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
* Description:  TTP hardware device
*
*/



#ifndef ASRHWDEVICE_H
#define ASRHWDEVICE_H

//  INCLUDES
#include <e32base.h>
 
// CLASS DECLARATION

/**
*  Base class for all hw devices
*   
*
*  @lib asrsadaptationhwdevice.lib
*  @since 2.8
*/
class CASRSHwDevice : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CASRSHwDevice* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CASRSHwDevice();

    public: // New functions
        
        /**
        * Use to initilize HW device
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C virtual void InitializeL();

        /**
        * Use to clear HW device
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C virtual void Clear();

		/**
        * Use to get custom interface
        * @since 2.8
        * @param aInterfaceId Identifier of the intercace implementation
        * @return Pointer to the interface implementation or NULL.
        */
        IMPORT_C virtual TAny* CustomInterface( TUid aInterfaceId );

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();



        /**
        * C++ default constructor.
        */
        CASRSHwDevice();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // ?INCLUDE_H   
            
// End of File
