/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef VUICDATASTORAGE_H
#define VUICDATASTORAGE_H

// INCLUDES
#include <e32base.h>

#include <nssvasapi.h>

#include <vuivoicerecogdefs.h>

// FORWARD DECLARATIONS
class CVoiceRecognitionDialogImpl;
class CNssRecognitionHandlerBuilder;
class MNssRecognitionHandler;
class CTonePlayer;
class CPropertyHandler;
class CTutorial;
class CTTSPlayer;
class MVasBasePbkHandler;

// CONSTANTS

const TInt KResetMode = -1;

// CLASS DECLARATION
/**
* COMMENT
*/
NONSHARABLE_CLASS( CDataStorage ) : public CBase
{
    public:     // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        */
        static CDataStorage* NewL( CVoiceRecognitionDialogImpl* aVoiceRecognitionImpl );

        /**
        * Destructor.
        */
        virtual ~CDataStorage();
                                
    public:     // New functions
        
        /**
        * Stores recognition handler builder pointer
        * @param aRecognitionHandlerBuilder Pointer to be stored
        */
        void SetRecognitionHandlerBuilder( CNssRecognitionHandlerBuilder* aRecognitionHandlerBuilder );
        
        /**
        * Stores recognition handler pointer
        * @param aRecognitionHandler Pointer to be stored
        */
        void SetRecognitionHandler( MNssRecognitionHandler* aRecognitionHandler );
        
        /**
        * Stores recognition handler builder pointer
        * @param aRecognitionHandlerBuilder Pointer to be stored
        */
        void SetVerificationRecognitionHandlerBuilder( CNssRecognitionHandlerBuilder* aRecognitionHandlerBuilder );
        
        /**
        * Stores recognition handler pointer
        * @param aRecognitionHandler Pointer to be stored
        */
        void SetVerificationRecognitionHandler( MNssRecognitionHandler* aRecognitionHandler );
                       
        /**
        * Stores additional tag list pointer
        * @param aAdditionalTagList Pointer to be stored
        */
        void SetAdditionalTagList( CArrayPtrFlat<MNssTag>* aAdditionalTagList );
        
        /**
        * Stores tag list pointer
        * @param aTagList Pointer to be stored
        */
        void SetCompleteTagList( CArrayPtrFlat<MNssTag>* aCompleteTagList );
        
        /**
        * Stores tag pointer
        * @param aTag Pointer to be stored
        * @param aDestroyIfExists Destroys old tag if one exists
        */
        void SetTag( MNssTag* aTag, TBool aDestroyIfExists );
        
        /**
        * Stores context pointer
        * @param aContext Pointer to be stored
        */
        void SetContext( MNssContext* aContext );
        
        /**
        * Stores adaptation state
        * @param aAdaptationEnabled ETrue to enable adaptation, else EFalse
        */
        void SetAdaptationEnabled( const TBool aAdaptationEnabled );
        
        /**
        * Stores play duration
        * @param aPlayDuration Play duration in milliseconds
        */
        void SetPlayDuration( const TTimeIntervalMicroSeconds& aPlayDuration );
        
        /**
        * Stores verification mode
        * @param aMode One of TVuiVerificationMode values or if not given the
        * value is reseted
        */
        void SetVerificationMode( const TInt aMode = KResetMode );
        
        /**
        * Sets verification try count
        * @param aCount Number of tries left
        */
        void SetVerificationTryCount( const TInt aCount );
        
        /**
        * Sets flag if device lock mode is enabled
        * @param aDeviceLockMode Flag value
        */
        void SetDeviceLockMode( const TBool aDeviceLockMode );
                
        /**
        * Returns RecognitionDialogImpl
        * @return Pointer to RecognitionDialogImpl object
        */
        CVoiceRecognitionDialogImpl* VoiceRecognitionImpl() const;
        
        /**
        * Returns recognition handler builder
        * @return Pointer to recognition handler builder object
        */
        CNssRecognitionHandlerBuilder* RecognitionHandlerBuilder() const;
        
        /**
        * Returns recognition handler
        * @return Pointer to recognition handler object
        */
        MNssRecognitionHandler* RecognitionHandler() const;
        
        /**
        * Returns recognition handler builder
        * @return Pointer to recognition handler builder object
        */
        CNssRecognitionHandlerBuilder* VerificationRecognitionHandlerBuilder() const;
        
        /**
        * Returns recognition handler
        * @return Pointer to recognition handler object
        */
        MNssRecognitionHandler* VerificationRecognitionHandler() const;
        
        /**
        * Returns tone player
        * @return Pointer to tone player object
        */
        CTonePlayer* TonePlayer() const;
        
        /**
        * Returns tts player
        * @return Pointer to tts player object
        */
        CTTSPlayer* TtsPlayer() const;
        
        /**
        * Returns phonebook handler
        * @return Pointer to phonebook handler object
        */
        MVasBasePbkHandler* PbkHandler() const;
                
        /**
        * Returns accessory button monitor
        * @return Pointer to accessory button monitor object
        */
        CPropertyHandler* AccessoryButtonMonitor() const;   
        
        /**
        * Returns tutorial
        * @return Pointer to tutorial object
        */
        CTutorial* Tutorial() const;
        
        /**
        * Returns additional tag list
        * @return Pointer to tag list object
        */
        CArrayPtrFlat<MNssTag>* AdditionalTagList() const;
        
        /**
        * Returns complete tag list
        * @return Pointer to tag list object
        */
        CArrayPtrFlat<MNssTag>* CompleteTagList() const;
        
        /**
        * Returns tag
        * @return Pointer to tag object
        */
        MNssTag* Tag() const;
        
        /**
        * Returns context 
        * @return Pointer to context object
        */
        MNssContext* Context() const;
        
        /**
        * Returns synthesizer mode
        * @return Current synthesizer mode, either 0 or 1
        */
        TInt SynthesizerMode() const;
        
        /**
        * Returns adaptation state
        * @return Current adaptation state, either ETrue or EFalse
        */
        TBool AdaptationEnabled() const;
        
        /**
        * Returns play duration
        * @return Play duration in microseconds
        */
        TTimeIntervalMicroSeconds PlayDuration() const;
        
        /**
        * Returns verification mode
        * @return One of TVuiVerificationMode values
        */
        TInt VerificationMode() const;
        
        /**
        * Returns verification try count
        * @return Number of verification tries left
        */
        TInt VerificationTryCount() const;
        
        /**
        * Returns if device lock mode is enabled
        * @return ETrue if device lock mode is enabled else EFalse
        */
        TBool DeviceLockMode() const;
         
    private:    // Functions
        
        /**
        * C++ default constructor.
        */
        CDataStorage( CVoiceRecognitionDialogImpl* aVoiceRecognitionImpl );
        
        /**
        * Symbian OS 2nd phase constructor.
        */
        virtual void ConstructL();
                
    private:    // Data

        // Main storage owner
        CVoiceRecognitionDialogImpl*        iVoiceRecognitionImpl;
        
        // Recognition handler builder
        CNssRecognitionHandlerBuilder*      iRecognitionHandlerBuilder;
        
        // Recognition handler
        MNssRecognitionHandler*             iRecognitionHandler;
        
        // Recognition handler builder
        CNssRecognitionHandlerBuilder*      iVerificationRecognitionHandlerBuilder;
        
        // Recognition handler
        MNssRecognitionHandler*             iVerificationRecognitionHandler;
               
        // For playing tones
        CTonePlayer*                        iTonePlayer;
        
        // For tts playback
        CTTSPlayer*                         iTtsPlayer;
        
        // For phonebook handling
        MVasBasePbkHandler*                 iPbkHandler;
        
        // For handling accessory buttons
        CPropertyHandler*                   iAccessoryButtonMonitor;
        
        // Tutorial
        CTutorial*                          iTutorial;
        
        // Received tag list includes tags for one contact
        CArrayPtrFlat<MNssTag>*             iAdditionalTagList;
        
        // Received tag list includes recognition results
        CArrayPtrFlat<MNssTag>*             iCompleteTagList;
        
        // One tag from tag list
        MNssTag*                            iTag;
        
        // Received context
        MNssContext*                        iContext;
        
        // TTS mode
        TInt                                iSynthesizerMode;
        
        // Adaptation state
        TBool                               iAdaptationEnabled;
        
        // Time of how long playback will take
        TTimeIntervalMicroSeconds           iPlayDuration;
        
        // Automatic/manual/voice confirmation mode
        TInt                                iVerificationMode;
        
        // Number of verification tries left
        TInt                                iVerificationTryCount;
        
        // Device lock mode flag
        TBool                               iDeviceLockMode;
};

#endif  // VUICDATASTORAGE_H

// End of File
