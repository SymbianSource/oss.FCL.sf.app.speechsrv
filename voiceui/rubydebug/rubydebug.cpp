/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Debugging utilities implementation.
*
*/


// INCLUDE FILES
#include "RubyDebug.h"

#ifdef __RUBY_DEBUG_TRACES_TO_FILE
#include <f32file.h>
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RRubyDebug::PrintToFile
// Method for writing traces to a file.
// -----------------------------------------------------------------------------
//
#ifdef __RUBY_DEBUG_TRACES_TO_FILE

#ifdef _DEBUG // UDEB version:
EXPORT_C void RRubyDebug::PrintToFile( TRefByValue<const TDesC> aFmt, ... )
    {
    _LIT( KRubyLogFileName, "\\Logs\\RubyTrace.log" );

    const TInt KRubyDebugMaxLineLength = 0x80; // rest will be truncated

    const TInt KRubyDebugOpenFileRetries = 100;
    const TInt KRubyDebugOpenFileInterval = 1000;

    const TUint16 KRubyDebugLineSep1 = 0x0d;
    const TUint16 KRubyDebugLineSep2 = 0x0a;

    // Handle variable argument list
    VA_LIST list;
	VA_START( list, aFmt );
	TBuf<KRubyDebugMaxLineLength+2> aBuf;
    TTruncateOverflow overflow;
	aBuf.AppendFormatList( aFmt, list, &overflow );
	if( aBuf.Length() > ( KRubyDebugMaxLineLength - 2 ) )
		{
		aBuf.Delete(aBuf.Length() - 2, 2);
		}
	
    // Add linefeed characters
    aBuf.Append( KRubyDebugLineSep1 );
    aBuf.Append( KRubyDebugLineSep2 );

    RFs fs;
    if ( fs.Connect() == KErrNone )
        {
        RFile file;

        // Open file in an exclusive mode so that only one thread 
        // can acess it simultaneously
        TUint fileMode = EFileWrite | EFileShareExclusive;

        TInt err = file.Open( fs, KRubyLogFileName, fileMode );

        // Create a file if it doesn't exist
        if ( err == KErrNotFound )
            {
            err = file.Create( fs, KRubyLogFileName, fileMode );
            }
        else
            {
            // Error in opening the file
            TInt retryCount = KRubyDebugOpenFileRetries;
            while ( err == KErrInUse && retryCount-- )
                {
                // Some other tread is accessing the file, wait a while...
                User::After( KRubyDebugOpenFileInterval );
                err = file.Open( fs, KRubyLogFileName, fileMode );
                }
            }

        // Check if we have access to a file
        if ( err == KErrNone )
            {
            TInt offset = 0;
            if ( file.Seek( ESeekEnd, offset ) == KErrNone )
                {
                // Append text to the end of file
                TPtr8 ptr8( (TUint8*)aBuf.Ptr(), aBuf.Size(), aBuf.Size() );
                file.Write( ptr8 );
                }
            file.Close();
            }

        fs.Close();
        }
    }

#else // UREL version:
EXPORT_C void RRubyDebug::PrintToFile( TRefByValue<const TDesC> /*aFmt*/, ... )
    {
    }
#endif

#endif // __RUBY_DEBUG_TRACES_TO_FILE

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
