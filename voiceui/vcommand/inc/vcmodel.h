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
* Description:  Model class for vcommand application wrapping CVCommandHandler
*
*/


#ifndef CVCMODEL_H
#define CVCMODEL_H

// INCLUDES
#include <vcommandapi.h>
#include "vcommanduientry.h"
#include "vcommanduientryarray.h"
#include "vcmodelappuiinterface.h"

//class MVcModelAppUiInterface;

// CLASS DECLARATION

/**
* Wraps a list voice commands (CVCommand-objects wrapped in CVCommandUiEntry-
* objects). 
*/
class CVCModel : public CBase, public MVCommandHandlerObserver
    {
    public: // Constructor and destructor
    
        /**
        * Two-phased constructor.
        */
        static CVCModel* NewL( MVcModelAppUiInterface* aAppUi );

        /**
        * Destructor.
        */      
        virtual ~CVCModel();
        
    public: // Functions from base classes
    
        /**
        * From MVCommandHandlerObserver. Is called whenever the VCommand set is
    	* changed by another instance of CVCommandHandler (not the one this
    	* class wraps. This function, however,does not load the new set.
        */
        void CommandSetChanged();
        
    public: // New Functions
    
        /**
        * Returns a CVCommandUiEntry-object based on its written text. If a
        * command matching with the written text is not found, this function
        * leaves
        * @param aWrittenText Text displayed in for this voice command
        * @return CVCommandUiEntry
        */
        virtual const CVCommandUiEntry& GetVCommandL( const TDesC& aWrittenText );
        
        /**
        * Returns a CVCommandUiEntry-object.
        * @param aIndex Commands index in the list of commands
        */
        virtual const CVCommandUiEntry& At( TInt aIndex ) const;
        
        /**
        * Loads vcommands from CVCommandHandler
        */
        virtual void LoadVCommandsL();
        
        /**
        * Sets a new spoken text for a voice vommand.
        * @param aWrittenText Text displayed for this voice command
        */
        virtual void SetNewSpokenTextL( const TDesC& aWrittenText, const TDesC& aNewSpokenText );
        
        /**
        * Returns a boolean value on whether a voice command with a given written text exists.
        * @param aWrittenText Text displayed in listbox for this voice command
        * @return TBool
        */
        virtual TBool HasCommandL( const TDesC& aWrittenText );
        
        /**
        * Returns the number of voice commands
        * @return TInt
        */
        virtual TInt Count() const;
        
        /**
        * Returns a list of CVCommandUiEntry-objects. Ownership of the array
        * is passed to the client.
        * @return CVCommandUiEntryArray
        */
        CVCommandUiEntryArray* VCommandArrayL();
        
        /**
        * Returns a CVCommandHandler-object this class wraps.
        */
        CVCommandHandler& Service();
        
    protected:
        /**
        * C++ default constructor.
        */
		CVCModel( MVcModelAppUiInterface* aAppUi );
        
	private:

        /**
        * By default Symbian OS constructor is private.
        */
		void ConstructL();
		
    private: // New Functions
    
        /**
        * Utility function returning the index of a voice command in the list
        * of voice commands
        * @param aWrittenText Text displayed in listbox for a voice command
        * @return TInt
        */
        TInt FindVCommandIndexL( const TDesC& aWrittenText );        
    
    private: // data
    
        MVcModelAppUiInterface* iAppUi;
    
        CVCommandHandler*       iVcHandler;
        
        CVCommandUiEntryArray*  iListBoxVCommands;
    };

#endif  // CVCMODEL_H

// End of File


