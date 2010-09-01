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
* Description:  The CNssSpeechItemBuilder provides methods to create SpeechItem object.
*
*/


#ifndef NSSVASCSPEECHITEMBUILDER_H
#define NSSVASCSPEECHITEMBUILDER_H

#include "nssvasccontext.h"
#include "nssvascspeechitem.h"
#include "nssvascspeechitemtrainer.h"
#include "nssvasmcoresyncrecoveryhandler.h"

// FORWARD DECLARATIONS
class CNssVASDatabase;

/**
* The CNssSpeechItemBuilder class builds SpeechItem instances when requested.
*
*  @lib NssVASApi.lib
*  @since 2.8
*/
class CNssSpeechItemBuilder : public CBase
{
  public:

    /**
    * Destructor.
    * The CNssSpeechItemBuilder destructor
    * Destroys the speech item builder. In addition, this must set the
    * state of the speech item portal termination state to termination pending.
    */   
    ~CNssSpeechItemBuilder();

    /**
    * Creates SpeechItemBuilder.  In addition, this must also create the
    * Speech Item Portal and Speech Item Trainer.
    * @since 2.0
    * @param aDatabase - interface to database
    * @return 
    */       
    static CNssSpeechItemBuilder* NewL( CNssVASDatabase* aDatabase );

    /**
    * 2 phase construction
    * @since 2.0
    * @param aDatabase - interface to database
    * @return 
    */      
    static CNssSpeechItemBuilder* NewLC( CNssVASDatabase* aDatabase );
    
    /**
    * Create SpeechItem object given a Context
    * @since 2.0
    * @param aContext - reference of Contaxt object  
    * @return *CNssSpeechItem - a pointer to a SpeechItem object
    */      
    CNssSpeechItem* CreateEmptySpeechItemL(CNssContext& aContext);

    /**
    * Create SpeechItem object given most of its data members.
    * @since 2.0
    * @param aContext - reference of Contaxt object  
    * @param aText - a text description
    * @param aTagID - a TagID 
    * @return *CNssSpeechItem - a pointer to a SpeechItem object
    */   
    CNssSpeechItem* CreateSpeechItemL(CNssContext& aContext,
        TUint32 aRuleID,const TDesC& aText, TInt aTagID,
        TNssVasCoreConstant::TNssTrainedType aTrainedType );


  private: //function


   /**
    * C++ constructor
    */ 
    CNssSpeechItemBuilder();


    /**
    * Symbian 2nd phase constructor 
    */      
    void ConstructL( CNssVASDatabase* aDatabase );


  private: //data

    // Pointer to SRS Portal object.
    // The SpeechItemBuilder own the SpeechItemPortal
    CNssSpeechItemSrsPortal* iPortal; 
    CNssSpeechItemTrainer*  iTrainer;
};



#endif // NSSVASCSPEECHITEMBUILDER_H

// End of file
