// VideteCheckDisk.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "VideteCheckDisk.h"
#include "VideteCheckDiskDlg.h"

#ifdef _DEBUG
 #define new DEBUG_NEW
 #include <locale.h>
 #define OEMGUI_DLL _T("OemGuiDU.dll")
 #define OEMGUI_ABOUTDIALOG  "?AboutDialog@COemGuiApi@@SAJPAVCWnd@@HK@Z"
#else
 #define OEMGUI_DLL _T("OemGuiU.dll")
 #define OEMGUI_ABOUTDIALOG  "?AboutDialog@COemGuiApi@@SAJPAVCWnd@@HK@Z"
#endif


// CVideteCheckDiskApp

BEGIN_MESSAGE_MAP(CVideteCheckDiskApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVideteCheckDiskApp-Erstellung


CVideteCheckDiskApp::CVideteCheckDiskApp()
{
#ifdef _DEBUG
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_NEUTRAL), 0));
#else
	InitVideteLocaleLanguage();
#endif
	m_nReturnCode = IDCANCEL;
	m_hOemGUI = NULL;
	m_pDlg = NULL;
}

CVideteCheckDiskApp::~CVideteCheckDiskApp()
{
	WK_DELETE(m_pDlg);
	FreeLibrary(m_hOemGUI);
}


// Das einzige CVideteCheckDiskApp-Objekt

CVideteCheckDiskApp theApp;


// CVideteCheckDiskApp Initialisierung
void CVideteCheckDiskApp::DisplayHelp()
{
	AfxMessageBox(IDS_HELP_TEXT, MB_OK|MB_ICONINFORMATION);
}

void CVideteCheckDiskApp::DisplayInfo()
{
	if (DoesFileExist(OEMGUI_DLL))
	{
		typedef LRESULT (__cdecl*ABOUTDIALOG)(CWnd*, BOOL, DWORD );
		if (m_hOemGUI == NULL)
		{
			m_hOemGUI = LoadLibrary(OEMGUI_DLL);
		}
		if (m_hOemGUI)
		{
			ABOUTDIALOG pfnc = (ABOUTDIALOG)GetProcAddress(m_hOemGUI, OEMGUI_ABOUTDIALOG);
			if (pfnc)
			{
				pfnc(m_pMainWnd, FALSE, 0);
			}
		}
	}
	else
	{
		AfxMessageBox(IDS_INFO_TEXT, MB_OK|MB_ICONINFORMATION);
	}
}

