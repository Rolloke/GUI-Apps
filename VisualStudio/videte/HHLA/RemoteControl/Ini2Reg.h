/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiniClient
// FILE:		$Workfile: Ini2Reg.h $
// ARCHIVE:		$Archive: /Project/HHLA/RemoteControl/Ini2Reg.h $
// CHECKIN:		$Date: 19.09.05 12:46 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 19.09.05 10:01 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "WK_Profile.h"

#ifndef __INI2REG_H__
#define __INI2REG_H__

/////////////////////////////////////////////////////////////////////////////
inline DWORD MyGetPrivateProfileInt(CString sSection, CString sEntry, int nDefault, CString sFileName)
{
	CWK_Profile prof;
	DWORD dwRet = prof.GetInt(sFileName+"\\"+sSection, sEntry, nDefault);
	if (dwRet != -1)
		prof.WriteInt(sFileName+"\\"+sSection, sEntry, dwRet);

	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD MyGetPrivateProfileString(CString sSection, CString sEntry, CString sDefault, LPTSTR strDest, int nSize, CString sFileName)
{
	CWK_Profile prof;
	CString sRet = prof.GetString(sFileName+_T("\\")+sSection, sEntry, sDefault);
	DWORD dwRet = sRet.GetLength();
	nSize *= sizeof(_TCHAR);
	ZeroMemory(strDest, nSize); 
	memcpy(strDest, (LPCTSTR)sRet, min(dwRet*sizeof(_TCHAR), (DWORD)nSize));
	if (sRet != _T(""))
		prof.WriteString(sFileName+_T("\\")+sSection, sEntry, strDest);

	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////
inline MyWritePrivateProfileString(CString sSection, CString sEntry, CString sString, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteString(sFileName+"\\"+sSection, sEntry, sString);
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD MyWritePrivateProfileInt(CString sSection, CString sEntry, int nVal, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteInt(sFileName+"\\"+sSection, sEntry, nVal);
}

#endif // __INI2REG_H__