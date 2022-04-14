// DongleTesterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DongleTester.h"
#include "DongleTesterDlg.h"

#include "WK_Dongle.h"
#include "wk_file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD DoDongleTest(DWORD dwNumTests);	// forward declaration

static DWORD DoDirTest(DWORD dwNumTests);	// forward declaration

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
// CDongleTesterDlg dialog

CDongleTesterDlg::CDongleTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDongleTesterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDongleTesterDlg)
	m_dwNumTests = 0;
	m_sNumTests = _T("");
	m_sElapsedTime = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwNumTests = 1000;
}

void CDongleTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDongleTesterDlg)
	DDX_Text(pDX, IDC_NUM_TESTS, m_dwNumTests);
	DDV_MinMaxDWord(pDX, m_dwNumTests, 1, 100000);
	DDX_Text(pDX, IDC_MSG_NUM_OUT, m_sNumTests);
	DDX_Text(pDX, IDC_MSG_ELAPSED_TIME, m_sElapsedTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDongleTesterDlg, CDialog)
	//{{AFX_MSG_MAP(CDongleTesterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DO_TEST, OnDoTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDongleTesterDlg message handlers

BOOL CDongleTesterDlg::OnInitDialog()
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
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDongleTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDongleTesterDlg::OnPaint() 
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
HCURSOR CDongleTesterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDongleTesterDlg::OnDoTest() 
{
	UpdateData();

	DWORD dwNumTests = max(1,m_dwNumTests);

	DWORD dwTicksUsed = DoDirTest(dwNumTests);

	m_sNumTests.Format("%8d",dwNumTests);
	m_sElapsedTime.Format("%f",((double)dwTicksUsed/(double)dwNumTests));

	UpdateData(FALSE);	// update the result box
}


static DWORD DoDongleTest(DWORD dwNumTests)
{
	DWORD dwStartTick = GetTickCount();

	CWK_Dongle *pDongle = NULL;
	CPtrArray dongles;
	for (DWORD d=0;d<dwNumTests;d++) {
		pDongle = new CWK_Dongle();
		dongles.Add(pDongle);
	}
	while (dongles.GetSize()) {
		pDongle = (CWK_Dongle*) dongles[0];
		dongles.RemoveAt(0);
		WK_DELETE(pDongle);
	}

	return (GetTickCount()-dwStartTick);
}
static DWORD DoDirTest(DWORD dwNumTests)
{
	DWORD dwStartTick = GetTickCount();

	BOOL b;
	for (DWORD d=0;d<dwNumTests;d++) 
	{
#if 1
		b = SetCurrentDirectory("C:\\LOG");
		b = SetCurrentDirectory("C:\\DATABASE");
		b = SetCurrentDirectory("E:\\PROJECT");
#else
		b = DoesFileExist("C:\\LOG");
		b = DoesFileExist("C:\\DATABASE");
		b = DoesFileExist("E:\\PROJECT");
#endif

	}

	return (GetTickCount()-dwStartTick);
}
