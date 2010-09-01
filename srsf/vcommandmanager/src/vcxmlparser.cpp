/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Parser for Voice Commands XML files
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <featmgr.h>
#include <parser.h>
#include <utf.h>
#include "vcxmlparser.h"
#include "vcresource.h"
#include "rubydebug.h"

// CONSTANTS
// XML Tags
_LIT8( KXmlDocumentTag, "nssvcommands" );
_LIT8( KXmlFolderTag, "vcommandfolder" );
_LIT8( KXmlVCommandTag, "vcommand" );
_LIT8( KXmlSpokenTag, "spoken" ); 
_LIT8( KXmlWrittenTag, "written" ); 
_LIT8( KXmlExecutesTag, "executes" );
_LIT8( KXmlCmdlineparamsTag, "cmdlineparams" );
_LIT8( KXmlIconTag, "icon" );
_LIT8( KXmlTooltipTag, "tooltip" );

// XML attributes
_LIT8( KXmlVersionAttr, "version" );
_LIT8( KXmlLocFileAttr, "localizationfile" );
_LIT8( KXmlLocAttr, "locindex" );
_LIT8( KXmlFolderTitleLocAttr, "titlelocindex" );
_LIT8( KXmlFolderIconFileAttr, "iconfile" );
_LIT8( KXmlFolderIconIndexAttr, "iconindex" );
_LIT8( KXmlTtsAttr, "tts" );
_LIT8( KXmlModifiableAttr, "modifiable" );
_LIT8( KXmlUidAttr, "uid" );
_LIT8( KXmlExeAttr, "exe" );

// XML attribute values
_LIT8( KXmlVersionValue, "1.0" );
_LIT8( KXmlTrueValue, "true" );
_LIT8( KXmlFalseValue, "false" );

// XML file MIME type
_LIT8( KMimeType, "text/xml" );

