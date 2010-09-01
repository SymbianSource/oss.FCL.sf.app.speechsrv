/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Generic interface for VUI plug-in
*
*/


#ifndef NSSVOICEUIPLUGIN_H
#define NSSVOICEUIPLUGIN_H

// TYPE DEFINITIONS

enum TVuipPriority
    {
    EVuipPriorityExtraLow = 0,
    EVuipPriorityLow = 25,
    EVuipPriorityNormal = 50,
    EVuipPriorityHigh = 75,
    EVuipPriorityExtraHigh = 100
    };


// CLASS DECLARATION

/**
*  MVoiceUiPlugin defines an interface for VUI plug-ins
*
*  @since 3.1
*/
class CVoiceUiPluginBase : public CBase
    {
     
    public:
    
        /**
        * Returns a reference to the array of Uids of events, which are handled by the plug-in.
        * @since 3.1
        * @param None
        * @return Event Uids
        */
        virtual RArray<TUid>& EventUids() = 0;

        /**
        * Runs a task of plug-in
        * @since 3.1
        * @param aEvent Event which has happened
        * @return status of execution: standard error or VUIPF status values
        * @see nssvoicestatus.h
        */
        virtual TInt Execute( TUid aEvent ) = 0;

        /**
        * Initializes a plug-in
        * @since 3.1
        * @param None
        * @return None.
        */
        virtual void InitializeL() = 0;

        /**
        * Retirieves a current priority value
        * @since 3.1
        * @param None
        * @return Priority value. 0 is idle, 100 is highest priority.
        * @see TVuipPriority
        */
        inline virtual TVuipPriority Priority() = 0;

    };

#endif // NSSVOICEUIPLUGIN_H   

// End of File
