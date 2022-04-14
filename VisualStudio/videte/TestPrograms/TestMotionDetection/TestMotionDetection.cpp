// TestMotionDetection.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "TestMotionDetection.h"
#include "MainFrm.h"
#include ".\testmotiondetection.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define REQUEST		_T("Request")	// Command
#define CONFIRM		_T("Confirm")

#define SET_DATA	_T("SetData")	// SubCommand
#define GET_DATA	_T("GetData")

// Communication String
// Command;SubCommand;Address;Data
#define CMD_FORMAT	_T("%s;%s;%s;%s")
#define DATA_FORMAT	_T("%d,%d,%d,%d,%d")

/////////////////////////////////////////////////////////////////////////////
// CTestMotionDetectionApp
BEGIN_MESSAGE_MAP(CTestMotionDetectionApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_UPDATE, OnFileUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMotionDetectionApp-Erstellung
CTestMotionDetectionApp::CTestMotionDetectionApp()
{
	m_szBlock.cx = 5;
	m_szBlock.cy = 5;
	m_nTimeOut   = 2000;
	m_nType		 = MOVING_BLOCK_RANDOM;
	m_BackGroundColor = DEFAULT_COLOR;
	m_nMonitor	 = 0;

	m_bServer	 = FALSE;
	DWORD dwLen = MAX_PATH;
	GetComputerNameEx(ComputerNameDnsHostname, m_sOwnHost.GetBufferSetLength(dwLen), &dwLen);
	m_sOwnHost.ReleaseBuffer();
}
/////////////////////////////////////////////////////////////////////////////
CTestMotionDetectionApp::~CTestMotionDetectionApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CTestMotionDetectionApp-Objekt
CTestMotionDetectionApp theApp;

#ifdef _DEBUG
void CalculateFps(int nMaxFps, int nCameras, short &n25fps, short &n5fps, short &n1fps)
{
	int nC, nMax5fps, nMax25fps;
	nMax25fps = nMaxFps / 25;
	nMax5fps  = nMaxFps / 5;
	n1fps     = nMaxFps;
	n5fps = n25fps = 0;

	if (nMax25fps > 1)
	{
		n25fps = nMax25fps-2;
		n5fps = nMax5fps;
	}
	
	for (nC=nMaxFps-4; nC>0; nC-=4)
	{
		if (nC < nCameras)	break;
		if (n5fps == nMax5fps)
		{
			nC -= nCameras;
			if (nC < 4) break;
			n25fps++; 
			nC = nMaxFps - n25fps*25;
			if (nC < 0)
			{
				n25fps--;
				break;
			}
			n1fps = nC;
			nMax5fps = nC / 5;
			n5fps = 0;
			nC += 4;
			continue;
		}
		n5fps++;
		n1fps -= 5;
	}

	nC = n1fps + n5fps+n25fps;
	if (nC < nCameras)
	{
		n1fps+=5;
		n5fps--;
	}
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CTestMotionDetectionApp Initialisierung
BOOL CTestMotionDetectionApp::InitInstance()
{
	CWinApp::InitInstance();

#ifdef _DEBUG
	int nFps, nCam;
	short n1fps, n5fps, n25fps;
	TRACE(_T("fps:cam: 25:  5:  1:sum\n"));
	nFps = 93;
	nCam = 9;
	for (nFps=52; nFps<109; nFps++)
//	for (nCam=56; nCam>0; nCam--)
	{
		CalculateFps(nFps, nCam, n25fps, n5fps, n1fps);
		TRACE(_T("%3d:%3d:%3d:%3d:%3d:%3d=%3d\n"),nFps, nCam, n25fps, n5fps, n1fps, n25fps+n5fps+n1fps, n25fps*25+n5fps*5+n1fps);
	}
#endif

	if(!AfxSocketInit())
	{
		TRACE(_T("!AfxSocketInit()\n"));
	}

	SetRegistryKey(_T("Videte"));


	m_szBlock.cx     = GetProfileInt(SETTINGS, BLOCK_SIZE_X, m_szBlock.cx);
	m_szBlock.cy     = GetProfileInt(SETTINGS, BLOCK_SIZE_Y, m_szBlock.cy);
	m_nTimeOut       = GetProfileInt(SETTINGS, TIMER_TIMEOUT, m_nTimeOut);
	m_BackGroundColor= GetProfileInt(SETTINGS, BACKGROUNDCOLOR, m_BackGroundColor);
	m_nType			 = GetProfileInt(SETTINGS, DRAW_TYPE, m_nType);
	m_nMonitor		 = GetProfileInt(SETTINGS, MONITOR, m_nMonitor);

	m_bServer        = GetProfileInt(SETTINGS, IS_SERVER, m_bServer);
	m_Socket.m_nPort = GetProfileInt(SETTINGS, SOCKET_PORT, m_Socket.m_nPort);
	m_Socket.m_sHost = GetProfileString(SETTINGS, SOCKET_ADDRESS, m_Socket.m_sHost);

	CString sAppName = m_pszAppName;
	CString sLogFile = sAppName+_T(".log");

	// Debugger öffnen
	InitDebugger(sLogFile, WAI_INVALID, NULL);

	InitSocket();

	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;

	m_pMainWnd = pFrame;
	// Rahmen mit Ressourcen erstellen und laden
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// Rufen Sie DragAcceptFiles nur auf, wenn eine Suffix vorhanden ist.
	//  In einer SDI-Anwendung ist dies nach ProcessShellCommand erforderlich
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CTestMotionDetectionApp::ExitInstance()
{
	WK_TRACE(_T("ExitInstance\n"));
	m_Socket.Close();

	WriteProfileInt(SETTINGS, BLOCK_SIZE_X, m_szBlock.cx);
	WriteProfileInt(SETTINGS, BLOCK_SIZE_Y, m_szBlock.cy);
	WriteProfileInt(SETTINGS, TIMER_TIMEOUT, m_nTimeOut);
	WriteProfileInt(SETTINGS, DRAW_TYPE, m_nType);
	WriteProfileInt(SETTINGS, MONITOR, m_nMonitor);
	WriteProfileInt(SETTINGS, BACKGROUNDCOLOR, m_BackGroundColor);

	WriteProfileInt(SETTINGS, IS_SERVER, m_bServer);
	WriteProfileInt(SETTINGS, SOCKET_PORT, m_Socket.m_nPort);
	WriteProfileString(SETTINGS, SOCKET_ADDRESS, m_Socket.m_sHost);

	CloseDebugger();

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
// CTestMotionDetectionApp Meldungshandler
void CTestMotionDetectionApp::OnFileUpdate()
{
	if (   m_bServer 
		&& m_Socket.m_hSocket != (SOCKET)INVALID_HANDLE_VALUE)
	{
		CString sSend;
		sSend.Format(CMD_FORMAT, CONFIRM, GET_DATA, m_Socket.m_sHost, FormatData());
		SendToClient(sSend);
	}
	else if (!m_bServer
		 && m_Socket.m_hSocket != (SOCKET)INVALID_HANDLE_VALUE)
	{
		CString sSend;
		sSend.Format(CMD_FORMAT, REQUEST, SET_DATA, m_Socket.m_sHost, FormatData());
		SendToServer(sSend);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTestMotionDetectionApp::OnReceive(const CString&sCmd, const CString&sSubCmd, const CString&sAddress, const CString&sData)
{
	if (sCmd == REQUEST && m_bServer)
	{
		if (sSubCmd == SET_DATA)
		{
			if (ScanData(sData))
			{
				m_pMainWnd->PostMessage(WM_COMMAND, ID_APPLY_NOW);
			}
			else
			{
				// no confirm
			}
		}
		else if (sSubCmd == GET_DATA)
		{
			m_sRemoteHost = sAddress;
			m_pMainWnd->PostMessage(WM_COMMAND, ID_FILE_UPDATE);
		}
	}
	else if (sCmd == CONFIRM && !m_bServer)
	{
		if (sSubCmd == GET_DATA)
		{
			if (ScanData(sData))
			{
				m_AboutDlg.UpdateData(FALSE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// Anwendungsbefehl zum Ausführen des Dialogfelds
void CTestMotionDetectionApp::OnAppAbout()
{
	m_AboutDlg.m_sHost = m_Socket.m_sHost;
	m_AboutDlg.m_nPort = m_Socket.m_nPort;
	DataToDlg();
	if (!m_bServer)
	{
		CString sSend;
		sSend.Format(CMD_FORMAT, REQUEST, GET_DATA, m_sOwnHost, FormatData());
		SendToServer(sSend);
	}
	if (m_AboutDlg.DoModal() == IDOK)
	{
		DlgToData();
		if (   m_AboutDlg.m_sHost != m_Socket.m_sHost
			|| m_AboutDlg.m_nPort != m_Socket.m_nPort)
		{
			if (m_Socket.m_hSocket != (SOCKET)INVALID_HANDLE_VALUE)
			{
				m_Socket.Close();
			}
			m_Socket.m_sHost = m_AboutDlg.m_sHost;
			m_Socket.m_nPort = m_AboutDlg.m_nPort;
			InitSocket();
		}
		m_pMainWnd->PostMessage(WM_COMMAND, ID_APPLY_NOW);
		m_pMainWnd->PostMessage(WM_COMMAND, ID_FILE_UPDATE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTestMotionDetectionApp::InitSocket()
{
	BOOL bResult = FALSE;
	LONG lEvents = FD_READ;
	if (!m_bServer)
	{
		lEvents |= FD_WRITE;
	}
	bResult = m_Socket.Create(m_Socket.m_nPort, SOCK_DGRAM, lEvents);
	if (bResult)
	{
		if (m_bServer)
		{
			WK_TRACE(_T("Socket created listening to port %d\n"), m_Socket.m_nPort);
		}
		else
		{
			bResult = m_Socket.Connect(m_Socket.m_sHost, m_Socket.m_nPort);
			if (!bResult) bResult = GetLastError();
			WK_TRACE(_T("Socket Connected to %s, port:%d: %d\n"), m_Socket.m_sHost, m_Socket.m_nPort, bResult);
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CTestMotionDetectionApp::SendToServer(const CString&sSend)
{
	BOOL bResult = m_Socket.Send((void*)LPCTSTR(sSend), sSend.GetLength()*sizeof(TCHAR));
	if (SOCKET_ERROR  == bResult)
	{
		WK_TRACE(_T("Server Send error: %d\n"), GetLastError());
		return TRUE;
	}
	else
	{
		WK_TRACE(_T("Send to Server: %s\n"), sSend);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CTestMotionDetectionApp::SendToClient(const CString&sSend)
{
	BOOL bResult = m_Socket.SendTo((void*)LPCTSTR(sSend), sSend.GetLength()*sizeof(TCHAR), m_Socket.m_nPort, m_sRemoteHost);
	if (SOCKET_ERROR  == bResult)
	{
		WK_TRACE(_T("Client Send error: %d\n"), GetLastError());
		return TRUE;
	}
	else
	{
		WK_TRACE(_T("Send to Client: %s\n"), sSend);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CTestMotionDetectionApp::FormatData()
{
	CString sData;
	sData.Format(DATA_FORMAT, m_nTimeOut, m_szBlock.cx, m_szBlock.cy, m_nType, m_BackGroundColor);
	return sData;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTestMotionDetectionApp::ScanData(const CString &sData)
{
	int n = _stscanf(sData, DATA_FORMAT, &m_nTimeOut, &m_szBlock.cx, &m_szBlock.cy, m_nType, m_BackGroundColor);
	return (n == 5) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CTestMotionDetectionApp::DataToDlg()
{
	m_AboutDlg.m_nTimeOut = m_nTimeOut;
	m_AboutDlg.m_szBlock  = m_szBlock;
	m_AboutDlg.m_nType    = m_nType-1;
}
/////////////////////////////////////////////////////////////////////////////
void CTestMotionDetectionApp::DlgToData()
{
	m_nTimeOut = m_AboutDlg.m_nTimeOut;
	m_szBlock  = m_AboutDlg.m_szBlock;
	m_nType    = m_AboutDlg.m_nType+1;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_BLOCK_CX, m_szBlock.cx);
	DDV_MinMaxInt(pDX, m_szBlock.cx, 1, 2000);
	DDX_Text(pDX, IDC_EDT_BLOCK_CY, m_szBlock.cy);
	DDV_MinMaxInt(pDX, m_szBlock.cy, 1, 2000);
	DDX_Text(pDX, IDC_EDT_TIME_OUT, m_nTimeOut);
	DDV_MinMaxInt(pDX, m_nTimeOut, 20, 100000);
	DDX_Text(pDX, IDC_EDT_HOST, m_sHost);
	DDX_Text(pDX, IDC_EDT_PORT, m_nPort);
	DDX_Radio(pDX, IDC_RD_TYPE0, m_nType);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_RD_TYPE0, OnBnClickedRdType0)
	ON_BN_CLICKED(IDC_RD_TYPE1, OnBnClickedRdType0)
	ON_BN_CLICKED(IDC_RD_TYPE2, OnBnClickedRdType0)
	ON_BN_CLICKED(IDC_RD_TYPE3, OnBnClickedRdType0)
	ON_BN_CLICKED(IDC_RD_TYPE4, OnBnClickedRdType0)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// class CMDTestSocket is derived from CAsyncSocket
CMDTestSocket::CMDTestSocket()
{
	m_nPort      = 1927; // standard videte
	m_sHost.Empty();
}
/////////////////////////////////////////////////////////////////////////////
void CMDTestSocket::OnReceive(int nErrorCode)
{
   TCHAR buff[256];
   int nRead;
   nRead = Receive(buff, 256) / sizeof(TCHAR); 
   
   switch (nRead)
   {
   case SOCKET_ERROR:
      if (GetLastError() != WSAEWOULDBLOCK) 
      {
         AfxMessageBox(_T("Error occurred"));
         Close();
      }
      break;
   default:
		if (nRead > 0 && nRead < 4096)
		{
			buff[nRead] = 0; //terminate the string
			WK_TRACE(_T("%s\n"), buff);
			CString sAddress;
			CStringArray ar;
			SplitString(buff, ar, _T(';'));
			if (ar.GetCount() >=4)
			{
				theApp.OnReceive(ar.GetAt(0),ar.GetAt(1),ar.GetAt(2),ar.GetAt(3));
			}
		}break;
   }
   CAsyncSocket::OnReceive(nErrorCode);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	HICON hIcon = LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_RGBCMY));
	SendDlgItemMessage(IDC_RD_TYPE1, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
	hIcon = LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_SHADE));
	SendDlgItemMessage(IDC_RD_TYPE2, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
	ShowHide();
	return TRUE;
}

void CAboutDlg::ShowHide()
{
	int nShowCX = SW_HIDE;
	int nShowCY = SW_HIDE;
	int nShowTimeOut = SW_HIDE;
	switch(m_nType+1)
	{
		case MOVING_BLOCK_RANDOM:
			nShowTimeOut = SW_SHOW;
			nShowCX = SW_SHOW;
			nShowCY = SW_SHOW;
			break;
		case COLORS_RGBCMY:
			break;
		case SHADE_WHITE_TO_BLACK:
			nShowCX = SW_SHOW;
			break;
		case BLINK_WHITE_AND_BLACK:
			nShowTimeOut = SW_SHOW;
			break;
		case HORZ_AND_VERT_LINES:
			nShowCX = SW_SHOW;
			nShowCY = SW_SHOW;
			break;
		case TEST_PICTURE5:
			nShowTimeOut = SW_SHOW;
			nShowCX = SW_SHOW;
			nShowCY = SW_SHOW;
			break;
		case TEST_PICTURE6:
			break;
		case TEST_PICTURE7:
			break;
		case TEST_PICTURE8:
			break;
		case TEST_PICTURE9:
			break;
	}
	GetDlgItem(IDC_EDT_BLOCK_CX)->ShowWindow(nShowCX);
	GetDlgItem(IDC_EDT_BLOCK_CY)->ShowWindow(nShowCY);
	GetDlgItem(IDC_EDT_TIME_OUT)->ShowWindow(nShowTimeOut);
}

void CAboutDlg::OnBnClickedRdType0()
{
	CDataExchange dx(this, TRUE);
	DDX_Radio(&dx, IDC_RD_TYPE0, m_nType);
	ShowHide();
}
