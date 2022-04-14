// CApplication.cpp: implementation of the CApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStarter.h"
#include "CApplication.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CDVStarterApp theApp;
CWK_Timer theTimer;	

#define MAX_OUTSTANDING_CONFIRMS 5

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CApplication::CApplication()
{
	ZeroMemory(&m_ProzessInformation, sizeof(PROCESS_INFORMATION));
	m_hWnd					= NULL;
	m_dwCloseTimeOut		= 5000;
	m_dwQuitTimeOut			= 5000;
	m_dwExitTimeOut			= 15000;
	m_dwLoadTimeout			= 5000;
	m_nOutstandingConfirms	= 0;
	m_bAutoTerminate		= FALSE;
	m_bWaitUntilProcessEnd	= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Create(const CString &sExeName, WK_ApplicationId id, int nMaxOutstandingConfirms)
{
	CWK_Profile prof;
	int nStart = prof.GetInt(DV_DVSTARTER_DEBUG, sExeName, -1);
	if (nStart == -1)
	{
		nStart = 1;	// default set to 1
		prof.WriteInt(DV_DVSTARTER_DEBUG, sExeName, nStart);
	}
	if (nStart)
	{
		m_sExeName					= sExeName;
		m_sWkAppName				= GetAppnameFromId(id);
		TRACE(_T("Create <%s> <%s>\n"), m_sExeName, m_sWkAppName);
	}
	else
	{
		m_sWkAppName = sExeName;
		WK_TRACE(_T("Not Starting %s\n"), sExeName);
	}
	m_AppId						= id;
	m_nMaxOutstandingConfirms	= nMaxOutstandingConfirms;

	theApp.AddApplication(this);

	return TRUE; //Load(SW_MINIMIZE);
}

//////////////////////////////////////////////////////////////////////
CApplication::~CApplication()
{
	theApp.RemoveApplication(this);
	if (!m_sWkAppName.IsEmpty())
	{
		WK_TRACE(_T("~CApplication <%s>\n"), m_sWkAppName);
	}
	if (m_bAutoTerminate)
	{
		Terminate();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CApplication::SetAutoTerminate(BOOL bTerminate)
{
    m_bAutoTerminate = bTerminate;
}

/////////////////////////////////////////////////////////////////////////////
void CApplication::Clear()
{
	WK_CLOSE_HANDLE(m_ProzessInformation.hProcess);
	WK_CLOSE_HANDLE(m_ProzessInformation.hThread);
	m_ProzessInformation.dwProcessId = NULL;
	m_ProzessInformation.dwThreadId = NULL;
	m_nOutstandingConfirms = 0;
	m_hWnd = NULL;
}

//////////////////////////////////////////////////////////////////////
BOOL CApplication::Load(DWORD dwStartFlags, DWORD dwTimeOut/*=5000*/)
{
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	DWORD				dwStartTime			= GetTickCount();
	CString				sCmdLine			= _T("");
	CString				sWorkingDirectory	= _T(".");
	BOOL				bWait				= FALSE;
	HANDLE				hProzess			= NULL;
	BOOL				bStatus				= FALSE;
	BOOL				bProcess			= FALSE;
	
	if (dwTimeOut == INVALID_DWORD_VALUE)
	{
		dwTimeOut = m_dwLoadTimeout;
	}
	else
	{
		m_dwLoadTimeout = dwTimeOut;
	}

	if (m_sExeName.IsEmpty())
		return FALSE;

	m_bAutoTerminate = TRUE;

	// Läuft der Prozess schon?
	if (FindRunningProcess(FALSE))
		return TRUE;

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = (WORD)(theApp.AmIShell() ? SW_HIDE : dwStartFlags);
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	if (!m_sCommandLine.IsEmpty())
	{
		sCmdLine = m_sCommandLine;
		m_sCommandLine.Empty();
	}
	CString sCommandLine;
	sCommandLine.Format(_T("%s %s"), LPCTSTR(m_sExeName), LPCTSTR(sCmdLine));

	int nIndex = m_sExeName.Find(_T(".exe"));
	if (nIndex != -1)
		theApp.ShowInfoText(_T("Loading ") + m_sExeName.Left(nIndex) + _T("..."));

	bProcess = CreateProcess(
							NULL, // (LPCSTR)m_sAppName,				// pointer to name of executable module 
							(LPTSTR)sCommandLine.GetBuffer(0),			// pointer to command line string
							(LPSECURITY_ATTRIBUTES)NULL,				// pointer to process security attributes 
							(LPSECURITY_ATTRIBUTES)NULL,				// pointer to thread security attributes 
							FALSE,										// handle inheritance flag 
							NORMAL_PRIORITY_CLASS,						//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
							(LPVOID)NULL,								// pointer to new environment block 
							(LPCTSTR)sWorkingDirectory,					// pointer to current directory name 
							(LPSTARTUPINFO)&startUpInfo,				// pointer to STARTUPINFO 
							(LPPROCESS_INFORMATION)&prozessInformation 	// pointer to PROCESS_INFORMATION  
							);
	sCommandLine.ReleaseBuffer();
	
	if (bProcess)
	{
		DWORD rWFII;

		rWFII = WaitForInputIdle(prozessInformation.hProcess, dwTimeOut);
		if (rWFII == 0)
			WK_TRACE(_T("%s gestartet %d ms\n"),LPCTSTR(sCommandLine), GetTimeSpan(dwStartTime));
		else if (rWFII == WAIT_TIMEOUT)
			WK_TRACE_ERROR(_T("%s wait for input idle timeout %d ms\n"),LPCTSTR(sCommandLine), GetTimeSpan(dwStartTime));
		
		if (bWait)
			WaitForSingleObject(prozessInformation.hProcess,INFINITE);
		hProzess = prozessInformation.hProcess;
		m_ProzessInformation = prozessInformation;
		bStatus = TRUE;
	
		FindOutWindowHandle();
		if (m_ProzessInformation.hProcess && m_hWnd)
		{
			WK_TRACE(_T("HWND and Process found %s\n"),m_sWkAppName);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Could not start %s\n"),m_sWkAppName);
	}

	if (m_bWaitUntilProcessEnd)
	{
		WaitForSingleObject(m_ProzessInformation.hProcess, dwTimeOut);
		GetExitCodeProcess(m_ProzessInformation.hProcess, &m_dwExitCode);
	}

	theApp.HideInfoText();
	return bStatus;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::FindRunningProcess(BOOL bTrace)
{
	DWORD	dwPID= 0;
	BOOL	bRet = FALSE;
	
	FindOutWindowHandle();

	if (m_hWnd && m_ProzessInformation.hProcess == NULL)
	{
		GetWindowThreadProcessId(m_hWnd,&dwPID);

		m_ProzessInformation.hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPID);
		if (m_ProzessInformation.hProcess!=NULL)
		{
			m_ProzessInformation.dwProcessId = dwPID;
			if (bTrace)
				WK_TRACE(_T(" ... found running program\n"));
			bRet = TRUE;
		}
		else
		{
			Clear();
			if (bTrace)
				WK_TRACE(_T(" ... did not found running program\n"));
		}
	}

	return bRet;
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
	if (m_hWnd == NULL || !::IsWindow(m_hWnd))
	{
		m_hWnd = FindWindow(m_sWkAppName,NULL);
		if (m_hWnd == NULL)
		{
			m_hWnd = FindWindow(NULL, m_sWkAppName);
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
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::IsRunning(BOOL bTrace /*=FALSE*/)
{
	if (m_ProzessInformation.hProcess == NULL)
		return FALSE;

	if (!GetExitCodeProcess(m_ProzessInformation.hProcess,&m_dwExitCode))
	{
		if (bTrace)
		{
            WK_TRACE_WARNING(_T("no programm exit code %s\n"),m_sWkAppName);
		}
		m_hWnd = NULL;
		return FALSE;
	}
	
	if (m_dwExitCode != STILL_ACTIVE)
	{
		if (bTrace)
		{
			WK_TRACE_WARNING(_T("programm does not run %s\n"),m_sWkAppName);
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
					WK_TRACE_WARNING(_T("no window %s\n"),m_sWkAppName);
				}
				return FALSE;
			}
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CApplication::Terminate()
{
	DWORD dwTime1 = GetTickCount();
	CString s;

	if (IsEmpty())
		return;

	if (!IsRunning())
		return;

	if (Exit())
		s.Format(_T("%s normal beendet %d ms"),LPCTSTR(m_sWkAppName), GetTimeSpan(dwTime1));
	else
	{
		if (Close())
			s.Format(_T("%s closed %d ms"),LPCTSTR(m_sWkAppName), GetTimeSpan(dwTime1));
		else
		{
			if (Quit())
				s.Format(_T("%s quitted %d ms"),LPCTSTR(m_sWkAppName), GetTimeSpan(dwTime1));
			else
			{
				if (Shutdown())
					s.Format(_T("ERROR:   %s terminated %d ms"), LPCTSTR(m_sWkAppName), GetTimeSpan(dwTime1));
				else
					s.Format(_T("ERROR:   %s NOT terminated %d ms"), LPCTSTR(m_sWkAppName), GetTimeSpan(dwTime1));
			}
		}
	}

	WK_TRACE(_T("%s\n"),s);
	Clear();
	WK_STAT_LOG(_T("Run"),0,m_sWkAppName);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Exit()
{
	if (m_hWnd==NULL)
	{
		FindOutWindowHandle();
	}
	
	if (!m_hWnd || !IsWindow(m_hWnd))
		return FALSE;
	
/*	HWND hWndOK = GetDlgItem(m_hWnd,IDOK);
	if (hWndOK)
	{
		WORD id = (WORD)GetDlgCtrlID(hWndOK);
		PostMessage(m_hWnd,WM_COMMAND,(WPARAM)MAKELONG(id,BN_CLICKED),(LPARAM)hWndOK);
	}
*/
	WK_TRACE(_T("post WM_COMMAND ID_APP_EXIT\n"));
	::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0L);

	return (WaitForSingleObject(m_ProzessInformation.hProcess, m_dwExitTimeOut) == WAIT_OBJECT_0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Close()
{
	if (!m_hWnd || !IsWindow(m_hWnd))
		return FALSE;

	::PostMessage(m_hWnd, WM_CLOSE, 0, 0L);

	return (WaitForSingleObject(m_ProzessInformation.hProcess, m_dwCloseTimeOut) == WAIT_OBJECT_0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Quit()
{
	if (!m_hWnd || !IsWindow(m_hWnd))
		return FALSE;
	
	::PostMessage(m_hWnd, WM_QUIT, 0, 0L);

	return (WaitForSingleObject(m_ProzessInformation.hProcess, m_dwQuitTimeOut) == WAIT_OBJECT_0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::Shutdown()
{
	GetExitCodeProcess(m_ProzessInformation.hProcess,&m_dwExitCode);
	return TerminateProcess(m_ProzessInformation.hProcess,m_dwExitCode);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::IsEmpty()
{
	return 	(m_ProzessInformation.hProcess == NULL) &&
			(m_ProzessInformation.hThread == NULL) &&
			(m_ProzessInformation.dwProcessId == NULL) &&
			(m_ProzessInformation.dwThreadId == NULL) &&
			(m_hWnd == NULL);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::MakeSelfCheck()
{
	if (m_nOutstandingConfirms < m_nMaxOutstandingConfirms)
	{
		SendSelfCheckMessage();
		return TRUE;
	}
	else
	{
		WK_TRACE_WARNING(_T("No response from %s... Terminating process\n"), m_sWkAppName);
		Terminate();
		return FALSE;
	}
}
////////////////////////////////////////////////////////////////////////////
BOOL CApplication::SendSelfCheckMessage()
{
	BOOL bResult = FALSE;

	if (IsRunning())
	{
		// CAVEAT: Process may still be running, but window may be killed by GP!
		// Always count tries, to watchdog these dead applications
		FindOutWindowHandle();

		if (IsWindow(m_hWnd))
		{
			bResult = ::PostMessage(m_hWnd, WM_SELFCHECK, (WPARAM)AfxGetMainWnd()->m_hWnd, m_AppId);
		}
		else
		{
			WK_TRACE_WARNING(_T("window not found for WM_SELFCHECK %s,%s\n"),
				m_sWkAppName,m_sExeName);
		}
		if (bResult)
		{
			m_nOutstandingConfirms++;
		}
	}

	return bResult;

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
////////////////////////////////////////////////////////////////////////////
LRESULT CApplication::PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = FALSE;

	if (IsRunning())
	{
		FindOutWindowHandle();

		if (IsWindow(m_hWnd))
		{
			bResult = ::PostMessage(m_hWnd, nMsg, wParam, lParam);
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::ConfirmSelfcheckMessage()
{
// CAVEAT: Uncomment the setting of m_dwSendTC in SendSelfCheckMessage()
//	WK_TRACE(_T("ConfirmTime=%lums (%s)\n"), WK_GetTickCount() - m_dwSendTC, m_sWkAppName);
	m_nOutstandingConfirms = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplication::WasCreated()
{
	CString sKey;
	BOOL bCreated = FALSE;
	if (!m_sExeName.IsEmpty())
	{
		bCreated = TRUE;
		sKey = m_sExeName;
	}
	else
	{
		sKey = m_sWkAppName;
	}
	if (!sKey.IsEmpty())
	{
		CWK_Profile prof;
		int nStart = prof.GetInt(DV_DVSTARTER_DEBUG, sKey, 1);
		if (nStart)
		{
			if (!bCreated)
			{
				Create(sKey, m_AppId, m_nMaxOutstandingConfirms);
				WK_TRACE(_T("Creating %s\n"), m_sWkAppName);
				bCreated = TRUE;
			}
		}
		else
		{
			if (bCreated)
			{
				Terminate();
				WK_TRACE(_T("Terminating %s\n"), m_sWkAppName);
				m_sWkAppName = m_sExeName;
				m_sExeName.Empty();
				bCreated = FALSE;
			}
		}
	}
	return bCreated;	
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::SetTemporaryCmdLine(LPCTSTR sCmdLine)
{
	m_sCommandLine = sCmdLine;
}
/////////////////////////////////////////////////////////////////////////////
void CApplication::SetWaitUntilProcessEnd(BOOL bWait)
{
	m_bWaitUntilProcessEnd = bWait;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CApplication::GetExitCode() const
{
	return m_dwExitCode;
}
