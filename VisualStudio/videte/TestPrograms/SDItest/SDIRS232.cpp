///////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIRS232.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/SDIRS232.cpp $
// CHECKIN:		$Date: 29.03.99 16:57 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 29.03.99 16:56 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"

//#include "ascii.h"
//#include "messages.h"
//#include "wk_profile.h"
//#include "WK_Trace.h"
#include "SDITest.h"
#include "Afxmt.h"
#include "SDIRS232.h"

extern CSDITestApp theApp;

//////////////////////////////////////////////////////////////////////
CSDIRS232::CSDIRS232(int iCom,
					 BOOL bTraceAscii,
					 BOOL bTraceHex,
					 BOOL bTraceEvents)
	: CWK_RS232(iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
}
//////////////////////////////////////////////////////////////////////
CSDIRS232::~CSDIRS232()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIRS232::OnTimeOutTransparent()
{
	// Nichts zu tun
}
/////////////////////////////////////////////////////////////////////////////
void CSDIRS232::SwitchToConfigMode(const CString& sPath, BOOL bReceive)
{
	// Nichts zu tun
}
/////////////////////////////////////////////////////////////////////////////
void CSDIRS232::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	// Daten werden ignoriert
}