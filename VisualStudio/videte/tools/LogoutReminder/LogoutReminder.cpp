// LogoutReminder.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LogoutReminder.h"

#include "MainFrm.h"
#include <process.h>
#include ".\logoutreminder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_SECTION			"Default"
#define DEFAULT_PATH			"Path"
#define DEFAULT_REMIND			"Remind"
#define DEFAULT_TIME			"Time"
#define DEFAULT_SAVDED			"Saved"
#define DEFAULT_RESTART			"Restart"
#define DEFAULT_WORKING_TIME	"WorkingTimespan"
#define DEFAULT_LOGOUT_TIME		"LogoutTime"

#define DEFAULT_INCREMENT "Increment"
#define INCREMENT_FMT	_T("%dd:%dh:%dm:%ds")
//#define  TIME_VALUE time_t
#define  TIME_VALUE __time64_t

/////////////////////////////////////////////////////////////////////////////
// CLogoutReminderApp

BEGIN_MESSAGE_MAP(CLogoutReminderApp, CWinApp)
	//{{AFX_MSG_MAP(CLogoutReminderApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogoutReminderApp construction

CLogoutReminderApp::CLogoutReminderApp()
{
	m_bShowDialog = DONT_SHOW;
	m_LogoutTime  = CTime::GetCurrentTime();
	TRACE("m_LogoutTime %i:%i:%i\n", m_LogoutTime.GetHour(), m_LogoutTime.GetMinute(), m_LogoutTime.GetSecond());
	m_bStopTimeoutDlgBox = false;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLogoutReminderApp object

CLogoutReminderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLogoutReminderApp initialization

BOOL CLogoutReminderApp::InitInstance()
{
	SetRegistryKey(_T("KESoft"));

	if (m_lpCmdLine != NULL)
	{
		for (int i=0; m_lpCmdLine[i] != 0; i++)
		{
			if (m_lpCmdLine[i] == '/')
			{
				switch (m_lpCmdLine[i+1])
				{
					case 's': m_bShowDialog        = SHOW_DLG; break;
					case 't': m_bStopTimeoutDlgBox = true;     break;
				}
			}
		}
	}


	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	CAboutDlg aboutDlg;
	aboutDlg.m_bLogOutTime = GetProfileInt(DEFAULT_SECTION, DEFAULT_REMIND, false);
	aboutDlg.m_bSavedTime  = GetProfileInt(DEFAULT_SECTION, DEFAULT_SAVDED, false);
	aboutDlg.OnCkLogoutOldTime();

	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);
	//long iResult, iDay=0, iHour=0, iMinute=0, iSecond=0;
	//CString str, strFormat;
 //   for (int i=0; ; i++)
 //   {
 //       strFormat.Format("ScheduledTime%d", i);
	//	str = GetProfileString(DEFAULT_SECTION, strFormat, str);
	//	if (!str.IsEmpty())
	//	{
	//		iResult = _stscanf(str, INCREMENT_FMT, &iDay, &iHour, &iMinute, &iSecond);
	//	}
 //       else
 //       {
 //           break;
 //       }

	//	if (   (iResult == 4)
 //           && (iHour   != -1)
	//		&& (iMinute != -1)
	//		&& (iSecond != -1)
	//		)
	//	{
 //           ScheduleTask task;
	//		CTime Today  = CTime::GetCurrentTime();
 //           strFormat.Format("ScheduledType%d", i);
 //           task.mSchedule = (ScheduleTask::eSchedule)GetProfileInt(DEFAULT_SECTION, strFormat, 0);
 //           if (task.mSchedule == ScheduleTask::atTime)
 //           {
	//		    CTime time(Today.GetYear(), Today.GetMonth(), Today.GetDay(), iHour, iMinute, iSecond, -1);
 //               task.mTime = time;
 //           }
 //           else if (task.mSchedule == ScheduleTask::afterStartup)
 //           {
 //               CTimeSpan timespan(iDay, iHour, iMinute, iSecond);
 //               task.mTime = Today + timespan;
 //           }
 //           strFormat.Format("ScheduledTask%d", i);
 //           task.mTask = GetProfileString(DEFAULT_SECTION, strFormat);
 //           mTasks.AddTail(task);
 //       }
 //   }

//	pFrame->ShowWindow(SW_SHOW);
//	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLogoutReminderApp message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strLogOutProgPath = _T("");
	m_bLogOutTime = FALSE;
	m_nRadio = -1;
	m_bCkDate = FALSE;
	//}}AFX_DATA_INIT
	m_LogoutTime = 0;
	m_nTimer     = 0;
	m_nSeconds   = 0;
	m_bSavedTime = 0;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_EDT_LOGOUT_PROGRAM_PATH, m_strLogOutProgPath);
	DDX_Check(pDX, IDC_CK_LOGOUT_TIME, m_bLogOutTime);
	DDX_DateTimeCtrl(pDX, IDC_LOGOUT_TIME, m_LogoutTime);
	DDX_Check(pDX, IDC_CK_LOGOUT_OLD_TIME, m_bSavedTime);
	DDX_Check(pDX, IDC_CK_DATE, m_bCkDate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH, OnBtnFindPath)
	ON_BN_CLICKED(IDC_BTN_LOGOUT, OnBtnLogout)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CK_STOP, OnCkStop)
	ON_BN_CLICKED(IDC_SAVE_LOGOUT_TIME, OnSaveLogoutTime)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_CK_LOGOUT_OLD_TIME, OnCkLogoutOldTime)
	ON_BN_CLICKED(IDC_CK_DATE, OnCkDate)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	//}}AFX_MSG_MAP
    ON_EN_KILLFOCUS(IDC_EDT_LOGOUT_PROGRAM_PATH, OnEnKillfocusEdtLogoutProgramPath)
