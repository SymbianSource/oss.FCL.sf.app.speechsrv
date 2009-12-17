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
* Description:  This is an implementation class for checking which 
*               application or profile needs to be activated and then
*               Starts it.
*
*/


// INCLUDE FILES
#include "rubydebug.h"
#include "nssvcapplauncher.h"
#include "nssvcapplauncher.hrh"
#include "nssvcapplauncherconstants.h"
#include <etel.h>
#include <etelmm.h>
#include <commdb.h>
#include <apgtask.h>     // TApaTask, TApaTaskList
#include <apacmdln.h>    // CApaCommandLine
#include <e32def.h>
#include <f32file.h>     // RFs
#include <bautils.h>     // BaflUtils
#include <RemConExtensionApi.h>
#include <remconinterfaceselector.h>
#include <remconcoreapicontroller.h>


#include <vcommandexecutor.rsg>

#include <AknGlobalConfirmationQuery.h> // Confirmation query.

// CONSTANTS

_LIT( KPanicCategory, "VCommandExecutor" );
//Command line parameter for launching calendar to day view
_LIT16( KCalendarDayCmdLine, "DATE %d %d %d 08 00 00 000000" );
//Command line parameter for launching calendar to week view
_LIT16( KCalendarWeekCmdLine, "WEEK %d %d %d %d 00 00 000000" );
//Command line parameter for launching calendar to month view
_LIT16( KCalendarMonthCmdLine, "MONTH %d %d %d 08 00 00 000000" );
// Command line parameter for Browser application for opening the browser
// to start page
_LIT16( KBrowserArgStartPage, "5" );

// Battery charge level in percentage scaled to five values
const TInt KBatteryFull = 85; // full >= 85
const TInt KBatteryAlmostFull = 65; // 65 <= almost full < 85 
const TInt KBatteryHalfFull = 35; // 35 <= half full < 65
const TInt KBatteryAlmostEmpty = 15; // 15 <= almost empty < 35
// low < 15

// Signal strength level in seven bars scaled to five values
const TInt KSignalExcellent = 6; // excellent >= 6
const TInt KSignalGood = 5; // good == 5 
const TInt KSignalRatherGood = 3; // 3 <= rather good < 5
const TInt KSignalLow = 1; // 1 <= low < 3
const TInt KSignalNone = 0;
// low < 15

// letters for drives in search order
const TBuf<1> KResourceDrivers = _L("z"); 
_LIT( KResourceDir, "\\resource\\" );
_LIT( KResourceFileName, "VCommandExecutor.rsc" );

// String from loc-file must be modified for TTime::FormatL function
_LIT( KLocFileHourString, "%0N" );
_LIT( KLocFileMinuteString, "%1N" );
_LIT( KTTimeHourString, "%J");
_LIT( KTTimeHourStringWithZero, "0%J");
_LIT( KTTimeMinuteString, "%T");

const TInt KDaysInAWeek = 7;


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CNssVCAppLauncher::CNssVCAppLauncher()
    {
    }

