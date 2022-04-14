// ICPCONClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ICPCONClient.h"
#include "ICPCONClientDlg.h"
#include "ICP7000Thread.h"
#include "i7000.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ICP_IMAGE_PORT    0
#define ICP_IMAGE_MODULE  1
#define ICP_IMAGE_INPUTS  2
#define ICP_IMAGE_OUTPUTS 3
#define ICP_IMAGE_INFO    4


#define POLL_PORT      1
#define POLL_ALL_PORTS 2

#define USER_MSG_GETCHECK_STATE         1
#define USER_MSG_SET_THREAD_CONTROL_BTN 2
#define USER_MSG_EXPAND_TREE_ITEM       3
#define USER_MSG_SET_PROGRESS_ADDRESS   4
#define USER_MSG_SET_PROGRESS_PORT      5
#define USER_MSG_DELETE_IF_NO_CHILDREN  6
#define USER_MSG_SETCHECK_STATE         7
#define USER_MSG_TOGGLE_CHECK_STATE     8

extern CIPCCONClient theApp;

/////////////////////////////////////////////////////////////////////////////
// CIPCCONClientDlg dialog
CIPCCONClientDlg::CIPCCONClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPCCONClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIPCCONClientDlg)
	m_dwBaudrate     = 9600;
	m_nComPort       = 1;
	m_strModuleID    = _T("");
	m_nModuleAddress = 2;
	m_nSizeX      = 50;
	m_nSizeY      = 30;
	m_bState      = FALSE;
	m_bBaud1200   = FALSE;
	m_bBaud2400   = FALSE;
	m_bBaud4800   = FALSE;
	m_bBaud9600   = FALSE;
	m_bBaud19200  = FALSE;
	m_bBaud38400  = FALSE;
	m_bBaud57600  = FALSE;
	m_bBaud115200 = FALSE;
	m_nTimeOut    = 100;
	m_nEdgeOrLevel    = -1;
	m_nPollTime = 1500;
	m_nTimeoutEdge    = 500;
	m_bPollState      = FALSE;
	m_nMinimizeTime    = 10;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   m_hThread     = INVALID_HANDLE_VALUE;
   m_nThreadID   = 0;
	m_nWhat       = 0;
	m_bStopThread = 0;
	m_pModule     = NULL;
	m_nActive     = -1;
	m_bAllowInputs= FALSE;
	m_hAccelerator     = NULL;
	m_pTreeEdit = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPCCONClientDlg)
	DDX_Control(pDX, IDC_PROGRESS_PORT, m_cProgressPorts);
	DDX_Control(pDX, IDC_PROGRESS_ADDRESS, m_cProgressAddress);
	DDX_Control(pDX, IDC_TREE_OF_MODULES, m_cTreeOfModules);
	DDX_Text(pDX, IDC_EDT_PORT, m_nComPort);
	DDV_MinMaxInt(pDX, m_nComPort, 1, 255);
	DDX_Text(pDX, IDC_EDT_MODULE_ADDRESS, m_nModuleAddress);
	DDV_MinMaxInt(pDX, m_nModuleAddress, 0, 255);
	DDX_Text(pDX, IDC_EDT_BUTTON_SIZE_X, m_nSizeX);
	DDV_MinMaxInt(pDX, m_nSizeX, 20, 200);
	DDX_Text(pDX, IDC_EDT_BUTTON_SIZE_Y, m_nSizeY);
	DDV_MinMaxInt(pDX, m_nSizeY, 20, 200);
	DDX_Check(pDX, IDC_CK_STATE, m_bState);
	DDX_Check(pDX, IDC_CK_BAUD_1200, m_bBaud1200);
	DDX_Check(pDX, IDC_CK_BAUD_2400, m_bBaud2400);
	DDX_Check(pDX, IDC_CK_BAUD_4800, m_bBaud4800);
	DDX_Check(pDX, IDC_CK_BAUD_9600, m_bBaud9600);
	DDX_Check(pDX, IDC_CK_BAUD_19200, m_bBaud19200);
	DDX_Check(pDX, IDC_CK_BAUD_38400, m_bBaud38400);
	DDX_Check(pDX, IDC_CK_BAUD_57600, m_bBaud57600);
	DDX_Check(pDX, IDC_CK_BAUD_115200, m_bBaud115200);
	DDX_Text(pDX, IDC_EDT_TIMEOUT, m_nTimeOut);
	DDV_MinMaxInt(pDX, m_nTimeOut, 10, 2000);
	DDX_Radio(pDX, IDC_RD_EDGE, m_nEdgeOrLevel);
	DDX_Text(pDX, IDC_EDT_TIME_POLL_RELAIS, m_nPollTime);
	DDV_MinMaxInt(pDX, m_nPollTime, 50, 60000);
	DDX_Text(pDX, IDC_EDT_TIMEOUT_EDGE, m_nTimeoutEdge);
	DDV_MinMaxInt(pDX, m_nTimeoutEdge, 100, 1000);
	DDX_Check(pDX, IDC_CK_POLL_STATE, m_bPollState);
	DDX_Text(pDX, IDC_EDT_TIMEOUT_MINIMIZE, m_nMinimizeTime);
	DDV_MinMaxInt(pDX, m_nMinimizeTime, 0, 240);
	//}}AFX_DATA_MAP
