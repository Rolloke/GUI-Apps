/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: SavicProfile.h $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/SavicProfile.h $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 11:42 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef __SAVICPROFILE_H__
#define __SAVICPROFILE_H__

/////////////////////////////////////////////////////////////////////////////
inline DWORD WriteConfigurationInt(CString sSection, CString sEntry, int nVal, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteInt(sFileName+"\\"+sSection, sEntry, nVal);
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL WriteConfigurationString(CString sSection, CString sEntry, CString sString, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteString(sFileName+"\\"+sSection, sEntry, sString);
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD ReadConfigurationInt(CString sSection, CString sEntry, int nDefault, CString sFileName)
{
	CWK_Profile prof;
	int  nRet = prof.GetInt(sFileName+"\\"+sSection, sEntry, nDefault);
	prof.WriteInt(sFileName+"\\"+sSection, sEntry, nRet);

	return (DWORD)nRet;
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD ReadConfigurationString(CString sSection, CString sEntry, CString sDefault, LPTSTR strDest, int nSize, CString sFileName)
{
	CWK_Profile prof;
	CString sRet = prof.GetString(sFileName+_T("\\")+sSection, sEntry, sDefault);
	DWORD dwRet = sRet.GetLength();
	ZeroMemory(strDest, nSize); 
	memcpy(strDest, LPCTSTR(sRet), min(dwRet, (DWORD)nSize)*sizeof(_TCHAR));
	if (sRet != _T(""))
		prof.WriteString(sFileName+_T("\\")+sSection, sEntry, strDest);

	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL DeleteEntry(CString sSection, CString sEntry, CString sFileName)
{
	CWK_Profile prof;
	return prof.DeleteEntry(sFileName+_T("\\")+sSection, sEntry);
}
#endif // __SAVICPROFILE_H__
