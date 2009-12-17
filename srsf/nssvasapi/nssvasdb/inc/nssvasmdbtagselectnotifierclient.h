/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MNssDBTagSelectNotifierClient is an interface for tag select notification
*				 and provdies a method for clients to be notified of tag selection.
*
*/


#ifndef MNSSDBTAGSELECTNOTIFIERCLIENT_H
#define MNSSDBTAGSELECTNOTIFIERCLIENT_H

// FORWARD DECL
class CNssTag;

// CLASS DECLARATION

/**
*  MNssDBTagSelectNotifierClient: A client implements for tag select notification
*  @lib NssVASApi.lib
*  @since 2.0
*/

class MNssDBTagSelectNotifierClient
{
public:

	// HandleSelection is called into the client by CNssDBTagSelectNotifier
	virtual void HandleSelection(CNssTag* aTag) = 0;

};


#endif      // MDBTAGSELECTNOTIFIERCLIENT_H
            
// End of File
