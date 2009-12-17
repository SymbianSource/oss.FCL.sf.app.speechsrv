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



#include "texttospeechdocument.h"
#include "texttospeechappui.h"

/**
 * Constructs the document class for the application.
 * @param aApplication the application instance
 */
CTextToSpeechDocument::CTextToSpeechDocument( CEikApplication& aApplication ) :
    CAknDocument( aApplication )
    {
    }

/**
 * Destructor
 */
CTextToSpeechDocument::~CTextToSpeechDocument()
    {
    }

/**
 * Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here.  
 */ 
void CTextToSpeechDocument::ConstructL()
    {
    }
    
/**
 * Symbian OS two-phase constructor.
 *
 * Creates an instance of CTextToSpeechDocument, constructs it, and
 * returns it.
 *
 * @param aApp the application instance
 * @return the new CTextToSpeechDocument
 */
CTextToSpeechDocument* CTextToSpeechDocument::NewL( CEikApplication& aApp )
    {
    CTextToSpeechDocument* self = new (ELeave) CTextToSpeechDocument( aApp );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

/**
 * Creates the application UI object for this document.
 * @return the new instance
 */    
CEikAppUi* CTextToSpeechDocument::CreateAppUiL()
    {
    return new (ELeave) CTextToSpeechAppUi();
    }

// End of file
