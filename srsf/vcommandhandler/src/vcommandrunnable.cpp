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
* Description:  Implementation of the CVCommandRunnable class
*
*/



#include <vcommandapi.h>
#include "rubydebug.h"

#include <apaid.h>
#include <apgcli.h>
#include <apacmdln.h>

/**
* The version of the streamed data protocol
* It is rather likely, that the format of the data streamed changes in the future.
* Using the protocol version, the code can check if it knows how to parse the 
* input stream
*/
const TInt32 KVCRunnableProtocolVersion = 2;

/**
* @leave KErrOverflow if aArguments is longer, than KMaxVCArgumentLength
* @leave KErrArgument if aAppUid is KNullUidValue
*/
EXPORT_C CVCRunnable* CVCRunnable::NewL( TUid aAppUid, const TDesC8& aArguments )
    {
    CVCRunnable* self = new ( ELeave ) CVCRunnable;
    CleanupStack::PushL( self );
    self->ConstructL( aAppUid, aArguments );
    CleanupStack::Pop( self );
    return self;
    }
    
/**
* Factory function
* @param aExeName Exe-file to start. Can be full or partical name according to 
*        the RProcess::Create rules. Cannot be KNullDesC
* @param aArguments Command line to pass to the application. This object makes
*        a copy of the passed string
* @leave KErrOverflow if aArguments is longer, than KMaxVCArgumentLength or 
*		 aExeName is longer, than KMaxOsName
* @leave KErrUndeflow if aExeName is empty
*/
EXPORT_C CVCRunnable* CVCRunnable::NewL( const TDesC& aExeName, const TDesC8& aArguments )
	{
	CVCRunnable* self = new( ELeave ) CVCRunnable;
	CleanupStack::PushL( self );
	self->ConstructL( aExeName, aArguments);
	CleanupStack::Pop( self );
	return self;
	}
    
/**
* Copy the existing CVRunnable
*/
EXPORT_C CVCRunnable* CVCRunnable::NewL( const CVCRunnable& aOriginal )
	{
	CVCRunnable* self = new (ELeave) CVCRunnable;
	CleanupStack::PushL( self );
	if( aOriginal.Uid() == KNullUid ) 
		{
		self->ConstructL( aOriginal.ExeName(), aOriginal.Arguments() );
		}
	else 
		{
		self->ConstructL( aOriginal.Uid(), aOriginal.Arguments() );
		}
	CleanupStack::Pop( self );
	return self;
	}

/** 
* Internalizes from stream 
* values are read in the same order as ExternalizeL wrote them
*/
EXPORT_C CVCRunnable* CVCRunnable::NewL( RReadStream &aStream )
    {
    CVCRunnable* self = new (ELeave) CVCRunnable;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;    
    }
    
void CVCRunnable::ConstructL( RReadStream &aStream )
	{
	TInt32 ver;
    aStream >> ver;
    __ASSERT_ALWAYS( ver == KVCRunnableProtocolVersion, User::Leave( KErrNotSupported  ) );
    iExeUid = TUid::Uid( aStream.ReadInt32L() );
    iArguments = HBufC8::NewL( aStream.ReadInt32L() );
    TPtr8 pArguments = iArguments->Des();
    aStream >> pArguments;
    TInt exeNameLength = aStream.ReadInt32L();
    if( exeNameLength > 0 )
    	{
    	iExeName = HBufC::NewL( exeNameLength );
    	TPtr pExeName = iExeName->Des();
	    aStream >> pExeName;
    	}
	}
    
void CVCRunnable::ConstructL( TUid aAppUid, const TDesC8& aArguments )
    {
    __ASSERT_ALWAYS( aArguments.Length() <= KMaxVCArgumentLength, User::Leave( KErrOverflow) );
    __ASSERT_ALWAYS( aAppUid != KNullUid, User::Leave( KErrArgument ) );
    iExeUid = TUid::Uid( aAppUid.iUid );
    iArguments = HBufC8::NewL( aArguments.Length() );
    *iArguments = aArguments;
    }

