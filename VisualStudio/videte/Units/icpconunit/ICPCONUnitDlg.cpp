/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: ICPCONUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/ICPCONUnit/ICPCONUnitDlg.cpp $
// CHECKIN:		$Date: 9.02.06 8:56 $
// VERSION:		$Revision: 40 $
// LAST CHANGE:	$Modtime: 9.02.06 8:56 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ICPCONUnit.h"
#include "ICPCONUnitDlg.h"
#include "ICP7000Thread.h"
#include "ICPCONDll\ICP7000Module.h"
#include "i7000.h"
#include "SwitchPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CICPCONUnitApp theApp;

#if (_TEST_TRACES == 1)
   #define TEST_TRACE WK_TRACE
#endif

#if (_TEST_TRACES == 2)
   #define TEST_TRACE theApp.OutputDebug
#endif

#ifndef _TEST_TRACES
	#define TEST_TRACE //
#endif

/////////////////////////////////////////////////////////////////////////////
// CICPCONUnitDlg dialog

CICPCONUnitDlg::CICPCONUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CICPCONUnitDlg::IDD, pParent)
{
	CString strKey;
	//{{AFX_DATA_INIT(CICPCONUnitDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nPollTimer         = 0;
	m_nPollTime          = 1000;
	m_nTimeout           = 400;
	m_bThreadsArePolling = FALSE;
	m_pSwitchPanel       = NULL;
	m_bPollState         = FALSE;
}

void CICPCONUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CICPCONUnitDlg)
	DDX_Control(pDX, IDC_EDT_LOG, m_cLogWindow);
	DDX_Control(pDX, IDC_LIST_MODULES, m_ModuleList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CICPCONUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CICPCONUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MODULES, OnClickListModules)
	ON_MESSAGE(WM_USER, OnUserMessage)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WK_WINTHREAD_STOP_THREAD, OnWinThreadStopThread)	
	ON_MESSAGE(WM_ENTERMENULOOP, OnEnterMenuLoop)	
	ON_MESSAGE(WM_EXITMENULOOP, OnExitMenuLoop)	
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CICPCONUnitDlg message handlers

BOOL CICPCONUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	int nCheckPerformance = wkp.GetInt(SETTINGS_SECTION, _T("CheckPerformance"), -1);
	if (nCheckPerformance == 1)
	{
		CICPI7000Thread::CreatePerformanceMonitor();
	}
	else if (nCheckPerformance == -1)
	{
		wkp.WriteInt(SETTINGS_SECTION, _T("CheckPerformance"), 0);
	}

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	m_ModuleList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
		LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
		LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_ModuleList.InsertColumn(0, _T(""), LVCFMT_LEFT, 65);

	int i;
	for (i=1; i<=16; i++)
	{
		m_ModuleList.InsertColumn(i, _T(""), LVCFMT_LEFT, 20);
	}

	if (theApp.m_bStandalone)
	{
		SetTimer(100, 2000, NULL);
	}
	else
	{
		CString str;
		str.LoadString(IDS_CLOSE);
		GetDlgItem(IDCANCEL)->SetWindowText(str);
	}

	SetWindowText(WK_APP_NAME_ICPCON_UNIT);

	CreateControlThreads();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CICPCONUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		OnAbout();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
