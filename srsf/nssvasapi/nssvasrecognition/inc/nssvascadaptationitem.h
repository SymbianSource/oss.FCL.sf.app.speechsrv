/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The MNssAdaptationItem interface class handles speaker adaptation,
*               when the client wants to decide herself, when to adapt.
*
*/


#ifndef NSSVASCADAPTATIONITEM_H
#define NSSVASCADAPTATIONITEM_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <nssvasmadaptationitem.h>
#include <nssvasmadaptationeventhandler.h>
#include "nssvascrecognitionhandler.h"

// CLASS DECLARATION

/**
*  CNssAdaptationItem class
*  Contains adaptation data.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssAdaptationItem : public CBase, public MNssAdaptationItem
    {

    public:  //enum

	  enum TNssRejectTagError
        {
        EVasErrorNone,
        EVasRejectTagFailed,
        EVasNoTagInContexts,
        EVasRecognizeUndefinedError10,   // for place holder
        EVasRecognizeUndefinedError9,
        EVasRecognizeUndefinedError8,
        EVasRecognizeUndefinedError7,
        EVasRecognizeUndefinedError6,
        EVasRecognizeUndefinedError5,
        EVasRecognizeUndefinedError4,
        EVasRecognizeUndefinedError3,
        EVasRecognizeUndefinedError2,
        EVasRecognizeUndefinedError1
        };

    public: // Constructors and destructors

    /**
    * NewL - 2-phase constructor
    * @param aRecHandlerHost The recognition handler, which created this item.
    * @param aRecResult Recognition result contains the data required for adaptation.
    * @since 2.8
    */
    IMPORT_C static CNssAdaptationItem* NewL(
        CNssRecognitionHandler* aRecHandlerHost,
        CSIClientResultSet* aRecResult );

    /**
	* ~MNssAdaptationItem - Destructor  
    * @since 2.8
    */
    IMPORT_C ~CNssAdaptationItem();

public: // Functions from base classes

    /**
	* AdaptL - Adapts the models using utterance and result
    * @from MNssAdaptationItem
    * @since 2.8
    * @param aHandler Adaptation handler is called when adaptation has finished.
    * @param aCorrect Which result from the N-best list is the correct one.
    */
    IMPORT_C void AdaptL( MNssAdaptationEventHandler *aHandler, TInt aCorrect );

    /**
    * Adapts the models using the tag information
    * 
    * @since 3.2
    * @param aHandler Event handler for this adaptation
    * @param aTag Selected tag
    */ 
    IMPORT_C void AdaptL( MNssAdaptationEventHandler* aHandler,
                          MNssTag& aTag );

public: // New functions

    /**
    * Disable - CNssRecognitionHandler calls this function, when the handler
    * is destroyed. It deactivates the adaptation item, since adaptation
    * can't happen without the recognition handler host.
    * @since 2.8
    */
    IMPORT_C void Disable();

    private: // New functions

    /**
    * 1st phase constructor
    */
    CNssAdaptationItem( CNssRecognitionHandler* aRecHandlerHost, CSIClientResultSet* aRecResult );

private: // Data

    // Pointer to the recognition handler host
    CNssRecognitionHandler* iRecHandler;

    // If the recognition handler host is destroyed, the associated 
    // adaptation items are deactivated.
    TBool iIsActive;

    // Recognition result set contains the data required for adaptation.
    CSIClientResultSet* iAdaptationData;
    };

#endif // NSSVASCADAPTATIONITEM_H   
            
// End of File
