/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface towards Tts custom command parser, public methods
*                called by the MMF
*
*/


#ifndef TTSCUSTOMCOMMANDPARSER_H
#define TTSCUSTOMCOMMANDPARSER_H

// INCLUDE FILES
#include <mmfcontroller.h>
#include <nssttscustomcommandimplementor.h>

// CLASS DECLARATION

/**
* Server side parser for TTS custom commands. 
*
* @since 2.8
* @lib nssttscustomcommands.lib
*/
class CTtsCustomCommandParser : public CMMFCustomCommandParserBase
    {
    
    public:
        
        /** 
        * Factory function for creating this class.
        *
        * @since 2.8
        * @param "MTtsCustomCommandImplementor& aImplementor" Rreference to 
        *        custom commands implementor.
        * @return Pointer to CSpeechRecognitionCustomCommandParser object.
        */
        IMPORT_C static CTtsCustomCommandParser* NewL( MTtsCustomCommandImplementor& aImplementor );
        
        /**
        * Destructor
        */
        IMPORT_C  ~CTtsCustomCommandParser();
        
        /**
        * Handles the request from the client
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" Message containing the CC information.
        */
        void HandleRequest( TMMFMessage& aMessage );
        
    private:
        
        /**
        * C++ constructor for CSpeechRecognitionCustomCommandParser class.
        *
        * @since 2.8
        * @param "MTtsCustomCommandImplementor& aImplementor" Reference to 
        *        custom commands implementor.
        */
        CTtsCustomCommandParser( MTtsCustomCommandImplementor& aImplementor );
        
        /**
        * Private method to handle the request from the client.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" Message containing the information
        *        of the request.
        */
        void DoHandleRequestL( TMMFMessage& aMessage );

        /**
        * Uses plugin to add a new style.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoAddStyleL( TMMFMessage& aMessage );

        /**
        * Uses plugin to delete a style.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoDeleteStyleL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get the synthesis position.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoGetPositionL( TMMFMessage& aMessage );
      
        /**
        * Uses plugin to get the number of registered styles.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoNumberOfStylesL( TMMFMessage& aMessage );

        /**
        * Uses plugin to open a parsed text source.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoOpenParsedTextL( TMMFMessage& aMessage );

        /**
        * Uses plugin to set synthesis position.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoSetPositionL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get style based on its ID.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoStyleIDL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get style based on its index.
        *
        * @since 2.8
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoStyleIndexL( TMMFMessage& aMessage );

        /**
        * Uses plugin to set default style to be used in synthesis
        * if not set otherwise. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoSetDefaultStyleL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get the default style in use. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoGetDefaultStyleL( TMMFMessage& aMessage );

        /**
        * Uses plugin to set speaking rate. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoSetSpeakingRateL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get spekaing rate in use. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoGetSpeakingRateL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get supported synthesis languages. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoGetSupportedLanguagesL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get number of supported synthesis languages. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoGetSupportedLanguagesCountL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get supported voices of particular language. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoGetSupportedVoicesL( TMMFMessage& aMessage );

        /**
        * Uses plugin to get number of supported voices of particular language. 
        *
        * @since 3.2
        * @param "TMMFMessage& aMessage" MMF message.
        */
        void DoGetSupportedVoicesCountL( TMMFMessage& aMessage );

    private:
    
        // Reference to the Custom Command Implementor class
        MTtsCustomCommandImplementor& iImplementor; 
  
        TAny* iReservedPtr1; // Reserved for future expansion
        TAny* iReservedPtr2; // Reserved for future expansion
};

#endif // TTSCUSTOMCOMMANDPARSER_H

// End of file
