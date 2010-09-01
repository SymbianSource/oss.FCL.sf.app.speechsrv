/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Algorithms for Speech Synthesis
*
*/





#ifndef TTSHWDEVICE_H
#define TTSHWDEVICE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <nssttscommon.h>
#include <asrshwdevice.h>

// FORWARD DECLARATIONS
class CTtsAlgorithm;

// CLASS DECLARATION
/**
*  Class to define callback functions
*
*  @lib asrsttshwdevice.lib
*  @since 2.8
*/
class MTtsHwDeviceObserver
    {
    public: // New functions

		/**
        * Called by the HW device when the synthesis has been completed.
        * @since 2.8
        * @param aStatus Error code, KErrNone if success
        * @return none
        */
		virtual void MthdTtsCompleted( TInt aStatus ) = 0;

		/**
        * Invoked by TTS HW Device when it needs a configuration data package.
        * @since 2.8
        * @param aPackageType Type identifier. 
        * @param aPackageID Identifier of package
        * @param aStartPosition Start index in bytes
        * @param aEndPosition End index in bytes. If the position is bigger 
        *         than the size of data package, the end part will be returned.
        * @return Data package in a buffer
        */
        virtual HBufC8* MthdoConfigurationData( TUint32 aPackageType, 
                                                TUint32 aPackageID, 
                                                TUint32 aStartPosition, 
                                                TUint32 aEndPosition) = 0;
		/**
        * Called by the HW device when custom command implementation wants to notify a client.
        * @since 2.8
        * @param aEvent Event code aStatus Error code, KErrNone if success.
        * @param aParameter Optional parameter 
        * @return none
        */
		virtual void MthdCustomEvent( TInt aEvent, 
                                      TInt aStatus, 
                                      const TDesC8& aParameter = KNullDesC8 ) = 0;

        /**
        * Called by the HW device when the HW device has synthesized a new audio buffer.
        * @since 2.8
        * @param aBuffer Synthesized audio buffer.
        * @return none
        */
        virtual void MthdProcessBuffer( const TDesC8& aBuffer ) = 0;

	};



/**
*  Class to provide speech synthesis
*
*  @lib asrsttshwdevice.lib
*  @since 2.8
*/
class CTtsHwDevice : public CASRSHwDevice
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aObserver Observer which implements callback functions.
        * @param aSamplingRate Sampling rate for output audio
        * @param aBufferSize Buffer size for output audio
        */
        IMPORT_C static CTtsHwDevice* NewL( MTtsHwDeviceObserver& aObserver,
                                            TUint aSamplingRate, 
                                            TUint aBufferSize );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CTtsHwDevice();

    public: // New functions

        /**
        * Adds new style to the collection. If equal style exists already, new style is not added.
        * @since 2.8
        * @param aStyle Style to be added.
        * @return Identifier of added style. If equal style exists already, identifier to the old style will be returned.
        */
        IMPORT_C TTtsStyleID AddStyleL( const TTtsStyle& aStyle );

        /**
        * Analyses stucture of text and adds style information.
        * @since 2.8
        * @param aText Updates text object by dividing it into segments with new styles.
        * @return none
        */
        IMPORT_C void AnalyzeProsodyL( CTtsParsedText& aText );

        /**
        * Analyses stucture of text by finding paragraphs and sentences.
        * @since 2.8
        * @param aText Updates text object by segmenting it into paragraphs and sentences.
        * @return none
        */
        IMPORT_C void AnalyzeStructureL( CTtsParsedText& aText );

        /**
        * Notifies that the buffer has been processed
        * @since 2.8
        * @param aBuffer The buffer which has been processed
        * @return none
        */
        IMPORT_C void BufferProcessed( const TDesC8& aBuffer );

        /**
        * Deletes style.
        * @since 2.8
        * @param aStyleID Identifier of style to be deleted.
        * @return none
        */
        IMPORT_C void DeleteStyleL( TTtsStyleID aStyleID );

        /**
        * Gives current position of synthesizing.
        * @since 2.8
        * @param aTime Time of the synthesizing position.
        * @return none
        */
        IMPORT_C void GetPositionL( TTimeIntervalMicroSeconds& aTime );

        /**
        * Gives current position of synthesizing.
        * @since 2.8
        * @param aSegment The segment being synthesizing
        * @param aWordIndex The index of word being synthesizing
        * @return none
        */
        IMPORT_C void GetPositionL( TTtsSegment& aSegment, 
                                    TInt& aWordIndex );

        /**
        * Ask if a language is supported or not.
        * @since 2.8
        * @param aLanguage Synthesizing language.
        * @return ETrue language is supported otherwise EFalse
        */
        IMPORT_C TBool IsLanguageSupported( TLanguage aLanguage );

        /**
        * Gives number of styles.
        * @since 2.8
        * @param none
        * @return Number of styles.
        */
        IMPORT_C TUint16 NumberOfStyles() const;

        /**
        * Pauses synthesizing. 
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void Pause();

        /**
        * Prepares for synthesizing.
        * @since 2.8
        * @param aStream Stream to be synthesized.
        * @return none
        */
        IMPORT_C void PrimeSynthesisL( MTtsSegmentStream& aStream );

        /**
        * Prepares for synthesizing. 
        * @since 2.8
        * @param aText Text containing segments to be synthesized.
        * @return none
        */
        IMPORT_C void PrimeSynthesisL( CTtsParsedText& aText );

        /**
        * Sets new position for synthesizing.
        * @since 2.8
        * @param aTime Time of the synthesizing postion.
        * @return none
        */
        IMPORT_C void SetPositionL( const TTimeIntervalMicroSeconds& aTime );

        /**
        * Sets new position for synthesizing.
        * @since 2.8
        * @param aSegment The segment being synthesizing
        * @param aWordIndex The index of word being synthesizing
        * @return none
        */
        IMPORT_C void SetPositionL( const TTtsSegment& aSegment, 
                                    TInt aWordIndex );

        /**
        * Stops synthesizing and releases resources of synthesizing. 
        * The method does nothing if already stopped.
        * @since 2.8
        * @param 
        * @return none
        */
        IMPORT_C void Stop();

        /**
        * Receives style information
        * @since 2.8
        * @param aStyleID Style identifier
        * @return Style
        */
        IMPORT_C TTtsStyle& StyleL( TTtsStyleID aStyleID );

        /**
        * Receives style information
        * @since 2.8
        * @param aIndex Index to style: 0..NumberOfStyles()
        * @return Style
        */
        IMPORT_C TTtsStyle& StyleL( TUint16 aIndex );

        /**
        * Starts waveform production. The synthesized audio buffers are send back 
        * to client using MthdProcessBuffer().
        * @since 2.8
        * @param none
        * @return none
        */
        IMPORT_C void SynthesizeL();
       
    private:

        /**
        * C++ default constructor.
        */
        CTtsHwDevice();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MTtsHwDeviceObserver& aObserver,
                         TUint aSamplingRate, 
                         TUint aBufferSize );

    private: // Data

		// Actual algorithm where all processing happens
		CTtsAlgorithm* iAlgorithm;

		// Reserved pointer for future extension
        TAny* iReserved;

    };

#endif // TTSHWDEVICE_H   
            
// End of File
