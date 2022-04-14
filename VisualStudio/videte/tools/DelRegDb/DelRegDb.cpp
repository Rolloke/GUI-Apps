// DelRegDb.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DelRegDb.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szSoftware[] = _T("Software");
static const TCHAR szRegistryKey[] = _T("w+k VideoCommunication");
static const TCHAR szSecurity[] = _T("Security");

////////////////////////////////////////////////////////////////////////////
BOOL CDelRegDbApp::DeleteAllSubKey(HKEY hSecurityKey, LPCTSTR lpszSection)
{
	LONG		lResult;
	TCHAR		cSubKeyName[_MAX_PATH];
	DWORD		dwLen = _MAX_PATH;
	HKEY		hKey = NULL;
	FILETIME	ft;

	if ((RegOpenKeyEx(hSecurityKey, lpszSection, 0, KEY_WRITE|KEY_READ, &hKey) != ERROR_SUCCESS) ||
		(hKey == NULL))
	{
		return FALSE;
	}

	while (ERROR_SUCCESS == (lResult = RegEnumKeyEx(hKey, 0, cSubKeyName, &dwLen, NULL, NULL, NULL, &ft)))
	{
        LPTSTR pszRegPathSubkey = new TCHAR[lstrlen(lpszSection) + dwLen + 2];
        wsprintf( pszRegPathSubkey, _T("%s\\%s"), lpszSection, cSubKeyName );

		// recurse call
		DeleteAllSubKey(hSecurityKey,pszRegPathSubkey);

        delete []pszRegPathSubkey;

		dwLen = _MAX_PATH;
	}
	RegCloseKey(hKey);

	lResult = RegDeleteKey(hSecurityKey, lpszSection);
	return lResult == ERROR_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////
HKEY CDelRegDbApp::GetCompanyKey()
{
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, szSoftware, 0, KEY_WRITE|KEY_READ,
										&hSoftKey))
	{
		DWORD dw;
		if (ERROR_SUCCESS == RegCreateKeyEx(hSoftKey, szRegistryKey, 0, REG_NONE,
											REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
											&hCompanyKey, &dw)
			)
		{
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);

	return hCompanyKey;
}
/////////////////////////////////////////////////////////////////////////////
HKEY CDelRegDbApp::GetNewCompanyKey()
{
	HKEY hSoftKey = NULL;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSoftware, 0, KEY_WRITE|KEY_READ,
		&hSoftKey) == ERROR_SUCCESS)
	{
		return hSoftKey;
	}

	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////
