// ProductVersionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProductVersion.h"
#include "ProductVersionDlg.h"

#include "WK_Dongle.h"
#include "WKClasses/WK_File.h"

#include "OemGui/OemGuiApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProductVersionDlg dialog

CProductVersionDlg::CProductVersionDlg( CWnd* pParent /*=NULL*/)
	: CDialog(CProductVersionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProductVersionDlg)
	m_sVersion = _T("");
	m_sProduct = _T("");
	m_sVersionInfos = _T("");
	m_sBuild = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	if (m_dongle.IsValid()) 
	{
		m_sVersion = m_dongle.GetVersionString();
//		m_sProduct = COemGuiApi::CalcProductName();
	}
}

void CProductVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProductVersionDlg)
	DDX_Control(pDX, IDC_BUTTON_CMP, m_ctlCompare);
	DDX_Control(pDX, IDC_EDIT, m_ctlVersionInfos);
	DDX_Text(pDX, IDC_MSG_VERSION, m_sVersion);
	DDX_Text(pDX, IDC_MSG_PRODUCT, m_sProduct);
	DDX_Text(pDX, IDC_EDIT, m_sVersionInfos);
	DDX_Text(pDX, IDC_MSG_BUILD, m_sBuild);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProductVersionDlg, CDialog)
	//{{AFX_MSG_MAP(CProductVersionDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProductVersionDlg message handlers

BOOL CProductVersionDlg::OnInitDialog()
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

	m_Font.CreatePointFont(100,_T("Courier New"));
	GetDlgItem(IDC_EDIT)->SetFont(&m_Font);

	FillEditControl2();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProductVersionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		//COemGuiApi::AboutDialog(this);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProductVersionDlg::OnPaint() 
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
HCURSOR CProductVersionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/////////////////////////////////////////////////////////////////////////////
void CProductVersionDlg::BrowseExtension(const CString& sDir, const CString& sExt, CString& result)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(sDir + _T("\\*.")+sExt);
	CString sFile,sVersion,sTime;
	CTime t;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (   !finder.IsDots()
			&& !finder.IsDirectory())
		{
			sFile = finder.GetFileName();
#ifdef _DEBUG
			sVersion = WK_GetFileVersion(sDir + _T("\\") + sFile);
#else
			sVersion = WK_GetFileVersion(sFile);
#endif		
			sVersion.TrimLeft();
			sVersion.TrimRight();
			if (sVersion.IsEmpty())
			{
				sVersion = _T("#.#.#.#");
			}
			finder.GetLastWriteTime(t);
			sTime = t.Format(_T("%d.%m.%Y"));
			CString sBlanks(_T(' '),30-sFile.GetLength());
			CString sBlanks2(_T(' '),16-sVersion.GetLength());
			result += sFile + sBlanks + /*_T("\t") +*/ sVersion + sBlanks2 + sTime + _T("\r\n");
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProductVersionDlg::FillEditControl2()
{
	CString sDir;
	GetModuleFileName(AfxGetApp()->m_hInstance,	sDir.GetBuffer(_MAX_PATH),_MAX_PATH);
	sDir.ReleaseBuffer();
	m_sVersion = WK_GetFileVersion(sDir);
	int p = m_sVersion.ReverseFind('.');
	m_sBuild = m_sVersion.Mid(p+1);
	m_sVersion = m_sVersion.Left(p);
	sDir = sDir.Left(sDir.ReverseFind(_T('\\')));
#ifdef  _DEBUG
	sDir = _T("c:\\security");
#endif

	BrowseExtension(sDir,_T("exe"),m_sVersionInfos);
	BrowseExtension(sDir,_T("dll"),m_sVersionInfos);

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
long CProductVersionDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_PRODUCT_VERSION, 0);

	return 0;
}