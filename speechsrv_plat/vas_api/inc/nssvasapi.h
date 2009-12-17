/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains a list of headers constituting the VAS APIs
*               A VAS client can choose to include all headers at once, or include
*               a header corresponding to the service required.
*
*/


// for Tag and Context management
#include <nssvascvasdbmgr.h>
#include <nssvasmtagmgr.h>				
#include <nssvasmcontextmgr.h>	

// for operations on a Tag, Context, RRD, and SpeechItem
#include <nssvasmtag.h>
#include <nssvasmcontext.h>			
#include <nssvasmrrd.h>
#include <nssvasmspeechitem.h>	
#include <nssvascoreconstant.h>
#include <nssvasmtagreference.h>

// for Tag train and playback call backs			
#include <nssvasmplayeventhandler.h>	
#include <nssvasmtrainvoiceeventhandler.h>
#include <nssvasmtraintexteventhandler.h>
		
// for Tag and Context: get, save and delete call backs
#include <nssvasmgettagclient.h>
#include <nssvasmsavetagclient.h>	
#include <nssvasmdeletetagclient.h>
#include <nssvasmgetcontextclient.h>			
#include <nssvasmsavecontextclient.h>	
#include <nssvasmdeletecontextclient.h>
#include <nssvasmgettagreferenceclient.h>			
	
// for NssVAS DB change notification
#include <nssvasmvasdatabaseobserver.h>
#include <nssvasmvasdbeventnotifier.h>
#include <nssvascvasdbevent.h>	

// for Tag recognition, and recognition call backs
#include <nssvascrecognitionhandlerbuilder.h>
#include <nssvasmrecognitionhandler.h>		
#include <nssvasmrecognizeeventhandler.h>
#include <nssvasmrecognizeinitcompletehandler.h>
#include <nssvasmrejecttagcompletehandler.h>

// for Tag selection, and tag selection call backs
#include <nssvascselectnotificationbuilder.h>
#include <nssvasmtagselecthandler.h>	
#include <nssvasmtagselectnotification.h>

// for Contact Handler
#include <nssvasccontacthandler.h>
