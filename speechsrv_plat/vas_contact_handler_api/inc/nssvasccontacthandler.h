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
* Description:  VAS contact handler interface
*
*/


#ifndef NSSCONTACTHANDLER_H
#define NSSCONTACTHANDLER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CNssContactHandlerImplementation;
class CContactHandlerStopper;

/**
*  Contact handler creates voice tags out of phonebook contacts and keeps
*  SIND data in sync with the phonebook
*
*  @lib NssVASContactHdlr.dll
*  @since 2.8
*/
class CNssContactHandler :  public CBase
    {
   public:  // Constructors and destructor

      /**
      * Two-phased constructor.
      */
      IMPORT_C static CNssContactHandler* NewL();
        
      /**
      * Destructor.
      */
      IMPORT_C virtual ~CNssContactHandler();

   public: // New functions
      
      /**
      * Disables the contact handler until EnableEventHandling is called
      * Typically this means 'until reboot'
      * Is used in case of unrecoverable error.
      * 
      * @todo Move to private?
      * @return void
      */
      IMPORT_C void DisableEventHandling();

      /**
      * Enables the contact handler. 
      * 
      * @deprecated Move to private?
      * @return void
      */
      IMPORT_C void EnableEventHandling();

      /**
      * Sets pointer to object which handles ContactHandler thread stopping.
      * 
      * @param "CContactHandlerStopper* aStopper" Pointer to stopper object
      * @return void
      */
      void SetStopperPointer( CContactHandlerStopper* aStopper );

   private:
       
      /**
      * C++ default constructor.
      */
      CNssContactHandler();
        
      /**
      * Second-phase constructor
      */
      void ConstructL();
        
      // Prohibit copy constructor if not deriving from CBase.
      /** @todo why? */
      CNssContactHandler( const CNssContactHandler& );
        
      // Prohibit assigment operator if not deriving from CBase.
      CNssContactHandler& operator= ( const CNssContactHandler& );
        
      /**
      * Thread function.
      *
      * @param "TAny* aParams" Void pointer given in the thread startup phase.
      */
      static TInt CHThreadFunction( TAny* aParams );
        
   private:

       // ContactHandler thread name
       TBuf<125> iThreadNameBuf;

       // Pointer to ContactHandler thread stopper
       CContactHandlerStopper* iStopper;
    };

#endif // CONTACTHANDLER_H
            
// End of File
