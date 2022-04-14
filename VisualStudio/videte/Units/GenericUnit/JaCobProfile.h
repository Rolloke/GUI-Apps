/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: JaCobProfile.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/JaCobProfile.h $
// CHECKIN:		$Date: 11.07.01 13:24 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.07.00 7:51 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef __JACOBPROFILE_H__
#define __JACOBPROFILE_H__

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
	CString sRet = prof.GetString(sFileName+"\\"+sSection, sEntry, sDefault);
	DWORD dwRet = sRet.GetLength();
	ZeroMemory(strDest, nSize); 
	memcpy(strDest, (LPCSTR)sRet, min(dwRet, (DWORD)nSize));
	if (sRet != "")
		prof.WriteString(sFileName+"\\"+sSection, sEntry, strDest);

	return dwRet;
}


#endif // __JACOBPROFILE_H__
