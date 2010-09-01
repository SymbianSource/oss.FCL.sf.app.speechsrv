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
* Description:  Implementation of the CVFolderInfo class
*
*/


#include <vcommandapi.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <gulicon.h>
#include <mifconvdefs.h>
#include "rubydebug.h"

/**
 * File name to read icons from if icon file name is not specified
 */
_LIT( KDefaultIconFile, "Z:\\resource\\apps\\vcommand.mif" );
const TInt KDefaultIconIndex = KMifIdFirst;  // first image in the file

/**
* The version of the streamed data protocol
* It is rather likely, that the format of the data streamed changes in the future.
* Using the protocol version, the code can check if it knows how to parse the 
* input stream
*/
const TInt32 KVCFolderInfoProtocolVersion = 2;

static const TAknsItemID* const KIconId[] = 
    {
    &KAknsIIDQgnPropFolderVcGeneral,
    &KAknsIIDQgnPropFolderVcTools,
    &KAknsIIDQgnPropFolderVcOrganiser,
    &KAknsIIDQgnPropFolderVcMessage,
    &KAknsIIDQgnPropFolderVcMedia,
    &KAknsIIDQgnPropFolderVcProfiles
    };

/**
 * This object does not take an ownership of the passed descriptors, 
 * but makes own copies. 
 * 
 * @param aTitle Folder title. It is shown at the top of the screen,
 *        when the folder is opened in the VCommand application
 * @param aListedName Version of a title shown when the folder is
 *        is displayed in the list of VCommands in the VCommand app
 * @param aHelpTopicId Topic to open when help is requested for the 
 *        given folder
 * @param aIconIndex Index of the folder icon in the folder icons mbm file
 *        each index correspods to two images - the actual icon and its mask
 * @param aIconFile Mbm file where the icons are obtained from. If KNullDesC, 
 * 		  the default icon file is used
 */
EXPORT_C CVCFolderInfo* CVCFolderInfo::NewL( const TDesC& aTitle,
        const TDesC& aListedName, TUint32 aHelpTopicId, TUint aIconIndex,
                const TDesC& aIconFile )
    {
    CVCFolderInfo* self = new (ELeave) CVCFolderInfo( aHelpTopicId, aIconIndex );
    CleanupStack::PushL( self );
    self->ConstructL( aTitle, aListedName, aIconFile );
    CleanupStack::Pop( self );
    return self;
    }
    
/**
* Copy the existing CVCFolderInfo
*/
EXPORT_C CVCFolderInfo* CVCFolderInfo::NewL( const CVCFolderInfo& aOriginal )
    {
    CVCFolderInfo* self = new (ELeave) CVCFolderInfo( aOriginal.HelpTopicId(), 
                                                      aOriginal.IconIndex() );
    CleanupStack::PushL( self );
    self->ConstructL( aOriginal.Title(), aOriginal.ListedName(), aOriginal.IconFile() );
    CleanupStack::Pop( self );
    return self;
    }
    
/**
 * Constructs the folder information from stream
 * @leave KErrNotSupported if the stream data format is unsupported
 */
 EXPORT_C CVCFolderInfo* CVCFolderInfo::NewL( RReadStream &aStream )
    {
    CVCFolderInfo* self = new (ELeave) CVCFolderInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }

CVCFolderInfo::CVCFolderInfo( TUint32 aHelpTopicId, TUint aIconIndex ) :
    iHelpTopicId( aHelpTopicId ), iIconIndex (aIconIndex )
    {
    // empty
    }
        
void CVCFolderInfo::ConstructL( RReadStream &aStream )
	{
	/**
	* @todo extract the repeated descriptor internalization code into
	*       a separate method
	*/
	TInt32 ver;
    aStream >> ver;
    __ASSERT_ALWAYS( ver == KVCFolderInfoProtocolVersion, User::Leave( KErrNotSupported  ) );
    iTitle = HBufC::NewL( aStream.ReadInt32L() );
    TPtr pTitle = iTitle->Des();
    aStream >> pTitle;
    iListedName = HBufC::NewL( aStream.ReadInt32L() );
    TPtr pListedName = iListedName->Des();
    aStream >> pListedName;
    iHelpTopicId = aStream.ReadUint32L();
    iIconFile = HBufC::NewL( aStream.ReadInt32L() );
    TPtr pIconFile = iIconFile->Des();
    aStream >> pIconFile;
    iIconIndex = aStream.ReadUint32L();
	}
    
    
