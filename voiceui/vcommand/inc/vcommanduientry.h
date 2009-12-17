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
* Description:  Class to wrap a CVCommand object
*
*/


#ifndef CVCOMMANDUIENTRY_H
#define CVCOMMANDUIENTRY_H

#include <e32base.h>

class CVCommand;
class CVCModel;
class CGulIcon;
class CVCommandHandler;
class MNssPlayEventHandler;

// CLASS DECLARATION

/**
* Class to wrap a CVCommand-object for abstraction and for simplified use.
*/
class CVCommandUiEntry : public CBase
    {
    public: // Constructor and destructor
    
        /**
        * Two-phased constructor.
        */
        static CVCommandUiEntry* NewL( const CVCommand& aCommand );

        /**
        * Destructor.
        */      
        virtual ~CVCommandUiEntry();
        
    public: // New Functions

        /**
        * @return Unmodifiable user-specified shortcut for the command
        *         KNullDesC if none
        */
        virtual const TDesC& UserText() const;
        
        /**
        * @return Unmodifiable written text
        */
        virtual const TDesC& WrittenText() const;
        
        /**
        * @return the text that the user is expected to pronounce
        */
        virtual const TDesC& SpokenText() const;
        
        /**
        * @return the user specified text that the user can pronounce to
        *         recognize the command. Is always identical to UserText
        */
        virtual const TDesC& AlternativeSpokenText() const;
        
        /**
         * Version of the title shown when the folder is displayed in the list
         * of voice commands
         */
        virtual const TDesC& FolderListedName() const;
        
        /**
         * Returns folder title to be shown at the top of the screen
         */
        virtual const TDesC& FolderTitle() const;
        
        /**
         * Returns folder title to be shown at the top of the screen
         */
        virtual const TDesC& Tooltip() const;
        
	    /**
        * Creates the command icon.
        * @return The command icon pushed to the cleanup stack
        */
        virtual CGulIcon* IconLC() const;
        
	    /**
        * Creates an icon for the folder to which this command belongs to.
	    * @return Icon for the folder
	    */
        virtual CGulIcon* FolderIconLC() const;
        
        /**
        * Asynchronous
        * Attempts to play the text expected to be recognized.
        */
        virtual void PlaySpokenTextL( CVCommandHandler& aService, 
            MNssPlayEventHandler& aPlayEventHandler ) const;
        
        /**
        * Asynchronous
        * Plays the user-specified alternative spoken text.
        */
        virtual void PlayAlternativeSpokenTextL( CVCommandHandler& aService,
            MNssPlayEventHandler& aPlayEventHandler ) const;
          
        /**
        * Cancels playback of a spoken or alternative spoken text
        * @param aHandler CVCommandHandler where the command is stored
        */  
        virtual void CancelPlaybackL( CVCommandHandler& aService ) const;
        
        /**
        * Checks whether this voice command object is equivalent to another one
        * @param aCommand A voice command to compare
        * @return TBool
        */  
        TBool operator==( const CVCommandUiEntry& aCommand ) const;
        
        /**
        * Utility function for getting the CVCommand-object this class wraps.
        */
        const CVCommand& Command() const;
        
	protected:
	
	    /**
        * Constructor
        */
        CVCommandUiEntry( const CVCommand& aCommand );
    
    private: // data
        const CVCommand& iCommand;
    };

#endif  // CVCOMMANDUIENTRY_H

// End of File


