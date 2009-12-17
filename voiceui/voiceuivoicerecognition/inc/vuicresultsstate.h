/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUICRESULTSSTATE_H
#define VUICRESULTSSTATE_H

// INCLUDES
#include <cntdef.h> 
#include <mccaconnection.h>

#include "vuicstate.h"

// FORWARD DECLARATIONS

class CNBestListDialog;
class CDesC16ArrayFlat;
class MNssTag;
class MCCAConnection;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CResultsState ) : public CState,
                                     public MCCAObserver
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CResultsState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CResultsState();
                                
    public:     // New functions
    
        /**
        * Handles received event
        * @param Event to be handled
        */
        virtual void HandleEventL( TInt aEvent );
        
        /**
        * Executes the state functionality
        */
        virtual void ExecuteL();
        
    public:
        
        /**
        * From MCCAObserver
        * @see MCCAObserver for more information
        */
        void CCASimpleNotifyL( TNotifyType aType, TInt aReason );
         
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CResultsState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
        
        /**
        * Handles timed event
        */
        void DoTimedEventL();
        
    private:    // Functions        
        
        /**
        * Handles keypress event
        * @return ETrue if event was handled else EFalse
        */
        TBool HandleKeypressL();
        
        /**
        * Plays currently selected name from the n-best list with TTS
        */
        void PlaySelectedNameL();
        
        /**
        * Changes options menu commands visibility if necessary
        */
        void CheckOptionsCommands();
        
        /**
        * Creates a contact array.
        */
        void CreateContactArrayL();
        
        /**
        * Updates contact array.
        * @return ETrue if array was updated else EFalse
        */
        TBool UpdateContactArray();
        
        /**
        * Checks if given tag exists in taglist
        * @param aTag Tag that is searched
        * @return ETrue if tag exists else EFalse
        */
        TBool CheckIfAlreadyAdded( MNssTag* aTag );
        
        /**
        * Creates new items array. Ownership is transferred to caller.
        * @return Items array     
        */
        CDesC16ArrayFlat* CreateItemsArrayL();
       
        /**
        * Fills buffer with dial tag information
        * @param aTag Tag containing all needed information
        * @param aBuffer Buffer to be filled
        */
        void FillDialContextBufferL( MNssTag* aTag, TDes& aBuffer );
        
        /**
        * Fills buffer with command tag information
        * @param aTag Tag containing all needed information
        * @param aBuffer Buffer to be filled
        */
        void FillCommandContextBuffer( MNssTag* aTag, TDes& aBuffer );
       
        /**
        * Gets the right icon for contact
        * @param aFieldType The type of contacts field
        * @return Icon type   
        */
        TPtrC GetContactIconType( TFieldType aFieldType );
        
        /**
        * Selects currently active tag from n-best list
        */
        void SelectTag();
        
        /**
        * Opens phonebook info view of the selected contact
        */
        void OpenPhonebookContactL();
               
    private:    // Data
    
        enum TInternalState
            {
            ENotStarted,
            EStarted,
            EInitialized,
            ECompleted
            };
    
        // N-best list dialog
        CNBestListDialog*           iNBestListDialog;
        
        // Stores a keypress event from HandleEventL
        TInt                        iKeypress;
        
        // For handling internal state logics
        TInternalState              iInternalState;
                
        // For storing copies of tag pointers
        CArrayPtrFlat<MNssTag>*     iTagList;
        
        MCCAConnection*             iConnection;
    };

#endif  // VUICRESULTSSTATE_H

// End of File
