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
* Description:  Settings list class
*
*/


#ifndef CVCSETTINGSLIST_H
#define CVCSETTINGSLIST_H

// INCLUDES
#include <aknsettingitemlist.h>
#include <CAknMemorySelectionDialog.h>
#include "vcommand.hrh"

// CLASS DECLARATION

/**
* Settings list class
*/
class CVCSettingsList : public CAknSettingItemList
    {
    public: // Constructors and destructor

        /** 
        * C++ constructor.
        */
        CVCSettingsList();

        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CVCSettingsList();

    public: // New functions

        /**
        * Save settings.
        */
        void SaveSettingsL();

    public: // Functions of base classes

        /**
        * From CAknSettingItemList
        * Launch the setting page for the current item by calling
        * EditItemL on it.
        */
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );
        
        /**
        * From CCoeControl set the size and position of its component controls.
        */
        void SizeChanged();

    private: // Functions from base classes

        /**
        * From CAknSettingItemList Handles creating setting items
        * @param aIdentifier Identifier of the setting item to create
        * @see CAknSettingItemList for more information.
		*/
        CAknSettingItem* CreateSettingItemL( TInt aIdentifier );

        /**
        * From CAknSettingItemList Get setting values
		* @see CAknSettingItemList for more information.
        */
        void LoadSettingsL();
        
        /**
        * Displays "Synthesizer enabled" note
        */
        void SynthesizerEnabledNoteL();
        
        /**
        * Displays "Voice verification disabled" note
        */
        void VoiceVerificationDisabledNoteL();

    private: // Data
        TInt iSynthesizer;
        TInt iVolumeValue;
        TInt iRejectionValue;
        TInt iVerification;
			
    };

#endif // CVCSETTINGSLIST_H

// End of File
