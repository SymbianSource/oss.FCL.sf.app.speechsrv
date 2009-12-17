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
* Description:  Implementation of the CVCommandUi class
*
*/


#include <vcommandapi.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include "rubydebug.h"

/**
* The version of the streamed data protocol
* It is rather likely, that the format of the data streamed changes in the future.
* Using the protocol version, the code can check if it knows how to parse the 
* input stream
*/
const TInt32 KVCCommandUiProtocolVersion = 5;


/**
* This object does not take an ownership of the passed descriptors, 
* but makes own copies. 
* 
* @param aWrittenText Text to display
* @param aTooltip Second line of the text if ant
* @param aFolderInfo Folder-related details
* @param aModifiable If user can modify the command
* @leave KErrOverflow if descriptors are too big
*/
EXPORT_C CVCCommandUi* CVCCommandUi::NewL( const TDesC& aWrittenText, 
                                  const CVCFolderInfo& aFolderInfo, 
                                  TBool aModifiable, const TDesC& aTooltip,
                                  const TUid& aIconUid, const TDesC& aUserText, 
                                  TBool aConfirmationNeeded ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCCommandUi::NewL" );
    CVCCommandUi* self = new (ELeave) CVCCommandUi;
    CleanupStack::PushL( self );
    self->ConstructL( aWrittenText, aFolderInfo, aModifiable, aTooltip, aIconUid, 
                      aUserText, aConfirmationNeeded );
    CleanupStack::Pop( self );
    return self;
    }
    
/**
* Copy the existing CVCCommandUi
*/
EXPORT_C CVCCommandUi* CVCCommandUi::NewL( const CVCCommandUi& aOriginal )
    {
    CVCCommandUi* self = new (ELeave) CVCCommandUi;
    CleanupStack::PushL( self );
    self->ConstructL( aOriginal.WrittenText(), aOriginal.FolderInfo(), 
                      aOriginal.Modifiable(), aOriginal.Tooltip(), 
                      aOriginal.IconUid(), aOriginal.UserText(),
                      aOriginal.ConfirmationNeeded() );
    CleanupStack::Pop( self );
    return self;
    }
    
/** 
* Internalizes the command from stream 
* values are read in the same order as ExternalizeL wrote them
*/
EXPORT_C CVCCommandUi* CVCCommandUi::NewL( RReadStream &aStream )
    {
    CVCCommandUi* self = new (ELeave) CVCCommandUi;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }
    
    
void CVCCommandUi::ConstructL( RReadStream &aStream )
	{
	/**
	* @todo extract the repeated descriptor internalization code into
	*       a separate method
	*/
	TInt32 ver;
    aStream >> ver;
    __ASSERT_ALWAYS( ver == KVCCommandUiProtocolVersion, User::Leave( KErrNotSupported  ) );
    iWrittenText = HBufC::NewL( aStream.ReadInt32L() );
    TPtr pWrittenText = iWrittenText->Des();
    aStream >> pWrittenText;
    iFolderInfo = CVCFolderInfo::NewL( aStream );
    TInt32 intModifiable;
    aStream >> intModifiable;
    iUserCanModify = static_cast<TBool>( intModifiable );
    iTooltip = HBufC::NewL( aStream.ReadInt32L() );
    TPtr pTooltip = iTooltip->Des();
    aStream >> pTooltip;
    iIconUid = TUid::Uid( aStream.ReadInt32L() );
    iUserText = HBufC::NewL( aStream.ReadInt32L() );
    TPtr pUserText = iUserText->Des();
    aStream >> pUserText;
    TInt32 intConfirmationNeeded;
    aStream >> intConfirmationNeeded;
    iConfirmationNeeded = static_cast<TBool>( intConfirmationNeeded );
	}
    
    
/** 
* Second-phase constructor 
* @see NewL for guarding conditions
*/    
void CVCCommandUi::ConstructL( const TDesC& aWrittenText, const CVCFolderInfo& aFolderInfo,
                               TBool aModifiable, const TDesC& aTooltip, const TUid& aIconUid,
                               const TDesC& aUserText, TBool aConfirmationNeeded ) 
    {
    RUBY_DEBUG_BLOCKL( "CVCCommandUi::ConstructL" );
    iWrittenText = HBufC::NewL( aWrittenText.Length() );
    *iWrittenText = aWrittenText;
    iFolderInfo = CVCFolderInfo::NewL( aFolderInfo );
    iTooltip = HBufC::NewL( aTooltip.Length() );
    *iTooltip = aTooltip;
    iUserCanModify = aModifiable;
    iIconUid = TUid::Uid( aIconUid.iUid );
    iUserText = HBufC::NewL( aUserText.Length() );
    *iUserText = aUserText;
    iConfirmationNeeded = aConfirmationNeeded;
    }
    
