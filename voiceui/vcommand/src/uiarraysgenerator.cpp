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


// INCLUDE FILES

#include "uiarraysgenerator.h"

#include "vcmodel.h"
#include <badesca.h>
#include <gulicon.h>
#include <AknIconArray.h>
#include <e32debug.h>
#include <mmfcontrollerpluginresolver.h>  // for CleanupResetAndDestroyPushL


/**
 * @param aIcon. Ownership is tranfered. I.e. the icon will be destroyed,
 *        when this object is destroyed, unless icon has been already removed by ReleaseIcon
 * @param aFirstLine
 * @param aSecondLine
 */
CUiEntryData* CUiEntryData::NewL( CGulIcon* aIcon, const TDesC& aFirstLine,
                            const TDesC& aSecondLine )
    {
    CUiEntryData* self = NewLC( aIcon, aFirstLine, aSecondLine );
    CleanupStack::Pop( self );
    return self;
    }

CUiEntryData* CUiEntryData::NewLC( CGulIcon* aIcon, const TDesC& aFirstLine,
                            const TDesC& aSecondLine )
    {
    CUiEntryData* self = new (ELeave) CUiEntryData( aIcon, aFirstLine, aSecondLine );
    CleanupStack::PushL( self );
    return self;
    }

CUiEntryData::CUiEntryData( CGulIcon* aIcon, const TDesC& aFirstLine, 
                            const TDesC& aSecondLine ) :
    iIcon( aIcon), iFirstLine( aFirstLine ), iSecondLine( aSecondLine )
    {
    }
                            
CUiEntryData::~CUiEntryData()
    {
    delete iIcon;
    }
    
CGulIcon* CUiEntryData::ReleaseIcon()
    {
    CGulIcon* result = iIcon;
    iIcon = NULL;
    return result;
    }
        
const TDesC& CUiEntryData::FirstLine() const
    {
    return iFirstLine;
    }

const TDesC& CUiEntryData::SecondLine() const
    {
    return iSecondLine;
    }
    
TInt CUiEntryData::CompareByFirstLine( const CUiEntryData& aAnotherEntry ) const
    {
    return iFirstLine.CompareC( aAnotherEntry.FirstLine() );
    }
    
TInt CUiEntryData::CompareByFirstLine( const CUiEntryData& entry1, const CUiEntryData& entry2 )
    {
    return entry1.CompareByFirstLine( entry2 );
    }
    
TDesC* CUiEntryData::ConstructListboxItemStringLC( TInt aIconIndex ) const
    {
    _LIT( KListboxStringPattern, "%d\t%S\t%S");
    const TInt KPatternOverheadLength = 2; // two characters for tabs
    const TInt KMaxIconIndexLength = 5;  // 5 characters for the decimal number represenation
    HBufC* result = HBufC::NewLC( KPatternOverheadLength + KMaxIconIndexLength +
                    FirstLine().Length() + SecondLine().Length() );
    result->Des().Format( KListboxStringPattern, aIconIndex, &FirstLine(), &SecondLine() );
    return result;                    
    }
    
CUiArraysGenerator* CUiArraysGenerator::NewLC()
	{
	CUiArraysGenerator* self = new (ELeave) CUiArraysGenerator;
	CleanupStack::PushL( self );
	return self;
	}
	
/**
 * Create entries for all the subfolders in the given folder and sort the entries
 * alphabetically
 * @param aSource Model to parse
 * @param aCurrentFolderTitle KNullDesC for the main folder
 * @return The array of entries pushed to CS two times. First PopAndDestroy
 *         will ResetAndDestroy, the second one will delete the array itself
 */               
RPointerArray<CUiEntryData>* CUiArraysGenerator::ExtractFoldersAndSortLC2( const CVCModel& aSource,
            const TDesC& aCurrentFolderTitle )
    {
    RPointerArray<CUiEntryData>* folderEntries = new ( ELeave ) RPointerArray<CUiEntryData>;
    CleanupStack::PushL( folderEntries );
    CleanupResetAndDestroyPushL( *folderEntries );
    if( aCurrentFolderTitle != KNullDesC() )
        {
        return folderEntries; // no multilevel folders
        }
        
    for( TInt i = 0; i < aSource.Count(); i++ )
        {
        if ( aSource.At(i).FolderTitle() != KNullDesC() )
            {
            CUiEntryData* entry = CUiEntryData::NewL( aSource.At(i).FolderIconLC(), 
                                        aSource.At(i).FolderListedName(), KNullDesC() );
            CleanupStack::Pop(); // icon
            CleanupStack::PushL( entry );
            TLinearOrder<CUiEntryData> order( CUiEntryData::CompareByFirstLine );
            // does not allow duplicates
            TInt err = folderEntries->InsertInOrder( entry, order );  
            if( err != KErrNone )
                {
                CleanupStack::PopAndDestroy( entry );
                }
            else
                {
                CleanupStack::Pop( entry );
                }
            }
        }
    return folderEntries;
    }

/**
 * Create entries for all the subfolders in the given folder and sort the entries
 * alphabetically
 * @param aSource Model to parse
 * @param aCurrentFolderTitle KNullDesC for the main folder
 * @return The array of entries pushed to CS two times. First PopAndDestroy
 *         will ResetAndDestroy, the second one will delete the array itself
 */               
