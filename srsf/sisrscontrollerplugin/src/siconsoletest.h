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
* Description:  This class is for unit testing purpose only.
*
*/


#ifndef __CONSOLETEST__
/* In order to build PluginConsoleTest.exe, you must uncomment the preprocessor directive
in SIConsoleTest.h located under /SDControllerPlugin/Src/:  #define __CONSOLETEST__.  
Once testing is complete, make sure this is commented back or plugin will not build.
*/
//#define __CONSOLETEST__

#define __MYTEST__ //?? Here I define a test 


#ifdef __CONSOLETEST__

#include <d32dbms.h>
#include <SpeechRecognitionDataDevASR.h>
#include <nsssispeechrecognitiondatadevasr.h>
#endif

#endif
