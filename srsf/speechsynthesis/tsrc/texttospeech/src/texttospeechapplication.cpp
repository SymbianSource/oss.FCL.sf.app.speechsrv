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
* Description:  
*
*/


#include <eikstart.h>

#include "texttospeechapplication.h"
#include "texttospeechdocument.h"
#include "texttospeechuid.hrh"

static const TUid KAppUid = { KUidTextToSpeechApplication };

/**
 *  Called by the application framework to construct the application object
 *  @return The application (CTextToSpeechApplication)
 */    
LOCAL_C CApaApplication* NewApplication()
    {
    return new CTextToSpeechApplication;
    }

/**
 * This standard export is the entry point for all Series 60 applications
 * @return error code
 */    
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

/**
 * Returns the application's UID (override from CApaApplication::AppDllUid())
 * @return UID for this application (KUidTextToSpeechApplication)
 */
TUid CTextToSpeechApplication::AppDllUid() const
    {
    return KAppUid;
    }

/**
 * Creates the application's document (override from CApaApplication::CreateDocumentL())
 * @return Pointer to the created document object (CTextToSpeechDocument)
 */
CApaDocument* CTextToSpeechApplication::CreateDocumentL()
    {
    return CTextToSpeechDocument::NewL( *this );
    }

// End of file