END_MESSAGE_MAP()

// App command to run the dialog
void CLogoutReminderApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.m_bLogOutTime = GetProfileInt(DEFAULT_SECTION, DEFAULT_REMIND, false);
	aboutDlg.m_bSavedTime  = GetProfileInt(DEFAULT_SECTION, DEFAULT_SAVDED, false);
	if (m_bShowDialog == SHOW_DLG)
	{
   		aboutDlg.OnCkLogoutOldTime();
//			m_LogoutTime = CTime::GetCurrentTime() + CTimeSpan(0, 8, 30, 0);
	}
	else // if (m_bShowDialog==REMIND_DLG)
	{
		aboutDlg.m_LogoutTime = m_LogoutTime;
	}

	CMainFrame *pF = (CMainFrame*)AfxGetMainWnd();
	pF->StopTimer();
	if (aboutDlg.DoModal() == IDOK)
	{
		m_LogoutTime = aboutDlg.m_LogoutTime;
		if ((aboutDlg.m_bLogOutTime != 0) && (m_bShowDialog != SHUT_DOWN_EXIT))
		{
			pF->StartTimer();
			m_bShowDialog  = REMIND_DLG;
			CString str = m_LogoutTime.Format(IDS_TOOLTIP2);
			pF->SetShellIconTip(str);
		}
		WriteProfileInt(DEFAULT_SECTION, DEFAULT_REMIND, aboutDlg.m_bLogOutTime);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CLogoutReminderApp message handlers
void CAboutDlg::OnBtnFindPath() 
{
	StopTimer();
	UpdateData(true);
	CFileDialog dialog(true, "EXE", m_strLogOutProgPath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Ausführbare Datei (*.exe)|*.exe||", this);
	if (dialog.DoModal() ==IDOK)
	{
		m_strLogOutProgPath = dialog.GetPathName();
		theApp.WriteProfileString(DEFAULT_SECTION, DEFAULT_PATH, m_strLogOutProgPath);
		UpdateData(false);
	}
	StartTimer();
}

BOOL CAboutDlg::OnInitDialog() 
{
	m_strLogOutProgPath = theApp.GetProfileString(DEFAULT_SECTION, DEFAULT_PATH);

	CDialog::OnInitDialog();
	SetIcon(AfxGetMainWnd()->GetIcon(true), true);

	if (theApp.m_bShowDialog == DONT_SHOW)
	{
		OnCancel();
		return false;
	}

	if (theApp.m_bShowDialog == REMIND_DLG)
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	}

	if ((theApp.m_bShowDialog == LOG_OFF) || (theApp.m_bShowDialog == SHUT_DOWN))
	{
		m_nRadio = (theApp.m_bShowDialog == SHUT_DOWN) ? 0 : 2;
		GetDlgItem(IDC_RADIO0)->EnableWindow();
		GetDlgItem(IDC_RADIO1)->EnableWindow();
		GetDlgItem(IDC_RADIO2)->EnableWindow();
		CDataExchange dx(this, false);
		DDX_Radio(&dx, IDC_RADIO0, m_nRadio);
	}

	if (!theApp.m_bStopTimeoutDlgBox)
	{
		StartTimer();
	}
	EnableToolTips(true);
	return TRUE;
}

void CAboutDlg::OnBtnLogout() 
{
	if (m_strLogOutProgPath.IsEmpty())
	{
		OnBtnFindPath();
		if (m_strLogOutProgPath.IsEmpty()) return;
	}
	SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	StopTimer();
	STARTUPINFO startupinfo;
	ZeroMemory(&startupinfo, sizeof(STARTUPINFO));              // StartupInfo Struktur füllen
	startupinfo.cb          = sizeof(STARTUPINFO);
	startupinfo.wShowWindow = SW_SHOW;
	CreateProcess(NULL, (char*)LPCTSTR(m_strLogOutProgPath), NULL, NULL, false, 0, NULL, NULL, &startupinfo, &m_pi);
	unsigned int nThreadID;
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, WaitThread, (void*)this, 0, &nThreadID);
	EnableWindow(false);
}

