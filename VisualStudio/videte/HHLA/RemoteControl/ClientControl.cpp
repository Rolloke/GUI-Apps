// ClientControl.cpp: implementation of the CClientControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RemoteControl.h"
#include "ClientControl.h"
#include "Ini2Reg.h"
#include "..\\MiniClient\\MiniClientDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CClientControl::CClientControl(const CString &sExePath, int nClientNr, HWND hWndRemoteControl)
{
	m_bOk				= FALSE;
	m_sCmdLine			= _T("XXX");
	m_sWorkingDirectory	= _T(".");
	m_sExePathName		= sExePath;
	m_bWait				= FALSE;
	m_dwStartFlags		= SW_SHOW;
	m_dwStartTime		= 5000;
	m_nClientNr			= nClientNr;
	m_hWndClient		= NULL;
	m_hWndRemoteControl	= hWndRemoteControl;
	m_dwPID				= 0;

	// Client laden.
	if (LoadClient())
		m_bOk = TRUE;
}

//////////////////////////////////////////////////////////////////////
CClientControl::~CClientControl()
{
	// Client beenden.
	if (m_hWndClient && IsWindow(m_hWndClient))
		PostMessage(m_hWndClient, WM_COMMAND, ID_APP_EXIT, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CClientControl::LoadClient()
{
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	DWORD				dwStartTime = GetTickCount();

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
	startUpInfo.dwFlags			= /*STARTF_FORCEONFEEDBACK |*/ STARTF_USESHOWWINDOW;
	startUpInfo.wShowWindow		= (WORD)m_dwStartFlags;  
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sCommandLine;
	sCommandLine.Format(_T("%s %s"), (const TCHAR *)m_sExePathName, (const TCHAR *)m_sCmdLine);
	
	BOOL bProcess = CreateProcess(
						NULL, //(LPCSTR)m_sExePathName,	// pointer to name of executable module 
						(LPTSTR)sCommandLine.GetBuffer(0),	// pointer to command line string
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
						FALSE,	// handle inheritance flag 
						NORMAL_PRIORITY_CLASS,//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
						(LPVOID)NULL,	// pointer to new environment block 
						(LPCTSTR)m_sWorkingDirectory,	// pointer to current directory name 
						(LPSTARTUPINFO)&startUpInfo,	// pointer to STARTUPINFO 
						(LPPROCESS_INFORMATION)&prozessInformation 	// pointer to PROCESS_INFORMATION  
						);

	sCommandLine.ReleaseBuffer();
	
	if (bProcess)
	{
		DWORD dwIdleTime=0;
		DWORD rWFII;
		rWFII = WaitForInputIdle(prozessInformation.hProcess,m_dwStartTime);
		if (rWFII == 0)
		{
			dwIdleTime = GetTickCount();	
			WK_TRACE(_T("%s gestartet %d ms\n"),(const TCHAR*)sCommandLine, dwIdleTime-dwStartTime);

			// Windowhandle des Client holen.
			m_hWndClient = FindWindowHandle(prozessInformation.dwProcessId);
			if (m_hWndClient && IsWindow(m_hWndClient))
			{
				m_dwPID = prozessInformation.dwProcessId;
				PostMessage(m_hWndClient, WM_SET_CLIENT_NUMBER, m_nClientNr, (LPARAM)m_hWndRemoteControl);
			}
			else
			{
//				CString sTemp;
//				sTemp.Format(_T("Verbindung zu '%s' gescheitert\n"), (LPCSTR)m_sExePathName); 
//				AfxMessageBox(sTemp, MB_OK);
			}
			
		}
		else if (rWFII == WAIT_TIMEOUT)
			WK_TRACE_ERROR(_T("%s wait for input idle timeout %d\n"),(const TCHAR*)sCommandLine, m_dwStartTime);
		else
			WK_TRACE_ERROR(_T("%s wait for input idle error %d\n"),(const TCHAR*)sCommandLine, GetLastError());

		if (m_bWait)
			WaitForSingleObject(prozessInformation.hProcess,INFINITE);
	}
	else
	{
		WK_TRACE_ERROR(_T("Could not start %s\n"),m_sExePathName);
		CString sTemp;
		sTemp.Format(_T("'%s' konnte nicht gestartet werden\n"), (LPCTSTR)m_sExePathName); 
		AfxMessageBox(sTemp, MB_OK);
	}

	return (bProcess && (m_hWndClient != NULL));
}

//////////////////////////////////////////////////////////////////////
HWND CClientControl::FindWindowHandle(DWORD dwProzessID)
{
	// Suche den Client mit der gewünschten ProzessID
	CWnd *pWnd = ClimbWindow((AfxGetMainWnd()->GetWindow(GW_HWNDFIRST)), dwProzessID);

	if (!pWnd)
	{
		WK_TRACE_ERROR(_T("Can't find Windowhandle of a client\n"));
		return NULL;
	}

	return pWnd->m_hWnd;
}

/////////////////////////////////////////////////////////////////////////////
CWnd* CClientControl::ClimbWindow(CWnd* pWnd, DWORD dwProzessID)
{
	DWORD	dwPID		= 0;
	CWnd	*pWndClient = NULL;

	// Rekursiv die ProzessID aller Fenster prüfen.
	while (pWnd)
	{ 
		::GetWindowThreadProcessId(pWnd->m_hWnd, &dwPID);
		if (dwPID == dwProzessID)
			return pWnd;

		pWndClient = ClimbWindow(pWnd->GetWindow(GW_CHILD), dwProzessID);
		if (pWndClient)
			break;

		pWnd = pWnd->GetNextWindow();
	}

	return pWndClient;
}	

/////////////////////////////////////////////////////////////////////////////
void CClientControl::DoRequestSetInputSource(int nViewType, BOOL bRefresh)
{
	if (m_hWndClient && IsWindow(m_hWndClient))
		PostMessage(m_hWndClient, WM_SET_INPUT_SOURCE, nViewType, bRefresh);
}

/////////////////////////////////////////////////////////////////////////////
void CClientControl::DoRequestSetWindowPosition(int nX, int nY, int nW, int nH)
{
	if (m_hWndClient && IsWindow(m_hWndClient))
		PostMessage(m_hWndClient, WM_SET_WINDOW_POSITION, MAKELONG((short int)nX, (short int)nY),
														  MAKELONG((short int)nW, (short int)nH));

	Sleep(100);
}

/////////////////////////////////////////////////////////////////////////////
void CClientControl::DoRequestSavePicture(const CString &sFileName)
{
	CString sSection;
	sSection.Format(_T("MiniClient\\Client%d"), m_nClientNr);

	MyWritePrivateProfileString(_T("JpegFile"), _T("Path"), sFileName, sSection);
	
	if (m_hWndClient && IsWindow(m_hWndClient))
		PostMessage(m_hWndClient, WM_SAVE_PICTURE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CClientControl::DoRequestSetCameraParameter(WORD wBrightness, WORD wContrast, WORD wSaturation)
{
	if (m_hWndClient && IsWindow(m_hWndClient))
		PostMessage(m_hWndClient, WM_SET_CAMERA_PARAMETER, wBrightness, MAKELONG(wContrast, wSaturation));
}

/////////////////////////////////////////////////////////////////////////////
void CClientControl::DoRequestCameraParameter()
{
	if (m_hWndClient && IsWindow(m_hWndClient))
		PostMessage(m_hWndClient, WM_REQ_CAMERA_PARAMETER, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CClientControl::DoRequestCheckCamera()
{
	if (m_hWndClient && IsWindow(m_hWndClient))
		PostMessage(m_hWndClient, WM_CHECK_CAMERA, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CClientControl::IsRunning()
{
	DWORD dwPID = 0;

	if (!m_hWndClient)
		return FALSE;

	if (!IsWindow(m_hWndClient))
		return FALSE;

	::GetWindowThreadProcessId(m_hWndClient, &dwPID);
	if (dwPID != m_dwPID)
		return FALSE;

	return TRUE;
}