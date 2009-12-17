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
* Description:  Mixin class which will be implemented by the TTS plugin.
*
*/


#ifndef NSSTTSCUSTOMCOMMANDIMPLEMENTOR_H
#define NSSTTSCUSTOMCOMMANDIMPLEMENTOR_H

// INCLUDES
#include <nssttscommon.h>

// CLASS DECLARATION

/**
* Mixin class which should be implemented by the controller plugin.
*
* @lib nssttscustomcommands.lib
* @since 2.8
*/
class MTtsCustomCommandImplementor
    {
    
    public:
        /**
        * Adds a new style.
        *
        * @since 2.8
        * @param "TTtsStyle& aStyle" Style to add.
        */
        virtual TTtsStyleID MttscAddStyleL( const TTtsStyle& aStyle ) = 0;

        /**
        * Deletes a style based on style ID.
        *
        * @since 2.8
        * @param "TTtsStyleID aID" Style ID to be deleted.
        */
        virtual TInt MttscDeleteStyle( TTtsStyleID aID ) = 0;

        /**
        * Gets the synthesis position.
        *
        * @since 2.8
        * @retval "TInt& aWordIndex" Returned position of synthesis.
        */
        virtual void MttscGetPositionL( TInt& aWordIndex ) = 0;

        /**
        * Returns the number of registered styles.
        *
        * @since 2.8
        * @return Number of styles.
        */
        virtual TUint16 MttscNumberOfStyles() = 0;

        /**
        * Opens parsed text as source.
        *
        * @since 2.8
        * @param "CTtsParsedText& aText" Reference to parsed text.
        */
        virtual void MttscOpenParsedTextL( CTtsParsedText& aText ) = 0;

        /**
        * Sets the synthesis position based on word index.
        *
        * @since 2.8
        * @param "TInt aWordIndex" Word index.
        */
        virtual void MttscSetPositionL( TInt aWordIndex ) = 0;

        /**
        * Returns the style based on ID.
        *
        * @since 2.8
        * @param "TTtsStyleID aStyleID" Style ID.
        * @return Reference to style object.
        */
        virtual TTtsStyle& MttscStyleL( TTtsStyleID aStyleID ) = 0;

        /**
        * Returns the style based on index.
        *
        * @since 2.8
        * @param "TUint16 aIndex" Style index.
        * @return Reference to style object.
        */
        virtual TTtsStyle& MttscStyleL( TUint16 aIndex ) = 0;
        
        /**
        * Sets the default style parameters for synthesis.
        *
        * @param "const TTtsStyle& aStyle" Style created by the client
        * @since 3.2        
        */        
        virtual void MttscSetDefaultStyleL( const TTtsStyle& aStyle ) = 0;
        
        /**
        * Returns the registered default style
        *
        * @return Style reference
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2             
        */
        virtual TTtsStyle& MttscDefaultStyleL() = 0;
        
        /**
        * Sets the speaking rate of synthesizer.
        *
        * @param TInt aRate Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @since 3.2        
        */        
        virtual void MttscSetSpeakingRateL( TInt aRate ) = 0;
        
        /**
        * Returns the current speaking rate value.
        *
        * @return Speaking rate value
        * @leave KErrNotSupported if synthesizer does not support speaking rate setting
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2        
        */
        virtual TInt MttscSpeakingRateL() = 0;
      
        /**
        * Returns the list of supported languages.
        * 
        * @param "RArray<TLanguage>& aLanguages" Output parameter which contains the languages
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2        
        */
        virtual void MttscGetSupportedLanguagesL( RArray<TLanguage>& aLanguages ) = 0;
        
        /**
        * Returns the list of supported voices for a certain language.
        * 
        * @param "TLanguage aLanguage" Language
        * @param "RArray<TTtsStyle>& aVoices" Output parameter which contains the voices
        * @leave KErrNotReady if no plugin is loaded to handle the function call
        * @since 3.2
        */        
        virtual void MttscGetSupportedVoicesL( TLanguage aLanguage, RArray<TTtsStyle>& aVoices ) = 0;

    };

#endif // NSSTTSCUSTOMCOMMANDIMPLEMENTOR_H
