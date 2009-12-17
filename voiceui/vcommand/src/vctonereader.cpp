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
* Description:  This class handles the tone playing for the Voice commands UI
*
*/


// INCLUDE FILES
#include <avkon.hrh>
#include <aknSoundinfo.h>
#include "vctonereader.h"
#include <eikcolib.h>
#include <aknsoundsystem.h>
#include <aknappui.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CVCToneReader::CVCToneReader()
    {
    }

// Default constructor can leave.
void CVCToneReader::ConstructL()
    {
#ifdef _DEBUG
   RDebug::Print(_L("CVCToneReader::ConstructL"));
#endif

    iAbortInfo = CAknSoundInfo::NewL();
    iErrorInfo = CAknSoundInfo::NewL();

    iAbortToneSequence = ReadToneSequenceL(iAbortInfo,
    							EAvkonSIDNameDiallerAbortTone,
                            	&iAbortTonePriority,&iAbortTonePreference,
                            	&iAbortToneVolume);
                            	
    iErrorToneSequence = ReadToneSequenceL(iErrorInfo,
    							EAvkonSIDNameDiallerErrorTone,
                            	&iErrorTonePriority,&iErrorTonePreference,
                            	&iErrorToneVolume);

    iToneUtility = CMdaAudioToneUtility::NewL(*this);    
    }

// Two-phased constructor.
CVCToneReader* CVCToneReader::NewL()
    {
    CVCToneReader* self = new ( ELeave ) CVCToneReader;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CVCToneReader::~CVCToneReader()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CVCToneReader::~CVCToneReader"));
#endif
   
    delete iToneUtility;
    
    delete iAbortInfo;
    delete iErrorInfo;
    iAbortToneSequence = NULL;
    iErrorToneSequence = NULL;
    }

// ---------------------------------------------------------
// CVCToneReader::ReadToneSequenceL
// Reads the tone sequence from the Avkon resource file
// ---------------------------------------------------------
//
HBufC8* CVCToneReader::ReadToneSequenceL( CAknSoundInfo* aInfo, TInt aSid, 
                                              TInt* aTonePriority,
                                              TMdaPriorityPreference* aTonePreference, 
                                              TInt* aToneVolume )
   {
#ifdef _DEBUG
   RDebug::Print(_L("CVCToneReader::ReadToneSequenceL: aSid = %d"),aSid); 
#endif
	if (iAvkonAppUiBase)
		{
		iAvkonAppUiBase->KeySounds()->RequestSoundInfoL( aSid, *aInfo );	// Static access
		}

   *aTonePriority = aInfo->iPriority;
   *aToneVolume = aInfo->iVolume;
   *aTonePreference = (TMdaPriorityPreference)aInfo->iPreference;
   return aInfo->iSequence;
   }


// ---------------------------------------------------------
// CVCToneReader::TonePriority
// Sets the tone priority 
// ---------------------------------------------------------
//
TInt CVCToneReader::TonePriority(TInt aSid)
   {
#ifdef _DEBUG
   RDebug::Print(_L("CVCToneReader::TonePriority"));  
#endif

   TInt priority = -1;
   switch (aSid)
      {
      case EAvkonSIDNameDiallerErrorTone:
         priority = iErrorTonePriority;
         break;

      case EAvkonSIDNameDiallerAbortTone:
         priority = iAbortTonePriority;
         break;

      default:
         break;
      }
   return priority;
   }

// ---------------------------------------------------------
// CVCToneReader::TonePriorityPreference
// Sets the appropriate priority preference
// ---------------------------------------------------------
//
TMdaPriorityPreference CVCToneReader::TonePriorityPreference(TInt aSid)
   {
#ifdef _DEBUG
   RDebug::Print(_L("CVCToneReader::TonePriorityPreference"));    
#endif
   TMdaPriorityPreference preference;
   switch (aSid)
      {
      case EAvkonSIDNameDiallerErrorTone:
         preference = iErrorTonePreference;
         break;

      case EAvkonSIDNameDiallerAbortTone:
         preference = iAbortTonePreference;
         break;

      default:
         preference = iErrorTonePreference;
         break;
      }
   return preference;
   }


// ---------------------------------------------------------
// CVCToneReader::ToneSequence
// Sets the appropriate tone sequence
// ---------------------------------------------------------
//
HBufC8* CVCToneReader::ToneSequence(TInt aSid)
   {
#ifdef _DEBUG
   RDebug::Print(_L("CVCToneReader::ToneSequence"));  
#endif
   HBufC8* sequence = NULL;

   switch (aSid)
      {
      case EAvkonSIDNameDiallerErrorTone:
         sequence = iErrorToneSequence;
         break;

      case EAvkonSIDNameDiallerAbortTone:
         sequence = iAbortToneSequence;
         break;

      default:
         break;
      }
   return sequence;
   }



// ---------------------------------------------------------
// CVCToneReader::ToneVolume
// Sets the appropriate tone volume
// ---------------------------------------------------------
//
TInt CVCToneReader::ToneVolume(TInt aSid)
   {
#ifdef _DEBUG
   RDebug::Print(_L("CVCToneReader::ToneVolume"));    
#endif
   TInt volume = -1;

   switch (aSid)
      {
      case EAvkonSIDNameDiallerErrorTone:
         volume = iErrorToneVolume;
         break;

      case EAvkonSIDNameDiallerAbortTone:
         volume = iAbortToneVolume;
         break;

      default:
         break;
      }
   return volume;
   }


// ---------------------------------------------------------
// CVCToneReader::PlayTone
// Plays the appropriate tone
// ---------------------------------------------------------
//
void CVCToneReader::PlayTone(MVCTonePlayer* aTonePlayer,TInt aSid)
    {
    iTonePlayer = aTonePlayer;
    iTonePriority = TonePriority(aSid);
    iTonePreference = TonePriorityPreference(aSid);
    iToneVolume = ToneVolume(aSid);

    iToneUtility->PrepareToPlayDesSequence(ToneSequence(aSid)->Des());
    }


// ---------------------------------------------------------
// CVCToneReader::MatoPrepareComplete
// Callback from the CMdaToneUtility when tone preparation is 
// complete
// ---------------------------------------------------------
//
void CVCToneReader::MatoPrepareComplete(TInt aError)
    {
   	if (aError == KErrNone)
      	{
      	iToneUtility->SetPriority(iTonePriority, iTonePreference);
      	iToneUtility->SetVolume(iToneVolume);
      	iToneUtility->Play();
      	}
      else
      	{
      	if (iTonePlayer)
    		{
    		iTonePlayer->MntpToneError();
    		}
      	}
	}
    

// ---------------------------------------------------------
// CVCToneReader::MatoPlayComplete
// Callback from the CMdaToneUtility when the tone playing
// is complete
// ---------------------------------------------------------
//
void CVCToneReader::MatoPlayComplete(TInt /*aError*/)
    {
#ifdef _DEBUG
   RDebug::Print(_L("CVCToneReader::MatoPlayComplete")); 
#endif
    if (iTonePlayer)
    	{
    	iTonePlayer->MntpToneComplete();
    	}
    }


//  End of File  







