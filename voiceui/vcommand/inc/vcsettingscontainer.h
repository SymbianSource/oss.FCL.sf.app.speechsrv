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
* Description:  Settings container
*
*/


#ifndef CVCSETTINGSCONTAINER_H
#define CVCSETTINGSCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <coedef.h>

// FORWARD DECLARATIONS
class CEikTextListBox;
class CVCSettingsView;
class CVCSettingsList;
CEikButtonGroupContainer;

// CLASS DECLARATION

/**
* Container for the settings view
* @since 2.8
*/
class CVCSettingsContainer : public CCoeControl,
								             public MCoeControlObserver,
								             public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * C++ constructor.
        */
        CVCSettingsContainer( CEikButtonGroupContainer& aCbaGroup );

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        virtual ~CVCSettingsContainer();
        
    public: // Functions from base classes
        
        /**
        * From CCoeControl
        * Gives the help context to be displayed
        * @param aContext help context related to current view
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        /**
		    * From MEikListBoxObserver
		    * @see MEikListBoxObserver for more information
		    */
		    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

    public: // New functions

        /**
        * Open setting page for currently selected setting item.
        * @param aCalledFromMenu True if editing command was chosen from the
                 options menu 
		* @return void
        */
        void EditCurrentL(  TBool aCalledFromMenu );

		/**
		* Shows reset confimation dialog
		* @return void
		*/
		void ResetL();

        /**
        * Save all settings.
		* @return void
        */
        void SaveSettingsL();

		/**
        * Returns current listbox index
		* @return void
        */
		TInt CurrentItemIndex() const;

        /**
        * Returns the size of the list box.
        *
        */
		TInt ItemListSize() const;
		
		/**
		* Returns the setting item identifier for the current setting item.
        * @return TInt The setting item identifier
		*/
		TInt CurrentItemIdentifier() const;
		
		/**
        * Sets the middle softkey label. NOTE: this should be handled in the
        * view class in a normal case where container would call this function
        * after focused item has changed. But since there are two view classes
        * (normal and plugin) that both use this container class, we can
        * only have a pointer to the views' super class.
        */
        void SetMiddleSoftkeyLabelL() const;
		

    protected: // Functions from base classes

        /**
        * From CCoeControl return the number of controls owned
        * @return TInt number of controls
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl returns a control
        * @param aIndex index of a control
        * @return CCoeControl* pointer on a control
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl event handling section
        * @param aKeyEvent the key event
        * @param aType the type of the event
        * @return TKeyResponse key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

		/**
       	* From CCoeControl
       	* @see CCoeControl for more information
       	*/
        void FocusChanged(TDrawNow aDrawNow);

		/**
        * From CCoeControl
        * @see CCoeControl for more information
        */
        void SizeChanged();

        /**
        * From CCoeControl
        * @see CCoeControl for more information
        */
        void HandleResourceChange(TInt aType);      
        
        /**
        * From MCoeControlObserver
        * @see MCoeControlObserver for more information
        */
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);        
        
	private:

		/**
        * returns title resource id
        * @return TInt
        */
        TInt TitleResourceId() const;
        
        /**
         * Sets the middle softkey label.
         * @param aResourceId Label text from resource file
         * @param aCommandId Menu c
         */                          
        void DoSetMiddleSoftKeyLabelL( const TInt aResourceId,
                                       const TInt aCommandId  ) const;

        /**
         * Remove MSK command mappings.
         */                          
        void RemoveCommandFromMSK() const;
        
    private: //data
    
        CVCSettingsList*	iSettingsList;
        CEikButtonGroupContainer& iCbaGroup;
        
	};

#endif // CVCSETTINGSCONTAINER_H

// End of File
