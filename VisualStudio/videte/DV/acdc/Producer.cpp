// Producer.cpp: implementation of the CProducer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "Producer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProducer::CProducer()
{

}

CProducer::~CProducer()
{

}

/*********************************************************************************************
 Class		   : CProducer
 Function      : FindInstalledBurnSoftware 
 Description   : This function checks the operating system (WinNT or 95/98) and checks the
				 installed burn software (DirectCD, Nero or WinXP).

 Parameters: -- 

 Result type:  returns the producer of the installed burn software (BurnProducer)
				BP_NOPRODUCER, BP_NERO, BP_XP or BP_DIRECT_CD

 Author: TKR
 created: August, 08 2002
 <TITLE FindInstalledBurnSoftware >
 <GROUP CProducer>
 <TOPICORDER 0>
 <KEYWORDS CProducer, FindInstalledBurnSoftware>
*********************************************************************************************/
BurnProducer CProducer::FindInstalledBurnSoftware()
{
	COsVersionInfo osinfo;
	_TCHAR szSystem[_MAX_PATH];
	CString sVersion;

	GetSystemDirectory(szSystem, _MAX_PATH);

	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		// Win95/98/Me
		if(osinfo.IsWin95())
		{
			WK_TRACE(_T("Windows95 wird nicht unterstützt\n"));
		}

		if(    osinfo.IsWin98()
			|| osinfo.IsWinME())
		{
			if(IsNeroInstalled())
			{
				return BP_NERO;
			}
			else if(IsDirectCDInstalled())
			{
				return BP_DIRECT_CD;
			}
		}
	}
	else if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Win NT
		if(IsNeroInstalled())
		{
			return BP_NERO;
		}
		else if(IsDirectCDInstalled())
		{
			return BP_DIRECT_CD;
		}
		else if(IsWinXP())
		{
			//war mal vorgesehen, hier mit XP eigenen Bordmitteln zu brennen
			return BP_XP;
		}
	}

	return BP_NOPRODUCER;
}

/*********************************************************************************************
 Class		   : CProducer
 Function      : DoesFileExist 
 Description   : This function checks if the given file or directory exists.

 Parameters: 
  szFileOrDirectory: (I): the file or directory to search (const char*)

 Result type:  return TRUE if the given file or directory exists (BOOL)

 Author: TKR
 created: August, 08 2002
 <TITLE DoesFileExist >
 <GROUP CProducer>
 <TOPICORDER 0>
 <KEYWORDS CProducer, DoesFileExist>
*********************************************************************************************/
BOOL CProducer::DoesFileExist(LPCTSTR szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}