//	DDX_Control(pDX, IDC_EDT_ERROR_LOG, m_CErrorLog);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIPCCONClientDlg, CDialog)
	//{{AFX_MSG_MAP(CIPCCONClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_POLL_PORT, OnBtnPollPort)
	ON_BN_CLICKED(IDC_BTN_SCAN_ALL_PORTS, OnBtnScanAllPorts)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_OF_MODULES, OnDeleteitemTreeOfModules)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_OF_MODULES, OnBeginlabeleditTreeOfModules)
	ON_BN_CLICKED(IDC_BTN_SAVE_SETTINGS, OnBtnSaveSettings)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_OF_MODULES, OnEndlabeleditTreeOfModules)
	ON_NOTIFY(NM_CLICK, IDC_TREE_OF_MODULES, OnClickTreeOfModules)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_CK_STATE, OnCkState)
	ON_BN_CLICKED(IDC_RD_EDGE, OnRdEdgeOrLevel)
	ON_BN_CLICKED(IDC_CK_MULTIPLE_ACCESS, OnCkComStates)
	ON_BN_CLICKED(IDC_CK_POLL_STATE, OnCkModuleStates)
	ON_BN_CLICKED(IDC_BTN_DELETE_TREE, OnBtnDeleteTree)
	ON_COMMAND(IDC_EDIT_TREE_LABEL, OnEditTreeLabel)
	ON_BN_CLICKED(IDC_RD_LEVEL, OnRdEdgeOrLevel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER, OnUserMessage)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CIPCCONClientDlg message handlers
