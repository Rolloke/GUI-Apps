/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Watchdog.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Watchdog.h $
// CHECKIN:		$Date: 2.02.05 12:35 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 2.02.05 10:24 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////


#define PF0 	0x0001
#define PF1 	0x0002
#define PF2 	0x0004
#define PF3 	0x0008
#define PF4 	0x0010
#define PF5 	0x0020
#define PF6 	0x0040
#define PF7 	0x0080
#define PF8 	0x0100
#define PF9 	0x0200
#define PF10 	0x0400
#define PF11 	0x0800
#define PF12 	0x1000
#define PF13 	0x2000
#define PF14 	0x4000
#define PF15 	0x8000

#define PF_WATCHDOG		PF11
//#define PF_WATCHDOG		PF8

BOOL TriggerWatchdog(int nTimeOut);
BOOL InitWatchdogTimer(int nFreq);
BOOL EnableWatchdog(int nTimeOut);
BOOL DisableWatchdog();
EX_INTERRUPT_HANDLER(WatchdogTimerISR);