/*********************************************************************************************
 Class		   : CProducer
 Function      : GetFileVersion 
 Description   : This function checks the file version of the given file.

 Parameters: 
  sFileName: (I): the file to check (const CString&)

 Result type:  the version string of the given file (CString)

 Author: TKR
 created: August, 08 2002
 <TITLE GetFileVersion >
 <GROUP CProducer>
 <TOPICORDER 0>
 <KEYWORDS CProducer, GetFileVersion>
*********************************************************************************************/
CString CProducer::GetFileVersion(const CString& sFileName)
{
	CString sRet;
	DWORD dwSize;
	DWORD dwHandle=0;
	PBYTE pData;
	UINT  dwValueLen;
	void* pValueData;
	CString sExe = sFileName;

	
	dwSize = GetFileVersionInfoSize(sExe.GetBuffer(0),&dwHandle);
	sExe.ReleaseBuffer();

	if (dwSize>0)
	{
		pData = new BYTE[dwSize];
		if (GetFileVersionInfo(sExe.GetBuffer(0),0,dwSize,pData))
		{
			struct LANGANDCODEPAGE 
			{
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;
			UINT cbTranslate = 0;

			// Read the list of languages and code pages.

			VerQueryValue(pData, 
						  TEXT("\\VarFileInfo\\Translation"),
						  (LPVOID*)&lpTranslate,
						  &cbTranslate);

			for (UINT i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
			{
				CString sLanguage;
				sLanguage.Format(TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
								lpTranslate[i].wLanguage,
								lpTranslate[i].wCodePage);
				
				// Retrieve file description for language and code page "i". 
				if (VerQueryValue(pData, 
								  sLanguage.GetBuffer(0), 
								  &pValueData, 
								  &dwValueLen))
				{
					sRet = (LPCTSTR)pValueData;
					sLanguage.ReleaseBuffer();
					break;
				}
				else
				{
					sLanguage.ReleaseBuffer();
				}
			}

		}
		sExe.ReleaseBuffer();
		WK_DELETE_ARRAY(pData);
	}
	return sRet;
}

/*********************************************************************************************
 Class		   : CProducer
 Function      : IsNeroInstalled 
 Description   : This function checks if Nero burn software is installed.

 Parameters: --

 Result type:  return TRUE if Nero is installed (BOOL)

 Author: TKR
 created: August, 08 2002
 <TITLE IsNeroInstalled >
 <GROUP CProducer>
 <TOPICORDER 0>
 <KEYWORDS CProducer, IsNeroInstalled>
*********************************************************************************************/
BOOL CProducer::IsNeroInstalled()
{
	BOOL bRet = FALSE;
	CString sNeroPath;
	CString sNeroApi(_T("NeroApi.dll"));
	CString sNeroExe(_T("Nero.exe"));
	CString sNeroApiVersion;

	// open registry path "SOFTWARE\\Ahead\\Shared", find key "NeroAPI" 
	//this key holds the installation path of Nero.exe		
	CFile file;
	HKEY namekey;
	CString sStrValue;
	LONG lResult;
	DWORD dwType, dwCount;

	//get Nero installation path from Nero`s registry values
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
							_T("SOFTWARE\\Ahead\\Shared"),
							0,
							KEY_QUERY_VALUE,
							&namekey);
	if(lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueEx(namekey, 
								_T("NeroApi"), 
								NULL, 
								&dwType, 
								NULL,
								&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(namekey, 
									  _T("NeroApi"), 
									  NULL, 
									  &dwType,
									  (LPBYTE)sStrValue.GetBuffer(dwCount/sizeof(TCHAR)), 
									  &dwCount);
			//sStrValue holds the path to the NeroApi file

		}
		RegCloseKey(namekey);

		sNeroPath.Format(_T("%s"), sStrValue);
		sStrValue.ReleaseBuffer();

		
		if(lResult == ERROR_SUCCESS)
		{
			//nero registry path not found (version to old), nero installation path is unknown
			//search for Nero.exe and get version from Nero.exe
			if(sNeroPath.IsEmpty())
			{
				//search for Nero.exe on all drives
				CString sLocation;
				SearchFileRecursiv(NULL, sNeroExe, sLocation);
				if(file.Open(sLocation, CFile::modeRead))
				{
					file.Close();
					m_sNeroVersion = GetFileVersion(sLocation);
					bRet = TRUE;
				}
			}
			else
			{
				if(sNeroPath.Right(3) == _T("exe"))
				{
					//find Nero.exe in folder from registry
					//is Nero Version smaller than 5.5.8.2 the NeroAPI.dll is included in the Nero.exe
					//so get the file version from the Nero.exe
					if(file.Open(sNeroPath, CFile::modeRead))
					{
						file.Close();
						m_sNeroVersion = GetFileVersion(sNeroPath);
						bRet = TRUE;
					}
				}
				else
				{
					sNeroPath = sNeroPath + _T("\\") + sNeroApi;
					//find NeroAPI.dll in folder from registry 
					//is Nero Version equal than or greater 5.5.8.2 get version from NeroAPI.dll 
					if(file.Open(sNeroPath, CFile::modeRead))
					{
						file.Close();
						m_sNeroVersion = GetFileVersion(sNeroPath);
						bRet = TRUE;
					}
				}
		
			}
		}
	}

	if(!m_sNeroVersion.IsEmpty())
	{
		CString s = m_sNeroVersion;
		s.Replace(_T(','), _T('.'));
		m_sNeroVersion = s;
	}

	return bRet;
}

BOOL CProducer::IsWinXP()
{
	BOOL bRet = FALSE;

	return bRet;
}
/*********************************************************************************************
Class		   : CProducer
Function      : IsDirectCDInstalled 
Description   : This function checks if DirectCD burn software is installed.

Parameters: --

Result type:  return TRUE if DirectCD is installed (BOOL)

Author: TKR
created: August, 25 2005
<TITLE IsDirectCDInstalled >_T("DirectCD");
<GROUP CProducer>
<TOPICORDER 0>
<KEYWORDS CProducer, IsDirectCDInstalled>
*********************************************************************************************/
BOOL CProducer::IsDirectCDInstalled()
{
	BOOL bRet = FALSE;

	_TCHAR szSystem[_MAX_PATH];
	GetSystemDirectory(szSystem, _MAX_PATH);
	CString sDriver;
	CString sVersion;
	sDriver = szSystem;
	sDriver += _T("\\drivers\\cdudf.sys");
	WK_TRACE(_T("searching DirectCD driver %s\n"), sDriver);
	if (DoesFileExist(sDriver))
	{
		sVersion = WK_GetFileVersion(sDriver);
		WK_TRACE(_T("DirectCD driver found %s, %s\n"), sDriver, sVersion);
		bRet = TRUE;
	}
	else
	{
		WK_TRACE(_T("DirectCD driver not exist: %s\n"));
	}
	return bRet;
}
/*********************************************************************************************
 Class		   : CProducer
 Function      : GetNeroVersion 
 Description   : This function checks if Nero version is smaller/bigger than a given number

 Parameters: 
  sInstalledVersion: (O): the installed Nero version (CString&)
  sAtLeastVersion:   (O): the Nero version to check for (CString&)


 Result type:  return TRUE if installed Nero Version is same or higher than at least version (BOOL)

 Author: TKR
 created: May, 27 2003
 <TITLE GetNeroVersion >
 <GROUP CProducer>
 <TOPICORDER 0>
 <KEYWORDS CProducer, GetNeroVersion>
*********************************************************************************************/
BOOL CProducer::GetNeroVersion(CString &sInstalledVersion, CString sAtLeastVersion /* = 5.5.8.2.*/)
{
	BOOL bMinNeroVersion = FALSE;
	CString s = m_sNeroVersion;
	DWORD dwInstalled1, dwInstalled2, dwInstalled3, dwInstalled4;
	DWORD dwMin1, dwMin2, dwMin3, dwMin4;

	sInstalledVersion = m_sNeroVersion;
	dwInstalled1 = dwInstalled2 = dwInstalled3 = dwInstalled4 = 0;
	dwMin1 = dwMin2 = dwMin3 = dwMin4 = 0;
	
	//Nero muss mind. Version 5.5.8.2 bzw. übergebene Version (z.B. 5.5.10.15) sein
	//extract übergebene Nero Version
	_stscanf(sAtLeastVersion,_T("%d.%d.%d.%d"),&dwMin1,&dwMin2,&dwMin3,&dwMin4);
	
	//extract installierte Nero Version
	_stscanf(s,_T("%d.%d.%d.%d"),&dwInstalled1,&dwInstalled2,&dwInstalled3,&dwInstalled4);

	if(dwInstalled1 == dwMin1)
	{
		if(dwInstalled2 == dwMin2)
		{
			if(dwInstalled3 == dwMin3)
			{
				if(dwInstalled4 >= dwMin4)
				{
					bMinNeroVersion = TRUE;
				}
			}
			else if(dwInstalled3 > dwMin3)
			{
				bMinNeroVersion = TRUE;
			}
		}
		else if(dwInstalled2 > dwMin2)
		{
			bMinNeroVersion = TRUE;
		}
	}
	else if(dwInstalled1 > dwMin1)
	{
		bMinNeroVersion = TRUE;
	}

	return bMinNeroVersion;
}
