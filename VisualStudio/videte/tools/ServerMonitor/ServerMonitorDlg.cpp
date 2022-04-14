// ServerMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ServerMonitor.h"
#include "ServerMonitorDlg.h"
#include "NetSpy.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerMonitorDlg dialog

CServerMonitorDlg::CServerMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerMonitorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerMonitorDlg)
	m_iSpyPort = 0;
	m_eReceiveData =_T("");
	m_sMessage = _T("");
	m_sCommand = _T("");
	m_sAddress = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pNetSpy = NULL;
	m_pClientConnection = NULL;
}
CServerMonitorDlg::~CServerMonitorDlg()
{
    if (m_pNetSpy)
	{
		delete m_pNetSpy;
		m_pNetSpy = NULL;
	}

	if (m_pClientConnection)
	{
		delete m_pClientConnection;
		m_pClientConnection = NULL;
	}
}


	

void CServerMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerMonitorDlg)
	DDX_Control(pDX, IDC_IPADDRESS, m_ctrlIP);
	DDX_Control(pDX, IDC_SendButton, m_SendButton);
	DDX_Control(pDX, IDC_COMMAND, m_cCommand);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_DATA, m_cReceiveData);
	DDX_Control(pDX, IDC_Stop_Listen_Button, m_StopListenButton);
	DDX_Control(pDX, IDC_Listening_Button, m_Listening_Button);
	DDX_Control(pDX, IDC_PORT, m_SpyPort);
	DDX_Text(pDX, IDC_PORT, m_iSpyPort);
	DDX_Text(pDX, IDC_DATA, m_eReceiveData);
	DDX_Text(pDX, IDC_MESSAGE, m_sMessage);
	DDX_Text(pDX, IDC_COMMAND, m_sCommand);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_sAddress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CServerMonitorDlg, CDialog)
	//{{AFX_MSG_MAP(CServerMonitorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_PORT, OnChangePort)
	ON_BN_CLICKED(IDC_Listening_Button, OnListeningButton)
	ON_BN_CLICKED(IDC_Stop_Listen_Button, OnStopListenButton)
	ON_EN_KILLFOCUS(IDC_DATA, OnKillfocusData)
	ON_EN_UPDATE(IDC_PORT, OnUpdatePort)
	ON_BN_CLICKED(IDC_SendButton, OnSendButton)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, OnButtonDisconnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerMonitorDlg message handlers

BOOL CServerMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_StopListenButton.EnableWindow(FALSE);
	m_SendButton.EnableWindow(FALSE);
		m_pClientConnection=NULL;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerMonitorDlg::OnPaint() 
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
HCURSOR CServerMonitorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CServerMonitorDlg::OnChangePort() 
{


	m_SpyPort.UpdateData(TRUE);
	
}

void CServerMonitorDlg::OnListeningButton() 
{
	UpdateData(TRUE);
	m_Listening_Button.EnableWindow(FALSE);
	m_StopListenButton.EnableWindow(TRUE);
	m_pNetSpy = new CNetSpy(this);
	TRACE("Listen on Port:%i\n",m_iSpyPort);
	m_pNetSpy->Create(m_iSpyPort);
	m_pNetSpy->Listen();
	m_eReceiveData="";
	m_sCommand="";
	m_cCommand.SetFocus();
	UpdateData(FALSE);
}

void CServerMonitorDlg::OnStopListenButton() 
{
	m_Listening_Button.EnableWindow(TRUE);
	m_StopListenButton.EnableWindow(FALSE);
    if (m_pNetSpy)
	{
		delete m_pNetSpy;
		m_pNetSpy = NULL;
	}
	if (m_pClientConnection)
	{
		delete(m_pClientConnection);
		m_pClientConnection=NULL;
	}
}

void CServerMonitorDlg::OnKillfocusData() 
{

}

void CServerMonitorDlg::OnUpdatePort() 
{

	
}

void CServerMonitorDlg::OnSendButton() 
{

	UpdateData(TRUE);

	if (m_pClientConnection)
	{
		TRACE("Sende:%s\n",m_sCommand);
		DWORD dwToSend = m_sCommand.GetLength();
		DWORD dwSent = m_pClientConnection->Send(m_sCommand,dwToSend);
		if (dwSent == SOCKET_ERROR)
		{
			int e = GetLastError();
			TRACE("Send failed, %d\n",e);
		}
		else if (dwSent==dwToSend)
		{
			TRACE("%s alles gesendet\n",m_sCommand);
		}
		else
		{
			// Äh, weiß nich
			// weiter senden ?
			TRACE("%s %d<%d Bytes gesendet\n",m_sCommand,dwSent,dwToSend);
		}

		m_cCommand.SetFocus();
	}
	else
	{
		TRACE("no connection\n");
	}

/*220 mail.wk-online.com VPOP3 SMTP Server Ready
  250-mail.wk-online.com VPOP3 SMTP Server - Hello <nohostname>, pleased to meet you
  250-ETRN
  250-SIZE 0
  250-HELP
  250 DSN
*/
	m_sCommand="";
	UpdateData(FALSE);
	UpdateData();
}

void CServerMonitorDlg::OnButtonConnect() 
{
	UpdateData();

	CString s = m_sAddress;
	if (s.IsEmpty())
	{
		BYTE b1,b2,b3,b4;
		b1 = b2 = b3 = b4 = 0;
		m_ctrlIP.GetAddress(b1,b2,b3,b4);
		s.Format("%d.%d.%d.%d",b1,b2,b3,b4);
	}
	if (m_pClientConnection)
	{
		delete m_pClientConnection;
		m_pClientConnection = NULL;
	}
	m_pClientConnection = new CClientConnection(this);
	m_pClientConnection->Create();
    m_pClientConnection->Connect(s,m_iSpyPort);
}

void CServerMonitorDlg::OnButtonDisconnect() 
{
	if (m_pClientConnection)
	{
		delete m_pClientConnection;
		m_pClientConnection = NULL;
	}
}
