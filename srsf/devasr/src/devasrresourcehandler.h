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
* Description:  This class reads the resource file and provides access to the
*               information read from the resource file.
*
*/


#ifndef CDEVASRRESOURCEHANDLER_H
#define CDEVASRRESOURCEHANDLER_H

// CLASS DECLARATION

/**
*  This class encapsulates resource file handling functions
*
*  @lib DevASR.lib
*  @since 2.0
*/
class CDevASRResourceHandler : public CBase
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDevASRResourceHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CDevASRResourceHandler();

    private:

        /**
        * C++ default constructor.
        */
        CDevASRResourceHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Reads the data from the resource file.
        * @since 2.0
        * @param	-
        * @return	-
        */
		void ReadResourceFileL();

	public: // Data

		// Recording sampling rate
		TInt iSamplingRate;
		// Number of bits per sample
		TInt iBitsPerSample;
		// Number of channels
		TInt iMicrophoneGain;
		// percentage of max speaker volume
		TInt iSpeakerVolume;
		// Number of frames to silence
		TInt iSilenceFrames;
		// Number of frames to pad at the end of end-pointed buffer
		TInt iPadEndFrames;
		// Number of frames to pad at the start of end-pointed buffer
		TInt iPadStartFrames;
        // Size of playback/record buffer in bytes
        TInt iBufferSize;
        // Number of buffers gathered in WINS
        TInt iBuffersInWins;
        // Number of buffers gathered in THUMB
        TInt iBuffersInThumb;
	};


#endif // CDevASRResourceHandler_H

// End of File
