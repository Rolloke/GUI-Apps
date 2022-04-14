// DiskToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiskTool.h"
#include "DiskToolDlg.h"

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
// CDiskToolDlg dialog

/////////////////////////////////////////////////////////////////////////////
CDiskToolDlg::CDiskToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiskToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiskToolDlg)
	m_bWriteCache = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	UINT uDevice = 0;
	m_pCWinAta = new CWinAta(uDevice);
	m_bHasAta = m_pCWinAta && (m_pCWinAta->GetAtaErrorCode() == CWinAta::aec_Ok);
	m_nMaxUDMAMode = -1;
	m_nUDMAMode = -1;
}

/////////////////////////////////////////////////////////////////////////////
CDiskToolDlg::~CDiskToolDlg()
{
	delete m_pCWinAta;
	m_pCWinAta = NULL;
}

void CDiskToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiskToolDlg)
	DDX_Control(pDX, IDC_WRITE_CACHE, m_ctrlWriteCache);
	DDX_Check(pDX, IDC_WRITE_CACHE, m_bWriteCache);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDiskToolDlg, CDialog)
	//{{AFX_MSG_MAP(CDiskToolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_WRITE_CACHE, OnWriteCache)
	ON_BN_CLICKED(IDC_RADIO_UDMA0, OnSetUDMA0)
	ON_BN_CLICKED(IDC_RADIO_UDMA1, OnSetUDMA1)
	ON_BN_CLICKED(IDC_RADIO_UDMA2, OnSetUDMA2)
	ON_BN_CLICKED(IDC_RADIO_UDMA3, OnSetUDMA3)
	ON_BN_CLICKED(IDC_RADIO_UDMA4, OnSetUDMA4)
	ON_BN_CLICKED(IDC_RADIO_UDMA5, OnSetUDMA5)
	ON_BN_CLICKED(IDC_RADIO_UDMA6, OnSetUDMA6)
	ON_BN_CLICKED(IDC_RADIO_DRV0, OnRadioDrv0)
	ON_BN_CLICKED(IDC_RADIO_DRV1, OnRadioDrv1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskToolDlg message handlers

BOOL CDiskToolDlg::OnInitDialog()
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
	CheckRadioButton(IDC_RADIO_DRV0, IDC_RADIO_DRV1, IDC_RADIO_DRV0);
	
	UpdateDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDiskToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDiskToolDlg::OnPaint() 
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
HCURSOR CDiskToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDiskToolDlg::OnWriteCache() 
{
	UpdateData(TRUE);

	if (m_pCWinAta && m_bHasAta)
	{
		m_ctrlWriteCache.EnableWindow(FALSE);
		if (m_bWriteCache)
			m_pCWinAta->EnableWriteCache();
		else
			m_pCWinAta->DisableWriteCache();
		
		m_pCWinAta->IsWriteCacheEnabled(m_bWriteCache);

		m_ctrlWriteCache.EnableWindow(TRUE);
		UpdateData(FALSE);
	}	
}

void CDiskToolDlg::OnSetUDMA0() 
{
	if (m_pCWinAta && m_pCWinAta->SetUDMATransferMode(0))
	{
		m_nUDMAMode = 0;
	}
	CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
}

void CDiskToolDlg::OnSetUDMA1() 
{
	if (m_pCWinAta && m_pCWinAta->SetUDMATransferMode(1))
	{
		m_nUDMAMode = 1;
	}
	CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
}

void CDiskToolDlg::OnSetUDMA2() 
{
	if (m_pCWinAta && m_pCWinAta->SetUDMATransferMode(2))
	{
		m_nUDMAMode = 2;
	}
	CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
}

void CDiskToolDlg::OnSetUDMA3() 
{
	if (m_pCWinAta && m_pCWinAta->SetUDMATransferMode(3))
	{
		m_nUDMAMode = 3;
	}
	CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
}

void CDiskToolDlg::OnSetUDMA4() 
{
	if (m_pCWinAta && m_pCWinAta->SetUDMATransferMode(4))
	{
		m_nUDMAMode = 4;
	}
	CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
}

void CDiskToolDlg::OnSetUDMA5() 
{
	if (m_pCWinAta && m_pCWinAta->SetUDMATransferMode(5))
	{
		m_nUDMAMode = 5;
	}
	CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
}

void CDiskToolDlg::OnSetUDMA6() 
{
	if (m_pCWinAta && m_pCWinAta->SetUDMATransferMode(6))
	{
		m_nUDMAMode = 6;
	}
	CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
}

void CDiskToolDlg::OnRadioDrv0() 
{

	if (m_pCWinAta)
	{
		delete m_pCWinAta;
		m_pCWinAta = NULL;
		m_bHasAta = FALSE;
	}

	m_pCWinAta = new CWinAta(0);

	m_bHasAta = m_pCWinAta && (m_pCWinAta->GetAtaErrorCode() == CWinAta::aec_Ok);
	m_nMaxUDMAMode = -1;
	m_nUDMAMode = -1;

	CheckRadioButton(IDC_RADIO_DRV0, IDC_RADIO_DRV1, IDC_RADIO_DRV0);
	UpdateDialog();
}

void CDiskToolDlg::OnRadioDrv1() 
{
	if (m_pCWinAta)
	{
		delete m_pCWinAta;
		m_pCWinAta = NULL;
		m_bHasAta = FALSE;
	}

	m_pCWinAta = new CWinAta(1);

	m_bHasAta = m_pCWinAta && (m_pCWinAta->GetAtaErrorCode() == CWinAta::aec_Ok);
	m_nMaxUDMAMode = -1;
	m_nUDMAMode = -1;
	CheckRadioButton(IDC_RADIO_DRV0, IDC_RADIO_DRV1, IDC_RADIO_DRV1);
	UpdateDialog();
}

BOOL CDiskToolDlg::UpdateDialog()
{
	for (int nI = 0; nI <= 6; nI++)
	{
		CWnd* pWnd = GetDlgItem(IDC_RADIO_UDMA0+nI);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
	}
	m_ctrlWriteCache.EnableWindow(FALSE);

	if (m_pCWinAta && m_bHasAta)
	{
		if (m_pCWinAta->IsWriteCacheEnabled(m_bWriteCache))
			m_ctrlWriteCache.EnableWindow(TRUE);

		if (m_pCWinAta->GetMaxUDMATransferMode(m_nMaxUDMAMode))
		{
			for (int nI = 0; nI <= m_nMaxUDMAMode; nI++)
			{
				CWnd* pWnd = GetDlgItem(IDC_RADIO_UDMA0+nI);
				if (pWnd)
					pWnd->EnableWindow(TRUE);
			}
		}
		
		if (m_pCWinAta->GetUDMATransferMode(m_nUDMAMode))
		{
			CheckRadioButton(IDC_RADIO_UDMA0, IDC_RADIO_UDMA6, IDC_RADIO_UDMA0+m_nUDMAMode);
		}
	}
	
	UpdateData(FALSE);

	return TRUE;
}
