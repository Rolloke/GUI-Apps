// RemoteControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CHHLA.h"
#include "RemoteControl.h"
#include "RemoteControlDlg.h"
#include "CIPC.h"
#include "WK_Names.h"
#include "ClientControl.h"
#include "Ini2Reg.h"
#include "WK_Util.h"
#include "WK_runtimeError.h"

#include "..\\MiniClient\\MiniClientDef.h"
#include "oemgui\oemguiapi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT ErrorThreadProc(LPVOID lpParam)
{
	CRemoteControlDlg *pWnd	= (CRemoteControlDlg*)lpParam;
	if (pWnd)
	{
		WORD	wCamID;
		BOOL	bFlag;
		CWK_RunTimeError RunTimeError;
		while (pWnd->GetRunStatus())
		{
			if (RunTimeError.ReadGlobalError())
			{
				if (RunTimeError.GetError() == RTE_CAMERA_MISSING)
				{
					wCamID	= (WORD)RunTimeError.GetParam1();
					bFlag	= RunTimeError.GetParam2();
					AfxGetMainWnd()->PostMessage(WM_RTE_VIDO_MISSING, wCamID, bFlag);
					RunTimeError.ConfirmGlobalError();					
				}		
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRemoteControlDlg, CDialog)
	//{{AFX_MSG_MAP(CRemoteControlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_MESSAGE(WM_CONF_CAMERA_PARAMETER,	OnConfirmGetCameraParameter)
	ON_MESSAGE(WM_SOCKET_CLOSED,			OnSocketClosed)
	ON_MESSAGE(WM_RTE_VIDO_MISSING,			OnCameraMissing)
	ON_WM_QUERYOPEN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CRemoteControlDlg::CRemoteControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteControlDlg::IDD, pParent)
{
	m_nMaxClients = GetPrivateProfileInt(_T("Private"),	_T("MaxCameras"),MAX_CLIENTS, INIFILENAME);
	m_nMaxClients = min(m_nMaxClients, MAX_CLIENTS);
	m_bForceMinimizeDialog = GetPrivateProfileInt(_T("Private"),_T("ForceMinimizeDlg"),1, INIFILENAME);
	m_bIsInit	= FALSE;
	m_bMiniMize = FALSE;

	//{{AFX_DATA_INIT(CRemoteControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_HH);

	// Pointer auf NULL initialisieren
	for (WORD wI = 0; wI <= m_nMaxClients; wI++)
		m_pClientCtrl[wI]   = NULL;

	m_nServerIPCnt					= 1;
	m_hServer						= NULL;
	m_hMiCoUnit						= NULL;
	m_pCHHLA						= NULL;
	m_pErrorThread					= NULL;

	// Server sollte beim Start nicht laufen
	CWnd* pWnd = FindWindow(WK_APP_NAME_SERVER, NULL);
	if (pWnd && IsWindow(pWnd->m_hWnd))
	{
		pWnd->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		Sleep(1000);
	}
	
	// MiCoUnit sollte beim Start nicht laufen
	pWnd = FindWindow(NULL, _T("JaCobUnit1"));
	if (pWnd && IsWindow(pWnd->m_hWnd))
	{
		pWnd->PostMessage(WM_CLOSE, 0, 0);
		Sleep(1000);
	}
}

/////////////////////////////////////////////////////////////////////////////
CRemoteControlDlg::~CRemoteControlDlg()
{
	StopErrorThread();

	// Alle Clients schlieﬂen
	for (WORD wI = 1; wI <= m_nMaxClients; wI++)
		WK_DELETE(m_pClientCtrl[wI]);

	WK_DELETE(m_pCHHLA);

	// Server beenden
	CWnd* pWnd = FindWindow(WK_APP_NAME_SERVER, NULL);
	if (pWnd && IsWindow(pWnd->m_hWnd))
		pWnd->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
	
	// MiCoUnit beenden
	pWnd = FindWindow(NULL, _T("JaCobUnit1"));
	if (pWnd && IsWindow(pWnd->m_hWnd))
		pWnd->PostMessage(WM_CLOSE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoteControlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowPos(&CWnd::wndTop, 0,0,0,0,SWP_NOSIZE | SWP_SHOWWINDOW);

	if (m_bForceMinimizeDialog)
		ShowWindow(SW_MINIMIZE);

	StartErrorThread();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
CClientControl* CRemoteControlDlg::GetClientControl(WORD wI)
{
	if ((wI > m_nMaxClients) || (wI == 0))
		return NULL;

	return m_pClientCtrl[wI];
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		COemGuiApi::AboutDialog(this, TRUE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::OnPaint() 
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

	// Nach dem der Dialog zusehen ist, mit der Initialisierung beginnen.
	if (!m_bIsInit)
	{
		m_bIsInit = TRUE;
		if (!Init())
			PostMessage(WM_CLOSE, 0, 0);
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CRemoteControlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
int CRemoteControlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
/* Konfiguration nur f¸r die MiCoISA notwendig
	CWK_Profile prof;
	if (prof.GetInt(_T("RemoteControl"), _T("Init"), 0) == 0)
	{
		AfxMessageBox(IDS_HARDWARE_NOT_CONFIGURED, MB_OK); 	
		Load(_T("Configure.exe"), SW_SHOW);
		return -1;
	}
*/
  	CMenu* pM;
	pM = GetSystemMenu(FALSE);
 
	pM->RemoveMenu(SC_RESTORE,MF_BYCOMMAND);
	pM->RemoveMenu(SC_MAXIMIZE,MF_BYCOMMAND);
	pM->RemoveMenu(SC_MINIMIZE,MF_BYCOMMAND);
	pM->RemoveMenu(SC_MOVE,MF_BYCOMMAND);
	pM->RemoveMenu(SC_SIZE,MF_BYCOMMAND);
	pM->RemoveMenu(0,MF_BYPOSITION);

	return 0;
}

//////////////////////////////////////////////////////////////////////
BOOL CRemoteControlDlg::Init()
{
	// Server starten
	SetStatusText(_T("Loading Server..."));
	m_hServer = Load(_T("Sec3.exe"), SW_MINIMIZE);

	if (!m_hServer)
		return FALSE;

	// MiCo starten
	SetStatusText(_T("Loading JaCobUnit..."));
	m_hMiCoUnit = Load(_T("JaCobUnit.exe"), SW_MINIMIZE);
	
	if (!m_hMiCoUnit)
		return FALSE;

	Sleep(10000);

	// Clients starten...
	for (WORD wI = 1; wI <= m_nMaxClients; wI++)
	{
		CString s;
		s.Format(_T("Loading Client %u"), wI);
		SetStatusText(s);
 		m_pClientCtrl[wI] = new CClientControl(_T("MiniClient.exe"), wI, m_hWnd);
		if (!m_pClientCtrl[wI] || !m_pClientCtrl[wI]->IsValid())
		{
			s.Format(_T("Can't start Client %u"), wI);
			SetStatusText(s);
			return FALSE;
		}
	}
	
	// HHLA-Objekt anlegen....
	m_pCHHLA	= new CHHLA(this);
	if (!m_pCHHLA)
		return FALSE;

	// Verbindung aufbauen
	DoConnection();

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
HANDLE CRemoteControlDlg::Load(const CString &sExePathName, DWORD dwStartFlags)
{
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	DWORD				dwStartTime			= GetTickCount();
	CString				sCmdLine			= _T("");
	CString				sWorkingDirectory	= _T(".");
	DWORD				dwTimeOut			= 5000;
	BOOL				bWait				= FALSE;
	HANDLE				hProzess			= NULL;

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
	startUpInfo.wShowWindow		= (WORD)dwStartFlags;  
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sCommandLine;
	sCommandLine.Format(_T("%s %s"), (const _TCHAR*)sExePathName, (const _TCHAR *)sCmdLine);
	
	BOOL bProcess = CreateProcess(
						NULL, // (LPCSTR)sExePathName,	// pointer to name of executable module 
						(LPTSTR)sCommandLine.GetBuffer(0),	// pointer to command line string
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
						FALSE,	// handle inheritance flag 
						NORMAL_PRIORITY_CLASS,//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
						(LPVOID)NULL,	// pointer to new environment block 
						(LPCTSTR)sWorkingDirectory,	// pointer to current directory name 
						(LPSTARTUPINFO)&startUpInfo,	// pointer to STARTUPINFO 
						(LPPROCESS_INFORMATION)&prozessInformation 	// pointer to PROCESS_INFORMATION  
						);

	sCommandLine.ReleaseBuffer();
	
	if (bProcess)
	{
		DWORD dwIdleTime=0;
		DWORD rWFII;

		rWFII = WaitForInputIdle(prozessInformation.hProcess, dwTimeOut);
		if (rWFII == 0)
		{
			dwIdleTime = GetTickCount();	
			WK_TRACE(_T("%s gestartet %d ms\n"),(const _TCHAR*)sCommandLine, dwIdleTime-dwStartTime);

		}
		else if (rWFII == WAIT_TIMEOUT)
			WK_TRACE_ERROR(_T("%s wait for input idle timeout %d\n"),(const _TCHAR*)sCommandLine, dwStartTime);
		
		if (bWait)
			WaitForSingleObject(prozessInformation.hProcess,INFINITE);
		hProzess = prozessInformation.hProcess;
	}
	else
	{
		WK_TRACE_ERROR(_T("Could not start %s\n"),sExePathName);
		CString sTemp;
		sTemp.Format(_T("'%s' konnte nicht gestartet werden\n"), (LPCTSTR)sExePathName); 
		AfxMessageBox(sTemp, MB_OK);
	}
	
	return hProzess;
}

/////////////////////////////////////////////////////////////////////////////
BOOL  CRemoteControlDlg::IsRunning(HANDLE hProzess)
{
	DWORD dwExitCode = 0;
	if (!::GetExitCodeProcess(hProzess, &dwExitCode))
		return FALSE;

	return (dwExitCode == STILL_ACTIVE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteControlDlg::DoConnection()
{
	CString sServer			= _T("");
	CString sServerIP		= _T("");
	CString sAppID			= _T("");
	TCHAR	szBuffer[255]	= {0};
	int		nConnectPort	= 0;
	int		nServerCount	= 0;

	if (!m_pCHHLA)
		return FALSE;

	KillTimer(1);

	// Der Reihe nach alle IP-Adressen abklappern
	do
	{
		// Anzahl der eingetragenen ServerIP's aus der Camera.ini lesen
		nServerCount  = GetPrivateProfileInt(_T("SERVER_ADDR"),	_T("SERVER_COUNTER"), 1, INIFILENAME);

		// ServerIP aus der Camera.ini lesen
		sServer.Format(_T("SERVER%u"), m_nServerIPCnt);
		if (GetPrivateProfileString(_T("SERVER_ADDR"), sServer, _T("127.0.0.1"), szBuffer, 255, INIFILENAME))
			sServerIP = szBuffer;

		// ApplicationsID aus der Camera.ini lesen
		if (GetPrivateProfileString(_T("APPL_CODE"), _T("CODE"), _T("Default!!"), szBuffer, 255, INIFILENAME))
			sAppID = szBuffer;
	
		// ConnectPort aus der Camera.ini lesen
		nConnectPort  = GetPrivateProfileInt(_T("SERVER_PORT"),	_T("PORT"),	10000, INIFILENAME);
		
		// Verbindung zum Server aufbauen
		if (m_pCHHLA->DoConnection(sServerIP, nConnectPort, sAppID))
			break;
		
		// Verbindungsaufbau gescheitert. N‰chste ServerIP verwenden.
		if (m_nServerIPCnt++ >= nServerCount)
			m_nServerIPCnt = 1;
	
	} while (TRUE);

	SetTimer(1, 5000, NULL);
	MessageBeep(0);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::OnTimer(UINT nIDEvent) 
{
	static DWORD dwLastTC = GetTickCount();
	KillTimer(1);

	DWORD dwNoTraffic = GetPrivateProfileInt(_T("TIMEOUTS"), _T("NO_TRAFFIC"), 5, INIFILENAME)*1000;
	
	if ((GetTickCount() - dwLastTC) > dwNoTraffic)
	{
		dwLastTC = GetTickCount();
		if (!m_pCHHLA || !m_pCHHLA->DoConfirmOnline())
		{
			WK_DELETE(m_pCHHLA);
			Sleep(100);
			m_pCHHLA	= new CHHLA(this);
			if (m_pCHHLA)
				DoConnection();
		}	
	}
	
	// Aktuellen Statustext sichern
	CString sNew, sOld;
	GetStatusText(sOld);
	BOOL bStatusTextChanged = FALSE;

	if (!IsRunning(m_hServer))
	{
		// Server starten
		SetStatusText(_T("Loading Server..."));
		bStatusTextChanged = TRUE;
		m_hServer = Load(_T("Sec3.exe"), SW_MINIMIZE);
		Sleep(6000);
	}

	if (!IsRunning(m_hMiCoUnit))
	{
		// Server starten
		SetStatusText(_T("Loading JaCobUnit..."));
		bStatusTextChanged = TRUE;
		m_hMiCoUnit = Load(_T("JaCobUnit.exe"), SW_SHOW);
	}

	for (WORD wClient = 1; wClient <= m_nMaxClients; wClient++)
	{
		if (IsRunning(m_hServer))
		{
			if (m_pClientCtrl[wClient] && !m_pClientCtrl[wClient]->IsRunning())
			{
				sNew.Format(_T("Loading Client %u"), wClient);
				SetStatusText(sNew);
				bStatusTextChanged = TRUE;
				WK_DELETE(m_pClientCtrl[wClient]);
 				m_pClientCtrl[wClient] = new CClientControl(_T("MiniClient.exe"), wClient, m_hWnd);
			}
		}
	}
	
	if (bStatusTextChanged)
	{
		// Den alten Statustext wieder anzeigen
		SetStatusText(sOld, TRUE);
	}

	CDialog::OnTimer(nIDEvent);

	SetTimer(1, 5000, NULL);
}

/////////////////////////////////////////////////////////////////////////////
long CRemoteControlDlg::OnSocketClosed(WPARAM wParam, LPARAM lParam)
{
	WK_DELETE(m_pCHHLA);
	Sleep(100);
	m_pCHHLA	= new CHHLA(this);
	if (m_pCHHLA)
		DoConnection();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
long CRemoteControlDlg::OnCameraMissing(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 1)
		WK_TRACE(_T("Kamera %u ist ausgefallen\n"), wParam);
	else if (lParam == 0)
		WK_TRACE(_T("Kamera %u arbeitet wieder\n"), wParam);

	if (m_pCHHLA)
		m_pCHHLA->CameraMissing((WORD)wParam, lParam);
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CRemoteControlDlg::OnConfirmGetCameraParameter(WPARAM wParam, LPARAM lParam)
{
	WORD wCamera = (WORD)wParam;

	// lParam 00000000 bbbbbbbb cccccccc ssssssss
	int nBrightness = HIBYTE(LOWORD(lParam));
	int nContrast   = LOBYTE(HIWORD(lParam));
	int nSaturation = HIBYTE(HIWORD(lParam));

	if (m_pCHHLA)
		m_pCHHLA->DoConfirmGetCameraParameter(wCamera, nBrightness, nContrast, nSaturation);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::SetStatusText(const CString &sText, BOOL bMiniMize /*= FALSE*/)
{
	SetDlgItemText(IDC_STATUS, sText);

	m_bMiniMize = bMiniMize;

	if (bMiniMize || m_bForceMinimizeDialog)
	{
		Sleep(1000);	// Die letzte Meldung noch ein wenig stehen lassen.
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE  | SWP_NOSIZE);
		ShowWindow(SW_MINIMIZE);
	}
	else
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE  | SWP_NOSIZE | SWP_SHOWWINDOW);
		ShowWindow(SW_SHOWNORMAL);
	}					 	 
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::GetStatusText(CString &sText)
{
	GetDlgItemText(IDC_STATUS, sText);
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::StartErrorThread()
{
	m_bRun = TRUE;

	m_pErrorThread = AfxBeginThread(ErrorThreadProc,this);
	if (m_pErrorThread)
		m_pErrorThread->m_bAutoDelete = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteControlDlg::StopErrorThread()
{
	m_bRun = FALSE;

	if (m_pErrorThread)
	{
		WaitForSingleObject(m_pErrorThread->m_hThread,2000);
		WK_DELETE(m_pErrorThread);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteControlDlg::OnQueryOpen() 
{
	return !m_bMiniMize;
}