void CVCRunnable::ConstructL( const TDesC& aExeName, const TDesC8& aArguments )
    {
    __ASSERT_ALWAYS( aArguments.Length() <= KMaxVCArgumentLength, User::Leave( KErrOverflow) );
    __ASSERT_ALWAYS( aExeName.Length() <= KMaxFileName, User::Leave( KErrOverflow ) );
    __ASSERT_ALWAYS( aExeName.Length() > 0, User::Leave( KErrUnderflow ) );
    iExeName = HBufC::NewL( aExeName.Length() );
    *iExeName = aExeName;
    iArguments = HBufC8::NewL( aArguments.Length() );
    *iArguments = aArguments;
    }

EXPORT_C CVCRunnable::~CVCRunnable()
    {
    delete iExeName;
    delete iArguments;
    }
    
/** 
 * Saves the object to stream.
 * The current format is as follows:
 * <protocol version: int32><uid: int32; KNullUidValue if no><argument length: int32>
 * <argument descriptor streamed by the default descriptor ExternalizeL>
 * <exe filename length><exe filename descriptor only if filename length is not 0>
 */
EXPORT_C void CVCRunnable::ExternalizeL( RWriteStream &aStream ) const
    {
    aStream << KVCRunnableProtocolVersion;
    aStream << iExeUid.iUid;
    aStream << static_cast<TInt32>( iArguments->Length() );
    aStream << *iArguments;
    if( iExeName )
    	{
    	aStream << static_cast<TInt32>( iExeName->Length() );
    	aStream << *iExeName;
    	}
    else 
    	{
    	aStream << static_cast<TInt32>( 0 );
    	}
    }
    
EXPORT_C TBool CVCRunnable::operator==( const CVCRunnable& aRunnable ) const
    {
    return ( 
            ( Uid() == aRunnable.Uid()) && 
            ( Arguments() == aRunnable.Arguments() ) &&
            ( ExeName() == aRunnable.ExeName() )
           );
    }
    

/**
* @return The uid of the application to be started
*/
EXPORT_C const TUid CVCRunnable::Uid() const
    {
    return iExeUid;
    }
    
/**
* @return The filename of the application to be started. KNullDesC if there is none
*/
EXPORT_C const TDesC& CVCRunnable::ExeName() const
	{
	if( iExeName )
		{
		return *iExeName;
		}
	else 
		{
		return KNullDesC;
		}
	}

/**
* The arguments to be passed to the application
*/
EXPORT_C const TDesC8& CVCRunnable::Arguments() const
    {
    return *iArguments;
    }
    
/** 
* Run the command. In case of aAppUid specified during the construction
* application is started via the RApaLsSession::StartApp, otherwise
* via the RProcess::Create
* @leave System-wide error code. In particular KErrNotFound if there is
*        no such app in the system
*/
EXPORT_C void CVCRunnable::ExecuteL() const
	{
	RUBY_DEBUG_BLOCK( "CVCRunnable::ExecuteL" );
	if( iExeName ) 
		{
		RProcess process;
		CleanupClosePushL( process );
		// Convert arguments into system_number_of_bits-bit descriptor 
		// (needed for exe, not needed for GUI apps)
		HBufC* arguments = HBufC::NewLC( iArguments->Length() );
		arguments->Des().Copy( *iArguments );
		
		User::LeaveIfError( process.Create( *iExeName, *arguments) );
		process.Resume();
		
		CleanupStack::PopAndDestroy( arguments );
		CleanupStack::PopAndDestroy( &process );
		}
	else 
		{
		TApaAppInfo appInfo;
		RApaLsSession apaLsSession;
		CleanupClosePushL( apaLsSession );
		User::LeaveIfError( apaLsSession.Connect() );
		User::LeaveIfError( apaLsSession.GetAppInfo( appInfo, iExeUid ) );
		
		CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();
		apaCommandLine->SetExecutableNameL( appInfo.iFullName );
        apaCommandLine->SetTailEndL( *iArguments );
        User::LeaveIfError( apaLsSession.StartApp( *apaCommandLine ) );

		CleanupStack::PopAndDestroy( apaCommandLine );
		CleanupStack::PopAndDestroy( &apaLsSession );
		}
	}

//End of file