// Destructor
CNssVCAppLauncher::~CNssVCAppLauncher()
    {
    
    if ( iApaLsSession )
        {
        iApaLsSession->Close();
        delete iApaLsSession;
        iApaLsSession = NULL;
        }
    if ( iTts )
        {
        delete iTts;
        iTts = NULL;
        }
    if ( iParsedText )
        {
        delete iParsedText;
        iParsedText = NULL;
        }
    if ( iStatusInfo )
        {
        delete iStatusInfo;
        iStatusInfo = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CNssVCAppLauncher::ConstructL
// Symbian 2nd phase constructor
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::ConstructL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::ConstructL" );
    }



// ----------------------------------------------------------------------------
// CNssVCAppLauncher* CNssVCAppLauncher::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CNssVCAppLauncher* CNssVCAppLauncher::NewL()                                                           
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::NewL" );
    
    CNssVCAppLauncher* self= new (ELeave) CNssVCAppLauncher();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// ----------------------------------------------------------------------------
// CNssVCAppLauncher::ExecuteCommandL
// Checks the id and calls the appropriate function.
// ----------------------------------------------------------------------------
//
TInt CNssVCAppLauncher::ExecuteCommandL( TUint aCommandId )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::ExecuteCommandL" );
    
    TInt err = KErrNone;

	switch( aCommandId )
		{
		case ECalendar:
			ActivateAppL( KUidAppCalendar );
			break;
			
        case ECalendarToday:
			CalendarDayViewL( EToday );
			break;
			
        case ECalendarTomorrow:
			CalendarDayViewL( ETomorrow );
			break;
			
        case ECalendarMonday:
			CalendarDayViewL( EMonday );
			break;
			
        case ECalendarTuesday:
			CalendarDayViewL( ETuesday );
			break;
			
        case ECalendarWednesday:
			CalendarDayViewL( EWednesday );
			break;
			
        case ECalendarThursday:
			CalendarDayViewL( EThursday );
			break;
			
        case ECalendarFriday:
			CalendarDayViewL( EFriday );
			break;
			
        case ECalendarSaturday:
			CalendarDayViewL( ESaturday );
			break;
			
        case ECalendarSunday:
			CalendarDayViewL( ESunday );
			break;
			
        case ECalendarThisWeek:
			CalendarWeekViewL( EThisWeek );
			break;
			
        case ECalendarNextWeek:
			CalendarWeekViewL( ENextWeek );
			break;
			
        case ECalendarThisMonth:
			CalendarMonthViewL( EThisMonth );
			break;
			
        case ECalendarNextMonth:
			CalendarMonthViewL( ENextMonth );
			break;
			
        case ECalendarJanuary:
			CalendarMonthViewL( EJanuary );
			break;
			
        case ECalendarFebruary:
			CalendarMonthViewL( EFebruary );
			break;
			
        case ECalendarMarch:
			CalendarMonthViewL( EMarch );
			break;
			
        case ECalendarApril:
			CalendarMonthViewL( EApril );
			break;
			
        case ECalendarMay:
			CalendarMonthViewL( EMay );
			break;
			
        case ECalendarJune:
			CalendarMonthViewL( EJune );
			break;
			
        case ECalendarJuly:
			CalendarMonthViewL( EJuly );
			break;
			
        case ECalendarAugust:
			CalendarMonthViewL( EAugust );
			break;
			
        case ECalendarSeptember:
			CalendarMonthViewL( ESeptember );
			break;
			
        case ECalendarOctober:
			CalendarMonthViewL( EOctober );
			break;
			
        case ECalendarNovember:
			CalendarMonthViewL( ENovember );
			break;
			
        case ECalendarDecember:
			CalendarMonthViewL( EDecember );
			break;
				
        case EStatusTime:
		    StatusTimeL();
		    break;
		
        case EStatusOperator:
		    StatusOperatorL();
		    break;
		    
        case EStatusBatteryStrength:
		    StatusBatteryL();
		    break;
		    
        case EStatusSignalStrength:
		    StatusSignalL();
		    break;
		    
        case EBrowser:
		    ActivateAppL( KUidAppBrowser );
		    break;
		    
        /*case ESMSReader:
            ActivateAppL( KUidAppSMSReader );
            break;*/
        
        case EMediaPlay:
            ExecuteMediaCommandL( EMediaPlay );
            break;
            
        case EMediaStop:
            ExecuteMediaCommandL( EMediaStop );
            break;
            
        case EMediaPause:
            ExecuteMediaCommandL( EMediaPause );
            break;
            
        case EMediaNextSong:
            ExecuteMediaCommandL( EMediaNextSong );
            break;
            
        case EMediaNextChannel:
            ExecuteMediaCommandL( EMediaNextChannel );
            break;
            
        case EMediaPreviousSong:
            ExecuteMediaCommandL( EMediaPreviousSong );
            break;
            
        case EMediaPreviousChannel:
            ExecuteMediaCommandL( EMediaPreviousChannel );
            break;
            
        case EMediaVolumeUp:
            ExecuteMediaCommandL( EMediaVolumeUp );
            break;
       		
       case EMediaVolumeDown:
            ExecuteMediaCommandL( EMediaVolumeDown );
            break;

       		
		default:
			err = KErrNotSupported;
			break;
		}
		
    return err;
    }

// ----------------------------------------------------------------------------
// CNssVCAppLauncher::CreateConnectionL
// Connects to RApaLsSession.
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::CreateApaConnectionL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::CreateApaConnectionL" );
    
    iApaLsSession = new RApaLsSession();
       
    if ( !iApaLsSession->Handle() )
        {
        User::LeaveIfError( iApaLsSession->Connect() );
        }
    }

// ----------------------------------------------------------------------------
// CNssVCAppLauncher::ActivateAppL
// Used to launch applications. Checks whether an instance of the launched
// application is already running.
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::ActivateAppL( const TUid aUid,
                                      const TDesC16* aCmdLine,
                                      TBool aMultipleInstances )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::ActivateAppL" );
    
    CreateApaConnectionL();

    if ( aMultipleInstances )
        {
        ActivateAppInstanceL( aUid );
        }
    else
        {
        // Session to window server
        RWsSession wsSession;
        User::LeaveIfError( wsSession.Connect() );
        
        TApaTaskList apaTaskList( wsSession );
        TApaTask apaTask = apaTaskList.FindApp( aUid );
        if ( apaTask.Exists() )
            {
            // Already running, close application
            apaTask.EndTask();
            }
        ActivateAppInstanceL( aUid, aCmdLine );
            
        wsSession.Close();
        }
    }

