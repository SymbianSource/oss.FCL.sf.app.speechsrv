/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class which takes care of contacts data after
*               voice tag recognition.
*
*/


#ifndef VCMANAGERCONTACTSELECTHANDLER_H
#define VCMANAGERCONTACTSELECTHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <coemain.h>

#include <cntdef.h> 
#include <PhCltTypes.h>

#include <nssvascvasdbmgr.h>
#include <nssvasapi.h>

#include "srsfbldvariant.hrh"

// FORWARD DECLARATIONS
class CSendUi;
class CEikonEnv;
class MVasBasePbkHandler;
class CVcManagerDialer;

// CONSTANTS
_LIT( KNssGCHNameDialContext, "NAMEDIAL" );

// CLASS DECLARATION

/**
*  Responsible for dialing a selected contact
*
*/
NONSHARABLE_CLASS ( CVcManagerContactSelectHandler ) : public CBase,
                                                       public MNssTagSelectHandler
   {
   public:

      /**
      * Two-phased constructor.
      * 
      * @param aContextManager VAS context manager
      * @param aTagSelectNotification VAS tag select notifier
      */
      static CVcManagerContactSelectHandler* NewL( 
                    MNssContextMgr* aContextManager,
                    MNssTagSelectNotification* aTagSelectNotification);

      /**
      * Destructor.
      */
      ~CVcManagerContactSelectHandler();

      /**
      * From MNssTagSelectHandler Called when a tag is selected following recognition
      */
      void HandleTagSelect( MNssTag* aTag );

   private:

      /**
      * Dials phone number
      * @param aName Descriptor containing the name of the contact
      * @param aPhoneNumber Descriptor containing the phone number
      * @param aNumberFieldType A UID which identifies a contact item field's type.
      * @param aCommandId Extension command id
      * @return void
      */
      void DialL( const TDesC& aName, const TDesC& aPhoneNumber,
                  TFieldType aNumberFieldType, TVasExtensionCommand aCommandId );

      /**
      * Checks if character is acceptable.
      *
      * @param aChar character.
      * @return ETrue iff characters is ok.
      */
      static TBool IsValidChar( TChar aChar );

      /**
      * Removes all illegal characters from descriptor.
      *
      * @param aString string from which those characters are removed.
      */
      static void RemoveInvalidChars( TDes& aString );

      /**
      * By default EPOC constructor is private.
      *
      * @param aContextManager VAS context manager
      * @param aTagSelectNotification VAS tag select notifier
      */
      void ConstructL( MNssContextMgr* aContextManager, 
                       MNssTagSelectNotification* aTagSelectNotification );

      /**
      * C++ default constructor.
      */
      CVcManagerContactSelectHandler();

      /**
      * Performs action after contact recognition
      */
      void DoHandleTagSelectL( MNssTag* aTag );
      
   private://data
       
       // VAS database manager
       CNssVASDBMgr* iVasDbManager;
       
       // Phonebook engine
       MVasBasePbkHandler* iPbkHandler;
       
       // Contact id
       TInt iContactId;

       // Number which will be dialed
       TPhCltTelephoneNumber iTelNumber;

       // VAS context
       MNssContext* iContext;

       // VAS tag select notifier to get information about selected tag
       MNssTagSelectNotification* iTagSelectNotification;
       
       // Video call support on/off
       TBool iVideoTelephonySupported;
       
       // VOIP support on/off
       TBool iVoIPFeatureSupported;
       
       // Send UI to open message editor
       CSendUi* iSendUi;
       
       // Eikon environment
       CEikonEnv* iEikonEnvironment;
       
       // Global dialer
       CVcManagerDialer* iGlobalDialer;
   };

#endif // VCMANAGERCONTACTSELECTHANDLER_H
