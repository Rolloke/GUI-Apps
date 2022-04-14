/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: SavicTrace.h $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/SavicTrace.h $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 11:38 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __ML_TRACE_H__
#define __ML_TRACE_H__

class CDebug;

AFX_EXT_CLASS void  SaVicDllInitDebugger(const CDebug* const pDebug);

#define MAX_DEBUG_STRING_LEN 500

void ML_TRACE(LPCTSTR lpszFormat, ...);
void ML_TRACE_DIAGNOSTIC(LPCTSTR lpszFormat, ...);
void ML_TRACE_ERROR(LPCTSTR lpszFormat, ...);
void ML_TRACE_WARNING(LPCTSTR lpszFormat, ...);
void ML_TRACE_USER(LPCTSTR lpszFormat, ...);

// creates log entries for SecAnalyzer
void ML_STAT_LOG(LPCTSTR szTopic,  int iValue, LPCTSTR sName);
void ML_STAT_PEAK(LPCTSTR szTopic, int iValue, LPCTSTR sName);
///////////////////////////////////////////////////////////////////////////
DWORD GET_SAVIC_STAT_TICK_COUNT();
void  SET_SAVIC_STAT_TICK_COUNT(DWORD dwTick);

#endif // __ML_TRACE_H__
