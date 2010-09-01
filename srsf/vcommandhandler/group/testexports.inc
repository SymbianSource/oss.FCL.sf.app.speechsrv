/*
* ==============================================================================
*        %name: testexports.inc %
*     %version: 7 %
*      Part of: SRSF / VCommandHandler
*  Description: List of test exports for the VCommandHandler modules.
*               To be included into bld.infs
*
*  Copyright c 2006 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

// Global test exports
//..\internal\tsrc\vcommandhandler_tests.cfg \epoc32\winscw\c\TestFramework\vcommandhandler_tests.cfg
//..\internal\tsrc\vcommandhandler_tests.cfg \epoc32\wins\c\TestFramework\vcommandhandler_tests.cfg
//..\internal\tsrc\vcommandhandler_tests.cfg \epoc32\data\c\TestFramework\vcommandhandler_tests.cfg

//..\internal\tsrc\testframework.ini \epoc32\data\c\TestFramework\testframework.ini
//..\internal\tsrc\testframework.ini \epoc32\wins\c\TestFramework\testframework.ini
//..\internal\tsrc\testframework.ini \epoc32\winscw\c\TestFramework\testframework.ini


// Exports of the test projects
#include "../internal/tsrc/VcsPrimTest/group/exports.inc"
#include "../internal/tsrc/VcsUnitTest/group/exports.inc"
#include "../internal/tsrc/VCommandVasTest/group/exports.inc"
#include "../internal/tsrc/VCommandHandlerTest/group/exports.inc"
#include "../internal/tsrc/TagCommandConverterTest/group/exports.inc"
#include "../internal/tsrc/MultiUseTest/group/exports.inc"
#include "../internal/tsrc/RecogHandlerTest/group/exports.inc"
#include "../internal/tsrc/IntPropertyWatcherTest/group/exports.inc"
#include "../internal/tsrc/VcsMultiThreadingTest/group/exports.inc"