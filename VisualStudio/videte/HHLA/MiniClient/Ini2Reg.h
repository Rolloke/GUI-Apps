/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiniClient
// FILE:		$Workfile: Ini2Reg.h $
// ARCHIVE:		$Archive: /Project/HHLA/MiniClient/Ini2Reg.h $
// CHECKIN:		$Date: 25.02.04 14:43 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 25.02.04 14:37 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "WK_Profile.h"

#ifndef __INI2REG_H__
#define __INI2REG_H__

/////////////////////////////////////////////////////////////////////////////
inline DWORD MyGetPrivateProfileInt(CString sSection, CString sEntry, int nDefault, CString sFileName)
{
	CWK_Profile prof;
	DWORD dwRet = prof.GetInt(sFileName+_T("\\")+sSection, sEntry, nDefault);
	if (dwRet != -1)
		prof.WriteInt(sFileName+_T("\\")+sSection, sEntry, dwRet);

	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD MyGetPrivateProfileString(CString sSection, CString sEntry, CString sDefault, LPTSTR strDest, int nSize, CString sFileName)
{
	CWK_Profile prof;
	CString sRet = prof.GetString(sFileName+_T("\\")+sSection, sEntry, sDefault);
	DWORD dwRet = sRet.GetLength();
#ifdef _UNICODE
	wmemset(strDest, 0, nSize); 
	wmemcpy(strDest, (LPCTSTR)sRet, min(dwRet, (DWORD)nSize));
#else
	memset(strDest, 0, nSize); 
	memcpy(strDest, (LPCTSTR)sRet, min(dwRet, (DWORD)nSize));
#endif
	if (sRet != _T(""))
		prof.WriteString(sFileName+_T("\\")+sSection, sEntry, strDest);

	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////
inline MyWritePrivateProfileString(CString sSection, CString sEntry, CString sString, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteString(sFileName+_T("\\")+sSection, sEntry, sString);
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD MyWritePrivateProfileInt(CString sSection, CString sEntry, int nVal, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteInt(sFileName+_T("\\")+sSection, sEntry, nVal);
}

#endif // __INI2REG_H__