/** 
* Second-phase constructor 
*/
void CVCFolderInfo::ConstructL( const TDesC& aTitle, const TDesC& aListedName,
                                const TDesC& aIconFile )
    {
    RUBY_DEBUG_BLOCKL( "" );
    iTitle = HBufC::NewL( aTitle.Length() );
    *iTitle = aTitle;
    iListedName = HBufC::NewL( aListedName.Length() );
    *iListedName = aListedName;    
    iIconFile = HBufC::NewL( aIconFile.Length() );
    *iIconFile = aIconFile;
    }
    
EXPORT_C CVCFolderInfo::~CVCFolderInfo() 
    {
    delete iListedName;
    delete iTitle;
    delete iIconFile;
    }
    
/** 
 * Saves the command to stream.
 * Descriptor components are saved as <length><descriptor> pairs, where 
 * <length> is TInt32 and <descriptor> is the default descriptor represetation
 * TBools are saved as TInt32 either
 */
EXPORT_C void CVCFolderInfo::ExternalizeL( RWriteStream &aStream ) const
    {
    aStream << KVCFolderInfoProtocolVersion;
    aStream << static_cast<TInt32>( iTitle->Length() );
    aStream << *iTitle;
    aStream << static_cast<TInt32>( iListedName->Length() );
    aStream << *iListedName;
    aStream << iHelpTopicId;
    aStream << static_cast<TInt32> ( iIconFile->Length() );
    aStream << *iIconFile;
    // Mandate saving index as Uint32 to preserve the string format
    aStream << static_cast<TUint32> ( iIconIndex );
    }
    

EXPORT_C const TDesC& CVCFolderInfo::Title() const 
    {
    return *iTitle;
    }

EXPORT_C const TDesC& CVCFolderInfo::ListedName() const 
    {
    return *iListedName;
    }

EXPORT_C TUint32 CVCFolderInfo::HelpTopicId() const 
    {
    return iHelpTopicId;
    }
    
EXPORT_C TBool CVCFolderInfo::operator==( const CVCFolderInfo& aFolderInfo ) const
    {
    return (
            ( iHelpTopicId == aFolderInfo.HelpTopicId() ) &&
            ( *iTitle == aFolderInfo.Title()            ) &&
            ( *iListedName == aFolderInfo.ListedName()  ) &&
            ( *iIconFile == aFolderInfo.IconFile()      ) &&
            ( iIconIndex == aFolderInfo.IconIndex()     ) 
           );
    }
    
EXPORT_C TDesC& CVCFolderInfo::IconFile() const
	{
	return *iIconFile;
	}    
    
EXPORT_C TUint CVCFolderInfo::IconIndex() const
	{
	return iIconIndex;
	}

/**
* Creates an icon to represent this folder. Works only if CEikonEnv is available
* @return Icon for the folder
*/
EXPORT_C CGulIcon* CVCFolderInfo::IconLC() const
	{
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    const TInt KBitmapsPerIcon = 2;
    
    // Every "index" corrsponds to two bitmaps - actual icon and its mask
    TInt iconIndex = KMifIdFirst + IconIndex()*KBitmapsPerIcon;
    TInt maskIndex = iconIndex + 1;
    TDesC& iconFile = IconFile();
    if ( iconFile == KNullDesC )
        {
        iconFile = KDefaultIconFile;
        }
    
    TRAPD( err,    
	    AknsUtils::CreateIconLC( skin, *KIconId[ IconIndex() ],
       	                     bitmap, mask, iconFile, iconIndex, 
       	                     maskIndex );
        CleanupStack::Pop( 2 );  // mask, bitmap
         )
    if( err != KErrNone )
        {
        RUBY_DEBUG1( "CreateIconLC failed with %d. Loading default icon", err );
        AknsUtils::CreateIconLC( skin, *KIconId[ IconIndex() ],
                             bitmap, mask, KDefaultIconFile, KDefaultIconIndex, 
                             KDefaultIconIndex + 1 );
        CleanupStack::Pop( 2 );  // mask, bitmap
        }
 
    CleanupStack::PushL( mask );
    CleanupStack::PushL( bitmap );
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop( bitmap );  
    CleanupStack::Pop( mask );  

    CleanupStack::PushL( icon );
    return icon;
	}
	    
//End of file