// ----------------------------------------------------------------------------
// CNssVCAppLauncher::ActivateAppInstanceL
// Activates an instance of an application.
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::ActivateAppInstanceL( const TUid aUid, const TDesC16* aCmdLine )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::ActivateAppInstanceL" );
    
	TApaAppInfo appInfo;
    User::LeaveIfError( iApaLsSession->GetAppInfo( appInfo, aUid ) );
	TFileName appName = appInfo.iFullName;
	CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();

	apaCommandLine->SetExecutableNameL( appName );
	apaCommandLine->SetCommandL( EApaCommandRun );
	if ( aCmdLine )
	    {
	    // copy the 16-bit data into 8-bit buffer
        HBufC8* paramBuf = HBufC8::NewLC( aCmdLine->Length() *2 );
        TPtr8 tailBuf = paramBuf->Des();
        tailBuf.Copy( reinterpret_cast<const TUint8*>( aCmdLine->Ptr() ), aCmdLine->Length() *2 );
	    
	    // Command line parameters
	    apaCommandLine->SetTailEndL( tailBuf );
	    
	    CleanupStack::PopAndDestroy( paramBuf );
	    }
	
	User::LeaveIfError ( iApaLsSession->StartApp( *apaCommandLine ) );
	CleanupStack::PopAndDestroy( apaCommandLine );
	}
    
// ----------------------------------------------------------------------------
// CNssVCAppLauncher::StatusTimeL
// Provides the answer to the voice command "What time is it?"
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::StatusTimeL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::StatusTimeL" );
    
    TTime time;
    time.HomeTime();
    
    HBufC* timeStatusString = NULL;
    
    TLocale locale;
    if ( locale.TimeFormat() == ETime24 )
        {
        // "It is %N0:%N1"
        GetLocalizedStringL( ETime24Format, timeStatusString );
        }
    else // 12-hour format
        {
        TAmPm timeOfDay;
        GetAmOrPmL( time, timeOfDay );
        if ( timeOfDay == EAm )
            {
            // "It is %0N:%1N AM"
            GetLocalizedStringL( ETimeAm, timeStatusString );
            }
        else
            {
            // "It is %0N:%1N PM"
            GetLocalizedStringL( ETimePm, timeStatusString );
            }
        }
    
    CleanupStack::PushL( timeStatusString );
    // Modify timeStatusString for TTime:FormatL
    TInt index( KErrNotFound );
    index = timeStatusString->Find( KLocFileHourString );
    if( index != KErrNotFound )
        {
        // Hour
    	if ( locale.TimeFormat() == ETime12 && 
                 ( ( time.DateTime().Hour() > 0 && time.DateTime().Hour() < 10 ) || 
                 ( time.DateTime().Hour() > 12 && time.DateTime().Hour() < 22 ) ) )
    	    {
    	    timeStatusString->Des().Replace( index, KLocFileHourString.iTypeLength,
                                             KTTimeHourStringWithZero );
        	}
        else
        	{
        	timeStatusString->Des().Replace( index, KLocFileHourString.iTypeLength,
                                                 KTTimeHourString );
        	}
        }
    index = timeStatusString->Find( KLocFileMinuteString );
    if( index != KErrNotFound )
        {
        // Minute
        timeStatusString->Des().Replace( index, KLocFileMinuteString.iTypeLength,
                                         KTTimeMinuteString );
        }
    iStatusInfo = HBufC::NewL( timeStatusString->Length() );
    TBuf<256> string;
    time.FormatL( string, *timeStatusString );
    iStatusInfo->Des().Append( string );
    CleanupStack::PopAndDestroy( timeStatusString );
    
    PlaybackStatusInfoL();
    }
    
// ----------------------------------------------------------------------------
// CNssVCAppLauncher::StatusOperatorL
// Provides the answer to the voice command "Who is my operator?"
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::StatusOperatorL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::StatusOperatorL" );
    
    // The operator
    TDesC* oper = NULL;
    HBufC* operStatusString = NULL;
    GetOperatorL( oper );
    
    if ( oper && oper->Length() > 0 )
        {
        CleanupStack::PushL( oper );
        // "Your operator is %U"
        GetLocalizedStringL( EOperator, operStatusString );
        CleanupStack::PushL( operStatusString );
        TInt index( -1 );
        index = operStatusString->Find( _L( "%U" ) );
        if( index > -1 )
           {
           operStatusString->Des().Replace( index, 2, _L( "%S" ) );
           }    
        iStatusInfo = HBufC::NewL( operStatusString->Length() * 2 ); // Just to be on the safe side
        iStatusInfo->Des().Format( operStatusString->Des(), oper );
        CleanupStack::PopAndDestroy( operStatusString );
        CleanupStack::PopAndDestroy( oper );
        }
    else
        {
        delete oper;
        // "Operator information not available"
        GetLocalizedStringL( EOperatorNotAvailable, operStatusString );
        CleanupStack::PushL( operStatusString );
        iStatusInfo = operStatusString->Des().Alloc();
        CleanupStack::PopAndDestroy( operStatusString );
        }
    PlaybackStatusInfoL();
    }
    
