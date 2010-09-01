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
* Description:  Implementation of the CVCommandRunnable class
*
*/



#include <vcommandapi.h>
#include "rubydebug.h"

/** 
* Constructs the non-modifiable CVCommandArray
* @param aSource Commands to store. CVCommandArray makes
*        copies of them
*/
EXPORT_C CVCommandArray* CVCommandArray::NewL( const RVCommandArray& aSource )
	{
	RUBY_DEBUG_BLOCK( "CVCommandArray::NewL" );
	CVCommandArray* self = new (ELeave) CVCommandArray;
	CleanupStack::PushL( self );
	self->ConstructL( aSource );
	CleanupStack::Pop( self );
	return self;
	}
	
void CVCommandArray::ConstructL( const RVCommandArray& aSource )
	{
	RUBY_DEBUG_BLOCKL( "CVCommandArray::ConstructL" );
	iCommands.ResetAndDestroy();
	for( TInt j = 0; j < aSource.Count(); j++ ) 
		{
		CVCommand* addition = CVCommand::NewL( *aSource[j] );
		CleanupStack::PushL( addition );
		iCommands.AppendL( addition );
		CleanupStack::Pop( addition );
		}
	}	

/**
* Destructor
*/
EXPORT_C CVCommandArray::~CVCommandArray()
	{
	RUBY_DEBUG0( "CVCommandArray::~CVCommandArray start" );
	iCommands.ResetAndDestroy();
	RUBY_DEBUG0( "CVCommandArray::~CVCommandArray end" );
	}

/**
* Returns the reference to the command stored in this
* CVCommandArray
* @param aIndex Zero-based index of the command. If aIndex is out of
*               bounds, this method will panic with USER 130
* @return Unmodifiable reference to CVCommand
*/
EXPORT_C const CVCommand& CVCommandArray::At( TInt aIndex ) const
	{
	return *iCommands[ aIndex ];
	}

/**
* Equivalent to the operator At
* @see At()
*/
EXPORT_C const CVCommand& CVCommandArray::operator[]( TInt aIndex ) const
	{
	return At( aIndex );
	}

/** 
* Returns the number of commands
*/
EXPORT_C TInt CVCommandArray::Count() const
	{
	return iCommands.Count();
	}

/**
* For the compatibility with routines, requiring RVCommandArray
* @return Unmodifiable array of pointers to VCommands
*/
EXPORT_C const RVCommandArray& CVCommandArray::PointerArray() const
    {
    return iCommands;
    }

/**
 * Figures out which commands have to untrained (removed from VCommandHandler)
 * if this command set is merged with aUpdates.
 * Merging is performed on the basis of comapring the commands' Runnable component
 * If runnables are equal, the commands are considered being equal.
 * It is useful e.g. when figuring out what to (un)train after the language change
 * 
 * @param aUpdates Set of commands to merge in. 
 * @return Set of commands to be removed from the system if the merging takes place
 */
EXPORT_C CVCommandArray* CVCommandArray::ProduceUntrainSetByRunnablesLC( 
                            const RVCommandArray& aUpdates ) const
    {
    RVCommandArray deduction;
    CleanupClosePushL( deduction );
    for( TInt i = 0; i < Count(); i++ )
        {
        for( TInt j = 0; j < aUpdates.Count(); j++ )
            {
            if( ( At(i).Runnable() == aUpdates[j]->Runnable() ) &&
                !( At(i).EqualNonUserChangeableData( *aUpdates[j] ) ) 
               )
                { 
                deduction.AppendL( &At(i) );
                break;
                }
            }
        }
    CVCommandArray* result = CVCommandArray::NewL( deduction );
    CleanupStack::PopAndDestroy( &deduction );
    CleanupStack::PushL( result );
    return result;
    }
    
/**
 * Figures out which commands have to trained (removed added to the system)
 * if this command set is merged with aUpdates.
 * Merging is performed on the basis of comapring the commands' Runnable component
 * If runnables are equal, the commands are considered being equal.
 * It is useful e.g. when figuring out what to (un)train after the language change
 * 
 * @param aUpdates Set of commands to merge in. 
 * @return Set of commands to be added to the system if the merging takes place
 * @see ProduceUntrainSetByRunnables
 */
EXPORT_C CVCommandArray* CVCommandArray::ProduceTrainSetByRunnablesLC( 
                            const RVCommandArray& aUpdates ) const
    {
    RVCommandArray addition;
    CleanupClosePushL( addition );
    for( TInt j = 0; j < aUpdates.Count(); j++ )
        {
        TBool injectedMatchesSomeTarget = EFalse;
        for( TInt i = 0; i < Count(); i++ )
            {
            if( At(i).EqualNonUserChangeableData( *aUpdates[j] ) )
                { 
                injectedMatchesSomeTarget = ETrue;
                break;
                }
            else if( At(i).Runnable() == aUpdates[j]->Runnable() &&
                     At(i).CommandUi().UserText() != KNullDesC )
                {
                CVCCommandUi* updatedCommandUi( NULL );
                updatedCommandUi = CVCCommandUi::NewL( aUpdates[j]->CommandUi().WrittenText(), 
                                                       aUpdates[j]->CommandUi().FolderInfo(),
                                                       aUpdates[j]->CommandUi().Modifiable(),
                                                       aUpdates[j]->CommandUi().Tooltip(), 
                                                       aUpdates[j]->CommandUi().IconUid(),
                                                       At(i).CommandUi().UserText(),
                                                       aUpdates[j]->CommandUi().ConfirmationNeeded() );
                CleanupStack::PushL( updatedCommandUi );

                CVCommand* updatedCommand( NULL );
                updatedCommand = CVCommand::NewL( aUpdates[j]->SpokenText(),
                                                  aUpdates[j]->Runnable(),
                                                  *updatedCommandUi );
                CleanupStack::PushL( updatedCommand );

                addition.AppendL( updatedCommand );

                CleanupStack::Pop( updatedCommand );
                CleanupStack::PopAndDestroy( updatedCommandUi );
                
                injectedMatchesSomeTarget = ETrue;
                break;
                }
            }  // for i
        if( !injectedMatchesSomeTarget )
            {
            addition.AppendL( aUpdates[j] );
            }  // if
        }  // for j
    CVCommandArray* result = CVCommandArray::NewL( addition );
    CleanupStack::PopAndDestroy( &addition );
    CleanupStack::PushL( result );
    return result;
    }
    
//End of file
