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
* Description:  Parser class for Voice Commands XML
*
*/


#ifndef VCXMLPARSER_H
#define VCXMLPARSER_H

// INCLUDES
#include <e32base.h>
#include <contenthandler.h>
#include <parser.h>

// DATA TYPES

// Enumeration that is used when saving information
// of last found xml tag
enum TVcXmlTagNesting
    {
     EXmlStarted = 0,
     EXmlDocument, 
     EXmlFolder,
     EXmlVCommand,
     EXmlSpoken,
     EXmlWritten,
     EXmlExecutes,
     EXmlCmdlineparams,
     EXmlIcon,
     EXmlTooltip
     };

// FORWARD DECLARATIONS
class CVcResource;

// CLASS DECLARATION

/**
* Observer mixin class for Voice Commands XML parser
*
* @lib nssvcommandmanager.exe
*
*/
NONSHARABLE_CLASS( MVcXmlParserObserver )
    {
    public: // New functions
        
        /**
        * Callback when voice command data has been completely resolved.
        * 
        * @param aWrittenText Voice Command text in written form
        * @param aSpokenText Voice Command text in spoken form
        * @param aTooltipText Tooltip text
        * @param aFolder Folder where Voice Command should be placed
        *                               in the UI
        * @param aFolderTitle String which is used in folder title pane
        * @param aParameters Command line parameters for the application
        *                                   which is started
        * @param aIcon Icon which is used when showing the voice command
        * @param aAppId UID of the application which will be started
        * @param aAppName Name of the application which will be started
        * @param aStartImmediately ETrue if no TTS verification is needed when
        *                                  command has been recognized
        * @param aUserCanModify ETrue if user can modify the command
        * @param aFolderIconFile Full path to the MBM file where to get the folder icon from
        *        or KNullDesC if not defined
        * @param aFolderIconIndex Index of the folder icon in the aFolderIconFile.
        *        or -1 if not defined
        */
        virtual void MvcxpoVoiceCommandFound( const TDesC& aWrittenText,
                                              const TDesC& aSpokenText,
                                              const TDesC& aTooltipText,
                                              const TDesC& aFolder,
                                              const TDesC& aFolderTitle,
                                              const TDesC& aParameters, 
                                              TUid aIcon,
                                              TUid aAppId,
                                              const TDesC& aAppName,
                                              TBool aStartImmediately,
                                              TBool aUserCanModify, 
                                              const TDesC& aFolderIconFile,
                                              TInt aFolderIconIndex
                                              ) = 0;
        
    };
    

