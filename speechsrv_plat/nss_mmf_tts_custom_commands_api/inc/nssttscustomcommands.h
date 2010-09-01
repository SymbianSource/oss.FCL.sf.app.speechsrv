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
* Description:  Client side interface to TTS custom commands.
*
*/


#ifndef NSSTTSCUSTOMCOMMANDS_H
#define NSSTTSCUSTOMCOMMANDS_H

// INCLUDE FILES
#include <mmfcontroller.h>
#include <nssttscommon.h>

// CLASS DECLARATION

/**
* TTS custom command client side interface.
*
* @lib nssttscustomcommands.lib.lib
* @since 2.8
*/
// CLASS DECLARATION
class RTtsCustomCommands : public RMMFCustomCommandsBase
	{
	public: // Constructors and destructor

		/**
		* Public C++ constructor for RSpeechRecognitionCustomCommands class.
        *
        * @since 2.8
        * @param "RMMFController& aController" Reference to RMMFController object.
		*/
		IMPORT_C RTtsCustomCommands( RMMFController& aController );

	public: // New functions

        /**
        * Adds a style to the style collection.
        *
        * @since 2.8
        * @param "TTtsStyle& aStyle" Style to be added.
        * @retval "TTtsStyleID& aID" Returns: style ID.
        * @return Error code.
        */
        IMPORT_C TInt AddStyle( const TTtsStyle& aStyle, TTtsStyleID& aID );

        /**
        * Deletes a style based on identifier.
        *
        * @since 2.8
        * @param "TTtsStyleID aID" Style ID which should be deleted.
        * @return Error code.
        */
        IMPORT_C TInt DeleteStyle( TTtsStyleID aID );

        /**
        * Gets the current synthesis position in words.
        *
        * @since 2.8
        * @retval "TInt& aWordIndex" Return; position in words.
        * @return Error code.
        */
        IMPORT_C TInt GetPosition( TInt& aWordIndex );

        /**
        * Gets the overall number of registered styles.
        *
        * @since 2.8
        * @retval "TUint16& aNumber" Return; number of styles.
        * @return Error code.
        */
        IMPORT_C TInt NumberOfStyles( TUint16& aNumber );

        /**
        * Opens parsed text object as synthesis source.
        *
        * @since 2.8
        * @param "CTtsParsedText& aText" Parsed text to open.
        * @return Error code.
        */
        IMPORT_C TInt OpenParsedText( CTtsParsedText& aText );

        /**
        * Sets synthesis position in words.
        *
        * @since 2.8
        * @param "TInt aWordIndex" Position to set.
        * @return Error code.
        */
        IMPORT_C TInt SetPosition( TInt aWordIndex );

        /**
        * Gets a registered style based on style identifier.
        *
        * @since 2.8
        * @param "TTtsStyleID aStyleID" Style ID.
        * @retval "TTtsStyle& aStyle" Returns: reference to style object.
        * @return Error code.
        */
        IMPORT_C TInt Style( TTtsStyleID aStyleID, TTtsStyle& aStyle );

        /**
        * Gets a registered style based on index.
        *
        * @since 2.8
        * @param "TUint16 aIndex" Index.
        * @retval "TTtsStyle& aStyle" Returns: reference to style object.
        * @return Error code.
        */
        IMPORT_C TInt Style( TUint16 aIndex, TTtsStyle& aStyle );

        /**
        * Sets the default style parameters for synthesis.
        *
        * @since 3.2 
        * @param "const TTtsStyle& aStyle" Style created by the client
        * @return Error code.
        */
        IMPORT_C TInt SetDefaultStyle( const TTtsStyle& aStyle );
        
        /**
        * Returns the registered default style
        *
        * @since 3.2 
        * @retval "TTtsStyle& aStyle" Returns: reference to style object.
        * @return Error code.
        */
        IMPORT_C TInt GetDefaultStyle( TTtsStyle& aStyle );
        
        /**
        * Sets the speaking rate of synthesizer.
        *
        * @since 3.2 
        * @param TInt aRate Speaking rate value
        * @return Error code.
        */        
        IMPORT_C TInt SetSpeakingRate( TInt aRate );
        
        /**
        * Returns the current speaking rate value.
        *
        * @since 3.2 
        * @return Speaking rate value
        * @retval "TInt aRate" Return; Speaking rate value.
        * @return Error code.
        */
        IMPORT_C TInt GetSpeakingRate( TInt& aRate );
        
        /**
        * Gets the list of supported languages.
        * 
        * @since 3.2
        * @param "RArray<TLanguage>& aLanguages" Output parameter which 
        *         contains the languages
        * @return Error code.
        */
        IMPORT_C TInt GetSupportedLanguages( RArray<TLanguage>& aLanguages );
        
        /**
        * Gets the list of supported voices for a certain language.
        * 
        * @since 3.2
        * @param "TLanguage aLanguage" Language
        * @param "RArray<TTtsStyle>& aVoices" Output parameter which contains the voices
        * @return Error code.
        */        
        IMPORT_C TInt GetSupportedVoices( TLanguage aLanguage, 
                                          RArray<TTtsStyle>& aVoices );

private:
	
		TAny* iReservedPtr_1;	// reserved for future expansion
		TAny* iReservedPtr_2;	// reserved for future expansion
};

#endif // NSSTTSCUSTOMCOMMANDS_H

// End of file