// ----------------------------------------------------------------------------
// CNssVCAppLauncher::StatusBatteryL
// Provides the answer to the voice command "What is the battery strength?"
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::StatusBatteryL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::StatusBatteryL" );
    
    TSpokenStatusInfo battStrength;
    GetBatteryStrengthL( battStrength );
    
    HBufC* battStatusString;
    
    switch ( battStrength )
        {
        case EBatteryFull:
            // "The battery is full"
            GetLocalizedStringL( EBatteryFull, battStatusString );
            iStatusInfo = battStatusString;
            break;
            
        case EBattetteryAlmostFull:
            // "The battery is almost full"
            GetLocalizedStringL( EBattetteryAlmostFull, battStatusString );
            iStatusInfo = battStatusString;
            break;
            
        case EBatteryHalfFull:
            // "The battery is half full"
            GetLocalizedStringL( EBatteryHalfFull, battStatusString );
            iStatusInfo = battStatusString;
            break;
            
        case EBatteryAlmostEmpty:
            // "The battery is almost empty"
            GetLocalizedStringL( EBatteryAlmostEmpty, battStatusString );
            iStatusInfo = battStatusString;
            break;
            
        case EBatteryLow:
            // "The battery is low. Please recharge"
            GetLocalizedStringL( EBatteryLow, battStatusString );
            iStatusInfo = battStatusString;
            break;
            
        case EBatteryStrengthNotAvailable:
            // "Battery strength information not available"
            GetLocalizedStringL( EBatteryStrengthNotAvailable, battStatusString );
            iStatusInfo = battStatusString;
            break;
        }
        
    PlaybackStatusInfoL();
    }
    
// ----------------------------------------------------------------------------
// CNssVCAppLauncher::StatusSignalL
// Provides the answer to the voice command "What is the signal strength?"
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::StatusSignalL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::StatusSignalL" );
    
    TSpokenStatusInfo signalStrength;
    GetSignalStrengthL( signalStrength );
    
    HBufC* signalStatusString;
    
    switch ( signalStrength )
        {
        case ESignalStrengthExcellent:
            // "The network strength is excellent"
            GetLocalizedStringL( ESignalStrengthExcellent, signalStatusString );
            iStatusInfo = signalStatusString;
            break;
            
        case ESignalStrengthGood:
            // "The network strength is good"
            GetLocalizedStringL( ESignalStrengthGood, signalStatusString );
            iStatusInfo = signalStatusString;
            break;
            
        case ESignalStrengthRatherGood:
            // "The network strength is rather good"
            GetLocalizedStringL( ESignalStrengthRatherGood, signalStatusString );
            iStatusInfo = signalStatusString;
            break;
            
        case ESignalStrengthLow:
            // "The network strength is low"
            GetLocalizedStringL( ESignalStrengthLow, signalStatusString );
            iStatusInfo = signalStatusString;
            break;
            
        case ESignalStrengthNoSignal:
            // "No network"
            GetLocalizedStringL( ESignalStrengthNoSignal, signalStatusString );
            iStatusInfo = signalStatusString;
            break;
            
        case ESignalStrengthNotAvailable:
            // "Signal strength information not available"
            GetLocalizedStringL( ESignalStrengthNotAvailable, signalStatusString );
            iStatusInfo = signalStatusString;
            break;
        }
        
    PlaybackStatusInfoL();
    }

