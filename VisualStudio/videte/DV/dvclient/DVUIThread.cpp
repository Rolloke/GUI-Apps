/* GlobalReplace: CSkinsDlg --> CPanel */
// DVUIThread.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "DVUIThread.h"
#include "MainFrame.h"
#include "CPanel.h"
#include <malloc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CDVClientApp theApp;


IMPLEMENT_DYNAMIC(CDVUIThread, CWinThread)
#ifdef _UNICODE
 #define WinExec CDVUIThread::WinExecW
#endif
/////////////////////////////////////////////////////////////////////////////
// CDVUIThread
CDVUIThread::CDVUIThread()
{
	m_bAutoDelete = FALSE;
	m_pMainWnd = NULL;
}
/////////////////////////////////////////////////////////////////////////////
CDVUIThread::~CDVUIThread()
{
//	TRACE(_T("CDVUIThread delete\n"));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVUIThread::InitInstance()
{
	WK_TRACE(_T("CDVUIThread::InitInstance()\n"));
	XTIB::SetThreadName(_T("CDVUIThread"));

	CPanel* pPanel = new CPanel();
	m_pMainWnd = pPanel;
	if (   pPanel 
		&& pPanel->Create())
	{
		WK_TRACE(_T("CDVUIThread Thread ID is %08lx\n"),GetCurrentThreadId());
	}
	else
	{
		m_pMainWnd = NULL;
		WK_DELETE(pPanel);
		WK_TRACE(_T("Could not create Panel\n"));
		return FALSE;
	}


	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CDVUIThread::ExitInstance()
{
	WK_TRACE(_T("CDVUIThread::ExitInstance()\n"));
	m_pMainWnd = NULL;
	CWnd* pWnd = theApp.GetMainFrame();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_CLOSE, 0, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVUIThread::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	CWnd *pWnd = CWnd::FromHandle(lpMsg->hwnd);
	CPanel* pPanel = NULL;
	if (pWnd == NULL)
	{
		return FALSE;
	}

	if (pWnd->IsKindOf(RUNTIME_CLASS(CPanel)))
	{
		pPanel = (CPanel*)pWnd;
	}
	else
	{
		// Use ::IsChild to get messages that may be going
		// to the dialog's controls.  In the case of
		// WM_KEYDOWN this is required.
		CWnd* pParent = pWnd->GetParent();
		if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CPanel)))
		{
			pPanel = (CPanel*)pParent;
		}
	}

