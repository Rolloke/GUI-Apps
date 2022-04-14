// RegMove.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RegMove.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SIZEOFARRAY( x ) (sizeof(x)/sizeof(x[0]))
#define WK_RegistryKey			"w+k VideoCommunication"
#define WK_SecurityKey			"Security\\3.00"

static const TCHAR szSoftware[] = "Software";
static const TCHAR szRegistryKey[] = WK_RegistryKey;
static const TCHAR szSecurity[] = WK_SecurityKey;

static const TCHAR szDVRT[] = "DVRT";

/////////////////////////////////////////////////////////////////////////////
BOOL DoesFileExist(const char* szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void SplitCommandLine(const CString& sCommandLine, CStringArray& saParameters)
{
	CString sCL = sCommandLine;
	CString sOne;
	int p;

	if (sCommandLine.IsEmpty())
	{
		return;
	}
	sCL.TrimLeft();
	p = sCL.Find(' ');
	while (p!=-1)
	{
		sOne = sCL.Left(p);
		sCL = sCL.Mid(p+1);
		sCL.TrimLeft();
		saParameters.Add(sOne);
		p = sCL.Find(' ');
	}
	if (!sCL.IsEmpty())
	{
		saParameters.Add(sCL);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CRegMoveApp

BEGIN_MESSAGE_MAP(CRegMoveApp, CWinApp)
	//{{AFX_MSG_MAP(CRegMoveApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegMoveApp construction

CRegMoveApp::CRegMoveApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRegMoveApp object

CRegMoveApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRegMoveApp initialization

BOOL CRegMoveApp::InitInstance()
{
	CStringArray saCommand;

	SplitCommandLine(m_lpCmdLine,saCommand);

	if (0==saCommand.GetSize())
	{
		// nothing to do
		// safe is better
	}
	else if (1==saCommand.GetSize())
	{
		if (0==saCommand.GetAt(0).CompareNoCase("movewk2dvrt"))
		{
			MoveWK2DVRT();
		}
		if (0==saCommand.GetAt(0).CompareNoCase("import"))
		{
			ImportReg();
		}
	}
	else if (2==saCommand.GetSize())
	{
		if (0==saCommand.GetAt(0).CompareNoCase("exportwk2dvrt"))
		{
			ExportWK2DVRT(saCommand.GetAt(1));
		}
		else if (0==saCommand.GetAt(0).CompareNoCase("exportwk"))
		{
			ExportWK(saCommand.GetAt(1));
		}
		else if (0==saCommand.GetAt(0).CompareNoCase("exportdvrt"))
		{
			ExportDVRT(saCommand.GetAt(1));
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::MoveWK2DVRT()
{
	HKEY hSourceKey = GetWKKey();
	HKEY hDestKey = GetDVRTKey();

	MoveRegistry( hSourceKey, hDestKey);

	RegCloseKey(hSourceKey);
	RegCloseKey(hDestKey);
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::ExportRegistryToFile(HKEY hKey, const CString &sDestKey, const CString& sFileName)
{
	CFile file;
	CString sDest = sDestKey;
	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite))
	{
		CString line = "REGEDIT4\r\n";
		file.Write((const char*)line,line.GetLength());
		ExportRegistry( hKey, sDest, file);
		file.Flush();
		file.Close();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::ExportWK2DVRT(const CString& sFileName)
{
	HKEY hKey = GetWKKey();
	ExportRegistryToFile(hKey,"HKEY_LOCAL_MACHINE\\Software\\DVRT",sFileName);
	RegCloseKey(hKey);
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::ExportWK(const CString& sFileName)
{
	HKEY hKey = GetWKKey();
	ExportRegistryToFile(hKey,
		"HKEY_CURRENT_USER\\Software\\w+k VideoCommunication\\Security\\3.00",
		sFileName);
	RegCloseKey(hKey);
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::ExportDVRT(const CString& sFileName)
{
	HKEY hKey = GetDVRTKey();
	ExportRegistryToFile(hKey,"HKEY_LOCAL_MACHINE\\Software\\DVRT",sFileName);
	RegCloseKey(hKey);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRegMoveApp::DeleteSettings(const CString& sPathName)
{
	CStdioFile file;
	CString sLine;
	CString s;
	CFileException e;
	BOOL bRet = FALSE;

	if (file.Open(sPathName,CFile::modeRead,&e))
	{
		TRY
		{
			while(file.ReadString(sLine))
			{
				if (0==sLine.Find('['))
				{
					sLine.MakeUpper();
					if (-1!=sLine.Find("HKEY_LOCAL_MACHINE\\SOFTWARE\\DVRT"))
					{
						// delete dvrt todo
						HKEY hSoftKey = NULL;
						if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSoftware, 0, KEY_ALL_ACCESS,
							&hSoftKey) == ERROR_SUCCESS)
						{
							DeleteAllSubKey(hSoftKey,szDVRT);
							RegCloseKey(hSoftKey);
						}
						bRet = TRUE;
						break;
					}
					else if (-1!=sLine.Find("HKEY_CURRENT_USER\\SOFTWARE\\W+K VIDEOCOMMUNICATION"))
					{
						// delete w+k todo
						HKEY hSoftKey = NULL;
						if (RegOpenKeyEx(HKEY_CURRENT_USER, szSoftware, 0, KEY_ALL_ACCESS,
							&hSoftKey) == ERROR_SUCCESS)
						{
							DeleteAllSubKey(hSoftKey,WK_RegistryKey);
							RegCloseKey(hSoftKey);
						}
						bRet = TRUE;
						break;
					}
				}
			}
		}
		CATCH(CFileException ,cfe)
		{
			bRet = FALSE;
		}
		END_CATCH


	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::ImportSettings(const CString& sPathName)
{
	CString sCommand;
	CString sMico;
	int p;

	sCommand = "regedit.exe " + sPathName;
	WinExec(sCommand,SW_SHOW);


	sMico = sPathName;
	p = sMico.ReverseFind('.');
	if (p!=-1)
	{
		char szPath[_MAX_PATH];
		CString sWin;
		::GetWindowsDirectory(szPath,_MAX_PATH);
		sWin = szPath;
		sWin += "\\micounit.ini";

		sMico = sMico.Left(p+1) + "ini";
		if (DoesFileExist(sWin))
		{
			DeleteFile(sWin);
		}

		if (DoesFileExist(sMico))
		{
			CopyFile(sMico,sWin,FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::ImportReg()
{
	CString sFilter;
	CString sName,sPathName;
	sFilter.LoadString(IDS_REG_FILTER);
	CFileDialog dlg(TRUE, "*.reg", NULL, OFN_HIDEREADONLY , sFilter);

	dlg.m_ofn.lpstrInitialDir = "c:\\";
	if (dlg.DoModal()==IDOK) 
	{
		CString sLoad;
		CString sRecover;

		sLoad.LoadString(IDS_WARNING_DELETE);
		sRecover.LoadString(IDS_RECOVER);

		if (IDYES==MessageBox(NULL,sLoad,sRecover,MB_YESNO|MB_ICONQUESTION))
		{
			if (DeleteSettings(dlg.GetPathName()))
			{
				ImportSettings(dlg.GetPathName());
				WriteProtocol("Import", dlg.GetPathName());
			}
			else
			{
				sLoad.LoadString(IDS_NO_DELETE);
				MessageBox(NULL,sLoad,sRecover,MB_OK|MB_ICONSTOP);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
BOOL CRegMoveApp::DeleteAllSubKey(HKEY hSecurityKey, LPCSTR lpszSection)
{
	LONG		lResult;
	TCHAR		cSubKeyName[_MAX_PATH];
	DWORD		dwLen = _MAX_PATH;
	HKEY		hKey = NULL;
	FILETIME	ft;

	if ((RegOpenKeyEx(hSecurityKey,lpszSection,0,KEY_WRITE|KEY_READ,&hKey) != ERROR_SUCCESS) ||
		(hKey == NULL))
	{
		return FALSE;
	}

	while (ERROR_SUCCESS == (lResult = RegEnumKeyEx(hKey,0,cSubKeyName,&dwLen,NULL,NULL,NULL,&ft)))
	{
        LPTSTR pszRegPathSubkey = new TCHAR[lstrlen(lpszSection) + dwLen + 2];
        wsprintf( pszRegPathSubkey, TEXT("%s\\%s"), lpszSection, cSubKeyName );

		// recurse call
		DeleteAllSubKey(hSecurityKey,pszRegPathSubkey);

        delete []pszRegPathSubkey;

		dwLen = _MAX_PATH;
	}
	RegCloseKey(hKey);

	lResult = RegDeleteKey(hSecurityKey,lpszSection);
	return lResult == ERROR_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::MoveRegistry( HKEY hSourceKey, HKEY hDestKey)
{
    DWORD regEnumIndex = 0;

    TCHAR szValueName[ MAX_PATH ];
    DWORD cbValueNameSize = SIZEOFARRAY(szValueName);
    TCHAR szValue[ MAX_PATH ];
    DWORD cbValueSize = SIZEOFARRAY(szValue);
    DWORD type;
    LONG result;

    while ( TRUE )
    {
		cbValueNameSize = SIZEOFARRAY(szValueName);
		cbValueSize = SIZEOFARRAY(szValue);
        result = RegEnumValue(  hSourceKey, regEnumIndex++,
                                szValueName, &cbValueNameSize,
                                0, &type, (LPBYTE)szValue, &cbValueSize );
                            
        if ( ERROR_NO_MORE_ITEMS == result )
		{
            break;
		}

		if (REG_SZ == type)
		{
			if (szValueName[0]=='\0')
			{
				// standard entry
				// nothing to do
			}
			else
			{
				// write string value to new destination
				RegSetValueEx(hDestKey, szValueName, NULL, REG_SZ,
							(LPBYTE)szValue, (lstrlen(szValue)+1)*sizeof(TCHAR));
			}
		}
		else if (REG_DWORD == type)
		{
			DWORD dwValue;
			CopyMemory(&dwValue,szValue,sizeof(DWORD));
			// write dword value to new destination
			RegSetValueEx(hDestKey, szValueName, NULL, REG_DWORD,
						(LPBYTE)&dwValue, sizeof(dwValue));

		}
		else
		{
			// unsupported reg entry
		}
    }

    // ================ Now iterate through all the child nodes ===============

    regEnumIndex = 0;
       TCHAR szSubkey[ MAX_PATH ];
    
    while ( TRUE )
    {
        DWORD cbSubkeySize = SIZEOFARRAY(szSubkey);
        
        result = RegEnumKeyEx(  hSourceKey, regEnumIndex++, szSubkey, &cbSubkeySize,
                                0, 0, 0, 0 );

        if ( ERROR_NO_MORE_ITEMS == result )
            break;

        if ( 0 != result )  // Some other error?  Ignore and continue
            continue;

        HKEY hSourceSubkey;
		HKEY hDestSubkey;
        
        result = RegOpenKeyEx( hSourceKey, szSubkey, 0, KEY_ALL_ACCESS, &hSourceSubkey );
                            
        if ( ERROR_SUCCESS != result )
		{
            continue;
		}

		DWORD dw;
		result = RegCreateKeyEx(hDestKey, szSubkey, 0, REG_NONE,
								REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
								&hDestSubkey, &dw);
                            
        if ( ERROR_SUCCESS != result )
		{
            continue;
		}

        // Recurse to find any child keys
        MoveRegistry( hSourceSubkey, hDestSubkey);

        RegCloseKey( hSourceSubkey );
        RegCloseKey( hDestSubkey );
    }
}
/////////////////////////////////////////////////////////////////////////////
HKEY CRegMoveApp::GetWKKey()
{
	HKEY hSecurityKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, szSoftware, 0, KEY_ALL_ACCESS,
		&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		if (RegCreateKeyEx(hSoftKey, szRegistryKey, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
			&hCompanyKey, &dw) == ERROR_SUCCESS)
		{
			RegCreateKeyEx(hCompanyKey, szSecurity, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
				&hSecurityKey, &dw);
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hCompanyKey != NULL)
		RegCloseKey(hCompanyKey);

	return hSecurityKey;
}
/////////////////////////////////////////////////////////////////////////////
HKEY CRegMoveApp::GetDVRTKey()
{
	HKEY hSoftKey = NULL;
	HKEY hProduktKey = NULL;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSoftware, 0, KEY_ALL_ACCESS,
		&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		RegCreateKeyEx(hSoftKey, szDVRT, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
			&hProduktKey, &dw);
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);

	return hProduktKey;
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::ExportRegistry( HKEY hKey, LPCTSTR pszRegPath , CFile& file)
{
    DWORD regEnumIndex = 0;

    TCHAR szValueName[ MAX_PATH ];
    DWORD cbValueNameSize = SIZEOFARRAY(szValueName);
    TCHAR szValue[ MAX_PATH ];
    DWORD cbValueSize = SIZEOFARRAY(szValue);
    DWORD type;
    LONG result;
	CString line;

	line.Format("\r\n[%s]\r\n",pszRegPath);
	file.Write((const char*)line,line.GetLength());
    while ( TRUE )
    {
		cbValueNameSize = SIZEOFARRAY(szValueName);
		cbValueSize = SIZEOFARRAY(szValue);
        result = RegEnumValue(  hKey, regEnumIndex++,
                                szValueName, &cbValueNameSize,
                                0, &type, (LPBYTE)szValue, &cbValueSize );
                            
        if ( ERROR_NO_MORE_ITEMS == result )
		{
            break;
		}

		if (REG_SZ == type)
		{
			if (szValueName[0]=='\0')
			{
				line.Format("@=\"%s\"\r\n",szValueName,szValue);
				line = DuplicateBackSlash(line);
				file.Write((const char*)line,line.GetLength());
			}
			else
			{
				line.Format("\"%s\"=\"%s\"\r\n",szValueName,szValue);
				line = DuplicateBackSlash(line);
				file.Write((const char*)line,line.GetLength());
			}
		}
		else if (REG_DWORD == type)
		{
			DWORD dwValue;
			CopyMemory(&dwValue,szValue,sizeof(DWORD));
			line.Format("\"%s\"=dword:%08lx\r\n",szValueName,dwValue);
			file.Write((const char*)line,line.GetLength());
		}
		else
		{
			TRACE("unsupported reg entry %s,%s\r\n",pszRegPath,szValueName);
		}
    }

    // ================ Now iterate through all the child nodes ===============

    regEnumIndex = 0;
       TCHAR szSubkey[ MAX_PATH ];
    
    while ( TRUE )
    {
        DWORD cbSubkeySize = SIZEOFARRAY(szSubkey);
        
        result = RegEnumKeyEx(  hKey, regEnumIndex++, szSubkey, &cbSubkeySize,
                                0, 0, 0, 0 );

        if ( ERROR_NO_MORE_ITEMS == result )
            break;

        if ( 0 != result )  // Some other error?  Ignore and continue
            continue;

        HKEY hSubkey;
        
        result = RegOpenKeyEx( hKey, szSubkey, 0, KEY_READ, &hSubkey );
                            
        if ( ERROR_SUCCESS != result )
            continue;

        // Create a fully qualified subkey name to pass to ExportRegistry
        LPTSTR pszRegPathSubkey
            = new TCHAR[lstrlen(pszRegPath) + cbSubkeySize + 2];
        wsprintf( pszRegPathSubkey, TEXT("%s\\%s"), pszRegPath, szSubkey );

        // Recurse to find any child keys
        ExportRegistry( hSubkey, pszRegPathSubkey , file);

        delete []pszRegPathSubkey;

        RegCloseKey( hSubkey );
    }
}
/////////////////////////////////////////////////////////////////////////////
CString CRegMoveApp::DuplicateBackSlash(CString s)
{
	CString r;
	int i;

	for (i=0;i<s.GetLength();i++)
	{
		r += s[i];
		if (s[i]=='\\')
		{
			r += '\\';
		}
	}
	return r;
}
/////////////////////////////////////////////////////////////////////////////
void CRegMoveApp::WriteProtocol(const CString& sSection, const CString sFileName)
{
	int i = GetPrivateProfileInt(sSection,"Nr",0,"dvrt.prv");
	CString sI,sValue;
	CTime ct = CTime::GetCurrentTime();

	sI.Format("%d",i+1);
	WritePrivateProfileString(sSection,"Nr",sI,"dvrt.prv");
	
	sI.Format("Date%d",i+1);
	sValue = ct.Format("%m/%d/%y %H:%M:%S");
	WritePrivateProfileString(sSection,sI,sValue,"dvrt.prv");

	sI.Format("%s%d",sSection,i+1);
	WritePrivateProfileString(sSection,sI,sFileName,"dvrt.prv");

	// flush ini file.
	WritePrivateProfileString(NULL,NULL,NULL,"dvrt.prv");
}


