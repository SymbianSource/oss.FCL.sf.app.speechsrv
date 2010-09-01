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
* Description:  
*
*/


#ifndef UIARRAYSGENERATOR_H
#define UIARRAYSGENERATOR_H

#include <e32base.h>

class CAknIconArray;
class CDesC16ArrayFlat;
class CVCModel;
class CGulIcon;

/**
 * Encapsulates the listbox element. Including icon, first and second line of the text
 */
class CUiEntryData : public CBase
	{
	public:
        /**
         * @param aIcon. Ownership is tranfered. I.e. the icon will be destroyed,
         *        when this object is destroyed
         * @param aFirstLine
         * @param aSecondLine
         */
		static CUiEntryData* NewL( CGulIcon* aIcon, const TDesC& aFirstLine, 
						         	const TDesC& aSecondLine = KNullDesC );

        static CUiEntryData* NewLC( CGulIcon* aIcon, const TDesC& aFirstLine, 
                                    const TDesC& aSecondLine = KNullDesC );
        
		virtual ~CUiEntryData();
        
        /**
         *  Compares entries alphabetically on the first line value basis. Comparison
         *  is done with the respect to the locale rules. @see TDesC16::CompareC
         * 
         *  @return 1. zero, if the two objects are equal.
         *          2. a negative value, if the first object is less than the second.
         *          3. a positive value, if the first object is greater than the second.
         */
        TInt CompareByFirstLine( const CUiEntryData& aAnotherEntry ) const;
        
        static TInt CompareByFirstLine( const CUiEntryData& entry1, const CUiEntryData& entry2 );
        
        /**
         * Returns the entry icon and tranfers the responsibility to the caller
         * I.e. after calling this method, this object is not responsible for deleting 
         * the icon anymore
         */
        CGulIcon* ReleaseIcon();
        
        const TDesC& FirstLine() const;
        
        const TDesC& SecondLine() const;
        
        /**
         * Construct a text ready to be used in the listbox
         * i.e. "<icon index>\t\FirstLine\tSecondLine"
         * @param aIconIndex Number to use before the first tab character
         */
        TDesC* ConstructListboxItemStringLC( TInt aIconIndex ) const;

    protected:
        CUiEntryData( CGulIcon* aIcon, const TDesC& aFirstLine, 
                            const TDesC& aSecondLine );

	private:
		CGulIcon* iIcon;
		const TDesC& iFirstLine;
        const TDesC& iSecondLine;
	};

class CUiArraysGenerator : public CBase
	{
	public:
		static CUiArraysGenerator* NewLC();
		
		/**
		 * Parses the model into a set of arrays required by the VCommand app ui
		 * I.e. basing on the "current folder" prepares the arrays for the listbox
		 * If "current" folder is the main one, it means including all the sub-folders and main
		 * folder commands. Otherwise, it means including all the current folder commands only
		 * 
		 * Both folders and commands are alphabetially sorted.
		 * All the target arrays are ResetAndDestroyed before the parsing
		 * 
		 * @param aSource Model to parse
		 * @param aCurrentFolderTitle KNullDesC if "curent" folder is the main one
		 * @param aIconArray icons to display in the listbox. Order is not specified
		 * @param aFolderTitles List of folder titles in the alphabetical order
		 * @param aItemIsFolder List of boolean values for all the listbox elements from top 
		 * 		  to bottom. ETrue for every folder, EFalse for every command
		 * @param aItemArray List of strings in the listbox expected 
		 * 		  format ( @see LB_SINGLE_GRAPHIC_HEADING ). Includes:
		 * 		  * a refence to the icon - zero-based index from the aIconArray
		 *        * written text of the command
		 *        * user-specified alternative text or KNullDesC if
		 * 		
		 */
		virtual void FillArraysL( const CVCModel& aSource, const TDesC& aCurrentFolderTitle,
						  CAknIconArray& aIconArray, CDesC16ArrayFlat& aFolderTitles, 
						  RArray<TBool>& aItemIsFolder, CDesC16ArrayFlat& aItemArray );
		
		/**
         * Updates a set of arrays required by the VCommand app ui to match current folder names
         * I.e. basing on the "current folder" prepares the arrays for the listbox
         * If "current" folder is the main one, it means including all the sub-folders and main
         * folder commands. Otherwise, it means including all the current folder commands only
         * 
         * Both folders and commands are alphabetially sorted.
         * 
         * @param aSource Model to parse
         * @param aCurrentFolderTitle KNullDesC if "curent" folder is the main one
         * @param aIconArray icons to display in the listbox. Order is not specified
         * @param aFolderTitles List of folder titles in the alphabetical order
         * @param aItemIsFolder List of boolean values for all the listbox elements from top 
         * 		  to bottom. ETrue for every folder, EFalse for every command
         * @param aItemArray List of strings in the listbox expected 
         * 		  format ( @see LB_SINGLE_GRAPHIC_HEADING ). Includes:
         * 		  * a refence to the icon - zero-based index from the aIconArray
         *        * written text of the command
         *        * user-specified alternative text or KNullDesC if
         * 		
         */					  
        void UpdateFolderArraysL( const CVCModel& aSource, const TDesC& aCurrentFolderTitle,
				          CAknIconArray& aIconArray, CDesC16ArrayFlat& aFolderTitles, 
				          RArray<TBool>& aItemIsFolder, CDesC16ArrayFlat& aItemArray );
						  
	protected:		
        /**
         * Create entries for all the subfolders in the given folder and sort the entries
         * alphabetically
         * @param aSource Model to parse
         * @param aCurrentFolderTitle KNullDesC for the main folder
         * @return The array of entries pushed to CS two times. First PopAndDestroy
         *         will ResetAndDestroy, the second one will delete the array itself
         */				  
		virtual RPointerArray<CUiEntryData>* ExtractFoldersAndSortLC2( const CVCModel& aSource, 
                    const TDesC& aCurrentFolderTitle = KNullDesC );

        /**
         * Create entries for all the subfolders in the given folder and sort the entries
         * alphabetically
         * @param aSource Model to parse
         * @param aCurrentFolderTitle KNullDesC for the main folder
         * @return The array of entries pushed to CS two times. First PopAndDestroy
         *         will ResetAndDestroy, the second one will delete the array itself
         */               
        virtual RPointerArray<CUiEntryData>* ExtractCommandsAndSortLC2( const CVCModel& aSource, 
                    const TDesC& aCurrentFolderTitle = KNullDesC );
        
        /**
         * Fills the arrays required by the ui side with the information from entries
         * @param aEntries Must contain folder entries only
         *        FirstLine is FolderListedName
         * @leave KErrCorrupt if in the aModel there is no command with the folder listed name
         *          from aEntries
         */                    
        virtual void FillFolderTitlesL( RArray<TBool>& aItemIsFolder, 
                    CDesC16ArrayFlat& aFolderTitles, const CVCModel& aModel,
                    const RPointerArray<CUiEntryData>& aEntries );
        
    };
	
#endif //UIARRAYSGENERATOR_H

// End of File


