/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: TashaTrace.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/TashaTrace.cpp $
// CHECKIN:		$Date: 2.09.04 9:38 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 2.09.04 9:30 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaTrace.h"
//#include "CCodec.h"
#include "CDebug.h"

const CDebug*	g_pDebug = NULL;

///////////////////////////////////////////////////////////////////////////
void  TashaDllInitDebugger(const CDebug* const pDebug)
{
	g_pDebug = pDebug;
}					  

///////////////////////////////////////////////////////////////////////////
void ML_TRACE(LPCTSTR lpszFormat, ...)
{
	_TCHAR szString[MAX_DEBUG_STRING_LEN] = {0};
	szString[0]=0;
	LPCTSTR lpszLocalFormat = lpszFormat;

	va_list args;
	va_start(args, lpszFormat);

	_vsntprintf(szString, MAX_DEBUG_STRING_LEN-1, lpszLocalFormat, args);
	CString s(szString);
	
	if (g_pDebug)
		g_pDebug->OnReceivedMessage(s);
}

///////////////////////////////////////////////////////////////////////////
void ML_TRACE_DIAGNOSTIC(LPCTSTR lpszFormat, ...)
{
	_TCHAR szString[MAX_DEBUG_STRING_LEN] = {0};
	szString[0]=0;
	LPCTSTR lpszLocalFormat = lpszFormat;

	va_list args;
	va_start(args, lpszFormat);

	_vsntprintf(szString, MAX_DEBUG_STRING_LEN-1, lpszLocalFormat, args);
	CString s(szString);
	
	if (g_pDebug)
		g_pDebug->OnReceivedDiagnosticMessage(s);
}

///////////////////////////////////////////////////////////////////////////
void ML_TRACE_ERROR(LPCTSTR lpszFormat, ...)
{
	_TCHAR szString[MAX_DEBUG_STRING_LEN] = {0};
	szString[0]=0;
	LPCTSTR lpszLocalFormat = lpszFormat;

	va_list args;
	va_start(args, lpszFormat);

	_vsntprintf(szString, MAX_DEBUG_STRING_LEN-1, lpszLocalFormat, args);
//	CString s(szString);

	if (g_pDebug)
		g_pDebug->OnReceivedError(szString);
}

///////////////////////////////////////////////////////////////////////////
void ML_TRACE_WARNING( LPCTSTR lpszFormat, ...)
{
	_TCHAR szString[MAX_DEBUG_STRING_LEN] = {0};
	szString[0]=0;
	LPCTSTR lpszLocalFormat = lpszFormat;

	va_list args;
	va_start(args, lpszFormat);

	_vsntprintf(szString, MAX_DEBUG_STRING_LEN-1, lpszLocalFormat, args);

	if (g_pDebug)
		g_pDebug->OnReceivedWarning(szString);
}

///////////////////////////////////////////////////////////////////////////
void ML_STAT_LOG(LPCTSTR szTopic,int iValue,LPCTSTR sName)
{
	if (g_pDebug)
		g_pDebug->OnReceivedStatLog(szTopic, iValue, sName);
}

///////////////////////////////////////////////////////////////////////////
void ML_STAT_LOG(int /*iUnusedFileIndex*/,LPCTSTR szTopic,int iValue,LPCTSTR sName)
{
	ML_STAT_LOG(szTopic,iValue,sName);
}

///////////////////////////////////////////////////////////////////////////
void ML_STAT_PEAK(LPCTSTR szTopic,int iValue,LPCTSTR sName)
{
	if (g_pDebug)
		g_pDebug->OnReceivedStatPeak(szTopic, iValue, sName);
}