BOOL CIPCCONClientDlg::OnInitDialog()
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	m_nSizeX           = wkp.GetInt(SETTINGS_SECTION, ICP_BUTTON_DX        , m_nSizeX);
	m_nSizeY           = wkp.GetInt(SETTINGS_SECTION, ICP_BUTTON_DY        , m_nSizeY);
	m_nComPort         = wkp.GetInt(SETTINGS_SECTION, ICP_COMPORT          , m_nComPort);
	m_nPollTime        = wkp.GetInt(SETTINGS_SECTION, ICP_POLLTIME         , m_nPollTime);
	m_nTimeoutEdge     = wkp.GetInt(SETTINGS_SECTION, ICP_TIMEOUT_EDGE     , m_nTimeoutEdge);
	m_nMinimizeTime    = wkp.GetInt(SETTINGS_SECTION, ICP_MINIMIZE_TIME    , m_nMinimizeTime);
	m_bAllowInputs     = wkp.GetInt(SETTINGS_SECTION, _T("AllowInputs"), m_bAllowInputs) ? true : false;
	DWORD dwBaudrates  = wkp.GetInt(SETTINGS_SECTION, ICP_BAUDRATES, 0);
	if (dwBaudrates)
	{
		if (dwBaudrates&0x00000001) m_bBaud1200   = TRUE;
		if (dwBaudrates&0x00000002) m_bBaud2400   = TRUE;
		if (dwBaudrates&0x00000004) m_bBaud4800   = TRUE;
		if (dwBaudrates&0x00000008) m_bBaud9600   = TRUE;
		if (dwBaudrates&0x00000010) m_bBaud19200  = TRUE;
		if (dwBaudrates&0x00000020) m_bBaud38400  = TRUE;
		if (dwBaudrates&0x00000040) m_bBaud57600  = TRUE;
		if (dwBaudrates&0x00000080) m_bBaud115200 = TRUE;
	}
	else
	{
		m_bBaud9600 = TRUE;
	}
	
	CDialog::OnInitDialog();

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

	m_cProgressPorts.SetRange(0, 255);
	m_cProgressPorts.SetPos(0);
	m_cProgressAddress.SetRange(0, 255);
	m_cProgressAddress.SetPos(0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnPaint() 
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
HCURSOR CIPCCONClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnDestroy() 
{
	OnBtnCancel();
	if (ComPortStatus(m_nComPort) == ComportOpened)
	{
		Close_Com(m_nComPort);
	}

	CDialog::OnDestroy();

	if (m_hAccelerator)
	{
		DestroyAcceleratorTable(m_hAccelerator);
		m_hAccelerator = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
unsigned int  CIPCCONClientDlg::ThreadProc(void *pParam)
{
	CIPCCONClientDlg*pThis = (CIPCCONClientDlg*)pParam;
	if (pThis->m_nWhat == POLL_PORT)
	{
		pThis->OnPollPort();
	}
	else if (pThis->m_nWhat == POLL_ALL_PORTS)
	{
		pThis->OnScanAllPorts();
	}
	pThis->m_hThread   = INVALID_HANDLE_VALUE;
	pThis->m_nThreadID = 0;
	pThis->m_nWhat     = 0;
	pThis->m_bStopThread = FALSE;
	pThis->PostMessage(WM_USER, USER_MSG_SET_THREAD_CONTROL_BTN, FALSE);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnBtnPollPort() 
{
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
		UpdateData();
		m_nWhat   = POLL_PORT;
		SetThreadControlBtn(TRUE);
	   m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
   }
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnBtnCancel() 
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		m_bStopThread = TRUE;
		WaitForSingleObject(m_hThread, 15000);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnBtnDeleteTree() 
{
	m_cTreeOfModules.DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::SetThreadControlBtn(BOOL bThread)
{
	GetDlgItem(IDC_BTN_SCAN_ALL_PORTS)->EnableWindow(!bThread);
	GetDlgItem(IDC_BTN_POLL_PORT)->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_9600  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_1200  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_4800  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_19200 )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_2400  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_38400 )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_57600 )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_115200)->EnableWindow(!bThread);
	((CEdit*)GetDlgItem(IDC_EDT_TIMEOUT       ))->SetReadOnly(bThread);
	((CEdit*)GetDlgItem(IDC_EDT_PORT          ))->SetReadOnly(bThread);
	((CEdit*)GetDlgItem(IDC_EDT_MODULE_ADDRESS))->SetReadOnly(bThread);
	GetDlgItem(IDC_TREE_OF_MODULES)->EnableWindow(!bThread);

	GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(bThread);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnPollPort() 
{
	CString str, strPort, strModule, strIO;
	WORD    wRetValue;
	WORD    wModuleID = 0, wT = 0;
	int     i, nModuleAddress, nFirst = 1, nBaud;
	char    szCmd[80], szResult[80];
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocationPorts, _T(""));
	CICPI7000Module *pModule = NULL;
	HTREEITEM hTemp = NULL;
	HTREEITEM hPort = NULL, hModule = NULL;


	for (nBaud=0; nBaud<7; nBaud++)
	{
		if (ComPortStatus(m_nComPort) == ComportOpened)
		{
			Close_Com(m_nComPort);
		}

		if (m_bStopThread) break;

		if (nBaud==0)
		{
			if (m_bBaud2400) m_dwBaudrate = 2400;
			else continue;
		}
		else if (nBaud==1)
		{
			if (m_bBaud4800) m_dwBaudrate = 4800;
			else continue;
		}
		else if (nBaud==2)
		{
			if (m_bBaud9600) m_dwBaudrate = 9600;
			else continue;
		}
		else if (nBaud==3)
		{
			if (m_bBaud19200) m_dwBaudrate = 19200;
			else continue;
		}
		else if (nBaud==4)
		{
			if (m_bBaud38400) m_dwBaudrate = 38400;
			else continue;
		}
		else if (nBaud==5)
		{
			if (m_bBaud57600) m_dwBaudrate = 57600;
			else continue;
		}
		else if (nBaud==6)
		{
			if (m_bBaud115200) m_dwBaudrate = 115200;
			else continue;
		}

		CTime t1 = CTime::GetCurrentTime();
		wRetValue = Open_Com(m_nComPort, m_dwBaudrate, 8, 0, 0);
		CTime t2 = CTime::GetCurrentTime();
		CTimeSpan ts = t2 - t1;
		if (ts.GetSeconds() > 1)
		{
			return;
		}
		
		PostMessage(WM_USER, USER_MSG_SET_PROGRESS_ADDRESS, 0);

		if (wRetValue == NoError)
		{
			strPort.Format(IDS_COM_PORT, m_nComPort);
			int nMultipleAccess = 0;
			nMultipleAccess = wkp.GetInt(strPort, ICP_MULTIPLE_ACCESS, nMultipleAccess);
			nMultipleAccess = (nMultipleAccess&FLAG_MULTIPLE_ACCESS) ? 1:0;
			hPort =m_cTreeOfModules.InsertItem(strPort, ICP_IMAGE_PORT, nMultipleAccess);
			m_cTreeOfModules.SetCheck(hPort, wkp.GetInt(strPort, _T(""), 0));
			for (nModuleAddress=0; nModuleAddress <= 0xff; nModuleAddress++)
			{
				if (m_bStopThread) break;

				PostMessage(WM_USER, USER_MSG_SET_PROGRESS_ADDRESS, nModuleAddress);

				wsprintf(szCmd, "$%02xM", nModuleAddress);			// Modulname
				wRetValue = Send_Receive_Cmd(m_nComPort, szCmd, szResult, m_nTimeOut, 0, &wT);
				if ((wRetValue != NoError) || (szResult[0] != '!'))
				{
					continue;
				}

				pModule = new CICPI7000Module();
				if (!pModule->SetModuleID(szResult))
				{
					delete pModule;
					continue;
				}
				pModule->SetComPort(m_nComPort);
				pModule->SetModuleAddress(nModuleAddress);
				pModule->SetBaudrate(m_dwBaudrate);
				
				int nModuleState = 0;
				strModule.Format(_T("%s\\%02x"), strPort, nModuleAddress);
				nModuleState = wkp.GetInt(strModule, _T("State"), nModuleState);

				str.Format(IDS_PORT_MODULES, nModuleAddress, &szResult[3]);
				hModule = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_MODULE, nModuleState, hPort);
				str.Format(IDS_BAUDRATE, m_dwBaudrate);
				m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, ICP_IMAGE_INFO, hModule);

				m_cTreeOfModules.SetCheck(hModule, wkp.GetInt(strModule, _T(""), 0));
				m_cTreeOfModules.SetItemData(hModule, (DWORD)pModule);
				m_cTreeOfModules.InsertItem(pModule->GetDIOmode(), ICP_IMAGE_INFO, ICP_IMAGE_INFO, hModule);

				wsprintf(szCmd, "$%02xF", nModuleAddress);			// Version
				wRetValue = Send_Receive_Cmd(m_nComPort, szCmd, szResult, m_nTimeOut, 0, &wT);
				if (wRetValue == NoError)
				{
					str.Format(IDS_FIRMWARE_VERSION, &szResult[3]);
					m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, ICP_IMAGE_INFO, hModule);
				}
				else
				{
					WK_TRACE(_T("Error:%d, Send_Receive_Cmd(%d, %s)\n"), wRetValue, m_nComPort, szCmd);
					m_cTreeOfModules.DeleteItem(hModule);
					continue;
				}

				wsprintf(szCmd, "$%02x2", nModuleAddress);			// Config
				wRetValue = Send_Receive_Cmd(m_nComPort, szCmd, szResult, m_nTimeOut, 0, &wT);
				if (wRetValue == NoError)
				{
					if (!pModule->SetConfig(szResult))
					{
						m_cTreeOfModules.DeleteItem(hModule);
						continue;
					}
				}
				else
				{
					WK_TRACE(_T("Error:%d, Send_Receive_Cmd(%d, %s)\n"), wRetValue, m_nComPort, szCmd);
					m_cTreeOfModules.DeleteItem(hModule);
					continue;
				}
				hTemp = m_cTreeOfModules.InsertItem(_T("Checksum"), ICP_IMAGE_INFO, ICP_IMAGE_INFO, hModule);
				m_cTreeOfModules.SetCheck(hTemp, pModule->GetChecksum());
				str = wkp.GetString(strModule, ICP_MODULE_LOCATION, NULL);
				if (str.IsEmpty()) str.LoadString(IDS_MODULE_LOCATION);
				hTemp = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, ICP_IMAGE_MODULE, hModule);
				if (pModule->HasInputs() && m_bAllowInputs)			// Inputs
				{
					str.LoadString(IDS_DIGITAL_IN);
					int nPollState = 0;
					strIO.Format(_T("%s\\%s"), strModule, ICP_INPUTS);
					nPollState = wkp.GetInt(strIO, ICP_POLL_STATE, nPollState);
					if (nPollState)  nPollState = FLAG_POLL_STATE;
					HTREEITEM hInputs = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, nPollState, hModule);
					m_cTreeOfModules.SetCheck(hInputs, wkp.GetInt(strIO, _T(""), 0));
					int nInputs = pModule->GetInputs();
					for (i=0; i<nInputs; i++)
					{
						str.Format(IDS_DIGITAL_IN_N, i+pModule->GetDIoffset());
						hTemp = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INPUTS, ICP_IMAGE_INPUTS, hInputs);
						m_cTreeOfModules.SetItemData(hTemp, i);
						str.Format(_T("%s\\DI%02d"), strIO, pModule->GetDIoffset()+i);
						m_cTreeOfModules.SetCheck(hTemp, wkp.GetInt(str, _T(""), 0));
						str = wkp.GetString(str, ICP_NAME, NULL);
						if (!str.IsEmpty())
						{
							m_cTreeOfModules.SetItemText(hTemp, str);
						}
					}
				}

				if (pModule->HasOutputs())										// Outputs
				{
					str.LoadString(IDS_DIGITAL_OUT);
					int nPollState = 0;
					strIO.Format(_T("%s\\%s"), strModule, ICP_OUTPUTS);
					nPollState = wkp.GetInt(strIO, ICP_POLL_STATE, nPollState);
					if (nPollState)  nPollState = FLAG_POLL_STATE;
					HTREEITEM hOutputs = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, nPollState, hModule);
					m_cTreeOfModules.SetCheck(hOutputs, wkp.GetInt(strIO, _T(""), 0));
					int nOutputs = pModule->GetOutputs();
					for (i=0; i<nOutputs; i++)
					{
						str.Format(IDS_DIGITAL_OUT_N, i+pModule->GetDOoffset());
						hTemp = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_OUTPUTS, 0, hOutputs);
						m_cTreeOfModules.SetItemData(hTemp, i);
						str.Format(_T("%s\\DO%02d"), strIO, pModule->GetDOoffset()+i);
						int nSelected = wkp.GetInt(str, ICP_CONTROL, 0);
						m_cTreeOfModules.SetCheck(hTemp, wkp.GetInt(str, _T(""), 0));
						str = wkp.GetString(str, ICP_NAME, NULL);
						m_cTreeOfModules.SetItemImage(hTemp, ICP_IMAGE_OUTPUTS, nSelected);
						if (!str.IsEmpty())
						{
							m_cTreeOfModules.SetItemText(hTemp, str);
						}
					}
				}

				if (hModule)
				{
					if (nFirst)
					{
						PostMessage(WM_USER, USER_MSG_EXPAND_TREE_ITEM, (LPARAM)hPort);
						nFirst = 0;
					}
					PostMessage(WM_USER, USER_MSG_EXPAND_TREE_ITEM, (LPARAM)hModule);
				}
				else
				{
					m_cTreeOfModules.DeleteItem(hPort);
				}
				hModule = NULL;
			}
			Close_Com(m_nComPort);
			PostMessage(WM_USER, USER_MSG_DELETE_IF_NO_CHILDREN, (LPARAM)hPort);
			hPort = NULL;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnBtnScanAllPorts() 
{
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
		UpdateData();
		m_cTreeOfModules.DeleteAllItems();
		m_nWhat   = POLL_ALL_PORTS;
		SetThreadControlBtn(TRUE);
	   m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
   }
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnScanAllPorts() 
{
	int nPort = m_nComPort;
	for (m_nComPort = 1; m_nComPort <= 0xff; m_nComPort++)
	{
		if (m_bStopThread) break;
		PostMessage(WM_USER, USER_MSG_SET_PROGRESS_PORT, m_nComPort);
		OnPollPort();
	}
	m_nComPort = nPort;
	PostMessage(WM_USER, USER_MSG_SET_PROGRESS_PORT, m_nComPort);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnDeleteitemTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	int nImage, nSelected;
	m_cTreeOfModules.GetItemImage(pNMTreeView->itemOld.hItem, nImage, nSelected);
	if (nImage == ICP_IMAGE_MODULE)
	{
		DWORD dwData = m_cTreeOfModules.GetItemData(pNMTreeView->itemOld.hItem);
		if (dwData)
		{
			CICPI7000Module *pModule = (CICPI7000Module*) dwData;
			delete pModule;
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnBeginlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	int nImage, nSelected;
	*pResult = 1;
	if (m_cTreeOfModules.GetItemImage(pTVDispInfo->item.hItem, nImage, nSelected))
	{
		if (nImage == ICP_IMAGE_INPUTS)
		{
			*pResult = 0;
		}
		else if (nImage == ICP_IMAGE_OUTPUTS)
		{
			*pResult = 0;
		}
		else if ((nImage == ICP_IMAGE_INFO) && (nSelected == ICP_IMAGE_MODULE))
		{
			*pResult = 0;
		}
	}
	if (*pResult == 0)
	{
		m_pTreeEdit = m_cTreeOfModules.GetEditControl();
		if (m_pTreeEdit)
		{
			m_pTreeEdit->ModifyStyle(0, ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN);
			
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnBtnSaveSettings() 
{
	CString strPort, strModule, strDIO, strTemp, strIO;
	int     nOutputs = 0;
	int     nInputs  = 0;
	bool    bChecked, bModuleChecked;
	CWK_Profile wkpPort( CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocationPorts, _T(""));
	CWK_Profile wkpClient(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation     , _T(""));
	HTREEITEM hPort = m_cTreeOfModules.GetRootItem();
	while (hPort)
	{
		bChecked = (m_cTreeOfModules.GetCheck(hPort)) ? true : false;
		HTREEITEM hModule = m_cTreeOfModules.GetNextItem(hPort, TVGN_CHILD);
		strPort = m_cTreeOfModules.GetItemText(hPort);
		int nImage, nState, nTemp;
		m_cTreeOfModules.GetItemImage(hPort, nImage, nState);
		wkpPort.WriteInt(strPort, _T(""), m_cTreeOfModules.GetCheck(hPort));
		nTemp = (nState & FLAG_MULTIPLE_ACCESS) ? 1:0;
		wkpPort.WriteInt(strPort, ICP_MULTIPLE_ACCESS, nTemp);
		while (hModule)
		{
			if (bChecked)
			{
				bModuleChecked = (m_cTreeOfModules.GetCheck(hModule)) ? true : false;
			}
			CICPI7000Module *pModule = (CICPI7000Module*)m_cTreeOfModules.GetItemData(hModule);
			if (pModule == NULL) break;
			strModule.Format(_T("%s\\%02x"), strPort, pModule->GetModuleAddress());
			wkpPort.WriteInt(strModule, _T(""), m_cTreeOfModules.GetCheck(hModule));
			wkpPort.WriteString(strModule, ICP_ITEMID  , pModule->GetItemID());
			wkpPort.WriteString(strModule, ICP_MODE    , pModule->GetDIOmode());
			wkpPort.WriteInt(   strModule, ICP_BAUDRATE, pModule->GetBaudrate());
			
			HTREEITEM hTemp = m_cTreeOfModules.GetNextItem(hModule, TVGN_CHILD);
			while (hTemp)
			{
				if (m_cTreeOfModules.ItemHasChildren(hTemp))
				{
					if (bModuleChecked)
					{
						bChecked = (m_cTreeOfModules.GetCheck(hTemp)) ? true : false;
					}
					int i = 0, nImage, nSelected;
					HTREEITEM hDIO = m_cTreeOfModules.GetNextItem(hTemp, TVGN_CHILD);
					while (hDIO)
					{
						m_cTreeOfModules.GetItemImage(hDIO, nImage, nSelected);
						strTemp = m_cTreeOfModules.GetItemText(hDIO);
						BOOL bCheck = m_cTreeOfModules.GetCheck(hDIO);
						if (nImage == ICP_IMAGE_INPUTS)
						{
							strIO.Format(_T("%s\\%s\\DI%02d"), strModule, ICP_INPUTS, pModule->GetDIoffset()+i);
							if (bChecked && bCheck) nInputs++;
						}
						else
						{
							strIO.Format(_T("%s\\%s\\DO%02d"), strModule, ICP_OUTPUTS, pModule->GetDOoffset()+i);
							if (bChecked && bCheck) nOutputs++;
						}
						wkpPort.WriteInt(strIO, _T(""), bCheck);
						strTemp.Replace(_T("\\n"), _T("\r\n"));
						wkpPort.WriteString(strIO, ICP_NAME, strTemp);
						wkpPort.WriteInt(strIO, ICP_CONTROL, nSelected);
						hDIO = m_cTreeOfModules.GetNextItem(hDIO, TVGN_NEXT);
						i++;
					}
					m_cTreeOfModules.GetItemImage(hTemp, nImage, nSelected);
					strDIO = strIO.Left(strIO.ReverseFind(_T('\\')));
					wkpPort.WriteInt(strDIO, _T(""), m_cTreeOfModules.GetCheck(hTemp));
					nSelected = (nSelected & FLAG_POLL_STATE) ? 1 : 0;
					wkpPort.WriteInt(strDIO, ICP_POLL_STATE, nSelected);
				}
				else
				{
					int nImage, nSelected;
					m_cTreeOfModules.GetItemImage(hTemp, nImage, nSelected);
					if ((nImage == ICP_IMAGE_INFO) && (nSelected == ICP_IMAGE_MODULE))
					{
						wkpPort.WriteString(strModule, ICP_MODULE_LOCATION, m_cTreeOfModules.GetItemText(hTemp));
					}
				}
				hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_NEXT);
			}
			hModule = m_cTreeOfModules.GetNextItem(hModule, TVGN_NEXT);
		}
		hPort = m_cTreeOfModules.GetNextItem(hPort, TVGN_NEXT);
	}

	wkpClient.WriteInt(SETTINGS_SECTION, ICP_OUTPUTS  ,nOutputs);
	wkpClient.WriteInt(SETTINGS_SECTION, ICP_INPUTS   ,nInputs);
	GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnEndlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if (pTVDispInfo->item.mask	& TVIF_TEXT)
	{
		m_cTreeOfModules.SetItemText(pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);
		GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow();
	}
	m_pTreeEdit = NULL;
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnClickTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO tvHTI;
	GetCursorPos(&tvHTI.pt);
	m_cTreeOfModules.ScreenToClient(&tvHTI.pt);

	m_cTreeOfModules.HitTest(&tvHTI);
	if (tvHTI.flags & TVHT_ONITEMSTATEICON)
	{
		int nImage, nSelected;
		m_cTreeOfModules.GetItemImage(tvHTI.hItem, nImage, nSelected);
		if ((nImage == ICP_IMAGE_INFO)  && !m_cTreeOfModules.ItemHasChildren(tvHTI.hItem))
		{
			PostMessage(WM_USER, USER_MSG_TOGGLE_CHECK_STATE, (LPARAM)tvHTI.hItem);
		}
		else if (tvHTI.hItem)
		{
			PostMessage(WM_USER, USER_MSG_GETCHECK_STATE, (LPARAM)tvHTI.hItem);
			int nImage, nSelected;
			m_cTreeOfModules.GetItemImage(tvHTI.hItem, nImage, nSelected);
			if (nImage == ICP_IMAGE_INFO)
			{
				PostMessage(WM_USER, USER_MSG_SETCHECK_STATE, (LPARAM)tvHTI.hItem);
			}
			GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow();
		}
	}
	if (tvHTI.flags & TVHT_ONITEM)
	{
		int nImage, nSelected, nParent, nTemp;
		BOOL bMultipleAccess = 0;
		HTREEITEM hTemp;
		CDataExchange dx(this, false);
		m_cTreeOfModules.GetItemImage(tvHTI.hItem, nImage, nSelected);
		if ((nImage == ICP_IMAGE_INPUTS)||(nImage == ICP_IMAGE_OUTPUTS))
		{
			BOOL  bEnable = TRUE;
			WORD  wRet = NoError;
			m_nEdgeOrLevel = nSelected;
			hTemp = tvHTI.hItem;
			while(hTemp) 
			{
				m_cTreeOfModules.GetItemImage(hTemp, nParent, nTemp);
				if (nParent == ICP_IMAGE_MODULE)
				{
					m_pModule = (CICPI7000Module*)m_cTreeOfModules.GetItemData(hTemp);
					ASSERT(m_pModule != NULL);
					break;
				}
/*
				else if (nParent == ICP_IMAGE_PORT)
				{
					bMultipleAccess = (nTemp & FLAG_MULTIPLE_ACCESS) ? 1 : 0;
				}
				else if (nParent == ICP_IMAGE_INFO)
				{
					m_bPollState    = (nTemp & FLAG_POLL_STATE     ) ? 1 : 0;
				}
*/
				hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_PARENT);
			};

			m_pModule->SetTimeOut(m_nTimeOut);
			m_nActive = m_cTreeOfModules.GetItemData(tvHTI.hItem);
			m_pModule->OpenPort();
			if ((nImage == ICP_IMAGE_OUTPUTS))
			{
				wRet = m_pModule->DigitalOutReadBack();
				m_bState = m_pModule->GetDObit(m_nActive) ? TRUE: FALSE;
			}
			else if (m_pModule->HasInputs())
			{
				wRet = m_pModule->DigitalIn();
				bEnable = FALSE;
				m_bState = m_pModule->GetDIbit(m_nActive) ? TRUE: FALSE;
			}
			m_pModule->ClosePort();
			DDX_Check(&dx, IDC_CK_STATE, m_bState);
			DDX_Check(&dx, IDC_CK_MULTIPLE_ACCESS, bMultipleAccess);
			DDX_Check(&dx, IDC_CK_POLL_STATE, m_bPollState);
			DDX_Radio(&dx, IDC_RD_EDGE, m_nEdgeOrLevel);
			GetDlgItem(IDC_CK_STATE)->EnableWindow(bEnable);
			GetDlgItem(IDC_RD_EDGE)->EnableWindow(bEnable);
			GetDlgItem(IDC_RD_LEVEL)->EnableWindow(bEnable);
			GetDlgItem(IDC_CK_POLL_STATE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CK_MULTIPLE_ACCESS)->EnableWindow(FALSE);
		}
		else
		{
			nTemp = nSelected;
			hTemp = tvHTI.hItem;
			BOOL bEnableMA = FALSE;
			BOOL bEnablePS = FALSE;
			if (nImage == ICP_IMAGE_PORT)
			{
				bMultipleAccess = (nTemp & FLAG_MULTIPLE_ACCESS) ? 1 : 0;
				DDX_Check(&dx, IDC_CK_MULTIPLE_ACCESS, bMultipleAccess);
				bEnableMA = TRUE;
			}
			else if ((nImage == ICP_IMAGE_INFO)  && m_cTreeOfModules.ItemHasChildren(hTemp))
			{
				m_bPollState    = (nTemp & FLAG_POLL_STATE     ) ? 1 : 0;
				DDX_Check(&dx, IDC_CK_POLL_STATE, m_bPollState);
				bEnablePS = TRUE;
			}
			GetDlgItem(IDC_CK_POLL_STATE)->EnableWindow(bEnablePS);
			GetDlgItem(IDC_CK_MULTIPLE_ACCESS)->EnableWindow(bEnableMA);
			GetDlgItem(IDC_CK_STATE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RD_EDGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RD_LEVEL)->EnableWindow(FALSE);
/*
			hTemp = tvHTI.hItem;
			while(hTemp) 
			{
				m_cTreeOfModules.GetItemImage(hTemp, nParent, nTemp);
				if (nParent == ICP_IMAGE_PORT)
				{
					bMultipleAccess = (nTemp & FLAG_MULTIPLE_ACCESS) ? 1 : 0;
				}
				if ((nParent == ICP_IMAGE_INFO) && m_cTreeOfModules.ItemHasChildren(hTemp))
				{
					m_bPollState    = (nTemp & FLAG_POLL_STATE     ) ? 1 : 0;
				}
				hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_PARENT);
			};
			
			m_pModule = NULL;
			m_nEdgeOrLevel = -1;
			DDX_Radio(&dx, IDC_RD_EDGE, m_nEdgeOrLevel);
			DDX_Check(&dx, IDC_CK_MULTIPLE_ACCESS, bMultipleAccess);
			DDX_Check(&dx, IDC_CK_POLL_STATE, m_bPollState);
			GetDlgItem(IDC_CK_STATE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RD_EDGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RD_LEVEL)->EnableWindow(FALSE);
*/
		}
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnCkState() 
{
	if (m_pModule)
	{
		WORD  wRet = NoError;
		CDataExchange dx(this, true);
		DDX_Check(&dx, IDC_CK_STATE, m_bState);
		m_pModule->OpenPort();
		m_pModule->SetTimeOut(m_nTimeOut);
		m_pModule->SetDObit(m_nActive, m_bState);

		if (m_pModule->HasOutputs())
		{
			wRet = m_pModule->DigitalOut();
		}
		m_pModule->ClosePort();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnCkComStates() 
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	while (hItem)
	{
		int nImage, bState, nTemp;
		m_cTreeOfModules.GetItemImage(hItem, nImage, nTemp);
		if (nImage == ICP_IMAGE_PORT)
		{
			CDataExchange dx(this, true);
			nTemp = 0;
			DDX_Check(&dx, IDC_CK_MULTIPLE_ACCESS, bState);
			if (bState) nTemp |= FLAG_MULTIPLE_ACCESS;
			m_cTreeOfModules.SetItemImage(hItem, nImage, nTemp);
			GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow();
			break;
		}
		hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_PARENT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnCkModuleStates() 
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	while (hItem)
	{
		int nImage, bState, nTemp;
		m_cTreeOfModules.GetItemImage(hItem, nImage, nTemp);
		if (nImage == ICP_IMAGE_INFO && m_cTreeOfModules.ItemHasChildren(hItem))
		{
			CDataExchange dx(this, true);
			nTemp = 0;
			DDX_Check(&dx, IDC_CK_POLL_STATE, bState);
			if (bState) nTemp |= FLAG_POLL_STATE;
			m_cTreeOfModules.SetItemImage(hItem, nImage, nTemp);
			GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow();
			break;
		}
		hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_PARENT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnRdEdgeOrLevel() 
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	if (hItem)
	{
		int nImage, nSelected;
		m_cTreeOfModules.GetItemImage(hItem, nImage, nSelected);
		if (nImage == ICP_IMAGE_OUTPUTS)
		{
			CDataExchange dx(this, true);
			DDX_Radio(&dx, IDC_RD_EDGE, m_nEdgeOrLevel);
			nSelected = m_nEdgeOrLevel;
			m_cTreeOfModules.SetItemImage(hItem, nImage, nSelected);
			GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIPCCONClientDlg::OnUserMessage(WPARAM wParam,  LPARAM lParam)
{
	switch (wParam)
	{
		case USER_MSG_GETCHECK_STATE:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			BOOL bCheck = m_cTreeOfModules.GetCheck(hItem);
			if (bCheck)
			{
				while (hItem)
				{
					hItem = m_cTreeOfModules.GetParentItem(hItem);
					if (hItem)
					{
						m_cTreeOfModules.SetCheck(hItem, bCheck);
					}
				}
			}
		} break;
		case USER_MSG_SET_THREAD_CONTROL_BTN:
		{
			SetThreadControlBtn((BOOL)lParam);
		} break;
		case USER_MSG_EXPAND_TREE_ITEM:
		{
			m_cTreeOfModules.Expand((HTREEITEM)lParam, TVE_EXPAND);
			m_cTreeOfModules.InvalidateRect(NULL);
		} break;
		case USER_MSG_SET_PROGRESS_ADDRESS:
		{
			m_cProgressAddress.SetPos(lParam);
			SetDlgItemInt(IDC_EDT_MODULE_ADDRESS, lParam, FALSE);
		} break;
		case USER_MSG_SET_PROGRESS_PORT:
		{
			m_cProgressPorts.SetPos(m_nComPort);
			SetDlgItemInt(IDC_EDT_PORT, m_nComPort, FALSE);
		} break;
		case USER_MSG_DELETE_IF_NO_CHILDREN:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			if (!m_cTreeOfModules.ItemHasChildren(hItem))
			{
				m_cTreeOfModules.DeleteItem(hItem);
			}
		} break;
		case USER_MSG_SETCHECK_STATE:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			BOOL bCheck = m_cTreeOfModules.GetCheck(hItem);
			hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_CHILD);
			while (hItem)
			{
				m_cTreeOfModules.SetCheck(hItem, bCheck);
				hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_NEXT);
			}
		} break;
		case USER_MSG_TOGGLE_CHECK_STATE:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			BOOL bCheck = m_cTreeOfModules.GetCheck(hItem);
			m_cTreeOfModules.SetCheck(hItem, !bCheck);
		} break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnOK() 
{
	if (m_pTreeEdit)
	{
		m_pTreeEdit->ReplaceSel(_T("\r\n"));
		return;
	}
	if (GetDlgItem(IDC_BTN_SAVE_SETTINGS)->IsWindowEnabled())
	{
		OnBtnSaveSettings();
	}

	CDialog::OnOK();
	
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	wkp.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DX    , m_nSizeX);
	wkp.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DY    , m_nSizeY);
	wkp.WriteInt(SETTINGS_SECTION, ICP_TIMEOUT      , m_nTimeOut);
	wkp.WriteInt(SETTINGS_SECTION, ICP_COMPORT      , m_nComPort);
	wkp.WriteInt(SETTINGS_SECTION, ICP_POLLTIME     , m_nPollTime);
	wkp.WriteInt(SETTINGS_SECTION, ICP_TIMEOUT_EDGE , m_nTimeoutEdge);
	wkp.WriteInt(SETTINGS_SECTION, ICP_MINIMIZE_TIME, m_nMinimizeTime);

	DWORD dwBaudrates  = 0;
	if (m_bBaud1200  ) dwBaudrates|=0x00000001;
	if (m_bBaud2400  ) dwBaudrates|=0x00000002;
	if (m_bBaud4800  ) dwBaudrates|=0x00000004;
	if (m_bBaud9600  ) dwBaudrates|=0x00000008;
	if (m_bBaud19200 ) dwBaudrates|=0x00000010;
	if (m_bBaud38400 ) dwBaudrates|=0x00000020;
	if (m_bBaud57600 ) dwBaudrates|=0x00000040;
	if (m_bBaud115200) dwBaudrates|=0x00000080;
	wkp.WriteInt(SETTINGS_SECTION, ICP_BAUDRATES, dwBaudrates);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCCONClientDlg::OnEditTreeLabel() 
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	if (hItem)
	{
		m_cTreeOfModules.EditLabel(hItem);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CIPCCONClientDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_hAccelerator = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR)); 	
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCCONClientDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (m_hAccelerator)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg))
		{
			return TRUE;
		}
	}
		
	return CDialog::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
long CIPCCONClientDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_ICPCON_CLIENT, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
