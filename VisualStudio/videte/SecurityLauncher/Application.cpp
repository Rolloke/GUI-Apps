// Application.cpp
//

#include "stdafx.h"
#include "SecurityLauncher.h"

#include "Application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSecurityLauncherApp theApp;
CWK_Timer theTimer;	

static TCHAR BASED_CODE szDialogClass[] = _T("#32770");

/////////////////////////////////////////////////////////////////////////////
// CApplication construction/destruction
CApplication::CApplication()
{
	Clear();
	m_StartTime = 500;
	m_bKeepRunning = true;
	m_iExitTry = 0;
	m_iCloseTry = 0;
	m_CreateStartTime = m_StartTime;
	m_bDialogBased = false;
	m_bLogged = false;
	m_bWaitUntilProcessEnd = false;
	m_Id = WAI_INVALID;
	m_iNrOfBChannels = 0;
	m_nMaxOutstandingConfirms=5;
	m_wUserID = SECID_NO_SUBID;
}
/////////////////////////////////////////////////////////////////////////////
CApplication::~CApplication()
{
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::SetExePathName(CString& appName)
{
	m_ExePathName = appName;
}
/////////////////////////////////////////////////////////////////////////////
CString CApplication::ReadExePathName()
{
	CString s = theApp.GetWorkingDirectory();

	if (WAI_PRODUCT_VIEW==m_Id)
	{
		return s + _T("\\ProductView.exe");
	}
	else if (WAI_PRODUCT_VERSION==m_Id)
	{
		return s + _T("\\ProductVersion.exe");
	}

	CString q;
	CString ExePathName;
	CWK_Profile wkp;

	q = wkp.GetString(theApp.GetModuleSection(),m_Key,_T(""));

	if (q.IsEmpty())
	{
		ExePathName = _T("");
	}
	else
	{
		if (q.GetLength()>2)
		{
			if (q[1]==_T(':'))
			{
				// seems to be full qualified path
				ExePathName = q;
			}
			else
			{
				if (q[0]==_T('\\'))
				{
					ExePathName = s + q;
				}
				else
				{
					ExePathName = s + _T('\\') + q;
				}
			}
		}
		else
		{
			ExePathName = s + _T('\\') + q;
		}

	}
	return ExePathName;
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::ExtractGPDialogName()
{
	CString str;
	int p;

	p = m_ExePathName.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		str = m_ExePathName.Mid(p+1);
	}
	else
	{
		str = m_ExePathName;
	}
	p = str.Find(_T('.'));
	
	if (p!=-1)
	{
		str = str.Left(p);
	}
	m_GPDialogName = str;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Create(WK_ApplicationId Id,BOOL bIsDialogBased,
						  DWORD dwStartTimeOut, 
						  int iExitTry, int iCloseTry, BOOL bKeepRunning)
{
	int p;

	m_Key = GetAppnameFromId(Id);
	m_WkAppName = m_Key;
	m_Id = Id;

	m_sName = COemGuiApi::GetApplicationName(Id);

	m_StartTime = dwStartTimeOut;
	m_bKeepRunning = bKeepRunning ? true : false;
	m_iExitTry = iExitTry;
	m_iCloseTry = iCloseTry;
	m_CreateStartTime = m_StartTime;

	m_bDialogBased = bIsDialogBased ? true : false;

	m_ExePathName = ReadExePathName();

	if (m_ExePathName.IsEmpty())
	{
		Clear();
		m_StartTime = 0;
		return FALSE;
	}

	ExtractGPDialogName();

	if (!DoesFileExist())
	{
		WK_TRACE_ERROR(_T("%s doesn't exist\n"),(LPCTSTR)m_ExePathName);
		m_ExePathName = _T("");
		Clear();
		m_StartTime = 0;
		return FALSE;
	}

	p = m_ExePathName.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		m_WorkingDirectory = m_ExePathName.Left(p);
	}
	m_sVersion = WK_GetFileVersion(m_ExePathName);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Create(const CString& sPathName, BOOL bIsDialogBased, LPCTSTR pszName/*=NULL*/)
{
	int p;

//	m_Key;
	if (pszName)
	{
		m_WkAppName = pszName;
	}
//	m_WkAppName;
//	m_Id;
//	m_sName = m_WkAppName;

	m_StartTime = 2000;
	m_bKeepRunning = false;
	m_iExitTry = 40;
	m_iCloseTry = 40;		 
	m_CreateStartTime = m_StartTime;

	m_bDialogBased = bIsDialogBased ? true : false;

	m_ExePathName = sPathName;
	if (m_ExePathName.IsEmpty())
	{
		Clear();
		m_StartTime = 0;
		return FALSE;
	}

	ExtractGPDialogName();

	if (!DoesFileExist())
	{
		m_ExePathName = theApp.GetWindowsDirectory() + _T("\\") + sPathName;

		if (!DoesFileExist())
		{
			m_ExePathName = theApp.GetSystemDirectory() + _T("\\") + sPathName;
			if (!DoesFileExist())
			{
				WK_TRACE_ERROR(_T("%s doesn't exist\n"),(LPCTSTR)m_ExePathName);
				m_ExePathName = _T("");
				Clear();
				m_StartTime = 0;
				return FALSE;
			}
		}
	}

	p = m_ExePathName.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		m_WorkingDirectory = m_ExePathName.Left(p);
		m_sName = m_ExePathName.Mid(p+1);
	}
	m_sVersion = WK_GetFileVersion(m_ExePathName);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Create(const CString& sKey, BOOL bIsDialogBased,
						  DWORD dwStartTimeOut, 
						  int iExitTry, int iCloseTry, BOOL bKeepRunning)
{
	int p;

	m_Key = sKey;
	m_WkAppName = m_Key;
//	m_Id = Id;

	m_sName = m_Key;

	m_StartTime = dwStartTimeOut;
	m_bKeepRunning = bKeepRunning ? true : false;
	m_iExitTry = iExitTry;
	m_iCloseTry = iCloseTry;
	m_CreateStartTime = m_StartTime;

	m_bDialogBased = bIsDialogBased ? true : false;

	m_ExePathName = ReadExePathName();
	if (m_ExePathName.IsEmpty())
	{
		Clear();
		m_StartTime = 0;
		return FALSE;
	}

	ExtractGPDialogName();

	if (!DoesFileExist())
	{
		WK_TRACE_ERROR(_T("%s doesn't exist\n"),(LPCTSTR)m_ExePathName);
		m_ExePathName = _T("");
		Clear();
		m_StartTime = 0;
		return FALSE;
	}

	p = m_ExePathName.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		m_WorkingDirectory = m_ExePathName.Left(p);
	}
	else
	{
		m_sName = m_ExePathName;
	}

	m_sVersion = WK_GetFileVersion(m_ExePathName);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::InitialStart()
{
	if (IsEmpty())
		return TRUE;

	if (!DoesFileExist())
		return FALSE;

	if (!WK_IS_RUNNING(m_WkAppName))
	{
		BOOL bRet;
		bRet = Execute(); // waits for input idle
		if (m_hWnd!=NULL) {
			WK_TRACE(_T("found\n"));
		}
		else {
			WK_TRACE_ERROR(_T("...not found %s\n"), m_sName);
		}
		return bRet;
	}
	else
	{
		WK_TRACE(_T("%s already running, searching window...\n"),m_sName);
		FindRunningProcess();
		return (m_hWnd != NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Execute()
{
	BOOL bProcess;
	DWORD dwStartTime;
	DWORD dwIdleTime;
	CFileStatus cfs;
	CString s,sTime;

	m_bLogged = false;
	if (m_ExePathName.IsEmpty())
		return FALSE;

	if (m_WkAppName == WK_APP_NAME_COCOUNIT)
	{
		HWND hWnd =	FindWindow(NULL,_T("cocosrv.exe"));
		if (hWnd)
		{
			DWORD dwPID;
			HANDLE hProcess;
			GetWindowThreadProcessId(hWnd,&dwPID);
			hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPID);
			WK_TRACE(_T("terminating cocosrv\n"));
			TerminateProcess(hProcess,0);
		}
	}
	if (GetId()==WAI_IDIP_CLIENT)
	{
		AfxGetMainWnd()->SetForegroundWindow();
	}

    m_StartUpInfo.dwFlags	      = /*STARTF_FORCEONFEEDBACK |*/ STARTF_USESHOWWINDOW;
    m_StartUpInfo.wShowWindow     = (WORD)(m_bKeepRunning ? (theApp.AmIShell() ? SW_HIDE : SW_SHOWMINNOACTIVE): SW_SHOWNORMAL);

	CFile::GetStatus(m_ExePathName,cfs);
	sTime = cfs.m_mtime.Format(_T("%d.%m.%y %H:%M:%S"));

	CString sCommandline;
	CString sExe(m_ExePathName);
	LPCTSTR pExe = NULL;
	if (sExe.Find(_T(' ')))
	{
		sCommandline = _T("\"") + sExe + _T("\"");
		pExe = NULL;
	}
	else
	{
		pExe = (LPCTSTR)sExe;
	}

	SetCurrentDirectory(m_WorkingDirectory);
	dwStartTime = GetTickCount();

	// RKE: Backup mode also for the IdipClient?
	if (GetId()==WAI_DATABASE_CLIENT)
	{
		if (theApp.IsBackupMode())
		{
			WK_TRACE(_T("starting recherche in backup mode\n"));
			sCommandline = sCommandline + pExe + CMD_PARAM_BACKUP;
			pExe = NULL;
		}
	}

	if (m_wUserID != SECID_NO_SUBID)
	{
		CUserArray	UserArray;
		CWK_Profile	prof;
		UserArray.Load(prof); 
		CUser *pUser = UserArray.GetUser(CSecID(SECID_GROUP_USER, m_wUserID));
		if (pUser)
		{
			CString sUser	  = pUser->GetName();
			CString sPassword = pUser->GetPassword();
			sCommandline = sCommandline + pExe + CMD_PARAM_USER + sUser + CMD_PARAM_PASSWORD + sPassword;
			pExe = NULL;
		}
		m_wUserID = SECID_NO_SUBID;
	}
	if (!m_sCommandLine.IsEmpty())
	{
		sCommandline = sCommandline + pExe + m_sCommandLine;
		m_sCommandLine.Empty();
	}

	WK_TRACE(_T("starting %s <%s> date %s version %s\n"), pExe ? pExe : _T(""), 
		sCommandline, sTime, m_sVersion);
	bProcess = CreateProcess(
						pExe,						// LPCTSTR pointer to name of executable module 
						sCommandline.GetBuffer(0),	// LPTSTR pointer to command line string
						NULL,						// LPSECURITY_ATTRIBUTES pointer to process security attributes 
						NULL,						// LPSECURITY_ATTRIBUTES pointer to thread security attributes 
						FALSE,						// handle inheritance flag 
						NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
						NULL,						// LPVOID pointer to new environment block 
						m_WorkingDirectory,			// LPCTSTR pointer to current directory name 
						&m_StartUpInfo,				// LPSTARTUPINFO pointer to STARTUPINFO 
						&m_ProzessInformation 		// LPPROCESS_INFORMATION pointer to PROCESS_INFORMATION  
						);

	sCommandline.ReleaseBuffer();
	if (bProcess)
	{
		DWORD rWFII;
		rWFII=WaitForInputIdle(m_ProzessInformation.hProcess,m_StartTime);
		if (rWFII==0)
		{
			dwIdleTime = GetTickCount();	
			WK_TRACE(_T("%s started %d ms\n"),(LPCTSTR)m_sName, dwIdleTime-dwStartTime);
		}
		else if(rWFII==WAIT_TIMEOUT)
		{
			WK_TRACE_WARNING(_T("%s wait for input idle timeout %d\n"),(LPCTSTR)m_sName, m_StartTime);
		}
		FindOutWindowHandle();

//		if (theApp.AmIShell())
		if (!theApp.AmIShell())
		{
			if (0==m_Key.CompareNoCase(_T("WinClk")))
			{
				WK_TRACE(_T("trying to hide winclk window %08lx\n"),m_hWnd);
//				MoveWindow(m_hWnd,-1000,-1000,200,200,FALSE);
//				ShowWindow(m_hWnd,SW_HIDE);
//				ShowWindow(m_hWnd,SW_HIDE);
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("%s application not started %s\n"),m_sName,GetLastErrorString());
	}
	WK_STAT_LOG(_T("Run"),bProcess,m_sName);
	if (m_bWaitUntilProcessEnd)
	{
		DWORD dwTimeOut = m_dwExitCode;
		m_dwExitCode = 0;
		WaitForSingleObject(m_ProzessInformation.hProcess, dwTimeOut);
		GetExitCodeProcess(m_ProzessInformation.hProcess, &m_dwExitCode);
	}
	return bProcess;
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::Clear()
{
	WK_CLOSE_HANDLE(m_ProzessInformation.hProcess);
	WK_CLOSE_HANDLE(m_ProzessInformation.hThread);
	m_ProzessInformation.dwProcessId = NULL;
	m_ProzessInformation.dwThreadId = NULL;
	m_hWnd = NULL;
	m_nOutstandingConfirms = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::IsEmpty()
{
	return 	(m_ProzessInformation.hProcess == NULL) &&
			(m_ProzessInformation.hThread == NULL) &&
			(m_ProzessInformation.dwProcessId == NULL) &&
			(m_ProzessInformation.dwThreadId == NULL) &&
			(m_hWnd == NULL) &&
			(m_StartTime == 0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::IsRunning(BOOL bTrace /*=FALSE*/, DWORD dwTimeOut/*=0*/)
{
	if (m_ProzessInformation.hProcess == NULL)
		return FALSE;

	if (dwTimeOut)
	{
		WaitForSingleObject(m_ProzessInformation.hProcess, dwTimeOut);
	}
	if (!GetExitCodeProcess(m_ProzessInformation.hProcess,&m_dwExitCode))
	{
		if (bTrace)
		{
			WK_TRACE_WARNING(_T("no programm exit code %s\n"),m_sName);
		}
		m_hWnd = NULL;
		return FALSE;
	}
	
	if (m_dwExitCode != STILL_ACTIVE)
	{
		if (bTrace)
		{
			WK_TRACE_WARNING(_T("programm does not run %s\n"),m_sName);
		}
		m_hWnd = NULL;
		return FALSE;
	}

	if (m_hWnd)
	{
		if (!IsWindow(m_hWnd))
		{
			m_hWnd = NULL;
			FindOutWindowHandle();
			if (m_hWnd == NULL)
			{
				if (bTrace)
				{
					WK_TRACE_WARNING(_T("no window %s\n"),m_sName);
				}
				return FALSE;
			}
		}
	}

	return WK_IS_RUNNING(m_Key);
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::Terminate()
{
	DWORD dwTime1 = GetTickCount();
	DWORD dwTime2;
	CString s;

	if (IsEmpty())
		return;

	if (!IsRunning())
		return;

	if (Exit())
	{
		dwTime2 = GetTickCount();
		s.Format(_T("%s normal beendet %d ms"),(LPCTSTR)m_sName,dwTime2-dwTime1);
	}
	else
	{
		if (Close())
		{
			dwTime2 = GetTickCount();
			s.Format(_T("%s closed %d ms"),(LPCTSTR)m_sName,dwTime2-dwTime1);
		}
		else
		{
			if (Quit())
			{
				dwTime2 = GetTickCount();
				s.Format(_T("%s quitted %d ms"),(LPCTSTR)m_sName,dwTime2-dwTime1);
			}
			else
			{
				if (Shutdown())
				{
					dwTime2 = GetTickCount();
					s.Format(_T("ERROR:   %s terminated %d ms"),
										(LPCTSTR)m_sName,dwTime2-dwTime1);
				}
				else
				{
					dwTime2 = GetTickCount();
					s.Format(_T("ERROR:   %s Shutdown FAILED in %d ms"),
										(LPCTSTR)m_sName,dwTime2-dwTime1);
				}
			}
		}
	}

	WK_TRACE(_T("%s\n"),s);
	Clear();
	HandleGPDialog();
	WK_STAT_LOG(_T("Run"),0,m_sName);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Exit()
{
	DWORD time = m_iExitTry;
	BOOL bRet = TRUE;

	if (time==0)
		return FALSE;

	if (m_hWnd==NULL)
	{
		FindOutWindowHandle();
	}
	if (m_hWnd)
	{
		if (IsDialogBased())
		{
			HWND hWndOK = GetDlgItem(m_hWnd,IDOK);
			if (hWndOK)
			{
				WORD id = (WORD)GetDlgCtrlID(hWndOK);
				::PostMessage(m_hWnd, WM_COMMAND, (WPARAM)MAKELONG(id,BN_CLICKED), (LPARAM)hWndOK);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0L);
		}

		DWORD dwReturn = WAIT_TIMEOUT;
		do
		{
			dwReturn = WaitForSingleObject(m_ProzessInformation.hProcess,50);
			time--;
		}
		while (    (TRUE==(bRet=IsRunning())) 
			    && (time>0)
				&& (WAIT_TIMEOUT==dwReturn));
	}
	else
	{
		WK_TRACE_WARNING(_T("Exit %s; m_hWnd not found\n"), m_sName);
	}
	return !bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Close()
{
	DWORD time = m_iCloseTry;
	BOOL bRet = TRUE;

	if (time==0)
		return FALSE;

	if (m_hWnd==NULL)
	{
		FindOutWindowHandle();
	}
	if (m_hWnd)
	{
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0L);
		do
		{
			Sleep(50);
			time--;
		}
		while ((TRUE==(bRet=IsRunning())) && (time>0));
	}
	else
	{
		WK_TRACE_WARNING(_T("Close %s; m_hWnd not found\n"), m_sName);
	}
	return !bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Quit()
{
	DWORD time = 10;
	BOOL bRet = TRUE;

	if (m_StartTime)
	{
		time = m_StartTime / 50;
	}

	if (m_hWnd)
	{
		::PostMessage(m_hWnd, WM_QUIT, 0, 0L);
		do
		{
			Sleep(50);
			time--;
		}
		while ((TRUE==(bRet=IsRunning())) && (time>0));
	}
	else
	{
		WK_TRACE_WARNING(_T("Exit %s; m_hWnd not found\n"), m_sName);
	}
	return !bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Shutdown()
{
	if (m_hWnd && IsWindow(m_hWnd))
	{
		NOTIFYICONDATA tnd;
		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;
		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}

	GetExitCodeProcess(m_ProzessInformation.hProcess,&m_dwExitCode);
	return TerminateProcess(m_ProzessInformation.hProcess,m_dwExitCode);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::HandleGPDialog()
{
	if (theApp.IsWin95())
	{
		return HandleGPDialog95();
	}
	else if (theApp.IsNT())
	{
		return HandleGPDialogNT();
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::HandleGPDialog95()
{
	HWND hWnd,hFirstChild,hSecondChild,hThirdChild,hFourthChild;
	CString s;
	TCHAR sz[128];

	hWnd = FindWindow(szDialogClass,m_GPDialogName);

	if ((hWnd==NULL) || (hWnd==m_hWnd) )
		return FALSE;
	
	hFirstChild = GetWindow(hWnd, GW_CHILD);
	if (hFirstChild==NULL)
		return FALSE;
	
	hSecondChild = GetWindow(hFirstChild, GW_HWNDNEXT);
	if (hSecondChild==NULL)
		return FALSE;

	hThirdChild = GetWindow(hSecondChild, GW_HWNDNEXT);
	if (hThirdChild==NULL)
		return FALSE;

	GetWindowText(hThirdChild, sz, sizeof(sz));
	s = sz;

	if (s == _T("&Details>>"))
	{
		WORD id = (WORD)GetDlgCtrlID(hThirdChild);
		WK_TRACE(_T("GP dialog found %s\n"),m_sName);
		WK_STAT_LOG(_T("Run"),0,m_sName);	
		SendMessage(hWnd,WM_COMMAND,(WPARAM)MAKELONG(id,BN_CLICKED),(LPARAM)hThirdChild);
	
		hFourthChild = GetWindow(hThirdChild, GW_HWNDNEXT);
		if (hFourthChild)
		{
			HGLOBAL hClip;
		    LPTSTR  lptstr; 

			// it's the details edit control
			// select all
			SendMessage(hFourthChild, EM_SETSEL,0,-1);
			// copy to clipboard
			SendMessage(hFourthChild, WM_COPY,0,0);
			// get data from clipboard
			if (IsClipboardFormatAvailable(CF_TEXT))
			{
				if (OpenClipboard(AfxGetMainWnd()->m_hWnd))
				{
					hClip = GetClipboardData(CF_TEXT); 
					if (hClip != NULL) 
					{
						lptstr = (LPTSTR)GlobalLock(hClip); 
						if (lptstr != NULL) 
						{ 
							int p;
							CString tr;
							
							// trace only the first and second line
							s = lptstr;
							p = s.Find(_T("\r\n"));
							if (p!=-1)
							{
								tr += s.Left(p);
								tr += _T(" ");
								s = s.Mid(p+2);
								p = s.Find(_T("\r\n"));
								if (p!=-1)
								{
									tr += s.Left(p);
								}
								else
								{
									tr += s;
								}
							}
							else
							{
								tr = s;
							}
							WK_TRACE(_T("%s\n"),tr);
						}
						GlobalUnlock(hClip); 
					} 
					EmptyClipboard();
			        CloseClipboard(); 
				}
			} 
		}
		WK_TRACE(_T("closing GP dialog %s\n"),m_sName);
		::PostMessage(hWnd, WM_CLOSE, 0, 0L);
		theApp.IncreaseGPCounter();
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::HandleGPDialogNT()
{
	HWND hWnd,hChild1,hChild2,hChild3,hChild4;
	CString s;
	TCHAR sz[256];

	hWnd = FindWindow(szDialogClass,m_GPDialogName);

	if ((hWnd==NULL) || (hWnd==m_hWnd) )
		return FALSE;
	
	// OK Button
	hChild1 = GetWindow(hWnd, GW_CHILD);
	if (hChild1==NULL)
		return FALSE;
	
	// Abbrechen Button
	hChild2 = GetWindow(hChild1, GW_HWNDNEXT);
	if (hChild2==NULL)
		return FALSE;

	// Icon
	hChild3 = GetWindow(hChild2, GW_HWNDNEXT);
	if (hChild3==NULL)
		return FALSE;

	// Meldung aus zwei Zeilen
	// Fehler und Anweisung OK zu druecken
	// wir wollen nur den Fehler
	hChild4 = GetWindow(hChild3, GW_HWNDNEXT);
	if (hChild4==NULL)
		return FALSE;

	GetWindowText(hChild4, sz, sizeof(sz));
	s = sz;
	s = s.Left(s.Find(_T("\n\n")));
	WK_TRACE(_T("GP dialog found %s\n"),m_sName);
	WK_TRACE(_T("%s\n"),s);
	WK_STAT_LOG(_T("Run"),0,m_sName);	
	WK_TRACE(_T("closing GP dialog %s\n"),m_sName);
	WORD id = (WORD)GetDlgCtrlID(hChild1);
	SendMessage(hWnd,WM_COMMAND,(WPARAM)MAKELONG(id,BN_CLICKED),(LPARAM)hChild1);
	theApp.IncreaseGPCounter();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::HandleCRTMessageBox()
{
	HWND hWnd,hFirstChild,hSecondChild,hThirdChild;
	CString s;
	TCHAR sz[512];

	hWnd = FindWindow(szDialogClass,_T("Microsoft Visual C++ Runtime Library"));

	if ((hWnd==NULL))
		return FALSE;

	// the OK Button
	hFirstChild = GetWindow(hWnd, GW_CHILD);
	if (hFirstChild==NULL)
		return FALSE;
	
	// the Message Box Icon
	hSecondChild = GetWindow(hFirstChild, GW_HWNDNEXT);
	if (hSecondChild==NULL)
		return FALSE;

	// the MessageBox Text;
	hThirdChild = GetWindow(hSecondChild, GW_HWNDNEXT);
	if (hThirdChild==NULL)
		return FALSE;

	GetWindowText(hThirdChild, sz, sizeof(sz));
	s = sz;
	int i,c;
	c = s.GetLength();
	for (i=0;i<c;i++)
	{
		if (s[i]==_T('\n'))
		{
			s.SetAt(i,_T(' '));
		}
		if (s[i]==_T('\r'))
		{
			s.SetAt(i,_T(' '));
		}
	}
	WK_TRACE(_T("found CRT Error: %s\n"),s);
	WORD id = (WORD)GetDlgCtrlID(hFirstChild);
	SendMessage(hWnd,WM_COMMAND,(WPARAM)MAKELONG(id,BN_CLICKED),(LPARAM)hFirstChild);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::DoesFileExist()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(m_ExePathName, &FindFileData);
	FindClose(h);
	if (h == INVALID_HANDLE_VALUE)
		return FALSE;
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CApplication::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	CApplication* pThis = (CApplication*)lParam;
	DWORD dwPID = (DWORD)pThis->m_ProzessInformation.dwProcessId;
	DWORD dwTempPID = 0;

	GetWindowThreadProcessId(hWnd, &dwTempPID);
	if (dwPID == dwTempPID)
	{
		pThis->m_hWnd = hWnd;
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::FindOutWindowHandle()
{
	if (IsDialogBased())
	{
		m_hWnd = FindWindow(szDialogClass,m_WkAppName);
		if (!m_hWnd)
			m_hWnd = FindWindow(szDialogClass,m_sName);
	}
	else
	{
		if (0==m_Key.CompareNoCase(_T("WinClk")))
		{
			m_hWnd = FindWindow(_T("TForm1"),NULL);
		}
		else
		{
			m_hWnd = FindWindow(m_WkAppName,NULL);
		}
	}
	if (m_hWnd == NULL)
	{
		if (m_ProzessInformation.hProcess)
		{
			EnumWindows((WNDENUMPROC)EnumWindowsProc,(LPARAM)this);
			if (m_hWnd)
			{
				HWND hWnd = m_hWnd;

				for (;;)
				{
					hWnd = GetParent(m_hWnd);
					if (hWnd)
					{
						DWORD dwTempPID = 0;
						GetWindowThreadProcessId(hWnd, &dwTempPID);
						if (m_ProzessInformation.dwProcessId == dwTempPID)
						{
							m_hWnd = hWnd;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Actualize(BOOL bTrace)
{
	CString sTemp = ReadExePathName();

	if (sTemp.CompareNoCase(m_ExePathName)!=0)
	{
		// exepath in registry differs from internal quit app
		// restart if new path is valid
		Terminate();
		Clear();
		m_ExePathName.Empty();
		if (sTemp.IsEmpty()==FALSE)
		{
			m_ExePathName = sTemp;
			ExtractGPDialogName();

			if (!DoesFileExist())
			{
				m_ExePathName.Empty();
				Clear();
				m_StartTime = 0;
				return TRUE;
			}

			int p = m_ExePathName.ReverseFind(_T('\\'));
			if (p!=-1)
			{
				m_WorkingDirectory = m_ExePathName.Left(p);
			}
			m_StartTime = m_CreateStartTime;
			if (KeepAlive())
			{
				return InitialStart();
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			Clear();
			m_StartTime = 0;
			return TRUE;
		}
	}

	if (IsEmpty())
	{
		return TRUE;
	}

	if (0!=m_Key.CompareNoCase(_T("WinClk")))
	{
		if (WK_IS_RUNNING(m_WkAppName) && (m_ProzessInformation.hProcess == NULL))
		{
			FindRunningProcess(FALSE);
		}
	}

	HandleGPDialog();

	if (IsRunning(bTrace))
	{
		MakeSelfCheck();

		// still running
		return TRUE;
	}
	else
	{
		Clear();
		HandleGPDialog();

		if (KeepAlive())
		{
			WK_STAT_LOG(_T("Run"),0,m_sName);	
			Terminate();
			return InitialStart();
		}
		else
		{
			if (m_bLogged==false)
			{
				WK_STAT_LOG(_T("Run"),0,m_sName);	
				m_bLogged = true;
			}
			return TRUE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::BringToTop()
{
	if (m_hWnd==NULL)
	{
		FindOutWindowHandle();
	}
	if (m_hWnd)
	{
		if (IsWindow(m_hWnd))
		{
			if (IsIconic(m_hWnd))
			{
				ShowWindow(m_hWnd,SW_RESTORE);
			}
// gf BringWindowToTop does not have the wanted effect at all conditions
//			if (!BringWindowToTop(m_hWnd))
			if (!SetForegroundWindow(m_hWnd))
			{
				WK_TRACE_WARNING(_T("SetForegroundWindow FAILED %s\n"), m_sName);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::FindRunningProcess(BOOL bTrace)
{
	DWORD dwPID;

	Clear();

	FindOutWindowHandle();

	if (m_hWnd)
	{
		GetWindowThreadProcessId(m_hWnd,&dwPID);

		m_ProzessInformation.hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPID);
		if (m_ProzessInformation.hProcess!=NULL)
		{
			m_ProzessInformation.dwProcessId = dwPID;
			if (bTrace)
				WK_TRACE(_T(" ... found running program\n"));
		}
		else
		{
			Clear();
			if (bTrace)
				WK_TRACE(_T(" ... did not found running program\n"));
		}
	}
	else
	{
		Clear();
		if (bTrace)
		{
			WK_TRACE(_T(" ... did not found window\n"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::SetNrOfBChannels(int iNrOfBChannels)
{
	m_iNrOfBChannels = iNrOfBChannels;
}
/////////////////////////////////////////////////////////////////////////////
int CApplication::GetNrOfBChannels()
{
	return m_iNrOfBChannels;
}

/////////////////////////////////////////////////////////////////////////////
void CApplication::MakeSelfCheck()
{
	if (m_nOutstandingConfirms < m_nMaxOutstandingConfirms)
	{
		SendSelfCheckMessage();
	}
	else
	{
		WK_TRACE_WARNING(_T("No response from %s... Terminating process\n"), m_sName);
		Terminate();
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::SendSelfCheckMessage()
{
	BOOL bResult		= FALSE;

	if (IsRunning())
	{
		// CAVEAT: Process may still be running, but window may be killed by GP!
		// Always count tries, to watchdog these dead applications
		m_nOutstandingConfirms++;

		FindOutWindowHandle();

		if (IsWindow(m_hWnd))
		{
			bResult = ::PostMessage(m_hWnd, WM_SELFCHECK, (WPARAM)AfxGetMainWnd()->m_hWnd, m_Id);
//			m_dwSendTimeTC = WK_GetTickCount();	// only for trace, not used
		}
	}


	return bResult;

}
/////////////////////////////////////////////////////////////////////////////
void CApplication::ConfirmSelfcheckMessage()
{
// CAVEAT: Uncomment the setting of m_dwSendTimeTC in SendSelfCheckMessage()
//	WK_TRACE(_T("Confirmtime=%lums (%s)\n"), WK_GetTickCount() - m_dwSendTimeTC, m_WkAppName);
	m_nOutstandingConfirms = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::SetUserID(WORD wUserID)
{
	m_wUserID = wUserID;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_hWnd == NULL)
	{
		FindOutWindowHandle();
	}
	if (m_hWnd)
	{
		::PostMessage(m_hWnd, nMsg, wParam, lParam);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::SendMessageTimeOut(UINT nMsg, WPARAM wParam, LPARAM lParam, UINT uTimeOut, DWORD*pdwResult)
{
	if (m_hWnd == NULL)
	{
		FindOutWindowHandle();
	}
	if (m_hWnd)
	{
		if (::SendMessageTimeout(m_hWnd, nMsg, wParam, lParam, SMTO_ABORTIFHUNG, uTimeOut, pdwResult))
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CApplication::SetCloseTry(int nTry)
{
	int nOld = m_iCloseTry;
	m_iCloseTry = nTry;
	return nOld;
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::SetWaitUntilProcessEnd(bool bWait, DWORD dwTimeOut/*=INFINITE*/)
{
	m_dwExitCode = dwTimeOut;
	m_bWaitUntilProcessEnd = bWait;
}
