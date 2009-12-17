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
* Description:  Abstract base class for phonebook data access
*
*/


#ifndef VASMBASEPBKHANDLER_H
#define VASMBASEPBKHANDLER_H

// INCLUDES
#include <cntdef.h>

// CONSTANTS

// DATA TYPES

// FORWARD DECLARATIONS
class MNssTag;
class MVasContactObserver;

// CLASS DECLARATION

/**
* Abstract base class for phonebook data access
*
* @since 3.2
*/
class MVasBasePbkHandler
    {
    public:  // Constructors and destructor
    
        /**
        * Destructor. Defined here so that client can use M-class pointer to destroy
        * the concrete C-object
        *
        * @since 3.2
        */
        virtual ~MVasBasePbkHandler() {};
   
    public: // New functions

        /**
        * Opens phonebook connection and contact stores
        *
        * @since 3.2
        */
        virtual void InitializeL() = 0;
        
        /**
        * Creates the contact change notifier and sets observer as a listener
        *
        * @since 3.2
        * @param aObserver Contact change listener
        */
        virtual void CreateContactObserverL( MVasContactObserver* aObserver ) = 0;
        
        /**
        * Compresses database
        *
        * @since 3.2
        */
        virtual void CompressL() = 0;
        
        /**
        * Returns all contact ids from database
        *
        * @since 3.2
        * @return Contact id array
        */
        virtual CContactIdArray* ContactIdArrayLC() = 0;
        
        /**
        * Finds a contact from phonebook
        *
        * @since 3.2
        * @param aTag Tag that is used as a search parameter
        */
        virtual void FindContactL( MNssTag* aTag ) = 0;
        
        /**
        * Finds a contact from phonebook
        *
        * @since 3.2
        * @param aContactId Id of the contact that should be found
        * @param aReadOnly ETrue if contact will not be edited else EFalse
        */
        virtual void FindContactL( TInt32 aContactId, TBool aReadOnly ) = 0;
        
        /**
        * Closes contact with or without saving changes
        *
        * @since 3.2
        * @param aSaveChanges ETrue to save changes else EFalse
        */
        virtual void CloseContactL( TBool aSaveChanges ) = 0;
        
        /**
        * Finds a field from contact
        *
        * @since 3.2
        * @param aTag Tag that is used as a search parameter
        */
        virtual void FindFieldL( MNssTag* aTag ) = 0;
        
        /**
        * Finds a field from contact
        *
        * @since 3.2
        * @param aFieldId Field id that is used as a search parameter
        */
        virtual void FindFieldL( TInt aFieldId ) = 0;
        
        /**
        * Finds a field from contact
        *
        * @since 3.2
        * @param aTag Tag that is used as a search parameter
        */
        virtual void FindContactFieldL( MNssTag* aTag ) = 0;
        
        /**
        * Searches for a default field of a given type
        *
        * @param aCommandType Command type that is used as a select parameter
        *                     Must be one of TVasExtensionCommand types
        */
        virtual void FindDefaultContactFieldL( TInt aCommandType ) = 0;
        
        /**
        * Changes current field's voice tag field status
        *
        * @since 3.2
        * @param aSet If ETrue field is set else field is removed
        */   
        virtual void ChangeVoiceTagFieldL( TBool aSet ) = 0;
        
        /**
        * Returns the field label
        *
        * @since 3.2
        * @return Field label
        */
        virtual TPtrC LabelL() = 0;
        
        /**
        * Returns the field text
        *
        * @since 3.2
        * @return Field text
        */
        virtual TPtrC TextL() = 0;
        
        /**
        * Returns the field type
        *
        * @since 3.2
        * @return Field type
        */
        virtual TFieldType FieldTypeL() = 0;
        
        /**
        * Returns the field id
        *
        * @since 3.2
        * @return Field id
        */
        virtual TInt FieldIdL() = 0;
        
        /**
        * Returns information if current field is empty
        *
        * @since 3.2
        * @return ETrue if field is empty else EFalse
        */
        virtual TBool IsFieldEmptyL() = 0;
                
    public: // Functions from base classes
        
    private:  // New functions
               
    private:    // Data
    
        };

#endif      // VASMBASEPBKHANDLER_H
            
// End of File