// ----------------------------------------------------------------------------
// CNssVCAppLauncher::PlaybackStatusInfoL
// Performs text to speech answer for a status query.
// ----------------------------------------------------------------------------
//
void CNssVCAppLauncher::PlaybackStatusInfoL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::PlaybackStatusInfoL" );
    
    __ASSERT_ALWAYS( iStatusInfo,
                     User::Panic( KPanicCategory, KErrArgument ) );
                     
    RUBY_DEBUG1( "TTS text: \"%S\"", iStatusInfo );
    
    // TTS utility for playback
    iTts = CTtsUtility::NewL( *this );
    
    // Create TTS style with language + NLP on
    TTtsStyle style;
    style.iNlp = ETrue;
    style.iLanguage = User::Language();
    
    // Add style to TTS
    TTtsStyleID styleId = iTts->AddStyleL( style );
    
    // Segment
    TTtsSegment segment( styleId );
    
    // Parsed text
    iParsedText = CTtsParsedText::NewL();  
    iParsedText->SetTextL( *iStatusInfo );
    segment.SetTextPtr( iParsedText->Text() );
    iParsedText->AddSegmentL( segment );
        
    // Run synthesis
    iTts->OpenAndPlayParsedTextL( *iParsedText );
    
    if ( !iWait.IsStarted() )
        {
        iWait.Start();
        }
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::MapcPlayComplete
// Callback, playback has been initialized.
// -----------------------------------------------------------------------------
//
void CNssVCAppLauncher::MapcInitComplete( TInt aError,
                                          const TTimeIntervalMicroSeconds& /*aDuration*/ )
    {
    RUBY_DEBUG1( "CNssVCAppLauncher::MapcInitComplete [%d]", aError );
    
    if ( ( aError != KErrNone ) && ( iWait.IsStarted() ) )
        {
        iWait.AsyncStop();
        }
    }  
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::MapcPlayComplete
// Callback, playback has been completed.
// -----------------------------------------------------------------------------
//     
#if defined(_DEBUG) && !defined(__RUBY_DEBUG_DISABLED)
void CNssVCAppLauncher::MapcPlayComplete( TInt aError )
#else
void CNssVCAppLauncher::MapcPlayComplete( TInt /*aError*/ )
#endif
    {
    RUBY_DEBUG1( "NssVCAppLauncher::MapcPlayComplete [%d]", aError );         
          
    if ( iTts )
        {
        iTts->Stop();
        iTts->Close();
        }
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::GetAmOrPmL
// Tells whether it is am or pm.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::GetAmOrPmL( const TTime& aTime, TAmPm& aTod )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::GetAmOrPmL" );
    
    TTime time = aTime;
    // Add one day
    time += TTimeIntervalDays( 1 );
    // Copy to TDateTime object
    TDateTime dateTime = time.DateTime();
    // Set to midnight
    dateTime.SetHour( 0 );
    dateTime.SetMinute( 0 );
    dateTime.SetMicroSecond( 0 );
    // Copy to TTime object
    time = dateTime;
    
    TTimeIntervalHours hours;
    aTime.HoursFrom( time, hours );
    // Hours from midnight
    TInt hoursInt = hours.Int();
    if ( hours.Int() > -12 )
        {
        aTod = EPm;
        }
    else
        {
        aTod = EAm;
        }
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::GetOperatorL
// Gets network operator's name.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::GetOperatorL( TDesC*& aOperator )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::GetOperatorL" );
    
    TFileName tsyName;
    GetTSYNameL( tsyName );
    RTelServer server;
    User::LeaveIfError( server.Connect() );
    server.LoadPhoneModule( tsyName );
    
    RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg networkInfoPkg( networkInfo );
    RMobilePhone::TMobilePhoneLocationAreaV1 locationInfo;

    TRequestStatus reqStatus = KErrNone;
    RMobilePhone phone;
    RTelServer::TPhoneInfo info;
    User::LeaveIfError( server.GetPhoneInfo( 0, info ) );
    User::LeaveIfError( phone.Open( server, info.iName ) );
    phone.GetCurrentNetwork( reqStatus, networkInfoPkg, locationInfo );
    
    User::WaitForRequest( reqStatus );
    if( reqStatus == KErrNone )
        {
        TBuf<256> networkName;
        networkName.Copy( networkInfo.iLongName );
        HBufC* oper = networkName.AllocL();
        aOperator = oper;
        }
        
    phone.Close(); 
    server.UnloadPhoneModule( tsyName );
    server.Close();
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::GetBatteryStrengthL
// Gets the battery strength.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::GetBatteryStrengthL( TSpokenStatusInfo& aBattStrength )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::GetBatteryStrengthL" );
    
    aBattStrength = EBatteryStrengthNotAvailable;
    
    TFileName tsyName;
    GetTSYNameL( tsyName );
    RTelServer server;
    User::LeaveIfError( server.Connect() );
    server.LoadPhoneModule( tsyName );
    
    TRequestStatus reqStatus = KErrNone;
    RMobilePhone phone;
    RTelServer::TPhoneInfo info;
    User::LeaveIfError( server.GetPhoneInfo( 0, info ) );
    User::LeaveIfError( phone.Open( server, info.iName ) );
    
    TUint32 batteryCaps;
    // Test if battery strength can be obtained
    if ( phone.GetBatteryCaps( batteryCaps ) != KErrNone )
        {
        return;
        }

    TInt charge = -1;
    if ( batteryCaps & RMobilePhone::KCapsGetBatteryInfo )
        {
        RMobilePhone::TMobilePhoneBatteryInfoV1 mobilePhoneBatteryInfo;
        TRequestStatus status;
        phone.GetBatteryInfo( status, mobilePhoneBatteryInfo );
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );
        if ( mobilePhoneBatteryInfo.iStatus == RMobilePhone::EPoweredByBattery )
            {
            // The percentage battery charge level
            charge = mobilePhoneBatteryInfo.iChargeLevel;
            if ( charge >= KBatteryFull )
                {
                aBattStrength = EBatteryFull;
                }
            else if ( charge >= KBatteryAlmostFull )
                {
                aBattStrength = EBattetteryAlmostFull;
                }
            else if ( charge >= KBatteryHalfFull )
                {
                aBattStrength = EBatteryHalfFull;
                }
            else if ( charge >= KBatteryAlmostEmpty )
                {
                aBattStrength = EBatteryAlmostEmpty;
                }
            else
                {
                aBattStrength = EBatteryLow;
                }
            }
        }
        //else BATTERY INFO NOT AVAILABLE
        
    phone.Close(); 
    server.UnloadPhoneModule( tsyName );
    server.Close();
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::GetSignalStrengthL
// Gets the signal strength.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::GetSignalStrengthL( TSpokenStatusInfo& aSignalStrength )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::GetSignalStrengthL" );
    
    aSignalStrength = ESignalStrengthNotAvailable;
    
    TFileName tsyName;
    GetTSYNameL( tsyName );
    RTelServer server;
    User::LeaveIfError( server.Connect() );
    server.LoadPhoneModule( tsyName );

    RMobilePhone phone;
    RTelServer::TPhoneInfo info;
    User::LeaveIfError( server.GetPhoneInfo( 0, info ) );
    User::LeaveIfError( phone.Open( server, info.iName ) );
    
    TUint32 signalCaps;
    // Test if signal information can be obtained
    User::LeaveIfError( phone.GetSignalCaps( signalCaps ) );

    TInt32 signalStrength = -1;
    TInt8 bars = -1;
    if ( signalCaps & RMobilePhone::KCapsGetSignalStrength )
        {
        TRequestStatus status = KErrNone;
        phone.GetSignalStrength( status, signalStrength, bars );
        User::WaitForRequest( status );
        RUBY_DEBUG1( "Signal strength in dBm: %d", signalStrength );
        RUBY_DEBUG1( "Number of signal bars: %d", bars );
        if ( bars >= KSignalExcellent )
            {
            aSignalStrength = ESignalStrengthExcellent;
            }
        else if ( bars == KSignalGood )
            {
            aSignalStrength = ESignalStrengthGood;
            }
        else if ( bars >= KSignalRatherGood )
            {
            aSignalStrength = ESignalStrengthRatherGood;
            }
        else if ( bars >= KSignalLow )
            {
            aSignalStrength = ESignalStrengthLow;
            }
        else if ( bars == KSignalNone )
            {
            aSignalStrength = ESignalStrengthNoSignal;
            }
        }
    
    phone.Close(); 
    server.UnloadPhoneModule( tsyName );
    server.Close();
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::CalendarDayViewL
// Activates calendars day view.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::CalendarDayViewL( const TInt aDay )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::CalendarDayViewL" );
    
    __ASSERT_ALWAYS( aDay >= EMonday && aDay <= ETomorrow,
                     User::Panic( KPanicCategory, KErrArgument ) );
    
    TTime time;
    // Current date
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    // TTime::DayNoInMonth()'s first day in month is zero
    const TInt KCurrentDayInMonth = time.DayNoInMonth() + 1;
    const TInt KCurrentDayInWeek = time.DayNoInWeek();
    const TInt KCurrentMonth = dateTime.Month();
    const TInt KCurrentYear = dateTime.Year();
    // Variables used to form parameters for calendar launching
    TInt day = KCurrentDayInMonth;
    TInt month = KCurrentMonth;
    TInt year = KCurrentYear;
    
    // Command for weekday
    if( aDay >= EMonday && aDay <= ESunday )
        {
        if ( KCurrentDayInWeek <= aDay )
            {
            // Depend on existing enum values, not good
            day += aDay - KCurrentDayInWeek; 
            }
        else if ( KCurrentDayInWeek > aDay )
            {
            // Depend on existing enum values, not good
            // Go to monday of next week and add whatever is necessery
            day += KDaysInAWeek - KCurrentDayInWeek + aDay;
            }
        
        // Month changes
        if ( day > time.DaysInMonth() )
            {
            day -= time.DaysInMonth();
            // Year changes
            if ( month == EDecember )
                {
                month = EJanuary;
                year++;
                }
            else
                {
                month++;
                }
            }
        }
   // Command for tomorrow     
   else if ( aDay == ETomorrow )
        {
        day++;
        // Month changes
        if ( day > time.DaysInMonth() )
            {
            // First day
            day = 1;
            month++;
            
            // Year changes
            if ( KCurrentMonth == EDecember )
                {
                month = EJanuary;
                year++;
                }
            }
        }
    
    // Offset for TMonth is zero, so we add one
    month++; 
    
    HBufC16* cmdLine = HBufC16::NewLC( KCalendarDayCmdLine().Length() * 2 ); // Just to be on the safe side
    cmdLine->Des().Format( KCalendarDayCmdLine, year, month, day );
    
    // Launch calendar
    ActivateAppL( KUidAppCalendar, cmdLine );
    
    CleanupStack::PopAndDestroy( cmdLine );
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::CalendarWeekViewL
// Launches calendars week view to a selected week.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::CalendarWeekViewL( const TInt aWeek )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::CalendarWeekViewL" );
    
    __ASSERT_ALWAYS( aWeek == EThisWeek || aWeek == ENextWeek,
                     User::Panic( KPanicCategory, KErrArgument ) );
    
    TLocale locale;
    TTime time;
    // Current date
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    // TTime::DayNoInMonth()'s first day in month is zero
    const TInt KCurrentDayInMonth = time.DayNoInMonth() + 1;
    // According to documentation, TTime::DayNoInWeek() should honour the 
    // setting specified in TLocale::SetStartOfWeek(). It doesn't though.
    TInt currentDayInWeek;
    if( time.DayNoInWeek() >= locale.StartOfWeek() )
        {
        currentDayInWeek = time.DayNoInWeek() - locale.StartOfWeek() ;
        }
    else
        {
        currentDayInWeek = KDaysInAWeek - locale.StartOfWeek() + time.DayNoInWeek();
        }
    const TInt KCurrentDayInWeek = currentDayInWeek;
    const TInt KCurrentMonth = dateTime.Month();
    const TInt KCurrentYear = dateTime.Year();
    // Variables used to form parameters for calendar launching
    TInt year = KCurrentYear;
    TInt month = KCurrentMonth;
    TInt day = KCurrentDayInMonth;
    TInt hour = dateTime.Hour();
    
    if ( aWeek == ENextWeek )
        {
        // 8 am
        hour = 8;
        // Go to the first day of next week
        day += KDaysInAWeek - KCurrentDayInWeek;
        
        // Month changes
        if( day > time.DaysInMonth() )
            {
            day -= time.DaysInMonth();
            month++;
            
            // Year changes
            if ( KCurrentMonth == EDecember )
                {
                year++;
                month = EJanuary;
                }
            }
        }
        
    // Offset for TMonth is zero, so we add one
    month++;
    
    HBufC16* cmdLine = HBufC16::NewLC( KCalendarWeekCmdLine().Length() * 2 ); // Just to be on the safe side
    cmdLine->Des().Format( KCalendarWeekCmdLine, year, month, day, hour );
    
    // Launch calendar
    ActivateAppL( KUidAppCalendar, cmdLine );
    
    CleanupStack::PopAndDestroy( cmdLine );
    }
        
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::CalendarMonthViewL
// Launches calendars month view to a selected month.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::CalendarMonthViewL( const TInt aMonth )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::CalendarMonthViewL" );
    
    __ASSERT_ALWAYS( aMonth >= EMonday && aMonth <= ENextMonth,
                     User::Panic( KPanicCategory, KErrArgument ) );
                     
    TTime time;
    // Current date
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    const TInt KCurrentMonth = dateTime.Month();
    // Variables used to form parameters for calendar launching
    TInt year = dateTime.Year();
    TInt month = KCurrentMonth;
    // TDateTime::Day() offset is zero
    TInt day = dateTime.Day() + 1;
    
    if ( aMonth >= EJanuary && aMonth <= EDecember )
        {
        if( aMonth != KCurrentMonth )
            {
            day = 1;
            }
        month = aMonth;
        // If the requested month is already past for this year
        // month view is openend for next year. 
        if ( aMonth < KCurrentMonth )
            {
            year++;
            }
        }
    else if ( aMonth == ENextMonth )
        {
        day = 1;
        if ( KCurrentMonth == EDecember )
            {
            year++;
            month = EJanuary;
            }
        else
            {
            month++;
            }
        }
        
    // Offset for TMonth is zero, so we add one
    month++;    
    
    HBufC16* cmdLine = HBufC16::NewLC( KCalendarMonthCmdLine().Length() * 2 ); // Just to be on the safe side
    cmdLine->Des().Format( KCalendarMonthCmdLine, year, month, day );
    
    // Launch calendar
    ActivateAppL( KUidAppCalendar, cmdLine );
    
    CleanupStack::PopAndDestroy( cmdLine );
    }

