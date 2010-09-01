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


#ifndef VUICCONTACTRESULTSSTATE_H
#define VUICCONTACTRESULTSSTATE_H

// INCLUDES
#include "vuicstate.h"

// FORWARD DECLARATIONS

class CNBestListDialog;
class CDesC16ArrayFlat;
class MNssTag;
class CNssVASDBMgr;
class MNssTagMgr;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CContactResultsState ) : public CState
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CContactResultsState* NewL( CDataStorage& aDataStorage, CUiModel& aUiModel );

        /**
        * Destructor.
        */
        virtual ~CContactResultsState();
                                
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
         
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CContactResultsState( CDataStorage& aDataStorage, CUiModel& aUiModel );
        
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
        * Creates contact info array
        */
        void CreateContactInfoArrayL();
        
        /**
        * Compares given tag and currently selected tag names
        * @param aTag Tag that is used in comparison
        * @return ETrue if names match else EFalse
        */
        TBool CompareContactNamesL( MNssTag* aTag );
        
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
        * Gets the right icon for contact
        * @param aIconId The type of contacts icon
        * @return Icon type   
        */
        TPtrC GetContactIconType( TFieldType aIconId );
        
        /**
        * Selects currently active tag from n-best list
        */
        void SelectTag();
                
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
        
        // Vas stuff for handling context operations
        CNssVASDBMgr*               iNssVasDbManager;
        MNssTagMgr*                 iNssTagManager;
    };

#endif  // VUICCONTACTRESULTSSTATE_H

// End of File
