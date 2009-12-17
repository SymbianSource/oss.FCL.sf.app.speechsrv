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


#ifndef TEXTTOSPEECHDOCUMENT_H
#define TEXTTOSPEECHDOCUMENT_H

#include <akndoc.h>
        
class CEikAppUi;

/**
* A CAknDocument-derived class is required by the S60 application 
* framework. It is responsible for creating the AppUi object. 
*/
class CTextToSpeechDocument : public CAknDocument
    {

    public: 
    
        // constructor
        static CTextToSpeechDocument* NewL( CEikApplication& aApp );
        
        // destructor.
        virtual ~CTextToSpeechDocument();
        
    private: 
    
        // constructors
        CTextToSpeechDocument( CEikApplication& aApp );
        void ConstructL();
    
    public: 
    
        // from base class CEikDocument
        CEikAppUi* CreateAppUiL();
        
    };

#endif // TEXTTOSPEECHDOCUMENT_H
