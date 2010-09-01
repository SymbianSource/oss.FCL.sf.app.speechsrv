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
* Description:  View class for folders
*
*/


// INCLUDE FILES

#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <AknGlobalNote.h>
#include <AknWaitDialog.h>

#include <vcommand.rsg>
#include "vcappui.h"
#include "vcfolderview.h"
#include "vcommandconstants.h"
#include "vcommand.hrh"
#include "vcommandconstants.h"


// ================= MEMBER FUNCTIONS =======================

//
//
//
CVCFolderView::CVCFolderView( /*CVCommandHandler& aService,*/ CVCModel& aModel ) : 
    CVCGenericView( /*aService,*/ aModel ) 
    {   
    }


// ---------------------------------------------------------
// TUid CVCDefaultView::Id
// ---------------------------------------------------------
//
TUid CVCFolderView::Id() const 
    {
    return KUidVCFolderView;
    }


// End of File