void CICPCONUnitDlg::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CICPCONUnitDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CICPCONUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
long CICPCONUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_ICPCON_UNIT, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPCONUnitDlg::OnWinThreadStopThread(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1)
	{
		SAFE_KILL_TIMER(m_nPollTimer);
		DestroyControlThreads(true);
		return 1;
	}
	else
	{
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
		CreateControlThreads();
		SetTimer(100, 2000, NULL);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPCONUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

int CICPCONUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (theApp.m_bStandalone)
	{	
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	else
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		tnd.cbSize = sizeof(NOTIFYICONDATA);
		tnd.hWnd	  = m_hWnd;
		tnd.uID    = 1;
		tnd.hIcon  = m_hIcon;
		tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		lstrcpyn(tnd.szTip, theApp.m_pszAppName, _tcslen(theApp.m_pszAppName)+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
	}
	return 0;
}

void CICPCONUnitDlg::OnDestroy() 
{
	SAFE_KILL_TIMER(m_nPollTimer);
	DestroyControlThreads(false);
	CICPI7000Thread::DeletePerformanceMonitor();
	
	if (!theApp.m_bStandalone)
	{
		NOTIFYICONDATA tnd;

		tnd.cbSize	= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID		= 1;
		tnd.uFlags	= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}

	WK_DELETE(m_pSwitchPanel);

	CDialog::OnDestroy();
	PostQuitMessage(0);
}

void CICPCONUnitDlg::OnCancel() 
{
	if	(theApp.m_bStandalone)
	{
		PostMessage(WM_DESTROY);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}
void CICPCONUnitDlg::OnOK() 
{
	DestroyWindow();
}

void CICPCONUnitDlg::OnSettings() 
{
   if (!theApp.m_bStandalone)
	{
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		SetForegroundWindow(); 
	}
}

void CICPCONUnitDlg::Lock()
{
	m_cs.Lock();
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::Unlock()
{
	m_cs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////

/*********************************************************************************************
 Class      : CICPCONUnitDlg
 Function   : OnTimer
 Description: Wenn mehrere Thread vorhanden sind (für jeden Port einer), so werden die
              Threads nacheinander abgefragt. Der Timer triggert immer den ersten möglichen
              Thread an, dieser triggert alle weiteren. Erst nachdem alle Threads
              nacheinander gepollt haben, wird erneut abgefragt.
              Wenn ein Thread hängt (beim Öffnen des Ports), so wird dies bei den nächsten
				  Timerevents erkannt. Die anderen Threads werden so nach einer kurzen
				  Verzögerung weiterhin abgefragt.

 Parameters:   
  nIDEvent: (E): Pollevent  (UINT)

 Result type:  (void)
 created: May, 05 2003
 <TITLE OnTimer>
*********************************************************************************************/
void CICPCONUnitDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nPollTimer)
	{
#ifdef _DEBUG
//		CThreadDebug td(_T("OnPollTimer"));
#endif
		int nCount = m_ThreadList.GetCount();
		if (nCount)
		{
			POSITION pos = m_ThreadList.GetHeadPosition();
			while (pos)															// Threads prüfen
			{
				CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
				BOOL bPollState = pThread->IncrementPollState();	// Pollt er zu lange (hängt)?
				if (bPollState == 5) // 3
				{
					CICPI7000Module*pModule = NULL;
					m_bThreadsArePolling = FALSE;							// die nächsten Threads starten

					WK_TRACE(_T("Error thread poll timeout on COM%d\n") , pThread->GetPort());
					POSITION posM = pThread->m_Modules.GetHeadPosition();
					while (posM)
					{
						pModule = (CICPI7000Module*) pThread->m_Modules.GetNext(posM);
						if (IsWindowVisible())
						{
							WORD wOld = pModule->GetFailSeconds();
							pModule->SetFailCount(1);
							SendMessage(WM_USER, USER_MSG_ALARM, (LPARAM)pModule);
							pModule->SetFailCount(wOld);
						}
					}
					if (pModule)
					{
						SendMessage(WM_USER, USER_MSG_PORT_ERROR, (LPARAM)pModule);
					}
				}
			}

			if (m_bThreadsArePolling)										// Kommt die Timernachricht zu oft
			{
				m_bThreadsArePolling++;
				if (m_bThreadsArePolling > 10)
				{
					m_bThreadsArePolling = FALSE;
				}
				WK_TRACE(_T("ThreadsArePolling %d\n"), m_bThreadsArePolling);
				return;															// nix tun
			}

			nCount = 0;															// Threadnummer in der Threadliste
			pos = m_ThreadList.GetHeadPosition();						// Pollvorgang für die Threads starten
			while (pos)
			{
				CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
				if (   pThread->HasFailed()								// Portfehler ?
					 || pThread->IsPolling())								// oder Pollt er gerade
				{
					TEST_TRACE(_T("ThreadFailed %d\n"), pThread->GetPort());
					continue;													// nächsten Thread nehmen
				}
				else																// 
				{
					if (pThread->DoPoll())									// nur zu pollende Threads pollen
					{
#ifdef _DEBUG
//						WK_TRACE(_T("PostThreadMessage(%d)\n"), pThread->GetPort());
#endif
						m_bThreadsArePolling = TRUE;						// ersten möglichen Thread anstoßen
						pThread->PostThreadMessage(WM_TIMER, nIDEvent, MAKELONG(nCount, 0));
						break;
					}
				}
				nCount++;
			}
		}
	}
	else if (nIDEvent == EVENT_REQUEST_RESET)
	{
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)																// Threads prüfen
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			bool     bRunThread = false;
			POSITION posM = pThread->m_Modules.GetHeadPosition();
			while (posM)
			{
				CICPI7000Module *pModule = (CICPI7000Module*) pThread->m_Modules.GetNext(posM);
				if (pModule->HasInputs() || pModule->HasOutputs())	// wurde einer gefunden der gepollt werden soll
				{
					bRunThread = true;
					break;
				}
			}
			if (bRunThread) pThread->ResumeThread();
			else            pThread->SuspendThread();
		}
	}
	else if (nIDEvent == 100)
	{
		if (InitList())
		{
			KillTimer(nIDEvent);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
CICPI7000Module* CICPCONUnitDlg::FindModule(int nComPort, int nAddress)
{
	POSITION pos = m_ThreadList.GetHeadPosition();
	while (pos)																// Threads prüfen
	{
		CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
		POSITION posM = pThread->m_Modules.GetHeadPosition();
		while (posM)
		{
			CICPI7000Module *pModule = (CICPI7000Module*) pThread->m_Modules.GetNext(posM);
			if ((pModule->GetComPort() == nComPort) && (pModule->GetModuleAddress() == nAddress))
			{
				return pModule;
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::DestroyControlThreads(bool bImmediate)
{
	int nCount = m_ThreadList.GetCount();
	if (nCount)
	{
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			pThread->StopThread(bImmediate);
			delete pThread;
		}
		m_ThreadList.RemoveAll();
	}
	m_ModuleList.DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::CreateControlThreads()
{
	CWK_Profile wkpModules(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocationModules, _T(""));
	CWK_Profile wkpUnit(   CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	CWK_Profile wkpBase(   CWK_Profile::WKP_DEFAULT, HKEY_LOCAL_MACHINE, _T(""), _T(""));

	m_nPollTime     = wkpUnit.GetInt(SETTINGS_SECTION, ICP_POLLTIME     , m_nPollTime);
	m_nTimeout      = wkpUnit.GetInt(SETTINGS_SECTION, ICP_TIMEOUT      , m_nTimeout);

	const int nValueNameSize = 64;
	TCHAR  szValueName[nValueNameSize];
	LONG  lResult = ERROR_SUCCESS;
	CICPI7000Module *pModule=NULL;
	HKEY  hModuleKey = NULL;
	int   nModule=0, nAddress, nComPort, nBaudrate, nInput, nMultipleAccess, nPollState, nGroupID;
	CString strSection, strText;
	bool  bStart = false;
	m_bPollState = FALSE;
	hModuleKey = wkpUnit.GetSectionKey(REG_LOC_MODULE_UNIT);
	if (hModuleKey)
	{
		for (nModule=0; lResult==ERROR_SUCCESS; nModule++)			// Enum the Modules
		{
			lResult = RegEnumKey(hModuleKey, nModule, (LPTSTR)szValueName, nValueNameSize-1);
			if (lResult == ERROR_SUCCESS)
			{
				_stscanf(szValueName, ICP_GROUP_FMT, &nGroupID);
				if      ((nGroupID & 0x0000f000) == SECID_GROUP_INPUT ) nInput = 1;
				else if ((nGroupID & 0x0000f000) == SECID_GROUP_OUTPUT) nInput = 0;
				else continue;													// nur Input oder Ouptut zugelassen

				if (!wkpModules.GetInt(szValueName, ICP_MODULE_ENABLED, 0))
					continue;													// Disabled 
				
				nAddress        = wkpModules.GetInt(szValueName, ICP_ADDRESS , -1);
				if (nAddress  == -1) continue;
				nComPort        = wkpModules.GetInt(szValueName, ICP_COMPORT , -1);
				if (nComPort  == -1) continue;
				nBaudrate       = wkpModules.GetInt(szValueName, ICP_BAUDRATE, -1);
				if (nBaudrate == -1) continue;

				nMultipleAccess = wkpModules.GetInt(szValueName, ICP_MULTIPLE_ACCESS, 0);
				nPollState      = wkpModules.GetInt(szValueName, ICP_POLL_STATE, 0);
				if (nPollState) m_bPollState = TRUE;
				CString strLocation = wkpModules.GetString(szValueName, ICP_MODULE_LOCATION, 0);
				if (strLocation.IsEmpty()) strLocation = _T("No Location");
				bStart = false;
				pModule = FindModule(nComPort, nAddress);
				if (pModule == NULL)
				{
					pModule = new CICPI7000Module();
					pModule->AddRef();
					pModule->SetComPort((WORD)nComPort);
					pModule->SetModuleAddress((WORD)nAddress);
					pModule->SetBaudrate(nBaudrate);
				}
				else
				{
					ASSERT(pModule->GetBaudrate() == (DWORD)nBaudrate);
				}
				
				pModule->SetModuleID(wkpModules.GetString(szValueName, ICP_MODULEID, NULL));
				pModule->SetTimeOut((WORD)m_nTimeout);
				if (nInput)
				{
					if (nPollState) pModule->SetPollDI(true);
					if (pModule->HasInputs())
					{
						bStart = pModule->CreateCIPCInput(this, (WORD)nGroupID);
					}
				}
				else
				{
					if (nPollState) pModule->SetPollDO(true);
					if (pModule->HasOutputs())
					{
						bStart = pModule->CreateCIPCOutput(this, (WORD)nGroupID);
					}
				}
				if (bStart)
				{
					WK_TRACE(_T("Start Module %s, %s (%02x%02x): %x\n"), pModule->GetModuleIDstring(), strLocation, nComPort, nAddress, nGroupID);
					SetPortThread((WORD)nComPort, pModule, nMultipleAccess?true:false);
					pModule = NULL;
				}
				else
				{
					pModule->Release();
				}
			}
		}
	}
	
	ASSERT(m_nPollTimer == 0);

	if (m_bPollState)
	{
		m_nPollTimer = SetTimer(POLL_TIMER, m_nPollTime, NULL);
	}
	else
	{
		PostMessage(WM_TIMER, m_nPollTimer, 0);
	}

	if (wkpUnit.GetInt(SETTINGS_SECTION, ICP_SWITCH_PANEL, 0))
	{
		CreateSwitchPanel();
	}
	else if (WK_IS_WINDOW(m_pSwitchPanel))
	{
		m_pSwitchPanel->PostMessage(WM_CLOSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPCONUnitDlg::OnUserMessage(WPARAM wParam,  LPARAM lParam)
{
	if (wParam == USER_MSG_POLL_THREADS_READY)
	{
#ifdef _DEBUG
//		WK_TRACE(_T("USER_MSG_POLL_THREADS_READY\n"));
#endif
		m_bThreadsArePolling = FALSE;
	}
	else if (wParam == USER_MSG_ALARM)
	{
		int i, j, k;
		CICPI7000Module *pModule = (CICPI7000Module*) lParam;
		if (pModule->IsValid())
		{
			for (j=0; j<2; j++)
			{
				if ((j == OUTPUT_PINS) && !pModule->HasOutputs())
					continue;
				if ((j == (int)INPUT_PINS) && !pModule->HasInputs())
					continue;
			
				lParam = pModule->GetPinID((BYTE)j);
				for (i=0; i<m_ModuleList.GetItemCount(); i++)
				{
					if (ICPCON_MODULE(m_ModuleList.GetItemData(i)) == ICPCON_MODULE(lParam))
					{
						for (k=0; k<16; k++)
						{
							lParam = pModule->GetPinID((BYTE)j, (BYTE)k);
							TCHAR szState[2] = _T("x");
							if (lParam & ICPCON_PIN_FAIL)
							{
								szState[0] = _T('F');
							}
							else if (lParam & ICPCON_PIN_ACTIVE)
							{
								szState[0] = (lParam & ICPCON_PIN_ON) ? _T('1') : _T('0');
							}
							m_ModuleList.SetItemText(i, ICPCON_GET_PIN(lParam)+1, szState);
						}
					}
				}
			}
		}
	}
	else if (wParam == USER_MSG_PORT_ERROR)
	{
		CString sError;
		CICPI7000Module *pModule = (CICPI7000Module*) lParam;
		sError.Format(_T("Error opening Port %d for ICP CON Module\n"), pModule->GetComPort());
		DWORD dwParam1 = pModule->GetPinID(0);
		DWORD dwParam2 = MAKELONG(0x0001, PortError); // Alarm on, I7000 errorcode
		CWK_RunTimeError RTerr(WAI_ICPCON_UNIT, REL_WARNING, RTE_EXTERNAL_DEVICE_FAILURE, sError, dwParam1, dwParam2);
		RTerr.Send();
		Sleep(20);
		RTerr.SetParam2(0);	// alarm off
		RTerr.SetErrorText(_T(""));
		RTerr.Send();
	}
	else if (wParam == USER_MSG_RESTART_TIMER)
	{
		SAFE_KILL_TIMER(m_nPollTimer);
		Sleep(MulDiv(m_nPollTime, 1, 2));
		m_nPollTimer = SetTimer(POLL_TIMER, m_nPollTime, NULL);
	}
	else if (wParam == EVENT_REQUEST_STATE)
	{
		CICPI7000Module *pM = (CICPI7000Module*) lParam;
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)															// Threads prüfen
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			CICPI7000Module*pModule = NULL;
			POSITION posM = pThread->m_Modules.GetHeadPosition();
			while (posM)
			{
				pModule = (CICPI7000Module*) pThread->m_Modules.GetNext(posM);
				if (pModule == pM)
				{
					pThread->PostThreadMessage(WM_TIMER, m_nPollTimer, NULL);
					break;
				}
			}
		}
	}
	else if ((wParam > USER_MSG_MODULE_ERROR) && (wParam <= USER_MSG_MODULE_LAST_ERROR))
	{
		CString sError;
		CICPI7000Module *pModule = (CICPI7000Module*) lParam;
		WORD wError = (WORD)(wParam - USER_MSG_MODULE_ERROR);
		sError.Format(_T("Error %d accessing ICP CON Module %s, Port %d, Address %02x\n"), wError, pModule->GetModuleIDstring(), pModule->GetComPort(), pModule->GetModuleAddress());
		DWORD dwParam1 = pModule->GetPinID(0);
		DWORD dwParam2 = MAKELONG(0x0001, wError); // alarm on, I7000 errorcode
		CWK_RunTimeError RTerr(WAI_ICPCON_UNIT, REL_WARNING, RTE_EXTERNAL_DEVICE_FAILURE, sError, dwParam1, dwParam2);
		RTerr.Send();
		Sleep(20);
		RTerr.SetParam2(0);	// alarm off
		RTerr.SetErrorText(_T(""));
		RTerr.Send();
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPCONUnitDlg::SetPortThread(WORD wPort, void *pModule, bool bMultipleAccess)
{
	int nCount = m_ThreadList.GetCount();
	if (nCount)
	{
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			if (pThread->GetPort() == (int) wPort)
			{
				pThread->AddModule(pModule);
				return true;
			}
		}
	}
	CICPI7000Thread*pNewThread = new CICPI7000Thread((int) wPort, pModule, this, bMultipleAccess);
	m_ThreadList.AddHead(pNewThread);
	pNewThread->CreateThread();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPCONUnitDlg::InitList()
{
	if (WK_IS_WINDOW(&m_ModuleList))
	{
		m_ModuleList.DeleteAllItems();
		CString strText;
		DWORD   dwData;
		int     nItem;
		int     nCount = 0, nValid = 0;
		BOOL    bValid;
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			POSITION posM = pThread->m_Modules.GetHeadPosition();
			while (posM)
			{
				CICPI7000Module *pModule = (CICPI7000Module*) pThread->m_Modules.GetNext(posM);
				nCount++;
				bValid = pModule->IsValid();
				if ((bValid & 0x03) == 0x03 || (bValid & 0x30) == 0x30)
				{
					nValid++;
					if (bValid & 0x01)
					{
						dwData = pModule->GetPinID(INPUT_PINS);
						nItem  = m_ModuleList.GetItemCount();
						strText.Format(_T("%08xh"), dwData);
						m_ModuleList.InsertItem(nItem, strText);
						m_ModuleList.SetItemData(nItem, dwData);
					}
					if (bValid & 0x10)
					{
						dwData = pModule->GetPinID(OUTPUT_PINS);
						nItem  = m_ModuleList.GetItemCount();
						strText.Format(_T("%08xh"), dwData);
						m_ModuleList.InsertItem(nItem, strText);
						m_ModuleList.SetItemData(nItem, dwData);
					}
					PostMessage(WM_USER, USER_MSG_ALARM, (LPARAM)pModule);
				}
			}
		}
		if (nCount)
		{
			return (nCount == nValid) ? true : false;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (!theApp.m_bStandalone && bShow)
	{
		SetTimer(100, 1000, NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::CreateSwitchPanel()
{
	if (m_pSwitchPanel == NULL)
	{
		m_pSwitchPanel = new CSwitchPanel;
	}
	if (!WK_IS_WINDOW(m_pSwitchPanel))
	{
		m_pSwitchPanel->Create(IDD_SWITCH_PANEL, this);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPCONUnitDlg::OnEnterMenuLoop(WPARAM, LPARAM)
{
	theApp.m_bIsInMenuLoop = TRUE;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT  CICPCONUnitDlg::OnExitMenuLoop(WPARAM, LPARAM)
{
	theApp.m_bIsInMenuLoop = FALSE;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::SetLogText(LPCTSTR szBuffer)
{
	m_cLogWindow.SetSel(-1, -1);
	m_cLogWindow.ReplaceSel(szBuffer);
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONUnitDlg::OnClickListModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LVHITTESTINFO lvhti = {0};
	
	GetCursorPos(&lvhti.pt);
	m_ModuleList.ScreenToClient(&lvhti.pt);
	int nItem = m_ModuleList.HitTest(&lvhti);
	if (nItem != -1)
	{
		m_ModuleList.SubItemHitTest(&lvhti);
		lvhti.iSubItem--;
		DWORD lModule = ICPCON_MODULE(m_ModuleList.GetItemData(nItem));
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			POSITION posM = pThread->m_Modules.GetHeadPosition();
			while (posM)
			{
				CICPI7000Module *pModule = (CICPI7000Module*) pThread->m_Modules.GetNext(posM);
				if (ICPCON_MODULE(pModule->GetPinID(INPUT_PINS)) == lModule)
				{
					bool bState = pModule->GetDIbit(lvhti.iSubItem);
					pModule->SetDIbit(lvhti.iSubItem, !bState);
					pModule->SetAlarm(lvhti.iSubItem, !bState);
				}
				else if (ICPCON_MODULE(pModule->GetPinID(OUTPUT_PINS)) == lModule)
				{
					bool bState = pModule->GetDObit(lvhti.iSubItem);
					pModule->SetDObit(lvhti.iSubItem, !bState);
					pModule->SetDOstateChanged(true);
				}
			}
		}
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPCONUnitDlg::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#else
	return 0;
#endif

}
