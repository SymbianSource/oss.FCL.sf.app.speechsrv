/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main class of Voice Command Manager
*
*/


#ifndef VCOMMANDMANAGER_H
#define VCOMMANDMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "vcxmlparser.h"

#include "vcommandapi.h"

// FORWARD DECLARATIONS

class CRepository;
class CVCommandHandler;
class CVCommandArray;
class CVCommand;

// CLASS DECLARATION

/**
* Main class of Voice Command Manager
*
* @lib nssvcommandmanager.exe
*
*/
class CVCommandManager : public CBase, public MVcXmlParserObserver
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aForceRetrain For testing purposes. ETrue to retrain all commands
        */
        static CVCommandManager* NewL( TBool aForceRetrain );
        
        /**
        * Destructor.
        */
        virtual ~CVCommandManager();
        
    public: // From MVcXmlParserObserver
    
        /**
        * Creates new voice command with given parameters
        *
        * @see MVcXmlParserObserver for more information
        */
        void MvcxpoVoiceCommandFound( const TDesC& aWrittenText,
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
                                      );
        
    public: // New functions

        /**
        * Thread function.
        */
        static TInt VCMThreadFunction( TAny* aParams );
       
    private:
        
        /**
        * C++ default constructor.
        * 
        * @param aForceRetrain For testing purposes. ETrue to retrain all commands
        */
        CVCommandManager( TBool aForceRetrain );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Goes through all XML files
        */ 
        void ParseAllFilesL();
        
        
        /**
        * Goes through all XML files in directory
        *
        * @param aDrive Contains directory path (e.g. C:\private\)
        */
        void ParseAllFilesFromDirectoryL( const TDesC& aDirectory );

        /**
        * Compares if two commands are equal
        * @param aFirst Command to compare
        * @param aSecond Command to compare
        * @return ETrue if commands are equal else EFalse
        */
        TBool CompareCommands( const CVCommand* aFirst, const CVCommand* aSecond );
        
        void ConstructCommandL( const TDesC& aWrittenText,
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
                                );

        
    private: // Data

        // Parser
        CVcXmlParser*                   iParser;
        
        // Command array for commands found from xml-files
        RVCommandArray                  iCommands;
        
        // Command index array
        RArray<TInt>                    iMatchingIndices;
        
        // Command array for commands found from VAS
        CVCommandArray*                 iCommandArray;
        
        // Tells if language has changed
        TBool                           iLanguageChanged;
        
        // File system handle
        RFs                             iRFs;
 
    };

#endif // VCOMMANDMANAGER_H

// End of File