/**
* Parser class for Voice Commands XML
*
* @lib nssvcommandmanager.exe
*
*/
NONSHARABLE_CLASS( CVcXmlParser ) : public CBase, public Xml::MContentHandler
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVcXmlParser* NewL( RFs& aRFs, MVcXmlParserObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CVcXmlParser();
        
    public: // New functions
        
        /**
        * Parses one XML file
        *
        * @param aFileName XML file name
        */
        void ParseFileL( const TFileName& aFileName );
        
        
        
    public: // From Xml::MContentHandler
        
        /**
        This method is a callback to indicate the start of the document.
        @param				aDocParam Specifies the various parameters of the document.
        @param				aDocParam.iCharacterSetName The character encoding of the document.
        @param				aErrorCode is the error code. 
				        	If this is not KErrNone then special action may be required.
        */
	    void OnStartDocumentL( const Xml::RDocumentParameters& aDocParam, 
	                           TInt aErrorCode );

        /**
        This method is a callback to indicate the end of the document.
        @param				aErrorCode is the error code. 
        					If this is not KErrNone then special action may be required.
        */
	    void OnEndDocumentL( TInt aErrorCode );

        /**
        This method is a callback to indicate an element has been parsed.
        @param				aElement is a handle to the element's details.
        @param				aAttributes contains the attributes for the element.
        @param				aErrorCode is the error code.
		        			If this is not KErrNone then special action may be required.
        */
	    void OnStartElementL( const Xml::RTagInfo& aElement, 
	                          const Xml::RAttributeArray& aAttributes, 
	                          TInt aErrorCode );

        /**
        This method is a callback to indicate the end of the element has been reached.
        @param				aElement is a handle to the element's details.
        @param				aErrorCode is the error code.
        					If this is not KErrNone then special action may be required.
        */
	    void OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode );

        /**
        This method is a callback that sends the content of the element.
        Not all the content may be returned in one go. The data may be sent in chunks.
        When an OnEndElementL is received this means there is no more content to be sent.
        @param				aBytes is the raw content data for the element. 
		        			The client is responsible for converting the data to the 
				        	required character set if necessary.
					        In some instances the content may be binary and must not be converted.
        @param				aErrorCode is the error code.
		        			If this is not KErrNone then special action may be required.
        */
	    void OnContentL( const TDesC8& aBytes, TInt aErrorCode );

        /**
        This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
        This method is always called before the corresponding OnStartElementL method.
        @param				aPrefix is the Namespace prefix being declared.
        @param				aUri is the Namespace URI the prefix is mapped to.
        @param				aErrorCode is the error code.
		        			If this is not KErrNone then special action may be required.
        */
	    void OnStartPrefixMappingL( const RString& aPrefix, 
	                                const RString& aUri, 
	                                TInt aErrorCode );

        /**
        This method is a notification of the end of the scope of a prefix-URI mapping.
        This method is called after the corresponding DoEndElementL method.
        @param				aPrefix is the Namespace prefix that was mapped.
        @param				aErrorCode is the error code.
		        			If this is not KErrNone then special action may be required.
        */
	    void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );

       /**
        This method is a notification of ignorable whitespace in element content.
        @param				aBytes are the ignored bytes from the document being parsed.
        @param				aErrorCode is the error code.
        					If this is not KErrNone then special action may be required.
        */
	    void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );

        /**
        This method is a notification of a skipped entity. If the parser encounters an 
        external entity it does not need to expand it - it can return the entity as aName 
        for the client to deal with.
        @param				aName is the name of the skipped entity.
        @param				aErrorCode is the error code.
		        			If this is not KErrNone then special action may be required.
        */
	    void OnSkippedEntityL( const RString& aName, TInt aErrorCode );

        /**
        This method is a receive notification of a processing instruction.
        @param				aTarget is the processing instruction target.
        @param				aData is the processing instruction data. If empty none was supplied.
        @param				aErrorCode is the error code.
        					If this is not KErrNone then special action may be required.
        */
	    void OnProcessingInstructionL( const TDesC8& aTarget, 
	                                   const TDesC8& aData, 
	                                   TInt aErrorCode );

        /**
        This method indicates an error has occurred.
        @param				aError is the error code
        */
	    void OnError( TInt aErrorCode );

        /**
        This method obtains the interface matching the specified uid.
        @return				0 if no interface matching the uid is found.
        					Otherwise, the this pointer cast to that interface.
        @param				aUid the uid identifying the required interface.
        */
	    TAny* GetExtendedInterface( const TInt32 aUid );

        
    private:
        
        /**
        * C++ default constructor.
        */
        CVcXmlParser( RFs& aRFs, MVcXmlParserObserver& aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Checks if attribute value equals to the expected. If no value is given, just
        * checks that the attribute exists.
        *
        * @param aAttributes List of attributes
        * @param aAttribute Name of the attribute which is searched
        * @param aValue Needed value.
        * @return ETrue if attribute was found, EFalse otherwise
        */        
        TBool CheckAttributeL( const Xml::RAttributeArray& aAttributes,
                               const TDesC8& aAttribute, const TDesC8& aValue = KNullDesC8 );
                             

        /**
        * Resolves content of <nssvcommands> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return ETrue if document tag was resolved
        */                             
        TBool ResolveDocumentTagL( const TDesC8& aName,
                                   const Xml::RAttributeArray& aAttributes );

        /**
        * Resolves content of <vcommandfolder> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return ETrue if folder tag was resolved
        */         
        TBool ResolveFolderTagL( const TDesC8& aName,
                                 const Xml::RAttributeArray& aAttributes );

        /**
        * Resolves content of <vcommand> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return ETrue if vcommand tag was resolved
        */         
        TBool ResolveVCommandTagL( const TDesC8& aName,
                                   const Xml::RAttributeArray& aAttributes );
        
        /**
        * Resolves content of <spoken> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return ETrue if spoken tag was resolved
        */         
        TBool ResolveSpokenTagL( const TDesC8& aName,
                                 const Xml::RAttributeArray& aAttributes );
        
        /**
        * Resolves content of <written> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return ETrue if written tag was resolved
        */         
        TBool ResolveWrittenTagL( const TDesC8& aName,
                                  const Xml::RAttributeArray& aAttributes );
        
        /**
        * Resolves content of <tooltip> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return Error code
        */         
        TBool ResolveTooltipTagL( const TDesC8& aName,
                                  const Xml::RAttributeArray& aAttributes );
        
        /**
        * Resolves content of <executes> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return Error code
        */         
        TBool ResolveExecutesTagL( const TDesC8& aName,
                                  const Xml::RAttributeArray& aAttributes );
        
        /**
        * Resolves content of <cmdlineparams> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return ETrue if commandline parameters were found correctly
        */         
        TBool ResolveCmdlineparamsTag( const TDesC8& aName,
                                       const Xml::RAttributeArray& aAttributes );

        /**
        * Resolves content of <icon> element.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return ETrue if icon element was found
        */        
        TBool ResolveIconTagL( const TDesC8& aName,
                               const Xml::RAttributeArray& aAttributes );
        
        /**
        * Destroys the data of one voice command folder.
        */ 
        void ResetVCommandFolder();
        
        /**
        * Destroys the data of one voice command.
        */        
        void ResetVCommandInfo();
        
        /**
        * Checks that all mandatory data is found for one voice command.
        */
        void CheckVoiceCommmandData();

        /**
        * Reads localized string from loc file based on locindex attribute
        *
        * @param aAttributes Attribute list
        * @param aAttributeName Name of the loc index attribute
        * @return Error code
        */      
        TBool GetLocalizedStringL( const Xml::RAttributeArray& aAttributes,
                                   const TDesC8& aAttributeName );
                
        /**
        * Reads uid attribute and constructs TUid
        *
        * @params aAttributes List of XML attributes
        * @return Parsed UID
        */
        TUid ResolveUidL( const Xml::RAttributeArray& aAttributes );
        
        /**
        * Reads exe attribute and constructs exe file name
        *
        * @params aAttributes List of XML attributes
        * @return Error code
        */
        TBool ResolveExeL( const Xml::RAttributeArray& aAttributes );
        
        /**
         * Reads the iconfile attribute and constructs the file name or
         * sets it to NULL
         * 
         * @params aAttributes List of XML attributes
         * @return ETrue if resolved successfully, EFalse otherwise
         */
        TBool ResolveFolderIconFileL( const Xml::RAttributeArray& aAttributes );

        /**
         * Reads the iconindex attribute and stores the index or sets it to 0
         * 
         * @params aAttributes List of XML attributes
         * @return parsed index if any. -1 if there is no icon index
         */
        TInt ResolveFolderIconIndexL( const Xml::RAttributeArray& aAttributes );
        
        /**
        * Common function to resolve content of all elements which contain localized data.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @param aExpectedName Expected element name
        * @param aTagEnum Enum value equal to tag name
        * @param aAttributeName name of the attribute which contains loc index
        * @return Error code
        */
        TBool ResolveLocalizedTagL( const TDesC8& aName, const Xml::RAttributeArray& aAttributes,
                                    const TDesC8& aExpectedName, TVcXmlTagNesting aTagEnum );
       
        
        
        /**
        * Tries to find matching element name from the attribute list.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return Found attribute as a 16-bit descriptor
        */
        const TDesC& Find16ValueL( const TDesC8& aName,
                                   const Xml::RAttributeArray& aAttributes );
        
        /**
        * Tries to find matching element name from the attribute list.
        *
        * @param aName Element name
        * @param aAttributes Attribute list
        * @return Found attribute as a 8-bit descriptor
        */  
        const TDesC8& Find8ValueL( const TDesC8& aName,
                                   const Xml::RAttributeArray& aAttributes );
                                   
        /**
        * Converts given string to lowercase
        *
        * @param aString String to convert
        * @return Converted string
        */  
        HBufC8* ToLowercaseLC( const TDesC8& aString );
        
        /**
        * Pops the last element from XML stack if aTagName and aExpectedTag are equal
        *
        * @param aTagName Tag name which was found in XML file
        * @param aExpectedTag Tag which was expected
        */
        void PopFromStackIfFoundL( const TDesC8& aTagName, const TDesC8& aExpectedTag );
     
    private: // Data
    
        // Callback observer
        MVcXmlParserObserver& iObserver;    
    
        // Currently open localization resource file
        CVcResource* iResource;
        
        // String read from localization resource file
        HBufC* iCurrentLocalizedString;
        
        // Stack of XML elements
        RArray<TVcXmlTagNesting> iXmlNesting;
        
        // Data for one Voice Command
        // Text shown on screen
        HBufC* iWrittenText;
        
        // Text which will be trained, if KNullDesC then iWrittenText will be trained
        HBufC* iSpokenText;
        
        // Icon
        TUid iIcon;
        
        // Text of the tooltip
        HBufC* iTooltipText;
        
        // Folder name, if it does not exist, then it will be created
        // If NULL then "root" will be used
        HBufC* iFolder;
        
        // Title pane text for folder
        HBufC* iFolderTitle;
        
        HBufC* iFolderIconFile;
        
        TInt iFolderIconIndex;
        
        // UID of the application which will be started when this voice command has been recognized
        TUid iAppId;
        
        // Name of the application which will be started when this voice command has been recognized
        HBufC* iAppName;
        
        // Command line parameters for the application which will be started
        HBufC* iParameters;
        HBufC8* iParameters8;
        
        // TTS feedback & adaptation of the recognition result on/off
        TBool iStartImmediately;
        
        // Can user modify?
        TBool iUserCanModify;
        
        // Value for attribute
        HBufC* iAttributeValue;
        
        // XML Parser
        Xml::CParser* iParser;
        
        // File system handle
        RFs& iRFs;
    };

#endif // VCXMLPARSER_H

// End of File
