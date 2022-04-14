/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: Ini2Reg.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/Ini2Reg.h $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 27.02.04 9:02 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "WK_Profile.h"

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
	CString sRet = prof.GetString(sFileName+"\\"+sSection, sEntry, sDefault);
	DWORD dwRet = sRet.GetLength();
	memset(strDest, 0, nSize); 
	memcpy(strDest, (LPCTSTR)sRet, min(dwRet, (DWORD)nSize));
	if (sRet != "")
		prof.WriteString(sFileName+"\\"+sSection, sEntry, strDest);

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
