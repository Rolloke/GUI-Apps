// CProfile.cpp: implementation of the CProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProfile::CProfile(const CString &sSection)
{
	m_hRootKey	= HKEY_LOCAL_MACHINE;
	m_sSection	= sSection;
}

CProfile::~CProfile()
{

}

///////////////////////////////////////////////////////////////////////////////////
BOOL CProfile::ReadProfileFlag(const CString &sEntry, BOOL bDefault)
{
    HKEY	hKey	= NULL;
    DWORD	dwType	= 0;
    BOOL	bValue	= bDefault;
    DWORD	dwData	= 0;
    DWORD	dwSize	= 0;
	
	if (RegOpenKey(m_hRootKey, m_sSection, &hKey) != ERROR_SUCCESS)
        return(bDefault);

    dwSize = sizeof(dwData);
    if (RegQueryValueEx(hKey, sEntry, NULL, &dwType, (PBYTE)&dwData, &dwSize) == ERROR_SUCCESS)
	{
		if (dwType == REG_DWORD)
		{
			if (dwData)
				bValue = TRUE;
			else
				bValue = FALSE;
		}
    }

    RegCloseKey(hKey);

    return bValue;
}


///////////////////////////////////////////////////////////////////////////////////
void CProfile::WriteProfileFlag(const CString &sEntry, BOOL bValue)
{
    HKEY hKey		= NULL;

    if (RegCreateKey(m_hRootKey, m_sSection, &hKey) == ERROR_SUCCESS)
	{
        RegSetValueEx(hKey, sEntry, 0, REG_DWORD, (PBYTE)&bValue, sizeof(bValue));
        RegCloseKey(hKey);
    }
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CProfile::ReadProfileInt(const CString &sEntry, DWORD dwDefault)
{
    HKEY	hKey		= NULL;
    DWORD	dwType		= 0;
    DWORD	dwValue		= dwDefault;
    DWORD	dwData		= 0;
    DWORD	dwSize		= 0;

	if (RegOpenKey(m_hRootKey, m_sSection, &hKey) != ERROR_SUCCESS)
        return dwDefault;

    dwSize = sizeof(dwData);
    if (RegQueryValueEx(hKey, sEntry, NULL, &dwType, (PBYTE)&dwData, &dwSize) == ERROR_SUCCESS)
	{
		if (dwType == REG_DWORD)
			dwValue = dwData;
	}

    RegCloseKey(hKey);

    return dwValue;
}


///////////////////////////////////////////////////////////////////////////////////
void CProfile::WriteProfileInt(const CString &sEntry, DWORD dwValue)
{
    HKEY	hKey	= NULL;
    DWORD	dwData	= dwValue;

    if (RegCreateKey(m_hRootKey, m_sSection, &hKey) == ERROR_SUCCESS)
	{
        RegSetValueEx(hKey, sEntry, 0, REG_DWORD, (PBYTE)&dwData, sizeof(dwData));
        RegCloseKey(hKey);
    }
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CProfile::ReadProfileString(const CString &sEntry, const CString &sDefault, CString &sResult)
{
    HKEY	hKey				= NULL;
    DWORD	dwType				= 0;
	DWORD	dwSize				= 1024;

	sResult = sDefault;

	if (RegOpenKey(m_hRootKey, m_sSection, &hKey) == ERROR_SUCCESS)
	{
        if (RegQueryValueEx(hKey, sEntry, NULL, &dwType, NULL, &dwSize) == ERROR_SUCCESS)
		{
			DWORD dwResult = RegQueryValueEx(hKey, sEntry, NULL, &dwType, (LPBYTE)sResult.GetBuffer(dwSize), &dwSize);
			sResult.ReleaseBuffer();
		}
        RegCloseKey(hKey);
    }

    return(sResult.GetLength());
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CProfile::EnumKey(int nIndex, CString& sKeyName, CString& sKeyValue)
{
    HKEY	hKey = NULL;

	TCHAR tcKeyName[_MAX_PATH] = {0};
	DWORD dwKeyNameLen = _MAX_PATH;
	BYTE  byKeyValue[_MAX_PATH] = {0};
	DWORD dwKeyValueLen = _MAX_PATH;
	DWORD dwType = 0;

	sKeyName.Empty();
	sKeyValue.Empty();

	DWORD dwResult = RegOpenKeyEx(m_hRootKey,
									m_sSection,
									0,
									KEY_ALL_ACCESS,
									&hKey);

	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwErr = 0;
		dwResult = RegEnumValue(hKey, nIndex, tcKeyName, &dwKeyNameLen, NULL, &dwType, byKeyValue, &dwKeyValueLen);
		if (dwResult == ERROR_SUCCESS)
		{
			sKeyName = tcKeyName;
			if (dwType == REG_SZ)
				sKeyValue = CString((PTCHAR)byKeyValue);
		}
		else
			dwErr = GetLastError();

		RegCloseKey(hKey);
	}

	return(dwResult);
}

///////////////////////////////////////////////////////////////////////////////////
void CProfile::WriteProfileString(const CString &sEntry, const CString &sData)
{
    HKEY	hKey	= NULL;

    if (RegCreateKey(m_hRootKey, m_sSection, &hKey) == ERROR_SUCCESS)
	{
		int nLen = (sData.GetLength()+1)*sizeof(TCHAR);
		CString sTemp = sData;
		RegSetValueEx(hKey, sEntry, 0, REG_SZ, (LPBYTE)sTemp.GetBuffer(nLen), nLen);
		sTemp.ReleaseBuffer();	
        RegCloseKey(hKey);
    }
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CProfile::ReadProfileBinary(const CString &sEntry, LPCVOID pDefault, LPVOID pResult, DWORD dwSize)
{
    HKEY	hKey		= NULL;
    DWORD	dwType		= 0;
    DWORD	dwResult	= dwSize;

    if (RegOpenKey(m_hRootKey, m_sSection, &hKey) == ERROR_SUCCESS)
	{
        if (RegQueryValueEx(hKey, sEntry, NULL, &dwType, (PBYTE)pResult, &dwResult) == ERROR_SUCCESS)
		{
			if (dwType == REG_BINARY)
			{
				// cbResult is the size
				RegCloseKey(hKey);
				return(dwResult);
			}
        }

        RegCloseKey(hKey);
    }

    // if we got here, we didn't find it, or it was the wrong type - return
    // the default values (use MoveMemory, since src could equal dst)
    MoveMemory (pResult, pDefault, dwSize);
    return dwSize;
}

///////////////////////////////////////////////////////////////////////////////////
void CProfile::WriteProfileBinary(const CString &sEntry, LPCVOID pData, DWORD dwData) 
{
    HKEY hKey	= NULL;

	if (RegCreateKey(m_hRootKey, m_sSection, &hKey) == ERROR_SUCCESS)
	{
        RegSetValueEx(hKey, sEntry, 0, REG_BINARY, (PBYTE)pData, dwData);
        RegCloseKey(hKey);
    }
}

