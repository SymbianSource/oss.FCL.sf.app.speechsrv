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
* Description:  
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "nssttscustomcommandparser.h"
#include "nssttscustomcommandcommon.h"
#include "rubydebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::CTtsCustomCommandParser
// C++ constructor.
// -----------------------------------------------------------------------------
//
CTtsCustomCommandParser::CTtsCustomCommandParser( MTtsCustomCommandImplementor& aImplementor ) :
        CMMFCustomCommandParserBase( KUidInterfaceTts ),
        iImplementor( aImplementor )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CTtsCustomCommandParser* CTtsCustomCommandParser::NewL( 
        MTtsCustomCommandImplementor& aImplementor )
    {
    return new ( ELeave ) CTtsCustomCommandParser(aImplementor);
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::~CTtsCustomCommandParser
// Destructor for CTtsCustomCommandParser class
// -----------------------------------------------------------------------------
//
EXPORT_C CTtsCustomCommandParser::~CTtsCustomCommandParser()
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::HandleRequest
// Handles the client side request.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::HandleRequest( TMMFMessage& aMessage )
    {
    ASSERT( aMessage.Destination().InterfaceId() == KUidInterfaceTts );
    TRAPD( error, DoHandleRequestL( aMessage ) );
    if ( error )
        {
        aMessage.Complete( error );
        }
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoHandleRequestL
// Decodes aMessage and calls the correct function. This function is trapped by
// HandleRequest() and the leave code, if any, sent to the client.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoHandleRequestL( TMMFMessage& aMessage )
    {
    
    // Call required function to do the requested operation
    switch( aMessage.Function() )
        {
        case ETtsAddStyle:
            DoAddStyleL( aMessage );
            break;

        case ETtsDeleteStyle:
            DoDeleteStyleL( aMessage );
            break;

        case ETtsGetPosition:
            DoGetPositionL( aMessage );
            break;

        case ETtsNumberOfStyles:
            DoNumberOfStylesL( aMessage );
            break;

        case ETtsOpenParsedText:
            DoOpenParsedTextL( aMessage );
            break;

        case ETtsSetPosition:
            DoSetPositionL( aMessage );
            break;

        case ETtsStyleID:
            DoStyleIDL( aMessage );
            break;

        case ETtsStyleIndex:
            DoStyleIndexL( aMessage );
            break;

        case ETtsSetDefaultStyle:
            DoSetDefaultStyleL( aMessage );
            break;

        case ETtsGetDefaultStyle:
            DoGetDefaultStyleL( aMessage );
            break;

        case ETtsSetSpeakingRate:
            DoSetSpeakingRateL( aMessage );
            break;

        case ETtsGetSpeakingRate:
            DoGetSpeakingRateL( aMessage );
            break;

        case ETtsSupportedLanguages:
            DoGetSupportedLanguagesL( aMessage );
            break;

        case ETtsSupportedLanguagesCount:
            DoGetSupportedLanguagesCountL( aMessage );
            break;

        case ETtsSupportedVoices:
            DoGetSupportedVoicesL( aMessage );
            break;

        case ETtsSupportedVoicesCount:
            DoGetSupportedVoicesCountL( aMessage );
            break;

        default:
            User::Leave( KErrNotSupported );
            
        } // End of switch
    
    aMessage.Complete( KErrNone );
    
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoAddStyleL
// Calls the controller plugin to add a style.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoAddStyleL( TMMFMessage& aMessage )
    {
    TTtsStylePtrPckg stylePtrPckg;
    TTtsStyleIDPtrPckg styleIDPtrPckg;

	aMessage.ReadData1FromClientL( stylePtrPckg );
	aMessage.ReadData2FromClientL( styleIDPtrPckg );

    TTtsStyle* style = stylePtrPckg();
    TTtsStyleID* ID = styleIDPtrPckg();

    *ID = iImplementor.MttscAddStyleL( *style );

    RUBY_DEBUG2( "CTtsCustomCommandParser::DoAddStyleL: %d, %x", *ID, style );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoDeleteStyleL
// Calls the controller plugin to delete a style.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoDeleteStyleL( TMMFMessage& aMessage )
    {
    TTtsStyleIDPckg styleIDPckg;

	aMessage.ReadData1FromClientL( styleIDPckg );

    TTtsStyleID ID = styleIDPckg();

    User::LeaveIfError( iImplementor.MttscDeleteStyle( ID ) );

    RUBY_DEBUG1( "CTtsCustomCommandParser::DoDeleteStyleL: %d", ID );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoGetPositionL
// Calls the controller plugin to get the synthesis position.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoGetPositionL( TMMFMessage& aMessage )
    {
    TTtsIntPtrPckg intPtrPckg;

    aMessage.ReadData1FromClientL( intPtrPckg );

    TInt* wordIndex = intPtrPckg();

    iImplementor.MttscGetPositionL( *wordIndex );

    RUBY_DEBUG1( "CTtsCustomCommandParser::DoGetPositionL: %d", *wordIndex );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoNumberOfStylesL
// Calls the controller plugin to get the number of styles.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoNumberOfStylesL( TMMFMessage& aMessage )
    {
    TTtsUintPtrPckg uintPtrPckg;

    aMessage.ReadData1FromClientL( uintPtrPckg );

    TUint16* count = uintPtrPckg();

    *count = iImplementor.MttscNumberOfStyles();

    RUBY_DEBUG1( "CTtsCustomCommandParser::DoNumberOfStylesL: %d", *count );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoOpenParsedTextL
// Calls the controller plugin to open a parsed text source.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoOpenParsedTextL( TMMFMessage& aMessage )
    {
    TTtsParsedTextPckg parsedTextPckg;

    aMessage.ReadData1FromClientL( parsedTextPckg );

    CTtsParsedText* parsedText = parsedTextPckg().iParsedText;

    iImplementor.MttscOpenParsedTextL( *parsedText );

    RUBY_DEBUG1( "CTtsCustomCommandParser::DoOpenParsedTextL: %x", parsedText );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoSetPositionL
// Calls the controller plugin to set the synthesis position.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoSetPositionL( TMMFMessage& aMessage )
    {
    TTtsIntPckg intPckg;

    aMessage.ReadData1FromClientL( intPckg );

    TInt index = intPckg();

    iImplementor.MttscSetPositionL( index );

    RUBY_DEBUG1( "CTtsCustomCommandParser::DoSetPositionL: %d", index );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoStyleIDL
// Calls the controller plugin to get the style based on ID.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoStyleIDL( TMMFMessage& aMessage )
    {
    TTtsStyleIDPckg styleIDPckg;
    TTtsStylePtrPckg stylePckg;

	aMessage.ReadData1FromClientL( styleIDPckg );
	aMessage.ReadData2FromClientL( stylePckg );

    TTtsStyleID ID = styleIDPckg();
    TTtsStyle* style = stylePckg();

    style = &( iImplementor.MttscStyleL( ID ) );

    RUBY_DEBUG2( "CTtsCustomCommandParser::DoStyleIDL: %d, %x", ID, style );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoStyleIndexL
// Calls the controller plugin to get the style based on ID.
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoStyleIndexL( TMMFMessage& aMessage )
    {
    TTtsUintPckg uintPckg;
    TTtsStylePtrPckg stylePckg;

	aMessage.ReadData1FromClientL( uintPckg );
	aMessage.ReadData2FromClientL( stylePckg );

    TUint16 index = uintPckg();
    TTtsStyle* style = stylePckg();
    
    style = &( iImplementor.MttscStyleL( index ) );

    RUBY_DEBUG2( "CTtsCustomCommandParser::DoStyleIndexL: %d, %x", index, style );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoSetDefaultStyleL
// Calls the controller plugin to set the default style
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoSetDefaultStyleL( TMMFMessage& aMessage )
    {
    TTtsStylePtrPckg stylePtrPckg;

	aMessage.ReadData1FromClientL( stylePtrPckg );

    TTtsStyle* style = stylePtrPckg();

    iImplementor.MttscSetDefaultStyleL( *style );

    RUBY_DEBUG0( "CTtsCustomCommandParser::DoSetDefaultStyleL" );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoGetDefaultStyleL
// Calls the controller plugin to get the default style
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoGetDefaultStyleL( TMMFMessage& aMessage )
    {
    TTtsStylePtrPckg stylePtrPckg;
    
    aMessage.ReadData1FromClientL( stylePtrPckg );
    
    TTtsStyle* style = stylePtrPckg();
    
    *style = iImplementor.MttscDefaultStyleL();
    
    RUBY_DEBUG1( "CTtsCustomCommandParser::DoGetDefaultStyleL: %x", style );
    }
    
// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoSetSpeakingRateL
// Calls the controller plugin to set the speaking rate
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoSetSpeakingRateL( TMMFMessage& aMessage )
    {
    TTtsIntPckg intPckg;

    aMessage.ReadData1FromClientL( intPckg );

    TInt rate = intPckg();

    iImplementor.MttscSetSpeakingRateL( rate );

    RUBY_DEBUG1( "CTtsCustomCommandParser::DoSetSpeakingRateL: %d", rate );
    }
    
// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoGetSpeakingRateL
// Calls the controller plugin to get the current speaking rate setting
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoGetSpeakingRateL( TMMFMessage& aMessage )
    {
    TTtsIntPtrPckg intPtrPckg;

    aMessage.ReadData1FromClientL( intPtrPckg );

    TInt* rate = intPtrPckg();

    *rate = iImplementor.MttscSpeakingRateL();

    RUBY_DEBUG1( "CTtsCustomCommandParser::DoGetSpeakingRateL: %d", *rate );
    }
    
// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoGetSupportedLanguagesL
// Calls the controller plugin to get the supported languages
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoGetSupportedLanguagesL( TMMFMessage& aMessage )
    {
    TTtsRArrayLanguagePtrPckg rArrayLanguagePtrPckg;
    
    aMessage.ReadData1FromClientL( rArrayLanguagePtrPckg );
    
    RArray<TLanguage> languages = *rArrayLanguagePtrPckg();
    
    RArray<TLanguage> tmp;
    CleanupClosePushL( tmp );
    
    iImplementor.MttscGetSupportedLanguagesL( tmp );
    
    if ( languages.Count() == tmp.Count() )
        {
        for ( TInt i( 0 ); i < languages.Count(); i++ )
            {
            languages[i] = tmp[i];
            }
        }
    CleanupStack::PopAndDestroy( &tmp );
    
    RUBY_DEBUG1( "CTtsCustomCommandParser::DoGetSupportedLanguagesL: Number of languages %d", 
                 languages.Count() );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoGetSupportedLanguagesCountL
// Calls the controller plugin to get the number of supported languages
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoGetSupportedLanguagesCountL( TMMFMessage& aMessage )
    {
    TTtsIntPtrPckg intPtrPckg;

    aMessage.ReadData1FromClientL( intPtrPckg );

    TInt* languageCount = intPtrPckg();
    
    RArray<TLanguage> languages;
    CleanupClosePushL( languages ); 
    iImplementor.MttscGetSupportedLanguagesL( languages );
    *languageCount = languages.Count();

    CleanupStack::PopAndDestroy();
    }
    
// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoGetSupportedVoicesL
// Calls the controller plugin to get the voices of specified language
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoGetSupportedVoicesL( TMMFMessage& aMessage )
    {
    TTtsLanguagePckg languagePckg;
    TTtsRArrayTtsStylePtrPckg rArrayTtsStylePtrPckg;
    
    aMessage.ReadData1FromClientL( languagePckg );
    aMessage.ReadData2FromClientL( rArrayTtsStylePtrPckg );
    
    TLanguage language = languagePckg();
    RArray<TTtsStyle> voices = *rArrayTtsStylePtrPckg();
    
    RArray<TTtsStyle> tmp;
    CleanupClosePushL( tmp );
    
    iImplementor.MttscGetSupportedVoicesL( language, tmp );
    
    if ( voices.Count() == tmp.Count() )
        {
        for ( TInt i( 0 ); i < voices.Count(); i++ )
            {
            voices[i] = tmp[i];
            }
        }
    CleanupStack::PopAndDestroy( &tmp );
    
    RUBY_DEBUG2( "CTtsCustomCommandParser::DoGetSupportedVoicesL: %d voices for language %d", 
                 voices.Count(), (TInt)language );
    }

// -----------------------------------------------------------------------------
// CTtsCustomCommandParser::DoGetSupportedVoicesCountL
// Calls the controller plugin to get the number of voices of specified language
// -----------------------------------------------------------------------------
//
void CTtsCustomCommandParser::DoGetSupportedVoicesCountL( TMMFMessage& aMessage )
    {
    TTtsLanguagePckg languagePckg;
    TTtsIntPtrPckg intPtrPckg;

    aMessage.ReadData1FromClientL( languagePckg );
    aMessage.ReadData2FromClientL( intPtrPckg );

    TLanguage language = languagePckg();
    TInt* voiceCount = intPtrPckg();
    
    RArray<TTtsStyle> voices;
    CleanupClosePushL( voices ); 
    iImplementor.MttscGetSupportedVoicesL( language, voices );
    *voiceCount = voices.Count();

    CleanupStack::PopAndDestroy( );
    }
// End of File
