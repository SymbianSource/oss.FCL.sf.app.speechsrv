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
* Description:  Phonebook handler that uses Virtual phonebook to access phonebook data
*
*/


#ifndef VASCVPBKHANDLER_H
#define VASCVPBKHANDLER_H

// INCLUDES
#include <e32base.h>

#include <cntdef.h> 

#include <VPbkFieldType.hrh>

#include <MVPbkContactStoreListObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactAttributeManager.h>
#include <MVPbkBatchOperationObserver.h> 
#include <MVPbkContactViewObserver.h> 

#include <vasmbasepbkhandler.h>
#include <vasmcontactobserver.h>

// CONSTANTS

// DATA TYPES

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CVPbkContactIdConverter;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MNssTag;
class MVasContactObserver;

// CLASS DECLARATION

/**
* Phonebook handler that uses Virtual phonebook to access phonebook data
*
* @since 3.2
*/
class CVasVPbkHandler : public CBase,
                        public MVasBasePbkHandler,
                        public MVPbkContactStoreListObserver,
                        public MVPbkSingleContactOperationObserver,
                        public MVPbkContactObserver,
                        public MVPbkSetAttributeObserver,
                        public MVPbkBatchOperationObserver,
                        public MVPbkContactViewObserver
    {
    public:  // Constructors and destructor
   
        /**
        * Two-phased constructor.
        *
        * @since 3.2
        */
        IMPORT_C static CVasVPbkHandler* NewL();
        
        /**
        * Destructor.
        *
        * @since 3.2
        */
        IMPORT_C virtual ~CVasVPbkHandler();

    public: // New functions

        /**
        * Opens phonebook connection and contact stores
        *
        * @since 3.2
        */
        IMPORT_C void InitializeL();
        
        /**
        * Creates the contact change notifier and sets observer as a listener
        *
        * @since 3.2
        * @param aObserver Contact change listener
        */
        IMPORT_C void CreateContactObserverL( MVasContactObserver* aObserver );
        
        /**
        * Compresses database
        *
        * @since 3.2
        */
        IMPORT_C void CompressL();
        
        /**
        * Returns all contact ids from database
        *
        * @since 3.2
        * @return Contact id array
        */
        IMPORT_C CContactIdArray* ContactIdArrayLC();
        
        
        /**
        * Finds a contact from phonebook
        *
        * @since 3.2
        * @param aTag Tag that is used as a search parameter
        */
        IMPORT_C void FindContactL( MNssTag* aTag );
        
        /**
        * Finds a contact from phonebook
        *
        * @since 3.2
        * @param aContactId Id of the contact that should be found
        * @param aReadOnly ETrue if contact will not be edited else EFalse
        */
        IMPORT_C void FindContactL( TInt32 aContactId, TBool aReadOnly );
        
        /**
        * Closes contact with or without saving changes
        *
        * @since 3.2
        * @param aSaveChanges ETrue to save changes else EFalse
        */
        IMPORT_C void CloseContactL( TBool aSaveChanges );
        
        /**
        * Finds a field from contact
        *
        * @since 3.2
        * @param aTag Tag that is used as a search parameter
        */
        IMPORT_C void FindFieldL( MNssTag* aTag );
        
        /**
        * Finds a field from contact
        *
        * @since 3.2
        * @param aFieldId Field id that is used as a search parameter
        */
        IMPORT_C void FindFieldL( TInt aFieldId );
        
        /**
        * Finds a field from contact
        *
        * @since 3.2
        * @param aTag Tag that is used as a search parameter
        */
        IMPORT_C void FindContactFieldL( MNssTag* aTag );
        
        /**
        * Searches for a default field of a given type
        *
        * @param aCommandType Command type that is used as a select parameter
        *                     Must be one of TVasExtensionCommand types
        */
        IMPORT_C void FindDefaultContactFieldL( TInt aCommandType );
        
        /**
        * Changes current field's voice tag field status
        *
        * @since 3.2
        * @param aSet If ETrue field is set else field is removed
        */   
        IMPORT_C void ChangeVoiceTagFieldL( TBool aSet );
        
        /**
        * Returns the field label
        *
        * @since 3.2
        * @return Field label
        */
        IMPORT_C TPtrC LabelL();
        
        /**
        * Returns the field text
        *
        * @since 3.2
        * @return Field text
        */
        IMPORT_C TPtrC TextL();
        
        /**
        * Returns the field type
        *
        * @since 3.2
        * @return Field type
        */
        IMPORT_C TFieldType FieldTypeL();
        
        /**
        * Returns the field id
        *
        * @since 3.2
        * @return Field id
        */
        IMPORT_C TInt FieldIdL();
        
        /**
        * Returns information if current field is empty
        *
        * @since 3.2
        * @return ETrue if field is empty else EFalse
        */
        IMPORT_C TBool IsFieldEmptyL();

    public: // Functions from base classes

        /**
        * From MVPbkContactStoreListObserver
        * @see MVPbkContactStoreListObserver
        */
        void StoreReady( MVPbkContactStore& aContactStore );

        /**
        * From MVPbkContactStoreListObserver
        * @see MVPbkContactStoreListObserver
        */
        void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );

        /**
        * From MVPbkContactStoreListObserver
        * @see MVPbkContactStoreListObserver
        */
        void HandleStoreEventL( MVPbkContactStore& aContactStore,
                                TVPbkContactStoreEvent aStoreEvent );
        
        /**
        * From MVPbkContactStoreListObserver
        * @see MVPbkContactStoreListObserver
        */
        void OpenComplete();
        
        /**
        * From MVPbkSingleContactOperationObserver
        * @see MVPbkSingleContactOperationObserver
        */
        void VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                                 MVPbkStoreContact* aContact );
                                                 
        /**
        * From MVPbkSingleContactOperationObserver
        * @see MVPbkSingleContactOperationObserver
        */                                         
        void VPbkSingleContactOperationFailed( MVPbkContactOperationBase& aOperation,
                                               TInt aError );
        
        /**
        * From MVPbkContactObserver
        * @see MVPbkContactObserver
        */                                         
        void ContactOperationCompleted( TContactOpResult aResult );

        /**
        * From MVPbkContactObserver
        * @see MVPbkContactObserver
        */  
        void ContactOperationFailed( TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified );

        /**
        * From MVPbkSetAttributeObserver
        * @see MVPbkSetAttributeObserver
        */                                       
        void AttributeOperationComplete( MVPbkContactOperationBase& aOperation ); 
        
        /**
        * From MVPbkSetAttributeObserver
        * @see MVPbkSetAttributeObserver
        */
        void AttributeOperationFailed( MVPbkContactOperationBase& aOperation, TInt aError );
        
        /**
        * From MVPbkBatchOperationObserver
        * @see MVPbkBatchOperationObserver
        */
        void StepComplete( MVPbkContactOperationBase& aOperation, TInt aStepSize );
        
        /**
        * From MVPbkBatchOperationObserver
        * @see MVPbkBatchOperationObserver
        */
        TBool StepFailed( MVPbkContactOperationBase& aOperation, TInt aStepSize, TInt aError );
        
        /**
        * From MVPbkBatchOperationObserver
        * @see MVPbkBatchOperationObserver
        */
        void OperationComplete( MVPbkContactOperationBase& aOperation );
        
        /**
        * From MVPbkContactViewObserver
        * @see MVPbkContactViewObserver
        */
        void ContactViewReady( MVPbkContactViewBase& aView );
        
        /**
        * From MVPbkContactViewObserver
        * @see MVPbkContactViewObserver
        */
        void ContactViewUnavailable( MVPbkContactViewBase& aView );
        
        /**
        * From MVPbkContactViewObserver
        * @see MVPbkContactViewObserver
        */
        void ContactAddedToView( MVPbkContactViewBase& aView, TInt aIndex, const MVPbkContactLink& aContactLink );
        
        /**
        * From MVPbkContactViewObserver
        * @see MVPbkContactViewObserver
        */
        void ContactRemovedFromView( MVPbkContactViewBase& aView, TInt aIndex, const MVPbkContactLink& aContactLink );
        
        /**
        * From MVPbkContactViewObserver
        * @see MVPbkContactViewObserver
        */
        void ContactViewError( MVPbkContactViewBase& aView, TInt aError, TBool aErrorNotified );
                
    private:  // New functions

        /**
        * C++ default constructor.
        */
        CVasVPbkHandler();

        /**
        * By default constructor is private.
        */
        void ConstructL();
        
        /**
        * Selects default field type for the given tag
        *
        * @param aCommandType Command type that is used as a select parameter
        *                     Must be one of TVasExtensionCommand types
        * @return Default field type
        */
        TVPbkDefaultType SelectDefaultType( TInt aCommandType );
        
        /**
        * Checks if given field should be used
        *
        * @param aField Field that is checked
        * @param aCommandType Command type that is used as a select parameter
        *                     Must be one of TVasExtensionCommand types
        * @return ETrue if field is accepted else EFalse
        */
        TBool AcceptDefaultField( MVPbkStoreContactField& aField, TInt aCommandType );
        
        /**
        * Converts field id to old phonebook id
        *
        * @param aField Field to be converted
        * @return Old phonebook field id
        */
        TInt ConvertFieldId( MVPbkStoreContactField& aField );
        
        /**
        * Converts contact event type
        *
        * @param aEvent Event to be converted
        * @return Corresponding vas contact event
        */
        TVasContactEvent ConvertEvent( const TVPbkContactStoreEvent& aEvent );
        
        /**
        * Gets the priority of the given field
        *
        * @param aField Field which priority is returned
        * @return Field priority
        */
        TInt FieldPriority( MVPbkStoreContactField& aField );

    private:    // Data

        // Contact manager
        CVPbkContactManager*        iContactManager;
        
        // Contact id converter
        CVPbkContactIdConverter*    iConverter;
        
        // Current operation
        MVPbkContactOperationBase*  iOperation;
        
        // Found contact
        MVPbkStoreContact*          iContact;
        
        // Found field
        MVPbkStoreContactField*     iField;
        
        // Contact change observer
        MVasContactObserver*        iObserver;
        
        // For waiting callbacks
        CActiveSchedulerWait        iWait;
        
        // Possible error code
        TInt                        iError;
        
        // Is mms supported or not
        TBool                       iMMSSupported;

        };

#endif      // VASCVPBKHANDLER_H
            
// End of File
