/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  For test ui voice recognition api modules
*
*/



#ifndef C_TESTDOMUIVOICERENGN_H
#define C_TESTDOMUIVOICERENGN_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <testclassassert.h>
#include <vuicvoicerecog.h>
#include <vuicvoicerecogdialog.h>

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KtestdomuivoicerengnLogPath, "\\logs\\testframework\\testdomuivoicerengn\\" ); 
// Log file
_LIT( KtestdomuivoicerengnLogFile, "testdomuivoicerengn.txt" ); 
_LIT( KtestdomuivoicerengnLogFileWithTitle, "testdomuivoicerengn_[%S].txt" );

class CVoiceRecog;
class CVoiceRecognitionDialog;

/**
*  CTestDOMUiVoiceRengn test class for STIF Test Framework TestScripter.
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTestDOMUiVoiceRengn ) : public CScriptBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CTestDOMUiVoiceRengn* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~CTestDOMUiVoiceRengn();

public: // Functions from base classes

    /**
    * From CScriptBase Runs a script line.
    * @since S60 5.0
    * @param aItem Script line containing method name and parameters
    * @return Symbian OS error code
    */
    virtual TInt RunMethodL( CStifItemParser& aItem );

private:

    /**
    * C++ default constructor.
    */
    CTestDOMUiVoiceRengn( CTestModuleIf& aTestModuleIf );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();
    
        /**
     * Turn off ScreenSaver
     * @since S60 5.0
     * @return Symbian OS error code.
     */
    void TurnOffScreenSaver();

    /**
     * Restore ScreenSaver
     * @since S60 5.0
     * @return Symbian OS error code.
     */
    void RestoreScreenSaver();

	// [TestMethods]
private:// for testing the vuicvoicerecog.h
	
    /**
    * TestVuicVocRecogNewL test method for testing the NewL method
    * @since S60 5.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt TestVuicVocRecogNewL( CStifItemParser& aItem );
    
    /**
    * TestVuicVocRecogNewLC test method for testing the NewLC method
    * @since S60 5.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt TestVuicVocRecogNewLCL( CStifItemParser& aItem );
    
    // for testing the vuicvoicerecogdialog.h
    /**
    * TestVoceRegDalgCVoceRegDialogL test method for testing the CVoiceRecognitionDialog method
    * @since S60 5.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt TestVoceRegDalgCVoceRegDialogL( CStifItemParser& aItem );
    
    /**
    * TestVoceRegDalgDestructor test method for testing the ~CVoiceRecognitionDialog method
    * @since S60 5.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt TestVoceRegDalgDestructor( CStifItemParser& aItem );
    /**
    * TestVoceRegDalgExecuteLD test method for testing the ExecuteLD method
    * @since S60 5.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt TestVoceRegDalgExecuteLDL( CStifItemParser& aItem );
    /**
    * TestVoceRegDalgCancel test method for testing the Cancel method
    * @since S60 5.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt TestVoceRegDalgCancel( CStifItemParser& aItem );
private:    // Data

    /**
     * ScreenSaver Property
     */
    TInt iOldScreenSaverProperty;

    /**
     * Own Member
     *
     */
    CVoiceRecog* iVoiceRecog;
    /**
     * Own Member
     *
     */
    CVoiceRecognitionDialog* iVoiceRecogDialog;

    };

#endif      // C_TESTDOMUIVOICERENGN_H

// End of File
