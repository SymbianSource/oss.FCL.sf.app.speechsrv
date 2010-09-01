/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the implementation of the client interface for the
*               TTS custom commands.
*
*/


// INCLUDE FILES
#include "nssttscustomcommands.h"
#include "nssttscustomcommandcommon.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RTtsCustomCommands::RTtsCustomCommands
// C++ constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RTtsCustomCommands::RTtsCustomCommands( RMMFController& aController) :
                             RMMFCustomCommandsBase( aController, 
                             KUidInterfaceTts )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::AddStyleL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::AddStyle( const TTtsStyle& aStyle, 
                                            TTtsStyleID& aID )
    {
    TTtsStylePtrPckg stylePtrPckg = ( TTtsStyle* ) &aStyle;
    TTtsStyleIDPtrPckg styleIDPtrPckg = &aID;
    return iController.CustomCommandSync( iDestinationPckg, ETtsAddStyle,
                                          stylePtrPckg, styleIDPtrPckg );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::DeleteStyle
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::DeleteStyle( TTtsStyleID aID )
    {
    TTtsStyleIDPckg styleIDPckg = aID;
    return iController.CustomCommandSync( iDestinationPckg, ETtsDeleteStyle, 
                                          styleIDPckg, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::GetPosition
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::GetPosition( TInt& aWordIndex )
    {
    TTtsIntPtrPckg intPtrPckg = &aWordIndex;
    return iController.CustomCommandSync( iDestinationPckg, ETtsGetPosition,
                                          intPtrPckg, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::NumberOfStyles
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::NumberOfStyles( TUint16& aNumber )
    {
	TTtsUintPtrPckg uintPtrPckg = &aNumber;
	return iController.CustomCommandSync( iDestinationPckg, ETtsNumberOfStyles,
									      uintPtrPckg, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::OpenParsedTextL
// 
// -----------------------------------------------------------------------------
//       
EXPORT_C TInt RTtsCustomCommands::OpenParsedText( CTtsParsedText& aText )
    {
    TTtsParsedTextPckg parsedTextPckg( &aText );
	return iController.CustomCommandSync( iDestinationPckg, ETtsOpenParsedText,
									      parsedTextPckg, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::SetPositionL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::SetPosition( TInt aWordIndex )
    {
    TTtsIntPckg intPckg = aWordIndex;
    return iController.CustomCommandSync( iDestinationPckg, ETtsSetPosition,
                                          intPckg, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::Style
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::Style( TTtsStyleID aStyleID, 
                                         TTtsStyle& aStyle )
    {
    TTtsStyleIDPckg styleIDPckg = aStyleID;
    TTtsStylePtrPckg stylePckg = &aStyle;
    return iController.CustomCommandSync( iDestinationPckg, ETtsStyleID,
                                          styleIDPckg, stylePckg );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::StyleL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::Style( TUint16 aIndex, TTtsStyle& aStyle )
    {
    TTtsUintPckg uintPckg = aIndex;
    TTtsStylePtrPckg stylePckg = &aStyle;
    return iController.CustomCommandSync( iDestinationPckg, ETtsStyleIndex,
                                          uintPckg, stylePckg );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::SetDefaultStyle
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::SetDefaultStyle( const TTtsStyle& aStyle )
    {
    TTtsStylePtrPckg stylePtrPckg = ( TTtsStyle* ) &aStyle;
    return iController.CustomCommandSync( iDestinationPckg, ETtsSetDefaultStyle,
                                          stylePtrPckg, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::GetDefaultStyle
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::GetDefaultStyle( TTtsStyle& aStyle )
    {
    TTtsStylePtrPckg stylePtrPckg = &aStyle;
    return iController.CustomCommandSync( iDestinationPckg, ETtsGetDefaultStyle,
                                          stylePtrPckg, KNullDesC8 );

    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::SetSpeakingRate
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::SetSpeakingRate( TInt aRate )
    {
    TTtsIntPckg intPckg = aRate;
    return iController.CustomCommandSync( iDestinationPckg, ETtsSetSpeakingRate,
                                          intPckg, KNullDesC8 );

    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::GetSpeakingRate
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::GetSpeakingRate( TInt& aRate )
    {
    TTtsIntPtrPckg intPtrPckg = &aRate;
    return iController.CustomCommandSync( iDestinationPckg, ETtsGetSpeakingRate,
                                          intPtrPckg, KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::GetSupportedLanguages
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::GetSupportedLanguages( RArray<TLanguage>& aLanguages )
    {
    TInt numberOfLanguages( -1 );
    
    TTtsIntPtrPckg intPtrPckg = &numberOfLanguages;
    TInt error =
        iController.CustomCommandSync( iDestinationPckg, ETtsSupportedLanguagesCount,
                                       intPtrPckg, KNullDesC8 );
    TInt i( 0 );
        
    while ( !error && i < numberOfLanguages )
        {
        i++;

        error = aLanguages.Append( ELangNone );
        }
        
    if ( error )
        {
        aLanguages.Reset();
        }
    else
        {
        TTtsRArrayLanguagePtrPckg rArrayLanguagePtrPckg = &aLanguages;
    
        error = iController.CustomCommandSync( iDestinationPckg, 
                                               ETtsSupportedLanguages,
                                               rArrayLanguagePtrPckg, 
                                               KNullDesC8 );
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
// RTtsCustomCommands::GetSupportedVoices
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTtsCustomCommands::GetSupportedVoices( TLanguage aLanguage, 
                                                      RArray<TTtsStyle>& aVoices )
    {
    TInt numberOfVoices( -1 );
    
    TTtsLanguagePckg languagePckg = aLanguage;
    TTtsIntPtrPckg intPtrPckg = &numberOfVoices;
    
    TInt error =
        iController.CustomCommandSync( iDestinationPckg, ETtsSupportedVoicesCount,
                                       languagePckg, intPtrPckg );
                                       
    TTtsStyle tmp;
    TInt i( 0 );
        
    while ( !error && i < numberOfVoices )
        {
        i++;

        error = aVoices.Append( tmp );
        }
    
    if ( error )
        {
        aVoices.Reset();
        }
    else
        {
        TTtsRArrayTtsStylePtrPckg rArrayTtsStylePtrPckg = &aVoices;
    
        error = iController.CustomCommandSync( iDestinationPckg, 
                                               ETtsSupportedVoices,
                                               languagePckg, 
                                               rArrayTtsStylePtrPckg );
        }
    
    return error;
    }

// End of file