unsigned int WINAPI CAboutDlg::WaitThread(void *pParam)
{
	CAboutDlg *pThis = (CAboutDlg*)pParam;
	WaitForSingleObject(pThis->m_pi.hProcess, INFINITE);
	CloseHandle(pThis->m_pi.hProcess);
	CloseHandle(pThis->m_pi.hThread);
	pThis->EnableWindow(true);
	pThis->SetFocus();
	pThis->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	pThis->StartTimer();
	CloseHandle(pThis->m_hThread);
	return 0;
}

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 100)
	{
		CDataExchange dx(this, false);
		DDX_Text(&dx, IDC_TXT_SEC, m_nSeconds);
		CDialog::OnTimer(nIDEvent);
		if (m_nSeconds-- == 0) OnOK();
	}
}

void CAboutDlg::StartTimer()
{
	m_nTimer   = SetTimer(100, 1000, NULL);
	m_nSeconds = 60;
	BOOL bStop = (m_nTimer == 100) ? 0 : 1;

	CDataExchange dx(this, false);
	DDX_Check(&dx, IDC_CK_STOP, bStop);
}

void CAboutDlg::StopTimer()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
		m_nTimer = 0;
	}
	BOOL bStop = 1;

	CDataExchange dx(this, false);
	DDX_Check(&dx, IDC_CK_STOP, bStop);
}

void CAboutDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CAboutDlg::OnOK() 
{
	if ((theApp.m_bShowDialog == LOG_OFF) || (theApp.m_bShowDialog == SHUT_DOWN))
	{
		HANDLE hToken;              // handle to process token 
		TOKEN_PRIVILEGES tkp;        // ptr. to token structure 

		BOOL fResult;                  // system shutdown flag 

		// Get the current process token handle 
		// so we can get debug privilege. 
		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;

		// Get the LUID for debug privilege. 
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

		// Get shutdown privilege for this process. 
		fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 

		CDataExchange dx(this, true);
		DDX_Radio(&dx, IDC_RADIO0, m_nRadio);
		if (m_nRadio == 0)
		{
			HKEY  hSecKey = NULL;
			LONG  lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\", 0, KEY_READ, &hSecKey);
			DWORD dwType;
			DWORD dwCount;
			theApp.m_bShowDialog = EWX_SHUTDOWN;
			if (lResult == ERROR_SUCCESS)
			{
				char szText[MAX_PATH];
				dwCount = MAX_PATH-1;
				lResult = RegQueryValueEx(hSecKey, "PowerDownAfterShutdown", NULL, &dwType, (LPBYTE)szText, &dwCount);
				if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ) && (szText[0] == '1'))
				{
					theApp.m_bShowDialog = EWX_POWEROFF;
				}
			}			
			theApp.WriteProfileBinary(DEFAULT_SECTION, DEFAULT_RESTART, NULL, 0);
		}
		else if (m_nRadio == 1)
		{
			theApp.m_bShowDialog = EWX_REBOOT;
			TIME_VALUE t =  m_LogoutTime.GetTime();
			theApp.WriteProfileBinary(DEFAULT_SECTION, DEFAULT_RESTART, (LPBYTE)&t, sizeof(TIME_VALUE));
		}
		else
		{
			theApp.m_bShowDialog = EWX_LOGOFF;
			theApp.WriteProfileBinary(DEFAULT_SECTION, DEFAULT_RESTART, NULL, 0);
		}

		ExitWindowsEx(theApp.m_bShowDialog, 0);
		theApp.m_bShowDialog = SHUT_DOWN_EXIT;
	}

	CDialog::OnOK();
}

void CAboutDlg::OnCkStop() 
{
	if (m_nTimer) StopTimer();
	else          StartTimer();
}


void CAboutDlg::OnCkLogoutOldTime() 
{
	if (m_hWnd)
	{
		CDataExchange dx(this, true);
		DDX_Check(&dx, IDC_CK_LOGOUT_OLD_TIME, m_bSavedTime);
	}

	LPBYTE pData = NULL;
	UINT nBytes  = sizeof(TIME_VALUE);
	TIME_VALUE nTime;
	if (theApp.GetProfileBinary(DEFAULT_SECTION, DEFAULT_RESTART, &pData, &nBytes) && nBytes == sizeof(TIME_VALUE))
	{
		m_LogoutTime = nTime = *((TIME_VALUE*)pData);
		delete pData;
		theApp.WriteProfileBinary(DEFAULT_SECTION, DEFAULT_RESTART, NULL, 0);
	}
	else if (m_bSavedTime)
	{
		long iDay=0, iHour=16, iMinute=30, iSecond=0;
		CString str;
		str = theApp.GetProfileString(DEFAULT_SECTION, DEFAULT_LOGOUT_TIME, str);
		if (str.IsEmpty())
		{
			str.Format(INCREMENT_FMT, iDay, iHour, iMinute, iSecond);
			theApp.WriteProfileString(DEFAULT_SECTION, DEFAULT_LOGOUT_TIME, str);
		}
		else
		{
			_stscanf(str, INCREMENT_FMT, &iDay, &iHour, &iMinute, &iSecond);
		}

		if (   (iHour   != -1)
			&& (iMinute != -1)
			&& (iSecond != -1)
			)
		{
			CTime Today  = CTime::GetCurrentTime();
			CTime time(Today.GetYear(), Today.GetMonth(), Today.GetDay(), iHour, iMinute, iSecond, -1);
			m_LogoutTime = time;
			TRACE("CTime restored %i:%i:%i\n", m_LogoutTime.GetHour(), m_LogoutTime.GetMinute(), m_LogoutTime.GetSecond());

		}
		else
		{
			nTime = theApp.GetProfileInt(DEFAULT_SECTION, DEFAULT_TIME, 0);
			if (nTime)
			{
				CTime TSaved = (time_t)nTime;
				CTime Today  = CTime::GetCurrentTime();
				CTime time(Today.GetYear(), Today.GetMonth(), Today.GetDay(), TSaved.GetHour(), TSaved.GetMinute(), TSaved.GetSecond(), -1);
				m_LogoutTime = time;
				TRACE("time_t     restored %i\n", (time_t)nTime);
				TRACE("CTimeSaved restored %i:%i:%i\n", TSaved.GetHour(), TSaved.GetMinute(), TSaved.GetSecond());
				TRACE("CTimetime  restored %i:%i:%i\n", time.GetHour(), time.GetMinute(), time.GetSecond());
				TRACE("time_t   calculated %i\n", m_LogoutTime.GetTime());
			}
		}
	}
	else
	{
		m_LogoutTime = CTime::GetCurrentTime() + theApp.GetIncrementTimeSpan(DEFAULT_WORKING_TIME);
	}

	if (m_hWnd)
	{
		CDataExchange dx(this, false);
		DDX_DateTimeCtrl(&dx, IDC_LOGOUT_TIME, m_LogoutTime);
		theApp.WriteProfileInt(DEFAULT_SECTION, DEFAULT_SAVDED, m_bSavedTime);
	}
}

