/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A list of CVCommandUiEntry-objects
*
*/


#ifndef CVCOMMANDUIENTRYARRAY_H
#define CVCOMMANDUIENTRYARRAY_H

// INCLUDES
#include <vcommandapi.h>
#include "vcommanduientry.h"

/** Array of pointers to CVCommandUiEntrys */
typedef RPointerArray<CVCommandUiEntry> RVCommandUiEntryArray;

/**
* Non-modifiable list of CVCommandUiEntrys
*/
class CVCommandUiEntryArray : public CBase
	{
	public:
		/** 
		* Constructs the non-modifiable CVCommandUiEntryArray
		* @param aSource Commands to store. CVCommandUiEntryArray makes
		*        copies of them. Ownership is passed to this class.
		*/
		static CVCommandUiEntryArray* NewL( CVCommandArray* aArray );
		
		/**
		* Destructor
		*/
        virtual ~CVCommandUiEntryArray();
		
		/**
		* Returns the reference to the command stored in this
		* CVCommandUiEntryArray
		* @param The position of the object pointer within the array. This
		*        value must not be negative and must not be greater than the
		*        number of objects currently in the array, otherwise the
		*        operator raises a USER-130 panic.
		* @return Unmodifiable reference to CVCommandUiEntry
		*/
		const CVCommandUiEntry& At( TInt aIndex ) const;
		
		/**
		* Equivalent to the operator At
		* @see At()
		*/
		const CVCommandUiEntry& operator[]( TInt aIndex ) const;

		/** 
		* Returns the number of commands
		* @return TInt The number of voice commands
		*/
		TInt Count() const;
		
		/*
		* Creates a copy of this object. Ownership is passed to the caller.
		* @return CVCommandUiEntryArray* Pointer to a CVCommandUiEntryArray
		*         object
		*/
		CVCommandUiEntryArray* CloneL();
		
	private:
	
	    /**
        * Constructor
        */
	    CVCommandUiEntryArray( CVCommandArray* aArray );
	
	    /**
        * Symbian C++ default constructor.
        */ 
		void ConstructL();
		
	private:
	
		CVCommandArray*        iCommands;
        
        RVCommandUiEntryArray  iListBoxCommands;
	};
	
#endif  // CVCOMMANDUIENTRYARRAY_H

// End of File


