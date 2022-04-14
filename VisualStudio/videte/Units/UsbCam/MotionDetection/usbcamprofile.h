/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: usbcamprofile.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/MotionDetection/usbcamprofile.h $
// CHECKIN:		$Date: 14.09.04 11:41 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 14.09.04 10:51 $
// BY AUTHOR:	$Martin Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef __USBCAMPROFILE_H__
#define __USBCAMPROFILE_H__

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
	DWORD dwRet = sRet.GetLength()*sizeof(_TCHAR);
	ZeroMemory(strDest, nSize); 
	memcpy(strDest, (LPCTSTR)sRet, min(dwRet, (DWORD)nSize));
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
#endif // __USBCAMPROFILE_H__