//	TRACE(_T("CDVUIThread::ProcessMessageFilter code=%i, message=%i, key=%i\n"),
//										code, lpMsg->message, lpMsg->wParam);
	if (pPanel)
	{
		// special handling for F10 (RTFM)
		if (lpMsg->message == WM_SYSKEYDOWN
			&& (char)lpMsg->wParam == VK_F10)
		{
			pPanel->OnKeyDown(lpMsg->wParam,
					   		  LOWORD(lpMsg->lParam),
							  HIWORD(lpMsg->lParam));
			return TRUE;
		}
		if (lpMsg->message == WM_KEYDOWN)
		{
			HandleKeyStroke((char)lpMsg->wParam);
	
	//		TRACE(_T("ScanCode=0x%x\n"), (lpMsg->lParam >> 16) & 0xff);

			if (((VK_SPACE<=lpMsg->wParam) && (lpMsg->wParam<=VK_DOWN)) ||
						((VK_F1<=lpMsg->wParam) && (lpMsg->wParam<=VK_F12)) ||
						(VK_RETURN==lpMsg->wParam) ||
						(VK_DECIMAL==lpMsg->wParam) ||
						(VK_ADD==lpMsg->wParam) ||
						(VK_DELETE==lpMsg->wParam)
						)
			{
					pPanel->OnKeyDown(lpMsg->wParam,
					   				  LOWORD(lpMsg->lParam),
									  HIWORD(lpMsg->lParam));
					return TRUE;
			}
		}
		if (lpMsg->message == WM_KEYUP)
		{
				if (((VK_SPACE<=lpMsg->wParam) && (lpMsg->wParam<=VK_DOWN)) ||
							((VK_F1<=lpMsg->wParam) && (lpMsg->wParam<=VK_F12)) ||
							(VK_RETURN==lpMsg->wParam) ||
							(VK_DECIMAL==lpMsg->wParam) ||
							(VK_ADD==lpMsg->wParam) ||
							(VK_DELETE==lpMsg->wParam)
							)
				{
						pPanel->OnKeyUp(lpMsg->wParam,
										LOWORD(lpMsg->lParam),
										HIWORD(lpMsg->lParam));
						return TRUE;
				}
		}
	}
	return CWinThread::ProcessMessageFilter(code, lpMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CDVUIThread::HandleKeyStroke(char vKey)
{
	// Cheatcodes werden nur beim Transmitter ausgewertet.
	if (theApp.IsReceiver())
		return;

	// NumPad0...NumPad9 in Ascii0...Ascii9 wandeln
	if (vKey >= VK_NUMPAD0 && vKey <= VK_NUMPAD9)
		vKey -= 0x30;

	if ((vKey >= _T('0') && vKey <= _T('9')) || (vKey >= _T('A') && vKey <=_T('Z')) || (vKey >= _T('a') && vKey <=_T('z')))
	{
		m_sInput += (_TCHAR) tolower(vKey);

		if (m_sInput.GetLength() > 32)
		{
			m_sInput = _T("");
		}

		CWK_Profile wkp;

		BOOL bMasterPwd = FALSE;

		// OEM Spezifische Einstellungen
		// DResearch bekommt ein eigenes Masterpaﬂwort.
		if ((OEM)wkp.GetInt(REGKEY_DVCLIENT, REGKEY_OEM,  (int)OemDTS) == OemDResearch)
			bMasterPwd = (m_sInput.Right(10).Find(_T("systemmode")) != -1);
		else
			bMasterPwd = (m_sInput.Right(8).Find(_T("cdefgahc")) != -1);
			
		BOOL bTmpPwd	= IsTempPassword(m_sInput.Right(8));
		BOOL bShellPwd	= (m_sInput.Right(10).Find(_T("shellmode")) != -1);
		BOOL bLogView	= (m_sInput.Right(12).Find(_T("startlogview")) != -1);
		BOOL bExplorer	= (m_sInput.Right(13).Find(_T("startexplorer")) != -1);
		BOOL bMDConfig1	= (m_sInput.Right(9).Find(_T("mdconfig1")) != -1);
		BOOL bMDConfig2	= (m_sInput.Right(9).Find(_T("mdconfig2")) != -1);
		BOOL bRegedit	= (m_sInput.Right(12).Find(_T("startregedit")) != -1);
		BOOL bWatchDog	= (m_sInput.Right(8).Find(_T("watchdog")) != -1);

		BOOL bShell = wkp.GetInt(REGKEY_DVSTARTER, _T("AsShell"), 0);
#ifdef _DEBUG
		bShell=TRUE;
#endif
		if (!bShell && bShellPwd)
		{
			// Zur¸ck in den Shellmodus
			wkp.WriteInt(REGKEY_DVSTARTER, _T("AsShell"), 1);

			WK_TRACE(_T("Enter 'shell' password... Shutting down system and reboot in shellmode\n"));
			for (int nI = 9; nI > 0; nI--)
			{
				theApp.MessageBeep(DEFAULT_BEEP);
				Sleep(10*nI);
			}

			((CPanel*)m_pMainWnd)->DoShutdown();
		}
		else if (bShell && bLogView)
		{
			WK_TRACE(_T("Enter logview....Starting logview.exe\n"));
			if (!ExecuteProgram(theApp.GetHomeDir()+_T("\\LogView.exe"), theApp.GetHomeDir()))
			{
				WK_TRACE(_T("failed to execute (LogView)\n"));
			}
			m_sInput = _T("");
		}
		else if (bShell && bExplorer)
		{
			if (theApp.IsNT40())
			{
				WK_TRACE(_T("Enter explorer....Starting Explorer.exe\n"));
				if (WinExec(_T("Explorer.exe"), SW_SHOW) > 31)
					WK_TRACE(_T("WinExec failed (Explorer)\n"));
			}
			else if (theApp.IsWinXP())
			{
				CString sWinFile, sWinDir;
				GetWindowsDirectory(sWinDir.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
				sWinDir.ReleaseBuffer(-1);
				if (sWinDir.GetAt(sWinDir.GetLength()-1) == _T('\\'))
				{
					sWinDir = sWinDir.Left(sWinDir.GetLength()-1);
				}
				sWinFile = sWinDir + _T("\\system32\\winfile.exe ");

				WK_TRACE(_T("Enter explorer....Starting %s\n"), sWinFile);
				if (GetFileAttributes(sWinFile) == (DWORD)-1)
				{
					WK_TRACE(_T("Did not find: %s\n"), sWinFile);
					sWinFile = sWinDir + _T("\\explorer.exe ");
				}
				if (!ExecuteProgram(sWinFile, theApp.GetHomeDir()))
				{
					WK_TRACE(_T("Failed to start: %s\n"), sWinFile);
				}
			}
			m_sInput = _T("");
		}
		else if (bShell && bRegedit)
		{
			WK_TRACE(_T("Enter regedit....Starting Regedit.exe\n"));
			if (!ExecuteProgram(_T("Regedit.exe"), NULL))
			{
				WK_TRACE(_T("WinExec failed (Regedit)\n"));
			}
			m_sInput = _T("");
	
		}
		else if (bShell && (bTmpPwd || bMasterPwd))
		{
			// Zur¸ck in den Systemmodus.
			wkp.WriteInt(REGKEY_DVSTARTER, _T("AsShell"), 0);
			WK_TRACE(_T("Enter masterpassword or temporary password... Shutting down system and reboot in systemmode\n"));
			for (int nI = 9; nI > 0; nI--)
			{
				theApp.MessageBeep(DEFAULT_BEEP);
				Sleep(10*nI);
			}

			((CPanel*)m_pMainWnd)->DoShutdown();
		}		
		else if (bShell && bMDConfig1)
		{
			if (WK_IS_RUNNING(WK_APP_NAME_JACOBUNIT1))
			{
				HWND hWnd = FindWindow(NULL, WK_APP_NAME_JACOBUNIT1);
				if (hWnd != NULL && IsWindow(hWnd))
				{
					CPanel* pPanel = (CPanel*)m_pMainWnd;
					PostMessage(hWnd, JACOB_OPEN_MD_DLG, pPanel->GetActiveCamera(), 0);
				}
			}
			m_sInput = _T("");
		}
		else if (bShell && bMDConfig2)
		{
			if (WK_IS_RUNNING(WK_APP_NAME_JACOBUNIT2))
			{
				HWND hWnd = FindWindow(NULL, WK_APP_NAME_JACOBUNIT2);
				if (hWnd != NULL && IsWindow(hWnd))
				{
					CPanel* pPanel = (CPanel*)m_pMainWnd;
					PostMessage(hWnd, JACOB_OPEN_MD_DLG, pPanel->GetActiveCamera(), 0);				
				}
			}
			m_sInput = _T("");
		}
		else if (bWatchDog)
		{
			if (WK_IS_RUNNING(WK_APP_NAME_JACOBUNIT1))
			{
				WK_TRACE(_T("testing WATCHDOG\n"));
				Sleep(100);
				HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE,FALSE,EV_JACOB_TRIGGER_WATCHDOG);
				if (hEvent)
				{
					CWK_Profile prof;
					prof.WriteInt(_T("JaCobUnit\\Device1\\General"), _T("WatchDogTimeout"), 1);
					if (!SetEvent(hEvent))
					{
						WK_TRACE(_T("cannot set %s ,%s\n"),EV_JACOB_TRIGGER_WATCHDOG,
							GetLastErrorString());
					}
					CloseHandle(hEvent);
				}
				else
				{
					WK_TRACE(_T("cannot open %s ,%s\n"),EV_JACOB_TRIGGER_WATCHDOG,
						GetLastErrorString());
				}
			}
			m_sInput = _T("");
		}
	}
}

////////////////////////////////////////////////////////////////////////////
BOOL CDVUIThread::IsTempPassword(const CString &sPassword)
{
	if (sPassword.GetLength() != 8)
		return FALSE;

	// Mappingtabelle
	CString sMap		= _T("qwertzuiopasdfgh");
	DWORD dwTime		= 0;
	CTime CurrTime		= CTime::GetCurrentTime();
	CTimeSpan Span;

	// Aus dem Paﬂwort ein DWORD bauen.
	for (int nI = 0; nI < sPassword.GetLength(); nI++)
	{
		int Val = sMap.Find(sPassword[nI]);
		if (Val == -1)
			return FALSE;
		dwTime |= Val<<(4*(7-nI));
	}

	CTime PassTime( (time_t)dwTime);

	if (CurrTime >= PassTime)
		Span = CurrTime - PassTime;
	else
		Span = PassTime - CurrTime;

	// Paﬂwort ist maximal 1/2 Stunde g¸ltig.
	LONG lSec = (LONG)Span.GetTotalSeconds();
	return ((lSec >= 0) && (lSec < 1800));
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDVUIThread, CWinThread)
	//{{AFX_MSG_MAP(CDVUIThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDVUIThread message handlers
#ifdef _UNICODE
 #undef WinExec
#endif
UINT CDVUIThread::WinExecW(LPCWSTR lpCmdLine, UINT uCmdShow)
{
	int i, nLen = wcslen(lpCmdLine);
	
//	char *pszCmdLine = new char[nLen+1];
	char *pszCmdLine = (char*)_alloca(nLen+1);
	for (i=0; i<nLen; i++)
		pszCmdLine[i] = (char)lpCmdLine[i];
	pszCmdLine[i] = 0;
	UINT nRet = WinExec(pszCmdLine, uCmdShow);
//	delete pszCmdLine;
	return nRet;
}