EXPORT_C CVCCommandUi::~CVCCommandUi() 
    {
    delete iUserText;
    delete iWrittenText;
    delete iFolderInfo;
    delete iTooltip;
    }
    
/** 
 * Saves the command to stream.
 * Descriptor components are saved as <length><descriptor> pairs, where 
 * <length> is TInt32 and <descriptor> is the default descriptor represetation
 * TBools are saved as TInt32 either
 */
EXPORT_C void CVCCommandUi::ExternalizeL( RWriteStream &aStream ) const
    {
    aStream << KVCCommandUiProtocolVersion;
    aStream << static_cast<TInt32>( iWrittenText->Length() );
    aStream << *iWrittenText;
    aStream << *iFolderInfo;
    aStream << static_cast<TInt32> ( iUserCanModify );
    aStream << static_cast<TInt32>( iTooltip->Length() );
    aStream << *iTooltip;
    aStream << iIconUid.iUid;
    aStream << static_cast<TInt32>( iUserText->Length() );
    aStream << *iUserText;
    aStream << static_cast<TInt32> ( iConfirmationNeeded );
    }
    

EXPORT_C const TDesC& CVCCommandUi::WrittenText() const 
    {
    return *iWrittenText;
    }

EXPORT_C const TDesC& CVCCommandUi::UserText() const 
    {
    return *iUserText;
    }

EXPORT_C const CVCFolderInfo& CVCCommandUi::FolderInfo() const 
    {
    return *iFolderInfo;
    }

EXPORT_C const TDesC& CVCCommandUi::Tooltip() const 
    {
    return *iTooltip;
    }
    
EXPORT_C TBool CVCCommandUi::Modifiable() const
    {
    return iUserCanModify;
    }

EXPORT_C TBool CVCCommandUi::ConfirmationNeeded() const
    {
    return iConfirmationNeeded;
    }
    
EXPORT_C const TUid& CVCCommandUi::IconUid() const 
    {
    return iIconUid;
    }
    
EXPORT_C TBool CVCCommandUi::operator==( const CVCCommandUi& aCommandUi ) const
    {
    return (
            ( *iWrittenText == aCommandUi.WrittenText() ) &&
            ( *iFolderInfo == aCommandUi.FolderInfo() ) &&
            ( *iTooltip == aCommandUi.Tooltip() ) &&
            ( iUserCanModify == aCommandUi.Modifiable() ) &&
            ( iIconUid == aCommandUi.IconUid() ) &&
            ( *iUserText == aCommandUi.UserText() ) &&
            ( iConfirmationNeeded == aCommandUi.ConfirmationNeeded() )
           );
    }
    
EXPORT_C CGulIcon* CVCCommandUi::IconLC() const
	{
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

	AknsUtils::CreateAppIconLC( skin, IconUid(), EAknsAppIconTypeList, bitmap, mask );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop( 2 ); // mask, bitmap

    CleanupStack::PushL( icon );
    return icon;
	}

/**
 * Tells if all non user-changeable parts of aCommandUi are equal to this object's
 * @return ETrue if equal, EFalse otherwise
 */
EXPORT_C TBool CVCCommandUi::EqualNonUserChangeableData( const CVCCommandUi& aCommandUi ) const
    {
    if( WrittenText() != aCommandUi.WrittenText() ) return EFalse;
    if( !(FolderInfo() == aCommandUi.FolderInfo()) ) return EFalse;    
    if( Modifiable() != aCommandUi.Modifiable() ) return EFalse;    
    if( ConfirmationNeeded() != aCommandUi.ConfirmationNeeded() ) return EFalse;    
    if( Tooltip() != aCommandUi.Tooltip() ) return EFalse;    
    if( IconUid() != aCommandUi.IconUid() ) return EFalse;    
    return ETrue;    
    }
    
//End of file