BOOL CDelRegDbApp::RunningAsAdministrator()
{ 
	BOOL   fAdmin = FALSE; 
	HANDLE  hThread;
	TOKEN_GROUPS *ptg = NULL; 
	DWORD  cbTokenGroups; 
	DWORD  dwGroup;
	PSID   psidAdmin;
	SID_IDENTIFIER_AUTHORITY SystemSidAuthority= SECURITY_NT_AUTHORITY;
	
	// First we must open a handle to the access token for this thread.
	if ( !OpenThreadToken ( GetCurrentThread(), TOKEN_QUERY, FALSE, &hThread)) 
	{
		if ( GetLastError() == ERROR_NO_TOKEN)  
		{
			// If the thread does not have an access token, we'll examine the
			// access token associated with the process.
			if (! OpenProcessToken ( GetCurrentProcess(), TOKEN_QUERY, &hThread))   
				return ( FALSE);  
		}
		else    
			return ( FALSE); 
	}

	// Then we must query the size of the group information associated with
	// the token. Note that we expect a FALSE result from GetTokenInformation
	// because we've given it a NULL buffer. On exit cbTokenGroups will tell
	// the size of the group information.
	if ( GetTokenInformation ( hThread, TokenGroups, NULL, 0, &cbTokenGroups))
		return ( FALSE);
	
	// Here we verify that GetTokenInformation failed for lack of a large
	// enough buffer. 
	if ( GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		return ( FALSE); 
	
	// Now we allocate a buffer for the group information.
	// Since _alloca allocates on the stack, we don't have
	// to explicitly deallocate it. That happens automatically
	// when we exit this function. 
	ptg = (TOKEN_GROUPS*)_alloca(cbTokenGroups);
	if (!ptg) 
		return ( FALSE); 
	
	// Now we ask for the group information again.
	// This may fail if an administrator has added this account
	// to an additional group between our first call to
	// GetTokenInformation and this one.
	
	if ( !GetTokenInformation ( hThread, TokenGroups, ptg, cbTokenGroups,
		&cbTokenGroups) )  
		return ( FALSE);
	
	// Now we must create a System Identifier for the Admin group.
	if ( ! AllocateAndInitializeSid ( &SystemSidAuthority, 2, 
		SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0, &psidAdmin) )  
		return ( FALSE);
	
	// Finally we'll iterate through the list of groups for this access
	// token looking for a match against the SID we created above. fAdmin= FALSE;
	for ( dwGroup= 0; dwGroup < ptg->GroupCount; dwGroup++) 
	{
		if ( EqualSid ( ptg->Groups[dwGroup].Sid, psidAdmin))  
		{   
			fAdmin = TRUE;
			break;  
		} 
	}
	
	// Before we exit we must explicity deallocate the SID we created.
	FreeSid ( psidAdmin); 
	return ( fAdmin);
}
/* eof - RunningAsAdministrator */
/////////////////////////////////////////////////////////////////////////////
// CDelRegDbApp

BEGIN_MESSAGE_MAP(CDelRegDbApp, CWinApp)
	//{{AFX_MSG_MAP(CDelRegDbApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelRegDbApp construction

CDelRegDbApp::CDelRegDbApp()
{
	m_bDelDB = FALSE;
	m_bDelReg = FALSE;
	m_bIdipJacob = FALSE;
	m_bDtsJacob = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDelRegDbApp object

CDelRegDbApp theApp;

//////////////////////////////////////////////////////////////////////////////////////
HKEY CDelRegDbApp::GetSectionKey(HKEY hKey,const CString & sSection)
{
	if (sSection.IsEmpty())
	{
		TRACE(_T("section cannot be empty"));
		return NULL;
	}

	HKEY hSecKey = NULL;

	DWORD dw;
	LONG lResult;

	lResult = RegCreateKeyEx(hKey, sSection, 0, REG_NONE,
							REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
							&hSecKey, &dw);
	if (lResult == ERROR_SUCCESS)
	{
		return hSecKey;
	}
	else
	{
		TRACE(_T("cannot open section key %s because\n"),sSection);
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////
DWORD CDelRegDbApp::GetInt(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszKey, DWORD dwDefault)
{
	if (hKey)
	{
		HKEY hSecKey = GetSectionKey(hKey,lpszSection);
		if (hSecKey == NULL)
		{
			return dwDefault;
		}
		DWORD dwValue;
		DWORD dwType;
		DWORD dwCount = sizeof(DWORD);
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszKey, NULL, &dwType,
										(LPBYTE)&dwValue, &dwCount);
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_DWORD);
			ASSERT(dwCount == sizeof(dwValue));
			return (UINT)dwValue;
		}
		else
		{
		}
		return dwDefault;
	}
	return dwDefault;
}
//////////////////////////////////////////////////////////////////////////////////////
CString CDelRegDbApp::GetString(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszDefault)
{
	if (hKey)
	{
		HKEY hSecKey = GetSectionKey(hKey,lpszSection);
		CString strValue;
		DWORD dwType, dwCount;
		LONG lResult;

		if (hSecKey == NULL)
			return lpszDefault;

		
		lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszKey, NULL, &dwType,
			NULL, &dwCount);

		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszKey, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		else
		{
			//WK_TRACE(_T("cannot read REG_SZ Value %s because %s"),lpszKey,GetLastErrorString(lResult));
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			return strValue;
		}
		return lpszDefault;
	}
	return lpszDefault;
}
//////////////////////////////////////////////////////////////////////////////////////
CString CDelRegDbApp::GetOemname()
{
	HKEY hKeyS = GetNewCompanyKey();
	if (hKeyS)
	{
		HKEY hKey = GetSectionKey(hKeyS,_T("DVRT"));
		CString strValue = GetString(hKey,_T("Version"),_T("Oemname"),_T(""));
		RegCloseKey(hKey);
		RegCloseKey(hKeyS);
		return strValue;
	}
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
// CDelRegDbApp initialization

BOOL CDelRegDbApp::InitInstance()
{

	CString sCommandLine = m_lpCmdLine;

	if (sCommandLine.IsEmpty())
	{
		return FALSE;
	}

	CString sOemname;
	sOemname = GetOemname();
	CString sTitle,sLoad,sText;
	sTitle.LoadString(IDS_TITLE);

	if (   WK_IS_RUNNING(WK_APP_NAME_DVSTARTER)
		|| WK_IS_RUNNING(WK_APP_NAME_LAUNCHER)
		)
	{
		sLoad.LoadString(IDP_STILL_RUNNING);
		sText.Format(sLoad,sOemname);
		MessageBox(NULL,sText,sTitle,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	sLoad.LoadString(IDP_ASK_UNINSTALL);
	sText.Format(sLoad,sOemname);

	if (IDNO==MessageBox(NULL,sText,sTitle,MB_YESNO|MB_ICONQUESTION))
	{
		return FALSE;
	}

	COsVersionInfo os;
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (!RunningAsAdministrator())
		{
			sText.LoadString(IDP_ADMIN);
			MessageBox(NULL,sText,sTitle,MB_OK|MB_ICONSTOP);
			return FALSE;
		}
	}

	m_bIdipJacob = DoesFileExist(_T("c:\\security\\jacobunit.exe"));
	m_bDtsJacob = DoesFileExist(_T("c:\\dv\\jacobunit.exe"));

	sText.LoadString(IDP_REG);
	if (IDYES==MessageBox(NULL,sText,sTitle,MB_YESNO|MB_ICONQUESTION))
	{
		int iDefaultValue = 0;
		int iKeyValue = 0;
		CString sDrive;
		char cValue;
		CWK_Profile wkProf;

		m_sDatabaseDrivesToDelete.Empty();
		//prüfe alle Laufwerkseinträge auf Datanbankeintrag (DWORD == 1)
		for(int i = 0; i <= 22; i++)
		{
			cValue = (char)(i + 68);
			sDrive = cValue;
			iKeyValue = wkProf.GetInt(_T("Drives"), sDrive, iDefaultValue);
			if(iKeyValue == 1) //found database drive
			{
				//Drive is Database Drive
				m_sDatabaseDrivesToDelete += cValue;
			}
		}
		
		m_sDatabaseDrivesToDelete.MakeLower();
		
		m_bDelReg = TRUE;
		DeleteSettings();
		m_bDelDB = TRUE;
		DeleteDatabase();
	}
	else
	{
		sText.LoadString(IDP_DATABASE);
		if (IDYES==MessageBox(NULL,sText,sTitle,MB_YESNO|MB_ICONQUESTION))
		{
			m_bDelDB = TRUE;
			DeleteDatabase();
		}
	}

	DeleteWinFiles();
	DeleteSystemFiles();
	DeleteMaskAndRegionFiles();

	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		Delete_PCI_NT();
	}

#ifdef _UNICODE
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

//	si.dwFlags		= STARTF_USESHOWWINDOW;
//	si.wShowWindow	= SW_SHOWNORMAL;
	LPTSTR szCommandLine = (LPTSTR)(LPCTSTR)sCommandLine; // only reading!
	BOOL bOK = CreateProcess( NULL,							// pointer to name of executable module 
							  szCommandLine,				// pointer to command line string
							  (LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
							  (LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
							  FALSE,						// handle inheritance flag 
							  NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
							  (LPVOID)NULL,					// pointer to new environment block 
							  (LPCTSTR)NULL,				// pointer to current directory name 
							  (LPSTARTUPINFO)&si,			// pointer to STARTUPINFO 
							  (LPPROCESS_INFORMATION)&pi 	// pointer to PROCESS_INFORMATION  
							);
	if (!bOK)
	{
		CString sMsg, sError;
		DWORD dwError = GetLastError();
		LPVOID lpMsgBuf;
		if (FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
							FORMAT_MESSAGE_FROM_SYSTEM | 
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							dwError,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
							(LPTSTR) &lpMsgBuf,
							0,
							NULL )
						)
		{
			sError.Format(_T("%s"), (LPCTSTR)lpMsgBuf);
		}
		else
		{
			sError.Format(_T("Error code %u"), dwError);
		}
		sMsg.Format(_T("%s\n%s"), sCommandLine, sError);
		MessageBox(NULL, sMsg, sTitle, MB_OK|MB_ICONQUESTION);
		sMsg.Format(_T("%s ; %s"), sCommandLine, sError);
		WK_TRACE_ERROR(sMsg);
	}
#else
	WinExec(sCommandLine,SW_SHOW);
#endif
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDelRegDbApp::DeleteSettings()
{
#ifdef _DTS
	HKEY hNewCK = GetNewCompanyKey();
	DeleteAllSubKey(hNewCK,_T("DVRT\\DV"));
	if (!m_bIdipJacob)
	{
		DeleteAllSubKey(hNewCK,_T("DVRT\\JacobUnit"));
	}
	RegCloseKey(hNewCK);
#else
	HKEY hCK = GetCompanyKey();
	DeleteAllSubKey(hCK,szSecurity);
	RegCloseKey(hCK);

	HKEY hNewCK = GetNewCompanyKey();
	DeleteAllSubKey(hNewCK,_T("DVRT"));
	RegCloseKey(hNewCK);

	DeleteDirRecursiv(_T("c:\\dvrtwww"));
#endif
	DeleteDirRecursiv(_T("c:\\log"));
}
/////////////////////////////////////////////////////////////////////////////
void CDelRegDbApp::DeleteDatabase()
{
	DWORD dwLD = GetLogicalDrives();
	DWORD dwBit = 1;
	CString sDrive,sDrives,sAnswer,sDir;
	char c = _T('a');
	int i;

	while (dwBit!=0)
	{
		if (dwLD & dwBit)
		{
			sDrives += c;
		}
		dwBit <<= 1;
		c++;
	}

	for (i=0;i<sDrives.GetLength();i++)
	{
		sDrive.Format(_T("%c:\\"),sDrives[i]);
		if (DRIVE_FIXED==GetDriveType(sDrive))
		{
#ifdef _DTS
			sDir = sDrive + _T("dvs");
			if (DoesFileExist(sDir))
			{
				DeleteDirRecursiv(sDir);
			}
#else
			
			if(m_sDatabaseDrivesToDelete.Find(sDrive.Left(1)) != -1) //found database drive
			{
				sDir = sDrive + _T("Database");
				if (DoesFileExist(sDrive))
				{
					DeleteDirRecursiv(sDir);
				}
				sDir = sDrive + _T("dbs");
				if (DoesFileExist(sDir))
				{
					DeleteDirRecursiv(sDir);
				}	
			}
#endif
		}
 	}

	CString sModule;

	GetModuleFileName(m_hInstance,sModule.GetBuffer(_MAX_PATH),_MAX_PATH);
	sModule.ReleaseBuffer();
	sModule = sModule.Left(sModule.ReverseFind(_T('\\')));
	
	CFileFind finder;
	BOOL bWorking = finder.FindFile(sModule + _T("\\*.dbf"));
	
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDots())
		{
			if (!finder.IsDirectory())
			{
				DeleteFile(finder.GetFilePath());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDelRegDbApp::DeleteWinFiles()
{
	/*
	Pathname=%WIN%\AkuUnit.ini
	Pathname=%WIN%\megrav2.ini
	Pathname=%WIN%\micounit.ini
	Pathname=%WIN%\CoCoIsa.ini
	Pathname=%WIN%\vidpi.ini
	*/
	TCHAR szWin[_MAX_PATH];
	CString toDelete;
	CString winDir;

	GetWindowsDirectory(szWin, sizeof(szWin));
	winDir = szWin;

	toDelete = winDir + _T("\\akuunit.ini");
	DeleteFile(toDelete);
	toDelete = winDir + _T("\\megrav2.ini");
	DeleteFile(toDelete);
	toDelete = winDir + _T("\\micounit.ini");
	DeleteFile(toDelete);
	toDelete = winDir + _T("\\cocoisa.ini");
	DeleteFile(toDelete);
	toDelete = winDir + _T("\\vidpi.ini");
	DeleteFile(toDelete);
}
/////////////////////////////////////////////////////////////////////////////
void CDelRegDbApp::DeleteSystemFiles()
{
	/*
	Pathname=%SYS%\MegraV2.dll
	Pathname=%SYS%\Convrt32.dll
	Pathname=%SYS%\CCodec32.dll
	Pathname=%SYS%\Finfo.dll
	Pathname=%SYS%\Mico.vxd
	Pathname=%SYS%\MicoPCI.vxd
	*/
	CString toDelete;
	CString sysDir;

	GetSystemDirectory(sysDir.GetBuffer(_MAX_PATH),_MAX_PATH);
	sysDir.ReleaseBuffer();
	toDelete = sysDir + _T("\\megrav2.dll");
	DeleteFile(toDelete);
	toDelete = sysDir + _T("\\Convrt32.dll");
	DeleteFile(toDelete);
	toDelete = sysDir + _T("\\CCodec32.dll");
	DeleteFile(toDelete);
	toDelete = sysDir + _T("\\Finfo.dll");
	DeleteFile(toDelete);
	toDelete = sysDir + _T("\\Mico.vxd");
	DeleteFile(toDelete);
	toDelete = sysDir + _T("\\MicoPCI.vxd");
	DeleteFile(toDelete);

	if (   (!m_bIdipJacob && m_bDtsJacob)
		|| (m_bIdipJacob && !m_bDtsJacob)
		)
	{
		toDelete = sysDir + _T("\\JaCob.vxd");
		DeleteFile(toDelete);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDelRegDbApp::DeleteMaskAndRegionFiles()
{
	CString toDelete;
	CString sModule;

	GetModuleFileName(m_hInstance,sModule.GetBuffer(_MAX_PATH),_MAX_PATH);
	sModule.ReleaseBuffer();
	sModule = sModule.Left(sModule.ReverseFind(_T('\\')));

	toDelete = sModule + _T("\\rterrors.dat");
	DeleteFile(toDelete);

	CFileFind finder;
//	BOOL bOK = TRUE;
	CString sName;
	BOOL bWorking = finder.FindFile(sModule + _T("\\*.*"));
	
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDots())
		{
			if (finder.IsDirectory())
			{
				if (0==finder.GetFileName().Find(_T("MDMask")))
				{
					DeleteDirRecursiv(finder.GetFilePath());
				}
			}
			else
			{
				sName = finder.GetFilePath();
				if (-1!=sName.Find(_T(".rgn")))
				{
					DeleteFile(sName);
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
static const TCHAR szServices[] = _T("System\\CurrentControlSet\\Services");
static const TCHAR szEnumRoot[] = _T("System\\CurrentControlSet\\Enum\\Root");
static const TCHAR szEvent[] = _T("System\\CurrentControlSet\\Services\\Eventlog\\System");
/////////////////////////////////////////////////////////////////////////////
void CDelRegDbApp::Delete_PCI_NT()
{
	HKEY hKey = NULL;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szServices, 0, KEY_ALL_ACCESS,
		&hKey) == ERROR_SUCCESS)
	{
#ifdef _DTS
		if (!m_bIdipJacob)
		{
			DeleteAllSubKey(hKey, _T("JaCob"));
		}
#else
		DeleteAllSubKey(hKey, _T("MiCoPCI"));
		if (!m_bDtsJacob)
		{
			DeleteAllSubKey(hKey, _T("JaCob"));
		}
#endif
		RegCloseKey(hKey);
	}
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szEvent, 0, KEY_ALL_ACCESS,
		&hKey) == ERROR_SUCCESS)
	{
#ifdef _DTS
		if (!m_bIdipJacob)
		{
			DeleteAllSubKey(hKey, _T("JaCob"));
		}
#else
		DeleteAllSubKey(hKey, _T("MiCoPCI"));
		if (!m_bDtsJacob)
		{
			DeleteAllSubKey(hKey, _T("JaCob"));
		}
#endif
		RegCloseKey(hKey);
	}
	
	TCHAR szSys[_MAX_PATH];
	CString toDelete;
	CString sysDir;

	GetSystemDirectory(szSys,sizeof(szSys));
	sysDir = szSys;
#ifdef _DTS
	if (!m_bIdipJacob)
	{
		toDelete = sysDir + _T("\\drivers\\JaCob.sys");
		if (!DeleteFile(toDelete))
		{
			MoveFileEx(toDelete,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}
#else
	toDelete = sysDir + _T("\\drivers\\MiCoPCI.sys");
	if (!DeleteFile(toDelete))
	{
		MoveFileEx(toDelete,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
	}
	if (!m_bDtsJacob)
	{
		toDelete = sysDir + _T("\\drivers\\JaCob.sys");
		if (!DeleteFile(toDelete))
		{
			MoveFileEx(toDelete,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////



