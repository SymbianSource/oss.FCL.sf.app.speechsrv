/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MNssContext is a pure interface for context processing. A "context" 
*               refers to a collection of voice  tags used by an application 
*               for a classification of operations (for example a collection of 
*               Name Dial voice tags could be contained in one context and 
*               Command voice tags could be contained in another context). 
*
*/


#ifndef NSSVASMCONTEXT_H
#define NSSVASMCONTEXT_H

//  INCLUDES
#include<e32base.h>

// CLASS DECLARATION

/**
*  MNssContext is a pure interface for context processing. 
*  MNssContext provides methods for setting & getting context name and setting & 
*  getting context global flag. A context can only be created using the context 
*  manager. The context manager returns an empty context, and the client had to 
*  populate its name and global flag using the methods below. A context can be 
*  destroyed using delete on the context pointer.
*  @lib NssVASApi.lib
*  @since 2.0
*/

class MNssContext
    {
public:

    /**
    * Get the name of the context
    * @since 2.0
    * @param 
    * @return reference to name of context
    */  
	virtual TDesC& ContextName() = 0;


	/**
    * Is the context global
    * @since 2.0
    * @param 
    * @return ETrue if context is global, EFalse otherwise
    */  
	virtual TBool IsGlobal() = 0;

	/**
    * Sets the name of the context
    * @since 2.0
    * @param aName name of the context as a descriptor
    * @return 
    */  
	virtual void SetNameL( const TDesC& aName ) = 0;

    /**
    * Sets/Resets the global flag for the context
    * @since 2.0
    * @param aGlobal ETrue to set the context as global, EFalse as not global 
    * @return 
    */  
	virtual void SetGlobal( TBool aGlobal ) = 0;

    /**
    * Sets/Resets the client data. The data must be saved with
    * MNssContextMgr::SaveClientData after this call.
    * @since 2.8
    * @param aData At most 100 bytes of serialized client data.
    */
    virtual void SetClientData( const TDesC8& aData ) = 0;

    /**
    * Gets the client data.
    * @since 2.8
    * @return At most 100 bytes of data.
    */
    virtual const TDesC8& ClientData() = 0;

    /**
    * virutal destructor to ensure proper cleanup 
    * @since 2.0
    * @param 
    * @return 
    */  
	virtual ~MNssContext(){}

    };


#endif // NSSVASMCONTEXT_H  
            
// End of File
