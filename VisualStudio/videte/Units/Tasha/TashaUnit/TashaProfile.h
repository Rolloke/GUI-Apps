/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: TashaProfile.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/TashaProfile.h $
// CHECKIN:		$Date: 16.02.04 9:37 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 16.02.04 9:37 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef __TASHAPROFILE_H__
#define __TASHAPROFILE_H__

/////////////////////////////////////////////////////////////////////////////
inline DWORD WriteConfigurationInt(CString sSection, CString sEntry, int nVal, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteInt(sFileName+_T("\\")+sSection, sEntry, nVal);
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL WriteConfigurationString(CString sSection, CString sEntry, CString sString, CString sFileName)
{
	CWK_Profile prof;
	return prof.WriteString(sFileName+_T("\\")+sSection, sEntry, sString);
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL WriteConfigurationBinary(CString sSection, CString sEntry, BYTE* pData, WORD wLen, CString sFileName)
{
	BOOL bResult = FALSE;

	if ((pData != NULL) && (wLen < 4096))
	{
		CWK_Profile prof;
		bResult = prof.WriteBinary(sFileName+_T("\\")+sSection, sEntry, pData, wLen);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD ReadConfigurationInt(CString sSection, CString sEntry, int nDefault, CString sFileName)
{
	CWK_Profile prof;
	int  nRet = prof.GetInt(sFileName+_T("\\")+sSection, sEntry, nDefault);
	prof.WriteInt(sFileName+_T("\\")+sSection, sEntry, nRet);

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
inline BOOL ReadConfigurationBinary(CString sSection, CString sEntry, BYTE** pData, UINT* pLen, CString sFileName)
{
	BOOL bResult = FALSE;
	CWK_Profile prof;
	if (prof.GetBinary(sFileName+_T("\\")+sSection, sEntry, pData, pLen))
		bResult = prof.WriteBinary(sFileName+_T("\\")+sSection, sEntry, *pData, *pLen);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL DeleteEntry(CString sSection, CString sEntry, CString sFileName)
{
	CWK_Profile prof;
	return prof.DeleteEntry(sFileName+_T("\\")+sSection, sEntry);
}
#endif // __TASHAPROFILE_H__
