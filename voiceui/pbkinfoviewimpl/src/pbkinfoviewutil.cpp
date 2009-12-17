/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility functions for PbkInfoView.
*
*/


// INCLUDES
#include <eikenv.h>
#include <bautils.h>
#include <avkon.rsg>

#include "pbkinfoviewutil.h"

// -------------------------------------------------------------------------------
// TPbkInfoViewUtil::Panic
//
// -------------------------------------------------------------------------------
//
void TPbkInfoViewUtil::Panic( TInt aReason )
    {
	_LIT( KPanicCategory,"Info view" );

	User::Panic( KPanicCategory, aReason ); 
    }

// -------------------------------------------------------------------------------
// TPbkInfoViewUtil::StrCopy
//
// String copy with length check.
// @param TDes8& aTarget Copied text will be put to this variable.
// @param TDesC& aSource Includes the text to be copied.
// -------------------------------------------------------------------------------
//
void TPbkInfoViewUtil::StrCopy( TDes8& aTarget, const TDesC& aSource )
    {
	TInt len = aTarget.MaxLength();
    if( len < aSource.Length() ) 
	    {
		aTarget.Copy( aSource.Left( len ) );
		return;
	    }
	aTarget.Copy( aSource );
    }

// -------------------------------------------------------------------------------
// TPbkInfoViewUtil::StrCopy
//
// String copy with length check.
// @param TDes& aTarget Copied text will be put to this variable.
// @param TDesC8& aSource Includes the text to be copied.
// -------------------------------------------------------------------------------
//
void TPbkInfoViewUtil::StrCopy( TDes& aTarget, const TDesC8& aSource )
    {
	TInt len = aTarget.MaxLength();
    if( len < aSource.Length() ) 
	    {
		aTarget.Copy( aSource.Left( len ) );
		return;
	    }
	aTarget.Copy( aSource );
    }

// -------------------------------------------------------------------------------
// TPbkInfoViewUtil::StrCopy
//
// String copy with length check.
// @param TDes& aTarget Copied text will be put to this variable.
// @param TDesC& aSource Includes the text to be copied.
// -------------------------------------------------------------------------------
//
void TPbkInfoViewUtil::StrCopy( TDes& aTarget, const TDesC& aSource )
    {
	TInt len = aTarget.MaxLength();
    if( len < aSource.Length() ) 
	    {
		aTarget.Copy( aSource.Left(len) );
		return;
	    }
	aTarget.Copy( aSource );
    }

// -----------------------------------------------------------------------------
// TPbkInfoViewUtil::AddResFileL
// Adds resource file to control environment.
// @param aFile File location.
// @return TInt Error code.
// -----------------------------------------------------------------------------
//
TInt TPbkInfoViewUtil::AddResFileL( const TDesC& aFile )
	{
	CEikonEnv* env = CEikonEnv::Static();

	TFileName fileName( aFile );

	BaflUtils::NearestLanguageFile( env->FsSession(), fileName );

    return env->AddResourceFileL( fileName );
	}

// End of file