static const TInt KNoTags = 0;
static const TInt KLastTag = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcXmlParser::CVcXmlParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcXmlParser::CVcXmlParser( RFs& aRFs, MVcXmlParserObserver& aObserver )
 : iObserver( aObserver ), iRFs( aRFs )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVcXmlParser::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CVcXmlParser::ConstructL" );
    
    ResetVCommandFolder();
    
    using namespace Xml;
    
    iParser = CParser::NewL( KMimeType, *this );
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVcXmlParser* CVcXmlParser::NewL( RFs& aRFs, MVcXmlParserObserver& aObserver )
    {
    CVcXmlParser* self = new( ELeave ) CVcXmlParser( aRFs, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::~CVcXmlParser
// Destructor.
// -----------------------------------------------------------------------------
//
CVcXmlParser::~CVcXmlParser()
    {
    iXmlNesting.Close();
    ResetVCommandInfo();
    delete iResource;
    delete iFolder;
    delete iFolderTitle;
    delete iFolderIconFile;
    delete iAttributeValue;
    delete iParser;
    delete iParameters;
    delete iParameters8;
    delete iAppName;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ParseFileL
// Parses a specific file.
// -----------------------------------------------------------------------------
//
void CVcXmlParser::ParseFileL( const TFileName& aFileName )
    {
    RUBY_DEBUG_BLOCK( "CVcXmlParser::ParseFileL" );
    
    iParser->ParseBeginL();
    
    ParseL( *iParser, iRFs, aFileName );
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnStartDocumentL
// Callback from XML parser when beginning of document has been found
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnStartDocumentL( const Xml::RDocumentParameters& /*aDocParam*/, 
                                     TInt aErrorCode )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::OnStartDocumentL" );
    
    delete iResource;
    iResource = NULL;
    
    if ( ( iXmlNesting.Count() == KNoTags ) && ( aErrorCode == KErrNone ) )
        {
        iXmlNesting.Append( EXmlStarted );
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnEndDocumentL
// Callback from XML parser when end of document has been reached
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnEndDocumentL( TInt aErrorCode )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::OnEndDocumentL" );
    
    if ( ( iXmlNesting.Count() == KLastTag ) && ( aErrorCode == KErrNone ) )
        {
        iXmlNesting.Reset();
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnStartElementL
// Callback from XML parser when new element has been found
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnStartElementL( const Xml::RTagInfo& aElement, 
	                                const Xml::RAttributeArray& aAttributes, 
	                                TInt aErrorCode )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::OnStartElementL" );
    __ASSERT_ALWAYS( iXmlNesting.Count() > 0, User::Leave( KErrCorrupt ) );    
    
    User::LeaveIfError( aErrorCode );

    HBufC8* name = ToLowercaseLC( aElement.LocalName().DesC() );

    // Check what was the previous tag
    switch ( iXmlNesting[ iXmlNesting.Count() - 1 ] )
        {
        case EXmlStarted:
            {
            if ( !ResolveDocumentTagL( name->Des(), aAttributes ) )
                {
                User::Leave( KErrNotFound );
                }
            break;
            }
            
        case EXmlDocument:
            {
            if ( !ResolveFolderTagL( name->Des(), aAttributes ) )
                {
                if ( !ResolveVCommandTagL( name->Des(), aAttributes ) )
                    {
                    RUBY_DEBUG0( "VC XML ERROR: <vcommandfolder> or <vcommand> exptected but not found" );
                    User::Leave( KErrNotFound );
                    }
                }
            break;
            }
        
        case EXmlFolder:
            {
            if ( !ResolveVCommandTagL( name->Des(), aAttributes ) )
                {
                RUBY_DEBUG0( "VC XML ERROR: <vcommand> exptected but not found" );
                User::Leave( KErrNotFound );
                }

            // New voice command starts
            ResetVCommandInfo();
             
            break;
            }
        
        case EXmlVCommand:
            {
            if ( !ResolveSpokenTagL( name->Des(), aAttributes ) )
                {
                if ( !ResolveWrittenTagL( name->Des(), aAttributes ) )
                    {
                    if ( !ResolveExecutesTagL( name->Des(), aAttributes ) )
                        {
                        if ( !ResolveCmdlineparamsTag( name->Des(), aAttributes ) )
                            {
                            if ( !ResolveIconTagL( name->Des(), aAttributes ) )
                                {
                                if ( !ResolveTooltipTagL( name->Des(), aAttributes ) )
                                    {
                                    RUBY_DEBUG0( "VC XML ERROR: <spoken> <written> <executes> or <cmdlineparams> exptected but not found" );   
                                    User::Leave( KErrNotFound );
                                    }
                                }
                            }
                        }
                    }
                }
            break;
            }
            
        default:
            {
            
            RUBY_DEBUG0( "VC XML ERROR: Unexpected starting tag found" );
            User::Leave( KErrNotFound );
            
            break;
            }

        }
        
    CleanupStack::PopAndDestroy( name );
 
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnEndElementL
// Callback from XML parser when ending tag has been found
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnEndElementL( const Xml::RTagInfo& aElement, 
                                  TInt aErrorCode )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::OnEndElementL" );
    __ASSERT_ALWAYS( iXmlNesting.Count() > 0, User::Leave( KErrCorrupt ) );
    
    User::LeaveIfError( aErrorCode );
    
    HBufC8* name = ToLowercaseLC( aElement.LocalName().DesC() );
    
    switch ( iXmlNesting[ iXmlNesting.Count() - 1 ] )
        {
        case EXmlDocument:
            {
            PopFromStackIfFoundL( name->Des(), KXmlDocumentTag );
            break;
            }
        
        case EXmlFolder:
            {
            PopFromStackIfFoundL( name->Des(), KXmlFolderTag );
            ResetVCommandFolder();
            break;
            }
        
        case EXmlVCommand:
            {
            PopFromStackIfFoundL( name->Des(), KXmlVCommandTag );
            CheckVoiceCommmandData();
            break;
            }
            
         case EXmlSpoken:
            {
            PopFromStackIfFoundL( name->Des(), KXmlSpokenTag );
            break;
            }
         
         case EXmlWritten:
            {
            PopFromStackIfFoundL( name->Des(), KXmlWrittenTag );
            break;
            }
        
         case EXmlExecutes:
            {
            PopFromStackIfFoundL( name->Des(), KXmlExecutesTag );
            break;
            }
         
         case EXmlCmdlineparams:
            {
            PopFromStackIfFoundL( name->Des(), KXmlCmdlineparamsTag );   
                
            // Convert content of <cmdlineparams> into unicode
            // First trim out unnecessary white space
            TPtr8 des8( iParameters8->Des() );
            des8.Trim();
                
            // Delete previous unicode buffer and create new
            delete iParameters;
            iParameters = NULL;
            iParameters = CnvUtfConverter::ConvertToUnicodeFromUtf8L( des8 );
                
            break;
            }

         case EXmlIcon:
            {
            PopFromStackIfFoundL( name->Des(), KXmlIconTag );
            break;
            }

         case EXmlTooltip:
            {
            PopFromStackIfFoundL( name->Des(), KXmlTooltipTag );
            break;
            }
           
        default:
            {
            RUBY_DEBUG0( "VC XML ERROR: Ending tag found when not expected" );
            User::Leave( KErrGeneral );
            break;
            }

        }
    CleanupStack::PopAndDestroy( name );
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::PopFromStackIfFoundL
// Checks that last item on stack is the expected one
// -----------------------------------------------------------------------------
//
void CVcXmlParser::PopFromStackIfFoundL( const TDesC8& aTagName, const TDesC8& aExpectedTag )
    {
    if ( aTagName != aExpectedTag )
        {
        RUBY_DEBUG0( "VC XML ERROR: Unexpected ending tag found" );
        User::Leave( KErrNotFound );
        }
        
    iXmlNesting.Remove( iXmlNesting.Count() - 1 );    
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnContentL
// Callback from XML parser when element has some content
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::OnContentL" );
    
    User::LeaveIfError( aErrorCode );
    
    TInt lastTag = iXmlNesting.Count() - 1;
    
    if ( iXmlNesting[ lastTag ] == EXmlCmdlineparams )
        {
        if ( !iParameters8 )
            {
            // Create new buffer
            iParameters8 = HBufC8::NewL( aBytes.Size() );
            TPtr8 des( iParameters8->Des() );
            des.Copy( aBytes );
            }
        else
            {
            // Append at the end of previous stuff
            iParameters8 = iParameters8->ReAllocL( iParameters8->Size() + aBytes.Size() );
            TPtr8 des( iParameters8->Des() );
            des.Append( aBytes );
            }

        RUBY_DEBUG0( "<cmdlineparams> content resolved" );
        }
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnStartPrefixMappingL
// Callback from XML parser
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnStartPrefixMappingL( const RString& /*aPrefix*/, 
	                                      const RString& /*aUri*/, 
	                                      TInt /*aErrorCode*/ )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnEndPrefixMappingL
// Callback from XML parser
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnEndPrefixMappingL( const RString& /*aPrefix*/, 
                                        TInt /*aErrorCode*/ )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnIgnorableWhiteSpaceL
// Callback from XML parser
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, 
                                           TInt /*aErrorCode*/ )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnSkippedEntityL
// Callback from XML parser
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnSkippedEntityL( const RString& /*aName*/, 
                                     TInt /*aErrorCode*/ )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnProcessingInstructionL
// Callback from XML parser
// -----------------------------------------------------------------------------
//
void CVcXmlParser::OnProcessingInstructionL( const TDesC8& /*aTarget*/, 
                                             const TDesC8& /*aData*/, 
	                                         TInt /*aErrorCode*/ )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::OnError
// Callback from XML parser when general error has occured
// -----------------------------------------------------------------------------
//
#if defined(_DEBUG) && !defined(__RUBY_DEBUG_DISABLED)
void CVcXmlParser::OnError( TInt aErrorCode )
    {
    RUBY_DEBUG1( "CVcXmlParser::OnError general error: [%d]", aErrorCode );
    }
#else
void CVcXmlParser::OnError( TInt /*aErrorCode*/ )
    {
    // Nothing
    }
#endif // #if defined(_DEBUG) && !defined(__RUBY_DEBUG_DISABLED)
    
// -----------------------------------------------------------------------------
// CVcXmlParser::GetExtendedInterface
// Callback from XML parser
// -----------------------------------------------------------------------------
//
TAny* CVcXmlParser::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::CheckAttributeL
// Checks the value of certain attribute
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::CheckAttributeL( const Xml::RAttributeArray& aAttributes,
                                     const TDesC8& aAttribute, const TDesC8& aValue )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::CheckAttributeL" );
    
    TBool resolved( EFalse );
    
    // Loop through attribute list
    for ( TInt iCounter = 0 ; iCounter < aAttributes.Count() ; iCounter++ )
        {        
        HBufC8* name = ToLowercaseLC( aAttributes[iCounter].Attribute().LocalName().DesC() );
       
        if ( name->Des() == aAttribute )
            {
            HBufC8* value = ToLowercaseLC( aAttributes[iCounter].Value().DesC() );
            
            // If value is null descriptor, then we just check if attribute exists           
            if ( aValue == KNullDesC8 || value->Des() == aValue )
                {                
                resolved = ETrue;
                }
            
            CleanupStack::PopAndDestroy( value );
            }
        CleanupStack::PopAndDestroy( name );
        }
        
    return resolved;
    }
    
// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveDocumentTagL
// Resolves the content of <nssvcommands>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveDocumentTagL( const TDesC8& aName, 
                                        const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveDocumentTagL" );
    
    TBool resolved( EFalse );
    
    if ( aName == KXmlDocumentTag )
        {
        iXmlNesting.Append( EXmlDocument );
        resolved = CheckAttributeL( aAttributes, KXmlVersionAttr, KXmlVersionValue );
        if ( !resolved )
            {
            RUBY_DEBUG0( "VC XML ERROR: Version information not found or it does not match" );
            User::Leave( KErrGeneral );
            }
        else
            {
            CVcResource* newResource( NULL );
            TPtrC8 locFileAttr( KXmlLocFileAttr );
            TRAPD( error, newResource =
                CVcResource::NewL( iRFs, Find16ValueL( locFileAttr, aAttributes ) ) );

            // Delete old resource object if new was found
            if ( newResource )
                {
                delete iResource;
                iResource = newResource;
                }
        
            if ( error != KErrNone )
                {
                resolved = EFalse;
                RUBY_DEBUG0( "VC XML ERROR: Resource file load error" );
                User::Leave( KErrNotFound );
                }
            }
        }
     else
        {
        RUBY_DEBUG0( "VC XML ERROR: <nssvoicecommands> exptected but not found" );
        User::Leave( KErrNotFound );        
        }
               
    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveFolderTagL
// Resolves the content of <vcommandfolder>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveFolderTagL( const TDesC8& aName, 
                                       const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveFolderTagL" );
    
    // Use specific resource file for folder strings if specified
    CVcResource* mainResource = iResource;
    
    CVcResource* newResource( NULL );
    TPtrC locFile = Find16ValueL( KXmlLocFileAttr, aAttributes );
    if ( locFile != KNullDesC )
        {
        newResource = CVcResource::NewL( iRFs, locFile );
        }
                  
    if ( newResource )
        {
        iResource = newResource;
        CleanupStack::PushL( mainResource );
        }    
    
    // Find folder name
    TBool resolved = ResolveLocalizedTagL( aName, aAttributes, KXmlFolderTag, 
                                           EXmlFolder );
    if ( resolved )
        {
        delete iFolder;
        iFolder = iCurrentLocalizedString;
        iCurrentLocalizedString = NULL;
        
        // Find folder title string
        TBool titleFound = GetLocalizedStringL( aAttributes, KXmlFolderTitleLocAttr );
        
        if ( titleFound )
            {
            delete iFolderTitle;
            iFolderTitle = iCurrentLocalizedString;
            iCurrentLocalizedString = NULL;
            }
            
        TBool iconFileFound = ResolveFolderIconFileL( aAttributes );
        RUBY_DEBUG1( "iconFileFound [%d]", iconFileFound );
        iFolderIconIndex = ResolveFolderIconIndexL( aAttributes );
       
        }
        
    // Revert back to use the old resource file
    iResource = mainResource;
    if ( newResource )
        {
        delete newResource;
        CleanupStack::Pop( mainResource );
        }
                
    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveVCommandTagL
// Resolves the content of <vcommand>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveVCommandTagL( const TDesC8& aName, 
                                         const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveVCommandTagL" );
    
    TBool resolved( EFalse );
    if ( aName == KXmlVCommandTag )
        {
        resolved = ETrue;
        iXmlNesting.Append( EXmlVCommand );
        }
    
    if ( resolved )
        {
        // TTS on/off  
        resolved = CheckAttributeL( aAttributes, KXmlTtsAttr, KXmlTrueValue );
        if ( resolved )
            {
            // By default, tts is on
            iStartImmediately = ETrue;
            }
        
        // Modifiable on/off
        resolved = CheckAttributeL( aAttributes, KXmlModifiableAttr, KXmlFalseValue );
        if ( resolved )
            {
            iUserCanModify = EFalse;
            }
            
        resolved = ETrue;
        }

    return resolved;
    }


// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveSpokenTagL
// Resolves the content of <spoken>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveSpokenTagL( const TDesC8& aName, 
                                       const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveSpokenTagL" );
    
    TBool resolved = ResolveLocalizedTagL( aName, aAttributes, KXmlSpokenTag, 
                                           EXmlSpoken );

    if ( resolved )
        {
        delete iSpokenText;
        iSpokenText = iCurrentLocalizedString;
        iCurrentLocalizedString = NULL;
        }
    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveWrittenTagL
// Resolves the content of <written>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveWrittenTagL( const TDesC8& aName,
                                        const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveWrittenTagL" );
    
    TBool resolved = ResolveLocalizedTagL( aName, aAttributes, KXmlWrittenTag, 
                                           EXmlWritten );
    if ( resolved )
        {
        delete iWrittenText;
        iWrittenText = iCurrentLocalizedString;
        iCurrentLocalizedString = NULL;
        }
    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveTooltipTagL
// Resolves the content of <tooltip>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveTooltipTagL( const TDesC8& aName,
                                        const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveTooltipTagL" );
    
    TBool resolved = ResolveLocalizedTagL( aName, aAttributes, KXmlTooltipTag, 
                                           EXmlTooltip );
    if ( resolved )
        {
        delete iTooltipText;
        iTooltipText = iCurrentLocalizedString;
        iCurrentLocalizedString = NULL;
        }
    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveLocalizedTagL
// Resolves content of tag which contains localized data
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveLocalizedTagL( const TDesC8& aName,
                                          const Xml::RAttributeArray& aAttributes,
                                          const TDesC8& aExpectedName,
                                          TVcXmlTagNesting aTagEnum )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveLocalizedTagL" );
    
    TBool resolved( EFalse );
    if ( aName == aExpectedName )
        {
        resolved = ETrue;
        iXmlNesting.Append( aTagEnum );
        
        resolved = GetLocalizedStringL( aAttributes, KXmlLocAttr );
        }

    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveExecutesTagL
// Resolves the content of <executes>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveExecutesTagL( const TDesC8& aName,
                                         const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveExecutesTagL" );
    
    TBool resolved( EFalse );
   
    if ( aName == KXmlExecutesTag )
        {
        resolved = ETrue;
        iXmlNesting.Append( EXmlExecutes );

        iAppId = ResolveUidL( aAttributes );
        if ( iAppId == KNullUid )
            {
            resolved = ResolveExeL( aAttributes );
            }
        }

    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveCmdlineparamsTag
// Resolves the content of <cmdlineparams>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveCmdlineparamsTag( const TDesC8& aName, 
                                             const Xml::RAttributeArray& /*aAttributes*/ )
    {
    TBool resolved( EFalse );
    
    if ( aName == KXmlCmdlineparamsTag )
        {
        iXmlNesting.Append( EXmlCmdlineparams );
        delete iParameters;
        delete iParameters8;
        iParameters = NULL;
        iParameters8 = NULL;
        resolved = ETrue;        
        }
        
    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveIconTagL
// Resolves the content of <icon>
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveIconTagL( const TDesC8& aName,
                                     const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveIconTagL" );
    
    TBool resolved( EFalse );
    
    if ( aName == KXmlIconTag )
        {
        resolved = ETrue;
        iXmlNesting.Append( EXmlIcon );

        iIcon = ResolveUidL( aAttributes );
        if ( iIcon == KNullUid )
            {
            resolved = EFalse;
            }
        }

    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::Find16ValueL
// Finds a certain attribute and returns its value as unicode
// -----------------------------------------------------------------------------
//
const TDesC& CVcXmlParser::Find16ValueL( const TDesC8& aName, 
                                         const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::Find16ValueL" );
    
    delete iAttributeValue;
    iAttributeValue = NULL;
    
    iAttributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf8L( Find8ValueL( aName, aAttributes ) );
       
    return *iAttributeValue;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::Find8ValueL
// Finds a certain attribute and returns its value as utf-8
// -----------------------------------------------------------------------------
//
const TDesC8& CVcXmlParser::Find8ValueL( const TDesC8& aName, 
                                         const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::Find8ValueL" );
    
    // Loop through attribute list
    for ( TInt iCounter = 0 ; iCounter < aAttributes.Count() ; iCounter++ )
        {
        HBufC8* name = ToLowercaseLC( aAttributes[iCounter].Attribute().LocalName().DesC() );
        
        if ( name->Des() == aName )
            {
            CleanupStack::PopAndDestroy( name );

            return aAttributes[iCounter].Value().DesC();
            }
        CleanupStack::PopAndDestroy( name );
        }
        
    return KNullDesC8;
    }
    
// -----------------------------------------------------------------------------
// CVcXmlParser::ResetVCommandFolder
// Destroys the data of one voice command folder
// -----------------------------------------------------------------------------
//
void CVcXmlParser::ResetVCommandFolder()
    {
    delete iFolder;
    iFolder = HBufC16::New( 0 );
    
    delete iFolderTitle;
    iFolderTitle = HBufC16::New( 0 );
    
    delete iFolderIconFile;
    iFolderIconFile = HBufC16::New( 0 );
    
    iFolderIconIndex = -1;
    
    ResetVCommandInfo();
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResetVCommandInfo
// Destroys the data of one voice command
// -----------------------------------------------------------------------------
//
void CVcXmlParser::ResetVCommandInfo()
    {
    delete iWrittenText;
    iWrittenText = NULL;
    delete iSpokenText;
    iSpokenText = NULL;
    delete iTooltipText;
    iTooltipText = NULL;
    delete iParameters;
    iParameters = NULL;
    delete iParameters8;
    iParameters8 = NULL;
    delete iAppName;
    iAppName = NULL;
    
    iAppId = KNullUid;
    iIcon = KNullUid;
    
    iStartImmediately = EFalse;
    iUserCanModify = ETrue;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::CheckVoiceCommmandData
// Checks if all mandatory data for one voice command is available and makes
// callback based on that
// -----------------------------------------------------------------------------
//
void CVcXmlParser::CheckVoiceCommmandData()
    {
    if ( iWrittenText && ( iAppId != KNullUid || iAppName ) )
        {
        RUBY_DEBUG0( "CVcXmlParser::CheckVoiceCommmandData(): All required data for voice command found ok" );
        
        // folder icon file is optional
        // if it is not present, reference to KNullDesC should be passed
        const TDesC* pFolderIconFile =iFolderIconFile;
        if( !pFolderIconFile )
            {
            pFolderIconFile = &KNullDesC;
            }
        
        iObserver.MvcxpoVoiceCommandFound( *iWrittenText, *iSpokenText, 
                                           *iTooltipText, *iFolder, *iFolderTitle, 
                                           *iParameters, iIcon,
                                           iAppId, *iAppName,
                                           iStartImmediately, iUserCanModify,
                                           *pFolderIconFile, iFolderIconIndex );
        }
    else
        {
        RUBY_DEBUG0( "VC XML ERROR: Mandatory data not found for Voice Command" );
        }
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::GetLocalizedStringL
// Reads localized string from resource file
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::GetLocalizedStringL( const Xml::RAttributeArray& aAttributes,
                                         const TDesC8& aAttributeName )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::GetLocalizedStringL" );
    
    TBool resolved( EFalse );
    iCurrentLocalizedString = NULL;
    
    // Resource file object should have been initialized
    if ( !iResource )
        {
        return resolved;
        }

    // locIndex is key to the localized resource file
    TPtrC8 locAttr( aAttributeName );    
    TPtrC locIndex = Find16ValueL( locAttr, aAttributes );
    
    if ( locIndex != KNullDesC )
        {
        HBufC* command = NULL;
        // Find string from loc file based on key
        TRAPD( error,  command = &iResource->GetCommandL( locIndex ) );
        if ( error == KErrNone )
            {
            // Make a copy of localized string
            TPtr des = command->Des();
            delete iCurrentLocalizedString;
            iCurrentLocalizedString = des.Alloc();
            resolved = ETrue;
            }
        else
            {
            resolved = EFalse;
            iCurrentLocalizedString = NULL;
            RUBY_DEBUG0( "VC XML ERROR: Cannot resolve localized string" );
            User::Leave( KErrGeneral );
            }
        }
        
    return resolved;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveUidL
// Resolves the content of uid="0x1234567" attribute
// -----------------------------------------------------------------------------
//
TUid CVcXmlParser::ResolveUidL( const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveUidL" );
    
    _LIT( KHexIdentifierLowercase, "0x" );
    _LIT( KHexIdentifierUppercase, "0X" );
    const TInt KHexIndentifierLength = 2;
    
    TUid newUid( KNullUid );
    
    TPtrC uidDes = Find16ValueL( KXmlUidAttr, aAttributes );
    
    if ( uidDes != KNullDesC )
        {
        TPtrC ptr( uidDes );
        // Check if string starts with 0x or 0X
        if ( ( uidDes.Find( KHexIdentifierLowercase ) == 0 ) ||
             ( uidDes.Find( KHexIdentifierUppercase ) == 0 ) )
            {
            ptr.Set( uidDes.Right( uidDes.Length() - KHexIndentifierLength ) );
            }
        
        // Do conversion from string to number
        TLex lex( ptr );
        TInt64 intUid( 0 );
        TInt error = lex.Val( intUid, EHex );
        newUid.iUid = intUid;
        }
        
    return newUid;
    }

// -----------------------------------------------------------------------------
// CVcXmlParser::ResolveExeL
// Resolves the content of exe="name.exe" attribute
// -----------------------------------------------------------------------------
//
TBool CVcXmlParser::ResolveExeL( const Xml::RAttributeArray& aAttributes )
    {
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveExeL" );
    
    TBool resolved( EFalse );
    
    TPtrC nameDes = Find16ValueL( KXmlExeAttr, aAttributes );
    if ( nameDes != KNullDesC )
        {
        delete iAppName;
        iAppName = NULL;
        
        iAppName = HBufC::NewL( nameDes.Length() );
        TPtr namePtr = iAppName->Des();
        namePtr.Copy( nameDes );
        
        resolved = ETrue;
        }
        
    return resolved;
    }
    
// -----------------------------------------------------------------------------
// CVcXmlParser::ToLowercaseLC
// Converts given string to lowercase
// -----------------------------------------------------------------------------
//
HBufC8* CVcXmlParser::ToLowercaseLC( const TDesC8& aString )
    {
    // RUBY_DEBUG0( "CVcXmlParser::ToLowercaseLC START" );
    
    // Convert string to lowercase    
    HBufC8* string = HBufC8::NewL( aString.Length() );
    CleanupStack::PushL( string );
    TPtr8 des = string->Des();
    des.Copy( aString );
    des.LowerCase();
    
    //RUBY_DEBUG0( "CVcXmlParser::ToLowercaseLC EXIT" );
    
    return string;
    }

// -----------------------------------------------------------------------------
// Reads the iconfile attribute and constructs the file name or
// sets it to NULL
// 
// @params aAttributes List of XML attributes
// @return ETrue if resolved successfully, EFalse otherwise
// -----------------------------------------------------------------------------
TBool CVcXmlParser::ResolveFolderIconFileL( const Xml::RAttributeArray& aAttributes )
	{
    RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveFolderIconFileL" );
    
    TBool resolved( EFalse );
    delete iFolderIconFile;
    iFolderIconFile = NULL;
    
    TPtrC nameDes = Find16ValueL( KXmlFolderIconFileAttr, aAttributes );
    if ( nameDes != KNullDesC )
        {
        iFolderIconFile = HBufC::NewL( nameDes.Length() );
        TPtr folderIconFilePtr = iFolderIconFile->Des();
        folderIconFilePtr.Copy( nameDes );
        
        resolved = ETrue;
        }
        
    return resolved;
	
	}

// -----------------------------------------------------------------------------
// Reads the iconindex attribute and stores the index or sets it to 0
// 
// @params aAttributes List of XML attributes
// @return ETrue if resolved successfully, EFalse otherwise
// -----------------------------------------------------------------------------
TInt CVcXmlParser::ResolveFolderIconIndexL( const Xml::RAttributeArray& aAttributes )
	{
	RUBY_DEBUG_BLOCKL( "CVcXmlParser::ResolveFolderIconIndexL" );
	
	TInt index = -1;
    
    TPtrC indexDes = Find16ValueL( KXmlFolderIconIndexAttr, aAttributes );
    
    if ( indexDes != KNullDesC )
        {
        TPtrC ptr( indexDes );
        
        // Do conversion from string to number
        TLex lex( ptr );
        TInt error = lex.Val( index );
        if( error != KErrNone )
        	{
        	RUBY_ERROR1( "Converting icon index from string to int resulted in [%d]", error );
        	index = -1;
        	}
        }
        
    return index;	
	}

// End of File