// -----------------------------------------------------------------------------
// CNssVCAppLauncher::GetOpenHomepageL
// Opens browser with the user defined homepage.
// ----------------------------------------------------------------------------- 
void CNssVCAppLauncher::OpenHomepageL()
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::OpenHomepageL" );
    
    // Command line parameter for starting the browser with start page
    HBufC16* cmdLine = KBrowserArgStartPage().AllocLC();
    
    // Launch browser
    ActivateAppL( KUidAppBrowser, cmdLine );
    
    CleanupStack::PopAndDestroy( cmdLine );
    }

// -----------------------------------------------------------------------------
// CNssVCAppLauncher::GetTSYNameL
// Gets the name of the currently selected TSY.
// -----------------------------------------------------------------------------    
void CNssVCAppLauncher::GetTSYNameL( TDes& aTSYName )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::GetTSYNameL" );
    
    CCommsDatabase* const db = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( db );
    TUint32 modemId = 0;
    db->GetGlobalSettingL( TPtrC( MODEM_PHONE_SERVICES_SMS ), modemId );
    CCommsDbTableView* const view
        = db->OpenViewMatchingUintLC( TPtrC( MODEM ), TPtrC( COMMDB_ID ),
                                      modemId );
    TInt err = view->GotoFirstRecord();
    User::LeaveIfError( err );
    view->ReadTextL( TPtrC( MODEM_TSY_NAME ), aTSYName );
    CleanupStack::PopAndDestroy( view );
    CleanupStack::PopAndDestroy( db );
    }
    