BOOL CVideteCheckDiskApp::InitInstance()
{
	CString sCmdLine(m_lpCmdLine);
	if (sCmdLine.Find(_T("/?")) != -1)
	{
		DisplayHelp();
		return FALSE;
	}
	if (WK_ALONE(WK_APP_NAME_VCD) == FALSE)
	{
		return FALSE;
	}
	CWinApp::InitInstance();

	CString sLogfile = m_pszAppName;
	sLogfile += _T(".Log");
	InitDebugger(sLogfile, WAI_INVALID);

	BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	if (m_pszRegistryKey)
	{
		free((void*)m_pszRegistryKey);
	}
	if (m_pszProfileName)
	{
		free((void*)m_pszProfileName);
	}
	CString sHelp(m_pszHelpFilePath);
	sHelp.MakeLower();
	sHelp.Replace(_T(".hlp"), _T(".ini"));
	m_pszProfileName = _tcsdup(sHelp);
	AfxEnableMemoryTracking(bEnable);


	m_pDlg = new CVideteCheckDiskDlg;
	if (!sCmdLine.IsEmpty())
	{
		int nFind;
		sCmdLine.MakeLower();
		if (sCmdLine.Find(_T("/a")) != -1)
		{
			m_pDlg->m_bAutoMode = TRUE;
		}
		if (sCmdLine.Find(_T("/c")) != -1)
		{
			m_pDlg->m_bAutoCheckDisk = TRUE;
		}
		if (sCmdLine.Find(_T("/e")) != -1)
		{
			m_pDlg->m_bAutoEnableEwf = TRUE;
		}
		else if (sCmdLine.Find(_T("/d")) != -1)
		{
			m_pDlg->m_bAutoEnableEwf = DISABLE;
		}
		else if ((nFind = sCmdLine.Find(_T("/r="))) != -1)
		{
			CStringArray saDrives, saVolumes;
			_TCHAR szVolume[MAX_PATH];
			CString sDrive, sOrg, sDest;

			sCmdLine = sCmdLine.Mid(nFind+3);
			sCmdLine.Replace(_T(" "), _T(""));
			sCmdLine.Replace(_T("\""), _T(""));
			SplitString(sCmdLine, saDrives, _T(","));

			int i, nSize = saDrives.GetSize();
			for (i=0; i<nSize; i++)
			{
				sDrive = saDrives.GetAt(i);
				if (   sDrive.GetAt(0) == sDrive.GetAt(4)	// dont rename to the same letter
					|| sOrg.Find(sDrive.GetAt(0))  != -1	// no duplicate original letters
					|| sDest.Find(sDrive.GetAt(4)) != -1	// no duplicate destination letters
					|| sDrive.GetAt(1) != _T(':')			// syntax analysis
					|| sDrive.GetAt(2) != _T('-')
					|| sDrive.GetAt(3) != _T('>')
					|| sDrive.GetAt(5) != _T(':'))
				{
					CString sMsg;
					sMsg.Format(_T("Syntax error at position %d:%s\n"), i+1, sDrive);
					AfxMessageBox(sMsg, MB_ICONERROR|MB_OK);
					return -1;
				}
				sOrg  += sDrive.GetAt(0);
				sDest += sDrive.GetAt(4);
			}
			for (i=0; i<nSize; i++)
			{
				sDrive = saDrives.GetAt(i).Left(2) + _T("\\");
				if (GetVolumeNameForVolumeMountPoint(sDrive, szVolume, MAX_PATH))
				{
					saVolumes.Add(szVolume);
				}
				else
				{
					CString sMsg;
					sMsg.Format(_T("Error:%d\nCould not determine volume name of %s"), GetLastError(), sDrive);
					AfxMessageBox(sMsg, MB_ICONERROR|MB_OK);
					return -1;
				}
				if (DeleteVolumeMountPoint(sDrive) == FALSE)
				{
					CString sMsg;
					sMsg.Format(_T("Error:%d\nCould not delete volume mount point of %s"), GetLastError(), sDrive);
					AfxMessageBox(sMsg, MB_ICONERROR|MB_OK);
					for (i--; i>=0; i--)
					{
						sDrive = saDrives.GetAt(i).Left(2) + _T("\\");
						SetVolumeMountPoint(sDrive, saVolumes.GetAt(i));
					}
					return -1;
				}
			}
			CString sMsg = _T("Renamed drive letters:\n");
			for (i=0; i<nSize; i++)
			{
				sMsg += saDrives.GetAt(i) + _T("\n");
				sDrive = saDrives.GetAt(i).Mid(4) + _T("\\");
				if (SetVolumeMountPoint(sDrive, saVolumes.GetAt(i)) == FALSE)
				{
					CString sMsg;
					sMsg.Format(_T("Error:%d\nCould not set volume mount point of %s"), GetLastError(), sDrive);
					AfxMessageBox(sMsg, MB_ICONERROR|MB_OK);
					for (i--; i>=0; i--)
					{
						sDrive = saDrives.GetAt(i).Left(2) + _T("\\");
						SetVolumeMountPoint(sDrive, saVolumes.GetAt(i));
					}
					return -1;
				}
			}
			AfxMessageBox(sMsg, MB_ICONINFORMATION|MB_OK);

			return 0;
		}

	}

	if (m_pDlg && m_pDlg->Create(m_pDlg->IDD))
	{
		m_pDlg->CheckHidden();
		m_pMainWnd = m_pDlg;
		return TRUE;
	}

	return FALSE;
}

int CVideteCheckDiskApp::ExitInstance()
{
	if (m_pDlg && m_nReturnCode != IDCANCEL)
	{
		m_sLogFilesSource      = m_pDlg->m_sLogFilesSource;
		m_sLogFilesDestination = m_pDlg->m_sLogFilesDestination;
		if (m_nReturnCode == IDOK)
		{
			WK_TRACE(_T("Returning normal Extit Code:1\n"));
		}
		else
		{
			WK_TRACE(_T("Returning Reboot Extit Code:%d\n"), m_nReturnCode);
		}
	}

	if (  !m_sLogFilesDestination.IsEmpty()
		&& m_sLogFilesDestination.CompareNoCase(m_sLogFilesSource) != 0
		&& DoesFileExist(m_sLogFilesSource) == DFE_IS_DIRECTORY
		&& DoesFileExist(m_sLogFilesDestination) == DFE_IS_DIRECTORY)
	{
		CString sCmd;
		sCmd.Format(_T("wait:xcopy %s\\*.* %s /Q /Y"), m_sLogFilesSource, m_sLogFilesDestination);
		WK_TRACE(_T("Executing: %s\n"), sCmd);
		ExecuteProgram(sCmd, NULL);
		WK_TRACE(_T("Deleting: %s\n"), m_sLogFilesSource);
		DeleteDirRecursiv(m_sLogFilesSource);
	}

	CloseDebugger();

	DWORD dwError = GetLastError();
	CWinApp::ExitInstance();
	return m_nReturnCode;
}