void CAboutDlg::OnSaveLogoutTime() 
{
	UpdateData();
	CString str;
	str.Format(INCREMENT_FMT, 0, m_LogoutTime.GetHour(), m_LogoutTime.GetMinute(), m_LogoutTime.GetSecond());
	theApp.WriteProfileString(DEFAULT_SECTION, DEFAULT_LOGOUT_TIME, str);
}

BOOL CAboutDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	if (   pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND)
		|| pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID != 0)
	{// will be zero on a separator
		pTTTA->lpszText = MAKEINTRESOURCE(nID);
		pTTTA->hinst = AfxGetResourceHandle();
		return(TRUE);
	}

	if (pNMHDR->code == TTN_NEEDTEXTA)
	{
		lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
	}
	else
	{
		//_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
	}
	*pResult = 0;

	return TRUE;    // message was handled
}

void CAboutDlg::OnClose() 
{
	CDialog::OnCancel();
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
}

void CAboutDlg::OnCkDate() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_DATE, m_bCkDate);
	if (m_bCkDate)
	{
		GetDlgItem(IDC_LOGOUT_TIME)->ModifyStyle(0x00000008, 0);
	}
	else
	{
		GetDlgItem(IDC_LOGOUT_TIME)->ModifyStyle(0, 0x00000008);
	}
}

CTimeSpan CLogoutReminderApp::GetIncrementTimeSpan(LPCTSTR sEntry)
{
	CString str;
	long nDays=0, nHours=0, nMinutes=0, nSeconds=0;
	if (sEntry == NULL)
	{
		sEntry = DEFAULT_INCREMENT;
		nMinutes = 15;
	}
	else if (_tcscmp(sEntry, DEFAULT_WORKING_TIME) == 0)
	{
		nHours = 8;
		nMinutes = 30;
	}

	str = GetProfileString(DEFAULT_SECTION, sEntry, str);
	if (str.IsEmpty())
	{
		str.Format(INCREMENT_FMT, nDays, nHours, nMinutes, nSeconds);
		WriteProfileString(DEFAULT_SECTION, sEntry, str);
	}
	else
	{
		_stscanf(str, INCREMENT_FMT, &nDays, &nHours, &nMinutes, &nSeconds);
	}
	return CTimeSpan(nDays, nHours, nMinutes, nSeconds);
}

void CAboutDlg::OnEnKillfocusEdtLogoutProgramPath()
{
    GetDlgItemText(IDC_EDT_LOGOUT_PROGRAM_PATH, m_strLogOutProgPath);
	theApp.WriteProfileString(DEFAULT_SECTION, DEFAULT_PATH, m_strLogOutProgPath);
}