// -----------------------------------------------------------------------------
// CNssVCAppLauncher::ExecuteMediaCommandL
// Executes a media command using Symbian Remote Control FW.
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::ExecuteMediaCommandL( TUint aMediaCommandId )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::ExecuteMediaCommandL" );
    
    __ASSERT_ALWAYS( aMediaCommandId >= EMediaPlay &&
                     aMediaCommandId <= EMediaVolumeDown,
                     User::Panic( KPanicCategory, KErrArgument ) );
    
    CRemConInterfaceSelector* interfaceSelector
        = CRemConInterfaceSelector::NewL();
    
    // Owned by interfaceSelector
    CRemConCoreApiController* coreIf
        = CRemConCoreApiController::NewL( *interfaceSelector, *this );
    
    interfaceSelector->OpenControllerL();
    
    TRemConAddress addr;
    addr.BearerUid() = KUidVCExecutorBearer;
    // No bearer-specific connection data
    interfaceSelector->GoConnectionOrientedL( addr );
        
    TRequestStatus status;
    
    interfaceSelector->ConnectBearer( status );
    User::WaitForRequest( status );    
    
    TUint numRemotes;
    
    switch ( aMediaCommandId )
        {
        // TODO: remove User::After calls. They are for testing purposes only.
        case EMediaPlay:
            coreIf->PausePlayFunction( status, numRemotes,
                                       ERemConCoreApiButtonPress );
            User::WaitForRequest( status );
            break;
         
        case EMediaPause:
            coreIf->Pause( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
        
        case EMediaStop:
            coreIf->Stop( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;    
       
        case EMediaNextSong:
            coreIf->Forward( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
            
        case EMediaNextChannel:
            coreIf->Forward( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
            
        case EMediaPreviousSong:
            coreIf->Backward( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
            
        case EMediaPreviousChannel:
            coreIf->Backward( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
            
        case EMediaChannelUp:
            coreIf->ChannelUp( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
            
        case EMediaChannelDown:
            coreIf->ChannelDown( status, numRemotes,
                                 ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
            
        case EMediaVolumeUp:
            coreIf->VolumeUp( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
            
        case EMediaVolumeDown:
            coreIf->VolumeDown( status, numRemotes, ERemConCoreApiButtonClick );
            User::WaitForRequest( status );
            break;
        }
    
    delete interfaceSelector;
    interfaceSelector = NULL;
    }

// -----------------------------------------------------------------------------
// CNssVCAppLauncher::MrccacoResponse
// Called by RemCon as a response for a remote control command sent by this
// RemCon controller client
// -----------------------------------------------------------------------------
void CNssVCAppLauncher::MrccacoResponse( TRemConCoreApiOperationId /*aOperationId*/,
                                         TInt /*aError*/ )
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CNssVCAppLauncher::GetLocalizedStringL
// Gets localized string from the resource file for text-to-speech status info.
// -----------------------------------------------------------------------------    
    
void CNssVCAppLauncher::GetLocalizedStringL( TSpokenStatusInfo aStringId,
                                             HBufC*& aString )
    {
    RUBY_DEBUG_BLOCKL( "CNssVCAppLauncher::GetLocalizedStringL" );
    
    __ASSERT_ALWAYS( aStringId >= ETime24Format &&
                     aStringId <= ESignalStrengthNotAvailable, 
                     User::Panic( KPanicCategory, KErrArgument ) );
    
    // load resources
    RResourceFile resourceFile;
    RFs fileServer;
    TBool found( EFalse );
    User::LeaveIfError( fileServer.Connect() );
    CleanupClosePushL( fileServer );
    
    // try finding a localized or default resource file browsing through the drivers 
    TFileName name;
    TInt i( 0 );
    // use the first drive
    name.Append( KResourceDrivers[i] );
    name.Append( ':' );
    name.Append( KResourceDir );
    name.Append( KResourceFileName );

    while ( !found && i < KResourceDrivers.Length() )
        {
        name[0] = KResourceDrivers[i++];

        BaflUtils::NearestLanguageFile( fileServer, name );
       
        if ( BaflUtils::FileExists(fileServer, name) )
            {
            // open resource
            resourceFile.OpenL( fileServer, name );
            CleanupClosePushL( resourceFile );
            resourceFile.ConfirmSignatureL();
            found = ETrue;
            }
        }

    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
        
    TResourceReader reader;
    
    // Get correct localized string    
    switch ( aStringId )
        {
        case ETime24Format:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_TIME24 ) );
        break;
            
        case ETimeAm:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_TIME12AM ) );
        break;
            
        case ETimePm:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_TIME12PM ) );
        break;
            
        case EOperator:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_OPERATOR ) );
        break;
        
        case EOperatorNotAvailable:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NOOPERATOR ) );
        break;
         
        case EBatteryFull:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_BATTERYFULL ) );
        break;
        
        case EBattetteryAlmostFull:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_BATTERYALMOSTFULL ) );
        break;
        
        case EBatteryHalfFull:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_BATTERYHALF ) );
        break;
        
        case EBatteryAlmostEmpty:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_BATTERYALMOSTEMPTY ) );
        break;
        
        case EBatteryLow:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_BATTERYLOW ) );
        break;
        
        case EBatteryStrengthNotAvailable:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NOBATTERY ) );
        break;
        
        case ESignalStrengthExcellent:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NETWORKEXCELLENT ) );
        break;
            
        case ESignalStrengthGood:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NETWORKGOOD ) );
        break;
        
        case ESignalStrengthRatherGood:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NETWORKRATHERGOOD ) );
        break;
        
        case ESignalStrengthLow:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NETWORKLOW ) );
        break;
        
        case ESignalStrengthNoSignal:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NETWORKNONE ) );
        break;
        
        case ESignalStrengthNotAvailable:
        reader.SetBuffer( resourceFile.AllocReadLC( PROMPT_NONETWORK ) );
        break;
        
        default:
            break;

        }
            
    aString = reader.ReadHBufCL();
    
    CleanupStack::PopAndDestroy(); // AllocReadLC
    
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( &fileServer );
    }

// End of File
