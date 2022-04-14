// PTZTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PTZTest.h"
#include "PTZTestDlg.h"

#include "ControlRecordBaxall.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTZTestDlg dialog

CPTZTestDlg::CPTZTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPTZTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPTZTestDlg)
	m_iResponse = -1;
	m_iProtocolIndex = LB_ERR;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pControlRecord = NULL;
}
//////////////////////////////////////////////////////////////////////
CPTZTestDlg::~CPTZTestDlg()
{
	WK_DELETE(m_pControlRecord);
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::EnableControls()
{
	BOOL bEnable = m_cbProtocol.GetCount();
	BOOL bEnableStart = bEnable && !m_pControlRecord;
	m_btnStartListen.EnableWindow(bEnableStart);
	m_btnStopListen.EnableWindow(!bEnableStart);
	m_btnClose.EnableWindow(!bEnable || bEnableStart);

	m_radioResponseIgnore.EnableWindow(!bEnableStart);
	m_radioResponseACK.EnableWindow(!bEnableStart);
	m_radioResponseNAK.EnableWindow(!bEnableStart);
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::FillProtocols()
{
	m_cbProtocol.ResetContent();
	int iIndex = LB_ERR;
	iIndex = m_cbProtocol.AddString("Baxall");
	if (iIndex != LB_ERR)
	{
		m_cbProtocol.SetItemData(iIndex, CCT_BAXALL);
	}
	if (m_cbProtocol.GetCount())
	{
		m_cbProtocol.SetCurSel(0);
	}
	UpdateData();
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPTZTestDlg)
	DDX_Control(pDX, IDC_GRP_RESPONSE, m_groupResponse);
	DDX_Control(pDX, IDC_RADIO_RESPONSE_IGNORE, m_radioResponseIgnore);
	DDX_Control(pDX, IDC_RADIO_RESPONSE_ACK, m_radioResponseACK);
	DDX_Control(pDX, IDC_RADIO_RESPONSE_NAK, m_radioResponseNAK);
	DDX_Control(pDX, IDOK, m_btnStartListen);
	DDX_Control(pDX, IDCANCEL, m_btnStopListen);
	DDX_Control(pDX, IDC_COMBO_PROTOCOL, m_cbProtocol);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Radio(pDX, IDC_RADIO_RESPONSE_IGNORE, m_iResponse);
	DDX_CBIndex(pDX, IDC_COMBO_PROTOCOL, m_iProtocolIndex);
	//}}AFX_DATA_MAP
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPTZTestDlg, CDialog)
	//{{AFX_MSG_MAP(CPTZTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_RADIO_RESPONSE_ACK, OnRadioResponse)
	ON_BN_CLICKED(IDC_RADIO_RESPONSE_IGNORE, OnRadioResponse)
	ON_BN_CLICKED(IDC_RADIO_RESPONSE_NAK, OnRadioResponse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CPTZTestDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	FillProtocols();
	m_iResponse = theApp.GetResponse();
	EnableControls();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
//////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CPTZTestDlg::OnPaint() 
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
//////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPTZTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::OnRadioResponse() 
{
	UpdateData();
	theApp.SetResponse(m_iResponse);
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::OnOK() 
{
	UpdateData();
	int iProtocol = CCT_UNKNOWN;
	if (m_iProtocolIndex != LB_ERR)
	{
		iProtocol = m_cbProtocol.GetItemData(m_iProtocolIndex);
	}
	switch(iProtocol)
	{
	case CCT_BAXALL:
		m_pControlRecord = new CControlRecordBaxall(1, TRUE);
		break;
	default:
		TRACE("Unknown protocol\n");
		break;
	}
	if (m_pControlRecord)
	{
		m_pControlRecord->Create();
	}
	EnableControls();
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::OnCancel() 
{
	if (m_pControlRecord)
	{
		m_pControlRecord->Destroy();
		WK_DELETE(m_pControlRecord);
	}
	EnableControls();
}
//////////////////////////////////////////////////////////////////////
void CPTZTestDlg::OnBtnClose() 
{
	CDialog::OnOK();
}
