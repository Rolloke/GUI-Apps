/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Crosspoint.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Crosspoint.h $
// CHECKIN:		$Date: 27.01.04 15:19 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 27.01.04 9:12 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#define SPI0_CROSSPOINT_BAUDRATE	1000000

BOOL EnableCrosspoint();
BOOL DisableCrosspoint();
BOOL GetCrosspointEnableState(BOOL& bState);

BOOL SetCrosspoint(DWORD dwCrosspointMask);
BOOL GetCrosspoint(DWORD& dwCrosspointMask);
BOOL WaitUntilSPI0TransferComplete(int nTimeOut);

