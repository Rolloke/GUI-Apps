#include "StdAfx.h"
#include ".\myservice.h"

CNTService* CNTService::m_pThis = NULL;

///////////////////////////////////////////////////////////////////////////
CMyService::CMyService(BOOL bAutoClose,
					   const CString& sCtrlApp,
					   const CString& sServiceName,
					   const CString& sServiceHelp)
					  : CNTService(sServiceName, sServiceName, sServiceHelp)
{
	m_pThis				= this;
	m_sCtrlApp			= sCtrlApp;
	m_bRunning			= FALSE;
	m_bPaused			= FALSE;
	m_bAutoClose		= bAutoClose;
}

///////////////////////////////////////////////////////////////////////////
CMyService::~CMyService(void)
{
}

///////////////////////////////////////////////////////////////////////////
BOOL CMyService::OnStop()
{
	m_bRunning=FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CMyService::OnPause()
{
	m_bPaused = TRUE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CMyService::OnContinue()
{
	m_bPaused = FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CMyService::OnRun()
{
	m_bRunning=TRUE;

	// Wenn der Prozess schon läuft brauch er nicht gestartet werden.
	if (!FindRunningProcess(m_sCtrlApp))
		StartProcess(m_sCtrlApp);

	while(m_bRunning)
	{
		if (!m_bPaused)
		{
			if (WaitForSingleObject(m_prozessInformation.hProcess, 1000) == WAIT_OBJECT_0)
				StartProcess(m_sCtrlApp);
		}
		else
			Sleep(250);
	}
	if (m_bAutoClose)
		CloseProcess();

	return TRUE;
}

#if (0)
////////////////////////////////////////////////////////////////////////////
// Startet einen Prozeß und kehr erst nach Beenden dieses zurück
BOOL CMyService::StartProcess(const CString& sExePathName,
								const CString& sWorkingDirectory /* _T("")*/,
								BOOL bWait	/* = FALSE */,
								DWORD dwStartFlags, /* = SW_SHOWNORMAL, */
								DWORD dwStartTime /* =30000 */)
{
	BOOL bProcess = FALSE;
	m_startUpInfo.cb				= sizeof(STARTUPINFO);
    m_startUpInfo.lpReserved		= NULL;
    m_startUpInfo.lpDesktop			= _T("");
    m_startUpInfo.lpTitle			= NULL;
    m_startUpInfo.dwX				= 0;
    m_startUpInfo.dwY				= 0;
    m_startUpInfo.dwXSize			= 0;
    m_startUpInfo.dwYSize			= 0;
    m_startUpInfo.dwXCountChars		= 0;
    m_startUpInfo.dwYCountChars		= 0;
    m_startUpInfo.dwFillAttribute	= 0;
	m_startUpInfo.dwFlags			= /*STARTF_FORCEONFEEDBACK |*/ STARTF_USESHOWWINDOW;
	m_startUpInfo.wShowWindow		= (WORD)dwStartFlags;  
    m_startUpInfo.cbReserved2		= 0;
    m_startUpInfo.lpReserved2		= NULL;


	CString sCommandLine = sExePathName;

	static HANDLE      hToken;

	if (LogonUser(_T("Martin.Lueck"), _T("VIDETE"), _T("passwort"), 
           LOGON32_LOGON_INTERACTIVE,
           LOGON32_PROVIDER_DEFAULT,
           &hToken) ) 
	{
		bProcess = CreateProcessAsUser(
							hToken,
							NULL,							// pointer to name of executable module 
							sCommandLine.GetBuffer(0),		// pointer to command line string
							(LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
							(LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
							FALSE, //TRUE,							// handle inheritance flag 
							NORMAL_PRIORITY_CLASS,			//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
							(LPVOID)NULL,					// pointer to new environment block 
							sWorkingDirectory,				// pointer to current directory name 
							(LPSTARTUPINFO)&m_startUpInfo,	// pointer to STARTUPINFO 
							(LPPROCESS_INFORMATION)&m_prozessInformation 	// pointer to PROCESS_INFORMATION  
							);
	}
	sCommandLine.ReleaseBuffer();

	if (bProcess && m_prozessInformation.hProcess)
	{
		DWORD dwIdleTime=0;
		DWORD rWFII;

		rWFII=WaitForInputIdle(m_prozessInformation.hProcess,dwStartTime);
		if (rWFII==0)
		{
			dwIdleTime = GetTickCount();	
			TRACE(_T("%s gestarted %d ms\n"), sCommandLine, dwIdleTime-dwStartTime);
		}
		else if(rWFII==WAIT_TIMEOUT)
			TRACE(_T("%s wait for input idle timeout %d\n"), sCommandLine, dwStartTime);
		
		if (bWait)
			DWORD dwTermination = WaitForSingleObject(m_prozessInformation.hProcess,INFINITE);
	}
	else
	{
		DWORD dwErr = GetLastError();
		TRACE("Could not start %s",sExePathName);
	}

	return bProcess;
}
#else
////////////////////////////////////////////////////////////////////////////
// Startet einen Prozeß und kehr erst nach Beenden dieses zurück
BOOL CMyService::StartProcess(const CString& sExePathName,
								const CString& sWorkingDirectory /* _T("")*/,
								BOOL bWait	/* = FALSE */,
								DWORD dwStartFlags, /* = SW_SHOWNORMAL, */
								DWORD dwStartTime /* =30000 */)
{
	BOOL bProcess = FALSE;

	m_startUpInfo.cb				= sizeof(STARTUPINFO);
    m_startUpInfo.lpReserved		= NULL;
    m_startUpInfo.lpDesktop			= NULL;
    m_startUpInfo.lpDesktop			= NULL;
    m_startUpInfo.lpTitle			= NULL;
    m_startUpInfo.dwX				= 0;
    m_startUpInfo.dwY				= 0;
    m_startUpInfo.dwXSize			= 0;
    m_startUpInfo.dwYSize			= 0;
    m_startUpInfo.dwXCountChars		= 0;
    m_startUpInfo.dwYCountChars		= 0;
    m_startUpInfo.dwFillAttribute	= 0;
	m_startUpInfo.dwFlags			= /*STARTF_FORCEONFEEDBACK |*/ STARTF_USESHOWWINDOW;
	m_startUpInfo.wShowWindow		= (WORD)dwStartFlags;  
    m_startUpInfo.cbReserved2		= 0;
    m_startUpInfo.lpReserved2		= NULL;

	CString sCommandLine = sExePathName;

	bProcess = CreateProcess(
						NULL,							// pointer to name of executable module 
						sCommandLine.GetBuffer(0),		// pointer to command line string
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
						FALSE,							// handle inheritance flag 
						NORMAL_PRIORITY_CLASS,			//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
						(LPVOID)NULL,					// pointer to new environment block 
						sWorkingDirectory,				// pointer to current directory name 
						(LPSTARTUPINFO)&m_startUpInfo,	// pointer to STARTUPINFO 
						(LPPROCESS_INFORMATION)&m_prozessInformation 	// pointer to PROCESS_INFORMATION  
						);
	sCommandLine.ReleaseBuffer();

	if (bProcess)
	{
		DWORD dwIdleTime=0;
		DWORD rWFII;

		rWFII=WaitForInputIdle(m_prozessInformation.hProcess,dwStartTime);
		if (rWFII==0)
		{
			dwIdleTime = GetTickCount();	
			TRACE(_T("%s gestarted %d ms\n"), sCommandLine, dwIdleTime-dwStartTime);
		}
		else if(rWFII==WAIT_TIMEOUT)
			TRACE(_T("%s wait for input idle timeout %d\n"), sCommandLine, dwStartTime);
		
		if (bWait)
			DWORD dwTermination = WaitForSingleObject(m_prozessInformation.hProcess,INFINITE);
	}
	else
	{
		DWORD dwErr = GetLastError();
		TRACE(_T("Could not start %s"),sExePathName);
	}

	return bProcess;
}
#endif

BOOL CMyService::CloseProcess()
{
	BOOL bResult = FALSE;
	HWND hWnd = FindOutWindowHandle();

	if (hWnd)
	{
		PostMessage(hWnd, WM_CLOSE,0,0L);
		if (WaitForSingleObject(m_prozessInformation.hProcess, 10000) != WAIT_OBJECT_0)
		{
			PostMessage(hWnd, WM_QUIT,0,0L);	
			if (WaitForSingleObject(m_prozessInformation.hProcess, 10000) != WAIT_OBJECT_0)
			{
				DWORD dwExitCode;
				GetExitCodeProcess(m_prozessInformation.hProcess, &dwExitCode);
				bResult  = TerminateProcess(m_prozessInformation.hProcess, dwExitCode);
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
HWND CMyService::FindOutWindowHandle()
{
	if (EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)this))
		return m_hWndProcess;
	else
        return NULL;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CMyService::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	CMyService* pThis = (CMyService*)lParam;
	DWORD dwPID = (DWORD)pThis->m_prozessInformation.dwProcessId;
	DWORD dwTempPID = 0;

	GetWindowThreadProcessId(hWnd, &dwTempPID);
//	if ((dwPID == dwTempPID) && IsWindowVisible(hWnd))
	if (dwPID == dwTempPID)
		pThis->m_hWndProcess = hWnd;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CMyService::FindRunningProcess(const CString& sModule)
{
	BOOL  bResult = FALSE;

	DWORD dwProcessIds[4096] = {0};
	DWORD dwNeeded = 0;

	if (EnumProcesses(dwProcessIds, sizeof(dwProcessIds), &dwNeeded))
	{
		for (int nI = 0; nI < (int)(dwNeeded/sizeof(DWORD)); nI++)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessIds[nI]);
			if (hProcess)
			{
				HMODULE hMod	= NULL;
				DWORD cbNeeded	= 0;

				TCHAR szProcessName[1024];
				if (EnumProcessModules( hProcess, &hMod, sizeof(hMod),	&cbNeeded))
				{
					GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName));
					TRACE(_T("Module:%s\n"), szProcessName);
					CString sProcessName(szProcessName);

					if (sProcessName.Find(sModule) != -1)
					{
						m_prozessInformation.hProcess	 = hProcess;
						m_prozessInformation.dwProcessId = dwProcessIds[nI];
						bResult = TRUE;
						break;
					}
				}	
				CloseHandle( hProcess );
			}
		}
	}

	return bResult;
}