RPointerArray<CUiEntryData>* CUiArraysGenerator::ExtractCommandsAndSortLC2( 
                const CVCModel& aSource, const TDesC& aCurrentFolderTitle )
    {
    RPointerArray<CUiEntryData>* commandEntries = new ( ELeave ) RPointerArray<CUiEntryData>;
    CleanupStack::PushL( commandEntries );
    CleanupResetAndDestroyPushL( *commandEntries );
        
    for( TInt i = 0; i < aSource.Count(); i++ )
        {
        if ( aSource.At(i).FolderTitle() == aCurrentFolderTitle )
            {
            CUiEntryData* entry = CUiEntryData::NewL( aSource.At(i).IconLC(), 
                      aSource.At(i).WrittenText(), aSource.At(i).AlternativeSpokenText() );
            CleanupStack::Pop(); // icon
            CleanupStack::PushL( entry );
            TLinearOrder<CUiEntryData> order( CUiEntryData::CompareByFirstLine );
            commandEntries->InsertInOrderAllowRepeatsL( entry, order );  
            CleanupStack::Pop( entry );
            }
        }
    return commandEntries;
    }
            	
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
void CUiArraysGenerator::FillArraysL( const CVCModel& aSource, const TDesC& aCurrentFolderTitle,
				  CAknIconArray& aIconArray, CDesC16ArrayFlat& aFolderTitles, 
				  RArray<TBool>& aItemIsFolder, CDesC16ArrayFlat& aItemArray )
	{
	CleanupClosePushL( aItemIsFolder ); 
	
	aIconArray.ResetAndDestroy();
	aFolderTitles.Reset();
	aItemIsFolder.Reset();
	aItemArray.Reset();
    // Get folder entries

    RPointerArray<CUiEntryData>* entries = 
            ExtractFoldersAndSortLC2( aSource, aCurrentFolderTitle );

    FillFolderTitlesL( aItemIsFolder, aFolderTitles, aSource, *entries );

    // get command entries            
    RPointerArray<CUiEntryData>* ordinaryCommands = 
            ExtractCommandsAndSortLC2( aSource, aCurrentFolderTitle );
    
    // append commands to folder list            
    while( ordinaryCommands->Count() > 0 )
        {
        entries->AppendL( (*ordinaryCommands)[0] );
        aItemIsFolder.AppendL( EFalse );
        ordinaryCommands->Remove( 0 );
        }
            
    CleanupStack::PopAndDestroy( ordinaryCommands );  // ResetAndDestroy
    CleanupStack::PopAndDestroy( ordinaryCommands );  // delete    
    
    for( TInt i = 0; i < entries->Count(); i++ )
        {
        aIconArray.AppendL( (*entries)[i]->ReleaseIcon() );
        TDesC* string = (*entries)[i]->ConstructListboxItemStringLC( i );
        aItemArray.AppendL( *string );
        CleanupStack::PopAndDestroy( string );
        }
    
    CleanupStack::PopAndDestroy( entries );  // ResetAndDestroy
    CleanupStack::PopAndDestroy( entries );  // delete
    
    CleanupStack::Pop(); //aItemIsFolder
	}

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
void CUiArraysGenerator::UpdateFolderArraysL( const CVCModel& aSource, const TDesC& aCurrentFolderTitle,
				  CAknIconArray& aIconArray, CDesC16ArrayFlat& aFolderTitles, 
				  RArray<TBool>& aItemIsFolder, CDesC16ArrayFlat& aItemArray )
	{
    // Get folder entries
    RPointerArray<CUiEntryData>* entries = 
            ExtractFoldersAndSortLC2( aSource, aCurrentFolderTitle );

    for( TInt i = aItemIsFolder.Count() - 1; i >= 0 ; i-- )
        {
        if ( aItemIsFolder[i] )
            {
            aFolderTitles.Delete(i);            
            aItemArray.Delete(i);            
            aItemIsFolder.Remove(i);
            }
        }
        
    for( TInt i = 0; i < entries->Count(); i++ )
        {
        aItemIsFolder.InsertL( ETrue, i );
        
        TBool titleForCurrentListedNameFound = EFalse;
        for( TInt j = 0; j < aSource.Count(); j++ )
            {
            if( aSource.At(j).FolderListedName() == (*entries)[i]->FirstLine() )
                {
                aFolderTitles.InsertL( i, aSource.At(j).FolderTitle() );
                titleForCurrentListedNameFound = ETrue;
                break;
                }
            }
        if (!titleForCurrentListedNameFound)
            {
            User::Leave( KErrCorrupt );
            }
            
        aIconArray.AppendL( (*entries)[i]->ReleaseIcon() );
        
        TDesC* string = (*entries)[i]->ConstructListboxItemStringLC( aIconArray.Count() - 1 );
        aItemArray.InsertL( i, *string );
        CleanupStack::PopAndDestroy( string );
        }
    
    CleanupStack::PopAndDestroy( entries );  // ResetAndDestroy
    CleanupStack::PopAndDestroy( entries );  // delete    
	}
    
/**
 * 		
 */	
void CUiArraysGenerator::FillFolderTitlesL( RArray<TBool>& aItemIsFolder, 
                    CDesC16ArrayFlat& aFolderTitles, const CVCModel& aModel, 
                    const RPointerArray<CUiEntryData>& aEntries )
    {
    // fill folder titles            
    for( TInt i = 0; i < aEntries.Count(); i++ )
        {
        aItemIsFolder.AppendL( ETrue );
        TBool titleForCurrentListedNameFound = EFalse;
        for( TInt j = 0; j < aModel.Count(); j++ )
            {
            if( aModel.At(j).FolderListedName() == aEntries[i]->FirstLine() )
                {
                aFolderTitles.AppendL( aModel.At(j).FolderTitle() );
                titleForCurrentListedNameFound = ETrue;
                break;
                }  // if
            }  // for j
        if (!titleForCurrentListedNameFound)
            {
            User::Leave( KErrCorrupt );
            }
        }  // for i
    }    
    
// End of File
