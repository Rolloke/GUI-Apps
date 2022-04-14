/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: TashaTrace.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/TashaTrace.h $
// CHECKIN:		$Date: 2.09.04 9:38 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 2.09.04 9:06 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __ML_TRACE_H__
#define __ML_TRACE_H__

class CDebug;

AFX_EXT_CLASS void  TashaDllInitDebugger(const CDebug* const pDebug);

#define MAX_DEBUG_STRING_LEN 500

void ML_TRACE(LPCTSTR lpszFormat, ...);
void ML_TRACE_DIAGNOSTIC(LPCTSTR lpszFormat, ...);
void ML_TRACE_ERROR(LPCTSTR lpszFormat, ...);
void ML_TRACE_WARNING(LPCTSTR lpszFormat, ...);
void ML_TRACE_USER(LPCTSTR lpszFormat, ...);

// creates log entries for SecAnalyzer
void ML_STAT_LOG(LPCTSTR szTopic,int iValue,LPCTSTR sName);
void ML_STAT_PEAK(LPCTSTR szTopic,int iValue,LPCTSTR sName);
///////////////////////////////////////////////////////////////////////////
DWORD GET_SAVIC_STAT_TICK_COUNT();
void  SET_SAVIC_STAT_TICK_COUNT(DWORD dwTick);

#endif // __ML_TRACE_H__
