// RunService.cpp: implementation of the CRunService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SecurityLauncher.h"
#include "RunService.h"
#include "Application.h"

#define SIZEOFARRAY( x ) (sizeof(x)/sizeof(x[0]))
static TCHAR szRun[] = "SOFTWARE\\MicroSoft\\Windows\\CurrentVersion\\Run";
static TCHAR szRunOnce[] = "SOFTWARE\\MicroSoft\\Windows\\CurrentVersion\\RunOnce";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRunService::CRunService()
{

}
//////////////////////////////////////////////////////////////////////
CRunService::~CRunService()
{

}
//////////////////////////////////////////////////////////////////////
void CRunService::RunServices()
{
	HKEY hKey;
	LONG lResult;

    DWORD regEnumIndex = 0;
    TCHAR szValueName[ MAX_PATH];
    DWORD cbValueNameSize = SIZEOFARRAY(szValueName);
    TCHAR szValue[ MAX_PATH];
    DWORD cbValueSize = SIZEOFARRAY(szValue);
    DWORD type;
	CString sValue;
	
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRun, 0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		while ( TRUE )
		{
			cbValueNameSize = SIZEOFARRAY(szValueName);
			cbValueSize = SIZEOFARRAY(szValue);
			lResult = RegEnumValue(  hKey, regEnumIndex++,
									szValueName, &cbValueNameSize,
									0, &type, (LPBYTE)szValue, &cbValueSize );
                            
			if ( ERROR_NO_MORE_ITEMS == lResult )
			{
				break;
			}

			if (REG_SZ == type)
			{
				// 	szValueName, szValue
				sValue = szValue;
				WK_TRACE("running service %s\n",sValue);
				WinExec(sValue,SW_SHOWMINNOACTIVE);
			}
		}

		RegCloseKey(hKey);
		hKey = NULL;
	}
	else
	{
		TRACE("cannot open run key %s because %s\n",szRun,GetLastErrorString(lResult));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CRunService::WriteRunOnce(const CString& sKey,const CString& sValue)
{
	return WriteEntry(szRunOnce,sKey,sKey);
}
//////////////////////////////////////////////////////////////////////
BOOL CRunService::WriteRun(const CString& sKey,const CString& sValue)
{
	return WriteEntry(szRun,sKey,sValue);
}
//////////////////////////////////////////////////////////////////////
BOOL CRunService::WriteEntry(const CString& sSection, const CString& sKey,const CString& sValue)
{
	BOOL bRet = FALSE;
	HKEY hKey = NULL;
	LONG lResult;

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSection, 0, KEY_ALL_ACCESS, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegSetValueEx(hKey, sKey, NULL, REG_SZ,
			(LPBYTE)(const char*)sValue, (lstrlen(sValue)+1)*sizeof(TCHAR));
		if (lResult != ERROR_SUCCESS)
		{
			WK_TRACE("cannot write REG_SZ Value %s because %s",
				sValue,GetLastErrorString(lResult));
		}
		else
		{
			bRet = TRUE;
		}

		RegCloseKey(hKey);
		hKey = NULL;
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CRunService::ClearRun(const CString& sKey)
{
	return ClearEntry(szRun,sKey);
}
//////////////////////////////////////////////////////////////////////
BOOL CRunService::ClearRunOnce(const CString& sKey)
{
	return ClearEntry(szRunOnce,sKey);
}
//////////////////////////////////////////////////////////////////////
BOOL CRunService::ClearEntry(const CString& sSection, const CString& sKey)
{
	BOOL bRet = FALSE;
	HKEY hKey = NULL;
	LONG lResult;

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSection, 0, KEY_ALL_ACCESS, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegDeleteValue(hKey, sKey);
		if (lResult != ERROR_SUCCESS)
		{
			WK_TRACE("cannot delete REG_SZ Key %s because %s",
				sKey,GetLastErrorString(lResult));
		}
		else
		{
			bRet = TRUE;
		}

		RegCloseKey(hKey);
		hKey = NULL;
	}

	return bRet;
}
