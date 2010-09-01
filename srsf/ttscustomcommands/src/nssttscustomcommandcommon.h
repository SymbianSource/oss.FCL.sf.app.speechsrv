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
* Description:  Common header file for TTS custom commands, both client &
*                server side need this
*
*/


#ifndef NSSTTSCUSTOMCOMMANDCOMMON_H
#define NSSTTSCUSTOMCOMMANDCOMMON_H

// INCLUDE FILES
#include <nssttscommon.h>

// FORWARD DECLARATIONS
class TTtsParsedText;

// CONSTANTS

const TUid KUidInterfaceTts = { 0x101FF939 };

// DATA TYPES

// Custom command messages
enum TTtsCustomCommandMessages
    {
    ETtsAddStyle,
    ETtsDeleteStyle,
    ETtsGetPosition,
    ETtsNumberOfStyles,
    ETtsOpenParsedText,
    ETtsSetPosition,
    ETtsStyleID, // Style query based on ID
    ETtsStyleIndex, // Style query based on index
    ETtsSetDefaultStyle,
    ETtsGetDefaultStyle,
    ETtsSetSpeakingRate,
    ETtsGetSpeakingRate,
    ETtsSupportedLanguages,
    ETtsSupportedLanguagesCount,
    ETtsSupportedVoices,
    ETtsSupportedVoicesCount
    };

// Packages
typedef TPckgBuf<TTtsStyleID> TTtsStyleIDPckg;
typedef TPckgBuf<TTtsStyleID*> TTtsStyleIDPtrPckg;
typedef TPckgBuf<TInt> TTtsIntPckg;
typedef TPckgBuf<TInt*> TTtsIntPtrPckg;
typedef TPckgBuf<TUint16> TTtsUintPckg;
typedef TPckgBuf<TUint16*> TTtsUintPtrPckg;
typedef TPckgBuf<TTtsStyle*> TTtsStylePtrPckg;
typedef TPckgBuf<TTtsParsedText> TTtsParsedTextPckg;
typedef TPckgBuf<RArray<TLanguage>*> TTtsRArrayLanguagePtrPckg;
typedef TPckgBuf<TLanguage> TTtsLanguagePckg;
typedef TPckgBuf<RArray<TTtsStyle>*> TTtsRArrayTtsStylePtrPckg;

// CLASS DECLARATION

/**
* Container for CTtsParsedText.
*
* @lib TtsCustomCommands.lib
* @since 2.8
*/
class TTtsParsedText
    {
    public:
        
        /**
        * C++ default constructor.
        */
        TTtsParsedText() {}
        
        /**
        * C++ constructor for this class.
        *
        * @since 2.8
        * @param "CtsParsedText* aParsedText" Pointer to parsed text.
        */
        TTtsParsedText( CTtsParsedText* aParsedText ) : 
                        iParsedText( aParsedText ) {}
         
        // Pointer to parsed text
        CTtsParsedText* iParsedText;
    };

#endif // NSSTTSCUSTOMCOMMANDCOMMON_H

// End of file
